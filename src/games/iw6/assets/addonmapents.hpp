#pragma once
#include "../iw6.hpp"

namespace zonetool::iw6
{
	class addon_map_ents
	{
	public:
		static void read(zone_reader& reader, AddonMapEnts* asset);
	};
}
