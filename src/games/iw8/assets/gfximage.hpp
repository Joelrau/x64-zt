#pragma once
#include "../iw8.hpp"

#include <dxgiformat.h>

namespace zonetool::iw8
{
	class gfx_image
	{
	public:
		static void read(zone_reader& reader, GfxImage* asset);
		static void dump(GfxImage* asset);
	};

	std::optional<std::string> get_image_pixels(const GfxImage* image);
	std::optional<std::string> get_image_stream_pixels(const GfxImage* image, int stream);
	DXGI_FORMAT get_dxgi_format(GfxPixelFormat format);
	std::uint32_t get_image_item_count(const GfxImage* image);
	std::optional<std::string> remove_level_padding(const GfxImage* image, std::string_view pixels, std::uint32_t first_level, std::uint32_t level_count);
}
