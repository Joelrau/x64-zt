#pragma once
#include "../include.hpp"

namespace zonetool::iw5::converter::iw7
{
	namespace clipmap
	{
		zonetool::iw7::clipMap_t* convert(zonetool::iw5::clipMap_t* asset, utils::memory::allocator& allocator);
		void dump(zonetool::iw5::clipMap_t* asset);
	}
}
