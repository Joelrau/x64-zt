#pragma once
#include "../h2.hpp"

namespace zonetool::h2
{
	class vehicle_track
	{
	public:
		static void read(zone_reader& reader, VehicleTrack* asset);
	};
}
