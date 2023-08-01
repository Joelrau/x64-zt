#pragma once

namespace zonetool::iw6
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