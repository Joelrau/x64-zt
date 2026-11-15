#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class string_table
	{
	public:
		static void read(zone_reader& reader, StringTable* asset);
	};
}
