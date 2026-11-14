#include <std_include.hpp>

#include "../include.hpp"
#include "clipmap.hpp"
#include "mapents.hpp"
#include "netconststrings.hpp"
#include "scriptable.hpp"
#include "scriptfile.hpp"

namespace zonetool::h1::converter::s2::mapents
{
	namespace
	{
		using scriptfile::h1_context;

		int* widen_array(const short* source, const std::size_t count, utils::memory::allocator& allocator)
		{
			if (!source || !count)
			{
				return nullptr;
			}

			const auto array = allocator.allocate_array<int>(count);
			for (auto i = 0u; i < count; i++)
			{
				array[i] = source[i];
			}

			return array;
		}

		void convert_client_triggers(const ClientTriggers& trigger, zonetool::s2::ClientTriggers& new_trigger, utils::memory::allocator& allocator)
		{
			new_trigger.trigger = convert_triggers(trigger.trigger, allocator);

			const auto count = trigger.trigger.count;
			new_trigger.numClientTriggerNodes = trigger.numClientTriggerNodes;
			new_trigger.clientTriggerAabbTree = retype<zonetool::s2::ClientTriggerAabbNode>(trigger.clientTriggerAabbTree);
			new_trigger.triggerStringLength = trigger.triggerStringLength;
			new_trigger.triggerString = trigger.triggerString;
			new_trigger.visionSetTriggers = widen_array(trigger.visionSetTriggers, count, allocator);
			new_trigger.lightSetTriggers = widen_array(trigger.lightSetTriggers, count, allocator);
			new_trigger.clutTriggers = widen_array(trigger.clutTriggers, count, allocator);
			new_trigger.triggerType = trigger.triggerType;
			new_trigger.origins = retype<zonetool::s2::vec3_t>(trigger.origins);
			new_trigger.scriptDelay = trigger.scriptDelay;
			new_trigger.audioTriggers = trigger.audioTriggers;
			new_trigger.blendLookup = trigger.blendLookup;
			new_trigger.npcTriggers = trigger.npcTriggers;
			new_trigger.contextTriggers = trigger.contextTriggers;
			new_trigger.waterTriggers = trigger.waterTriggers;
		}

		zonetool::s2::ClientTriggerBlend convert_blend_nodes(const ClientTriggerBlend& blend, utils::memory::allocator& allocator)
		{
			zonetool::s2::ClientTriggerBlend new_blend{};
			new_blend.numClientTriggerBlendNodes = blend.numClientTriggerBlendNodes;
			if (!blend.blendNodes || !blend.numClientTriggerBlendNodes)
			{
				return new_blend;
			}

			new_blend.blendNodes = allocator.allocate_array<zonetool::s2::ClientTriggerBlendNode>(blend.numClientTriggerBlendNodes);
			for (auto i = 0; i < blend.numClientTriggerBlendNodes; i++)
			{
				auto& node = new_blend.blendNodes[i];
				node = std::bit_cast<zonetool::s2::ClientTriggerBlendNode>(blend.blendNodes[i]);
				node.triggerA = node.triggerA == CLIENT_TRIGGER_BLEND_NO_TRIGGER ? zonetool::s2::CLIENT_TRIGGER_BLEND_NO_TRIGGER : node.triggerA;
				node.triggerB = node.triggerB == CLIENT_TRIGGER_BLEND_NO_TRIGGER ? zonetool::s2::CLIENT_TRIGGER_BLEND_NO_TRIGGER : node.triggerB;
			}

			return new_blend;
		}

		std::optional<std::string> convert_key(const std::string& name)
		{
			if (name.starts_with("_id_"))
			{
				ZONETOOL_WARNING("map_ents: key \"%s\" has no name, dropping it", name.data());
				return {};
			}

			return std::format("\"{}\"", name);
		}

		std::optional<std::string> convert_key(const std::uint32_t h1_id)
		{
			return convert_key(h1_context().token_name(h1_id));
		}

		std::optional<std::string> convert_line(const std::string& line)
		{
			if (line.starts_with("\""))
			{
				const auto key_end = line.find('"', 1);
				if (key_end == std::string::npos)
				{
					return line;
				}

				const auto key = convert_key(line.substr(1, key_end - 1));
				return key ? std::optional{*key + line.substr(key_end + 1)} : std::nullopt;
			}

			const auto first_space = line.find_first_of(' ');
			const auto id = static_cast<std::uint32_t>(std::atoi(line.data()));
			if (line.starts_with("{") || line.starts_with("}") || first_space == std::string::npos || id == 0)
			{
				return line;
			}

			const auto key = convert_key(id);
			return key ? std::optional{*key + line.substr(first_space)} : std::nullopt;
		}

		std::string replace_entity_keys(const std::string& data)
		{
			std::string buffer{};
			for (const auto& line : utils::string::split(data.substr(0, data.find('\0')), '\n'))
			{
				const auto new_line = convert_line(line);
				if (!new_line)
				{
					continue;
				}

				if (!buffer.empty())
				{
					buffer.append("\n");
				}

				buffer.append(*new_line);
			}

			return buffer;
		}

		std::string care_package_entities(const std::string& map_name)
		{
			const auto model = clipmap::care_package_model(map_name);
			const auto* clip_map = zonetool::h1::db_find_x_asset_header_safe(ASSET_TYPE_CLIPMAP, map_name).clipMap;
			if (!model || !clip_map->numSubModels)
			{
				return {};
			}

			const auto& world = clip_map->cmodels[0].bounds;
			const auto x = std::round(world.midPoint[0]);
			const auto y = std::round(world.midPoint[1]);
			const auto z = std::floor(world.midPoint[2] - world.halfSize[2]) - 512.0f;
			ZONETOOL_INFO("map_ents \"%s\": added care package collision *%u at %g %g %g", map_name.data(), *model, x, y, z);

			return std::format("\n{{\n\"target\" \"care_package_collision\"\n\"targetname\" \"care_package\"\n\"origin\" \"{} {} {}\"\n"
				"\"classname\" \"script_model\"\n\"model\" \"ger_carepackage_crate\"\n}}\n{{\n\"script_gameobjectname\" \"airdrop_pallet\"\n"
				"\"targetname\" \"care_package_collision\"\n\"classname\" \"script_brushmodel\"\n\"origin\" \"{} {} {}\"\n\"model\" \"*{}\"\n}}",
				x, y, z + 15.0f, x, y, z, *model);
		}

		std::vector<std::array<float, 3>> minimap_corners(const std::string& entities)
		{
			std::vector<std::array<float, 3>> corners;
			for (auto start = entities.find('{'); start != std::string::npos; start = entities.find('{', start + 1))
			{
				const auto end = entities.find('}', start);
				const auto entity = entities.substr(start, end - start);
				const auto origin = entity.find("\"origin\" \"");
				if (entity.find("\"targetname\" \"minimap_corner\"") == std::string::npos || origin == std::string::npos)
				{
					continue;
				}

				std::array<float, 3> corner{};
				if (std::sscanf(entity.data() + origin + 10, "%f %f %f", &corner[0], &corner[1], &corner[2]) == 3)
				{
					corners.emplace_back(corner);
				}
			}

			return corners;
		}

		std::string remote_missile_entities(const MapEnts* asset, const std::string& entities)
		{
			if (!std::string_view(asset->name).starts_with("maps/mp/") || entities.find("\"remoteMissileSpawn\"") != std::string::npos)
			{
				return {};
			}

			std::array<float, 2> mins{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
			std::array<float, 2> maxs{-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()};
			const auto add_point = [&](const float x, const float y)
			{
				mins = {std::min(mins[0], x), std::min(mins[1], y)};
				maxs = {std::max(maxs[0], x), std::max(maxs[1], y)};
			};

			const auto corners = minimap_corners(entities);
			std::vector<float> heights;
			for (auto i = 0; asset->spawnList.spawns && i < asset->spawnList.spawnsCount; i++)
			{
				const auto& origin = asset->spawnList.spawns[i].origin;
				heights.emplace_back(origin[2]);
				if (corners.size() < 2)
				{
					add_point(origin[0], origin[1]);
				}
			}

			for (const auto& corner : corners)
			{
				add_point(corner[0], corner[1]);
			}

			if (heights.empty() || mins[0] > maxs[0])
			{
				return {};
			}

			std::ranges::nth_element(heights, heights.begin() + heights.size() / 2);
			const auto ground = std::round(heights[heights.size() / 2]);
			const auto center_x = std::round((mins[0] + maxs[0]) * 0.5f);
			const auto center_y = std::round((mins[1] + maxs[1]) * 0.5f);
			const auto quarter_x = std::round((maxs[0] - mins[0]) * 0.25f);
			const auto quarter_y = std::round((maxs[1] - mins[1]) * 0.25f);

			std::string result;
			constexpr std::array<std::array<float, 2>, 4> quadrants{{{1.0f, 1.0f}, {-1.0f, 1.0f}, {-1.0f, -1.0f}, {1.0f, -1.0f}}};
			for (auto i = 0u; i < quadrants.size(); i++)
			{
				const auto target = std::format("remote_missile_target_{}", i);
				result += std::format("\n{{\n\"targetname\" \"{}\"\n\"classname\" \"script_origin\"\n\"origin\" \"{} {} {}\"\n}}", target,
					center_x + quadrants[i][0] * quarter_x, center_y + quadrants[i][1] * quarter_y, ground + 64.0f);
				result += std::format("\n{{\n\"script_noteworthy\" \"{}\"\n\"target\" \"{}\"\n\"targetname\" \"remoteMissileSpawn\"\n\"classname\" \"script_origin\"\n"
					"\"origin\" \"{} {} {}\"\n}}", i, target, center_x, center_y, ground + 3400.0f);
			}

			ZONETOOL_INFO("map_ents \"%s\": added 4 remoteMissileSpawn ents over %g %g, ground %g", asset->name, center_x, center_y, ground);
			return result;
		}

		zonetool::s2::MapEnts* convert(MapEnts* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::s2::MapEnts>();
			new_asset->name = asset->name;

			auto entity_string = replace_entity_keys({asset->entityString, static_cast<std::size_t>(asset->numEntityChars)});
			const auto added = care_package_entities(asset->name) + remote_missile_entities(asset, entity_string);
			if (!added.empty())
			{
				while (entity_string.ends_with('\n'))
				{
					entity_string.pop_back();
				}

				entity_string += added;
			}
			new_asset->numEntityChars = static_cast<int>(entity_string.size() + 1);
			new_asset->entityString = allocator.allocate_array<char>(new_asset->numEntityChars);
			std::memcpy(new_asset->entityString, entity_string.data(), entity_string.size());

			new_asset->trigger = convert_triggers(asset->trigger, allocator);
			convert_client_triggers(asset->clientTrigger, new_asset->clientTrigger, allocator);

			new_asset->clientTriggerBlend = convert_blend_nodes(asset->clientTriggerBlend, allocator);

			new_asset->spawnList.spawnsCount = asset->spawnList.spawnsCount;
			new_asset->spawnList.spawns = retype<zonetool::s2::SpawnPointEntityRecord>(asset->spawnList.spawns);

			new_asset->splineList.splineCount = asset->splineList.splineCount;
			new_asset->splineList.splines = retype<zonetool::s2::SplinePointRecordList>(asset->splineList.splines);

			if (const auto* clip_map = zonetool::h1::db_find_x_asset_header_safe(ASSET_TYPE_CLIPMAP, asset->name).clipMap)
			{
				new_asset->scriptableMapEnts = scriptable::convert_map_ents(clip_map->scriptableMapEnts, allocator);
			}

			return new_asset;
		}
	}

	zonetool::s2::MapTriggers convert_triggers(const MapTriggers& trigger, utils::memory::allocator& allocator)
	{
		zonetool::s2::MapTriggers new_trigger{};
		new_trigger.count = trigger.count;
		new_trigger.models = allocator.allocate_array<zonetool::s2::TriggerModel>(trigger.count);
		for (auto i = 0u; i < trigger.count; i++)
		{
			new_trigger.models[i].contents = static_cast<std::uint32_t>(trigger.models[i].contents);
			new_trigger.models[i].hullCount = trigger.models[i].hullCount;
			new_trigger.models[i].firstHull = trigger.models[i].firstHull;
		}

		new_trigger.hullCount = trigger.hullCount;
		new_trigger.hulls = allocator.allocate_array<zonetool::s2::TriggerHull>(trigger.hullCount);
		for (auto i = 0u; i < trigger.hullCount; i++)
		{
			new_trigger.hulls[i].bounds = std::bit_cast<zonetool::s2::Bounds>(trigger.hulls[i].bounds);
			new_trigger.hulls[i].contents = static_cast<std::uint32_t>(trigger.hulls[i].contents);
			new_trigger.hulls[i].slabCount = trigger.hulls[i].slabCount;
			new_trigger.hulls[i].firstSlab = trigger.hulls[i].firstSlab;
		}

		new_trigger.slabCount = trigger.slabCount;
		new_trigger.slabs = retype<zonetool::s2::TriggerSlab>(trigger.slabs);
		return new_trigger;
	}

	void dump(MapEnts* asset)
	{
		utils::memory::allocator allocator;
		const auto new_asset = convert(asset, allocator);
		zonetool::s2::map_ents::dump(new_asset);
		add_csv_row("map_ents", new_asset->name);
		netconststrings::add_level_strings();
	}
}
