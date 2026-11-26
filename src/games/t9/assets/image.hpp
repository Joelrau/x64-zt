#pragma once
#include "../t9.hpp"

#pragma warning( push )
#pragma warning( disable : 4459 )
#include <DirectXTex.h>
#pragma warning( pop )

namespace zonetool::t9
{
	class image
	{
	public:
		static void read(auto& reader, GfxImage* asset);
		static void dump(GfxImage* asset);

		static bool is_placeholder(const GfxImage* asset)
		{
			return !asset->format;
		}
	};

	std::uint32_t stream_part_size(const GfxImage* asset, std::uint32_t part);
	std::string read_image_pixels(const GfxImage* asset);
	std::optional<DirectX::ScratchImage> load_image(const GfxImage* asset);
}
