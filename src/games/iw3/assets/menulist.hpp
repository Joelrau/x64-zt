#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class menu_list
	{
	public:
		static void read(zone_reader32& reader, MenuList* asset);
	};
}
