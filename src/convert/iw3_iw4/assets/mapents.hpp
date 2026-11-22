#pragma once
#include "../include.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace mapents
	{
		zonetool::iw4::MapEnts* convert(zonetool::iw3::MapEnts* asset, x86::allocator& allocator);
	}
}
