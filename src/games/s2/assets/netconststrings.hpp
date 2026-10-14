#pragma once

namespace zonetool::s2
{
	class net_const_strings
	{
	public:
		static void read(zone_reader& reader, NetConstStrings* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto strings = reader.read_array(asset->stringList, 7, asset->entryCount))
			{
				for (auto i = 0u; i < asset->entryCount; i++)
				{
					reader.read_string(strings[i]);
				}
			}
			reader.pop_stream();
		}
	};
}
