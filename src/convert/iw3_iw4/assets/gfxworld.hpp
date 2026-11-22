#pragma once
#include "../include.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace gfxworld
	{
		zonetool::iw4::GfxWorld* convert(zonetool::iw3::GfxWorld* asset, x86::allocator& allocator);
	}
}
