#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class vehicle
	{
	public:
		static void read(zone_reader& reader, VehicleDef* asset);
	};
}
