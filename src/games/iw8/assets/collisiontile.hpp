#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class collision_tile
	{
	public:
		static void read(zone_reader& reader, CollisionTile* asset);
	};
}
