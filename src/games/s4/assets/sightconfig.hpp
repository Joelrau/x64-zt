#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class sight_config
	{
	public:
		static void read(zone_reader& reader, SightConfig* asset);
	};
}
