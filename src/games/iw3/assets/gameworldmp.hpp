#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class game_world_mp
	{
	public:
		static void read(zone_reader32& reader, GameWorldMp* asset);
	};
}
