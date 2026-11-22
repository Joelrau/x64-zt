#pragma once

#include "program_analysis.hpp"
#include "slots.hpp"

namespace zonetool::s2::converter::h1::techset
{
	struct light_array_request
	{
		std::uint32_t object_row;
		std::optional<std::pair<std::uint32_t, std::uint32_t>> grid_params;
		bool sun_first;
		std::set<std::uint32_t> reserved_samplers;
		light_source_t source;
	};

	struct light_array_const
	{
		std::uint32_t row;
		std::int32_t source;
		std::uint8_t rows;
	};

	struct light_array_shadow_slots
	{
		std::array<std::optional<std::uint32_t>, 4> textures;
		std::uint32_t compare_sampler;
	};

	struct light_array_result
	{
		bool fed;
		std::optional<light_array_shadow_slots> shadows;
	};

	constexpr auto light_array_object_rows = 47u;

	const std::vector<light_array_const>& light_array_object_consts(light_source_t source);

	light_array_result feed_light_array(alys::shader::shader_object& shader, instruction_list& instructions, const light_array_request& request);
}
