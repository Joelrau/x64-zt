#pragma once
#include "../include.hpp"

namespace zonetool::iw4::converter::iw5
{
	namespace comworld
	{
		zonetool::iw5::ComWorld* convert(zonetool::iw4::ComWorld* asset, x86::allocator& allocator);
	}
}
