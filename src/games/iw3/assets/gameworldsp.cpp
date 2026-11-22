#include <std_include.hpp>
#include "gameworldsp.hpp"

namespace zonetool::iw3
{
	namespace
	{
		void read_tree(zone_reader32& reader, pathnode_tree_t* tree)
		{
			if (tree->axis < 0)
			{
				reader.read_array(tree->u.s.nodes, 1, tree->u.s.nodeCount);
				return;
			}

			for (auto& field : tree->u.child)
			{
				if (const auto child = reader.read_single(field, 3))
				{
					read_tree(reader, child);
				}
			}
		}

		void read_path_data(zone_reader32& reader, PathData& path)
		{
			if (const auto nodes = reader.read_array(path.nodes, 3, path.nodeCount))
			{
				for (auto i = 0u; i < path.nodeCount; i++)
				{
					auto& constant = nodes[i].constant;
					reader.read_script_string(constant.targetname);
					reader.read_script_string(constant.script_linkName);
					reader.read_script_string(constant.script_noteworthy);
					reader.read_script_string(constant.target);
					reader.read_script_string(constant.animscript);
					reader.read_array(constant.Links, 3, constant.totalLinkCount);
				}
			}

			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(path.basenodes, 15, path.nodeCount);
			reader.pop_stream();

			reader.read_array(path.chainNodeForNode, 1, path.nodeCount);
			reader.read_array(path.nodeForChainNode, 1, path.nodeCount);
			reader.read_array(path.pathVis, 0, path.visBytes);

			if (const auto trees = reader.read_array(path.nodeTree, 3, path.nodeTreeCount))
			{
				for (auto i = 0; i < path.nodeTreeCount; i++)
				{
					read_tree(reader, &trees[i]);
				}
			}
		}
	}

	void game_world_sp::read(zone_reader32& reader, GameWorldSp* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		read_path_data(reader, asset->path);
		reader.pop_stream();
	}
}
