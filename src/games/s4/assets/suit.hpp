#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class suit_def
	{
	public:
		static void read(zone_reader& reader, SuitDef* asset);
	};
}
