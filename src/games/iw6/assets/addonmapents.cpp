#include <std_include.hpp>
#include "addonmapents.hpp"

namespace zonetool::iw6
{
	void addon_map_ents::read(zone_reader& reader, AddonMapEnts* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->entityString, 0, asset->numEntityChars);
		map_ents::read_triggers(reader, &asset->trigger);

		if (const auto info = reader.read_single(asset->info, 3))
		{
			clip_map::read_info(reader, info);
		}

		if (const auto models = reader.read_array(asset->cmodels, 3, asset->numSubModels))
		{
			for (auto i = 0u; i < asset->numSubModels; i++)
			{
				if (const auto info = reader.read_single(models[i].info, 3))
				{
					clip_map::read_info(reader, info);
				}
			}
		}

		reader.read_array(asset->models, 3, asset->numSubModels);
		reader.pop_stream();
	}
}
