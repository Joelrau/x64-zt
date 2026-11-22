#pragma once
#include "../include.hpp"

namespace zonetool::iw5::converter::iw7
{
	namespace comworld
	{
		zonetool::iw7::ComWorld* convert(zonetool::iw5::ComWorld* asset, utils::memory::allocator& allocator);
		void dump(zonetool::iw5::ComWorld* asset);
	}
}
