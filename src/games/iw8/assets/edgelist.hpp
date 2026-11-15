#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class map_edge_list
	{
	public:
		static void read(zone_reader& reader, MapEdgeList* asset);
	};
}
