#include <std_include.hpp>

#include "../include.hpp"
#include "xanim_parts.hpp"

namespace zonetool::iw4::converter::iw5
{
	namespace xanim_parts
	{
		zonetool::iw5::XAnimParts* convert(zonetool::iw4::XAnimParts* asset)
		{
			static_assert(sizeof(zonetool::iw4::XAnimParts) == sizeof(zonetool::iw5::XAnimParts));
			static_assert(sizeof(zonetool::iw4::XAnimDeltaPart) == sizeof(zonetool::iw5::XAnimDeltaPart));
			return reinterpret_cast<zonetool::iw5::XAnimParts*>(asset);
		}
	}
}
