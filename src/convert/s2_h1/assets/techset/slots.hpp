#pragma once

namespace zonetool::s2::converter::h1::techset
{
	std::int32_t s2_base_slot(std::int32_t h1_base);
	std::int32_t s2_group(std::int32_t h1_group);
	bool is_light_pass(std::int32_t h1_base);
	bool uses_lit_state(std::int32_t h1_base);
	std::int32_t find_lit_slot(const MaterialTechniqueSet* techset, std::int32_t s2_group_begin);
	std::int32_t find_source_slot(const MaterialTechniqueSet* techset, std::int32_t h1_slot);
	enum sun_mode_t : std::int32_t
	{
		SUN_MODE_NONE = 0,
		SUN_MODE_LIT = 1,
		SUN_MODE_SHADOW = 2,
		SUN_MODE_SHADOW_LIGHT_ARRAY = 3,
	};

	enum light_source_t : std::int32_t
	{
		LIGHT_SOURCE_NONE = 0,
		LIGHT_SOURCE_ARRAY = 1,
		LIGHT_SOURCE_SPOT = 2,
		LIGHT_SOURCE_SPOT_SHADOW = 3,
		LIGHT_SOURCE_OMNI = 4,
	};

	sun_mode_t sun_mode(std::int32_t h1_slot);
	light_source_t light_source(std::int32_t h1_slot);
	float sun_mode_w(sun_mode_t mode);
}
