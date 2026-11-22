#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class gfx_world
	{
	public:
		static void read(zone_reader32& reader, GfxWorld* asset);
	};
}
