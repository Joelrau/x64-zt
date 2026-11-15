#include <std_include.hpp>
#include "gradingclut.hpp"

namespace zonetool::iw8
{
	void grading_clut::read(zone_reader& reader, GfxGradingClut* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->hdrClutImage);
		reader.pop_stream();
	}
}
