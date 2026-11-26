#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class collision_tile
	{
	public:
		static void read(zone_reader& reader, CollisionTile* asset);
	};
}
