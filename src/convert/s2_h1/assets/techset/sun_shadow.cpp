#include <std_include.hpp>

#include "../../include.hpp"
#include "../techset.hpp"
#include "sun_shadow.hpp"

namespace zonetool::s2::converter::h1::techset
{
	namespace
	{
		using alys::shader::detail::instruction_t;
		using alys::shader::detail::operand_t;

		constexpr auto source_shader = "lmo_sm_lmpb_l0d_l0sd_b0c0nl0sl0b1c1v1nl1sl1_nofog_vcl.hlsl";

		constexpr auto source_object_row = 4u;
		constexpr auto source_stable_row = 10u;
		constexpr std::uint32_t source_shadow_texture = 7u;
		constexpr std::uint32_t source_translucent_texture = 8u;
		constexpr std::uint32_t source_compare_sampler = 6u;
		constexpr std::uint32_t source_linear_sampler = 0u;

		constexpr std::uint32_t source_visibility = 2u * 4 + 3;
		constexpr std::uint32_t source_blend = 5u * 4 + 1;

		struct source_block
		{
			instruction_list code;
			std::vector<instruction_t> declarations;
			std::uint32_t position_input;
			std::uint32_t screen_input;
			std::set<std::uint32_t> temps;
		};

		bool reads_row(const instruction_t& instruction, const std::uint32_t buffer, const std::uint32_t row)
		{
			return std::ranges::any_of(instruction.operands, [&](const operand_t& operand)
			{
				return operand.type == D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER && operand.indices[0].value.uint32 == buffer
					&& operand.indices[1].value.uint32 == row;
			});
		}

		bool reads_input(const instruction_t& instruction, const std::uint32_t input)
		{
			return std::ranges::any_of(instruction.operands, [&](const operand_t& operand)
			{
				return operand.type == D3D10_SB_OPERAND_TYPE_INPUT && operand.indices[0].value.uint32 == input;
			});
		}

		std::uint32_t find_input(alys::shader::shader_object& shader, const char* name, const std::uint32_t index)
		{
			return dxbc::find_signature_element(shader, alys::shader::chunk_isgn, [&](const dxbc::signature_element& element)
			{
				return element.name == name && element.semantic_index == index;
			}, name).register_;
		}

		void collect_temps(const operand_t& operand, std::set<std::uint32_t>& temps)
		{
			for (const auto& index : operand.indices)
			{
				if (index.extra_operand)
				{
					collect_temps(*index.extra_operand, temps);
				}
			}

			if (operand.type == D3D10_SB_OPERAND_TYPE_TEMP)
			{
				temps.insert(operand.indices[0].value.uint32);
			}
		}

		void add_reads(const operand_t& operand, std::set<std::uint32_t>& reads)
		{
			for (const auto& index : operand.indices)
			{
				if (index.extra_operand)
				{
					add_reads(*index.extra_operand, reads);
				}
			}

			if (operand.type != D3D10_SB_OPERAND_TYPE_TEMP || operand.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE)
			{
				return;
			}

			const auto count = operand.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE ? 4 : 1;
			for (auto i = 0; i < count; i++)
			{
				reads.insert(operand.indices[0].value.uint32 * 4 + operand.components.names[i]);
			}
		}

		void check_inputs(const instruction_list& code)
		{
			std::set<std::uint32_t> written;
			for (const auto& instruction : code)
			{
				std::set<std::uint32_t> reads;
				for (const auto& operand : instruction.operands)
				{
					add_reads(operand, reads);
				}

				for (const auto component : reads)
				{
					if (!written.contains(component))
					{
						throw std::runtime_error(utils::string::va("h1 sun shadow block reads r%u.%c before writing it", component / 4, "xyzw"[component % 4]));
					}
				}

				for (const auto& operand : instruction.operands)
				{
					if (operand.type == D3D10_SB_OPERAND_TYPE_TEMP && operand.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE)
					{
						for (auto component = 0u; component < 4; component++)
						{
							if (operand.components.mask & (1u << component))
							{
								written.insert(operand.indices[0].value.uint32 * 4 + component);
							}
						}
					}
				}
			}
		}

		source_block load_source_block()
		{
			const auto program = load_h1_program(zonetool::h1::ASSET_TYPE_PIXELSHADER, source_shader);
			if (program.empty())
			{
				throw std::runtime_error(utils::string::va("h1 shader \"%s\" is not in the h1 zones", source_shader));
			}

			auto shader = alys::shader::shader_object::parse(program);
			const auto& instructions = shader.get_instructions();
			source_block result{};
			result.position_input = find_input(shader, "TEXCOORD", 5);
			result.screen_input = find_input(shader, "SV_POSITION", 0);

			const auto find = [&](const std::size_t from, const auto& accept) -> std::size_t
			{
				for (auto i = from; i < instructions.size(); i++)
				{
					if (accept(instructions[i]))
					{
						return i;
					}
				}

				throw std::runtime_error(utils::string::va("h1 shader \"%s\" changed: the sun shadow block is not found", source_shader));
			};

			const auto lookup = find(0, [&](const instruction_t& instruction)
			{
				return instruction.opcode.type == D3D10_SB_OPCODE_DP4 && reads_row(instruction, 1, source_object_row);
			});

			const auto noise = find(lookup, [&](const instruction_t& instruction)
			{
				return instruction.opcode.type == D3D10_SB_OPCODE_DP2 && reads_input(instruction, result.screen_input);
			});

			const auto rotation = find(noise, [](const instruction_t& instruction)
			{
				return instruction.opcode.type == D3D10_SB_OPCODE_SINCOS;
			});

			const auto discard = find(rotation, [](const instruction_t& instruction)
			{
				return instruction.opcode.type == D3D10_SB_OPCODE_DISCARD;
			});

			const auto end = discard - 2;
			auto depth = 0;
			for (auto i = rotation + 1; i <= end; i++)
			{
				depth += instructions[i].opcode.type == D3D10_SB_OPCODE_IF;
				depth -= instructions[i].opcode.type == D3D10_SB_OPCODE_ENDIF;
			}

			const auto valid = lookup >= 2 && instructions[lookup - 2].opcode.type == D3D10_SB_OPCODE_MOV && reads_input(instructions[lookup - 2], result.position_input)
				&& reads_row(instructions[lookup + 1], 1, source_object_row + 1) && reads_row(instructions[lookup + 2], 1, source_object_row + 2)
				&& rotation - noise == 5 && instructions[end].opcode.type == D3D10_SB_OPCODE_ENDIF && depth == 0;
			if (!valid)
			{
				throw std::runtime_error(utils::string::va("h1 shader \"%s\" changed: the sun shadow block does not match", source_shader));
			}

			for (const auto& range : {std::pair{lookup - 2, lookup + 3}, std::pair{noise, rotation + 1}, std::pair{rotation + 1, end + 1}})
			{
				result.code.insert(result.code.end(), instructions.begin() + range.first, instructions.begin() + range.second);
			}

			for (const auto& instruction : result.code)
			{
				for (const auto& operand : instruction.operands)
				{
					collect_temps(operand, result.temps);
				}
			}

			check_inputs(result.code);

			for (const auto& instruction : instructions)
			{
				if (!dxbc::is_declaration(instruction) || instruction.operands.empty())
				{
					continue;
				}

				const auto& operand = instruction.operands[0];
				const auto slot = operand.indices[0].value.uint32;
				const auto resource = operand.type == D3D10_SB_OPERAND_TYPE_RESOURCE && (slot == source_shadow_texture || slot == source_translucent_texture);
				const auto sampler = operand.type == D3D10_SB_OPERAND_TYPE_SAMPLER && (slot == source_compare_sampler || slot == source_linear_sampler);
				if (resource || sampler)
				{
					result.declarations.emplace_back(instruction);
				}
			}

			if (result.declarations.size() != 4)
			{
				throw std::runtime_error(utils::string::va("h1 shader \"%s\" changed: the sun shadow resources are not declared", source_shader));
			}

			return result;
		}

		const source_block& get_source_block()
		{
			static const auto block = load_source_block();
			return block;
		}

		std::uint32_t free_slot(const instruction_list& instructions, const std::uint32_t type, std::set<std::uint32_t>& taken)
		{
			for (const auto& instruction : instructions)
			{
				if (dxbc::is_declaration(instruction) && !instruction.operands.empty() && instruction.operands[0].type == type)
				{
					taken.insert(instruction.operands[0].indices[0].value.uint32);
				}
			}

			auto slot = 0u;
			while (taken.contains(slot))
			{
				slot++;
			}

			taken.insert(slot);
			return slot;
		}

		struct block_remap
		{
			std::map<std::uint32_t, std::uint32_t> temps;
			std::map<std::uint32_t, std::uint32_t> inputs;
			std::map<std::uint32_t, std::uint32_t> textures;
			std::map<std::uint32_t, std::uint32_t> samplers;
			std::uint32_t object_row;
			std::uint32_t stable_row;
		};

		void remap_operand(operand_t& operand, const block_remap& remap)
		{
			for (auto& index : operand.indices)
			{
				if (index.extra_operand)
				{
					remap_operand(*index.extra_operand, remap);
				}
			}

			auto& slot = operand.indices[0].value.uint32;
			const auto remap_slot = [&](const std::map<std::uint32_t, std::uint32_t>& slots, const char* what)
			{
				const auto entry = slots.find(slot);
				if (entry == slots.end())
				{
					throw std::runtime_error(utils::string::va("h1 sun shadow block reads %s %u", what, slot));
				}

				slot = entry->second;
			};

			switch (operand.type)
			{
			case D3D10_SB_OPERAND_TYPE_TEMP:
				remap_slot(remap.temps, "temp");
				break;
			case D3D10_SB_OPERAND_TYPE_INPUT:
				remap_slot(remap.inputs, "input");
				break;
			case D3D10_SB_OPERAND_TYPE_RESOURCE:
				remap_slot(remap.textures, "texture");
				break;
			case D3D10_SB_OPERAND_TYPE_SAMPLER:
				remap_slot(remap.samplers, "sampler");
				break;
			case D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER:
			{
				auto& row = operand.indices[1].value.uint32;
				if (operand.indices[1].representation != D3D10_SB_OPERAND_INDEX_IMMEDIATE32)
				{
					throw std::runtime_error("h1 sun shadow block indexes a constant buffer");
				}

				if (slot == 1 && row >= source_object_row && row < source_object_row + sun_shadow_object_rows)
				{
					row = remap.object_row + row - source_object_row;
				}
				else if (slot == 2 && row >= source_stable_row && row < source_stable_row + sun_shadow_stable_rows)
				{
					row = remap.stable_row + row - source_stable_row;
				}
				else
				{
					throw std::runtime_error(utils::string::va("h1 sun shadow block reads cb%u[%u]", slot, row));
				}
				break;
			}
			default:
				break;
			}
		}

		instruction_t make_mov(const operand_t& dest, const operand_t& source)
		{
			instruction_t instruction{};
			instruction.opcode = alys::shader::detail::create_opcode(D3D10_SB_OPCODE_MOV);
			instruction.operands = {dest, source};
			return instruction;
		}

		void grow_constant_buffer(instruction_list& instructions, const std::uint32_t slot, const std::uint32_t rows)
		{
			const auto existing = std::ranges::find_if(instructions, [&](const instruction_t& instruction)
			{
				return instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER && instruction.operands[0].indices[0].value.uint32 == slot;
			});

			if (existing != instructions.end())
			{
				auto& size = existing->operands[0].indices[1].value.uint32;
				size = std::max(size, rows);
				return;
			}

			const auto first = std::ranges::find_if(instructions, [](const instruction_t& instruction)
			{
				return instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER;
			});

			if (first == instructions.end())
			{
				throw std::runtime_error("sun shadow shader has no constant buffer");
			}

			auto declaration = *first;
			declaration.operands[0].indices[0].value.uint32 = slot;
			declaration.operands[0].indices[1].value.uint32 = rows;
			instructions.insert(first, declaration);
		}
	}

	const std::vector<std::pair<std::uint32_t, std::int32_t>>& sun_shadow_stable_consts()
	{
		static const std::vector<std::pair<std::uint32_t, std::int32_t>> consts
		{
			{0, zonetool::h1::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_0},
			{1, zonetool::h1::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_1},
			{2, zonetool::h1::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_2},
			{3, zonetool::h1::CONST_SRC_CODE_SUN_SHADOWMAP_CASCADE_V_CLAMP},
			{6, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION_ARRAY_0},
			{7, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION_ARRAY_1},
			{8, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_DISTANCE_BIAS},
			{9, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_SCALE},
			{10, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_PARTITION_UV_OFFSET},
			{11, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_CASCADE_MASK},
			{12, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_DISTANCE_BLEND},
		};

		return consts;
	}

	sun_shadow_slots patch_sun_shadow(alys::shader::shader_object& shader, instruction_list& instructions, const sun_shadow_request& request)
	{
		const auto& block = get_source_block();
		const auto sample = std::ranges::find_if(instructions, [&](const instruction_t& instruction)
		{
			return dxbc::is_sample(instruction, request.mask_texture) || (instruction.opcode.type == D3D10_SB_OPCODE_SAMPLE_L
				&& dxbc::is_resource(instruction.operands[2], request.mask_texture));
		});

		if (sample == instructions.end())
		{
			throw std::runtime_error(utils::string::va("sun shadow mask t%u is not sampled", request.mask_texture));
		}

		const auto uses = std::ranges::count_if(instructions, [&](const instruction_t& instruction)
		{
			return !dxbc::is_declaration(instruction) && std::ranges::any_of(instruction.operands, [&](const operand_t& operand)
			{
				return dxbc::is_resource(operand, request.mask_texture);
			});
		});

		if (uses != 1)
		{
			throw std::runtime_error(utils::string::va("sun shadow mask t%u is read %zu times", request.mask_texture, static_cast<std::size_t>(uses)));
		}

		auto temps = std::ranges::find_if(instructions, [](const instruction_t& instruction)
		{
			return instruction.opcode.type == D3D10_SB_OPCODE_DCL_TEMPS;
		});

		if (temps == instructions.end())
		{
			throw std::runtime_error("sun shadow shader declares no temps");
		}

		block_remap remap{};
		auto next_temp = temps->operands[0].custom.u.value;
		for (const auto temp : block.temps)
		{
			remap.temps.emplace(temp, next_temp++);
		}

		const auto result_temp = next_temp++;
		temps->operands[0].custom.u.value = next_temp;

		remap.inputs = {{block.position_input, find_input(shader, "TEXCOORD", 5)}, {block.screen_input, find_input(shader, "SV_POSITION", 0)}};
		remap.object_row = request.object_row;
		remap.stable_row = request.stable_row;

		std::set<std::uint32_t> textures;
		auto samplers = request.reserved_samplers;
		sun_shadow_slots slots{};
		slots.shadow_texture = free_slot(instructions, D3D10_SB_OPERAND_TYPE_RESOURCE, textures);
		slots.translucent_texture = free_slot(instructions, D3D10_SB_OPERAND_TYPE_RESOURCE, textures);
		slots.compare_sampler = free_slot(instructions, D3D10_SB_OPERAND_TYPE_SAMPLER, samplers);
		slots.linear_sampler = free_slot(instructions, D3D10_SB_OPERAND_TYPE_SAMPLER, samplers);
		remap.textures = {{source_shadow_texture, slots.shadow_texture}, {source_translucent_texture, slots.translucent_texture}};
		remap.samplers = {{source_compare_sampler, slots.compare_sampler}, {source_linear_sampler, slots.linear_sampler}};

		instruction_list code = block.code;
		for (auto& instruction : code)
		{
			for (auto& operand : instruction.operands)
			{
				remap_operand(operand, remap);
			}
		}

		auto mask = sample->operands[2];
		mask.type = D3D10_SB_OPERAND_TYPE_TEMP;
		mask.indices[0].value.uint32 = result_temp;
		mask.extensions.clear();
		code.emplace_back(make_mov(asm_::r(result_temp).x().mask(), asm_::r(remap.temps.at(source_visibility / 4)).select(source_visibility % 4 + 1).scalar()));
		code.emplace_back(make_mov(asm_::r(result_temp).y().mask(), asm_::r(remap.temps.at(source_blend / 4)).select(source_blend % 4 + 1).scalar()));
		code.emplace_back(make_mov(asm_::r(result_temp).select("zw").mask(), asm_::l(0.0f, 0.0f, 0.0f, 0.0f)));
		code.emplace_back(make_mov(sample->operands[0], mask));

		const auto position = sample - instructions.begin();
		instructions.erase(instructions.begin() + position);
		instructions.insert(instructions.begin() + position, code.begin(), code.end());

		for (auto declaration : block.declarations)
		{
			remap_operand(declaration.operands[0], remap);
			const auto first_code = std::ranges::find_if(instructions, [](const instruction_t& instruction)
			{
				return !dxbc::is_declaration(instruction);
			});

			instructions.insert(first_code, declaration);
		}

		grow_constant_buffer(instructions, 1, request.object_row + sun_shadow_object_rows);
		grow_constant_buffer(instructions, 2, request.stable_row + sun_shadow_stable_rows);
		return slots;
	}
}
