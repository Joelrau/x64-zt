#include <std_include.hpp>
#include "streamtreeoverride.hpp"

namespace zonetool::s4
{
	void stream_tree_override::read(zone_reader& reader, StreamTreeOverride* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto nodes = reader.read_array(asset->imageNodes, 7, asset->imageNodeCount))
		{
			for (auto i = 0u; i < asset->imageNodeCount; i++)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, nodes[i].desImage);
			}
		}
		reader.pop_stream();
	}
}
