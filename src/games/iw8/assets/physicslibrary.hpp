#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class physics_library
	{
	public:
		static void read(zone_reader& reader, PhysicsLibrary* asset);
	};
}
