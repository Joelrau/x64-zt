#include <std_include.hpp>
#include "clothasset.hpp"

namespace zonetool::iw8
{
	void cloth_asset::read(zone_reader& reader, ClothAsset* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->havokData, 15, asset->havokDataSize);

		if (const auto sets = reader.read_array(asset->boneMappingSets, 7, asset->boneMappingSetCount))
		{
			for (auto i = 0u; i < asset->boneMappingSetCount; i++)
			{
				if (const auto mappings = reader.read_array(sets[i].boneMappings, 7, sets[i].boneMappingCount))
				{
					for (auto j = 0u; j < sets[i].boneMappingCount; j++)
					{
						reader.read_script_string_array(mappings[j].scrTagNames, mappings[j].scrTagCount);
					}
				}
			}
		}

		reader.read_string(asset->animationStateName);

		if (const auto lods = reader.read_array(asset->simulationLods, 7, asset->simulationLodCount))
		{
			for (auto i = 0u; i < asset->simulationLodCount; i++)
			{
				reader.read_string(lods[i].stateName);
			}
		}

		if (const auto sounds = reader.read_array(asset->sounds, 7, asset->soundCount))
		{
			for (auto i = 0u; i < asset->soundCount; i++)
			{
				reader.read_string(sounds[i].soundAlias);
			}
		}
		reader.pop_stream();
	}
}
