#pragma once
#include "../include.hpp"

namespace zonetool::iw4::converter::iw5
{
	namespace fx
	{
		zonetool::iw5::FxEffectDef* convert(zonetool::iw4::FxEffectDef* asset, x86::allocator& allocator);
	}
}
