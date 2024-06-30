#pragma once

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace comworld
		{
			zonetool::iw7::ComWorld* convert(zonetool::h1::ComWorld* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h1::ComWorld* asset);
		}
	}
}
