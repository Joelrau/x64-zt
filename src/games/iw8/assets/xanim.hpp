#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class xanim_parts
	{
	public:
		static void read(zone_reader& reader, XAnimParts* asset);
	};
}
