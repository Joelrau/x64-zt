#pragma once

namespace zonetool::iw6
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