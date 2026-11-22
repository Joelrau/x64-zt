#pragma once
#include "../include.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace gfx_image
	{
		zonetool::iw4::GfxImage* convert(zonetool::iw3::GfxImage* asset, x86::allocator& allocator);
	}
}
