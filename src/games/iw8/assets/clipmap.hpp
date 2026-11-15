#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class clip_map
	{
	public:
		static void read(zone_reader& reader, clipMap_t* asset);
	};
}
