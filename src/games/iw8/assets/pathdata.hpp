#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class path_data
	{
	public:
		static void read(zone_reader& reader, PathData* asset);
	};
}
