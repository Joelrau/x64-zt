#pragma once

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace gfxworld
		{
			zonetool::h1::GfxWorld* convert(zonetool::iw7::GfxWorld* asset, utils::memory::allocator& allocator);
			void dump(zonetool::iw7::GfxWorld* asset);
		}
	}
}
