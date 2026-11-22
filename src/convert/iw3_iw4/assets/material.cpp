#include <std_include.hpp>

#include "../include.hpp"
#include "material.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace material
	{
		namespace
		{
			const std::unordered_map<unsigned char, unsigned char> sort_keys =
			{
				{0, 43},
				{3, 0},
				{4, 1},
				{5, 2},
				{6, 3},
				{7, 4},
				{8, 5},
				{9, 6},
				{10, 7},
				{11, 8},
				{12, 9},
				{13, 10},
				{14, 11},
				{15, 12},
				{24, 13},
				{29, 14},
				{30, 15},
				{31, 16},
				{32, 17},
				{33, 17}, // guess
				{34, 4}, // hair, matched to a german shepherd material
				{35, 19},
				{36, 20},
				{37, 33}, // guess
				{38, 24},
				{39, 25},
				{40, 44}, // guess
				{41, 45}, // guess
				{42, 46}, // guess
				{43, 29},
				{48, 48},
				{51, 9},
				{56, 49},
				{57, 50},
				{58, 51},
				{59, 53},
			};

			zonetool::iw4::GfxImage* image_reference(const zonetool::iw3::GfxImage* image, x86::allocator& allocator)
			{
				if (!image)
				{
					return nullptr;
				}

				const auto reference = allocator.allocate<zonetool::iw4::GfxImage>();
				reference->name.value = image->name.value;
				return reference;
			}
		}

		zonetool::iw4::Material* convert(zonetool::iw3::Material* asset, x86::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw4::Material>();

			new_asset->name.value = asset->info.name.value;
			new_asset->gameFlags = asset->info.gameFlags;

			const auto sort_key = sort_keys.find(asset->info.sortKey);
			if (sort_key == sort_keys.end())
			{
				ZONETOOL_ERROR("sort key %d of material %s has no iw4 counterpart", asset->info.sortKey, asset->info.name.get());
				new_asset->sortKey = asset->info.sortKey;
			}
			else
			{
				new_asset->sortKey = sort_key->second;
			}

			new_asset->textureAtlasRowCount = asset->info.textureAtlasRowCount;
			new_asset->textureAtlasColumnCount = asset->info.textureAtlasColumnCount;
			new_asset->surfaceTypeBits = asset->info.surfaceTypeBits;
			new_asset->hashIndex = asset->info.hashIndex;

			std::memset(new_asset->stateBitsEntry, 0xFF, sizeof(new_asset->stateBitsEntry));

			new_asset->textureCount = asset->textureCount;
			new_asset->constantCount = asset->constantCount;
			new_asset->stateBitsCount = asset->stateBitsCount;
			new_asset->stateFlags = asset->stateFlags;
			new_asset->cameraRegion = asset->cameraRegion >= zonetool::iw3::CAMERA_REGION_NONE
				? static_cast<char>(zonetool::iw4::CAMERA_REGION_NONE)
				: static_cast<char>(asset->cameraRegion);

			if (const auto techset = asset->techniqueSet.get())
			{
				const auto reference = allocator.allocate<zonetool::iw4::MaterialTechniqueSet>();
				reference->name.value = techset->name.value;
				new_asset->techniqueSet = reference;
			}

			if (const auto source = asset->textureTable.get())
			{
				const auto textures = allocator.allocate<zonetool::iw4::MaterialTextureDef>(asset->textureCount);
				static_assert(sizeof(zonetool::iw4::MaterialTextureDef) == sizeof(zonetool::iw3::MaterialTextureDef));
				for (auto i = 0; i < asset->textureCount; i++)
				{
					std::memcpy(&textures[i], &source[i], sizeof(textures[i]));

					if (source[i].semantic == 11) // water
					{
						const auto water = allocator.allocate<zonetool::iw4::water_t>();
						static_assert(sizeof(zonetool::iw4::water_t) == sizeof(zonetool::iw3::water_t));
						std::memcpy(water, source[i].u.water.get(), sizeof(*water));
						water->image = image_reference(source[i].u.water.get()->image.get(), allocator);
						textures[i].u.water = water;
					}
					else
					{
						textures[i].u.image = image_reference(source[i].u.image.get(), allocator);
					}
				}
				new_asset->textureTable = textures;
			}

			if (const auto source = asset->constantTable.get())
			{
				static_assert(sizeof(zonetool::iw4::MaterialConstantDef) == sizeof(zonetool::iw3::MaterialConstantDef));
				const auto constants = allocator.allocate<zonetool::iw4::MaterialConstantDef>(asset->constantCount);
				std::memcpy(constants, source, sizeof(zonetool::iw4::MaterialConstantDef) * asset->constantCount);
				new_asset->constantTable = constants;
			}

			return new_asset;
		}
	}
}
