#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class rawfile
	{
	public:
		static void read(zone_reader32& reader, RawFile* asset);
		static std::string text(RawFile* asset);
		static void dump(RawFile* asset);
	};
}
