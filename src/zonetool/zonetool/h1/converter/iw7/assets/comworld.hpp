#pragma once

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace com_world
		{
			zonetool::iw7::ComWorld* convert(zonetool::h1::ComWorld* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h1::ComWorld* asset);
		}
	}
}
