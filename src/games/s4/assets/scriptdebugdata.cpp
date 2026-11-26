#include <std_include.hpp>
#include "scriptdebugdata.hpp"

namespace zonetool::s4
{
	void script_debug_data::read(zone_reader& reader, ScriptDebugData* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto strings = reader.read_array(asset->strings, 7, asset->stringCount))
		{
			for (auto i = 0; i < asset->stringCount; i++)
			{
				reader.read_string(strings[i]);
			}
		}
		reader.pop_stream();
	}
}
