#pragma once
#include "../include.hpp"

namespace zonetool::iw4::converter::iw5
{
	namespace material
	{
		zonetool::iw5::Material* convert(zonetool::iw4::Material* asset, x86::allocator& allocator);
	}
}
