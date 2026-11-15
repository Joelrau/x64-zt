#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class streaming_info
	{
	public:
		static void read(zone_reader& reader, StreamingInfo* asset);
	};
}
