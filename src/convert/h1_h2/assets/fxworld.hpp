#pragma once

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace fxworld
		{
			zonetool::h2::FxWorld* convert(zonetool::h1::FxWorld* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h1::FxWorld* asset);
		}
	}
}
