#include <std_include.hpp>
#include "calloutmarkerping.hpp"

namespace zonetool::s4
{
	void callout_marker_ping::read(zone_reader& reader, CalloutMarkerPing* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}
}
