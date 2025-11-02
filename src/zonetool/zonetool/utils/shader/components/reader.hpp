#pragma once

#include "definitions.hpp"

namespace shader::asm_::reader
{
	operand_extended_t read_extended_operand(utils::bit_buffer_le& bit_buffer);
	operand_t read_operand(utils::bit_buffer_le& bit_buffer);
	opcode_extended_t read_opcode_extended(utils::bit_buffer_le& input_buffer);
	opcode_t read_opcode(utils::bit_buffer_le& input_buffer);
}