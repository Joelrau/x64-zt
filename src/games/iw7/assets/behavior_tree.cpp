#include <std_include.hpp>
#include "behavior_tree.hpp"

namespace zonetool::iw7
{
	void behavior_tree::read(zone_reader& reader, BehaviorTree* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto nodes = reader.read_array(asset->nodes, 7, asset->nodeCount))
		{
			for (auto i = 0; i < asset->nodeCount; i++)
			{
				reader.read_string(nodes[i].name);
			}
		}
		reader.pop_stream();
	}
}
