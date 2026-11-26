#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class light_state
	{
	public:
		static void read(zone_reader& reader, LightState* asset);
	};
}
