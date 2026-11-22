#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class xanim_parts
	{
	public:
		static void read(zone_reader32& reader, XAnimParts* asset);
	};
}
