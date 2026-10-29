#pragma once
#include "../h1.hpp"

namespace zonetool::h1
{
	class vehicle_track
	{
	public:
		static void read(zone_reader& reader, VehicleTrack* asset);
	};
}
