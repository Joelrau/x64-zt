#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class impact_fx
	{
	public:
		static void read(zone_reader& reader, FxImpactTable* asset);
	};

	class surface_fx
	{
	public:
		static void read(zone_reader& reader, SurfaceFxTable* asset);
	};
}
