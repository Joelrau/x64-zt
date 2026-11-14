#pragma once
#include "../iw7.hpp"

namespace zonetool::iw7
{
	class spaceship_target
	{
	public:
		static void read(zone_reader& reader, SpaceshipTargetDef* asset);
	};
}
