#pragma once

namespace zonetool::h1
{
	namespace converter::s1
	{
		namespace gfxworld
		{
			zonetool::s1::GfxWorld* convert(GfxWorld* asset, utils::memory::allocator& allocator);
			void dump(GfxWorld* asset);
		}
	}
}
