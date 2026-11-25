#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class menu
	{
	public:
		static void read(zone_reader32& reader, menuDef_t* asset);
	};
}
