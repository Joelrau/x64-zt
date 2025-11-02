#include <std_include.hpp>

#include "reader.hpp"

namespace shader::asm_::writer
{
	void write_operand(utils::bit_buffer_le& output_buffer, const operand_t& operand)
	{
		output_buffer.write_bits(2, operand.components.type);
		output_buffer.write_bits(2, operand.components.selection_mode);

		switch (operand.components.selection_mode)
		{
		case D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE:
			output_buffer.write_bits(4, operand.components.mask);
			output_buffer.write_bits(4, 0);
			break;
		case D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE:
			output_buffer.write_bits(2, operand.components.names[0]);
			output_buffer.write_bits(2, operand.components.names[1]);
			output_buffer.write_bits(2, operand.components.names[2]);
			output_buffer.write_bits(2, operand.components.names[3]);
			break;
		case D3D10_SB_OPERAND_4_COMPONENT_SELECT_1_MODE:
			output_buffer.write_bits(2, operand.components.names[0]);
			output_buffer.write_bits(6, 0);
			break;
		}

		output_buffer.write_bits(8, operand.type);
		output_buffer.write_bits(2, operand.dimension);

		output_buffer.write_bits(3, operand.indices[0].representation);
		output_buffer.write_bits(3, operand.indices[1].representation);
		output_buffer.write_bits(3, operand.indices[2].representation);

		output_buffer.write_bits(1, operand.extended);

		for (const auto& operand_extended : operand.extensions)
		{
			output_buffer.write_bits(6, operand_extended.type);
			output_buffer.write_bits(8, operand_extended.modifier);
			output_buffer.write_bits(17, 0);
			output_buffer.write_bits(1, operand_extended.extended);
		}

		const auto write_index = [&](const std::uint32_t index)
		{
			switch (operand.indices[index].representation)
			{
			case D3D10_SB_OPERAND_INDEX_IMMEDIATE32:
				output_buffer.write_bytes(4, operand.indices[index].values[0].u32);
				break;
			case D3D10_SB_OPERAND_INDEX_IMMEDIATE64:
				output_buffer.write_bytes(8, operand.indices[index].values[0].u64.value);
				break;
			case D3D10_SB_OPERAND_INDEX_RELATIVE:
				write_operand(output_buffer, *operand.extra_operand);
				break;
			case D3D10_SB_OPERAND_INDEX_IMMEDIATE32_PLUS_RELATIVE:
				output_buffer.write_bytes(4, operand.indices[index].values[0].u32);
				write_operand(output_buffer, *operand.extra_operand);
				break;
			case D3D10_SB_OPERAND_INDEX_IMMEDIATE64_PLUS_RELATIVE:
				output_buffer.write_bytes(8, operand.indices[index].values[0].u64.value);
				write_operand(output_buffer, *operand.extra_operand);
				break;
			}
		};

		if (operand.dimension >= D3D10_SB_OPERAND_INDEX_1D)
		{
			write_index(0);
		}

		if (operand.dimension >= D3D10_SB_OPERAND_INDEX_2D)
		{
			write_index(1);
		}

		if (operand.dimension == D3D10_SB_OPERAND_INDEX_3D)
		{
			write_index(2);
		}

		if (operand.type == D3D10_SB_OPERAND_TYPE_IMMEDIATE32)
		{
			if (operand.components.type == D3D10_SB_OPERAND_4_COMPONENT)
			{
				output_buffer.write_bytes(4, operand.indices[0].values[0].u32);
				output_buffer.write_bytes(4, operand.indices[0].values[1].u32);
				output_buffer.write_bytes(4, operand.indices[0].values[2].u32);
				output_buffer.write_bytes(4, operand.indices[0].values[3].u32);
			}
			else
			{
				output_buffer.write_bytes(4, operand.indices[0].values[0].u32);
			}
		}

		if (operand.type == D3D10_SB_OPERAND_TYPE_IMMEDIATE64)
		{
			output_buffer.write_bytes(8, operand.indices[0].values[0].u64.value);
		}
	}

	void write_opcode_extended(utils::bit_buffer_le& output_buffer, const opcode_extended_t& opcode)
	{
		output_buffer.write_bits(6, opcode.type);

		switch (opcode.type)
		{
		case D3D10_SB_EXTENDED_OPCODE_SAMPLE_CONTROLS:
			output_buffer.write_bits(3, 0);
			output_buffer.write_bits(4, opcode.values[0]);
			output_buffer.write_bits(4, opcode.values[0]);
			output_buffer.write_bits(4, opcode.values[0]);
			output_buffer.write_bits(10, 0);
			break;
		case D3D11_SB_EXTENDED_OPCODE_RESOURCE_DIM:
			output_buffer.write_bits(5, opcode.values[0]);
			output_buffer.write_bits(20, 0);
			break;
		case D3D11_SB_EXTENDED_OPCODE_RESOURCE_RETURN_TYPE:
			output_buffer.write_bits(4, opcode.values[0]);
			output_buffer.write_bits(4, opcode.values[1]);
			output_buffer.write_bits(4, opcode.values[2]);
			output_buffer.write_bits(4, opcode.values[3]);
			output_buffer.write_bits(9, 0);
			break;
		default:
			output_buffer.write_bits(25, 0);
			break;
		}

		output_buffer.write_bits(1, opcode.extended);
	}

	void write_opcode(utils::bit_buffer_le& output_buffer, const opcode_t& opcode)
	{
		output_buffer.write_bits(11, opcode.type);
		output_buffer.write_bits(13, opcode.controls);
		output_buffer.write_bits(7, opcode.length);
		output_buffer.write_bits(1, opcode.extended);

		for (const auto& opcode_extended : opcode.extensions)
		{
			write_opcode_extended(output_buffer, opcode_extended);
		}
	}

	std::uint32_t get_operand_length(const operand_t& operand)
	{
		std::uint32_t length = 1;

		length += static_cast<std::uint32_t>(operand.extensions.size());

		const auto write_index = [&](const std::uint32_t index)
		{
			switch (operand.indices[index].representation)
			{
			case D3D10_SB_OPERAND_INDEX_IMMEDIATE32:
				length += 1;
				break;
			case D3D10_SB_OPERAND_INDEX_IMMEDIATE64:
				length += 2;
				break;
			case D3D10_SB_OPERAND_INDEX_RELATIVE:
				length += get_operand_length(*operand.extra_operand);
				break;
			case D3D10_SB_OPERAND_INDEX_IMMEDIATE32_PLUS_RELATIVE:
				length += get_operand_length(*operand.extra_operand);
				length += 1;
				break;
			case D3D10_SB_OPERAND_INDEX_IMMEDIATE64_PLUS_RELATIVE:
				length += get_operand_length(*operand.extra_operand);
				length += 2;
				break;
			}
		};

		if (operand.dimension >= D3D10_SB_OPERAND_INDEX_1D)
		{
			write_index(0);
		}

		if (operand.dimension >= D3D10_SB_OPERAND_INDEX_2D)
		{
			write_index(1);
		}

		if (operand.dimension == D3D10_SB_OPERAND_INDEX_3D)
		{
			write_index(2);
		}

		if (operand.type == D3D10_SB_OPERAND_TYPE_IMMEDIATE32)
		{
			if (operand.components.type == D3D10_SB_OPERAND_4_COMPONENT)
			{
				length += 4;
			}
			else
			{
				length += 1;
			}
		}

		if (operand.type == D3D10_SB_OPERAND_TYPE_IMMEDIATE64)
		{
			length += 2;
		}

		return length;
	}

	void set_opcode_length(instruction_t& instruction)
	{
		instruction.opcode.length = 1;
		for (const auto& operand : instruction.operands)
		{
			instruction.opcode.length += get_operand_length(operand);
		}
	}
}
