#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class radiant_telemetry
	{
	public:
		static void read(zone_reader& reader, RadiantTelemetry* asset);
	};
}
