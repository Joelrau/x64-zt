#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		void read_node_tree(zone_reader& reader, pathnode_tree_t* tree)
		{
			if (tree->axis < 0)
			{
				reader.read_array(tree->u.s.nodes, 1, tree->u.s.nodeCount);
				return;
			}

			for (auto& child : tree->u.child)
			{
				if (const auto child_tree = reader.read_single(child, 7))
				{
					read_node_tree(reader, child_tree);
				}
			}
		}

		void read_node_trees(zone_reader& reader, pathnode_tree_t*& field, const int count)
		{
			if (const auto trees = reader.read_array(field, 7, count))
			{
				for (auto i = 0; i < count; i++)
				{
					read_node_tree(reader, &trees[i]);
				}
			}
		}

		void read_node_constant(zone_reader& reader, pathnode_constant_t& node, const bool parent_index_resolved)
		{
			reader.read_script_string(node.targetname);
			reader.read_script_string(node.script_linkName);
			reader.read_script_string(node.script_noteworthy);
			reader.read_script_string(node.target);
			reader.read_script_string(node.animscript);
			if (!parent_index_resolved)
			{
				reader.read_script_string(node.parentName);
			}

			reader.read_array(node.Links, 3, node.totalLinkCount);
		}
	}

	void path_data::read(zone_reader& reader, PathData* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto nodes = reader.read_array(asset->nodes, 7, asset->nodeCount))
		{
			for (auto i = 0u; i < asset->nodeCount; i++)
			{
				read_node_constant(reader, nodes[i].constant, asset->parentIndexResolved);
			}
		}

		reader.read_array(asset->pathVis, 0, asset->visBytes);
		read_node_trees(reader, asset->nodeTree, asset->nodeTreeCount);

		if (const auto groups = reader.read_array(asset->dynamicNodeGroups, 7, asset->dynamicNodeGroupCount))
		{
			for (auto i = 0; i < asset->dynamicNodeGroupCount; i++)
			{
				read_node_trees(reader, groups[i].nodeTree, groups[i].nodeTreeCount);
			}
		}

		reader.read_array(asset->pathExposure, 0, asset->exposureBytes);
		reader.read_array(asset->pathNoPeekVis, 0, asset->noPeekVisBytes);
		reader.read_array(asset->pathZones, 1, asset->zoneCount ? asset->nodeCount : 0);
		reader.read_array(asset->pathDynStates, 0, asset->dynStatesBytes);
		reader.pop_stream();
	}
}
