#pragma once

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace xmodel
		{
			zonetool::h1::XModel* convert(zonetool::h2::XModel* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h2::XModel* asset);
		}
	}
}