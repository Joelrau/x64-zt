#pragma once

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace comworld
		{
			zonetool::h2::ComWorld* convert(zonetool::h1::ComWorld* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h1::ComWorld* asset);
		}
	}
}
