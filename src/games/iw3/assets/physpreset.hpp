#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class phys_preset
	{
	public:
		static void read(zone_reader32& reader, PhysPreset* asset);
	};
}
