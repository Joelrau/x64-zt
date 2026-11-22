#pragma once
#include "../include.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace fx
	{
		zonetool::iw4::FxEffectDef* convert(zonetool::iw3::FxEffectDef* asset, x86::allocator& allocator);
	}
}
