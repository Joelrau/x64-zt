#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class player_anim
	{
	public:
		static void read(zone_reader& reader, PlayerAnim* asset);
	};
}
