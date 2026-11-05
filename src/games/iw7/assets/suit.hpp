#pragma once
#include "../iw7.hpp"

namespace zonetool::iw7
{
	class suit
	{
	public:
		static void read(zone_reader& reader, SuitDef* asset);
	};
}
