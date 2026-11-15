#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class key_value_pairs
	{
	public:
		static void read(zone_reader& reader, KeyValuePairs* asset);
		static void dump(KeyValuePairs* asset);
	};
}
