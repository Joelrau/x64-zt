#include <std_include.hpp>

#include "../include.hpp"
#include "pathdata.hpp"

#include <iw8/assets/pathdata.hpp>

namespace zonetool::iw7::converter::iw8::pathdata
{
	namespace
	{
		namespace target = zonetool::iw8;

		target::scr_string_t convert_script_string(const scr_string_t value)
		{
			return static_cast<target::scr_string_t>(value);
		}

		target::pathnode_tree_t* convert_node_trees(pathnode_tree_t* trees, int count, utils::memory::allocator& allocator);

		void convert_node_tree(const pathnode_tree_t& tree, target::pathnode_tree_t& converted, utils::memory::allocator& allocator)
		{
			converted.axis = tree.axis;
			converted.dist = tree.dist;

			if (tree.axis < 0)
			{
				converted.u.s.nodeCount = tree.u.s.nodeCount;
				if (tree.u.s.nodes && tree.u.s.nodeCount > 0)
				{
					converted.u.s.nodes = allocator.allocate_array<unsigned short>(tree.u.s.nodeCount);
					std::memcpy(converted.u.s.nodes, tree.u.s.nodes, sizeof(unsigned short) * tree.u.s.nodeCount);
				}
				return;
			}

			for (auto i = 0; i < 2; i++)
			{
				converted.u.child[i] = convert_node_trees(tree.u.child[i], 1, allocator);
			}
		}

		target::pathnode_tree_t* convert_node_trees(pathnode_tree_t* trees, const int count, utils::memory::allocator& allocator)
		{
			if (!trees || count <= 0)
			{
				return nullptr;
			}

			auto* converted = allocator.allocate_array<target::pathnode_tree_t>(count);
			for (auto i = 0; i < count; i++)
			{
				convert_node_tree(trees[i], converted[i], allocator);
			}

			return converted;
		}

		void convert_node(const pathnode_constant_t& node, target::pathnode_constant_t& converted, utils::memory::allocator& allocator)
		{
			converted.spawnflags = node.spawnflags;
			converted.targetname = convert_script_string(node.targetname);
			converted.script_linkName = convert_script_string(node.script_linkName);
			converted.script_noteworthy = convert_script_string(node.script_noteworthy);
			converted.target = convert_script_string(node.target);
			converted.animscript = convert_script_string(node.animscript);
			converted.animscriptfunc = node.animscriptfunc;
			std::memcpy(converted.vLocalOrigin, node.vLocalOrigin, sizeof(converted.vLocalOrigin));
			std::memcpy(converted.angles, &node.orientation, sizeof(converted.angles));
			converted.parent.name = convert_script_string(node.parent.name);
			converted.minUseDistSq = node.___u11.minUseDistSq;
			converted.wOverlapNode[0] = node.wOverlapNode[0];
			converted.wOverlapNode[1] = node.wOverlapNode[1];
			converted.totalLinkCount = node.totalLinkCount;
			converted.type = node.type;

			if (node.Links && node.totalLinkCount)
			{
				converted.Links = allocator.allocate_array<target::pathlink_s>(node.totalLinkCount);
				std::memcpy(converted.Links, node.Links, sizeof(target::pathlink_s) * node.totalLinkCount);
			}
		}

		void convert_exposure(const PathData* asset, target::PathData* converted, utils::memory::allocator& allocator)
		{
			if (!asset->pathExposure || asset->exposureBytes <= 0)
			{
				return;
			}

			const auto record_bytes = asset->nodeCount * 33; // sub_11E3210
			const auto bit_row_bytes = (asset->nodeCount + 7) / 8; // sub_11E3220
			if (static_cast<unsigned int>(asset->exposureBytes) != record_bytes + bit_row_bytes)
			{
				ZONETOOL_WARNING("aipaths \"%s\": exposure size %d does not match the node count, exposure is not converted", asset->name, asset->exposureBytes);
				return;
			}

			converted->exposureBytes = static_cast<int>(record_bytes + bit_row_bytes * 2);
			converted->pathExposure = allocator.allocate_array<char>(converted->exposureBytes);
			std::memcpy(converted->pathExposure, asset->pathExposure, asset->exposureBytes);
		}

		target::PathData* convert(PathData* asset, utils::memory::allocator& allocator)
		{
			auto* converted = allocator.allocate<target::PathData>();
			converted->name = asset->name;
			converted->version = 30;
			converted->parentIndexResolved = false;

			converted->nodeCount = asset->nodeCount;
			if (asset->nodes && asset->nodeCount)
			{
				converted->nodes = allocator.allocate_array<target::pathnode_t>(asset->nodeCount);
				for (auto i = 0u; i < asset->nodeCount; i++)
				{
					convert_node(asset->nodes[i].constant, converted->nodes[i].constant, allocator);
				}
			}

			converted->nodeTreeCount = asset->nodeTreeCount;
			converted->nodeTree = convert_node_trees(asset->nodeTree, asset->nodeTreeCount, allocator);

			if (asset->dynamicNodeGroups && asset->dynamicNodeGroupCount > 0)
			{
				converted->dynamicNodeGroupCount = asset->dynamicNodeGroupCount;
				converted->dynamicNodeGroups = allocator.allocate_array<target::PathDynamicNodeGroup>(asset->dynamicNodeGroupCount);
				for (auto i = 0; i < asset->dynamicNodeGroupCount; i++)
				{
					const auto& group = asset->dynamicNodeGroups[i];
					converted->dynamicNodeGroups[i].parentIndex = group.parentIndex;
					converted->dynamicNodeGroups[i].nodeTreeCount = group.nodeTreeCount;
					converted->dynamicNodeGroups[i].nodeTree = convert_node_trees(group.nodeTree, group.nodeTreeCount, allocator);
				}
			}

			convert_exposure(asset, converted, allocator);

			if (asset->pathNoPeekVis && asset->noPeekVisBytes > 0)
			{
				converted->noPeekVisBytes = asset->noPeekVisBytes;
				converted->pathNoPeekVis = allocator.allocate_array<char>(asset->noPeekVisBytes);
				std::memcpy(converted->pathNoPeekVis, asset->pathNoPeekVis, asset->noPeekVisBytes);
			}

			converted->zoneCount = asset->zoneCount;
			if (asset->pathZones && asset->zonesBytes > 0)
			{
				converted->zonesBytes = asset->zonesBytes;
				converted->pathZones = allocator.allocate_array<char>(asset->zonesBytes);
				std::memcpy(converted->pathZones, asset->pathZones, asset->zonesBytes);
			}

			converted->usePathExtraData = asset->zoneCount > 0; // spawns zone nodes (sub_11DDDC0)
			converted->fixedNodeCount = asset->fixedNodeCount;
			converted->maxDynamicSpawnedNodeCount = asset->maxDynamicSpawnedNodeCount;

			if (asset->pathDynStates && asset->dynStatesBytes > 0)
			{
				converted->dynStatesBytes = asset->dynStatesBytes;
				converted->pathDynStates = allocator.allocate_array<char>(asset->dynStatesBytes);
				std::memcpy(converted->pathDynStates, asset->pathDynStates, asset->dynStatesBytes);
			}

			return converted;
		}
	}

	void dump(PathData* asset)
	{
		utils::memory::allocator allocator;
		const auto converted = convert(asset, allocator);
		target::path_data::dump(converted);
		add_csv_row("aipaths", asset->name);
	}
}
