#pragma once

#include "definitions.hpp"

namespace shader::asm_::tokens
{
	opcode_t create_opcode(const std::uint32_t type, const std::uint32_t controls = 0u);
	instruction_t create_instruction(const std::uint32_t type, const std::uint32_t controls = 0u);

	void add_operand(instruction_t& instruction, const operand_t& operand);

	operand_t create_literal_operand(const float value);
	operand_t create_literal_operand(const float x, const float y, const float z, const float w);

	std::uint32_t get_swizzle_component(const char c);

	operand_t create_operand(const std::uint32_t type, const operand_components_t& operand_components, const std::vector<std::uint32_t>& indices);
	operand_t create_operand(const std::uint32_t type, const std::vector<std::uint32_t>& components, const std::vector<std::uint32_t>& indices);
	operand_t create_operand(const std::uint32_t type, const std::string& component_names, const std::vector<std::uint32_t>& indices);
	operand_t create_operand(const std::uint32_t type, const std::uint32_t component_mask, const std::vector<std::uint32_t>& indices);

	template <typename T, typename ...Args>
	operand_t create_operand(const std::uint32_t type, const T& components, Args&&... args)
	{
		return create_operand(type, components, {std::forward<Args>(args)...});
	}

	std::vector<std::uint32_t> find_operands(const instruction_t& instruction, const std::uint32_t beg,
		const std::function<bool(const operand_t&)>& cb);
}
