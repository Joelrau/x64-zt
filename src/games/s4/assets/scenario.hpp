#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class scenario
	{
	public:
		static void read(zone_reader& reader, Scenario* asset);
	};
}
