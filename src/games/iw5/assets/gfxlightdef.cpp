#include <std_include.hpp>
#include "gfxlightdef.hpp"

namespace zonetool::iw5
{
	void gfx_light_def::read(zone_reader32& reader, GfxLightDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->attenuation.image);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->cucoloris.image);
		reader.pop_stream();
	}
}
