#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class st_terrain
	{
	public:
		static void read(zone_reader& reader, StTerrain* asset);
	};
}
