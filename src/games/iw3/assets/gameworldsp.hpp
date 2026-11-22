#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class game_world_sp
	{
	public:
		static void read(zone_reader32& reader, GameWorldSp* asset);
	};
}
