#pragma once

namespace zonetool::s2
{
	class map_ents
	{
	public:
		static void read(zone_reader& reader, MapEnts* asset);
	};

	class addon_map_ents
	{
	public:
		static void read(zone_reader& reader, AddonMapEnts* asset);
	};
}
