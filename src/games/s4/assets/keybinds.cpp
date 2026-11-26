#include <std_include.hpp>
#include "keybinds.hpp"

namespace zonetool::s4
{
	void key_binds::read(zone_reader& reader, KeyBinds* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto names = reader.read_array(asset->names, 7, asset->nameCount))
		{
			for (auto i = 0; i < asset->nameCount; i++)
			{
				reader.read_string(names[i].name);
			}
		}

		if (const auto commands = reader.read_array(asset->commands, 7, asset->commandCount))
		{
			for (auto i = 0; i < asset->commandCount; i++)
			{
				for (auto& string : commands[i].strings)
				{
					reader.read_string(string);
				}
			}
		}

		reader.pop_stream();
	}
}
