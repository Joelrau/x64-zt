#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class leaderboard_def
	{
	public:
		static void read(zone_reader& reader, LeaderboardDef* asset);
	};

	class virtual_leaderboard_def
	{
	public:
		static void read(zone_reader& reader, VirtualLeaderboardDef* asset);
	};
}
