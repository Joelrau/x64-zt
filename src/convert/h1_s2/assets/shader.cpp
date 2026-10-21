#include <std_include.hpp>

#include "../include.hpp"
#include "shader.hpp"

#include <shader-tool/shader.hpp>

namespace asm_ = alys::shader::literals;

namespace zonetool::h1::converter::s2::shader
{
	namespace
	{
		constexpr auto h1_stable_constant_buffer = 2u;
		constexpr auto h1_constant_buffer_count = 5u;
		constexpr auto h1_instance_matrix_buffer = 6u;
		constexpr auto s2_instance_matrix_buffer = 7u;
		constexpr auto h1_instance_row_shift = 2u;
		constexpr auto s2_instance_row_shift = 3u;
		constexpr auto h1_model_lighting_buffer = 7u;
		constexpr auto s2_lit_instance_rows = 9u;
		constexpr auto s2_lit_instance_lighting_row = 8u;
		constexpr auto light_grid_direction_count = 12u;
		constexpr auto light_grid_volume_slot = 7u; // fixed t7 of the stock "lmdl" shaders

		constexpr std::array<std::array<float, 3>, light_grid_direction_count> light_grid_directions
		{{
			{0.850651f, 0.525731f, 0.0f},
			{-0.850651f, 0.525731f, 0.0f},
			{0.850651f, -0.525731f, 0.0f},
			{-0.850651f, -0.525731f, 0.0f},
			{0.0f, 0.850651f, 0.525731f},
			{0.0f, -0.850651f, 0.525731f},
			{0.0f, 0.850651f, -0.525731f},
			{0.0f, -0.850651f, -0.525731f},
			{0.525731f, 0.0f, 0.850651f},
			{-0.525731f, 0.0f, 0.850651f},
			{0.525731f, 0.0f, -0.850651f},
			{-0.525731f, 0.0f, -0.850651f},
		}};

		using signature_element = alys::shader::shader_object::signature_element;

		void relocate_constant_row(alys::shader::detail::operand_t& operand, const std::uint32_t buffer, const std::uint32_t from, const std::uint32_t to,
			const std::optional<std::uint32_t> to_buffer = {})
		{
			for (auto& index : operand.indices)
			{
				if (index.extra_operand)
				{
					relocate_constant_row(*index.extra_operand, buffer, from, to, to_buffer);
				}
			}

			if (operand.type == D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER && operand.indices[0].value.uint32 == buffer
				&& operand.indices[1].representation == D3D10_SB_OPERAND_INDEX_IMMEDIATE32 && operand.indices[1].value.uint32 == from)
			{
				operand.indices[0].value.uint32 = to_buffer.value_or(buffer);
				operand.indices[1].value.uint32 = to;
			}
		}

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

		void redirect_input(alys::shader::detail::operand_t& operand, const std::uint32_t input, const std::uint32_t temp)
		{
			for (auto& index : operand.indices)
			{
				if (index.extra_operand)
				{
					redirect_input(*index.extra_operand, input, temp);
				}
			}

			if (operand.type != D3D10_SB_OPERAND_TYPE_INPUT)
			{
				return;
			}

			if (operand.indices[0].representation != D3D10_SB_OPERAND_INDEX_IMMEDIATE32)
			{
				throw std::runtime_error("vertex shader indexes its inputs");
			}

			if (operand.indices[0].value.uint32 == input)
			{
				operand.type = D3D10_SB_OPERAND_TYPE_TEMP;
				operand.indices[0].value.uint32 = temp;
			}
		}

		bool is_declaration(const alys::shader::detail::instruction_t& instruction)
		{
			return instruction.opcode.type == D3D10_SB_OPCODE_CUSTOMDATA
				|| (alys::shader::detail::get_instruction_flags(instruction) & alys::shader::detail::flag_declaration);
		}

		bool is_input_declaration(const alys::shader::detail::instruction_t& instruction)
		{
			return instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT
				|| instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_SGV
				|| instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_SIV;
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

			// declaring zw too is a d3d11 input layout linkage error
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

		bool is_destination(const alys::shader::detail::operand_t& operand)
		{
			return operand.type == D3D10_SB_OPERAND_TYPE_NULL || (operand.type != D3D10_SB_OPERAND_TYPE_IMMEDIATE32
				&& operand.components.type == D3D10_SB_OPERAND_4_COMPONENT && operand.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE);
		}

		bool is_resource(const alys::shader::detail::operand_t& operand, const std::uint32_t texture)
		{
			return operand.type == D3D10_SB_OPERAND_TYPE_RESOURCE && operand.indices[0].value.uint32 == texture;
		}

		bool is_sample(const alys::shader::detail::instruction_t& instruction, const std::uint32_t texture)
		{
			switch (instruction.opcode.type)
			{
			case D3D10_SB_OPCODE_SAMPLE:
			case D3D10_SB_OPCODE_SAMPLE_C:
			case D3D10_SB_OPCODE_SAMPLE_C_LZ:
			case D3D10_SB_OPCODE_SAMPLE_L:
			case D3D10_SB_OPCODE_SAMPLE_D:
			case D3D10_SB_OPCODE_SAMPLE_B:
			case D3D10_1_SB_OPCODE_GATHER4:
			case D3D11_SB_OPCODE_GATHER4_C:
				return is_resource(instruction.operands[2], texture);
			default:
				return false;
			}
		}

		std::uint8_t read_components(const alys::shader::detail::operand_t& source, const std::uint8_t written, const std::uint32_t opcode)
		{
			if (source.components.type != D3D10_SB_OPERAND_4_COMPONENT)
			{
				return 0;
			}

			if (source.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_SELECT_1_MODE)
			{
				return static_cast<std::uint8_t>(1 << source.components.names[0]);
			}

			const auto dot_size = opcode == D3D10_SB_OPCODE_DP2 ? 2 : opcode == D3D10_SB_OPCODE_DP3 ? 3 : opcode == D3D10_SB_OPCODE_DP4 ? 4 : 0;
			std::uint8_t result{};
			for (auto component = 0; component < 4; component++)
			{
				const auto used = dot_size ? component < dot_size : (written >> component) & 1;
				if (used)
				{
					result |= static_cast<std::uint8_t>(1 << source.components.names[component]);
				}
			}

			return result;
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

			// "sst" samples the unfiltered cascade depth right after the filtered samples
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

		void set_resource_dimension(alys::shader::detail::instruction_t& instruction, const std::uint16_t dimension)
		{
			for (auto& extension : instruction.opcode.extensions)
			{
				if (extension.type == D3D11_SB_EXTENDED_OPCODE_RESOURCE_DIM)
				{
					extension.values[0] = dimension;
				}
			}
		}

		bool declares_instance_matrices(const std::vector<alys::shader::detail::instruction_t>& instructions)
		{
			return std::ranges::any_of(instructions, [](const alys::shader::detail::instruction_t& instruction)
			{
				return instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER && instruction.operands[0].indices[0].value.uint32 == h1_instance_matrix_buffer;
			});
		}

		std::optional<std::uint32_t> find_instance_input(const std::vector<alys::shader::detail::instruction_t>& instructions)
		{
			const auto input = std::ranges::find_if(instructions, [](const alys::shader::detail::instruction_t& instruction)
			{
				return instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_SGV && instruction.operands[1].custom.u.value == D3D10_SB_NAME_INSTANCE_ID;
			});

			return input != instructions.end() ? std::optional(input->operands[0].indices[0].value.uint32) : std::nullopt;
		}

		bool reads_instance_matrices(const alys::shader::detail::operand_t& operand)
		{
			return operand.type == D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER && operand.indices[0].value.uint32 == s2_instance_matrix_buffer;
		}

		std::uint32_t temp_component(const std::uint32_t temp, const std::uint32_t component)
		{
			return temp * 4 + component;
		}

		bool reads_instance_row(const alys::shader::detail::operand_t& operand, const std::unordered_set<std::uint32_t>& instance_rows)
		{
			const auto& row = operand.indices[1];
			const auto is_relative = row.representation == D3D10_SB_OPERAND_INDEX_RELATIVE;
			const auto offset = is_relative ? 0u : row.value.uint32;
			if ((!is_relative && row.representation != D3D10_SB_OPERAND_INDEX_IMMEDIATE32_PLUS_RELATIVE) || offset >= (1u << h1_instance_row_shift)
				|| !row.extra_operand || row.extra_operand->type != D3D10_SB_OPERAND_TYPE_TEMP)
			{
				return false;
			}

			const auto& temp = *row.extra_operand;
			return instance_rows.contains(temp_component(temp.indices[0].value.uint32, temp.components.names[0]));
		}

		bool is_instance_row(const alys::shader::detail::instruction_t& instruction, const std::uint32_t instance_input)
		{
			const auto& operands = instruction.operands;
			return instruction.opcode.type == D3D10_SB_OPCODE_ISHL && operands[0].type == D3D10_SB_OPERAND_TYPE_TEMP
				&& operands[1].type == D3D10_SB_OPERAND_TYPE_INPUT && operands[1].indices[0].value.uint32 == instance_input
				&& operands[2].type == D3D10_SB_OPERAND_TYPE_IMMEDIATE32 && operands[2].immediate_values[0].uint32 == h1_instance_row_shift;
		}

		void patch_instance_rows(alys::shader::detail::instruction_t& instruction, const std::uint32_t instance_input, const bool lit_rows,
			std::unordered_set<std::uint32_t>& instance_rows)
		{
			const auto reads_without_row = std::ranges::any_of(instruction.operands, [&](const alys::shader::detail::operand_t& operand)
			{
				return reads_instance_matrices(operand) && !reads_instance_row(operand, instance_rows);
			});

			if (reads_without_row)
			{
				throw std::runtime_error("shader reads the instance matrices without the instance row");
			}

			const auto& operands = instruction.operands;
			const auto first_source = std::ranges::find_if_not(operands, is_destination) - operands.begin();
			for (auto dest = 0; dest < first_source; dest++)
			{
				for (auto component = 0u; operands[dest].type == D3D10_SB_OPERAND_TYPE_TEMP && component < 4; component++)
				{
					if ((operands[dest].components.mask >> component) & 1)
					{
						instance_rows.erase(temp_component(operands[dest].indices[0].value.uint32, component));
					}
				}
			}

			if (!is_instance_row(instruction, instance_input))
			{
				return;
			}

			if (lit_rows)
			{
				instruction.opcode.type = D3D10_SB_OPCODE_IMAD;
				instruction.operands[2].immediate_values[0].uint32 = s2_lit_instance_rows;
				auto zero = instruction.operands[2];
				zero.immediate_values[0].uint32 = 0;
				instruction.operands.emplace_back(zero);
			}
			else
			{
				instruction.operands[2].immediate_values[0].uint32 = s2_instance_row_shift;
			}

			for (auto component = 0u; component < 4; component++)
			{
				if ((operands[0].components.mask >> component) & 1)
				{
					instance_rows.insert(temp_component(operands[0].indices[0].value.uint32, component));
				}
			}
		}

		alys::shader::detail::operand_t uint_literal(const std::uint32_t value)
		{
			auto result = asm_::l(0.0f);
			result.immediate_values[0].uint32 = value;
			return result;
		}

		signature_element& find_signature_element(alys::shader::shader_object& shader, const std::uint32_t chunk,
			const std::function<bool(const signature_element&)>& accept, const char* what)
		{
			auto& signatures = shader.get_signatures();
			if (const auto elements = signatures.find(chunk); elements != signatures.end())
			{
				if (const auto element = std::ranges::find_if(elements->second, accept); element != elements->second.end())
				{
					return *element;
				}
			}

			throw std::runtime_error(utils::string::va("shader has no %s", what));
		}

		bool declares_resource(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::uint32_t slot)
		{
			return std::ranges::any_of(instructions, [&](const alys::shader::detail::instruction_t& instruction)
			{
				return is_declaration(instruction) && !instruction.operands.empty() && is_resource(instruction.operands[0], slot);
			});
		}

		bool declares_sampler(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::uint32_t slot)
		{
			return std::ranges::any_of(instructions, [&](const alys::shader::detail::instruction_t& instruction)
			{
				return instruction.opcode.type == D3D10_SB_OPCODE_DCL_SAMPLER && instruction.operands[0].indices[0].value.uint32 == slot;
			});
		}

		bool is_light_grid_slot(const std::uint32_t slot)
		{
			return (slot >= 7 && slot <= 9) || (slot >= 16 && slot <= 25);
		}

		std::map<std::uint32_t, std::uint32_t> relocate_textures(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::uint32_t lighting_texture)
		{
			constexpr auto texture_slot_count = 16u;

			std::set<std::uint32_t> used;
			for (const auto& instruction : instructions)
			{
				if (is_declaration(instruction) && !instruction.operands.empty() && instruction.operands[0].type == D3D10_SB_OPERAND_TYPE_RESOURCE)
				{
					used.insert(instruction.operands[0].indices[0].value.uint32);
				}
			}

			used.erase(lighting_texture);

			std::vector<std::uint32_t> free;
			for (auto slot = 0u; slot < texture_slot_count; slot++)
			{
				if (!is_light_grid_slot(slot) && !used.contains(slot))
				{
					free.emplace_back(slot);
				}
			}

			std::map<std::uint32_t, std::uint32_t> result;
			auto next = free.begin();
			for (const auto slot : used)
			{
				if (!is_light_grid_slot(slot))
				{
					continue;
				}

				if (next == free.end())
				{
					throw std::runtime_error(utils::string::va("pixel shader has no free slot for t%u", slot));
				}

				result.emplace(slot, *next++);
			}

			return result;
		}

		struct vertex_lighting
		{
			std::size_t coords_write;
			std::size_t position_write;
			std::uint32_t output;
			std::uint32_t semantic;
		};

		vertex_lighting find_vertex_lighting(alys::shader::shader_object& shader, const model_lighting& lighting)
		{
			const auto& instructions = shader.get_instructions();
			const auto writes_coords = [&](const alys::shader::detail::instruction_t& instruction)
			{
				if (instruction.opcode.type != D3D10_SB_OPCODE_MOV || instruction.operands[0].type != D3D10_SB_OPERAND_TYPE_OUTPUT
					|| instruction.operands[1].type != D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER)
				{
					return false;
				}

				const auto& source = instruction.operands[1];
				if (lighting.instanced)
				{
					return source.indices[0].value.uint32 == h1_model_lighting_buffer;
				}

				return source.indices[0].value.uint32 == 0 && source.indices[1].representation == D3D10_SB_OPERAND_INDEX_IMMEDIATE32
					&& source.indices[1].value.uint32 == lighting.coords_dest;
			};

			const auto coords_write = std::ranges::find_if(instructions, writes_coords);
			if (coords_write == instructions.end() || std::find_if(coords_write + 1, instructions.end(), writes_coords) != instructions.end())
			{
				throw std::runtime_error("vertex shader does not write the model lighting coords once");
			}

			vertex_lighting result{static_cast<std::size_t>(coords_write - instructions.begin())};
			result.output = coords_write->operands[0].indices[0].value.uint32;
			result.semantic = find_signature_element(shader, alys::shader::chunk_osgn, [&](const signature_element& element)
			{
				return element.register_ == result.output;
			}, "model lighting output").semantic_index;

			const auto position = find_signature_element(shader, alys::shader::chunk_osgn, [](const signature_element& element)
			{
				return element.system_value_type == alys::shader::POS;
			}, "position output").register_;

			std::optional<std::size_t> position_write;
			for (auto i = 0u; i < result.coords_write; i++)
			{
				const auto& operands = instructions[i].operands;
				if (instructions[i].opcode.type == D3D10_SB_OPCODE_DP4 && operands[0].type == D3D10_SB_OPERAND_TYPE_OUTPUT
					&& operands[0].indices[0].value.uint32 == position && operands[1].type == D3D10_SB_OPERAND_TYPE_TEMP)
				{
					position_write = i;
				}
			}

			if (!position_write)
			{
				throw std::runtime_error("vertex shader writes no clip position before the model lighting coords");
			}

			result.position_write = *position_write;
			return result;
		}

		void emit_vertex_lighting(alys::shader::assembler& a, const alys::shader::detail::operand_t& position, const model_lighting& lighting,
			const std::optional<std::uint32_t> instance_input, const std::uint32_t temp)
		{
			const auto world = temp;
			const auto volume = temp + 1;
			const auto coords = temp + 2;

			a.add(asm_::r(world).xyz(), position, asm_::cb(h1_stable_constant_buffer + 1, lighting.eye_offset_dest).select("xyzx"));
			a.mov(asm_::r(world).w(), asm_::l(1.0f));

			if (lighting.instanced)
			{
				a.imad(asm_::r(volume).x(), asm_::v(*instance_input).x(), uint_literal(s2_lit_instance_rows), uint_literal(s2_lit_instance_lighting_row));
				alys::shader::detail::operand_t index = asm_::cb(s2_instance_matrix_buffer, 0).select("xxxx").swz();
				index.indices[1].representation = D3D10_SB_OPERAND_INDEX_RELATIVE;
				index.indices[1].value.uint32 = 0;
				index.indices[1].extra_operand = std::make_shared<alys::shader::detail::operand_t>(asm_::r(volume).x().scalar());
				a.ftou(asm_::r(volume).x(), index);
			}
			else
			{
				a.ftou(asm_::r(volume).x(), asm_::cb(0, s2_model_lighting_index_dest).select("xxxx").swz());
			}

			a.ld_structured(asm_::r(volume).x(), asm_::r(volume).x(), uint_literal(0), asm_::t(5).select("xxxx"));
			for (auto row = 0u; row < 3; row++)
			{
				a.ld_structured(asm_::r(coords).select("xyzw"), asm_::r(volume).x(), uint_literal(row * 16), asm_::t(6).select("xyzw"));
				a.dp4(asm_::r(volume).select(alys::shader::component_y + row), asm_::r(coords).select("xyzw"), asm_::r(world).select("xyzw"));
			}

			a.mov(asm_::r(coords).xyz(), asm_::r(volume).select("yzwy"));
			a.utof(asm_::r(coords).w(), asm_::r(volume).x());
		}

		void emit_light_grid_lookup(alys::shader::assembler& a, const program_patch& patch, const alys::shader::detail::operand_t& sampler,
			const std::uint32_t input, const std::uint32_t temp, const bool directional)
		{
			constexpr auto ambient_weight = 0.7f / 6.0f + 0.3f / 10.0f; // sphere mean of the directional weight

			const auto dir = temp;
			const auto volume = temp + 1;
			const auto coords = temp + 2;
			const auto probe = temp + 3;
			const auto weight = temp + 4;
			const auto color = temp + 5;
			const auto texture = light_grid_volume_slot;

			a.add(asm_::r(volume).x(), asm_::v(input).w(), asm_::l(0.5f));
			a.ftou(asm_::r(volume).x(), asm_::r(volume).x());
			a.ld_structured(asm_::r(coords).xyz(), asm_::r(volume).x(), uint_literal(0), asm_::t(texture).select("xyzx"));
			a.ld_structured(asm_::r(probe).xyz(), asm_::r(volume).x(), uint_literal(16), asm_::t(texture).select("xyzx"));
			a.mov_sat(asm_::r(weight).xyz(), asm_::v(input).select("xyzx"));
			a.mad(asm_::r(coords).xyz(), asm_::r(weight).select("xyzx"), asm_::r(coords).select("xyzx"), asm_::r(probe).select("xyzx"));
			if (patch.grid == light_grid::probe_buffer)
			{
				a.add(asm_::r(coords).x(), asm_::r(coords).x(), asm_::l(0.5f));
				a.ftoi(asm_::r(coords).x(), asm_::r(coords).x());
			}

			a.mov(asm_::r(color).select("xyzw"), asm_::l(0.0f, 0.0f, 0.0f, 0.0f));
			for (auto i = 0u; i < light_grid_direction_count; i++)
			{
				const auto& direction = light_grid_directions[i];
				if (directional)
				{
					a.dp3_sat(asm_::r(weight).x(), asm_::r(dir).select("xyzx"), asm_::l(direction[0], direction[1], direction[2], 0.0f));
					a.mul(asm_::r(weight).x(), asm_::r(weight).x(), asm_::r(weight).x());
					a.mul(asm_::r(weight).y(), asm_::r(weight).x(), asm_::r(weight).x());
					a.mul(asm_::r(weight).y(), asm_::r(weight).y(), asm_::l(0.3f));
					a.mad(asm_::r(weight).x(), asm_::r(weight).x(), asm_::l(0.7f), asm_::r(weight).y());
				}
				else
				{
					a.mov(asm_::r(weight).x(), asm_::l(ambient_weight));
				}

				switch (patch.grid)
				{
				case light_grid::probe_buffer:
					a.imad(asm_::r(probe).x(), asm_::r(coords).x(), uint_literal(light_grid_direction_count), uint_literal(i));
					a.ld_indexable(asm_::r(probe).xyz(), asm_::r(probe).select("xxxx"), asm_::t(8).select("xyzw"));
					break;
				case light_grid::array_2d:
					a.mov(asm_::r(probe).xy(), asm_::r(coords).select("xyxx"));
					a.mov(asm_::r(probe).z(), asm_::l(static_cast<float>(i)));
					a.sample_l_indexable(asm_::r(probe).xyz(), asm_::r(probe).select("xyzx"), asm_::t(light_grid_colors_slot).select("xyzw"), sampler, asm_::l(0.0f));
					break;
				case light_grid::volume_3d:
					a.mov(asm_::r(probe).xy(), asm_::r(coords).select("xyxx"));
					a.mad(asm_::r(probe).z(), asm_::r(coords).z(), asm_::l(1.0f / light_grid_direction_count),
						asm_::l(static_cast<float>(i) / light_grid_direction_count));
					a.sample_l_indexable(asm_::r(probe).xyz(), asm_::r(probe).select("xyzx"), asm_::t(light_grid_colors_slot).select("xyzw"), sampler, asm_::l(0.0f));
					break;
				}

				a.mad(asm_::r(color).xyz(), asm_::r(probe).select("xyzx"), asm_::r(weight).select("xxxx"), asm_::r(color).select("xyzx"));
			}

			switch (patch.grid)
			{
			case light_grid::probe_buffer:
				a.ld_indexable(asm_::r(probe).x(), asm_::r(coords).select("xxxx"), asm_::t(9).select("xyzw"));
				break;
			case light_grid::array_2d:
				a.sample_l_indexable(asm_::r(probe).x(), asm_::r(coords).select("xyxx"), asm_::t(light_grid_visibility_slot).select("xyzw"), sampler, asm_::l(0.0f));
				break;
			case light_grid::volume_3d:
				a.sample_l_indexable(asm_::r(probe).x(), asm_::r(coords).select("xyzx"), asm_::t(light_grid_visibility_slot).select("xyzw"), sampler, asm_::l(0.0f));
				break;
			}

			a.mov(asm_::r(color).w(), asm_::r(probe).x());
		}

		void emit_probe_lookup(alys::shader::assembler& a, const alys::shader::detail::instruction_t& sample, const program_patch& patch,
			const std::uint32_t input, const std::uint32_t temp)
		{
			const auto dir = temp;
			const auto color = temp + 5;

			a.mad(asm_::r(dir).xyz(), asm_::v(input).select("xyzx"), asm_::l(-1.0f), sample.operands[1]);
			a.dp3(asm_::r(dir).w(), asm_::r(dir).select("xyzx"), asm_::r(dir).select("xyzx"));
			a.rsq(asm_::r(dir).w(), asm_::r(dir).w());
			a.mul(asm_::r(dir).xyz(), asm_::r(dir).select("xyzx"), asm_::r(dir).select("wwww"));

			emit_light_grid_lookup(a, patch, sample.operands[3], input, temp, true);

			alys::shader::detail::operand_t result = asm_::r(color).select("xyzw").swz();
			std::ranges::copy(sample.operands[2].components.names, result.components.names);
			a.mov(sample.operands[0], result);
		}

		void declare_light_grid(alys::shader::assembler& a, const program_patch& patch)
		{
			constexpr std::array<std::uint8_t, 4> float4{asm_::t_float, asm_::t_float, asm_::t_float, asm_::t_float};

			a.dcl_resource_structured(asm_::t(light_grid_volume_slot), 32u);
			switch (patch.grid)
			{
			case light_grid::probe_buffer:
				a.dcl_resource(asm_::t(8), asm_::buffer, float4);
				a.dcl_resource(asm_::t(9), asm_::buffer, float4);
				break;
			case light_grid::array_2d:
				a.dcl_resource(asm_::t(light_grid_colors_slot), asm_::texture2darray, float4);
				a.dcl_resource(asm_::t(light_grid_visibility_slot), asm_::texture2d, float4);
				break;
			case light_grid::volume_3d:
				a.dcl_resource(asm_::t(light_grid_colors_slot), asm_::texture3d, float4);
				a.dcl_resource(asm_::t(light_grid_visibility_slot), asm_::texture3d, float4);
				break;
			}
		}

		bool writes_operand(const alys::shader::detail::instruction_t& instruction, const std::uint32_t type, const std::uint32_t index)
		{
			return !instruction.operands.empty() && instruction.operands[0].type == type && instruction.operands[0].indices[0].value.uint32 == index;
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
				if (is_vertex_shader && inputs != signatures.end())
				{
					this->lightmap_ = add_lightmap_input(inputs->second);
				}

				this->first_code_ = std::ranges::find_if_not(this->instructions_, is_declaration) - this->instructions_.begin();
				const auto last_input = std::find_if(this->instructions_.rbegin(), this->instructions_.rend(), is_input_declaration);
				this->last_input_index_ = this->instructions_.rend() - last_input - 1;
				if (this->lightmap_ && (last_input == this->instructions_.rend() || this->first_code_ == static_cast<std::ptrdiff_t>(this->instructions_.size())))
				{
					throw std::runtime_error("vertex shader has no input declarations or code");
				}

				const auto instance_matrices = declares_instance_matrices(this->instructions_);
				if (instance_matrices)
				{
					this->instance_input_ = find_instance_input(this->instructions_);
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

				if (this->vs_lighting_ && (declares_resource(this->instructions_, 5) || declares_resource(this->instructions_, 6)))
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

				this->lit_instance_rows_ = this->vs_lighting_ && patch.lighting->instanced;
				this->base_temps_ = this->lightmap_.has_value() || patch.sun_shadow.texture.has_value() ? 1u : 0u;
				const auto lighting_temps = this->vs_lighting_ ? 3u : this->pixel_lighting_ ? 6u : 0u;
				this->temp_count_ = this->base_temps_ + lighting_temps;
			}

			program_patcher(const program_patcher&) = delete;
			program_patcher& operator=(const program_patcher&) = delete;

			patched_program patch()
			{
				for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(this->instructions_.size()); i++)
				{
					auto instruction = this->instructions_[i];
					if (this->is_lighting_coords_buffer(instruction))
					{
						continue;
					}

					if (this->emit_lighting_coords(i))
					{
						continue;
					}

					this->patch_constant_buffers(i, instruction);
					this->patch_instance_matrices(i, instruction);
					this->patch_temps(i, instruction);
					if (i == this->first_code_)
					{
						this->emit_prologue();
					}

					this->redirect_lighting_input(i, instruction);
					this->patch_declaration_masks(instruction);
					this->redirect_lightmap_input(i, instruction);
					if (this->patch_model_lighting(instruction))
					{
						continue;
					}

					this->patch_sun_shadow(i, instruction);
					this->relocate_texture_slots(instruction);
					this->assembler_(instruction);
					this->emit_epilogue(i, instruction);
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
			std::ptrdiff_t first_code_{};
			std::ptrdiff_t last_input_index_{};
			std::optional<std::uint32_t> instance_input_;
			std::optional<vertex_lighting> vs_lighting_;
			bool pixel_lighting_{};
			bool pixel_ambient_{};
			std::optional<std::uint32_t> lighting_input_;
			std::map<std::uint32_t, std::uint32_t> texture_slots_;
			std::unordered_set<std::uint32_t> instance_rows_;
			bool lit_instance_rows_{};
			std::uint32_t base_temps_{};
			std::uint32_t temp_count_{};
			std::optional<std::uint32_t> first_temp_;
			std::optional<std::uint32_t> temp_;

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

				if (!this->patch_.stable_literal_rows.empty() && instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER
					&& instruction.operands[0].indices[0].value.uint32 == h1_stable_constant_buffer)
				{
					auto& rows = instruction.operands[0].indices[1].value.uint32;
					for (const auto& row : this->patch_.stable_literal_rows)
					{
						rows = std::max(rows, row.to + 1);
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
					this->temp_ = *this->first_temp_;
				}
			}

			void emit_prologue()
			{
				if (this->vs_lighting_)
				{
					this->assembler_.dcl_resource_structured(asm_::t(5), 4u);
					this->assembler_.dcl_resource_structured(asm_::t(6), 64u);
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

				if (this->lightmap_)
				{
					this->assembler_.mov(asm_::r(*this->temp_).xy(), asm_::v(this->lightmap_->texcoord).xyxx());
					this->assembler_.mov(asm_::r(*this->temp_).zw(), asm_::v(this->lightmap_->lightmap).xxxy());
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
					redirect_input(operand, this->lightmap_->texcoord, *this->temp_);
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
					this->assembler_.mov(asm_::r(*this->temp_).xy(), instruction.operands[1]);
					this->assembler_.mov(asm_::r(*this->temp_).z(), asm_::l(find_cascade_slice(this->instructions_, static_cast<std::size_t>(index), sun_shadow)));
					instruction.operands[1] = asm_::r(*this->temp_).xyzx().swz();
					set_resource_dimension(instruction, D3D10_SB_RESOURCE_DIMENSION_TEXTURE2DARRAY);
				}
				else if (this->reads_sun_shadow(instruction))
				{
					throw std::runtime_error("pixel shader reads the sun shadowmap without a sample");
				}
			}

			void relocate_texture_slots(instruction_t& instruction) const
			{
				for (auto& operand : instruction.operands)
				{
					const auto slot = operand.type == D3D10_SB_OPERAND_TYPE_RESOURCE ? this->texture_slots_.find(operand.indices[0].value.uint32) : this->texture_slots_.end();
					if (slot != this->texture_slots_.end())
					{
						operand.indices[0].value.uint32 = slot->second;
					}
				}
			}

			void emit_epilogue(const std::ptrdiff_t index, const instruction_t& instruction)
			{
				if (this->lightmap_ && index == this->last_input_index_)
				{
					this->assembler_.dcl_input(asm_::v(this->lightmap_->lightmap).xy());
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
