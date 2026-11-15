#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	void read_lightset_probe_data(zone_reader& reader, GfxDynamicLightsetProbeData& data);

	class gfx_world_transient_zone
	{
	public:
		static void read(zone_reader& reader, GfxWorldTransientZone* asset);
	};
}
