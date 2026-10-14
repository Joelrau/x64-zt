#pragma once

namespace zonetool::s2
{
	class leaderboard
	{
	public:
		static void read(zone_reader& reader, LeaderboardDef* asset)
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
	};

	class virtual_leaderboard
	{
	public:
		static void read(zone_reader& reader, VirtualLeaderboardDef* asset)
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
	};
}
