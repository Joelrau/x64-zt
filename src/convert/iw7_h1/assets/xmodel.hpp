#pragma once

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace xmodel
		{
			zonetool::h1::XModel* convert(zonetool::iw7::XModel* asset, utils::memory::allocator& allocator);
			void dump(zonetool::iw7::XModel* asset);
		}
	}
}