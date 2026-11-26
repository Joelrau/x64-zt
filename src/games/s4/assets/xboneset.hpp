#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class xbone_set
	{
	public:
		static void read(zone_reader& reader, XBoneSet* asset);
	};
}
