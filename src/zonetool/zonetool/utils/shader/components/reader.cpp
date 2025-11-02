#include <std_include.hpp>

#include "reader.hpp"

namespace shader::asm_::reader
{
	operand_extended_t read_extended_operand(utils::bit_buffer_le& bit_buffer)
	{
		operand_extended_t operand{};

		operand.type = bit_buffer.read_bits(6);
		if (operand.type == D3D10_SB_EXTENDED_OPERAND_MODIFIER)
		{
			operand.modifier = bit_buffer.read_bits(8);
			bit_buffer.read_bits(17);
		}
		else
		{
			bit_buffer.read_bits(25);
		}

		operand.extended = bit_buffer.read_bits(1);

		return operand;
	}

	operand_t read_operand(utils::bit_buffer_le& bit_buffer)
	{
		operand_t operand{};

		operand.components.type = bit_buffer.read_bits(2);

		switch (operand.components.type)
		{
		case D3D10_SB_OPERAND_4_COMPONENT:
		{
			operand.components.selection_mode = bit_buffer.read_bits(2);
			if (operand.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE)
			{
				operand.components.mask = bit_buffer.read_bits(4);
				bit_buffer.read_bits(4);
			}
			else if (operand.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE)
			{
				operand.components.names[0] = bit_buffer.read_bits(2);
				operand.components.names[1] = bit_buffer.read_bits(2);
				operand.components.names[2] = bit_buffer.read_bits(2);
				operand.components.names[3] = bit_buffer.read_bits(2);
			}
			else if (operand.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_SELECT_1_MODE)
			{
				operand.components.names[0] = bit_buffer.read_bits(2);
				bit_buffer.read_bits(6);
			}

			break;
		}
		case D3D10_SB_OPERAND_0_COMPONENT:
		case D3D10_SB_OPERAND_1_COMPONENT:
		default:
			bit_buffer.read_bits(10);
			break;
		}

		operand.type = bit_buffer.read_bits(8);
		operand.dimension = bit_buffer.read_bits(2);

		operand.indices[0].representation = bit_buffer.read_bits(3);
		operand.indices[1].representation = bit_buffer.read_bits(3);
		operand.indices[2].representation = bit_buffer.read_bits(3);

		operand.extended = bit_buffer.read_bits(1);
		auto extended = operand.extended;
		while (extended)
		{
			const auto operand_extended = read_extended_operand(bit_buffer);
			operand.extensions.emplace_back(operand_extended);
			extended = operand_extended.extended;
		}

		const auto read_index_rep = [&](const std::uint32_t index)
		{
			switch (operand.indices[index].representation)
			{
			case D3D10_SB_OPERAND_INDEX_IMMEDIATE32:
				operand.indices[index].values[0].u32 = bit_buffer.read_bytes(4);
				break;
			case D3D10_SB_OPERAND_INDEX_IMMEDIATE64:
				operand.indices[index].values[0].u64.fields.low = bit_buffer.read_bytes(4);
				operand.indices[index].values[0].u64.fields.high = bit_buffer.read_bytes(4);
				break;
			case D3D10_SB_OPERAND_INDEX_RELATIVE:
				operand.extra_operand = std::make_shared<operand_t>(read_operand(bit_buffer));
				break;
			case D3D10_SB_OPERAND_INDEX_IMMEDIATE32_PLUS_RELATIVE:
				operand.indices[index].values[0].u32 = bit_buffer.read_bytes(4);
				operand.extra_operand = std::make_shared<operand_t>(read_operand(bit_buffer));
				break;
			case D3D10_SB_OPERAND_INDEX_IMMEDIATE64_PLUS_RELATIVE:
				operand.indices[index].values[0].u64.fields.low = bit_buffer.read_bytes(4);
				operand.indices[index].values[0].u64.fields.high = bit_buffer.read_bytes(4);
				operand.extra_operand = std::make_shared<operand_t>(read_operand(bit_buffer));
				break;
			}
		};

		if (operand.dimension >= D3D10_SB_OPERAND_INDEX_1D)
		{
			read_index_rep(0);
		}

		if (operand.dimension >= D3D10_SB_OPERAND_INDEX_2D)
		{
			read_index_rep(1);
		}

		if (operand.dimension == D3D10_SB_OPERAND_INDEX_3D)
		{
			read_index_rep(2);
		}

		if (operand.type == D3D10_SB_OPERAND_TYPE_IMMEDIATE32)
		{
			if (operand.components.type == D3D10_SB_OPERAND_4_COMPONENT)
			{
				operand.indices[0].values[0].u32 = bit_buffer.read_bytes(4);
				operand.indices[0].values[1].u32 = bit_buffer.read_bytes(4);
				operand.indices[0].values[2].u32 = bit_buffer.read_bytes(4);
				operand.indices[0].values[3].u32 = bit_buffer.read_bytes(4);
			}
			else
			{
				operand.indices[0].values[0].u32 = bit_buffer.read_bytes(4);
			}
		}

		if (operand.type == D3D10_SB_OPERAND_TYPE_IMMEDIATE64)
		{
			operand.indices[0].values[0].u64.fields.low = bit_buffer.read_bytes(4);
			operand.indices[0].values[0].u64.fields.high = bit_buffer.read_bytes(4);
		}

		return operand;
	}

	opcode_extended_t read_opcode_extended(utils::bit_buffer_le& input_buffer)
	{
		opcode_extended_t opcode{};

		opcode.type = input_buffer.read_bits(6);
		switch (opcode.type)
		{
		case D3D10_SB_EXTENDED_OPCODE_SAMPLE_CONTROLS:
			input_buffer.read_bits(3);
			opcode.values[0] = input_buffer.read_bits(4);
			opcode.values[1] = input_buffer.read_bits(4);
			opcode.values[2] = input_buffer.read_bits(4);
			input_buffer.read_bits(10);
			break;
		case D3D11_SB_EXTENDED_OPCODE_RESOURCE_DIM:
			opcode.values[0] = input_buffer.read_bits(5);
			input_buffer.read_bits(20);
			break;
		case D3D11_SB_EXTENDED_OPCODE_RESOURCE_RETURN_TYPE:
			opcode.values[0] = input_buffer.read_bits(4);
			opcode.values[1] = input_buffer.read_bits(4);
			opcode.values[2] = input_buffer.read_bits(4);
			opcode.values[3] = input_buffer.read_bits(4);
			input_buffer.read_bits(9);
			break;
		default:
			input_buffer.read_bits(25);
			break;
		}

		opcode.extended = input_buffer.read_bits(1);

		return opcode;
	}

	opcode_t read_opcode(utils::bit_buffer_le& input_buffer)
	{
		opcode_t opcode{};

		opcode.type = input_buffer.read_bits(11);
		opcode.controls = input_buffer.read_bits(13);
		opcode.length = input_buffer.read_bits(7);
		opcode.extended = input_buffer.read_bits(1);

		auto extended = opcode.extended;
		while (extended)
		{
			const auto opcode_extended = read_opcode_extended(input_buffer);
			opcode.extensions.emplace_back(opcode_extended);
			extended = opcode_extended.extended;
		}

		return opcode;
	}
}
