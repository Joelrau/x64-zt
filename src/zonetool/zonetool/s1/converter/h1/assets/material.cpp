#include <std_include.hpp>
#include "zonetool/s1/converter/h1/include.hpp"
#include "material.hpp"

#include "zonetool/h1/assets/material.hpp"

#include "techset.hpp"

#include <utils/string.hpp>

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace material
		{
			zonetool::h1::Material* convert(Material* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::Material>();

				REINTERPRET_CAST_SAFE(name);

				new_asset->info.gameFlags = asset->info.gameFlags; // convert?
				new_asset->info.sortKey = asset->info.sortKey; // convert?
				new_asset->info.textureAtlasRowCount = asset->info.textureAtlasRowCount;
				new_asset->info.textureAtlasColumnCount = asset->info.textureAtlasColumnCount;
				new_asset->info.textureAtlasFrameBlend = asset->info.textureAtlasFrameBlend;
				new_asset->info.textureAtlasAsArray = asset->info.textureAtlasAsArray;
				new_asset->info.renderFlags = asset->info.renderFlags; // convert?
				COPY_VALUE_CAST(info.drawSurf); // todo?
				COPY_VALUE_CAST(info.surfaceTypeBits); // todo?
				new_asset->info.hashIndex = asset->info.hashIndex;

				std::memset(new_asset->stateBitsEntry, 0xFF, zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT);
				std::memcpy(new_asset->stateBitsEntry, asset->stateBitsEntry, MaterialTechniqueType::TECHNIQUE_COUNT);

				new_asset->textureCount = asset->textureCount;
				new_asset->constantCount = asset->constantCount;
				new_asset->stateBitsCount = asset->stateBitsCount;
				new_asset->stateFlags = asset->stateFlags; // convert?
				new_asset->cameraRegion = asset->cameraRegion; // convert?
				new_asset->materialType = asset->materialType; // convert?
				new_asset->layerCount = asset->layerCount;
				new_asset->assetFlags = asset->assetFlags; // convert?

				new_asset->techniqueSet = allocator.allocate<zonetool::h1::MaterialTechniqueSet>();
				new_asset->techniqueSet->name = allocator.duplicate_string(game::add_source_postfix(asset->techniqueSet->name, game::s1));

				REINTERPRET_CAST_SAFE(textureTable);
				REINTERPRET_CAST_SAFE(constantTable);
				REINTERPRET_CAST_SAFE(stateBitsTable);

				std::memset(new_asset->constantBufferIndex, 0xFF, zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT);
				std::memcpy(new_asset->constantBufferIndex, asset->constantBufferIndex, MaterialTechniqueType::TECHNIQUE_COUNT);

				REINTERPRET_CAST_SAFE(constantBufferTable);
				new_asset->constantBufferCount = asset->constantBufferCount;

				REINTERPRET_CAST_SAFE(subMaterials);

				return new_asset;
			}

			void dump(Material* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::material::dump(converted_asset);
			}
		}
	}
}
