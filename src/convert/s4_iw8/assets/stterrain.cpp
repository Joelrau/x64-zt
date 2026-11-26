#include <std_include.hpp>

#include "../include.hpp"
#include "stterrain.hpp"
#include "gfximage.hpp"

#include <iw8/assets/stterrain.hpp>

namespace zonetool::s4::converter::iw8::stterrain
{
	namespace
	{
		namespace target = zonetool::iw8;
	}

	void dump(StTerrain* asset)
	{
		static_assert(sizeof(target::StTerrainMaps) == sizeof(StTerrainMaps));
		static_assert(sizeof(target::StClutterSamplePoints) == sizeof(StClutterSamplePoints));

		if (asset->surfaceCount)
		{
			ZONETOOL_WARNING("stterrain \"%s\" is lost, the converter has no terrain surfaces (%u)", asset->name, asset->surfaceCount);
			return;
		}

		utils::memory::allocator allocator;
		const auto terrain = allocator.allocate<target::StTerrain>();
		terrain->name = asset->name;
		terrain->mapsCount = asset->mapsCount;
		terrain->maps = reinterpret_cast<target::StTerrainMaps*>(asset->maps);

		terrain->flattenedImagesCount = asset->flattenedImagesCount;
		if (asset->flattenedImages && asset->flattenedImagesCount)
		{
			terrain->flattenedImages = allocator.allocate_array<target::GfxImage*>(asset->flattenedImagesCount);
			for (auto i = 0u; i < asset->flattenedImagesCount; i++)
			{
				terrain->flattenedImages[i] = gfximage::image_reference(asset->flattenedImages[i], allocator);
			}
		}

		std::memcpy(&terrain->clutterSamplePoints, &asset->clutterSamplePoints, sizeof(terrain->clutterSamplePoints));
		terrain->lightmapType = static_cast<target::GfxLightmapType>(asset->lightmapType);

		terrain->lightmapCount = asset->lightmapCount;
		if (asset->lightmaps && asset->lightmapCount)
		{
			terrain->lightmaps = allocator.allocate_array<target::StLightmap>(asset->lightmapCount);
			for (auto i = 0u; i < asset->lightmapCount; i++)
			{
				for (auto j = 0u; j < std::size(terrain->lightmaps[i].images); j++)
				{
					terrain->lightmaps[i].images[j] = gfximage::image_reference(asset->lightmaps[i].images[j], allocator);
				}
			}
		}

		target::st_terrain::dump(terrain);
		add_csv_row(target::type_to_string(target::ASSET_TYPE_ST_TERRAIN), asset->name);
	}
}
