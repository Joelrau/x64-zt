#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class clip_map
	{
	public:
		static void read(zone_reader32& reader, clipMap_t* asset);
	};
}
