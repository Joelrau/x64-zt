#include <std_include.hpp>
#include "material.hpp"

namespace zonetool::iw3
{
	namespace
	{
		void read_water(zone_reader32& reader, water_t* water)
		{
			const auto count = static_cast<std::size_t>(water->N) * water->M;
			reader.read_array(water->H0, 3, count);
			reader.read_array(water->wTerm, 3, count);
			reader.read_asset(ASSET_TYPE_IMAGE, water->image);
		}
	}

	void material::read(zone_reader32& reader, Material* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->info.name);
		reader.read_asset(ASSET_TYPE_TECHNIQUE_SET, asset->techniqueSet);

		if (const auto textures = reader.read_array(asset->textureTable, 3, asset->textureCount))
		{
			for (auto i = 0; i < asset->textureCount; i++)
			{
				if (textures[i].semantic == TS_WATER_MAP)
				{
					if (const auto water = reader.read_single(textures[i].u.water, 3))
					{
						read_water(reader, water);
					}
				}
				else
				{
					reader.read_asset(ASSET_TYPE_IMAGE, textures[i].u.image);
				}
			}
		}

		reader.read_array(asset->constantTable, 15, asset->constantCount);
		reader.read_array(asset->stateBitsTable, 3, asset->stateBitsCount);
		reader.pop_stream();
	}
}
