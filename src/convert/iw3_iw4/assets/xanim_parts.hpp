#pragma once
#include "../include.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace xanim_parts
	{
		zonetool::iw4::XAnimParts* convert(zonetool::iw3::XAnimParts* asset, x86::allocator& allocator);
	}
}
