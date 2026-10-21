#pragma once

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace gfxworld
		{
			zonetool::h2::GfxWorld* convert(zonetool::h1::GfxWorld* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h1::GfxWorld* asset);
		}
	}
}
