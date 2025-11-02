#include <std_include.hpp>

#include "generic.hpp"

namespace shader::asm_
{
	instruction_t general_instruction::read(utils::bit_buffer_le& input_buffer)
	{
		instruction_t instruction{};
		instruction.opcode = reader::read_opcode(input_buffer);

		const auto opcode_op_len = 1 + instruction.opcode.extensions.size();
		const auto end = input_buffer.total() + (instruction.opcode.length - opcode_op_len) * 8 * 4;
		while (input_buffer.total() < end)
		{
			const auto operand = reader::read_operand(input_buffer);
			instruction.operands.emplace_back(operand);
		}

		return instruction;
	}

	void general_instruction::write(utils::bit_buffer_le& output_buffer, const instruction_t& instruction)
	{
		writer::write_opcode(output_buffer, instruction.opcode);
		for (const auto& operand : instruction.operands)
		{
			writer::write_operand(output_buffer, operand);
		}
	}

	void general_instruction::print(const instruction_t& instruction)
	{
		disassembler::print_opcode(instruction.opcode);
		printf(" ");
		disassembler::print_operands(instruction.operands);
		printf("\n");
	}

	instruction_t declaration_instruction_nametoken::read(utils::bit_buffer_le& input_buffer)
	{
		instruction_t instruction{};
		operand_t op1{};

		instruction.opcode = reader::read_opcode(input_buffer);

		const auto op0 = reader::read_operand(input_buffer);
		op1.custom.u.value = input_buffer.read_bits(15);

		input_buffer.read_bits(17);

		instruction.operands.emplace_back(op0);
		instruction.operands.emplace_back(op1);

		return instruction;
	}

	void declaration_instruction_nametoken::write(utils::bit_buffer_le& output_buffer, const instruction_t& instruction)
	{
		writer::write_opcode(output_buffer, instruction.opcode);
		writer::write_operand(output_buffer, instruction.operands[0]);
		output_buffer.write_bits(15, instruction.operands[1].custom.u.value);
		output_buffer.write_bits(17, 0);
	}

	void declaration_instruction_nametoken::print(const instruction_t& instruction)
	{
		disassembler::print_opcode(instruction.opcode);
		printf(" ");
		disassembler::print_operand(instruction.operands[0]);
		printf(", ");
		printf("%s\n", get_name_token(instruction.operands[1].custom.u.value));
	}
}
