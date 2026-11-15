#include <std_include.hpp>
#include "fogspline.hpp"

namespace zonetool::iw8
{
	void fog_spline::read(zone_reader& reader, GfxFogSpline* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image);
		reader.pop_stream();
	}
}
