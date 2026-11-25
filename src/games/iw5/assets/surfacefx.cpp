#include <std_include.hpp>
#include "surfacefx.hpp"

namespace zonetool::iw5
{
	void surface_fx::read(zone_reader32& reader, SurfaceFxTable* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto table = reader.read_array(asset->table, 3, 6))
		{
			for (auto i = 0; i < 6; i++)
			{
				for (auto& effect : table[i].surfaceEffect)
				{
					reader.read_asset(ASSET_TYPE_FX, effect);
				}
			}
		}
		reader.pop_stream();
	}
}
