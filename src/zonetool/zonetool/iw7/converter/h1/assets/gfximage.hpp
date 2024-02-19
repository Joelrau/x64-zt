#pragma once

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace gfximage
		{
			zonetool::h2::GfxImage* convert(GfxImage* asset, utils::memory::allocator& allocator);
			void dump(GfxImage* asset);
		}
	}
}