#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class font
	{
	public:
		static void read(zone_reader32& reader, Font_s* asset);
	};
}
