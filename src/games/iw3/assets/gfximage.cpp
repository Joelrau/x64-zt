#include <std_include.hpp>
#include "gfximage.hpp"

namespace zonetool::iw3
{
	void gfx_image::read(zone_reader32& reader, GfxImage* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		reader.push_stream(XFILE_BLOCK_TEMP);
		if (!reader.read_dynamic(asset->texture.loadDef, 3, offsetof(GfxImageLoadDef, data), [](const GfxImageLoadDef* load_def)
		{
			return offsetof(GfxImageLoadDef, data) + load_def->resourceSize;
		}, true))
		{
			asset->texture.loadDef = nullptr;
		}
		else
		{
			const auto load_def = asset->texture.loadDef.get();
			const auto size = offsetof(GfxImageLoadDef, data) + load_def->resourceSize;
			const auto persistent = static_cast<GfxImageLoadDef*>(x86::address_space::allocate(size, 4)); // the temp block is rewound after every image
			std::memcpy(persistent, load_def, size);
			asset->texture.loadDef = persistent;
		}
		reader.pop_stream();

		reader.pop_stream();
	}
}
