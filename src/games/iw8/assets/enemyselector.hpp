#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class enemy_selector
	{
	public:
		static void read(zone_reader& reader, EnemySelector* asset);
	};
}
