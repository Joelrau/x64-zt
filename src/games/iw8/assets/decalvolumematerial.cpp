#include <std_include.hpp>
#include "decalvolumematerial.hpp"

namespace zonetool::iw8
{
	void decal_volume_material::read(zone_reader& reader, GfxDecalVolumeMaterial* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		for (auto& channel : asset->channels)
		{
			reader.read_asset(ASSET_TYPE_IMAGE, channel);
		}
		reader.pop_stream();
	}
}
