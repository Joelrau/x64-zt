#pragma once

namespace zonetool::t7
{
	namespace converter::iw7
	{
		namespace xmodel
		{
			zonetool::iw7::XModel* convert(XModel* asset, utils::memory::allocator& allocator);
			void dump(XModel* asset);
		}
	}
}
