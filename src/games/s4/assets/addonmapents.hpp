#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class addon_map_ents
	{
	public:
		static void read(zone_reader& reader, AddonMapEnts* asset);
	};
}
