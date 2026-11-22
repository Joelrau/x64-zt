#pragma once
#include "../include.hpp"

namespace zonetool::iw5::converter::iw7
{
	namespace xanim_parts
	{
		zonetool::iw7::XAnimParts* convert(zonetool::iw5::XAnimParts* asset, utils::memory::allocator& allocator);
		void dump(zonetool::iw5::XAnimParts* asset);
	}
}
