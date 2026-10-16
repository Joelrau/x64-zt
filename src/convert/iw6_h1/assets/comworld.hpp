#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace comworld
		{
			zonetool::h1::ComWorld* convert(ComWorld* asset, utils::memory::allocator& allocator);
			void dump(ComWorld* asset);
		}
	}
}