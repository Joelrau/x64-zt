#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class stream_key
	{
	public:
		static void read(zone_reader& reader, StreamKey* asset);
	};
}
