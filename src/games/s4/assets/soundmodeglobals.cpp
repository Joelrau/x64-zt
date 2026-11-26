#include <std_include.hpp>
#include "soundmodeglobals.hpp"

namespace zonetool::s4
{
	void sound_mode_globals::read(zone_reader& reader, SndModeGlobals* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto entries = reader.read_array(asset->entries, 7, asset->count))
		{
			for (auto i = 0u; i < asset->count; i++)
			{
				auto& entry = entries[i];
				for (auto& string : entry.strings)
				{
					reader.read_string(string);
				}

				if (const auto sub_entries = reader.read_array(entry.subEntries, 7, entry.count))
				{
					for (auto j = 0u; j < entry.count; j++)
					{
						auto& sub_entry = sub_entries[j];
						reader.read_string(sub_entry.name);
						if (const auto strings = reader.read_array(sub_entry.strings, 7, sub_entry.count))
						{
							for (auto k = 0u; k < sub_entry.count; k++)
							{
								reader.read_string(strings[k]);
							}
						}

						reader.read_array(sub_entry.aliases, 7, sub_entry.count);
					}
				}

				reader.read_array(entry.aliases, 7, entry.count);
				reader.read_array(entry.values, 3, entry.count);
			}
		}

		reader.read_array(asset->aliases, 7, asset->count);
		reader.pop_stream();
	}
}
