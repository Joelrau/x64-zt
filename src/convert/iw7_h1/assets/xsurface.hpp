#pragma once

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace xsurface
		{
			zonetool::h1::XModelSurfs* convert(XModelSurfs* asset, utils::memory::allocator& allocator);
			void dump(XModelSurfs* asset);
		}
	}
}