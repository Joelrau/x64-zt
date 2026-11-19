#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class cover_selector
	{
	public:
		static void read(zone_reader& reader, CoverSelector* asset);
	};
}
