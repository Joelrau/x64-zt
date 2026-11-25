#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class menu_list
	{
	public:
		static void read(zone_reader32& reader, MenuList* asset);
	};
}
