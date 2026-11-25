#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class phys_collmap
	{
	public:
		static void read(zone_reader32& reader, PhysCollmap* asset);
	};
}
