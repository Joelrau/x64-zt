#pragma once

#include <utils/bit_buffer.hpp>

#include "../components/reader.hpp"
#include "../components/writer.hpp"
#include "../components/disassembler.hpp"

namespace shader::asm_
{
	class base_instruction
	{
	public:
		base_instruction() = default;
		virtual ~base_instruction()
		{
		}

		virtual instruction_t read(utils::bit_buffer_le&) = 0;
		virtual void write(utils::bit_buffer_le&, const instruction_t&) = 0;
		virtual void print(const instruction_t&) = 0;
	};
}
