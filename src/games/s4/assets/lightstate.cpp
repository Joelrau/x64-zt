#include <std_include.hpp>
#include "lightstate.hpp"

namespace zonetool::s4
{
	void light_state::read(zone_reader& reader, LightState* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		for (auto& string : asset->strings)
		{
			reader.read_string(string);
		}
		reader.pop_stream();
	}
}
