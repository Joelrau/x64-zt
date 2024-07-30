#pragma once

namespace zonetool::t7
{
	namespace converter::h1
	{
		namespace xanim
		{
			zonetool::h1::XAnimParts* convert(XAnimParts* asset, utils::memory::allocator& allocator);
			void dump(XAnimParts* asset);
		}
	}
}
