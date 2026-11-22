#include <std_include.hpp>

#include "../include.hpp"
#include "material.hpp"

namespace zonetool::iw4::converter::iw5
{
	namespace material
	{
		namespace
		{
			zonetool::iw5::GfxImage* image_reference(const zonetool::iw4::GfxImage* image, x86::allocator& allocator)
			{
				if (!image)
				{
					return nullptr;
				}

				const auto reference = allocator.allocate<zonetool::iw5::GfxImage>();
				reference->name.value = image->name.value;
				return reference;
			}
		}

		zonetool::iw5::Material* convert(zonetool::iw4::Material* asset, x86::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw5::Material>();

			new_asset->info.name.value = asset->name.value;
			new_asset->info.gameFlags = asset->gameFlags;
			new_asset->info.sortKey = asset->sortKey;
			new_asset->info.textureAtlasRowCount = asset->textureAtlasRowCount;
			new_asset->info.textureAtlasColumnCount = asset->textureAtlasColumnCount;
			new_asset->info.surfaceTypeBits = asset->surfaceTypeBits;

			std::memset(new_asset->stateBitsEntry, 0xFF, sizeof(new_asset->stateBitsEntry));

			new_asset->textureCount = asset->textureCount;
			new_asset->constantCount = asset->constantCount;
			new_asset->stateBitsCount = asset->stateBitsCount;
			new_asset->stateFlags = asset->stateFlags;
			new_asset->cameraRegion = asset->cameraRegion;

			if (const auto techset = asset->techniqueSet.get())
			{
				const auto reference = allocator.allocate<zonetool::iw5::MaterialTechniqueSet>();
				reference->name.value = techset->name.value;
				new_asset->techniqueSet = reference;
			}

			if (const auto source = asset->textureTable.get())
			{
				const auto textures = allocator.allocate<zonetool::iw5::MaterialTextureDef>(asset->textureCount);
				for (auto i = 0; i < asset->textureCount; i++)
				{
					std::memcpy(&textures[i], &source[i], offsetof(zonetool::iw5::MaterialTextureDef, u));

					if (source[i].semantic == 11) // water
					{
						const auto water = allocator.allocate<zonetool::iw5::water_t>();
						static_assert(sizeof(zonetool::iw5::water_t) == sizeof(zonetool::iw4::water_t));
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
				static_assert(sizeof(zonetool::iw5::MaterialConstantDef) == sizeof(zonetool::iw4::MaterialConstantDef));
				const auto constants = allocator.allocate<zonetool::iw5::MaterialConstantDef>(asset->constantCount);
				std::memcpy(constants, source, sizeof(zonetool::iw5::MaterialConstantDef) * asset->constantCount);
				new_asset->constantTable = constants;
			}

			return new_asset;
		}
	}
}
