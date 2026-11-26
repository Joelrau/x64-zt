#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class dlog_schema
	{
	public:
		static void read(zone_reader& reader, DLogSchema* asset);
	};
}
