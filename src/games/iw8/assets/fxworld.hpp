#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class fx_world
	{
	public:
		static void read(zone_reader& reader, FxWorld* asset);
	};

	class fx_world_transient_zone
	{
	public:
		static void read(zone_reader& reader, FxWorldTransientZone* asset);
	};
}
