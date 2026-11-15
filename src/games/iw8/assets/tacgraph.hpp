#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class tactical_graph
	{
	public:
		static void read(zone_reader& reader, TacticalGraphData* asset);
	};
}
