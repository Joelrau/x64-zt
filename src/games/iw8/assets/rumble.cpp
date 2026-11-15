#include <std_include.hpp>
#include "rumble.hpp"

namespace zonetool::iw8
{
	void rumble::read(zone_reader& reader, RumbleInfo* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->highRumbleGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->lowRumbleGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->leftTriggerRumbleGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->rightTriggerRumbleGraph);
		reader.pop_stream();
	}

	void rumble_graph::read(zone_reader& reader, RumbleGraph* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}
}
