#pragma once
#include "../iw6.hpp"

namespace zonetool::iw6
{
	class vehicle_track
	{
	public:
		static void read(zone_reader& reader, VehicleTrack* asset);
	};
}
