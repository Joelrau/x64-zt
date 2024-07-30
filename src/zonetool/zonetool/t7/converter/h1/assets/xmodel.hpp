#pragma once

namespace zonetool::t7
{
	namespace converter::h1
	{
		namespace xmodel
		{
			zonetool::h1::XModel* convert(XModel* asset, utils::memory::allocator& allocator);
			void dump(XModel* asset);
		}
	}
}
