#pragma once

#include "program_analysis.hpp"

namespace zonetool::s2::converter::h1::techset
{
	struct sun_shadow_request
	{
		std::uint32_t mask_texture;
		std::uint32_t object_row;
		std::uint32_t stable_row;
		std::set<std::uint32_t> reserved_samplers;
	};

	struct sun_shadow_slots
	{
		std::uint32_t shadow_texture;
		std::uint32_t translucent_texture;
		std::uint32_t compare_sampler;
		std::uint32_t linear_sampler;
	};

	constexpr auto sun_shadow_object_rows = 4u;
	constexpr auto sun_shadow_stable_rows = 13u;

	const std::vector<std::pair<std::uint32_t, std::int32_t>>& sun_shadow_stable_consts();

	sun_shadow_slots patch_sun_shadow(alys::shader::shader_object& shader, instruction_list& instructions, const sun_shadow_request& request);
}
