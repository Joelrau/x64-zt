#include <std_include.hpp>
#include "blendspace2d.hpp"

namespace zonetool::iw8
{
	void blend_space_2d::read(zone_reader& reader, BlendSpace2DDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto entries = reader.read_array(asset->animEntries, 7, asset->numAnimEntries))
		{
			for (auto i = 0u; i < asset->numAnimEntries; i++)
			{
				reader.read_asset(ASSET_TYPE_XANIMPARTS, entries[i].anim);
			}
		}

		reader.read_array(asset->triangleCoords, 3, asset->numTriangleCoords);
		reader.pop_stream();
	}
}
