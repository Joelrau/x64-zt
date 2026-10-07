#pragma once
#include "../h2.hpp"

namespace zonetool::h2
{
	class proto
	{
	public:
		static void read(zone_reader& reader, Proto* asset);
	};
}
