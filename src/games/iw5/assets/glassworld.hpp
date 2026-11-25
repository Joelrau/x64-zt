#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class glass_world
	{
	public:
		static void read(zone_reader32& reader, GlassWorld* asset);
	};
}
