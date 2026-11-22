#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class rawfile
	{
	public:
		static void read(zone_reader32& reader, RawFile* asset);
		static void dump(RawFile* asset);
	};
}
