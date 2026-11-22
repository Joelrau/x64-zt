#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class string_table
	{
	public:
		static void read(zone_reader32& reader, StringTable* asset);
		static void dump(StringTable* asset);
	};
}
