#include <std_include.hpp>
#include "camo.hpp"

namespace zonetool::iw8
{
	void camo::read(zone_reader& reader, Camo* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_script_string(asset->internalName);
		if (const auto defs = reader.read_array(asset->textureDefs, 7, asset->textureDefCount))
		{
			for (auto i = 0u; i < asset->textureDefCount; i++)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, defs[i].image);
			}
		}
		reader.pop_stream();
	}
}
