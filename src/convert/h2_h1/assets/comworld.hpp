#pragma once

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace comworld
		{
			zonetool::h1::ComWorld* convert(zonetool::h2::ComWorld* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h2::ComWorld* asset);
		}
	}
}
