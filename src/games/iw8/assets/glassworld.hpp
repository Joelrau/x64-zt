#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	void read_spatial_tree(zone_reader& reader, SpatialPartition_Tree*& field);

	class glass_world
	{
	public:
		static void read(zone_reader& reader, GlassWorld* asset);
	};
}
