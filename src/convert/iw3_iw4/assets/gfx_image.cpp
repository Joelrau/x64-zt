#include <std_include.hpp>

#include "../include.hpp"
#include "gfx_image.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace gfx_image
	{
		zonetool::iw4::GfxImage* convert(zonetool::iw3::GfxImage* asset, x86::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw4::GfxImage>();
			const auto load_def = asset->texture.loadDef.get();

			new_asset->mapType = static_cast<unsigned char>(asset->mapType);
			new_asset->semantic = static_cast<unsigned char>(asset->semantic);
			new_asset->category = static_cast<unsigned char>(asset->category);
			new_asset->cardMemory.platform[0] = asset->cardMemory.platform[0];
			new_asset->cardMemory.platform[1] = asset->cardMemory.platform[1];
			new_asset->width = asset->width;
			new_asset->height = asset->height;
			new_asset->depth = asset->depth;
			new_asset->delayLoadPixels = true;
			new_asset->name.value = asset->name.value;

			if (!load_def)
			{
				return new_asset;
			}

			const auto header_size = offsetof(zonetool::iw4::GfxImageLoadDef, data);
			const auto new_load_def = allocator.manual_allocate<zonetool::iw4::GfxImageLoadDef>(header_size + load_def->resourceSize);
			new_load_def->levelCount = load_def->levelCount;
			new_load_def->flags = load_def->flags;
			new_load_def->format = load_def->format;
			new_load_def->resourceSize = static_cast<int>(load_def->resourceSize);
			std::memcpy(new_load_def->data, load_def->data, load_def->resourceSize);
			new_asset->texture = new_load_def;

			return new_asset;
		}
	}
}
