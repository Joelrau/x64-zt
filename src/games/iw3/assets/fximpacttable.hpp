#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class impact_fx
	{
	public:
		static void read(zone_reader32& reader, FxImpactTable* asset);
	};
}
