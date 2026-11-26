#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class carry_object
	{
	public:
		static void read(zone_reader& reader, CarryObjectDef* asset);
	};
}
