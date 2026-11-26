#pragma once
#include "../s4.hpp"

namespace zonetool::s4
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
