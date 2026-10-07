#pragma once
#include "../h2.hpp"

namespace zonetool::h2
{
	class addon_map_ents
	{
	public:
		static void read(zone_reader& reader, AddonMapEnts* asset);
		static void dump(AddonMapEnts* asset);
	};
}
