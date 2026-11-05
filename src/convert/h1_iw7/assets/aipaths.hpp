#pragma once

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace path_data
		{
			zonetool::iw7::PathData* convert(PathData* asset, utils::memory::allocator& allocator);
			void dump(PathData* asset);
		}
	}
}
