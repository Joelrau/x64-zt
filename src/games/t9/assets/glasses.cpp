#include <std_include.hpp>
#include "glasses.hpp"

namespace zonetool::t9
{
	void glasses::read(auto& reader, Glasses* asset)
	{
		if (const auto entries = reader.read_array(asset->entries, 7, asset->entryCount))
		{
			for (auto i = 0u; i < asset->entryCount; i++)
			{
				if (const auto def = reader.read_array(entries[i].def, 7))
				{
					for (auto& material : def->materials)
					{
						reader.read_asset(ASSET_TYPE_MATERIAL, material);
					}

					for (auto& effect : def->effects)
					{
						reader.read_asset(ASSET_TYPE_FX, effect);
					}
				}

				reader.read_array(entries[i].values, 3, entries[i].valueCount);
			}
		}

		reader.push_stream(XFILE_BLOCK_MEMMAPPED);
		reader.read_array(asset->data, 0, asset->dataSize);
		reader.pop_stream();
	}
}

namespace zonetool::t9
{
	template void glasses::read(zone_reader&, Glasses*);
	template void glasses::read(zone_writer&, Glasses*);
}
