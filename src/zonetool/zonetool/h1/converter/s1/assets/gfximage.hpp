#pragma once

namespace zonetool::h1
{
	namespace converter::s1
	{
		namespace gfximage
		{
			zonetool::s1::GfxImage* convert(GfxImage* asset, utils::memory::allocator& allocator);
			void dump(GfxImage* asset);
		}
	}
}
