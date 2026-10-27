#include <std_include.hpp>

#include "../include.hpp"
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
			constexpr auto h1_no_trigger = 0x400;
			constexpr auto s2_no_trigger = 0x800;

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
				node.triggerA = node.triggerA == h1_no_trigger ? s2_no_trigger : node.triggerA;
				node.triggerB = node.triggerB == h1_no_trigger ? s2_no_trigger : node.triggerB;
			}

			return new_blend;
		}

		std::optional<std::string> convert_key(const std::string& name)
		{
			// s2 resolves a quoted key name to its canonical string
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

		zonetool::s2::MapEnts* convert(MapEnts* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::s2::MapEnts>();
			new_asset->name = asset->name;

			const auto entity_string = replace_entity_keys({asset->entityString, static_cast<std::size_t>(asset->numEntityChars)});
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
