#include <std_include.hpp>
#include "comworld.hpp"

namespace zonetool::iw8
{
	void com_world::read(zone_reader& reader, ComWorld* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto lights = reader.read_array(asset->primaryLights, 7, asset->primaryLightCount))
		{
			for (auto i = 0u; i < asset->primaryLightCount; i++)
			{
				reader.read_string(lights[i].defName);
			}
		}

		reader.read_array(asset->transientTable, 1, asset->transientTableSize);
		reader.read_string(asset->changeListInfo.userName);

		if (const auto names = reader.read_array(asset->umbraGateNames, 7, asset->numUmbraGates))
		{
			for (auto i = 0u; i < asset->numUmbraGates; i++)
			{
				reader.read_string(names[i]);
			}
		}
		reader.pop_stream();
	}
}
