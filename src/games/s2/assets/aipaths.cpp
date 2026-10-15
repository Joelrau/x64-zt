#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		template <typename V>
		void visit_node_tree(const V& tree)
		{
			if (tree.template at<int>(offsetof(pathnode_tree_t, axis)) < 0)
			{
				constexpr auto nodes = offsetof(pathnode_tree_t, u) + offsetof(pathnode_tree_info_t, s);
				tree.array(nodes + offsetof(pathnode_tree_nodes_t, nodes), sizeof(unsigned short), 1,
					tree.template at<int>(nodes + offsetof(pathnode_tree_nodes_t, nodeCount)));
				return;
			}

			for (auto i = 0u; i < 2; i++)
			{
				tree.entries(offsetof(pathnode_tree_t, u) + i * sizeof(pathnode_tree_t*), sizeof(pathnode_tree_t), 1, [](const auto& child, std::size_t)
				{
					visit_node_tree(child);
				});
			}
		}

		template <typename V>
		void visit_node_trees(const V& parent, const std::size_t offset, const int count)
		{
			parent.entries(offset, sizeof(pathnode_tree_t), count, [](const auto& tree, std::size_t)
			{
				visit_node_tree(tree);
			});
		}

		template <typename V>
		void visit_node_constant(const V& node, const bool parent_index_resolved)
		{
			node.script_string(offsetof(pathnode_constant_t, targetname));
			node.script_string(offsetof(pathnode_constant_t, script_linkName));
			node.script_string(offsetof(pathnode_constant_t, script_noteworthy));
			node.script_string(offsetof(pathnode_constant_t, target));
			node.script_string(offsetof(pathnode_constant_t, animscript));
			if (!parent_index_resolved)
			{
				node.script_string(offsetof(pathnode_constant_t, parentName));
			}

			node.array(S2_ARRAY_FIELD(pathnode_constant_t, Links), 3, node.template at<unsigned short>(offsetof(pathnode_constant_t, totalLinkCount)));
		}
	}

	template <typename V>
	void path_data_layout::visit(const V& w)
	{
		const auto count = [&](const std::size_t offset)
		{
			return w.template at<int>(offset);
		};

		const auto node_count = w.template at<unsigned int>(offsetof(PathData, nodeCount));
		const auto parent_index_resolved = w.template at<bool>(offsetof(PathData, parentIndexResolved));

		w.string(offsetof(PathData, name));
		w.entries(offsetof(PathData, nodes), sizeof(pathnode_t), node_count, [&](const auto& node, std::size_t)
		{
			visit_node_constant(node.sub(offsetof(pathnode_t, constant)), parent_index_resolved);
		});
		w.bytes(offsetof(PathData, pathVis), 0, w.template at<unsigned int>(offsetof(PathData, visBytes)));
		visit_node_trees(w, offsetof(PathData, nodeTree), count(offsetof(PathData, nodeTreeCount)));
		w.entries(offsetof(PathData, dynamicNodeGroups), sizeof(PathDynamicNodeGroup), count(offsetof(PathData, dynamicNodeGroupCount)), [](const auto& group, std::size_t)
		{
			visit_node_trees(group, offsetof(PathDynamicNodeGroup, nodeTree), group.template at<int>(offsetof(PathDynamicNodeGroup, nodeTreeCount)));
		});
		w.bytes(offsetof(PathData, pathExposure), 0, count(offsetof(PathData, exposureBytes)));
		w.bytes(offsetof(PathData, pathNoPeekVis), 0, count(offsetof(PathData, noPeekVisBytes)));
		w.array(S2_ARRAY_FIELD(PathData, pathZones), 1, count(offsetof(PathData, zoneCount)) ? node_count : 0);
		w.bytes(offsetof(PathData, pathDynStates), 0, count(offsetof(PathData, dynStatesBytes)));
	}

	template void path_data_layout::visit(const field_reader&);
	template void path_data_layout::visit(const field_writer&);
	template void path_data_layout::visit(const field_dumper&);
	template void path_data_layout::visit(const field_parser&);
	template void path_data_layout::visit(const field_dependencies&);
	template void path_data_layout::visit(const field_preparer&);
}
