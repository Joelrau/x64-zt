#pragma once
#include "../include.hpp"

namespace zonetool::iw4::converter::iw5
{
	namespace gfxworld
	{
		zonetool::iw5::GfxWorld* convert(zonetool::iw4::GfxWorld* asset, x86::allocator& allocator);
	}
}
