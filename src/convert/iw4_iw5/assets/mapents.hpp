#pragma once
#include "../include.hpp"

namespace zonetool::iw4::converter::iw5
{
	namespace mapents
	{
		zonetool::iw5::MapEnts* convert(zonetool::iw4::MapEnts* asset, x86::allocator& allocator);
	}
}
