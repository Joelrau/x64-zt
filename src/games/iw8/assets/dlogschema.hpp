#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class dlog_schema
	{
	public:
		static void read(zone_reader& reader, DLogSchema* asset);
	};
}
