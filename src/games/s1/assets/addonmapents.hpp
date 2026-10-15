#pragma once
#include "../s1.hpp"

namespace zonetool::s1
{
	class addon_map_ents
	{
	public:
		static void read(zone_reader& reader, AddonMapEnts* asset);
	};
}
