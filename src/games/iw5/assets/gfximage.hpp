#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class gfx_image
	{
	public:
		static void read(zone_reader32& reader, GfxImage* asset);
	};
}
