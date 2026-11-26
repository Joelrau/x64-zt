#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class loc_dmg_table
	{
	public:
		static void read(zone_reader& reader, LocDmgTable* asset);
	};
}
