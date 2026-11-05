#pragma once
#include "../iw7.hpp"

namespace zonetool::iw7
{
	class virtual_leaderboard
	{
	public:
		static void read(zone_reader& reader, VirtualLeaderboardDef* asset);
	};
}
