#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class com_world
	{
	public:
		static void read(zone_reader& reader, ComWorld* asset);
	};
}
