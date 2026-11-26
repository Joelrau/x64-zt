#include <std_include.hpp>
#include "waterbuoyancy.hpp"

namespace zonetool::s4
{
	void water_buoyancy::read(zone_reader& reader, WaterBuoyancy* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}
}
