#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class carry_object
	{
	public:
		static void read(zone_reader& reader, CarryObjectDef* asset);
	};
}
