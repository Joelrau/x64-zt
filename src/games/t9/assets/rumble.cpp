#include <std_include.hpp>
#include "rumble.hpp"

namespace zonetool::t9
{
	void rumble::read(auto& reader, RumbleInfo* asset)
	{
		reader.read_array(asset->highRumbleGraph, 7, 1);
		reader.read_array(asset->lowRumbleGraph, 7, 1);
	}
}

namespace zonetool::t9
{
	template void rumble::read(zone_reader&, RumbleInfo*);
	template void rumble::read(zone_writer&, RumbleInfo*);
}
