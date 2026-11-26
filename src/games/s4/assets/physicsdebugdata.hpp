#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class physics_debug_data
	{
	public:
		static void read(zone_reader& reader, PhysicsDebugData* asset);
	};
}
