#pragma once

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace comworld
		{
			zonetool::h1::ComWorld* convert(zonetool::iw7::ComWorld* asset, utils::memory::allocator& allocator);
			void dump(zonetool::iw7::ComWorld* asset);
		}
	}
}
