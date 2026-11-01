#include <std_include.hpp>

#include "../../include.hpp"
#include "dxbc.hpp"
#include "forward_lights.hpp"
#include "instancing.hpp"
#include "layout.hpp"
#include "light_grid.hpp"
#include "sprite.hpp"
#include "wind.hpp"

namespace zonetool::h1::converter::s2::shader
{
	namespace
	{
		void remap_constant_buffers(alys::shader::detail::operand_t& operand)
		{
			for (auto& index : operand.indices)
			{
				if (index.extra_operand)
				{
					remap_constant_buffers(*index.extra_operand);
				}
			}

			if (operand.type != D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER)
			{
				return;
			}

			auto& slot = operand.indices[0].value.uint32;
			if (slot == h1_instance_matrix_buffer)
			{
				slot = s2_instance_matrix_buffer;
				return;
			}

			if (slot >= h1_constant_buffer_count)
			{
				throw std::runtime_error(utils::string::va("constant buffer cb%u has no s2 slot", slot));
			}

			if (slot >= h1_stable_constant_buffer)
			{
				slot++;
			}
		}

		struct lightmap_input
		{
			std::uint32_t texcoord;
			std::uint32_t lightmap;
		};

		std::optional<lightmap_input> add_lightmap_input(alys::shader::shader_object::signature& inputs)
		{
			const auto texcoord = std::ranges::find_if(inputs, [](const alys::shader::shader_object::signature_element& element)
			{
				return element.name == "TEXCOORD" && element.semantic_index == 0 && (element.rw_mask & 0xC);
			});

			if (texcoord == inputs.end())
			{
				return {};
			}

			const auto last = std::ranges::max(inputs, {}, &alys::shader::shader_object::signature_element::register_);
			const lightmap_input result{texcoord->register_, last.register_ + 1};

			// a zw declaration fails the d3d11 input layout linkage
			texcoord->mask &= 0x3;
			texcoord->rw_mask &= 0x3;

			alys::shader::shader_object::signature_element lightmap{};
			lightmap.name = "TEXCOORD";
			lightmap.semantic_index = zonetool::s2::STREAM_DST_TEXCOORD_15 - zonetool::s2::STREAM_DST_TEXCOORD_0;
			lightmap.system_value_type = alys::shader::NONE;
			lightmap.component_type = alys::shader::format_float;
			lightmap.register_ = result.lightmap;
			lightmap.mask = 0x3;
			lightmap.rw_mask = 0x3;
			inputs.emplace_back(lightmap);

			return result;
		}

		constexpr auto fog_temp_count = 3u;

		std::optional<std::uint32_t> fog_row_index(const alys::shader::detail::operand_t& operand, const atmos_fog_rows& fog)
		{
			if (operand.type != D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER || operand.indices[0].value.uint32 != h1_stable_constant_buffer
				|| operand.indices[1].representation != D3D10_SB_OPERAND_INDEX_IMMEDIATE32)
			{
				return {};
			}

			const auto row = operand.indices[1].value.uint32;
			const std::array rows{fog.extinction, fog.in_scatter, fog.sun_dir};
			for (auto i = 0u; i < rows.size(); i++)
			{
				if (rows[i] == row)
				{
					return i;
				}
			}

			return {};
		}

		bool reads_fog_rows(const std::vector<alys::shader::detail::instruction_t>& instructions, const atmos_fog_rows& fog)
		{
			return std::ranges::any_of(instructions, [&](const alys::shader::detail::instruction_t& instruction)
			{
				return !is_declaration(instruction) && std::ranges::any_of(instruction.operands, [&](const alys::shader::detail::operand_t& operand)
				{
					return fog_row_index(operand, fog).has_value();
				});
			});
		}

		void emit_atmos_fog_rows(alys::shader::assembler& a, const atmos_fog_rows& fog, const std::uint32_t temp)
		{
			// s2: parms 2 scaled by log2(e), parms 3 folded into 0 and 1, parms 4 w = sky minus start distance
			constexpr auto ln_2 = 0.693147182f;

			const auto row = [](const std::uint32_t index) { return asm_::cb(s2_stable_constant_buffer, index); };
			if (fog.extinction)
			{
				a.mul(asm_::r(temp).select("xyzw"), row(*fog.extinction).select("xyzw"), asm_::l(ln_2, ln_2, ln_2, 1.0f));
			}

			if (fog.in_scatter)
			{
				a.mov(asm_::r(temp + 1).xyz(), asm_::l(1.0f, 1.0f, 1.0f, 0.0f));
				a.mov(asm_::r(temp + 1).w(), row(*fog.in_scatter).w());
			}

			if (fog.sun_dir)
			{
				a.mov(asm_::r(temp + 2).select("xyzw"), row(*fog.sun_dir).select("xyzw"));
				if (fog.in_scatter)
				{
					a.add(asm_::r(temp + 2).w(), row(*fog.sun_dir).w(), row(*fog.in_scatter).w().neg());
				}
			}
		}

		constexpr auto ssr_buffer_max = 128.0f; // the s2 ssr buffer keeps the fireflies that the h1 bloom prefilter drops

		void emit_ssr_clamp(alys::shader::assembler& a, const alys::shader::detail::operand_t& dest)
		{
			if (dest.type != D3D10_SB_OPERAND_TYPE_TEMP)
			{
				throw std::runtime_error("ssr buffer sample is not written to a temp");
			}

			a.min(dest, asm_::r(dest.indices[0].value.uint32).select("xyzw"), asm_::l(ssr_buffer_max));
		}

		std::optional<float> read_cascade_slice(const alys::shader::detail::operand_t& operand, const sun_shadow_lookup& sun_shadow)
		{
			if (operand.type != D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER || operand.indices[0].value.uint32 != h1_stable_constant_buffer
				|| operand.indices[1].representation != D3D10_SB_OPERAND_INDEX_IMMEDIATE32)
			{
				return {};
			}

			const auto entry = sun_shadow.cascade_slices.find(operand.indices[1].value.uint32);
			return entry != sun_shadow.cascade_slices.end() ? std::optional(entry->second) : std::nullopt;
		}

		std::optional<float> trace_cascade_slice(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::size_t sample,
			const sun_shadow_lookup& sun_shadow)
		{
			std::unordered_map<std::uint32_t, std::uint8_t> wanted;
			const auto& uv = instructions[sample].operands[1];
			if (uv.type == D3D10_SB_OPERAND_TYPE_TEMP)
			{
				wanted[uv.indices[0].value.uint32] = read_components(uv, 0x3, instructions[sample].opcode.type);
			}

			for (auto i = sample; i-- > 0 && !wanted.empty();)
			{
				const auto& operands = instructions[i].operands;
				const auto first_source = static_cast<std::size_t>(std::ranges::find_if_not(operands, is_destination) - operands.begin());
				for (auto dest = 0u; dest < first_source; dest++)
				{
					const auto entry = operands[dest].type == D3D10_SB_OPERAND_TYPE_TEMP ? wanted.find(operands[dest].indices[0].value.uint32) : wanted.end();
					const auto written = static_cast<std::uint8_t>(entry != wanted.end() ? entry->second & operands[dest].components.mask : 0);
					if (!written)
					{
						continue;
					}

					entry->second &= ~written;
					if (!entry->second)
					{
						wanted.erase(entry);
					}

					for (auto source = first_source; source < operands.size(); source++)
					{
						if (const auto slice = read_cascade_slice(operands[source], sun_shadow))
						{
							return *slice;
						}

						if (operands[source].type == D3D10_SB_OPERAND_TYPE_TEMP)
						{
							wanted[operands[source].indices[0].value.uint32] |= read_components(operands[source], written, instructions[i].opcode.type);
						}
					}
				}
			}

			return {};
		}

		float find_cascade_slice(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::size_t sample, const sun_shadow_lookup& sun_shadow)
		{
			if (const auto slice = trace_cascade_slice(instructions, sample, sun_shadow))
			{
				return *slice;
			}

			// "sst" shaders sample the unfiltered cascade depth after the filtered samples
			for (auto i = sample; i-- > 0;)
			{
				if (is_sample(instructions[i], *sun_shadow.texture))
				{
					if (const auto slice = trace_cascade_slice(instructions, i, sun_shadow))
					{
						return *slice;
					}
				}
			}

			throw std::runtime_error("sun shadow sample has no cascade");
		}

		class program_patcher
		{
		public:
			program_patcher(const unsigned char* program, const unsigned int size, const bool is_vertex_shader, const program_patch& patch)
				: shader_(alys::shader::shader_object::parse({reinterpret_cast<const char*>(program), size}))
				, patch_(patch)
				, instructions_(this->shader_.get_instructions())
				, assembler_(this->result_.get_assembler())
			{
				this->result_.get_info() = this->shader_.get_info();
				this->result_.get_signatures() = this->shader_.get_signatures();
				this->result_.get_unknown_chunks() = this->shader_.get_unknown_chunks();

				auto& signatures = this->result_.get_signatures();
				const auto inputs = signatures.find(alys::shader::chunk_isgn);
				if (is_vertex_shader && patch.sprite)
				{
					if (inputs == signatures.end())
					{
						throw std::runtime_error("sprite vertex shader has no inputs");
					}

					this->sprite_inputs_ = take_sprite_inputs(inputs->second);
				}
				else if (is_vertex_shader && inputs != signatures.end())
				{
					this->lightmap_ = add_lightmap_input(inputs->second);
					if (patch.wind)
					{
						this->wind_inputs_ = add_wind_inputs(inputs->second);
						this->input_registers_ = order_wind_inputs(inputs->second, this->wind_inputs_->declare_normal);
						this->wind_inputs_->color = this->input_registers_.at(this->wind_inputs_->color);
						this->wind_inputs_->normal = this->input_registers_.at(this->wind_inputs_->normal);
						if (this->lightmap_)
						{
							this->lightmap_->texcoord = this->input_registers_.at(this->lightmap_->texcoord);
							this->lightmap_->lightmap = this->input_registers_.at(this->lightmap_->lightmap);
						}
					}
				}

				if (patch.wind && !this->wind_inputs_)
				{
					throw std::runtime_error("foliage wind on a shader without vertex inputs");
				}

				if (patch.wind && !declares_constant_buffer(this->instructions_, h1_stable_constant_buffer))
				{
					throw std::runtime_error("foliage wind on a vertex shader without the stable constants");
				}

				this->first_code_ = std::ranges::find_if_not(this->instructions_, is_declaration) - this->instructions_.begin();
				const auto last_input = std::find_if(this->instructions_.rbegin(), this->instructions_.rend(), is_input_declaration);
				this->last_input_index_ = this->instructions_.rend() - last_input - 1;
				if ((this->lightmap_ || this->wind_inputs_) && (last_input == this->instructions_.rend() || this->first_code_ == static_cast<std::ptrdiff_t>(this->instructions_.size())))
				{
					throw std::runtime_error("vertex shader has no input declarations or code");
				}

				const auto instance_matrices = declares_instance_matrices(this->instructions_);
				if (instance_matrices)
				{
					this->instance_input_ = find_instance_input(this->instructions_);
					if (this->instance_input_ && !this->input_registers_.empty())
					{
						this->instance_input_ = this->input_registers_.at(*this->instance_input_);
					}
				}

				if (instance_matrices && !this->instance_input_)
				{
					throw std::runtime_error("shader reads the instance matrices without the instance id");
				}

				if (is_vertex_shader && patch.lighting)
				{
					this->vs_lighting_ = find_vertex_lighting(this->shader_, *patch.lighting);
				}

				this->pixel_lighting_ = !is_vertex_shader && patch.lighting.has_value();
				this->pixel_ambient_ = this->pixel_lighting_ && patch.lighting->ambient;
				if (this->pixel_ambient_ && patch.grid != light_grid::probe_buffer && declares_sampler(this->instructions_, ambient_light_grid_sampler))
				{
					throw std::runtime_error(utils::string::va("pixel shader already uses the light grid sampler s%u", ambient_light_grid_sampler));
				}

				if (this->vs_lighting_ && (declares_resource(this->instructions_, light_grid_index_slot) || declares_resource(this->instructions_, light_grid_transform_slot)))
				{
					throw std::runtime_error("vertex shader already uses the light grid volume slots");
				}

				if (this->vs_lighting_ && patch.lighting->instanced && !this->instance_input_)
				{
					throw std::runtime_error("instanced model lighting without the instance id");
				}

				if (this->vs_lighting_)
				{
					auto& output = find_signature_element(this->result_, alys::shader::chunk_osgn, [&](const signature_element& element)
					{
						return element.register_ == this->vs_lighting_->output;
					}, "model lighting output");
					output.mask = 0xF;
					output.rw_mask = 0;
				}
				else if (this->pixel_lighting_)
				{
					auto& input = find_signature_element(this->result_, alys::shader::chunk_isgn, [&](const signature_element& element)
					{
						return element.name == "TEXCOORD" && element.semantic_index == patch.lighting_semantic;
					}, "model lighting input");
					input.mask = 0xF;
					input.rw_mask = 0xF;
					this->lighting_input_ = input.register_;
				}

				if (this->pixel_lighting_)
				{
					this->texture_slots_ = relocate_textures(this->instructions_, patch.lighting->texture);
				}

				this->lit_instance_rows_ = (this->vs_lighting_ && patch.lighting->instanced) || patch.lit_instance_rows;
				this->base_temps_ = this->lightmap_.has_value() || patch.sun_shadow.texture.has_value() ? 1u : 0u;
				const auto lighting_temps = this->vs_lighting_ ? 3u : this->pixel_lighting_ ? 6u : 0u;
				this->sprite_temp_ = this->base_temps_ + lighting_temps;
				this->temp_count_ = this->sprite_temp_ + (this->sprite_inputs_ ? sprite_temp_count : 0u);
				if (this->wind_inputs_)
				{
					this->wind_temp_ = this->temp_count_;
					this->temp_count_ += wind_temp_count;
				}

				if (this->pixel_lighting_ && patch.grid == light_grid::probe_buffer && reads_sun_color(this->instructions_)
					&& (this->pixel_ambient_ || declares_resource(this->instructions_, patch.lighting->texture)))
				{
					this->sun_temp_ = this->temp_count_;
					this->temp_count_ += sun_color_temp_count;
				}

				if (reads_fog_rows(this->instructions_, patch.fog))
				{
					this->fog_temp_ = this->temp_count_;
					this->temp_count_ += fog_temp_count;
				}

				if (!is_vertex_shader && patch.lights)
				{
					this->prepare_lights();
					this->light_temp_ = this->temp_count_;
					this->temp_count_ += light_temp_count;
				}
			}

			program_patcher(const program_patcher&) = delete;
			program_patcher& operator=(const program_patcher&) = delete;

			patched_program patch()
			{
				for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(this->instructions_.size()); i++)
				{
					auto instruction = this->instructions_[i];
					if (!this->input_registers_.empty())
					{
						for (auto& operand : instruction.operands)
						{
							remap_input_registers(operand, this->input_registers_);
						}
					}

					if (this->is_lighting_coords_buffer(instruction) || (this->sprite_inputs_ && is_input_declaration(instruction)) || this->is_reactive_motion_buffer(instruction))
					{
						continue;
					}

					if (this->emit_wind(i, instruction))
					{
						continue;
					}

					if (this->emit_lighting_coords(i))
					{
						continue;
					}

					if (i == this->light_return_)
					{
						this->emit_light_loop_end();
						continue;
					}

					this->patch_lights(i, instruction);
					this->redirect_fog_rows(i, instruction);
					this->patch_constant_buffers(i, instruction);
					this->patch_instance_matrices(i, instruction);
					this->patch_temps(i, instruction);
					if (i == this->first_code_)
					{
						this->emit_prologue();
					}

					this->redirect_lighting_input(i, instruction);
					this->redirect_sun_color(i, instruction);
					this->patch_declaration_masks(instruction);
					this->redirect_lightmap_input(i, instruction);
					this->redirect_sprite_inputs(i, instruction);
					if (this->patch_model_lighting(instruction))
					{
						continue;
					}

					this->patch_sun_shadow(i, instruction);
					const auto ssr_sample = this->patch_.ssr_texture && is_sample(instruction, *this->patch_.ssr_texture);
					relocate_resource_slots(instruction, this->texture_slots_);
					this->assembler_(instruction);
					if (ssr_sample)
					{
						emit_ssr_clamp(this->assembler_, instruction.operands[0]);
					}

					this->emit_epilogue(i, instruction);
				}

				if (this->wind_inputs_ && !this->wind_emitted_)
				{
					throw std::runtime_error("foliage wind on a vertex shader without reactive motion offsets");
				}

				return {this->result_.serialize(), this->lightmap_.has_value(), this->vs_lighting_ ? std::optional(this->vs_lighting_->semantic) : std::nullopt, this->texture_slots_};
			}

		private:
			using instruction_t = alys::shader::detail::instruction_t;
			using operand_t = alys::shader::detail::operand_t;

			alys::shader::shader_object shader_;
			const program_patch& patch_;
			const std::vector<instruction_t>& instructions_;
			alys::shader::shader_object result_;
			alys::shader::assembler assembler_;
			std::optional<lightmap_input> lightmap_;
			std::optional<wind_inputs> wind_inputs_;
			std::unordered_map<std::uint32_t, std::uint32_t> input_registers_;
			std::uint32_t wind_temp_{};
			bool wind_emitted_{};
			std::optional<std::vector<std::pair<std::uint32_t, sprite_input>>> sprite_inputs_;
			std::uint32_t sprite_temp_{};
			std::ptrdiff_t first_code_{};
			std::ptrdiff_t last_input_index_{};
			std::optional<std::uint32_t> instance_input_;
			std::optional<vertex_lighting> vs_lighting_;
			bool pixel_lighting_{};
			bool pixel_ambient_{};
			std::optional<std::uint32_t> sun_temp_;
			std::optional<std::uint32_t> fog_temp_;
			std::optional<std::uint32_t> lighting_input_;
			std::map<std::uint32_t, std::uint32_t> texture_slots_;
			std::unordered_set<std::uint32_t> instance_rows_;
			bool lit_instance_rows_{};
			std::uint32_t base_temps_{};
			std::uint32_t temp_count_{};
			std::optional<std::uint32_t> first_temp_;
			std::optional<std::uint32_t> scratch_temp_;
			std::ptrdiff_t light_return_ = -1;
			std::uint32_t light_temp_{};
			std::uint32_t light_position_input_{};
			std::uint32_t light_screen_input_{};
			bool declares_stable_buffer_{};
			bool declares_screen_input_{};

			void prepare_lights()
			{
				const auto& lights = *this->patch_.lights;
				if (this->patch_.lighting || !lights.rows[0])
				{
					throw std::runtime_error("light pixel shader with model lighting or without the light position");
				}

				check_single_return(this->instructions_, "light");
				check_writes_only_o0(this->result_, "light");

				this->texture_slots_ = relocate_light_textures(this->instructions_);

				auto& screen = find_signature_element(this->result_, alys::shader::chunk_isgn, [](const signature_element& element)
				{
					return element.system_value_type == alys::shader::POS;
				}, "screen position input");
				screen.rw_mask |= 0x3;
				this->light_screen_input_ = screen.register_;
				const auto& position = find_signature_element(this->result_, alys::shader::chunk_isgn, [](const signature_element& element)
				{
					return element.name == "TEXCOORD" && element.semantic_index == h1_eye_position_semantic && (element.rw_mask & 0x7) == 0x7;
				}, "eye relative position input");
				this->light_position_input_ = position.register_;
				this->light_return_ = static_cast<std::ptrdiff_t>(this->instructions_.size() - 1);
			}

			void patch_lights(const std::ptrdiff_t index, instruction_t& instruction)
			{
				if (this->light_return_ < 0)
				{
					return;
				}

				const auto& lights = *this->patch_.lights;
				if (instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER && instruction.operands[0].indices[0].value.uint32 == h1_stable_constant_buffer)
				{
					auto& rows = instruction.operands[0].indices[1].value.uint32;
					rows = std::max({rows, lights.render_target_size_row + 1, lights.tile_parms_row + 1});
					this->declares_stable_buffer_ = true;
				}

				if (instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_PS_SIV && writes_operand(instruction, D3D10_SB_OPERAND_TYPE_INPUT, this->light_screen_input_))
				{
					instruction.operands[0].components.mask |= 0x3;
					this->declares_screen_input_ = true;
				}

				if (index < this->first_code_)
				{
					return;
				}

				for (auto& operand : instruction.operands)
				{
					if (operand.type == D3D10_SB_OPERAND_TYPE_OUTPUT)
					{
						operand = as_temp(operand, *this->first_temp_ + this->light_temp_ + 1);
						continue;
					}

					if (operand.type != D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER || operand.indices[0].value.uint32 != lights.buffer)
					{
						continue;
					}

					if (operand.indices[1].representation != D3D10_SB_OPERAND_INDEX_IMMEDIATE32)
					{
						throw std::runtime_error("light pixel shader indexes the light constants");
					}

					for (auto i = 0u; i < light_const_count; i++)
					{
						if (lights.rows[i] && operand.indices[1].value.uint32 == *lights.rows[i])
						{
							operand = as_temp(operand, *this->first_temp_ + this->light_temp_ + 2 + i);
							break;
						}
					}
				}
			}

			void emit_light_loop_begin()
			{
				auto& a = this->assembler_;
				const auto& lights = *this->patch_.lights;
				const auto stable = s2_stable_constant_buffer;
				declare_forward_lights(a, lights);
				if (!this->declares_screen_input_)
				{
					a.dcl_input_ps_siv(asm_::v(this->light_screen_input_).xy(), asm_::position, asm_::linear_noperspective);
				}

				if (!this->declares_stable_buffer_)
				{
					a.dcl_constantbuffer(asm_::cb(stable, std::max(lights.render_target_size_row, lights.tile_parms_row) + 1).select("xyzw"),
						D3D10_SB_CONSTANT_BUFFER_IMMEDIATE_INDEXED);
				}

				emit_forward_light_loop_begin(a, lights, *this->first_temp_ + this->light_temp_, this->light_screen_input_,
					asm_::v(this->light_position_input_).select("xyzx").swz());
			}

			void emit_light_loop_end()
			{
				emit_forward_light_loop_end(this->assembler_, *this->first_temp_ + this->light_temp_, {});
			}

			bool reads_sun_shadow(const instruction_t& instruction) const
			{
				return std::ranges::any_of(instruction.operands, [&](const operand_t& operand)
				{
					return is_resource(operand, *this->patch_.sun_shadow.texture);
				});
			}

			bool reads_model_lighting(const instruction_t& instruction) const
			{
				return std::ranges::any_of(instruction.operands, [&](const operand_t& operand)
				{
					return is_resource(operand, this->patch_.lighting->texture);
				});
			}

			bool is_lighting_coords_buffer(const instruction_t& instruction) const
			{
				return this->lit_instance_rows_ && instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER
					&& instruction.operands[0].indices[0].value.uint32 == h1_model_lighting_buffer;
			}

			bool is_reactive_motion_buffer(const instruction_t& instruction) const
			{
				return this->wind_inputs_ && instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER
					&& instruction.operands[0].indices[0].value.uint32 == h1_reactive_motion_buffer;
			}

			bool emit_wind(const std::ptrdiff_t index, const instruction_t& instruction)
			{
				if (!this->wind_inputs_ || index < this->first_code_ || !reads_reactive_motion(instruction))
				{
					return false;
				}

				const auto& dest = instruction.operands[0];
				const auto position = std::ranges::find_if(instruction.operands, [](const operand_t& operand)
				{
					return operand.type == D3D10_SB_OPERAND_TYPE_INPUT;
				});

				if (index == this->first_code_ || instruction.opcode.type != D3D10_SB_OPCODE_MUL || dest.type != D3D10_SB_OPERAND_TYPE_TEMP
					|| dest.components.mask != 0x7
					|| position == instruction.operands.end())
				{
					throw std::runtime_error("reactive motion offset is not a position weight times cb9 into a temp xyz");
				}

				const auto temp = *this->first_temp_ + this->wind_temp_;
				std::array<operand_t, 3> world;
				for (auto row = 0u; row < world.size(); row++)
				{
					world[row] = asm_::cb(0, row).select("xyzx").swz();
				}

				if (this->instance_input_)
				{
					const auto row_index = temp + wind_temp_count - 1;
					this->assembler_.imad(asm_::r(row_index).x(), asm_::v(*this->instance_input_).x(),
						uint_literal(this->lit_instance_rows_ ? s2_lit_instance_rows : 1u << s2_instance_row_shift), uint_literal(0));
					for (auto row = 0u; row < world.size(); row++)
					{
						world[row] = asm_::cb(s2_instance_matrix_buffer, 0).select("xyzx").swz();
						world[row].indices[1].representation = D3D10_SB_OPERAND_INDEX_IMMEDIATE32_PLUS_RELATIVE;
						world[row].indices[1].value.uint32 = row;
						world[row].indices[1].extra_operand = std::make_shared<operand_t>(asm_::r(row_index).x().scalar());
					}
				}

				emit_foliage_wind(this->assembler_, dest, position->indices[0].value.uint32, *this->wind_inputs_, *this->patch_.wind, world, temp);
				this->wind_emitted_ = true;
				return true;
			}

			bool emit_lighting_coords(const std::ptrdiff_t index)
			{
				if (!this->vs_lighting_ || index != static_cast<std::ptrdiff_t>(this->vs_lighting_->coords_write))
				{
					return false;
				}

				this->assembler_.mov(asm_::o(this->vs_lighting_->output).select("xyzw"), asm_::r(*this->first_temp_ + this->base_temps_ + 2).select("xyzw"));
				return true;
			}

			void patch_constant_buffers(const std::ptrdiff_t index, instruction_t& instruction) const
			{
				this->patch_material_rows(index, instruction);
				if (index >= this->first_code_)
				{
					for (const auto& [buffer, from, to] : this->patch_.stable_literal_rows)
					{
						for (auto& operand : instruction.operands)
						{
							relocate_constant_row(operand, buffer, from, to, h1_stable_constant_buffer);
						}
					}
				}

				if (instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER && instruction.operands[0].indices[0].value.uint32 == h1_stable_constant_buffer)
				{
					auto& rows = instruction.operands[0].indices[1].value.uint32;
					for (const auto& row : this->patch_.stable_literal_rows)
					{
						rows = std::max(rows, row.to + 1);
					}

					if (const auto& wind = this->patch_.wind)
					{
						rows = std::max({rows, wind->time_row + 1, wind->wind_row + 1, wind->params_row + 1});
					}

					if (this->fog_temp_)
					{
						for (const auto& row : {this->patch_.fog.extinction, this->patch_.fog.in_scatter, this->patch_.fog.sun_dir})
						{
							rows = std::max(rows, row.value_or(0) + 1);
						}
					}
				}

				for (auto& operand : instruction.operands)
				{
					remap_constant_buffers(operand);
				}

				if (this->patch_.lightmap_params_dest && index >= this->first_code_)
				{
					for (auto& operand : instruction.operands)
					{
						relocate_constant_row(operand, 0, *this->patch_.lightmap_params_dest, s2_model_lightmap_params_dest);
					}
				}

				const auto per_prim_rows = this->vs_lighting_ && !this->patch_.lighting->instanced ? s2_model_lighting_index_dest + 1
					: this->patch_.lightmap_params_dest ? s2_model_lightmap_params_dest + 1 : 0u;
				if (per_prim_rows && instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER
					&& instruction.operands[0].indices[0].value.uint32 == 0)
				{
					auto& rows = instruction.operands[0].indices[1].value.uint32;
					rows = std::max(rows, per_prim_rows);
				}
			}

			void patch_material_rows(const std::ptrdiff_t index, instruction_t& instruction) const
			{
				const auto& rows = this->patch_.material_rows;
				if (rows.empty())
				{
					return;
				}

				if (instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER && instruction.operands[0].indices[0].value.uint32 == h1_material_constant_buffer)
				{
					auto& count = instruction.operands[0].indices[1].value.uint32;
					for (const auto& [from, to] : rows)
					{
						count = std::max(count, to + 1);
					}

					return;
				}

				if (index < this->first_code_)
				{
					return;
				}

				for (auto& operand : instruction.operands)
				{
					if (operand.type != D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER || operand.indices[0].value.uint32 != h1_material_constant_buffer)
					{
						continue;
					}

					if (operand.indices[1].representation != D3D10_SB_OPERAND_INDEX_IMMEDIATE32)
					{
						throw std::runtime_error("shader indexes the material constants");
					}

					if (const auto row = rows.find(operand.indices[1].value.uint32); row != rows.end())
					{
						operand.indices[1].value.uint32 = row->second;
					}
				}
			}

			void patch_instance_matrices(const std::ptrdiff_t index, instruction_t& instruction)
			{
				if (!this->instance_input_)
				{
					return;
				}

				if (instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER && reads_instance_matrices(instruction.operands[0]))
				{
					auto& rows = instruction.operands[0].indices[1].value.uint32;
					rows = this->lit_instance_rows_ ? (rows >> h1_instance_row_shift) * s2_lit_instance_rows : rows >> h1_instance_row_shift << s2_instance_row_shift;
				}
				else if (index >= this->first_code_)
				{
					patch_instance_rows(instruction, *this->instance_input_, this->lit_instance_rows_, this->instance_rows_);
				}
			}

			void patch_temps(const std::ptrdiff_t index, instruction_t& instruction)
			{
				if (this->temp_count_ && instruction.opcode.type == D3D10_SB_OPCODE_DCL_TEMPS)
				{
					this->first_temp_ = instruction.operands[0].custom.u.value;
					instruction.operands[0].custom.u.value += this->temp_count_;
				}

				if (this->temp_count_ && index == this->first_code_ && !this->first_temp_)
				{
					this->first_temp_ = 0;
					this->assembler_.dcl_temps(this->temp_count_);
				}

				if (index == this->first_code_ && this->first_temp_ && this->base_temps_)
				{
					this->scratch_temp_ = *this->first_temp_;
				}
			}

			void emit_prologue()
			{
				if (this->vs_lighting_)
				{
					this->assembler_.dcl_resource_structured(asm_::t(light_grid_index_slot), 4u);
					this->assembler_.dcl_resource_structured(asm_::t(light_grid_transform_slot), 64u);
				}

				if (this->pixel_ambient_)
				{
					declare_light_grid(this->assembler_, this->patch_);
					if (this->patch_.grid != light_grid::probe_buffer)
					{
						this->assembler_.dcl_sampler(asm_::s(ambient_light_grid_sampler));
					}

					emit_light_grid_lookup(this->assembler_, this->patch_, asm_::s(ambient_light_grid_sampler), *this->lighting_input_, *this->first_temp_ + this->base_temps_, false);
				}

				if (this->sun_temp_)
				{
					emit_probe_sun_color(this->assembler_, this->patch_, *this->lighting_input_, *this->first_temp_ + *this->sun_temp_);
				}

				if (this->fog_temp_)
				{
					emit_atmos_fog_rows(this->assembler_, this->patch_.fog, *this->first_temp_ + *this->fog_temp_);
				}

				if (this->lightmap_)
				{
					this->assembler_.mov(asm_::r(*this->scratch_temp_).xy(), asm_::v(this->lightmap_->texcoord).xyxx());
					this->assembler_.mov(asm_::r(*this->scratch_temp_).zw(), asm_::v(this->lightmap_->lightmap).xxxy());
				}

				if (this->sprite_inputs_)
				{
					this->assembler_.dcl_input_sgv(asm_::v(0).x(), asm_::vertex_id);
					this->assembler_.dcl_resource_raw(asm_::t(sprite_buffer_slot));
					const auto atlas_blend = std::ranges::any_of(*this->sprite_inputs_, [](const auto& input)
					{
						return input.second == sprite_input::atlas_step;
					});
					emit_sprite_fetch(this->assembler_, *this->first_temp_ + this->sprite_temp_, atlas_blend);
				}

				if (this->light_return_ >= 0)
				{
					this->emit_light_loop_begin();
				}
			}

			void redirect_sprite_inputs(const std::ptrdiff_t index, instruction_t& instruction) const
			{
				if (!this->sprite_inputs_ || index < this->first_code_)
				{
					return;
				}

				for (auto& operand : instruction.operands)
				{
					for (const auto& [input, value] : *this->sprite_inputs_)
					{
						redirect_input(operand, input, *this->first_temp_ + this->sprite_temp_ + static_cast<std::uint32_t>(value));
					}
				}
			}

			void redirect_sun_color(const std::ptrdiff_t index, instruction_t& instruction) const
			{
				if (!this->sun_temp_ || index < this->first_code_)
				{
					return;
				}

				for (auto& operand : instruction.operands)
				{
					if (const auto sun_index = sun_color_index(operand))
					{
						operand = as_temp(operand, *this->first_temp_ + *this->sun_temp_ + 1 + *sun_index);
					}
				}
			}

			void redirect_fog_rows(const std::ptrdiff_t index, instruction_t& instruction) const
			{
				if (!this->fog_temp_ || index < this->first_code_)
				{
					return;
				}

				for (auto& operand : instruction.operands)
				{
					if (const auto fog_index = fog_row_index(operand, this->patch_.fog))
					{
						operand = as_temp(operand, *this->first_temp_ + *this->fog_temp_ + *fog_index);
					}
				}
			}

			void redirect_lighting_input(const std::ptrdiff_t index, instruction_t& instruction) const
			{
				if (!this->pixel_ambient_ || index < this->first_code_)
				{
					return;
				}

				for (auto& operand : instruction.operands)
				{
					redirect_input(operand, *this->lighting_input_, *this->first_temp_ + this->base_temps_ + 5);
				}
			}

			void patch_declaration_masks(instruction_t& instruction) const
			{
				if (this->vs_lighting_ && instruction.opcode.type == D3D10_SB_OPCODE_DCL_OUTPUT
					&& writes_operand(instruction, D3D10_SB_OPERAND_TYPE_OUTPUT, this->vs_lighting_->output))
				{
					instruction.operands[0].components.mask = 0xF;
				}

				if (this->pixel_lighting_ && instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_PS
					&& writes_operand(instruction, D3D10_SB_OPERAND_TYPE_INPUT, *this->lighting_input_))
				{
					instruction.operands[0].components.mask = 0xF;
				}

				if (this->lightmap_ && instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT
					&& writes_operand(instruction, D3D10_SB_OPERAND_TYPE_INPUT, this->lightmap_->texcoord))
				{
					instruction.operands[0].components.mask &= 0x3;
				}
			}

			void redirect_lightmap_input(const std::ptrdiff_t index, instruction_t& instruction) const
			{
				if (!this->lightmap_ || index < this->first_code_)
				{
					return;
				}

				for (auto& operand : instruction.operands)
				{
					redirect_input(operand, this->lightmap_->texcoord, *this->scratch_temp_);
				}
			}

			bool patch_model_lighting(const instruction_t& instruction)
			{
				if (!this->pixel_lighting_ || !this->reads_model_lighting(instruction))
				{
					return false;
				}

				if (instruction.opcode.type == D3D10_SB_OPCODE_DCL_RESOURCE)
				{
					if (instruction.opcode.controls != D3D10_SB_RESOURCE_DIMENSION_TEXTURE3D)
					{
						throw std::runtime_error("model lighting is not a texture3d");
					}

					declare_light_grid(this->assembler_, this->patch_);
					return true;
				}

				if (instruction.opcode.type != D3D10_SB_OPCODE_SAMPLE || !is_sample(instruction, this->patch_.lighting->texture))
				{
					throw std::runtime_error("pixel shader reads the model lighting without a sample");
				}

				emit_probe_lookup(this->assembler_, instruction, this->patch_, *this->lighting_input_, *this->first_temp_ + this->base_temps_);
				return true;
			}

			void patch_sun_shadow(const std::ptrdiff_t index, instruction_t& instruction)
			{
				const auto& sun_shadow = this->patch_.sun_shadow;
				if (!sun_shadow.texture)
				{
					return;
				}

				if (instruction.opcode.type == D3D10_SB_OPCODE_DCL_RESOURCE && this->reads_sun_shadow(instruction))
				{
					if (instruction.opcode.controls != D3D10_SB_RESOURCE_DIMENSION_TEXTURE2D)
					{
						throw std::runtime_error("sun shadowmap is not a texture2d");
					}

					instruction.opcode.controls = D3D10_SB_RESOURCE_DIMENSION_TEXTURE2DARRAY; // s2 keeps each sun cascade in its own slice
				}
				else if (is_sample(instruction, *sun_shadow.texture))
				{
					this->assembler_.mov(asm_::r(*this->scratch_temp_).xy(), instruction.operands[1]);
					this->assembler_.mov(asm_::r(*this->scratch_temp_).z(), asm_::l(find_cascade_slice(this->instructions_, static_cast<std::size_t>(index), sun_shadow)));
					instruction.operands[1] = asm_::r(*this->scratch_temp_).xyzx().swz();
					set_resource_dimension(instruction, D3D10_SB_RESOURCE_DIMENSION_TEXTURE2DARRAY);
				}
				else if (this->reads_sun_shadow(instruction))
				{
					throw std::runtime_error("pixel shader reads the sun shadowmap without a sample");
				}
			}

			void emit_epilogue(const std::ptrdiff_t index, const instruction_t& instruction)
			{
				if (this->lightmap_ && index == this->last_input_index_)
				{
					this->assembler_.dcl_input(asm_::v(this->lightmap_->lightmap).xy());
				}

				if (this->wind_inputs_ && index == this->last_input_index_)
				{
					this->assembler_.dcl_input(asm_::v(this->wind_inputs_->color).xyz());
					if (this->wind_inputs_->declare_normal)
					{
						this->assembler_.dcl_input(asm_::v(this->wind_inputs_->normal).xyz());
					}
				}

				if (this->vs_lighting_ && index == static_cast<std::ptrdiff_t>(this->vs_lighting_->position_write))
				{
					emit_vertex_lighting(this->assembler_, instruction.operands[1], *this->patch_.lighting, this->instance_input_, *this->first_temp_ + this->base_temps_);
				}
			}
		};
	}

	patched_program patch_program(const unsigned char* program, const unsigned int size, const bool is_vertex_shader, const program_patch& patch)
	{
		program_patcher patcher(program, size, is_vertex_shader, patch);
		return patcher.patch();
	}
}
