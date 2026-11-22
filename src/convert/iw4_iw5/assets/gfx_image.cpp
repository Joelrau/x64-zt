#include <std_include.hpp>

#include "../include.hpp"
#include "gfx_image.hpp"

namespace zonetool::iw4::converter::iw5
{
	namespace gfx_image
	{
		zonetool::iw5::GfxImage* convert(zonetool::iw4::GfxImage* asset)
		{
			static_assert(sizeof(zonetool::iw4::GfxImage) == sizeof(zonetool::iw5::GfxImage));
			static_assert(sizeof(zonetool::iw4::GfxImageLoadDef) == sizeof(zonetool::iw5::GfxImageLoadDef));
			return reinterpret_cast<zonetool::iw5::GfxImage*>(asset);
		}
	}
}
