#pragma once

#include "dxbc.hpp"

namespace zonetool::h1::converter::s2::shader
{
	enum class sprite_input
	{
		position,
		color,
		texcoord,
		normal,
		atlas_step,
	};

	constexpr auto sprite_temp_count = 8u;

	std::vector<std::pair<std::uint32_t, sprite_input>> take_sprite_inputs(alys::shader::shader_object::signature& inputs);
	void emit_sprite_fetch(alys::shader::assembler& a, const std::uint32_t temp, const bool atlas_blend);
}
