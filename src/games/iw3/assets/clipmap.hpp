#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class clip_map
	{
	public:
		static void read(zone_reader32& reader, clipMap_t* asset);
	};
}
