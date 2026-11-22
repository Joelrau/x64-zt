#pragma once
#include "../include.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace clipmap
	{
		zonetool::iw4::clipMap_t* convert(zonetool::iw3::clipMap_t* asset, x86::allocator& allocator);
	}
}
