#pragma once

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace clip_map
		{
			zonetool::iw7::clipMap_t* convert(clipMap_t* asset, utils::memory::allocator& allocator);
			void dump(clipMap_t* asset);
		}
	}
}