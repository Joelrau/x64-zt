#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class water_buoyancy
	{
	public:
		static void read(zone_reader& reader, WaterBuoyancy* asset);
	};
}
