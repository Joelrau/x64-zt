#include <std_include.hpp>
#include "lightdef.hpp"

namespace zonetool::iw8
{
	void light_def::read(zone_reader& reader, GfxLightDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->iesProfile);
		reader.pop_stream();
	}
}
