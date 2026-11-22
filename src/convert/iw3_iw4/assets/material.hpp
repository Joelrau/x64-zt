#pragma once
#include "../include.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace material
	{
		zonetool::iw4::Material* convert(zonetool::iw3::Material* asset, x86::allocator& allocator);
	}
}
