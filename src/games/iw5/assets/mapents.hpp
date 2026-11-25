#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class map_ents
	{
	public:
		static void read(zone_reader32& reader, MapEnts* asset);
		static void dump(MapEnts* asset);
	};
}
