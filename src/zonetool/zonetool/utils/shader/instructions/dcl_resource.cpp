#include <std_include.hpp>

#include "dcl_resource.hpp"

namespace shader::asm_
{
	instruction_t dcl_resource::read(utils::bit_buffer_le& input_buffer)
	{
		instruction_t instruction;
		instruction.opcode = reader::read_opcode(input_buffer);

		instruction.operands.emplace_back(reader::read_operand(input_buffer));

		operand_t operand{};
		operand.custom.u.values[0] = input_buffer.read_bits(4);
		operand.custom.u.values[1] = input_buffer.read_bits(4);
		operand.custom.u.values[2] = input_buffer.read_bits(4);
		operand.custom.u.values[3] = input_buffer.read_bits(4);
		input_buffer.read_bits(16);

		instruction.operands.emplace_back(operand);

		return instruction;
	}

	void dcl_resource::write(utils::bit_buffer_le& output_buffer, const instruction_t& instruction)
	{
		writer::write_opcode(output_buffer, instruction.opcode);
		writer::write_operand(output_buffer, instruction.operands[0]);
		output_buffer.write_bits(4, instruction.operands[1].custom.u.values[0]);
		output_buffer.write_bits(4, instruction.operands[1].custom.u.values[1]);
		output_buffer.write_bits(4, instruction.operands[1].custom.u.values[2]);
		output_buffer.write_bits(4, instruction.operands[1].custom.u.values[3]);
		output_buffer.write_bits(16, 0);
	}
	
	void dcl_resource::print(const instruction_t& instruction)
	{
		const auto& resource_return_type = instruction.operands[1].custom.u.values;
		printf("dcl_resource_%s ", get_resource_dimension_name(instruction.opcode.controls));
		printf("(%s,%s,%s,%s) ", get_return_type_name(resource_return_type[0]),
			get_return_type_name(resource_return_type[1]),
			get_return_type_name(resource_return_type[2]),
			get_return_type_name(resource_return_type[3]));
		disassembler::print_operand(instruction.operands[0]);
		printf("\n");
	}
}
