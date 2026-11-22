#pragma once
#include "../include.hpp"

namespace zonetool::iw4::converter::iw5
{
	namespace xmodel
	{
		zonetool::iw5::XModel* convert(zonetool::iw4::XModel* asset, x86::allocator& allocator);
	}
}
