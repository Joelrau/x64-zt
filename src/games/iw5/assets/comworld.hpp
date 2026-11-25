#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class com_world
	{
	public:
		static void read(zone_reader32& reader, ComWorld* asset);
	};
}
