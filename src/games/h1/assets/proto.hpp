#pragma once
#include "../h1.hpp"

namespace zonetool::h1
{
	class proto
	{
	public:
		static void read(zone_reader& reader, Proto* asset);
	};
}
