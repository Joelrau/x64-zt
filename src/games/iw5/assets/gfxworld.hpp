#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class gfx_world
	{
	public:
		static void read(zone_reader32& reader, GfxWorld* asset);
	};
}
