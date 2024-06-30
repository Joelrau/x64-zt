#pragma once

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace gfximage
		{
			zonetool::iw7::GfxImage* convert(GfxImage* asset, utils::memory::allocator& allocator);
			void dump(GfxImage* asset);
		}
	}
}