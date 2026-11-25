#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class material
	{
	public:
		static void read(zone_reader32& reader, Material* asset);
	};
}
