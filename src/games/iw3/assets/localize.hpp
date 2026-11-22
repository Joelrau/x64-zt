#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class localize
	{
	public:
		static void read(zone_reader32& reader, LocalizeEntry* asset);
		static void dump(LocalizeEntry* asset);
	};
}
