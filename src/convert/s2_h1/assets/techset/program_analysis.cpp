#include <std_include.hpp>

#include "../../include.hpp"
#include "program_analysis.hpp"

namespace zonetool::s2::converter::h1::techset
{
	namespace
	{
		using alys::shader::detail::instruction_t;
		using alys::shader::detail::operand_t;

		using component_set = std::set<std::uint32_t>;

		bool opens_block(const instruction_t& instruction)
		{
			switch (instruction.opcode.type)
			{
			case D3D10_SB_OPCODE_IF:
			case D3D10_SB_OPCODE_LOOP:
			case D3D10_SB_OPCODE_SWITCH:
				return true;
			default:
				return false;
			}
		}

		bool closes_block(const instruction_t& instruction)
		{
			switch (instruction.opcode.type)
			{
			case D3D10_SB_OPCODE_ENDIF:
			case D3D10_SB_OPCODE_ENDLOOP:
			case D3D10_SB_OPCODE_ENDSWITCH:
				return true;
			default:
				return false;
			}
		}

		bool is_write(const operand_t& operand)
		{
			return operand.components.type == D3D10_SB_OPERAND_4_COMPONENT && operand.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE;
		}

		void add_reads(const operand_t& operand, component_set& reads)
		{
			for (const auto& index : operand.indices)
			{
				if (index.extra_operand)
				{
					add_reads(*index.extra_operand, reads);
				}
			}

			if (operand.type != D3D10_SB_OPERAND_TYPE_TEMP || is_write(operand))
			{
				return;
			}

			const auto temp = operand.indices[0].value.uint32;
			if (operand.components.type != D3D10_SB_OPERAND_4_COMPONENT)
			{
				reads.insert(temp * 4);
				return;
			}

			const auto count = operand.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE ? 4 : 1;
			for (auto i = 0; i < count; i++)
			{
				reads.insert(temp * 4 + operand.components.names[i]);
			}
		}

		struct instruction_access
		{
			component_set reads;
			component_set writes;
			bool removable;
		};

		instruction_access get_access(const instruction_t& instruction)
		{
			instruction_access result{};
			if (dxbc::is_declaration(instruction))
			{
				return result;
			}

			auto has_temp_write = false;
			auto has_other_write = false;
			for (const auto& operand : instruction.operands)
			{
				add_reads(operand, result.reads);
				if (!is_write(operand))
				{
					continue;
				}

				if (operand.type == D3D10_SB_OPERAND_TYPE_TEMP)
				{
					has_temp_write = true;
					for (auto component = 0u; component < 4; component++)
					{
						if (operand.components.mask & (1u << component))
						{
							result.writes.insert(operand.indices[0].value.uint32 * 4 + component);
						}
					}
				}
				else if (operand.type != D3D10_SB_OPERAND_TYPE_NULL)
				{
					has_other_write = true;
				}
			}

			result.removable = has_temp_write && !has_other_write && !opens_block(instruction) && !closes_block(instruction);
			return result;
		}

		std::vector<std::size_t> loop_depths(const instruction_list& instructions)
		{
			std::vector<std::size_t> result(instructions.size());
			std::vector<bool> stack;
			auto loops = 0u;
			for (auto i = 0u; i < instructions.size(); i++)
			{
				const auto& instruction = instructions[i];
				if (closes_block(instruction) && !stack.empty())
				{
					loops -= stack.back();
					stack.pop_back();
				}

				result[i] = loops;
				if (opens_block(instruction))
				{
					const auto loop = instruction.opcode.type == D3D10_SB_OPCODE_LOOP;
					stack.push_back(loop);
					loops += loop;
				}
			}

			return result;
		}

		bool is_live_from(const std::vector<instruction_access>& access, const instruction_list& instructions, const std::size_t start, const std::uint32_t component)
		{
			auto depth = 0;
			for (auto i = start; i < instructions.size(); i++)
			{
				const auto& instruction = instructions[i];
				if (depth == 0 && (instruction.opcode.type == D3D10_SB_OPCODE_CASE || instruction.opcode.type == D3D10_SB_OPCODE_DEFAULT))
				{
					return true;
				}

				if (depth == 0 && instruction.opcode.type == D3D10_SB_OPCODE_ELSE)
				{
					for (auto nested = 0; ++i < instructions.size();)
					{
						if (opens_block(instructions[i]))
						{
							nested++;
						}
						else if (closes_block(instructions[i]) && nested-- == 0)
						{
							break;
						}
					}

					continue;
				}

				if (closes_block(instruction))
				{
					if (--depth < 0)
					{
						if (instruction.opcode.type == D3D10_SB_OPCODE_ENDLOOP)
						{
							return true;
						}

						depth = 0;
					}
				}

				if (access[i].reads.contains(component))
				{
					return true;
				}

				if (depth == 0 && access[i].writes.contains(component))
				{
					return false;
				}

				if (depth == 0 && instruction.opcode.type == D3D10_SB_OPCODE_RET)
				{
					return false;
				}

				if (opens_block(instruction))
				{
					depth++;
				}
			}

			return false;
		}

		bool reads_buffer_resource(const instruction_t& instruction, const std::set<std::uint32_t>& buffers)
		{
			return !dxbc::is_declaration(instruction) && std::ranges::any_of(instruction.operands, [&](const operand_t& operand)
			{
				return operand.type == D3D10_SB_OPERAND_TYPE_RESOURCE && buffers.contains(operand.indices[0].value.uint32);
			});
		}

		std::set<std::uint32_t> buffer_resources(const instruction_list& instructions)
		{
			std::set<std::uint32_t> result;
			for (const auto& instruction : instructions)
			{
				const auto type = instruction.opcode.type;
				const auto is_buffer = type == D3D11_SB_OPCODE_DCL_RESOURCE_STRUCTURED || type == D3D11_SB_OPCODE_DCL_RESOURCE_RAW
					|| (type == D3D10_SB_OPCODE_DCL_RESOURCE && (instruction.opcode.controls & 0x1F) == D3D10_SB_RESOURCE_DIMENSION_BUFFER);
				if (is_buffer && !instruction.operands.empty())
				{
					result.insert(instruction.operands[0].indices[0].value.uint32);
				}
			}

			return result;
		}
	}

	std::size_t remove_light_loops(instruction_list& instructions)
	{
		const auto buffers = buffer_resources(instructions);
		if (buffers.empty())
		{
			return 0;
		}

		std::vector<std::pair<std::size_t, std::size_t>> loops;
		std::vector<std::size_t> open;
		for (auto i = 0u; i < instructions.size(); i++)
		{
			const auto& instruction = instructions[i];
			if (instruction.opcode.type == D3D10_SB_OPCODE_LOOP)
			{
				open.push_back(i);
			}
			else if (instruction.opcode.type == D3D10_SB_OPCODE_ENDLOOP && !open.empty())
			{
				const auto begin = open.back();
				open.pop_back();
				if (open.empty())
				{
					loops.emplace_back(begin, i);
				}
			}
		}

		std::vector<instruction_access> access;
		for (const auto& instruction : instructions)
		{
			access.emplace_back(get_access(instruction));
		}

		std::vector<std::pair<std::size_t, std::size_t>> removed;
		for (const auto& [begin, end] : loops)
		{
			const auto reads_buffer = std::any_of(instructions.begin() + begin, instructions.begin() + end, [&](const instruction_t& instruction)
			{
				return reads_buffer_resource(instruction, buffers);
			});

			if (!reads_buffer)
			{
				continue;
			}

			component_set written;
			for (auto i = begin; i <= end; i++)
			{
				written.insert(access[i].writes.begin(), access[i].writes.end());
			}

			for (const auto component : written)
			{
				if (!is_live_from(access, instructions, end + 1, component))
				{
					continue;
				}

				const auto has_value = std::any_of(access.begin(), access.begin() + begin, [&](const instruction_access& entry)
				{
					return entry.writes.contains(component);
				});

				if (!has_value)
				{
					throw std::runtime_error(utils::string::va("light loop result r%u.%c has no value outside the loop", component / 4, "xyzw"[component % 4]));
				}
			}

			removed.emplace_back(begin, end);
		}

		for (const auto& [begin, end] : std::views::reverse(removed))
		{
			instructions.erase(instructions.begin() + begin, instructions.begin() + end + 1);
		}

		return removed.size();
	}

	void remove_dead_code(instruction_list& instructions)
	{
		for (auto changed = true; changed;)
		{
			changed = false;
			std::vector<instruction_access> access;
			for (const auto& instruction : instructions)
			{
				access.emplace_back(get_access(instruction));
			}

			const auto depths = loop_depths(instructions);
			std::vector<bool> dead(instructions.size());
			for (auto i = 0u; i < instructions.size(); i++)
			{
				if (!access[i].removable || depths[i])
				{
					continue;
				}

				dead[i] = std::ranges::none_of(access[i].writes, [&](const std::uint32_t component)
				{
					return is_live_from(access, instructions, i + 1, component);
				});

				changed |= dead[i];
			}

			if (!changed)
			{
				break;
			}

			instruction_list kept;
			for (auto i = 0u; i < instructions.size(); i++)
			{
				if (!dead[i])
				{
					kept.emplace_back(std::move(instructions[i]));
				}
			}

			instructions = std::move(kept);
		}
	}

	resource_usage collect_usage(const instruction_list& instructions)
	{
		resource_usage result;
		const std::function<void(const operand_t&)> add = [&](const operand_t& operand)
		{
			for (const auto& index : operand.indices)
			{
				if (index.extra_operand)
				{
					add(*index.extra_operand);
				}
			}

			switch (operand.type)
			{
			case D3D10_SB_OPERAND_TYPE_RESOURCE:
				result.textures.insert(operand.indices[0].value.uint32);
				break;
			case D3D10_SB_OPERAND_TYPE_SAMPLER:
				result.samplers.insert(operand.indices[0].value.uint32);
				break;
			case D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER:
				if (operand.indices[1].representation == D3D10_SB_OPERAND_INDEX_IMMEDIATE32)
				{
					result.rows[operand.indices[0].value.uint32].insert(operand.indices[1].value.uint32);
				}
				else
				{
					result.whole_buffers.insert(operand.indices[0].value.uint32);
				}
				break;
			default:
				break;
			}
		};

		for (const auto& instruction : instructions)
		{
			if (!dxbc::is_declaration(instruction))
			{
				std::ranges::for_each(instruction.operands, add);
			}
		}

		return result;
	}
}
