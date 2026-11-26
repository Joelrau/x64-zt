#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	void read_terrain_regions(auto& reader, TerrainRegionList* list);

	class terrain_gfx
	{
	public:
		static void read(auto& reader, TerrainGfx* asset);
	};
}
