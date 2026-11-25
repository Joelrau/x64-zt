#include <std_include.hpp>
#include "comworld.hpp"

namespace zonetool::iw5
{
	void com_world::read(zone_reader32& reader, ComWorld* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto lights = reader.read_array(asset->primaryLights, 3, asset->primaryLightCount))
		{
			for (auto i = 0u; i < asset->primaryLightCount; i++)
			{
				reader.read_string(lights[i].defName);
			}
		}

		reader.pop_stream();
	}
}
