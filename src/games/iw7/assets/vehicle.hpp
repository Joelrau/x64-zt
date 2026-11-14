#pragma once
#include "../iw7.hpp"

namespace zonetool::iw7
{
	class vehicle
	{
	public:
		static void read(zone_reader& reader, VehicleDef* asset);
	};
}
