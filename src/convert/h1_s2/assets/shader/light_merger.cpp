#include <std_include.hpp>

#include "../../include.hpp"
#include "dxbc.hpp"
#include "forward_lights.hpp"

namespace zonetool::h1::converter::s2::shader
{
	namespace
	{
		class light_merger
		{
		public:
			light_merger(const unsigned char* lit, const unsigned int lit_size, const unsigned char* light, const unsigned int light_size, const light_merge& merge)
				: lit_(alys::shader::shader_object::parse({reinterpret_cast<const char*>(lit), lit_size}))
				, light_(alys::shader::shader_object::parse({reinterpret_cast<const char*>(light), light_size}))
				, merge_(merge)
				, assembler_(this->result_.get_assembler())
			{
				this->result_.get_info() = this->lit_.get_info();
				this->result_.get_signatures() = this->lit_.get_signatures();
				this->result_.get_unknown_chunks() = this->lit_.get_unknown_chunks();

				this->lit_slots_ = relocate_light_textures(this->lit_.get_instructions());
				for (auto& instruction : this->lit_.get_instructions())
				{
					relocate_resource_slots(instruction, this->lit_slots_);
				}
			}

			patched_program merge()
			{
				const auto& lit = this->lit_.get_instructions();
				const auto& light = this->light_.get_instructions();
				check_body(lit, "lit");
				check_body(light, "light");
				for (const auto& [from, to] : this->merge_.texture_slots)
				{
					const auto slot = this->lit_slots_.find(to);
					this->texture_slots_[from] = slot != this->lit_slots_.end() ? slot->second : to;
				}

				this->map_inputs();
				this->lit_temps_ = temp_count(lit);
				this->light_temps_ = temp_count(light);
				const auto acc = this->lit_temps_ + this->light_temps_;
				const auto lit_out = acc + light_temp_count;
				this->gather_inputs(lit_out + 1);

				const auto lit_code = std::ranges::find_if_not(lit, is_declaration) - lit.begin();
				for (auto i = 0; i < lit_code; i++)
				{
					auto instruction = lit[i];
					if (instruction.opcode.type == D3D10_SB_OPCODE_DCL_TEMPS)
					{
						continue;
					}

					this->patch_lit_declaration(instruction);
					this->assembler_(instruction);
				}

				this->emit_declarations();
				this->assembler_.dcl_temps(lit_out + 1 + static_cast<std::uint32_t>(this->gathered_.size()) + (this->merge_.lights.attenuation_texture ? 1u : 0u));
				for (auto i = lit_code; i + 1 < static_cast<std::ptrdiff_t>(lit.size()); i++)
				{
					auto instruction = lit[i];
					for (auto& operand : instruction.operands)
					{
						if (operand.type == D3D10_SB_OPERAND_TYPE_OUTPUT && operand.indices[0].value.uint32 == 0)
						{
							operand = as_temp(operand, lit_out);
						}
					}

					this->assembler_(instruction);
				}

				this->emit_gathered_inputs();
				emit_forward_light_loop_begin(this->assembler_, this->merge_.lights, acc, this->screen_input_, this->position_);
				const auto light_code = std::ranges::find_if_not(light, is_declaration) - light.begin();
				for (auto i = light_code; i + 1 < static_cast<std::ptrdiff_t>(light.size()); i++)
				{
					auto instruction = light[i];
					if (this->merge_.lights.attenuation_texture && is_sample(instruction, *this->merge_.lights.attenuation_texture))
					{
						this->patch_light_operand(instruction.operands[0], acc);
						this->patch_light_operand(instruction.operands[1], acc);
						this->emit_attenuation(instruction, lit_out + 1 + static_cast<std::uint32_t>(this->gathered_.size()));
						continue;
					}

					for (auto& operand : instruction.operands)
					{
						this->patch_light_operand(operand, acc);
					}

					this->assembler_(instruction);
				}

				emit_forward_light_loop_end(this->assembler_, acc, lit_out);
				return {this->result_.serialize(), false, {}, this->lit_slots_};
			}

		private:
			struct lane
			{
				bool valid{};
				bool fog_transmittance{};
				std::uint32_t reg{};
				std::uint8_t component{};
			};

			struct lit_input
			{
				std::uint8_t mask{};
				std::uint32_t interpolation{};
				std::uint16_t opcode{};
				std::optional<std::uint32_t> system_value;
			};

			alys::shader::shader_object lit_;
			alys::shader::shader_object light_;
			alys::shader::shader_object result_;
			const light_merge& merge_;
			alys::shader::assembler assembler_;
			std::map<std::uint32_t, std::array<lane, 4>> lanes_;
			std::map<std::uint32_t, std::uint32_t> gathered_;
			std::map<std::uint32_t, lit_input> lit_needs_;
			alys::shader::detail::operand_t position_;
			std::map<std::uint32_t, std::uint32_t> lit_slots_;
			std::map<std::uint32_t, std::uint32_t> texture_slots_;
			std::uint32_t screen_input_{};
			std::uint32_t lit_temps_{};
			std::uint32_t light_temps_{};
			std::set<std::uint32_t> lit_inputs_;
			bool lit_stable_buffer_{};
			bool lit_material_buffer_{};
			bool lit_screen_input_{};

			static void check_body(const std::vector<alys::shader::detail::instruction_t>& instructions, const char* what)
			{
				check_single_return(instructions, what);
				for (const auto& instruction : instructions)
				{
					if (instruction.opcode.type == D3D10_SB_OPCODE_DCL_INDEXABLE_TEMP || instruction.opcode.type == D3D10_SB_OPCODE_CUSTOMDATA)
					{
						throw std::runtime_error(utils::string::va("%s pixel shader has indexable temps or an immediate constant buffer", what));
					}
				}
			}

			static std::uint32_t temp_count(const std::vector<alys::shader::detail::instruction_t>& instructions)
			{
				const auto temps = std::ranges::find(instructions, static_cast<std::uint16_t>(D3D10_SB_OPCODE_DCL_TEMPS),
					[](const alys::shader::detail::instruction_t& instruction) { return instruction.opcode.type; });
				return temps != instructions.end() ? temps->operands[0].custom.u.value : 0u;
			}

			std::uint32_t stable_rows()
			{
				auto rows = std::max(this->merge_.lights.render_target_size_row, this->merge_.lights.tile_parms_row) + 1;
				if (this->merge_.lights.cluster_row)
				{
					rows = std::max(rows, *this->merge_.lights.cluster_row + 3);
				}

				for (const auto& [from, to] : this->merge_.stable_rows)
				{
					rows = std::max(rows, to + 1);
				}

				return rows;
			}

			std::uint32_t material_rows()
			{
				auto rows = 1u;
				for (const auto& [from, to] : this->merge_.material_rows)
				{
					rows = std::max(rows, to + 1);
				}

				for (const auto& instruction : this->light_.get_instructions())
				{
					if (instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER && instruction.operands[0].indices[0].value.uint32 == h1_material_constant_buffer)
					{
						for (auto row = 0u; row < instruction.operands[0].indices[1].value.uint32; row++)
						{
							const auto entry = this->merge_.material_rows.find(row);
							rows = std::max(rows, (entry != this->merge_.material_rows.end() ? entry->second : row) + 1);
						}
					}
				}

				return rows;
			}

			void map_inputs()
			{
				auto& lit_inputs = this->result_.get_signatures()[alys::shader::chunk_isgn];
				const auto light_signatures = this->light_.get_signatures();
				const auto light_inputs = light_signatures.find(alys::shader::chunk_isgn);
				if (light_inputs == light_signatures.end())
				{
					throw std::runtime_error("light pixel shader has no inputs");
				}

				const auto find_lit = [&](const std::function<bool(const signature_element&)>& accept, const char* what) -> signature_element&
				{
					const auto element = std::ranges::find_if(lit_inputs, accept);
					if (element == lit_inputs.end())
					{
						throw std::runtime_error(utils::string::va("lit pixel shader has no %s input", what));
					}

					return *element;
				};

				auto& screen = find_lit([](const signature_element& element) { return element.system_value_type == alys::shader::POS; }, "screen position");
				screen.rw_mask |= light_screen_mask(this->merge_.lights);
				this->screen_input_ = screen.register_;

				for (const auto& instruction : this->lit_.get_instructions())
				{
					if (is_input_ps_declaration(instruction))
					{
						this->lit_inputs_.insert(instruction.operands[0].indices[0].value.uint32);
					}
				}

				auto has_position = false;
				for (const auto& instruction : this->light_.get_instructions())
				{
					if (!is_input_ps_declaration(instruction))
					{
						continue;
					}

					const auto reg = instruction.operands[0].indices[0].value.uint32;
					const auto declared = instruction.operands[0].components.mask;
					auto& lanes = this->lanes_[reg];
					for (const auto& element : light_inputs->second)
					{
						auto used = static_cast<std::uint8_t>(element.register_ == reg ? declared & element.mask : 0);

						if (this->merge_.lights.attenuation_texture && element.name == "TEXCOORD" && element.semantic_index == 1 && (used & 0x8))
						{
							lanes[3] = lane{true, true};
							used &= 0x7;
						}

						if (!used)
						{
							continue;
						}

						if (element.name == "TEXCOORD" && element.semantic_index == h1_fog_transmittance_semantic)
						{
							for (auto c = 0; c < 4; c++)
							{
								lanes[c] = (used >> c) & 1 ? lane{true, true} : lanes[c];
							}
							continue;
						}

						const auto lit_element = [&](const std::uint32_t index) -> signature_element*
						{
							const auto found = std::ranges::find_if(lit_inputs, [&](const signature_element& other)
							{
								return other.system_value_type == alys::shader::NONE && other.name == "TEXCOORD" && other.semantic_index == index;
							});
							return found != lit_inputs.end() ? &*found : nullptr;
						};

						if (element.name == "TEXCOORD" && element.semantic_index == 0 && !lit_element(0))
						{
							auto* u = lit_element(1);
							auto* v = lit_element(5);
							if (u && v && (u->mask & 0x8) && (v->mask & 0x8))
							{
								for (auto c = 0; c < 4; c++)
								{
									if (!((used >> c) & 1))
									{
										continue;
									}

									auto* source = c == 0 ? u : c == 1 ? v : nullptr;
									if (!source)
									{
										throw std::runtime_error("light pixel shader reads more than the uv of TEXCOORD0");
									}

									lanes[c] = {true, false, source->register_, 3};
									source->rw_mask |= 0x8;
									auto& need = this->lit_needs_[source->register_];
									need.mask |= 0x8;
									need.interpolation = instruction.opcode.controls;
									need.opcode = instruction.opcode.type;
								}
								continue;
							}
						}

						auto& lit = find_lit([&](const signature_element& other)
						{
							return element.system_value_type != alys::shader::NONE ? other.system_value_type == element.system_value_type
								: other.name == element.name && other.semantic_index == element.semantic_index;
						}, element.name.data());

						const auto shift = std::countr_zero(lit.mask) - std::countr_zero(element.mask);
						const auto moved = static_cast<std::uint8_t>(shift >= 0 ? used << shift : used >> -shift);
						if (std::popcount(element.mask) > std::popcount(lit.mask) || (moved & ~lit.mask) || std::popcount(moved) != std::popcount(used))
						{
							throw std::runtime_error(utils::string::va("lit pixel shader packs %s%u differently", element.name.data(), element.semantic_index));
						}

						for (auto c = 0; c < 4; c++)
						{
							if ((used >> c) & 1)
							{
								lanes[c] = {true, false, lit.register_, static_cast<std::uint8_t>(c + shift)};
							}
						}

						lit.rw_mask |= moved;
						auto& need = this->lit_needs_[lit.register_];
						need.mask |= moved;
						need.interpolation = instruction.opcode.controls;
						need.opcode = instruction.opcode.type;
						if (instruction.opcode.type != D3D10_SB_OPCODE_DCL_INPUT_PS)
						{
							need.system_value = instruction.operands[1].custom.u.value;
						}

						if (element.name == "TEXCOORD" && element.semantic_index == h1_eye_position_semantic)
						{
							const auto first = static_cast<std::uint32_t>(std::countr_zero(moved));
							constexpr std::string_view names = "xyzw";
							const std::string swizzle{names[first], names[first + 1], names[first + 2], names[first]};
							this->position_ = asm_::v(lit.register_).select(swizzle).swz();
							has_position = std::popcount(moved) == 3;
						}
					}
				}

				if (!has_position)
				{
					throw std::runtime_error("light pixel shader reads no eye relative position");
				}
			}

			void patch_lit_declaration(alys::shader::detail::instruction_t& instruction)
			{
				if (instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER)
				{
					auto& rows = instruction.operands[0].indices[1].value.uint32;
					const auto buffer = instruction.operands[0].indices[0].value.uint32;
					if (buffer == s2_stable_constant_buffer)
					{
						rows = std::max(rows, this->stable_rows());
						this->lit_stable_buffer_ = true;
					}
					else if (buffer == s2_material_constant_buffer)
					{
						rows = std::max(rows, this->material_rows());
						this->lit_material_buffer_ = true;
					}
				}

				if (is_input_ps_declaration(instruction))
				{
					const auto reg = instruction.operands[0].indices[0].value.uint32;
					if (const auto need = this->lit_needs_.find(reg); need != this->lit_needs_.end())
					{
						instruction.operands[0].components.mask |= need->second.mask;
					}

					if (reg == this->screen_input_)
					{
						instruction.operands[0].components.mask |= light_screen_mask(this->merge_.lights);
						this->lit_screen_input_ = true;
					}
				}
			}

			void emit_declarations()
			{
				auto& a = this->assembler_;
				declare_forward_lights(a, this->merge_.lights);
				if (!this->lit_screen_input_)
				{
					alys::shader::detail::operand_t screen = asm_::v(this->screen_input_).select("xyzw");
					screen.components.selection_mode = D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE;
					screen.components.mask = light_screen_mask(this->merge_.lights);
					a.dcl_input_ps_siv(screen, asm_::position, asm_::linear_noperspective);
				}

				for (const auto& [reg, need] : this->lit_needs_)
				{
					if (this->lit_inputs_.contains(reg) || reg == this->screen_input_)
					{
						continue;
					}

					alys::shader::detail::operand_t operand = asm_::v(reg).select("xyzw");
					operand.components.selection_mode = D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE;
					operand.components.mask = need.mask;
					if (need.opcode == D3D10_SB_OPCODE_DCL_INPUT_PS)
					{
						a.dcl_input_ps(operand, need.interpolation);
					}
					else if (need.opcode == D3D10_SB_OPCODE_DCL_INPUT_PS_SGV)
					{
						a.dcl_input_ps_sgv(operand, *need.system_value);
					}
					else
					{
						a.dcl_input_ps_siv(operand, *need.system_value, need.interpolation);
					}
				}

				if (!this->lit_stable_buffer_)
				{
					a.dcl_constantbuffer(asm_::cb(s2_stable_constant_buffer, this->stable_rows()).select("xyzw"), D3D10_SB_CONSTANT_BUFFER_IMMEDIATE_INDEXED);
				}

				const auto reads_material = std::ranges::any_of(this->light_.get_instructions(), [](const alys::shader::detail::instruction_t& instruction)
				{
					return instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER && instruction.operands[0].indices[0].value.uint32 == h1_material_constant_buffer;
				});
				if (reads_material && !this->lit_material_buffer_)
				{
					a.dcl_constantbuffer(asm_::cb(s2_material_constant_buffer, this->material_rows()).select("xyzw"), D3D10_SB_CONSTANT_BUFFER_IMMEDIATE_INDEXED);
				}

				const auto& lit = this->lit_.get_instructions();
				for (auto instruction : this->light_.get_instructions())
				{
					if (!is_resource_declaration(instruction))
					{
						continue;
					}

					const auto is_sampler = instruction.operands[0].type == D3D10_SB_OPERAND_TYPE_SAMPLER;
					if (instruction.operands[0].indices[0].value.uint32 == (is_sampler ? this->merge_.lights.attenuation_sampler : this->merge_.lights.attenuation_texture))
					{
						continue;
					}

					const auto& slots = is_sampler ? this->merge_.sampler_slots : this->texture_slots_;
					const auto slot = slots.find(instruction.operands[0].indices[0].value.uint32);
					if (slot == slots.end())
					{
						throw std::runtime_error(utils::string::va("light %s%u has no lit slot", is_sampler ? "s" : "t", instruction.operands[0].indices[0].value.uint32));
					}

					instruction.operands[0].indices[0].value.uint32 = slot->second;
					const auto existing = std::ranges::find_if(lit, [&](const alys::shader::detail::instruction_t& other)
					{
						return is_resource_declaration(other) && other.operands[0].type == instruction.operands[0].type
							&& other.operands[0].indices[0].value.uint32 == slot->second;
					});

					if (existing == lit.end())
					{
						a(instruction);
					}
					else if (existing->opcode.type != instruction.opcode.type || existing->opcode.controls != instruction.opcode.controls)
					{
						throw std::runtime_error(utils::string::va("lit and light shaders declare %s%u differently", is_sampler ? "s" : "t", slot->second));
					}
				}
			}

			void gather_inputs(const std::uint32_t first_temp)
			{
				for (const auto& [reg, lanes] : this->lanes_)
				{
					std::optional<lane> first;
					auto split = false;
					for (const auto& entry : lanes)
					{
						if (!entry.valid)
						{
							continue;
						}

						split |= first && (first->fog_transmittance != entry.fog_transmittance || first->reg != entry.reg);
						first = first ? first : std::optional(entry);
					}

					if (split)
					{
						this->gathered_.emplace(reg, first_temp + static_cast<std::uint32_t>(this->gathered_.size()));
					}
				}
			}

			void emit_gathered_inputs()
			{
				constexpr std::string_view names = "xyzw";
				for (const auto& [reg, temp] : this->gathered_)
				{
					const auto& lanes = this->lanes_.at(reg);
					for (auto c = 0u; c < 4; c++)
					{
						if (!lanes[c].valid)
						{
							continue;
						}

						const auto dest = asm_::r(temp).select(std::string(1, names[c]));
						if (lanes[c].fog_transmittance)
						{
							this->assembler_.mov(dest, asm_::l(1.0f));
						}
						else
						{
							this->assembler_.mov(dest, asm_::v(lanes[c].reg).select(std::string(4, names[lanes[c].component])).swz());
						}
					}
				}
			}

			void emit_attenuation(const alys::shader::detail::instruction_t& sample, const std::uint32_t temp)
			{
				constexpr std::array coefficients{6.2571f, -5.1194f, 0.3302f, 1.0034f}; // falloff_linear fit

				auto& a = this->assembler_;
				a.mov_sat(asm_::r(temp).x(), sample.operands[1]);
				a.mov(asm_::r(temp).y(), asm_::l(-2.4957f));
				for (const auto coefficient : coefficients)
				{
					a.mad(asm_::r(temp).y(), asm_::r(temp).select("yyyy"), asm_::r(temp).select("xxxx"), asm_::l(coefficient));
				}

				a.mov_sat(sample.operands[0], asm_::r(temp).select("yyyy"));
			}

			void patch_light_input(alys::shader::detail::operand_t& operand) const
			{
				if (const auto gathered = this->gathered_.find(operand.indices[0].value.uint32); gathered != this->gathered_.end())
				{
					operand.type = D3D10_SB_OPERAND_TYPE_TEMP;
					operand.indices[0].value.uint32 = gathered->second;
					return;
				}

				const auto& lanes = this->lanes_.at(operand.indices[0].value.uint32);
				auto& components = operand.components;
				const auto count = components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE ? 4 : 1;
				if (components.type != D3D10_SB_OPERAND_4_COMPONENT || components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE)
				{
					throw std::runtime_error("light pixel shader reads an input without a swizzle");
				}

				std::optional<lane> first;
				for (auto i = 0; i < count; i++)
				{
					const auto& entry = lanes[components.names[i]];
					if (!entry.valid || (first && (first->fog_transmittance != entry.fog_transmittance || first->reg != entry.reg)))
					{
						throw std::runtime_error("light pixel shader mixes inputs that the lit shader packs apart");
					}

					first = first ? first : std::optional(entry);
				}

				if (first->fog_transmittance)
				{
					operand = count == 1 ? asm_::l(1.0f) : asm_::l(1.0f, 1.0f, 1.0f, 1.0f);
					return;
				}

				operand.indices[0].value.uint32 = first->reg;
				for (auto i = 0; i < count; i++)
				{
					components.names[i] = lanes[components.names[i]].component;
				}
			}

			void patch_light_operand(alys::shader::detail::operand_t& operand, const std::uint32_t acc) const
			{
				for (auto& index : operand.indices)
				{
					if (index.extra_operand)
					{
						this->patch_light_operand(*index.extra_operand, acc);
					}
				}

				switch (operand.type)
				{
				case D3D10_SB_OPERAND_TYPE_TEMP:
					operand.indices[0].value.uint32 += this->lit_temps_;
					return;
				case D3D10_SB_OPERAND_TYPE_OUTPUT:
					operand = as_temp(operand, acc + 1);
					return;
				case D3D10_SB_OPERAND_TYPE_INPUT:
					this->patch_light_input(operand);
					return;
				case D3D10_SB_OPERAND_TYPE_RESOURCE:
				case D3D10_SB_OPERAND_TYPE_SAMPLER:
				{
					const auto& slots = operand.type == D3D10_SB_OPERAND_TYPE_SAMPLER ? this->merge_.sampler_slots : this->texture_slots_;
					operand.indices[0].value.uint32 = slots.at(operand.indices[0].value.uint32);
					return;
				}
				case D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER:
					break;
				case D3D11_SB_OPERAND_TYPE_INPUT_COVERAGE_MASK:
				case D3D10_SB_OPERAND_TYPE_INDEXABLE_TEMP:
				case D3D10_SB_OPERAND_TYPE_IMMEDIATE_CONSTANT_BUFFER:
					throw std::runtime_error("light pixel shader reads an unsupported register");
				default:
					return;
				}

				if (operand.indices[1].representation != D3D10_SB_OPERAND_INDEX_IMMEDIATE32)
				{
					throw std::runtime_error("light pixel shader indexes a constant buffer");
				}

				const auto buffer = operand.indices[0].value.uint32;
				auto& row = operand.indices[1].value.uint32;
				const auto& lights = this->merge_.lights;
				if (buffer == lights.buffer)
				{
					const auto light = std::ranges::find(lights.rows, std::optional(row));
					if (light == lights.rows.end())
					{
						throw std::runtime_error(utils::string::va("light pixel shader reads cb%u[%u]", buffer, row));
					}

					operand = as_temp(operand, acc + 2 + static_cast<std::uint32_t>(light - lights.rows.begin()));
					return;
				}

				const auto& rows = buffer == h1_material_constant_buffer ? this->merge_.material_rows : this->merge_.stable_rows;
				if (buffer != h1_material_constant_buffer && buffer != h1_stable_constant_buffer)
				{
					throw std::runtime_error(utils::string::va("light pixel shader reads cb%u", buffer));
				}

				const auto entry = rows.find(row);
				if (entry == rows.end() && buffer == h1_stable_constant_buffer)
				{
					throw std::runtime_error(utils::string::va("light pixel shader stable row %u has no merged row", row));
				}

				row = entry != rows.end() ? entry->second : row;
				operand.indices[0].value.uint32 = buffer + 1;
			}
		};
	}

	patched_program merge_light_program(const unsigned char* lit, const unsigned int lit_size, const unsigned char* light, const unsigned int light_size,
		const light_merge& merge)
	{
		light_merger merger(lit, lit_size, light, light_size, merge);
		return merger.merge();
	}

	std::pair<std::set<std::uint32_t>, std::set<std::uint32_t>> declared_slots(const unsigned char* program, const unsigned int size)
	{
		auto shader = alys::shader::shader_object::parse({reinterpret_cast<const char*>(program), size});
		std::pair<std::set<std::uint32_t>, std::set<std::uint32_t>> result;
		for (const auto& instruction : shader.get_instructions())
		{
			if (is_resource_declaration(instruction))
			{
				(instruction.operands[0].type == D3D10_SB_OPERAND_TYPE_SAMPLER ? result.second : result.first).insert(instruction.operands[0].indices[0].value.uint32);
			}
		}

		return result;
	}

	bool squares_color(const unsigned char* program, const unsigned int size)
	{
		auto shader = alys::shader::shader_object::parse({reinterpret_cast<const char*>(program), size});
		return std::ranges::any_of(shader.get_instructions(), [](const alys::shader::detail::instruction_t& instruction)
		{
			if ((instruction.opcode.type != D3D10_SB_OPCODE_MUL && instruction.opcode.type != D3D10_SB_OPCODE_MAD) || instruction.operands.size() < 3)
			{
				return false;
			}

			const auto& dest = instruction.operands[0];
			const auto& first = instruction.operands[1];
			const auto& second = instruction.operands[2];
			const auto reads_xyz = [](const alys::shader::detail::operand_t& operand)
			{
				return operand.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE
					&& operand.components.names[0] == 0 && operand.components.names[1] == 1 && operand.components.names[2] == 2;
			};

			return dest.type == D3D10_SB_OPERAND_TYPE_TEMP && dest.components.mask == 0x7 && first.type == D3D10_SB_OPERAND_TYPE_TEMP && second.type == D3D10_SB_OPERAND_TYPE_TEMP
				&& first.indices[0].value.uint32 == second.indices[0].value.uint32 && reads_xyz(first) && reads_xyz(second);
		});
	}
}
