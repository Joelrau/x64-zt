#pragma once
#include "../include.hpp"

namespace zonetool::iw5::converter::iw7
{
	namespace mapents
	{
		zonetool::iw7::MapEnts* convert(zonetool::iw5::MapEnts* asset, utils::memory::allocator& allocator);
		void dump(zonetool::iw5::MapEnts* asset);
	}
}
