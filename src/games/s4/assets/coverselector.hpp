#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class cover_selector
	{
	public:
		static void read(zone_reader& reader, CoverSelector* asset);
	};
}
