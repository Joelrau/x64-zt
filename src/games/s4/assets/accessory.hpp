#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class accessory_def
	{
	public:
		static void read(zone_reader& reader, AccessoryDef* asset);
	};
}
