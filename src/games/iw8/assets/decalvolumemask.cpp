#include <std_include.hpp>
#include "decalvolumemask.hpp"

namespace zonetool::iw8
{
	void decal_volume_mask::read(zone_reader& reader, GfxDecalVolumeMask* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->blendMap);
		reader.pop_stream();
	}
}
