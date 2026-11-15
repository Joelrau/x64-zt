#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class fog_spline
	{
	public:
		static void read(zone_reader& reader, GfxFogSpline* asset);
	};
}
