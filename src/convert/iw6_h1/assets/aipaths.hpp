#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace aipaths
		{
			zonetool::h1::PathData* convert(PathData* asset, utils::memory::allocator& allocator);
			void dump(PathData* asset);
		}
	}
}
