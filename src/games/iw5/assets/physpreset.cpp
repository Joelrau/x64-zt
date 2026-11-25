#include <std_include.hpp>
#include "physpreset.hpp"

namespace zonetool::iw5
{
	void phys_preset::read(zone_reader32& reader, PhysPreset* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->sndAliasPrefix);
		reader.pop_stream();
	}
}
