#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class rumble
	{
	public:
		static void read(zone_reader& reader, RumbleInfo* asset);
	};

	class rumble_graph
	{
	public:
		static void read(zone_reader& reader, RumbleGraph* asset);
	};
}
