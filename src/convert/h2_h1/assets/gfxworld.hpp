#pragma once

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace gfxworld
		{
			zonetool::h1::GfxWorld* convert(zonetool::h2::GfxWorld* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h2::GfxWorld* asset);
		}
	}
}
