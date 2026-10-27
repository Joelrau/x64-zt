#include <std_include.hpp>

#include "../include.hpp"
#include "aipaths.hpp"

namespace zonetool::h1::converter::s2::aipaths
{
	namespace
	{
		constexpr auto s2_path_version = 29;
		constexpr std::uint8_t h1_no_zone = 0xFF;
		constexpr std::uint16_t s2_no_zone = 0xFFFF;

		zonetool::s2::nodeType convert_type(const std::uint16_t type)
		{
			switch (type)
			{
			case NODE_COVER_WIDE_RIGHT:
				return zonetool::s2::NODE_COVER_RIGHT;
			case NODE_COVER_WIDE_LEFT:
				return zonetool::s2::NODE_COVER_LEFT;
			case NODE_COVER_MULTI:
				return zonetool::s2::NODE_COVER_MULTI;
			default:
				break;
			}

			if (type < NODE_COVER_WIDE_RIGHT)
			{
				return static_cast<zonetool::s2::nodeType>(type);
			}

			if (type < NODE_NUMTYPES)
			{
				return static_cast<zonetool::s2::nodeType>(type - 2);
			}

			return zonetool::s2::NODE_ERROR;
		}

		void convert_node(const pathnode_t& node, zonetool::s2::pathnode_t& result)
		{
			const auto& constant = node.constant;
			auto& dest = result.constant;
			static_assert(sizeof(constant.___u9) == sizeof(dest.orientation));
			static_assert(sizeof(constant.___u11) == sizeof(dest.use));

			dest.type = convert_type(constant.type);
			dest.spawnflags = constant.spawnflags;
			dest.targetname = static_cast<zonetool::s2::scr_string_t>(constant.targetname);
			dest.script_linkName = static_cast<zonetool::s2::scr_string_t>(constant.script_linkName);
			dest.script_noteworthy = static_cast<zonetool::s2::scr_string_t>(constant.script_noteworthy);
			dest.target = static_cast<zonetool::s2::scr_string_t>(constant.target);
			dest.animscript = static_cast<zonetool::s2::scr_string_t>(constant.animscript);
			dest.animscriptfunc = constant.animscriptfunc;
			std::memcpy(dest.vLocalOrigin, constant.vLocalOrigin, sizeof(dest.vLocalOrigin));
			std::memcpy(&dest.orientation, &constant.___u9, sizeof(dest.orientation));
			std::memcpy(&dest.parent, &constant.parent, sizeof(dest.parent));
			std::memcpy(&dest.use, &constant.___u11, sizeof(dest.use));
			std::memcpy(dest.wOverlapNode, constant.wOverlapNode, sizeof(dest.wOverlapNode));
			dest.totalLinkCount = constant.totalLinkCount;
			dest.Links = retype<zonetool::s2::pathlink_s>(constant.Links);
		}

		unsigned short* convert_zones(const PathData* asset, utils::memory::allocator& allocator)
		{
			if (!asset->zoneCount || !asset->pathZones)
			{
				return nullptr;
			}

			if (asset->zonesBytes != static_cast<int>(asset->nodeCount))
			{
				throw std::runtime_error(std::format("{} zone bytes for {} nodes", asset->zonesBytes, asset->nodeCount));
			}

			const auto zones = allocator.allocate_array<unsigned short>(asset->nodeCount);
			for (auto i = 0u; i < asset->nodeCount; i++)
			{
				zones[i] = asset->pathZones[i] == h1_no_zone ? s2_no_zone : asset->pathZones[i];
			}

			return zones;
		}

		zonetool::s2::PathData* convert(PathData* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::s2::PathData>();
			new_asset->name = asset->name;
			new_asset->nodeCount = asset->nodeCount;
			new_asset->nodes = allocator.allocate_array<zonetool::s2::pathnode_t>(asset->nodeCount);
			for (auto i = 0u; i < asset->nodeCount; i++)
			{
				convert_node(asset->nodes[i], new_asset->nodes[i]);
			}

			new_asset->parentIndexResolved = asset->parentIndexResolved;
			new_asset->version = s2_path_version;
			new_asset->visBytes = asset->visBytes;
			new_asset->pathVis = asset->pathVis;
			new_asset->nodeTreeCount = asset->nodeTreeCount;
			new_asset->nodeTree = retype<zonetool::s2::pathnode_tree_t>(asset->nodeTree);
			new_asset->dynamicNodeGroupCount = asset->dynamicNodeGroupCount;
			new_asset->dynamicNodeGroups = retype<zonetool::s2::PathDynamicNodeGroup>(asset->dynamicNodeGroups);
			new_asset->exposureBytes = asset->exposureBytes;
			new_asset->pathExposure = asset->pathExposure;
			new_asset->noPeekVisBytes = asset->noPeekVisBytes;
			new_asset->pathNoPeekVis = asset->pathNoPeekVis;
			new_asset->zoneCount = asset->zoneCount;
			new_asset->pathZones = convert_zones(asset, allocator);
			new_asset->dynStatesBytes = asset->dynStatesBytes;
			new_asset->pathDynStates = asset->pathDynStates;
			return new_asset;
		}
	}

	void dump(PathData* asset)
	{
		utils::memory::allocator allocator;
		const auto converted = convert(asset, allocator);
		zonetool::s2::path_data::dump(converted);
		add_csv_row(zonetool::s2::type_to_string(zonetool::s2::ASSET_TYPE_PATHDATA), asset->name);
	}
}
