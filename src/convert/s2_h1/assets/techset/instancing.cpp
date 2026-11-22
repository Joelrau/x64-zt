#include <std_include.hpp>

#include "../../include.hpp"
#include "instancing.hpp"

#include <formats/dxbc.hpp>

namespace zonetool::s2::converter::h1::techset
{
	namespace
	{
		constexpr auto h1_instance_row_shift = 2u;
		constexpr auto one = 0x3F800000u;

		std::optional<std::uint32_t> find_instance_input(const std::vector<alys::shader::detail::instruction_t>& instructions)
		{
			const auto input = std::ranges::find_if(instructions, [](const alys::shader::detail::instruction_t& instruction)
			{
				return instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_SGV && instruction.operands[1].custom.u.value == D3D10_SB_NAME_INSTANCE_ID;
			});

			return input != instructions.end() ? std::optional(input->operands[0].indices[0].value.uint32) : std::nullopt;
		}

		std::optional<std::uint32_t> written_component(const alys::shader::detail::operand_t& operand)
		{
			if (operand.type != D3D10_SB_OPERAND_TYPE_TEMP || std::popcount(operand.components.mask) != 1)
			{
				return {};
			}

			return dxbc::temp_component(operand.indices[0].value.uint32, static_cast<std::uint32_t>(std::countr_zero(operand.components.mask)));
		}

		bool is_instance_row(const alys::shader::detail::instruction_t& instruction, const std::uint32_t instance_input)
		{
			const auto& operands = instruction.operands;
			return instruction.opcode.type == D3D10_SB_OPCODE_ISHL && operands[1].type == D3D10_SB_OPERAND_TYPE_INPUT
				&& operands[1].indices[0].value.uint32 == instance_input && operands[2].type == D3D10_SB_OPERAND_TYPE_IMMEDIATE32
				&& operands[2].immediate_values[0].uint32 == 3u && written_component(operands[0]);
		}

		void convert_instance_read(alys::shader::detail::operand_t& operand, const std::unordered_set<std::uint32_t>& instance_rows)
		{
			for (auto& index : operand.indices)
			{
				if (index.extra_operand)
				{
					convert_instance_read(*index.extra_operand, instance_rows);
				}
			}

			if (operand.type != D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER || operand.indices[0].value.uint32 != s2_instance_matrix_buffer)
			{
				return;
			}

			const auto& row = operand.indices[1];
			const auto is_relative = row.representation == D3D10_SB_OPERAND_INDEX_RELATIVE
				|| row.representation == D3D10_SB_OPERAND_INDEX_IMMEDIATE32_PLUS_RELATIVE;
			const auto offset = row.representation == D3D10_SB_OPERAND_INDEX_RELATIVE ? 0u : row.value.uint32;
			if (!is_relative || !row.extra_operand || row.extra_operand->type != D3D10_SB_OPERAND_TYPE_TEMP)
			{
				throw std::runtime_error("shader reads the instance buffer without the instance row");
			}

			const auto& temp = *row.extra_operand;
			if (!instance_rows.contains(dxbc::temp_component(temp.indices[0].value.uint32, temp.components.names[0])))
			{
				throw std::runtime_error("shader reads the instance buffer without the instance row");
			}

			if (offset == 4u)
			{
				auto literal = dxbc::uint_literals(one, one, one, one); // TODO: per instance model scale, like WORLD_MATRIX_INV_SCALE_SQ
				literal.extensions = operand.extensions;
				operand = literal;
				return;
			}

			if (offset >= (1u << h1_instance_row_shift))
			{
				throw std::runtime_error(utils::string::va("instance row %u has no h1 source", offset));
			}
		}
	}

	void convert_instancing(std::vector<alys::shader::detail::instruction_t>& instructions)
	{
		if (!dxbc::declares_constant_buffer(instructions, s2_instance_matrix_buffer))
		{
			return;
		}

		const auto instance_input = find_instance_input(instructions);
		if (!instance_input)
		{
			throw std::runtime_error("shader declares the instance buffer without an instance id");
		}

		std::unordered_set<std::uint32_t> instance_rows;
		for (auto& instruction : instructions)
		{
			if (instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER && instruction.operands[0].indices[0].value.uint32 == s2_instance_matrix_buffer)
			{
				instruction.operands[0].indices[1].value.uint32 = 512u;
				continue;
			}

			if (dxbc::is_declaration(instruction))
			{
				continue;
			}

			for (auto i = 1u; i < instruction.operands.size(); i++)
			{
				convert_instance_read(instruction.operands[i], instance_rows);
			}

			if (is_instance_row(instruction, *instance_input))
			{
				instruction.operands[2].immediate_values[0].uint32 = h1_instance_row_shift;
				instance_rows.insert(*written_component(instruction.operands[0]));
			}
			else if (!instruction.operands.empty() && instruction.operands[0].type == D3D10_SB_OPERAND_TYPE_TEMP
				&& dxbc::is_destination(instruction.operands[0]))
			{
				const auto& destination = instruction.operands[0];
				for (auto component = 0u; component < 4u; component++)
				{
					if (destination.components.mask & (1u << component))
					{
						instance_rows.erase(dxbc::temp_component(destination.indices[0].value.uint32, component));
					}
				}
			}
		}
	}
}
