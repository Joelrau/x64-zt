#include <std_include.hpp>
#include "behaviortree.hpp"

namespace zonetool::s4
{
	void behavior_tree::read(zone_reader& reader, BehaviorTree* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto nodes = reader.read_array(asset->nodes, 7, asset->nodeCount))
		{
			for (auto i = 0u; i < asset->nodeCount; i++)
			{
				reader.read_string(nodes[i].name);
			}
		}
		reader.read_array(asset->funcIDs, 3, asset->numFuncIDs);
		reader.read_array(asset->decorators, 1, asset->decoratorCount);
		reader.pop_stream();
	}
}
