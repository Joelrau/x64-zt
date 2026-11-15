#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class light_def
	{
	public:
		static void read(zone_reader& reader, GfxLightDef* asset);
	};
}
