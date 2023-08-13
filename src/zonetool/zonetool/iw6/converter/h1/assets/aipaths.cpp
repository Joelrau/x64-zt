#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "aipaths.hpp"

#include "zonetool/h1/assets/aipaths.hpp"

#include "zonetool/iw6/functions.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace aipaths
		{
			void convert_node(pathnode_t* node, zonetool::h1::pathnode_t* dest, PathData* asset, zonetool::h1::PathData* new_asset, 
				std::unordered_map<pathnode_t*, zonetool::h1::pathnode_t*>& pathnode_map)
			{
				if (pathnode_map.contains(node))
				{
					return;
				}
				pathnode_map[node] = dest;

				dest->constant.type = node->constant.type; // convert?
				dest->constant.spawnflags = node->constant.spawnflags;
				std::memcpy(&dest->constant.targetname, &node->constant.targetname, sizeof(scr_string_t) * 5); // copy strings
				dest->constant.animscriptfunc = node->constant.animscriptfunc;
				std::memcpy(&dest->constant.vLocalOrigin, &node->constant.vLocalOrigin, sizeof(float[3]));
				std::memcpy(&dest->constant.___u9, &node->constant.___u9, 12);
				std::memcpy(&dest->constant.parent, &node->constant.parent, 4);
				std::memcpy(&dest->constant.___u11, &node->constant.___u11, 4);
				std::memcpy(&dest->constant.wOverlapNode, &node->constant.wOverlapNode, sizeof(short[2]));
				dest->constant.totalLinkCount = node->constant.totalLinkCount;
				dest->constant.Links = reinterpret_cast<zonetool::h1::pathlink_s*>(node->constant.Links);
				dest->constant.unk = zonetool::h1::scr_string_t(0);

				std::memcpy(&dest->dynamic, &node->dynamic, sizeof(pathnode_dynamic_t));
				std::memcpy(&dest->transient, &node->transient, sizeof(pathnode_transient_t));

				if (node->transient.pNextOpen)
				{
					auto index = ((reinterpret_cast<std::uintptr_t>(node->transient.pNextOpen) - reinterpret_cast<std::uintptr_t>(asset->nodes)) / sizeof(zonetool::iw6::pathnode_t));
					convert_node(node->transient.pNextOpen, &new_asset->nodes[index], asset, new_asset, pathnode_map);
					dest->transient.pNextOpen = &new_asset->nodes[index];
				}
				if (node->transient.pPrevOpen)
				{
					auto index = ((reinterpret_cast<std::uintptr_t>(node->transient.pPrevOpen) - reinterpret_cast<std::uintptr_t>(asset->nodes)) / sizeof(zonetool::iw6::pathnode_t));
					convert_node(node->transient.pPrevOpen, &new_asset->nodes[index], asset, new_asset, pathnode_map);
					dest->transient.pPrevOpen = &new_asset->nodes[index];
				}
				if (node->transient.pParent)
				{
					auto index = ((reinterpret_cast<std::uintptr_t>(node->transient.pParent) - reinterpret_cast<std::uintptr_t>(asset->nodes)) / sizeof(zonetool::iw6::pathnode_t));
					convert_node(node->transient.pParent, &new_asset->nodes[index], asset, new_asset, pathnode_map);
					dest->transient.pParent = &new_asset->nodes[index];
				}
			};

			zonetool::h1::PathData* convert(PathData* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::PathData>();

				REINTERPRET_CAST_SAFE(name);

				std::unordered_map<pathnode_t*, zonetool::h1::pathnode_t*> pathnode_map;
				new_asset->nodeCount = asset->nodeCount;
				new_asset->nodes = allocator.allocate_array<zonetool::h1::pathnode_t>(asset->nodeCount);
				for (auto i = 0u; i < asset->nodeCount; i++)
				{
					const auto node = &asset->nodes[i];
					const auto new_node = &new_asset->nodes[i];

					convert_node(node, new_node, asset, new_asset, pathnode_map);
				}

				new_asset->parentIndexResolved = asset->parentIndexResolved;
				new_asset->version = 26;
				new_asset->visBytes = asset->visBytes;
				new_asset->pathVis = asset->pathVis;
				new_asset->nodeTreeCount = asset->nodeTreeCount;
				new_asset->nodeTree = reinterpret_cast<zonetool::h1::pathnode_tree_t*>(asset->nodeTree);
				new_asset->dynamicNodeGroupCount = asset->dynamicNodeGroupCount;
				new_asset->dynamicNodeGroups = reinterpret_cast<zonetool::h1::PathDynamicNodeGroup*>(asset->dynamicNodeGroups);
				new_asset->exposureBytes = asset->exposureBytes;
				new_asset->pathExposure = asset->pathExposure;
				new_asset->noPeekVisBytes = asset->noPeekVisBytes;
				new_asset->pathNoPeekVis = asset->pathNoPeekVis;
				new_asset->zoneCount = asset->zoneCount;
				new_asset->zonesBytes = asset->zonesBytes;
				new_asset->pathZones = asset->pathZones;
				new_asset->dynStatesBytes = asset->dynStatesBytes;
				new_asset->pathDynStates = asset->pathDynStates;

				return new_asset;
			}

			void dump(PathData* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::path_data::dump(converted_asset);
			}
		}
	}
}
