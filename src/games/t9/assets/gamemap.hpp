#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class game_map
	{
	public:
		static void read(auto& reader, GameMap* asset);
	};
}
