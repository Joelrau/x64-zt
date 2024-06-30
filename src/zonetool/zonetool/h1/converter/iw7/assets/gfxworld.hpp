#pragma once

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace gfxworld
		{
			zonetool::iw7::GfxWorld* convert(GfxWorld* asset, utils::memory::allocator& allocator);
			void dump(GfxWorld* asset);
		}
	}
}
