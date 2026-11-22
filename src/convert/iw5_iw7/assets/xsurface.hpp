#pragma once
#include "../include.hpp"

namespace zonetool::iw5::converter::iw7
{
	namespace xsurface
	{
		zonetool::iw7::XModelSurfs* convert(zonetool::iw5::XModelSurfs* asset, utils::memory::allocator& allocator);
		void dump(zonetool::iw5::XModelSurfs* asset);
	}
}
