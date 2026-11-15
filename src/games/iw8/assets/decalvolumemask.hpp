#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class decal_volume_mask
	{
	public:
		static void read(zone_reader& reader, GfxDecalVolumeMask* asset);
	};
}
