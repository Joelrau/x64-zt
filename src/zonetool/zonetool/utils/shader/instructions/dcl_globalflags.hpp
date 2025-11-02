#pragma once

#include "base.hpp"

namespace shader::asm_
{
	class dcl_globalflags final : public base_instruction
	{
	public:
		instruction_t read(utils::bit_buffer_le&);
		void write(utils::bit_buffer_le&, const instruction_t&);
		void print(const instruction_t&);
	};
}
