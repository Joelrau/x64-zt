#pragma once
#include "../include.hpp"

namespace zonetool::iw5::converter::iw7
{
	namespace glass_world
	{
		zonetool::iw7::GlassWorld* convert(zonetool::iw5::GlassWorld* asset, utils::memory::allocator& allocator);
		void dump(zonetool::iw5::GlassWorld* asset);
	}
}
