#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class league_play_seasons
	{
	public:
		static void read(zone_reader& reader, LeaguePlaySeasons* asset);
	};
}
