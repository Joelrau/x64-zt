#include <std_include.hpp>

#include "../include.hpp"
#include "shader.hpp"

#include <shader-tool/shader.hpp>

namespace asm_ = alys::shader::literals;

namespace zonetool::h1::converter::s2::shader
{
	namespace
	{
		constexpr auto h1_constant_buffer_count = 5u;
		constexpr auto h1_instance_matrix_buffer = 6u;
		constexpr auto s2_instance_matrix_buffer = 7u;
		constexpr auto h1_instance_row_shift = 2u;
		constexpr auto s2_instance_row_shift = 3u;
		constexpr auto h1_model_lighting_buffer = 7u;
		constexpr auto s2_lit_instance_rows = 9u;
		constexpr auto s2_lit_instance_lighting_row = 8u;
		constexpr auto light_grid_direction_count = 12u;
		constexpr auto light_grid_index_slot = 5u; // vertex shader structured t5, lighting index -> volume
		constexpr auto light_grid_transform_slot = 6u; // vertex shader structured t6, world -> volume rows of each volume
		constexpr auto light_grid_volume_slot = 7u; // fixed t7 of the stock "lmdl" shaders
		constexpr auto light_grid_probe_colors_slot = 8u; // "lgv0d" buffer t8 with the direction colors of each probe
		constexpr auto light_grid_probe_visibility_slot = 9u; // "lgv0d" buffer t9 with the sun visibility of each probe

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

		constexpr std::array<std::uint8_t, 4> float4_type{asm_::t_float, asm_::t_float, asm_::t_float, asm_::t_float};
		constexpr std::array<std::uint8_t, 4> uint4_type{asm_::t_uint, asm_::t_uint, asm_::t_uint, asm_::t_uint};

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

		alys::shader::detail::operand_t uint_literals(const std::uint32_t x, const std::uint32_t y, const std::uint32_t z, const std::uint32_t w)
		{
			auto result = asm_::l(0.0f, 0.0f, 0.0f, 0.0f);
			result.immediate_values[0].uint32 = x;
			result.immediate_values[1].uint32 = y;
			result.immediate_values[2].uint32 = z;
			result.immediate_values[3].uint32 = w;
			return result;
		}

		enum class sprite_input
		{
			position,
			color,
			texcoord,
			normal,
			atlas_step,
		};

		constexpr auto sprite_temp_count = 8u;

		std::optional<sprite_input> find_sprite_input(const signature_element& element)
		{
			if ((element.name == "POSITION" || element.name == "SV_POSITION") && element.semantic_index == 0)
			{
				return sprite_input::position;
			}

			if (element.name == "COLOR" && element.semantic_index == 0)
			{
				return sprite_input::color;
			}

			if (element.name == "TEXCOORD" && element.semantic_index == 0)
			{
				return sprite_input::texcoord;
			}

			if (element.name == "NORMAL" && element.semantic_index == 0)
			{
				return sprite_input::normal;
			}

			// h1 atlas blend ("tab") shaders read the v step to the next frame here and the u step from NORMAL
			if (element.name == "TEXCOORD" && element.semantic_index == 1)
			{
				return sprite_input::atlas_step;
			}

			return {};
		}

		// the sprite draw binds no vertex buffer, the vertex id is the only input
		std::vector<std::pair<std::uint32_t, sprite_input>> take_sprite_inputs(alys::shader::shader_object::signature& inputs)
		{
			std::vector<std::pair<std::uint32_t, sprite_input>> result;
			for (const auto& element : inputs)
			{
				const auto input = find_sprite_input(element);
				if (!input)
				{
					throw std::runtime_error(utils::string::va("sprite vertex shader reads %s%u", element.name.data(), element.semantic_index));
				}

				result.emplace_back(element.register_, *input);
			}

			signature_element vertex_id{};
			vertex_id.name = "SV_VertexID";
			vertex_id.system_value_type = alys::shader::VERTID;
			vertex_id.component_type = alys::shader::format_uint;
			vertex_id.mask = 0x1;
			vertex_id.rw_mask = 0x1;
			inputs = {vertex_id};
			return result;
		}

		void emit_unpack_unorm(alys::shader::assembler& a, const std::uint32_t dest, const alys::shader::detail::operand_t& packed)
		{
			a.ushr(asm_::r(dest).xyz(), packed, uint_literals(0, 10, 20, 0));
			a.and_(asm_::r(dest).xyz(), asm_::r(dest).select("xyzx"), uint_literal(1023));
			a.utof(asm_::r(dest).xyz(), asm_::r(dest).select("xyzx"));
			a.mul(asm_::r(dest).xyz(), asm_::r(dest).select("xyzx"), asm_::l(1.0f / 1023.0f));
		}

		// h1 "tab" vertex shaders decode the uv step to the next frame as x + y / 1023 of the normalized step * 2 - 1
		void emit_encode_atlas_step(alys::shader::assembler& a, const std::uint32_t dest, const alys::shader::detail::operand_t& step)
		{
			a.mad(asm_::r(dest).x(), step, asm_::l(0.5f), asm_::l(0.5f));
			a.mov(asm_::r(dest).y(), asm_::l(0.5f));
			a.mad(asm_::r(dest).z(), step, step, asm_::l(-1.0f));
			a.sqrt(asm_::r(dest).z(), asm_::r(dest).z().neg());
			a.mad(asm_::r(dest).z(), asm_::r(dest).z(), asm_::l(0.5f), asm_::l(0.5f));
		}

		// stock "tab" shaders: frame = high half at +16, blend = frac(frame), the next frame steps a column or wraps to the next row or frame 0;
		// the atlas size comes from the record uv size, stock reads it from the material constants
		void emit_atlas_step(alys::shader::assembler& a, const std::uint32_t position, const std::uint32_t u_step, const std::uint32_t v_step,
			const std::uint32_t index, const std::uint32_t first, const std::uint32_t second)
		{
			a.iadd(asm_::r(index).y(), asm_::r(index).x(), uint_literal(16));
			a.ld_raw(asm_::r(index).y(), asm_::r(index).y(), asm_::t(sprite_buffer_slot).select("xxxx"));
			a.ushr(asm_::r(index).y(), asm_::r(index).y(), uint_literal(16));
			a.f16tof32(asm_::r(index).y(), asm_::r(index).y());
			a.frc(asm_::r(position).w(), asm_::r(index).y());
			a.round_ni(asm_::r(index).y(), asm_::r(index).y());
			a.ftou(asm_::r(index).y(), asm_::r(index).y());
			a.iadd(asm_::r(index).y(), asm_::r(index).y(), uint_literal(1));

			// second.zw = 1 / (columns, rows), first = (frame count, columns, rows)
			a.mov(asm_::r(second).z(), asm_::r(second).y().abs());
			a.mov(asm_::r(second).w(), asm_::r(first).y().abs());
			a.rcp(asm_::r(first).zw(), asm_::r(second).select("xyzw"));
			a.round_ne(asm_::r(first).zw(), asm_::r(first).select("xyzw"));
			a.mul(asm_::r(first).y(), asm_::r(first).z(), asm_::r(first).w());
			a.ftou(asm_::r(first).yzw(), asm_::r(first).select("xyzw"));
			a.udiv(asm_::r(second).x(), asm_::r(index).z(), asm_::r(index).y(), asm_::r(first).z());
			a.udiv(asm_::r(second).y(), asm_::r(index).w(), asm_::r(index).y(), asm_::r(first).y());
			a.ieq(asm_::r(index).zw(), asm_::r(index).select("xyzw"), uint_literal(0));
			a.add(asm_::r(first).zw(), asm_::r(second).select("xyzw"), asm_::l(-1.0f));
			a.movc(asm_::r(second).x(), asm_::r(index).z(), asm_::r(first).z(), asm_::r(second).z());
			a.and_(asm_::r(second).y(), asm_::r(index).z(), asm_::r(second).w());
			a.movc(asm_::r(second).y(), asm_::r(index).w(), asm_::r(first).w(), asm_::r(second).y());

			emit_encode_atlas_step(a, u_step, asm_::r(second).select("xxxx").swz());
			emit_encode_atlas_step(a, v_step, asm_::r(second).select("yyyy").swz());
		}

		// 64 byte sprite record of the stock "_dso_xl" vertex shaders: +0 origin, rotation; +16 half second frame size, frame;
		// +20 normal, +24 axis a, +28 axis b (10:10:10 unorm); +32 half uv origin, size, second frame uv; +48 half rg, ba, width height, pivot
		void emit_sprite_fetch(alys::shader::assembler& a, const std::uint32_t temp, const bool atlas_blend)
		{
			const auto position = temp;
			const auto color = temp + 1;
			const auto texcoord = temp + 2;
			const auto normal = temp + 3;
			const auto atlas_step = temp + 4;
			const auto index = temp + 5;
			const auto first = temp + 6;
			const auto second = temp + 7;
			const auto buffer = asm_::t(sprite_buffer_slot);

			a.ushr(asm_::r(index).x(), asm_::v(0).x(), uint_literal(2));
			a.ishl(asm_::r(index).x(), asm_::r(index).x(), uint_literal(6));
			a.and_(asm_::r(index).y(), asm_::v(0).x(), uint_literal(3));
			a.ld_raw(asm_::r(position).select("xyzw"), asm_::r(index).x(), buffer.select("xyzw"));
			a.iadd(asm_::r(index).z(), asm_::r(index).x(), uint_literal(20));
			a.ld_raw(asm_::r(first).xyz(), asm_::r(index).z(), buffer.select("xyzx"));
			emit_unpack_unorm(a, normal, asm_::r(first).select("xxxx").swz());
			emit_unpack_unorm(a, texcoord, asm_::r(first).select("yyyy").swz());
			emit_unpack_unorm(a, color, asm_::r(first).select("zzzz").swz());
			a.mad(asm_::r(texcoord).xyz(), asm_::r(texcoord).select("xyzx"), asm_::l(2.0f), asm_::l(-1.0f));
			a.mad(asm_::r(color).xyz(), asm_::r(color).select("xyzx"), asm_::l(2.0f), asm_::l(-1.0f));

			// rotated axes x = a * cos + b * sin, y = a * sin - b * cos
			a.sincos(asm_::r(first).x(), asm_::r(first).y(), asm_::r(position).w());
			a.mul(asm_::r(second).xyz(), asm_::r(color).select("xyzx"), asm_::r(first).select("xxxx"));
			a.mad(asm_::r(second).xyz(), asm_::r(texcoord).select("xyzx"), asm_::r(first).select("yyyy"), asm_::r(second).select("xyzx"));
			a.mul(asm_::r(color).xyz(), asm_::r(color).select("xyzx"), asm_::r(first).select("yyyy"));
			a.mad(asm_::r(color).xyz(), asm_::r(texcoord).select("xyzx"), asm_::r(first).select("xxxx"), asm_::r(color).select("xyzx").neg());

			// texcoord = (r, b, width, pivot x), first = (g, a, height, pivot y)
			a.iadd(asm_::r(index).z(), asm_::r(index).x(), uint_literal(48));
			a.ld_raw(asm_::r(first).select("xyzw"), asm_::r(index).z(), buffer.select("xyzw"));
			a.and_(asm_::r(texcoord).select("xyzw"), asm_::r(first).select("xyzw"), uint_literal(0xFFFF));
			a.ushr(asm_::r(first).select("xyzw"), asm_::r(first).select("xyzw"), uint_literal(16));
			a.f16tof32(asm_::r(texcoord).select("xyzw"), asm_::r(texcoord).select("xyzw"));
			a.f16tof32(asm_::r(first).select("xyzw"), asm_::r(first).select("xyzw"));
			a.mul(asm_::r(second).xyz(), asm_::r(second).select("xyzx"), asm_::r(texcoord).select("zzzz"));
			a.mul(asm_::r(color).xyz(), asm_::r(color).select("xyzx"), asm_::r(first).select("zzzz"));

			// corners 0 to 3 are (-1, 1), (-1, -1), (1, -1), (1, 1) with uv (0, 1), (0, 0), (1, 0), (1, 1)
			a.ushr(asm_::r(index).z(), asm_::r(index).y(), uint_literal(1));
			a.xor_(asm_::r(index).w(), asm_::r(index).y(), asm_::r(index).z());
			a.and_(asm_::r(index).w(), asm_::r(index).w(), uint_literal(1));
			a.utof(asm_::r(index).zw(), asm_::r(index).select("xyzw"));
			a.mad(asm_::r(index).w(), asm_::r(index).w(), asm_::l(-1.0f), asm_::l(1.0f));
			a.mad(asm_::r(index).y(), asm_::r(index).z(), asm_::l(2.0f), asm_::l(-1.0f));
			a.add(asm_::r(index).y(), asm_::r(index).y(), asm_::r(texcoord).w());
			a.mad(asm_::r(position).xyz(), asm_::r(second).select("xyzx"), asm_::r(index).select("yyyy"), asm_::r(position).select("xyzx"));
			a.mad(asm_::r(index).y(), asm_::r(index).w(), asm_::l(2.0f), asm_::l(-1.0f));
			a.add(asm_::r(index).y(), asm_::r(index).y(), asm_::r(first).w());
			a.mad(asm_::r(position).xyz(), asm_::r(color).select("xyzx"), asm_::r(index).select("yyyy"), asm_::r(position).select("xyzx"));
			a.mov(asm_::r(position).w(), asm_::l(0.0f)); // h1 atlas blend weight
			a.mov(asm_::r(color).xz(), asm_::r(texcoord).select("xxyy"));
			a.mov(asm_::r(color).yw(), asm_::r(first).select("xxyy"));

			a.iadd(asm_::r(index).y(), asm_::r(index).x(), uint_literal(32));
			a.ld_raw(asm_::r(first).xy(), asm_::r(index).y(), buffer.select("xyxx"));
			a.and_(asm_::r(second).xy(), asm_::r(first).select("xyxx"), uint_literal(0xFFFF));
			a.ushr(asm_::r(first).xy(), asm_::r(first).select("xyxx"), uint_literal(16));
			a.f16tof32(asm_::r(second).xy(), asm_::r(second).select("xyxx"));
			a.f16tof32(asm_::r(first).xy(), asm_::r(first).select("xyxx"));
			a.mad(asm_::r(texcoord).x(), asm_::r(index).z(), asm_::r(second).y(), asm_::r(second).x());
			a.mad(asm_::r(texcoord).y(), asm_::r(index).w(), asm_::r(first).y(), asm_::r(first).x());
			if (atlas_blend)
			{
				emit_atlas_step(a, position, normal, atlas_step, index, first, second);
			}
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
			return (slot >= light_grid_volume_slot && slot <= light_grid_probe_visibility_slot) || (slot >= forward_lights_slot && slot <= light_tile_zbins_slot);
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

			a.add(asm_::r(world).xyz(), position, asm_::cb(s2_stable_constant_buffer, lighting.eye_offset_dest).select("xyzx"));
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

			a.ld_structured(asm_::r(volume).x(), asm_::r(volume).x(), uint_literal(0), asm_::t(light_grid_index_slot).select("xxxx"));
			for (auto row = 0u; row < 3; row++)
			{
				a.ld_structured(asm_::r(coords).select("xyzw"), asm_::r(volume).x(), uint_literal(row * 16), asm_::t(light_grid_transform_slot).select("xyzw"));
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
					a.ld_indexable(asm_::r(probe).xyz(), asm_::r(probe).select("xxxx"), asm_::t(light_grid_probe_colors_slot).select("xyzw"));
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
				a.ld_indexable(asm_::r(probe).x(), asm_::r(coords).select("xxxx"), asm_::t(light_grid_probe_visibility_slot).select("xyzw"));
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
			a.dcl_resource_structured(asm_::t(light_grid_volume_slot), 32u);
			switch (patch.grid)
			{
			case light_grid::probe_buffer:
				a.dcl_resource(asm_::t(light_grid_probe_colors_slot), asm_::buffer, float4_type);
				a.dcl_resource(asm_::t(light_grid_probe_visibility_slot), asm_::buffer, float4_type);
				break;
			case light_grid::array_2d:
				a.dcl_resource(asm_::t(light_grid_colors_slot), asm_::texture2darray, float4_type);
				a.dcl_resource(asm_::t(light_grid_visibility_slot), asm_::texture2d, float4_type);
				break;
			case light_grid::volume_3d:
				a.dcl_resource(asm_::t(light_grid_colors_slot), asm_::texture3d, float4_type);
				a.dcl_resource(asm_::t(light_grid_visibility_slot), asm_::texture3d, float4_type);
				break;
			}
		}

		constexpr auto light_temp_count = 15u;
		constexpr auto light_spot_type = 2u;

		bool is_return(const alys::shader::detail::instruction_t& instruction)
		{
			return instruction.opcode.type == D3D10_SB_OPCODE_RET || instruction.opcode.type == D3D10_SB_OPCODE_RETC;
		}

		alys::shader::detail::operand_t as_temp(const alys::shader::detail::operand_t& operand, const std::uint32_t temp)
		{
			alys::shader::detail::operand_t result = asm_::r(temp).select("xyzw");
			result.components = operand.components;
			result.extensions = operand.extensions;
			return result;
		}

		// the s2 forward light buffers are bound to fixed slots, h1 shaders with many layers use them for material textures
		std::map<std::uint32_t, std::uint32_t> relocate_light_textures(const std::vector<alys::shader::detail::instruction_t>& instructions)
		{
			constexpr auto first_free_slot = 26u;

			std::map<std::uint32_t, std::uint32_t> result;
			auto next = first_free_slot;
			for (const auto slot : forward_light_slots)
			{
				if (!declares_resource(instructions, slot))
				{
					continue;
				}

				while (declares_resource(instructions, next))
				{
					next++;
				}

				result.emplace(slot, next++);
			}

			return result;
		}

		bool writes_operand(const alys::shader::detail::instruction_t& instruction, const std::uint32_t type, const std::uint32_t index)
		{
			return !instruction.operands.empty() && instruction.operands[0].type == type && instruction.operands[0].indices[0].value.uint32 == index;
		}

		alys::shader::detail::operand_t negate(alys::shader::detail::operand_t operand)
		{
			alys::shader::detail::operand_extended_t modifier{};
			modifier.type = D3D10_SB_EXTENDED_OPERAND_MODIFIER;
			modifier.modifier = D3D10_SB_OPERAND_MODIFIER_NEG;
			operand.extensions = {modifier};
			return operand;
		}

		void declare_forward_lights(alys::shader::assembler& a, const primary_lights& lights)
		{
			a.dcl_resource_structured(asm_::t(forward_lights_slot), 104u);
			a.dcl_resource(asm_::t(lights.cluster_row ? light_cluster_bits_slot : light_tile_bits_slot), asm_::buffer, uint4_type);
			if (lights.shadows)
			{
				a.dcl_resource_structured(asm_::t(spot_shadow_record_slot), 96u);
				a.dcl_resource(asm_::t(spot_shadow_slot), asm_::texture2darray, float4_type);
				a.dcl_resource(asm_::t(spot_shadow_dynamic_slot), asm_::texture2darray, float4_type);
				a.dcl_sampler(asm_::s(spot_shadow_sampler), D3D10_SB_SAMPLER_MODE_COMPARISON);
			}
		}

		// s2 gives an omni one spot like shadow frustum along its dir, so like the stock loop its strength is scaled by the cone (shadow.w)
		// and the omni is unshadowed outside that frustum
		// the stock lw_fplus spot shadow: shadow index = light flags byte 0 (255 none), strength = hi half of record +68, static / dynamic blend =
		// lo half of +72; the t17 record holds 4 matrix rows for the eye relative position, then the filter scale (xy) and the dynamic and
		// static slices (z, w); 8 comparison taps per map (the stock offsets, without the per pixel rotation), light factor *= 1 - strength * (1 - shadow)
		void emit_spot_shadow(alys::shader::assembler& a, const std::uint32_t acc, const alys::shader::detail::operand_t& position)
		{
			constexpr std::array<std::pair<float, float>, 8> taps
			{{
				{-0.7071f, 0.7071f}, {0.0f, -0.9308f}, {0.5997f, 0.5997f}, {-0.7516f, 0.0f},
				{0.4531f, -0.4531f}, {0.0f, 0.5148f}, {-0.2633f, -0.2633f}, {0.21f, 0.0f},
			}};
			const auto out = acc + 1;
			const auto fade = acc + 5;
			const auto light = acc + 8;
			const auto dir = acc + 9;
			const auto record = acc + 12;
			const auto shadow = acc + 13;
			const auto address = acc + 14;
			const auto records = asm_::t(spot_shadow_record_slot);

			a.ld_structured(asm_::r(record).xy(), asm_::r(light).x(), uint_literal(68), asm_::t(forward_lights_slot).select("xyxx"));
			a.ushr(asm_::r(shadow).y(), asm_::r(record).x(), uint_literal(16));
			a.and_(asm_::r(shadow).z(), asm_::r(record).y(), uint_literal(0xFFFF));
			a.f16tof32(asm_::r(shadow).yz(), asm_::r(shadow).select("xyzx"));
			a.mul(asm_::r(record).w(), asm_::r(shadow).y(), asm_::r(shadow).w());
			a.movc(asm_::r(shadow).y(), asm_::r(light).z(), asm_::r(shadow).y(), asm_::r(record).w());
			a.ine(asm_::r(shadow).w(), asm_::r(shadow).x(), uint_literal(255));
			a.lt(asm_::r(record).z(), asm_::l(0.0f), asm_::r(shadow).y());
			a.and_(asm_::r(shadow).w(), asm_::r(shadow).w(), asm_::r(record).z());
			a.if_nz(asm_::r(shadow).w());

			// fade.xyz = shadow map uv and depth
			for (auto row = 0u; row < 4; row++)
			{
				a.ld_structured(asm_::r(record).select("xyzw"), asm_::r(shadow).x(), uint_literal(row * 16), records.select("xyzw"));
				const auto dest = row < 3 ? asm_::r(fade).select(std::string(1, "xyz"[row])) : asm_::r(out).x();
				a.dp3(dest, asm_::r(record).select("xyzx"), position);
				a.add(dest, dest, asm_::r(record).select("wwww").swz());
			}

			a.rcp(asm_::r(out).x(), asm_::r(out).x());
			a.mul(asm_::r(fade).xyz(), asm_::r(fade).select("xyzx"), asm_::r(out).select("xxxx"));
			a.ld_structured(asm_::r(record).select("xyzw"), asm_::r(shadow).x(), uint_literal(64), records.select("xyzw"));

			const auto sum_taps = [&](const std::uint32_t texture, const char* slice, const char* sum)
			{
				a.round_z(asm_::r(address).z(), asm_::r(record).select(std::string(4, slice[0])).swz());
				a.mov(asm_::r(out).select(sum), asm_::l(0.0f));
				for (const auto& [x, y] : taps)
				{
					a.mad(asm_::r(address).xy(), asm_::r(record).select("xyxx"), asm_::l(x, y, 0.0f, 0.0f), asm_::r(fade).select("xyxx"));
					a.sample_c_lz_indexable(asm_::r(address).w(), asm_::r(address).select("xyzx"), asm_::t(texture).select("xxxx"),
						asm_::s(spot_shadow_sampler), asm_::r(fade).z());
					a.add(asm_::r(out).select(sum), asm_::r(out).select(std::string(4, sum[0])).swz(), asm_::r(address).select("wwww").swz());
				}

				a.mul(asm_::r(out).select(sum), asm_::r(out).select(std::string(4, sum[0])).swz(), asm_::l(0.125f));
			};

			// out.y = static, out.z = dynamic, blended as static * blend + dynamic * (1 - blend) like the stock loop
			// shadow.w is free once the outer if took it, record.z is the dynamic slice
			a.mov(asm_::r(out).y(), asm_::l(0.0f));
			a.ge(asm_::r(shadow).w(), asm_::r(shadow).z(), asm_::l(0.0f));
			a.if_nz(asm_::r(shadow).w());
			sum_taps(spot_shadow_slot, "w", "y");
			a.endif();
			a.lt(asm_::r(shadow).w(), asm_::r(shadow).z(), asm_::l(1.0f));
			a.if_nz(asm_::r(shadow).w());
			sum_taps(spot_shadow_dynamic_slot, "z", "z");
			a.add(asm_::r(out).w(), asm_::r(out).y(), negate(asm_::r(out).select("zzzz").swz()));
			a.mad(asm_::r(out).y(), asm_::r(shadow).z(), asm_::r(out).w(), asm_::r(out).z());
			a.endif();

			a.add(asm_::r(out).y(), asm_::r(out).y(), asm_::l(-1.0f));
			a.mad(asm_::r(out).y(), asm_::r(shadow).y(), asm_::r(out).y(), asm_::l(1.0f));
			a.mul(asm_::r(dir).w(), asm_::r(dir).w(), asm_::r(out).y());
			a.endif();
		}

		// the screen position components the light loop reads, the cluster loop adds the view depth w
		std::uint8_t light_screen_mask(const primary_lights& lights)
		{
			return lights.cluster_row ? 0xB : 0x3;
		}

		// the stock lw_fplus_*_bmtile tile loop without the per pixel z bin mask, so every pixel of a quad runs the same lights and keeps its derivatives;
		// temps acc .. acc + 14: sum, light body output, the five h1 light constants, tile, light, dir, color, spot, three shadow temps
		void emit_forward_light_loop_begin(alys::shader::assembler& a, const primary_lights& lights, const std::uint32_t acc, const std::uint32_t screen_input,
			const alys::shader::detail::operand_t& position)
		{
			const auto out = acc + 1;
			const auto light_position = acc + 2;
			const auto diffuse = acc + 3;
			const auto physical_size = acc + 4;
			const auto fade = acc + 5;
			const auto specular = acc + 6;
			const auto tile = acc + 7;
			const auto light = acc + 8;
			const auto dir = acc + 9;
			const auto color = acc + 10;
			const auto spot = acc + 11;
			const auto stable = s2_stable_constant_buffer;
			const auto rt_size = asm_::cb(stable, lights.render_target_size_row);
			const auto tile_parms = asm_::cb(stable, lights.tile_parms_row);
			const auto tile_words = tile_parms.select("xxxx").swz();
			const auto bits_slot = lights.cluster_row ? light_cluster_bits_slot : light_tile_bits_slot;

			a.mov(asm_::r(acc).select("xyzw"), asm_::l(0.0f, 0.0f, 0.0f, 0.0f));
			a.mov(asm_::r(out).select("xyzw"), asm_::l(0.0f, 0.0f, 0.0f, 0.0f));

			// tile = (index, word, bits, first light of the word)
			if (lights.cluster_row)
			{
				// the stock lw_fplus_*_bmclust froxel: screen cell light.xy of the cell grid light.zw, view depth slice dir.x of 32
				const auto cells = asm_::cb(stable, *lights.cluster_row);
				const auto slice_starts = asm_::cb(stable, *lights.cluster_row + 1);
				const auto slice_scales = asm_::cb(stable, *lights.cluster_row + 2);
				a.mul(asm_::r(light).xy(), asm_::v(screen_input).select("xyxx"), rt_size.select("zwzz"));
				a.mul(asm_::r(light).xy(), asm_::r(light).select("xyxx"), rt_size.select("xyxx"));
				a.mul(asm_::r(light).xy(), asm_::r(light).select("xyxx"), cells.select("zwzz"));
				a.add(asm_::r(light).zw(), tile_parms.select("xxzw"), cells.select("xxxy"));
				a.add(asm_::r(light).zw(), asm_::r(light).select("xxzw"), asm_::l(0.0f, 0.0f, -1.0f, -1.0f));
				a.mul(asm_::r(light).zw(), asm_::r(light).select("xxzw"), cells.select("xxzw"));
				a.ftou(asm_::r(light).select("xyzw"), asm_::r(light).select("xyzw"));
				a.add(asm_::r(dir).x(), asm_::v(screen_input).select("wwww").swz(), asm_::l(-4.0f));
				a.add(asm_::r(dir).y(), asm_::r(dir).x(), negate(slice_starts.select("xxxx").swz()));
				a.add(asm_::r(dir).z(), asm_::r(dir).y(), negate(slice_starts.select("yyyy").swz()));
				a.add(asm_::r(dir).w(), asm_::r(dir).z(), negate(slice_starts.select("zzzz").swz()));
				a.mul_sat(asm_::r(dir).select("xyzw"), asm_::r(dir).select("xyzw"), slice_scales.select("xyzw"));
				a.dp4(asm_::r(dir).x(), asm_::r(dir).select("xyzw"), asm_::l(0.25f, 0.25f, 0.25f, 0.25f));
				a.mul(asm_::r(dir).x(), asm_::r(dir).x(), asm_::l(32.0f));
				a.ftou(asm_::r(dir).x(), asm_::r(dir).x());
				a.umin(asm_::r(dir).x(), asm_::r(dir).x(), uint_literal(31));
				a.imad(asm_::r(tile).z(), asm_::r(light).w(), asm_::r(light).z(), uint_literal(0));
				a.imad(asm_::r(tile).y(), asm_::r(light).z(), asm_::r(light).y(), asm_::r(light).x());
				a.imad(asm_::r(tile).x(), asm_::r(dir).x(), asm_::r(tile).z(), asm_::r(tile).y());
			}
			else
			{
				a.mul(asm_::r(tile).xy(), asm_::v(screen_input).select("xyxx"), rt_size.select("zwzz"));
				a.mul(asm_::r(tile).xy(), asm_::r(tile).select("xyxx"), rt_size.select("xyxx"));
				a.ftou(asm_::r(tile).xy(), asm_::r(tile).select("xyxx"));
				a.ushr(asm_::r(tile).xy(), asm_::r(tile).select("xyxx"), uint_literals(3, 3, 0, 0));
				a.add(asm_::r(tile).z(), rt_size.select("xxxx").swz(), asm_::l(7.0f));
				a.mul(asm_::r(tile).z(), asm_::r(tile).z(), asm_::l(0.125f));
				a.ftou(asm_::r(tile).z(), asm_::r(tile).z());
				a.imad(asm_::r(tile).x(), asm_::r(tile).y(), asm_::r(tile).z(), asm_::r(tile).x());
			}

			a.mov(asm_::r(tile).y(), uint_literal(0));
			a.loop();
			a.uge(asm_::r(tile).z(), asm_::r(tile).y(), tile_words);
			a.breakc_nz(asm_::r(tile).z());
			a.imad(asm_::r(tile).z(), asm_::r(tile).x(), tile_words, asm_::r(tile).y());
			a.ld_indexable(asm_::r(tile).z(), asm_::r(tile).select("zzzz"), asm_::t(bits_slot).select("xxxx"));
			a.ishl(asm_::r(tile).w(), asm_::r(tile).y(), uint_literal(5));
			a.loop();
			a.breakc_z(asm_::r(tile).z());
			a.firstbit_lo(asm_::r(light).x(), asm_::r(tile).z());
			a.ishl(asm_::r(light).y(), uint_literal(1), asm_::r(light).x());
			a.xor_(asm_::r(tile).z(), asm_::r(tile).z(), asm_::r(light).y());
			a.iadd(asm_::r(light).x(), asm_::r(light).x(), asm_::r(tile).w());

			const auto lights_buffer = asm_::t(forward_lights_slot);
			a.ld_structured(asm_::r(light_position).select("xyzw"), asm_::r(light).x(), uint_literal(0), lights_buffer.select("xyzw"));
			a.ld_structured(asm_::r(dir).select("xyzw"), asm_::r(light).x(), uint_literal(16), lights_buffer.select("xyzw"));
			a.ld_structured(asm_::r(color).select("xyzw"), asm_::r(light).x(), uint_literal(32), lights_buffer.select("xyzw"));
			a.ld_structured(asm_::r(physical_size).select("xyzw"), asm_::r(light).x(), uint_literal(48), lights_buffer.select("yzwx"));
			a.ld_structured(asm_::r(spot).xy(), asm_::r(light).x(), uint_literal(64), lights_buffer.select("xyxx"));
			if (lights.shadows)
			{
				a.and_(asm_::r(acc + 13).x(), asm_::r(dir).w(), uint_literal(255));
			}

			// light.z = spot, spot = (scale, exponent, falloff scale, falloff bias), light.w = spot bias
			a.ubfe(asm_::r(light).z(), uint_literal(2), uint_literal(8), asm_::r(dir).w());
			a.ieq(asm_::r(light).z(), asm_::r(light).z(), uint_literal(light_spot_type));
			a.ushr(asm_::r(light).w(), asm_::r(spot).x(), uint_literal(16));
			a.and_(asm_::r(spot).xy(), asm_::r(spot).select("xyxx"), uint_literal(0xFFFF));
			a.and_(asm_::r(spot).z(), asm_::r(color).w(), uint_literal(0xFFFF));
			a.ushr(asm_::r(spot).w(), asm_::r(color).w(), uint_literal(16));
			a.f16tof32(asm_::r(spot).select("xyzw"), asm_::r(spot).select("xyzw"));
			a.f16tof32(asm_::r(light).w(), asm_::r(light).w());

			// spot factor = cone^exponent * saturate(plane * falloff scale + bias)^2, the h1 omni body then skips its distance falloff
			a.add(asm_::r(fade).xyz(), asm_::r(light_position).select("xyzx"), negate(position));
			a.dp3(asm_::r(dir).w(), asm_::r(fade).select("xyzx"), asm_::r(dir).select("xyzx"));
			a.add(asm_::r(dir).w(), asm_::r(dir).w(), asm_::r(light_position).w());
			a.mad_sat(asm_::r(dir).w(), asm_::r(dir).w(), asm_::r(spot).z(), asm_::r(spot).w());
			a.mul(asm_::r(dir).w(), asm_::r(dir).w(), asm_::r(dir).w());
			a.dp3(asm_::r(light).y(), asm_::r(fade).select("xyzx"), asm_::r(fade).select("xyzx"));
			a.mov(asm_::r(out).x(), asm_::r(light).y());
			a.rsq(asm_::r(light).y(), asm_::r(light).y());
			a.mul(asm_::r(fade).xyz(), asm_::r(fade).select("xyzx"), asm_::r(light).select("yyyy"));

			// out.y = saturate((radius - distance) / (radius * 0.05)), the h1 falloff tail ends exactly at the radius the tiles are culled with
			a.mul(asm_::r(out).x(), asm_::r(out).x(), asm_::r(light).y());
			a.mul(asm_::r(out).z(), asm_::r(light_position).w(), asm_::l(20.0f));
			a.mad_sat(asm_::r(out).y(), asm_::r(out).x().neg(), asm_::r(out).z(), asm_::l(20.0f));

			a.dp3(asm_::r(light).y(), asm_::r(fade).select("xyzx"), asm_::r(dir).select("xyzx"));
			a.mad_sat(asm_::r(light).y(), asm_::r(light).y(), asm_::r(spot).x(), asm_::r(light).w());
			if (lights.shadows)
			{
				a.mov(asm_::r(acc + 13).w(), asm_::r(light).y()); // the cone before the exponent fades omni shadows
			}
			a.ge(asm_::r(light).w(), asm_::l(0.0f), asm_::r(light).y());
			a.log(asm_::r(light).y(), asm_::r(light).y());
			a.mul(asm_::r(light).y(), asm_::r(light).y(), asm_::r(spot).y());
			a.exp(asm_::r(light).y(), asm_::r(light).y());
			a.movc(asm_::r(light).y(), asm_::r(light).w(), asm_::l(0.0f), asm_::r(light).y());
			a.mul(asm_::r(dir).w(), asm_::r(dir).w(), asm_::r(light).y());
			a.movc(asm_::r(dir).w(), asm_::r(light).z(), asm_::r(dir).w(), asm_::l(1.0f));
			a.mul(asm_::r(dir).w(), asm_::r(dir).w(), asm_::r(out).y());
			if (lights.shadows)
			{
				emit_spot_shadow(a, acc, position);
			}

			a.mul(asm_::r(diffuse).xyz(), asm_::r(color).select("xyzx"), asm_::r(dir).select("wwww"));
			a.mov(asm_::r(diffuse).w(), asm_::l(0.0f));
			a.mov(asm_::r(specular).select("xyzw"), asm_::r(diffuse).select("xyzw"));
			a.movc(asm_::r(fade).xy(), asm_::r(light).select("zzzz"), asm_::l(0.0f, -1.0f, 0.0f, 0.0f), asm_::r(spot).select("zwzz"));
			a.mov(asm_::r(fade).zw(), asm_::l(0.0f, 0.0f, 0.0f, 0.0f));
		}

		// writes o0 = lit + lights, or the lights alone for the separate light pass
		void emit_forward_light_loop_end(alys::shader::assembler& a, const std::uint32_t acc, const std::optional<std::uint32_t> lit)
		{
			const auto out = acc + 1;
			const auto tile = acc + 7;

			// the h1 body divides by zero at the center of a highlight (rsq of a zero vector times the bulb radius); max / min return the
			// other operand for NaN, so clamping like the stock lit shaders drops that light for the pixel instead of the whole frame
			a.max(asm_::r(out).xyz(), asm_::r(out).select("xyzx"), asm_::l(0.0f));
			a.min(asm_::r(out).xyz(), asm_::r(out).select("xyzx"), asm_::l(65504.0f));
			a.add(asm_::r(acc).xyz(), asm_::r(acc).select("xyzx"), asm_::r(out).select("xyzx"));
			a.mov(asm_::r(acc).w(), asm_::r(out).w());
			a.endloop();
			a.iadd(asm_::r(tile).y(), asm_::r(tile).y(), uint_literal(1));
			a.endloop();
			if (lit)
			{
				a.add(asm_::r(*lit).xyz(), asm_::r(*lit).select("xyzx"), asm_::r(acc).select("xyzx"));
				a.mov(asm_::o(0).select("xyzw"), asm_::r(*lit).select("xyzw"));
			}
			else
			{
				a.mov(asm_::o(0).select("xyzw"), asm_::r(acc).select("xyzw"));
			}

			a.ret();
		}

		bool is_input_ps_declaration(const alys::shader::detail::instruction_t& instruction)
		{
			return instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_PS || instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_PS_SIV
				|| instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_PS_SGV;
		}

		bool is_resource_declaration(const alys::shader::detail::instruction_t& instruction)
		{
			return is_declaration(instruction) && !instruction.operands.empty()
				&& (instruction.operands[0].type == D3D10_SB_OPERAND_TYPE_RESOURCE || instruction.operands[0].type == D3D10_SB_OPERAND_TYPE_SAMPLER);
		}

		// throws unless the pixel shader returns only at its end
		void check_single_return(const std::vector<alys::shader::detail::instruction_t>& instructions, const char* what)
		{
			if (instructions.empty() || instructions.back().opcode.type != D3D10_SB_OPCODE_RET
				|| std::any_of(instructions.begin(), instructions.end() - 1, is_return))
			{
				throw std::runtime_error(utils::string::va("%s pixel shader returns early", what));
			}
		}

		void check_writes_only_o0(alys::shader::shader_object& shader, const char* what)
		{
			const auto outputs = shader.get_signatures().find(alys::shader::chunk_osgn);
			if (outputs == shader.get_signatures().end() || outputs->second.size() != 1 || outputs->second[0].register_ != 0)
			{
				throw std::runtime_error(utils::string::va("%s pixel shader writes more than o0", what));
			}
		}

		void relocate_resource_slots(alys::shader::detail::instruction_t& instruction, const std::map<std::uint32_t, std::uint32_t>& slots)
		{
			for (auto& operand : instruction.operands)
			{
				const auto slot = operand.type == D3D10_SB_OPERAND_TYPE_RESOURCE ? slots.find(operand.indices[0].value.uint32) : slots.end();
				if (slot != slots.end())
				{
					operand.indices[0].value.uint32 = slot->second;
				}
			}
		}

		// the h1 light body reads the light pass inputs, constants and textures at the slots of the lit pass
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

				// the forward light buffers take fixed slots, multi layer lit shaders move the textures there
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

				check_writes_only_o0(this->lit_, "lit");

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
				this->assembler_.dcl_temps(lit_out + 1 + static_cast<std::uint32_t>(this->gathered_.size()));
				for (auto i = lit_code; i + 1 < static_cast<std::ptrdiff_t>(lit.size()); i++)
				{
					auto instruction = lit[i];
					for (auto& operand : instruction.operands)
					{
						if (operand.type == D3D10_SB_OPERAND_TYPE_OUTPUT)
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
				bool one{}; // the h1 light fog transmittance, s2 has no fog there
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
			std::map<std::uint32_t, std::array<lane, 4>> lanes_; // light input register components -> lit
			std::map<std::uint32_t, std::uint32_t> gathered_; // light input registers whose components come from several lit registers -> temp
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
						const auto used = static_cast<std::uint8_t>(element.register_ == reg ? declared & element.mask : 0);
						if (!used)
						{
							continue;
						}

						if (element.name == "TEXCOORD" && element.semantic_index == 25)
						{
							for (auto c = 0; c < 4; c++)
							{
								lanes[c] = (used >> c) & 1 ? lane{true, true} : lanes[c];
							}
							continue;
						}

						// h1 model lit shaders (lpo_*) have no TEXCOORD0, the uv rides in the w of TEXCOORD1 (u) and TEXCOORD5 (v)
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

						if (element.name == "TEXCOORD" && element.semantic_index == 5)
						{
							const auto first = static_cast<std::uint32_t>(std::countr_zero(moved));
							// a string swizzle and swz(): converted straight to an operand the selection stays a write mask, which reads as one
							// replicated component, and the numeric select() overload does not build xyz either
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

			// a light input read as one operand must come from one lit register; one assembled from several (the TEXCOORD0 uv of the lpo
			// shaders) gets a temp filled before the light loop
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

						split |= first && (first->one != entry.one || first->reg != entry.reg);
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
						if (lanes[c].one)
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
					if (!entry.valid || (first && (first->one != entry.one || first->reg != entry.reg)))
					{
						throw std::runtime_error("light pixel shader mixes inputs that the lit shader packs apart");
					}

					first = first ? first : std::optional(entry);
				}

				if (first->one)
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

				this->lit_instance_rows_ = this->vs_lighting_ && patch.lighting->instanced;
				this->base_temps_ = this->lightmap_.has_value() || patch.sun_shadow.texture.has_value() ? 1u : 0u;
				const auto lighting_temps = this->vs_lighting_ ? 3u : this->pixel_lighting_ ? 6u : 0u;
				this->sprite_temp_ = this->base_temps_ + lighting_temps;
				this->temp_count_ = this->sprite_temp_ + (this->sprite_inputs_ ? sprite_temp_count : 0u);

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
					if (this->is_lighting_coords_buffer(instruction) || (this->sprite_inputs_ && is_input_declaration(instruction)))
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
					this->redirect_sprite_inputs(i, instruction);
					if (this->patch_model_lighting(instruction))
					{
						continue;
					}

					this->patch_sun_shadow(i, instruction);
					relocate_resource_slots(instruction, this->texture_slots_);
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
			std::optional<std::vector<std::pair<std::uint32_t, sprite_input>>> sprite_inputs_;
			std::uint32_t sprite_temp_{};
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
					return element.name == "TEXCOORD" && element.semantic_index == 5 && (element.rw_mask & 0x7) == 0x7;
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
					this->temp_ = *this->first_temp_;
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

				if (this->lightmap_)
				{
					this->assembler_.mov(asm_::r(*this->temp_).xy(), asm_::v(this->lightmap_->texcoord).xyxx());
					this->assembler_.mov(asm_::r(*this->temp_).zw(), asm_::v(this->lightmap_->lightmap).xxxy());
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
}
