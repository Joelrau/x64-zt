#pragma once

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace fxworld
		{
			zonetool::h1::FxWorld* convert(zonetool::iw7::FxWorld* asset, utils::memory::allocator& allocator);
			void dump(zonetool::iw7::FxWorld* asset);
		}
	}
}
