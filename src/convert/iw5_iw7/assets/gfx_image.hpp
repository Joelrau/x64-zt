#pragma once
#include "../include.hpp"

namespace zonetool::iw5::converter::iw7
{
	namespace gfx_image
	{
		inline std::function<std::string(const std::string&)> read_file;

		std::uint8_t convert_semantic(std::uint8_t semantic);
		zonetool::iw7::GfxImage* convert(zonetool::iw5::GfxImage* asset, utils::memory::allocator& allocator);
		void dump(zonetool::iw5::GfxImage* asset);
	}
}
