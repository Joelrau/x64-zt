#pragma once
#include "../include.hpp"

namespace zonetool::iw4::converter::iw5
{
	namespace clipmap
	{
		zonetool::iw5::clipMap_t* convert(zonetool::iw4::clipMap_t* asset, x86::allocator& allocator);
	}
}
