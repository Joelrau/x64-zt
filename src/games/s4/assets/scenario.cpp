#include <std_include.hpp>
#include "scenario.hpp"

namespace zonetool::s4
{
	void scenario::read(zone_reader& reader, Scenario* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto entries = reader.read_array(asset->unknown0, 7, asset->unknown0Count))
		{
			for (auto i = 0; i < asset->unknown0Count; i++)
			{
				reader.read_string(entries[i].name);
			}
		}

		if (const auto entries = reader.read_array(asset->unknown1, 7, asset->unknown1Count))
		{
			for (auto i = 0; i < asset->unknown1Count; i++)
			{
				reader.read_string(entries[i].name0);
				reader.read_string(entries[i].name1);
				reader.read_array(entries[i].hashes, 7, entries[i].hashCount);
			}
		}

		reader.read_array(asset->unknown2, 7, asset->unknown2Count);
		reader.pop_stream();
	}
}
