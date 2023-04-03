#pragma once

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace xmodel
		{
			zonetool::h2::XModel* convert(zonetool::h1::XModel* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h1::XModel* asset, const std::function<const char*(unsigned int)>& convert_to_string);
		}
	}
}