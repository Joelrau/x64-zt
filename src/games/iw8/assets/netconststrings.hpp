#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class net_const_strings
	{
	public:
		static void read(zone_reader& reader, NetConstStrings* asset);
		static void dump(NetConstStrings* asset);
	};
}
