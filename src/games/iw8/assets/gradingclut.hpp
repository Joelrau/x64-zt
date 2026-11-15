#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class grading_clut
	{
	public:
		static void read(zone_reader& reader, GfxGradingClut* asset);
	};
}
