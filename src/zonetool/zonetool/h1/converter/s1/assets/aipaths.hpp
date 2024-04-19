#pragma once

namespace zonetool::h1
{
	namespace converter::s1
	{
		namespace aipaths
		{
			zonetool::s1::PathData* convert(PathData* asset, utils::memory::allocator& allocator);
			void dump(PathData* asset);
		}
	}
}
