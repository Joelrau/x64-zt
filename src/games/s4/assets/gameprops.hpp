#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class game_props
	{
	public:
		static void read(zone_reader& reader, GameProps* asset);
	};
}
