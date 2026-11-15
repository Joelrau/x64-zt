#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class loc_dmg_table
	{
	public:
		static void read(zone_reader& reader, LocDmgTable* asset);
	};
}
