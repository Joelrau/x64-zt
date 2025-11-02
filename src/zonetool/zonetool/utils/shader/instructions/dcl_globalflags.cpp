#include <std_include.hpp>

#include "dcl_globalflags.hpp"

namespace shader::asm_
{
	instruction_t dcl_globalflags::read(utils::bit_buffer_le& input_buffer)
	{
		instruction_t instruction;
		instruction.opcode = reader::read_opcode(input_buffer);
		return instruction;
	}

	void dcl_globalflags::write(utils::bit_buffer_le& output_buffer, const instruction_t& instruction)
	{
		writer::write_opcode(output_buffer, instruction.opcode);
	}

	void dcl_globalflags::print(const instruction_t& instruction)
	{
		printf("dcl_globalFlags ");

		if ((instruction.opcode.controls & (1 << 0)) != 0)
		{
			printf("refactoringAllowed ");
		}

		if ((instruction.opcode.controls & (1 << 1)) != 0)
		{
			printf("enableDoublePrecision ");
		}

		if ((instruction.opcode.controls & (1 << 2)) != 0)
		{
			printf("forceEarlyDepthStencilTest ");
		}

		if ((instruction.opcode.controls & (1 << 3)) != 0)
		{
			printf("enableRawStructuredBuffers ");
		}

		printf("\n");
	}
}
