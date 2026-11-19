#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class accessory_def
	{
	public:
		static void read(zone_reader& reader, AccessoryDef* asset);
	};
}
