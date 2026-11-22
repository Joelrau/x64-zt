#include <std_include.hpp>
#include "fximpacttable.hpp"

namespace zonetool::iw3
{
	void impact_fx::read(zone_reader32& reader, FxImpactTable* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto entries = reader.read_array(asset->table, 3, 12))
		{
			for (auto i = 0; i < 12; i++)
			{
				for (auto& effect : entries[i].nonflesh)
				{
					reader.read_asset(ASSET_TYPE_FX, effect);
				}

				for (auto& effect : entries[i].flesh)
				{
					reader.read_asset(ASSET_TYPE_FX, effect);
				}
			}
		}

		reader.pop_stream();
	}
}
