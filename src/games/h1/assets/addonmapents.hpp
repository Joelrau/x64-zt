#pragma once
#include "../h1.hpp"

namespace zonetool::h1
{
	class addon_map_ents
	{
	public:
		static void read(zone_reader& reader, AddonMapEnts* asset);
	};
}
