#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	void read_map_triggers(zone_reader& reader, MapTriggers& triggers);

	class map_ents
	{
	public:
		static void read(zone_reader& reader, MapEnts* asset);
	};
}
