#pragma once
#include "../iw7.hpp"

namespace zonetool::iw7
{
	class surface_fx
	{
	public:
		static void read(zone_reader& reader, SurfaceFxTable* asset);
	};
}
