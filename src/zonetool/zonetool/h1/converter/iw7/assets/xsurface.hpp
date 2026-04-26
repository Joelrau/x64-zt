#pragma once

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace xsurface
		{
			zonetool::iw7::XModelSurfs* convert(zonetool::h1::XModelSurfs* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h1::XModelSurfs* asset);
		}
	}
}
