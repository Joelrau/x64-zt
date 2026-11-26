#include <std_include.hpp>
#include "addonmapents.hpp"
#include "mapents.hpp"

namespace zonetool::s4
{
	void addon_map_ents::read(zone_reader& reader, AddonMapEnts* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		reader.read_array(asset->entityString, 0, asset->numEntityChars);
		walk_map_triggers(read_walk{reader}, asset->trigger);

		if (const auto cmodels = reader.read_array(asset->cmodels, 7, asset->numSubModels))
		{
			for (auto i = 0u; i < asset->numSubModels; i++)
			{
				reader.read_asset(ASSET_TYPE_PHYSICSASSET, cmodels[i].physicsAsset);
			}
		}

		reader.read_array(asset->models, 3, asset->numSubModels);
		reader.pop_stream();
	}
}
