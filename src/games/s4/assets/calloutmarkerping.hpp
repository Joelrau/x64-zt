#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class callout_marker_ping
	{
	public:
		static void read(zone_reader& reader, CalloutMarkerPing* asset);
	};
}
