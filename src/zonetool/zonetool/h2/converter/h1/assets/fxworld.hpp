#pragma once

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace fxworld
		{
			zonetool::h1::FxWorld* convert(zonetool::h2::FxWorld* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h2::FxWorld* asset);
		}
	}
}
