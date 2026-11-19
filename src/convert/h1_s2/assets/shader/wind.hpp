#pragma once

#include "dxbc.hpp"

namespace zonetool::h1::converter::s2::shader
{
	constexpr auto wind_temp_count = 5u;

	struct wind_inputs
	{
		std::uint32_t color;
		std::uint32_t normal;
		bool declare_normal;
	};

	wind_inputs add_wind_inputs(alys::shader::shader_object::signature& inputs);
	std::unordered_map<std::uint32_t, std::uint32_t> order_wind_inputs(alys::shader::shader_object::signature& inputs, const bool added_normal);
	void remap_input_registers(alys::shader::detail::operand_t& operand, const std::unordered_map<std::uint32_t, std::uint32_t>& registers);
	bool reads_reactive_motion(const alys::shader::detail::instruction_t& instruction);
	void emit_foliage_wind(alys::shader::assembler& a, const alys::shader::detail::operand_t& dest, const std::uint32_t position, const wind_inputs& inputs,
		const foliage_wind& wind, const std::array<alys::shader::detail::operand_t, 3>& world, const std::uint32_t temp);
}
