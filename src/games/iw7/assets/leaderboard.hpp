#pragma once
#include "../iw7.hpp"

namespace zonetool::iw7
{
	class leaderboard
	{
	public:
		static void read(zone_reader& reader, LeaderboardDef* asset);
	};
}
