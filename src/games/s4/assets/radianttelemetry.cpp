#include <std_include.hpp>
#include "radianttelemetry.hpp"

namespace zonetool::s4
{
	void radiant_telemetry::read(zone_reader& reader, RadiantTelemetry* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		reader.read_array(asset->unknown0, 3, asset->unknown0Count);
		reader.read_array(asset->unknown1, 3, asset->unknown1Count);
		reader.read_array(asset->unknown2, 3, asset->unknown2Count);
		reader.read_array(asset->unknown3, 3, asset->unknown3Count);
		reader.read_array(asset->unknown4, 3, asset->unknown4Count);
		reader.read_array(asset->unknown5, 3, asset->unknown5Count);
		reader.read_array(asset->unknown6, 0, asset->unknown6Count);
		reader.read_array(asset->unknown7, 0, asset->unknown7Count);
		reader.read_array(asset->unknown8, 0, asset->unknown8Count);
		reader.pop_stream();
	}
}
