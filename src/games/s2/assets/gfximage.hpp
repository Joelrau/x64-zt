#pragma once

namespace zonetool::s2
{
	std::optional<std::string> read_image_stream(const XStreamFileKey& key);

	class gfx_image
	{
	public:
		static void read(zone_reader& reader, GfxImage* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);

			if (asset->semantic == TS_RESIDENT)
			{
				reader.read_array(asset->pixelData, 0, asset->dataLen1);
			}
			else
			{
				reader.push_stream(XFILE_BLOCK_TEMP);
				if (const auto data = reader.read_aliased_array(asset->pixelData, 0, asset->dataLen1))
				{
					asset->pixelData = reader.duplicate(data, asset->dataLen1);
				}
				reader.pop_stream();
			}

			reader.pop_stream();
		}
	};

	class clut
	{
	public:
		static void read(zone_reader& reader, Clut* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_array(asset->data, 0, 4 * asset->width * asset->height * asset->depth);
			reader.read_asset(ASSET_TYPE_IMAGE, asset->image);
			reader.read_string(asset->name);
			reader.pop_stream();
		}
	};
}
