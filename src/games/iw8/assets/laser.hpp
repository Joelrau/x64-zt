#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class laser_def
	{
	public:
		static void read(zone_reader& reader, LaserDef* asset);
	};
}
