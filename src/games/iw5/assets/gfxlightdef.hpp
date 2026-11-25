#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class gfx_light_def
	{
	public:
		static void read(zone_reader32& reader, GfxLightDef* asset);
	};
}
