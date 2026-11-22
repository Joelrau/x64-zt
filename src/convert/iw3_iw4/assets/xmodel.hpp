#pragma once
#include "../include.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace xmodel
	{
		zonetool::iw4::XModel* convert(zonetool::iw3::XModel* asset, x86::allocator& allocator);
	}
}
