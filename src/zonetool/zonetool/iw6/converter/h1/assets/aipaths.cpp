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
			std::unordered_map<std::uint32_t, const char*> node_type_names =
			{
				{NODE_ERROR, "NODE_ERROR"},
				{NODE_PATHNODE, "NODE_PATHNODE"},
				{NODE_COVER_STAND, "NODE_COVER_STAND"},
				{NODE_COVER_CROUCH, "NODE_COVER_CROUCH"},
				{NODE_COVER_CROUCH_WINDOW, "NODE_COVER_CROUCH_WINDOW"},
				{NODE_COVER_PRONE, "NODE_COVER_PRONE"},
				{NODE_COVER_RIGHT, "NODE_COVER_RIGHT"},
				{NODE_COVER_LEFT, "NODE_COVER_LEFT"},
				{NODE_COVER_MULTI, "NODE_COVER_MULTI"},
				{NODE_AMBUSH, "NODE_AMBUSH"},
				{NODE_EXPOSED, "NODE_EXPOSED"},
				{NODE_CONCEALMENT_STAND, "NODE_CONCEALMENT_STAND"},
				{NODE_CONCEALMENT_CROUCH, "NODE_CONCEALMENT_CROUCH"},
				{NODE_CONCEALMENT_PRONE, "NODE_CONCEALMENT_PRONE"},
				{NODE_DOOR, "NODE_DOOR"},
				{NODE_DOOR_INTERIOR, "NODE_DOOR_INTERIOR"},
				{NODE_SCRIPTED, "NODE_SCRIPTED"},
				{NODE_NEGOTIATION_BEGIN, "NODE_NEGOTIATION_BEGIN"},
				{NODE_NEGOTIATION_END, "NODE_NEGOTIATION_END"},
				{NODE_TURRET, "NODE_TURRET"},
				{NODE_GUARD, "NODE_GUARD"},
				{NODE_PATHNODE_3D, "NODE_PATHNODE_3D"},
				{NODE_COVER_UP_3D, "NODE_COVER_UP_3D"},
				{NODE_COVER_RIGHT_3D, "NODE_COVER_RIGHT_3D"},
				{NODE_COVER_LEFT_3D, "NODE_COVER_LEFT_3D"},
				{NODE_EXPOSED_3D, "NODE_EXPOSED_3D"},
				{NODE_SCRIPTED_3D, "NODE_SCRIPTED_3D"},
				{NODE_NEGOTIATION_BEGIN_3D, "NODE_NEGOTIATION_BEGIN_3D"},
				{NODE_NEGOTIATION_END_3D, "NODE_NEGOTIATION_END_3D"},
				{NODE_JUMP, "NODE_JUMP"},
				{NODE_JUMP_ATTACK, "NODE_JUMP_ATTACK"},
				{NODE_NUMTYPES, "NODE_NUMTYPES"},
				{NODE_DONTLINK, "NODE_DONTLINK"},
			};

			std::unordered_map<std::uint16_t, std::uint16_t> mapped_node_types =
			{
				{NODE_ERROR, zonetool::h1::NODE_ERROR},
				{NODE_PATHNODE, zonetool::h1::NODE_PATHNODE},
				{NODE_COVER_STAND, zonetool::h1::NODE_COVER_STAND},
				{NODE_COVER_CROUCH, zonetool::h1::NODE_COVER_CROUCH},
				{NODE_COVER_CROUCH_WINDOW, zonetool::h1::NODE_COVER_CROUCH_WINDOW},
				{NODE_COVER_PRONE, zonetool::h1::NODE_COVER_PRONE},
				{NODE_COVER_RIGHT, zonetool::h1::NODE_COVER_RIGHT},
				{NODE_COVER_LEFT, zonetool::h1::NODE_COVER_LEFT},
				{NODE_COVER_MULTI, zonetool::h1::NODE_COVER_MULTI},
				{NODE_AMBUSH, zonetool::h1::NODE_AMBUSH},
				{NODE_EXPOSED, zonetool::h1::NODE_EXPOSED},
				{NODE_CONCEALMENT_STAND, zonetool::h1::NODE_CONCEALMENT_STAND},
				{NODE_CONCEALMENT_CROUCH, zonetool::h1::NODE_CONCEALMENT_CROUCH},
				{NODE_CONCEALMENT_PRONE, zonetool::h1::NODE_CONCEALMENT_PRONE},
				{NODE_DOOR, zonetool::h1::NODE_DOOR},
				{NODE_DOOR_INTERIOR, zonetool::h1::NODE_DOOR_INTERIOR},
				{NODE_SCRIPTED, zonetool::h1::NODE_SCRIPTED},
				{NODE_NEGOTIATION_BEGIN, zonetool::h1::NODE_NEGOTIATION_BEGIN},
				{NODE_NEGOTIATION_END, zonetool::h1::NODE_NEGOTIATION_END},
				{NODE_TURRET, zonetool::h1::NODE_TURRET},
				{NODE_GUARD, zonetool::h1::NODE_GUARD},
				{NODE_PATHNODE_3D, zonetool::h1::NODE_PATHNODE_3D},
				{NODE_COVER_UP_3D, zonetool::h1::NODE_COVER_UP_3D},
				{NODE_COVER_RIGHT_3D, zonetool::h1::NODE_COVER_RIGHT_3D},
				{NODE_COVER_LEFT_3D, zonetool::h1::NODE_COVER_LEFT_3D},
				{NODE_EXPOSED_3D, zonetool::h1::NODE_EXPOSED_3D},
				{NODE_SCRIPTED_3D, zonetool::h1::NODE_SCRIPTED_3D},
				{NODE_NEGOTIATION_BEGIN_3D, zonetool::h1::NODE_NEGOTIATION_BEGIN_3D},
				{NODE_NEGOTIATION_END_3D, zonetool::h1::NODE_NEGOTIATION_END_3D},
				//{NODE_JUMP, zonetool::h1::NODE_JUMP},
				//{NODE_JUMP_ATTACK, zonetool::h1::NODE_JUMP_ATTACK},
				{NODE_DONTLINK, zonetool::h1::NODE_DONTLINK},
			};

			std::uint16_t convert_type(const std::uint16_t type)
			{
				const auto iter = mapped_node_types.find(type);
				if (iter == mapped_node_types.end())
				{
					ZONETOOL_ERROR("Unable to map pathnode type %s, using NODE_PATHNODE", node_type_names[type]);
					return static_cast<std::uint16_t>(zonetool::h1::NODE_PATHNODE);
				}
				else
				{
					return iter->second;
				}
			}

			void convert_node(pathnode_t* node, zonetool::h1::pathnode_t* dest, PathData* asset, zonetool::h1::PathData* new_asset, 
				std::unordered_map<pathnode_t*, zonetool::h1::pathnode_t*>& pathnode_map)
			{
				if (pathnode_map.contains(node))
				{
					return;
				}
				pathnode_map[node] = dest;
				dest->constant.type = convert_type(node->constant.type);
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
				dest->constant.customangles = zonetool::h1::scr_string_t(0);

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
