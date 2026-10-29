#pragma once

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace glass_world
		{
			zonetool::iw7::GlassWorld* convert(zonetool::h1::GlassWorld* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h1::GlassWorld* asset);
		}
	}
}
