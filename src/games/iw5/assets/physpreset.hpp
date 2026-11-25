#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class phys_preset
	{
	public:
		static void read(zone_reader32& reader, PhysPreset* asset);
	};
}
