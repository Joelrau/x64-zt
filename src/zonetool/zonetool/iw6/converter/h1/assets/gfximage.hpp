#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace gfximage
		{
			zonetool::h1::GfxImage* convert(GfxImage* asset, utils::memory::allocator& allocator);
			void dump(GfxImage* asset);
			void dump_h2(GfxImage* asset);
		}
	}
}