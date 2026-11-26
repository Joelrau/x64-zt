#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class enemy_selector
	{
	public:
		static void read(zone_reader& reader, EnemySelector* asset);
	};
}
