#include <std_include.hpp>
#include "gfximage.hpp"

namespace zonetool::iw5
{
	void gfx_image::read(zone_reader32& reader, GfxImage* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		reader.push_stream(XFILE_BLOCK_TEMP);
		const auto load_def = reader.read_dynamic(asset->texture.loadDef, 3, offsetof(GfxImageLoadDef, data), [](const GfxImageLoadDef* data)
		{
			return offsetof(GfxImageLoadDef, data) + data->resourceSize;
		}, true);

		if (load_def)
		{
			const auto size = offsetof(GfxImageLoadDef, data) + load_def->resourceSize;
			const auto persistent = static_cast<GfxImageLoadDef*>(x86::address_space::allocate(size, 4)); // the temp block is rewound after every image
			std::memcpy(persistent, load_def, size);
			asset->texture.loadDef = persistent;
		}
		else
		{
			asset->texture.loadDef = nullptr;
		}
		reader.pop_stream();

		reader.pop_stream();
	}
}
