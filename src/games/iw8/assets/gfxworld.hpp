#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class gfx_world
	{
	public:
		static void read(zone_reader& reader, GfxWorld* asset);
	};
}
