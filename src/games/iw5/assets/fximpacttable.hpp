#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class impact_fx
	{
	public:
		static void read(zone_reader32& reader, FxImpactTable* asset);
	};
}
