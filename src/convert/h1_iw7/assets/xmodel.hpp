#pragma once

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace xmodel
		{
			zonetool::iw7::XModel* convert(zonetool::h1::XModel* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h1::XModel* asset);
		}
	}
}
