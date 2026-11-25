#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class xanim_parts
	{
	public:
		static void read(zone_reader32& reader, XAnimParts* asset);
	};
}
