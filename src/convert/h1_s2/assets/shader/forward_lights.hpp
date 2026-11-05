#pragma once

#include "dxbc.hpp"

namespace zonetool::h1::converter::s2::shader
{
	constexpr auto light_temp_count = 15u;

	constexpr auto h1_eye_position_semantic = 5u;
	constexpr auto h1_fog_transmittance_semantic = 25u;

	std::map<std::uint32_t, std::uint32_t> relocate_light_textures(const std::vector<alys::shader::detail::instruction_t>& instructions);
	void declare_forward_lights(alys::shader::assembler& a, const primary_lights& lights);
	std::uint8_t light_screen_mask(const primary_lights& lights);
	void emit_forward_light_loop_begin(alys::shader::assembler& a, const primary_lights& lights, const std::uint32_t acc, const std::uint32_t screen_input,
		const alys::shader::detail::operand_t& position);
	void emit_forward_light_loop_end(alys::shader::assembler& a, const std::uint32_t acc, const std::optional<std::uint32_t> lit);
}
