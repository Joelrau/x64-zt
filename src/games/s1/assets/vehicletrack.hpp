#pragma once
#include "../s1.hpp"

namespace zonetool::s1
{
	class vehicle_track
	{
	public:
		static void read(zone_reader& reader, VehicleTrack* asset);
	};
}
