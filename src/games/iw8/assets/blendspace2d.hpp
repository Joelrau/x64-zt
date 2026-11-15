#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class blend_space_2d
	{
	public:
		static void read(zone_reader& reader, BlendSpace2DDef* asset);
	};
}
