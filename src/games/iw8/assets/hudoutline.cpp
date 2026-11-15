#include <std_include.hpp>
#include "hudoutline.hpp"

namespace zonetool::iw8
{
	void hud_outline::read(zone_reader& reader, HudOutlineDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}
}
