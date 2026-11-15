#include <std_include.hpp>
#include "pathdata.hpp"

namespace zonetool::iw8
{
	namespace
	{
		void read_node_tree_children(zone_reader& reader, pathnode_tree_t* tree);

		void read_node_tree_array(zone_reader& reader, pathnode_tree_t*& field, const std::size_t count)
		{
			if (const auto trees = reader.read_array(field, 7, count))
			{
				for (std::size_t i = 0; i < count; i++)
				{
					read_node_tree_children(reader, &trees[i]);
				}
			}
		}

		void read_node_tree_children(zone_reader& reader, pathnode_tree_t* tree)
		{
			if (tree->axis < 0)
			{
				reader.read_array(tree->u.s.nodes, 1, tree->u.s.nodeCount);
				return;
			}

			for (auto& child : tree->u.child)
			{
				read_node_tree_array(reader, child, 1);
			}
		}

		void read_node(zone_reader& reader, pathnode_constant_t& node)
		{
			reader.read_script_string(node.targetname);
			reader.read_script_string(node.script_linkName);
			reader.read_script_string(node.script_noteworthy);
			reader.read_script_string(node.target);
			reader.read_script_string(node.animscript);
			reader.read_script_string(node.interactable);
			reader.read_script_string(node.parent.name);
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
				read_node(reader, nodes[i].constant);
			}
		}

		reader.read_array(asset->pathVis, 0, asset->visBytes);
		read_node_tree_array(reader, asset->nodeTree, asset->nodeTreeCount);

		if (const auto groups = reader.read_array(asset->dynamicNodeGroups, 7, asset->dynamicNodeGroupCount))
		{
			for (auto i = 0; i < asset->dynamicNodeGroupCount; i++)
			{
				read_node_tree_array(reader, groups[i].nodeTree, groups[i].nodeTreeCount);
			}
		}

		reader.read_array(asset->pathExposure, 0, asset->exposureBytes);
		reader.read_array(asset->pathNoPeekVis, 0, asset->noPeekVisBytes);
		reader.read_array(asset->pathZones, 0, asset->zonesBytes);
		reader.read_array(asset->pathDynStates, 0, asset->dynStatesBytes);
		reader.pop_stream();
	}
}
