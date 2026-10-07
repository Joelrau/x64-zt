#pragma once

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace gfximage
		{
			zonetool::h1::GfxImage* convert(zonetool::h2::GfxImage* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h2::GfxImage* asset);
		}
	}
}