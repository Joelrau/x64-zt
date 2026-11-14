#include <std_include.hpp>

#include "../../include.hpp"
#include "dxbc.hpp"

namespace zonetool::h1::converter::s2::shader
{
	namespace
	{
		bool is_return(const alys::shader::detail::instruction_t& instruction)
		{
			return instruction.opcode.type == D3D10_SB_OPCODE_RET || instruction.opcode.type == D3D10_SB_OPCODE_RETC;
		}
	}

	void relocate_constant_row(alys::shader::detail::operand_t& operand, const std::uint32_t buffer, const std::uint32_t from, const std::uint32_t to,
		const std::optional<std::uint32_t> to_buffer)
	{
		for (auto& index : operand.indices)
		{
			if (index.extra_operand)
			{
				relocate_constant_row(*index.extra_operand, buffer, from, to, to_buffer);
			}
		}

		if (operand.type == D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER && operand.indices[0].value.uint32 == buffer
			&& operand.indices[1].representation == D3D10_SB_OPERAND_INDEX_IMMEDIATE32 && operand.indices[1].value.uint32 == from)
		{
			operand.indices[0].value.uint32 = to_buffer.value_or(buffer);
			operand.indices[1].value.uint32 = to;
		}
	}

	void redirect_input(alys::shader::detail::operand_t& operand, const std::uint32_t input, const std::uint32_t temp)
	{
		for (auto& index : operand.indices)
		{
			if (index.extra_operand)
			{
				redirect_input(*index.extra_operand, input, temp);
			}
		}

		if (operand.type != D3D10_SB_OPERAND_TYPE_INPUT)
		{
			return;
		}

		if (operand.indices[0].representation != D3D10_SB_OPERAND_INDEX_IMMEDIATE32)
		{
			throw std::runtime_error("vertex shader indexes its inputs");
		}

		if (operand.indices[0].value.uint32 == input)
		{
			operand.type = D3D10_SB_OPERAND_TYPE_TEMP;
			operand.indices[0].value.uint32 = temp;
		}
	}

	bool is_declaration(const alys::shader::detail::instruction_t& instruction)
	{
		return instruction.opcode.type == D3D10_SB_OPCODE_CUSTOMDATA
			|| (alys::shader::detail::get_instruction_flags(instruction) & alys::shader::detail::flag_declaration);
	}

	bool is_input_declaration(const alys::shader::detail::instruction_t& instruction)
	{
		return instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT
			|| instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_SGV
			|| instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_SIV;
	}

	bool is_destination(const alys::shader::detail::operand_t& operand)
	{
		return operand.type == D3D10_SB_OPERAND_TYPE_NULL || (operand.type != D3D10_SB_OPERAND_TYPE_IMMEDIATE32
			&& operand.components.type == D3D10_SB_OPERAND_4_COMPONENT && operand.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE);
	}

	bool is_resource(const alys::shader::detail::operand_t& operand, const std::uint32_t texture)
	{
		return operand.type == D3D10_SB_OPERAND_TYPE_RESOURCE && operand.indices[0].value.uint32 == texture;
	}

	bool is_sample(const alys::shader::detail::instruction_t& instruction, const std::uint32_t texture)
	{
		switch (instruction.opcode.type)
		{
		case D3D10_SB_OPCODE_SAMPLE:
		case D3D10_SB_OPCODE_SAMPLE_C:
		case D3D10_SB_OPCODE_SAMPLE_C_LZ:
		case D3D10_SB_OPCODE_SAMPLE_L:
		case D3D10_SB_OPCODE_SAMPLE_D:
		case D3D10_SB_OPCODE_SAMPLE_B:
		case D3D10_1_SB_OPCODE_GATHER4:
		case D3D11_SB_OPCODE_GATHER4_C:
			return is_resource(instruction.operands[2], texture);
		default:
			return false;
		}
	}

	std::uint8_t read_components(const alys::shader::detail::operand_t& source, const std::uint8_t written, const std::uint32_t opcode)
	{
		if (source.components.type != D3D10_SB_OPERAND_4_COMPONENT)
		{
			return 0;
		}

		if (source.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_SELECT_1_MODE)
		{
			return static_cast<std::uint8_t>(1 << source.components.names[0]);
		}

		const auto dot_size = opcode == D3D10_SB_OPCODE_DP2 ? 2 : opcode == D3D10_SB_OPCODE_DP3 ? 3 : opcode == D3D10_SB_OPCODE_DP4 ? 4 : 0;
		std::uint8_t result{};
		for (auto component = 0; component < 4; component++)
		{
			const auto used = dot_size ? component < dot_size : (written >> component) & 1;
			if (used)
			{
				result |= static_cast<std::uint8_t>(1 << source.components.names[component]);
			}
		}

		return result;
	}

	void set_resource_dimension(alys::shader::detail::instruction_t& instruction, const std::uint16_t dimension)
	{
		for (auto& extension : instruction.opcode.extensions)
		{
			if (extension.type == D3D11_SB_EXTENDED_OPCODE_RESOURCE_DIM)
			{
				extension.values[0] = dimension;
			}
		}
	}

	std::uint32_t temp_component(const std::uint32_t temp, const std::uint32_t component)
	{
		return temp * 4 + component;
	}

	alys::shader::detail::operand_t uint_literal(const std::uint32_t value)
	{
		auto result = asm_::l(0.0f);
		result.immediate_values[0].uint32 = value;
		return result;
	}

	alys::shader::detail::operand_t uint_literals(const std::uint32_t x, const std::uint32_t y, const std::uint32_t z, const std::uint32_t w)
	{
		auto result = asm_::l(0.0f, 0.0f, 0.0f, 0.0f);
		result.immediate_values[0].uint32 = x;
		result.immediate_values[1].uint32 = y;
		result.immediate_values[2].uint32 = z;
		result.immediate_values[3].uint32 = w;
		return result;
	}

	signature_element& find_signature_element(alys::shader::shader_object& shader, const std::uint32_t chunk,
		const std::function<bool(const signature_element&)>& accept, const char* what)
	{
		auto& signatures = shader.get_signatures();
		if (const auto elements = signatures.find(chunk); elements != signatures.end())
		{
			if (const auto element = std::ranges::find_if(elements->second, accept); element != elements->second.end())
			{
				return *element;
			}
		}

		throw std::runtime_error(utils::string::va("shader has no %s", what));
	}

	bool declares_resource(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::uint32_t slot)
	{
		return std::ranges::any_of(instructions, [&](const alys::shader::detail::instruction_t& instruction)
		{
			return is_declaration(instruction) && !instruction.operands.empty() && is_resource(instruction.operands[0], slot);
		});
	}

	bool declares_constant_buffer(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::uint32_t slot)
	{
		return std::ranges::any_of(instructions, [&](const alys::shader::detail::instruction_t& instruction)
		{
			return instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER && instruction.operands[0].indices[0].value.uint32 == slot;
		});
	}

	bool declares_sampler(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::uint32_t slot)
	{
		return std::ranges::any_of(instructions, [&](const alys::shader::detail::instruction_t& instruction)
		{
			return instruction.opcode.type == D3D10_SB_OPCODE_DCL_SAMPLER && instruction.operands[0].indices[0].value.uint32 == slot;
		});
	}

	alys::shader::detail::operand_t as_temp(const alys::shader::detail::operand_t& operand, const std::uint32_t temp)
	{
		alys::shader::detail::operand_t result = asm_::r(temp).select("xyzw");
		result.components = operand.components;
		result.extensions = operand.extensions;
		return result;
	}

	bool writes_operand(const alys::shader::detail::instruction_t& instruction, const std::uint32_t type, const std::uint32_t index)
	{
		return !instruction.operands.empty() && instruction.operands[0].type == type && instruction.operands[0].indices[0].value.uint32 == index;
	}

	alys::shader::detail::operand_t negate(alys::shader::detail::operand_t operand)
	{
		alys::shader::detail::operand_extended_t modifier{};
		modifier.type = D3D10_SB_EXTENDED_OPERAND_MODIFIER;
		modifier.modifier = D3D10_SB_OPERAND_MODIFIER_NEG;
		operand.extensions = {modifier};
		return operand;
	}

	bool is_input_ps_declaration(const alys::shader::detail::instruction_t& instruction)
	{
		return instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_PS || instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_PS_SIV
			|| instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_PS_SGV;
	}

	bool is_resource_declaration(const alys::shader::detail::instruction_t& instruction)
	{
		return is_declaration(instruction) && !instruction.operands.empty()
			&& (instruction.operands[0].type == D3D10_SB_OPERAND_TYPE_RESOURCE || instruction.operands[0].type == D3D10_SB_OPERAND_TYPE_SAMPLER);
	}

	void check_single_return(const std::vector<alys::shader::detail::instruction_t>& instructions, const char* what)
	{
		if (instructions.empty() || instructions.back().opcode.type != D3D10_SB_OPCODE_RET
			|| std::any_of(instructions.begin(), instructions.end() - 1, is_return))
		{
			throw std::runtime_error(utils::string::va("%s pixel shader returns early", what));
		}
	}

	void check_writes_only_o0(alys::shader::shader_object& shader, const char* what)
	{
		const auto outputs = shader.get_signatures().find(alys::shader::chunk_osgn);
		if (outputs == shader.get_signatures().end() || outputs->second.size() != 1 || outputs->second[0].register_ != 0)
		{
			throw std::runtime_error(utils::string::va("%s pixel shader writes more than o0", what));
		}
	}

	void relocate_resource_slots(alys::shader::detail::instruction_t& instruction, const std::map<std::uint32_t, std::uint32_t>& slots)
	{
		for (auto& operand : instruction.operands)
		{
			const auto slot = operand.type == D3D10_SB_OPERAND_TYPE_RESOURCE ? slots.find(operand.indices[0].value.uint32) : slots.end();
			if (slot != slots.end())
			{
				operand.indices[0].value.uint32 = slot->second;
			}
		}
	}
}
