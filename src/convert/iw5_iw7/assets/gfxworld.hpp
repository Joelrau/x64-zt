#pragma once
#include "../include.hpp"

namespace zonetool::iw5::converter::iw7
{
	namespace gfxworld
	{
		zonetool::iw7::GfxWorld* convert(zonetool::iw5::GfxWorld* asset, utils::memory::allocator& allocator);
		void dump(zonetool::iw5::GfxWorld* asset);
	}
}
