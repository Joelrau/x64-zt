#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class stream_tree_override
	{
	public:
		static void read(zone_reader& reader, StreamTreeOverride* asset);
	};
}
