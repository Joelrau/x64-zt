#include <std_include.hpp>

#include "customdata.hpp"

namespace shader::asm_
{
	instruction_t customdata::read(utils::bit_buffer_le& input_buffer)
	{
		instruction_t instruction;
		instruction.opcode.type = input_buffer.read_bits(11);
		instruction.customdata.data_class = input_buffer.read_bits(21);
		instruction.customdata.count = input_buffer.read_bytes(4);

		for (auto i = 2u; i < instruction.customdata.count; i++)
		{
			instruction.customdata.values.emplace_back(input_buffer.read_bytes(4));
		}

		return instruction;
	}

	void customdata::write(utils::bit_buffer_le& output_buffer, const instruction_t& instruction)
	{
		output_buffer.write_bits(11, D3D10_SB_OPCODE_CUSTOMDATA);
		output_buffer.write_bits(21, instruction.customdata.data_class);
		output_buffer.write_bytes(4, instruction.customdata.count);

		for (const auto& value : instruction.customdata.values)
		{
			output_buffer.write_bytes(4, value.u32);
		}
	}

	void customdata::print(const instruction_t& instruction)
	{
		switch (instruction.customdata.data_class)
		{
		case D3D10_SB_CUSTOMDATA_COMMENT:
			break;
		case D3D10_SB_CUSTOMDATA_DEBUGINFO:
			break;
		case D3D10_SB_CUSTOMDATA_OPAQUE:
			break;
		case D3D10_SB_CUSTOMDATA_DCL_IMMEDIATE_CONSTANT_BUFFER:
		{
			printf("dcl_immediateConstantBuffer ");
			const auto const_count = (instruction.customdata.count - 2) / 4;
			printf("\n{\n");
			for (auto i = 0u; i < const_count; i++)
			{
				const auto values = &instruction.customdata.values[i * 4];
				printf("\t{%f, %f, %f, %f}", values[0].f32, values[1].f32, values[2].f32, values[3].f32);
				if (i < const_count - 1)
				{
					printf(",\n");
				}
			}
			printf("\n}\n");
		}
		}
	}
}
