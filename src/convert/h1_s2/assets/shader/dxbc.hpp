#pragma once

#include <shader-tool/shader.hpp>

#include "shader.hpp"

namespace asm_ = alys::shader::literals;

namespace zonetool::h1::converter::s2::shader
{
	constexpr std::array<std::uint8_t, 4> float4_type{asm_::t_float, asm_::t_float, asm_::t_float, asm_::t_float};
	constexpr std::array<std::uint8_t, 4> uint4_type{asm_::t_uint, asm_::t_uint, asm_::t_uint, asm_::t_uint};

	using signature_element = alys::shader::shader_object::signature_element;

	void relocate_constant_row(alys::shader::detail::operand_t& operand, const std::uint32_t buffer, const std::uint32_t from, const std::uint32_t to,
		const std::optional<std::uint32_t> to_buffer = {});
	void redirect_input(alys::shader::detail::operand_t& operand, const std::uint32_t input, const std::uint32_t temp);
	bool is_declaration(const alys::shader::detail::instruction_t& instruction);
	bool is_input_declaration(const alys::shader::detail::instruction_t& instruction);
	bool is_destination(const alys::shader::detail::operand_t& operand);
	bool is_resource(const alys::shader::detail::operand_t& operand, const std::uint32_t texture);
	bool is_sample(const alys::shader::detail::instruction_t& instruction, const std::uint32_t texture);
	std::uint8_t read_components(const alys::shader::detail::operand_t& source, const std::uint8_t written, const std::uint32_t opcode);
	void set_resource_dimension(alys::shader::detail::instruction_t& instruction, const std::uint16_t dimension);
	std::uint32_t temp_component(const std::uint32_t temp, const std::uint32_t component);
	alys::shader::detail::operand_t uint_literal(const std::uint32_t value);
	alys::shader::detail::operand_t uint_literals(const std::uint32_t x, const std::uint32_t y, const std::uint32_t z, const std::uint32_t w);
	signature_element& find_signature_element(alys::shader::shader_object& shader, const std::uint32_t chunk,
		const std::function<bool(const signature_element&)>& accept, const char* what);
	bool declares_resource(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::uint32_t slot);
	bool declares_constant_buffer(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::uint32_t slot);
	bool declares_sampler(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::uint32_t slot);
	alys::shader::detail::operand_t as_temp(const alys::shader::detail::operand_t& operand, const std::uint32_t temp);
	bool writes_operand(const alys::shader::detail::instruction_t& instruction, const std::uint32_t type, const std::uint32_t index);
	alys::shader::detail::operand_t negate(alys::shader::detail::operand_t operand);
	bool is_input_ps_declaration(const alys::shader::detail::instruction_t& instruction);
	bool is_resource_declaration(const alys::shader::detail::instruction_t& instruction);
	void check_single_return(const std::vector<alys::shader::detail::instruction_t>& instructions, const char* what);
	void check_writes_only_o0(alys::shader::shader_object& shader, const char* what);
	void relocate_resource_slots(alys::shader::detail::instruction_t& instruction, const std::map<std::uint32_t, std::uint32_t>& slots);
}
