#pragma once
#include "../include.hpp"

namespace zonetool::iw5::converter::iw7
{
	namespace fx
	{
		zonetool::iw7::FxEffectDef* convert(zonetool::iw5::FxEffectDef* asset, utils::memory::allocator& allocator);
		void dump(zonetool::iw5::FxEffectDef* asset);
	}
}
