#include <std_include.hpp>

#include "zonetool/iw7/converter/h1/include.hpp"
#include "material.hpp"
#include "techset.hpp"
#include "zonetool/h1/assets/material.hpp"

#include "zonetool/iw7/functions.hpp"

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace material
		{
			namespace
			{
				// TODO
				std::unordered_map<std::uint8_t, std::uint8_t> mapped_camera_regions =
				{
					{12, 14},
				};

				// TODO
				std::uint8_t convert_sortkey(const std::uint8_t sort_key)
				{
					if (sort_key >= 38)
					{
						return sort_key + 1;
					}

					return sort_key;
				}

				std::uint8_t convert_camera_region(const std::uint8_t camera_region)
				{
					if (mapped_camera_regions.find(camera_region) != mapped_camera_regions.end())
					{
						return mapped_camera_regions[camera_region];
					}

					return camera_region;
				}
			}

			zonetool::h1::Material* convert(zonetool::iw7::Material* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::Material>();

				REINTERPRET_CAST_SAFE(name);

				new_asset->info.name = asset->info.name;
				new_asset->info.gameFlags = asset->info.gameFlags;
				new_asset->info.sortKey = convert_sortkey(asset->info.sortKey);
				new_asset->info.textureAtlasRowCount = asset->info.textureAtlasRowCount;
				new_asset->info.textureAtlasColumnCount = asset->info.textureAtlasColumnCount;
				new_asset->info.textureAtlasFrameBlend = asset->info.textureAtlasFrameBlend;
				new_asset->info.textureAtlasAsArray = asset->info.textureAtlasAsArray;
				new_asset->info.renderFlags = asset->info.renderFlags; // convert?
				COPY_VALUE_CAST(info.drawSurf);
				COPY_VALUE_CAST(info.surfaceTypeBits);
				new_asset->info.hashIndex = asset->info.hashIndex;

				// TODO
				const auto state_bits = converter::h1::techset::remap(asset->stateBitsEntry, allocator);
				std::memset(new_asset->stateBitsEntry, 0xFF, zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT);
				std::memcpy(new_asset->stateBitsEntry, state_bits, zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT);

				new_asset->textureCount = asset->textureCount;
				new_asset->constantCount = asset->constantCount;
				new_asset->stateBitsCount = asset->stateBitsCount;
				new_asset->stateFlags = asset->stateFlags;
				new_asset->cameraRegion = convert_camera_region(asset->cameraRegion); //
				new_asset->materialType = asset->materialType;
				new_asset->layerCount = asset->layerCount;
				new_asset->assetFlags = asset->assetFlags; // convert?

				// TODO: the technique array is now a ** instead of *[240], and you can use techniqueCount to get the max
				// means we should be able to convert
				// MaterialTechnique is the same (convert flags?), and MaterialPass should be pretty simple to fix up (just the MaterialShaderArgument, which is same)
				new_asset->techniqueSet = allocator.allocate<zonetool::h1::MaterialTechniqueSet>();
				new_asset->techniqueSet->name = allocator.duplicate_string(asset->techniqueSet->name);
				REINTERPRET_CAST_SAFE(textureTable);	// MaterialTextureDef is 1:1, just has union at end with .image to be used
				REINTERPRET_CAST_SAFE(constantTable);	// ^
				REINTERPRET_CAST_SAFE(stateBitsTable);	// TODO: almost the same, but depthStencilState is 14 on IW7, and 10 on H1

				const auto constant_buffer_index = converter::h1::techset::remap(asset->constantBufferIndex, allocator);
				std::memset(new_asset->constantBufferIndex, 0xFF, zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT);
				std::memcpy(new_asset->constantBufferIndex, constant_buffer_index, MaterialTechniqueType::TECHNIQUE_COUNT);

				REINTERPRET_CAST_SAFE(constantBufferTable); // MaterialConstantBufferDef is 1:1 :)
				new_asset->constantBufferCount = asset->constantBufferCount;

				REINTERPRET_CAST_SAFE(subMaterials);

				return new_asset;
			}

			void dump(zonetool::iw7::Material* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::material::dump(converted_asset);
			}
		}
	}
}
