#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class string_table
	{
	public:
		static void read(zone_reader32& reader, StringTable* asset);
		static void dump(StringTable* asset);
	};
}
