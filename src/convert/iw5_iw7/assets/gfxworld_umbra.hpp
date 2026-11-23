#pragma once
#include "../include.hpp"

namespace zonetool::iw5::converter::iw7
{
	namespace gfxworld
	{
		// Falls back to the draw-everything tome when the generator is missing or fails.
		void generate_umbra_tome(zonetool::iw5::GfxWorld* asset, zonetool::iw7::GfxWorld* world, utils::memory::allocator& allocator);
	}
}
