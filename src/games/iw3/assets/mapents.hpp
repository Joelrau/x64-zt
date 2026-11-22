#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class map_ents
	{
	public:
		static void read(zone_reader32& reader, MapEnts* asset);
		static void dump(MapEnts* asset);
	};
}
