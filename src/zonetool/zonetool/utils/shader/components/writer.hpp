#pragma once

#include "definitions.hpp"

namespace shader::asm_::writer
{
	void write_operand(utils::bit_buffer_le& output_buffer, const operand_t& operand);
	void write_opcode_extended(utils::bit_buffer_le& output_buffer, const opcode_extended_t& opcode);
	void write_opcode(utils::bit_buffer_le& output_buffer, const opcode_t& opcode);

	std::uint32_t get_operand_length(const operand_t& operand);
	void set_opcode_length(instruction_t& instruction);
}
