#include <std_include.hpp>
#include "leaderboard.hpp"

namespace zonetool::iw7
{
	void leaderboard::read(zone_reader& reader, LeaderboardDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto columns = reader.read_array(asset->columns, 7, asset->columnCount))
		{
			for (auto i = 0; i < asset->columnCount; i++)
			{
				reader.read_string(columns[i].name);
				reader.read_string(columns[i].statName);
			}
		}
		reader.pop_stream();
	}
}
