#pragma once

namespace zonetool::s2
{
	class phys_collmap
	{
	public:
		static void read_polytope(zone_reader& reader, dmPolytopeData* data);
		static void read(zone_reader& reader, PhysCollmap* asset);
	};

	class phys_worldmap
	{
	public:
		static void read(zone_reader& reader, PhysWorld* asset);
	};
}
