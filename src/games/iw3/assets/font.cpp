#include <std_include.hpp>
#include "font.hpp"

namespace zonetool::iw3
{
	void font::read(zone_reader32& reader, Font_s* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->fontName);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->material);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->glowMaterial);
		reader.read_array(asset->glyphs, 3, asset->glyphCount);
		reader.pop_stream();
	}
}
