#pragma once
#include "../iw7.hpp"

namespace zonetool::iw7
{
	class player_anim_script
	{
	public:
		static void read(zone_reader& reader, PlayerAnimScript* asset);
	};
}
