#pragma once
#include "../include.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace comworld
	{
		zonetool::iw4::ComWorld* convert(zonetool::iw3::ComWorld* asset, x86::allocator& allocator);
	}
}
