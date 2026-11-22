#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class gfx_light_def
	{
	public:
		static void read(zone_reader32& reader, GfxLightDef* asset);
	};
}
