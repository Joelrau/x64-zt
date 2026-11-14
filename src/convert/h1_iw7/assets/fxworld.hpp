#pragma once

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace fx_world
		{
			zonetool::iw7::FxWorld* convert(FxWorld* asset, utils::memory::allocator& allocator);
			void dump(FxWorld* asset);
		}
	}
}
