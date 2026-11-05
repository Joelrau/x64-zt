#include <std_include.hpp>
#include "surface_fx.hpp"

namespace zonetool::iw7
{
	void surface_fx::read(zone_reader& reader, SurfaceFxTable* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto table = reader.read_array(asset->table, 7, 6))
		{
			for (auto i = 0; i < 6; i++)
			{
				for (auto& effect : table[i].surfaceEffect)
				{
					read_fx_combined(reader, effect);
				}
			}
		}
		reader.pop_stream();
	}
}
