#include <std_include.hpp>
#include "surface_fx.hpp"

namespace zonetool::iw7
{
	void surface_fx::read(zone_reader& reader, SurfaceFxTable* asset)
	{
		constexpr auto entry_count = 6;

		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto table = reader.read_array(asset->table, 7, entry_count))
		{
			for (auto i = 0; i < entry_count; i++)
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
