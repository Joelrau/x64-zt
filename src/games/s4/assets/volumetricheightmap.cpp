#include <std_include.hpp>
#include "volumetricheightmap.hpp"

namespace zonetool::s4
{
	void volumetric_heightmap::read(zone_reader& reader, VolumetricHeightmap* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		for (auto& image : asset->images)
		{
			reader.read_asset(ASSET_TYPE_IMAGE, image);
		}
		reader.pop_stream();
	}
}
