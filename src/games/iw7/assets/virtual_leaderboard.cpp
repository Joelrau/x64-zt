#include <std_include.hpp>
#include "virtual_leaderboard.hpp"

namespace zonetool::iw7
{
	void virtual_leaderboard::read(zone_reader& reader, VirtualLeaderboardDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->sourceName);
		if (const auto columns = reader.read_array(asset->columns, 7, asset->columnCount))
		{
			for (auto i = 0; i < asset->columnCount; i++)
			{
				reader.read_string(columns[i].name);
			}
		}
		reader.pop_stream();
	}
}
