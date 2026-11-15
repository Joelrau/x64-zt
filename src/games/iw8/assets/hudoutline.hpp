#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class hud_outline
	{
	public:
		static void read(zone_reader& reader, HudOutlineDef* asset);
	};
}
