#include <std_include.hpp>
#include "sightconfig.hpp"

namespace zonetool::s4
{
	void sight_config::read(zone_reader& reader, SightConfig* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		reader.read_script_string(asset->unknown0);

		if (const auto entries = reader.read_array(asset->entries, 7, asset->entryCount))
		{
			for (auto i = 0; i < asset->entryCount; i++)
			{
				auto& entry = entries[i];
				if (entry.type == 1)
				{
					reader.read_single(entry.data.type1.shape, 3);
				}
				else if (entry.type == 2)
				{
					reader.read_single(entry.data.type2.shape, 3);
					reader.read_array(entry.data.type2.points, 3, entry.data.type2.pointCount);
				}
			}
		}
		reader.pop_stream();
	}
}
