#include <std_include.hpp>
#include "zonetool/s1/converter/h1/include.hpp"
#include "aipaths.hpp"

#include "zonetool/h1/assets/aipaths.hpp"

#include "zonetool/s1/functions.hpp"

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace aipaths
		{
			zonetool::h1::PathData* convert(PathData* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::PathData>();

				REINTERPRET_CAST_SAFE(name);

				new_asset->nodeCount = asset->nodeCount;
				new_asset->nodes = allocator.allocate_array<zonetool::h1::pathnode_t>(asset->nodeCount);
				for (auto i = 0u; i < asset->nodeCount; i++)
				{
					// something is wrong in these structs
					const auto node = &asset->nodes[i];
					const auto new_node = &new_asset->nodes[i];

					new_node->constant.type = node->constant.type; // convert?
					new_node->constant.spawnflags = node->constant.spawnflags;
					std::memcpy(&new_node->constant.targetname, &node->constant.targetname, sizeof(scr_string_t) * 5); // copy strings
					new_node->constant.animscriptfunc = node->constant.animscriptfunc;
					std::memcpy(&new_node->constant.vLocalOrigin, &node->constant.vLocalOrigin, sizeof(float[3]));
					std::memcpy(&new_node->constant.___u9, &node->constant.___u9, 12);
					std::memcpy(&new_node->constant.parent, &node->constant.parent, 4);
					std::memcpy(&new_node->constant.___u11, &node->constant.___u11, 4);
					std::memcpy(&new_node->constant.wOverlapNode, &node->constant.wOverlapNode, sizeof(short[2]));
					new_node->constant.totalLinkCount = node->constant.totalLinkCount;
					new_node->constant.Links = reinterpret_cast<zonetool::h1::pathlink_s*>(node->constant.Links);
					new_node->constant.unk = zonetool::h1::scr_string_t(0);

					// pad0, pad1

					std::memcpy(&new_node->dynamic, &node->dynamic, sizeof(pathnode_dynamic_t));
					std::memcpy(&new_node->transient, &node->transient, sizeof(pathnode_transient_t));
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
