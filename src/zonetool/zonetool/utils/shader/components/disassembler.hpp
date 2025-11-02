#pragma once

#include "definitions.hpp"

namespace shader::asm_::disassembler
{
	void print_operand(const operand_t& op);
	void print_operands(const std::vector<operand_t>& operands);
	void print_opcode_extended(const opcode_extended_t& opcode);
	void print_opcode(const opcode_t& opcode);
}
