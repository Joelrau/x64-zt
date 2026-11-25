#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class surface_fx
	{
	public:
		static void read(zone_reader32& reader, SurfaceFxTable* asset);
	};
}
