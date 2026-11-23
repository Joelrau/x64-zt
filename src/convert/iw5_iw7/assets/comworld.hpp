#pragma once
#include "../include.hpp"

namespace zonetool::iw5::converter::iw7
{
	namespace comworld
	{
		struct light_reach
		{
			unsigned char type;
			bool can_use_shadow_map;
			float origin[3];
			float dir[3];
			float radius;
			float cos_half_fov_outer;
		};

		const std::vector<light_reach>& last_primary_lights();

		zonetool::iw7::ComWorld* convert(zonetool::iw5::ComWorld* asset, utils::memory::allocator& allocator);
		void dump(zonetool::iw5::ComWorld* asset);
	}
}
