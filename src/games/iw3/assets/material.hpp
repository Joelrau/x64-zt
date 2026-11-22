#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class material
	{
	public:
		static void read(zone_reader32& reader, Material* asset);
	};
}
