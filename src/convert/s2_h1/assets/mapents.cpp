#include <std_include.hpp>

#include "../include.hpp"
#include "mapents.hpp"
#include "scriptfile.hpp"

#include <h1/assets/mapents.hpp>
#include <h1/assets/netconststrings.hpp>

namespace zonetool::s2::converter::h1::mapents
{
	namespace
	{
		template <typename T>
		T* allocate_zeroed(utils::memory::allocator& allocator, const std::size_t count)
		{
			return count ? allocator.allocate_array<T>(count) : nullptr;
		}

		template <typename T, typename S>
		T* copy_array(const S* source, const std::size_t count, utils::memory::allocator& allocator)
		{
			static_assert(sizeof(T) == sizeof(S));
			if (!source || !count)
			{
				return nullptr;
			}

			const auto array = allocator.allocate_array<T>(count);
			std::memcpy(array, source, sizeof(T) * count);
			return array;
		}

		template <typename T>
		short* narrow_array(const T* source, const std::size_t count, utils::memory::allocator& allocator)
		{
			if (!source || !count)
			{
				return nullptr;
			}

			const auto array = allocator.allocate_array<short>(count);
			for (auto i = 0u; i < count; i++)
			{
				array[i] = static_cast<short>(source[i]);
			}

			return array;
		}

		void convert_triggers(const MapTriggers& trigger, zonetool::h1::MapTriggers& new_trigger, utils::memory::allocator& allocator)
		{
			new_trigger.count = trigger.count;
			new_trigger.models = allocate_zeroed<zonetool::h1::TriggerModel>(allocator, trigger.count);
			for (auto i = 0u; i < trigger.count; i++)
			{
				new_trigger.models[i].contents = static_cast<int>(trigger.models[i].contents);
				new_trigger.models[i].hullCount = trigger.models[i].hullCount;
				new_trigger.models[i].firstHull = trigger.models[i].firstHull;
			}

			new_trigger.hullCount = trigger.hullCount;
			new_trigger.hulls = allocate_zeroed<zonetool::h1::TriggerHull>(allocator, trigger.hullCount);
			for (auto i = 0u; i < trigger.hullCount; i++)
			{
				new_trigger.hulls[i].bounds = std::bit_cast<zonetool::h1::Bounds>(trigger.hulls[i].bounds);
				new_trigger.hulls[i].contents = static_cast<int>(trigger.hulls[i].contents);
				new_trigger.hulls[i].slabCount = trigger.hulls[i].slabCount;
				new_trigger.hulls[i].firstSlab = trigger.hulls[i].firstSlab;
			}

			new_trigger.slabCount = trigger.slabCount;
			new_trigger.slabs = copy_array<zonetool::h1::TriggerSlab>(trigger.slabs, trigger.slabCount, allocator);
		}

		void convert_client_triggers(const ClientTriggers& trigger, zonetool::h1::ClientTriggers& new_trigger, utils::memory::allocator& allocator)
		{
			convert_triggers(trigger.trigger, new_trigger.trigger, allocator);

			const auto count = trigger.trigger.count;
			new_trigger.numClientTriggerNodes = trigger.numClientTriggerNodes;
			new_trigger.clientTriggerAabbTree = copy_array<zonetool::h1::ClientTriggerAabbNode>(trigger.clientTriggerAabbTree, trigger.numClientTriggerNodes, allocator);
			new_trigger.triggerStringLength = trigger.triggerStringLength;
			new_trigger.triggerString = copy_array<char>(trigger.triggerString, trigger.triggerStringLength, allocator);
			new_trigger.visionSetTriggers = narrow_array(trigger.visionSetTriggers, count, allocator);
			new_trigger.lightSetTriggers = narrow_array(trigger.lightSetTriggers, count, allocator);
			new_trigger.clutTriggers = narrow_array(trigger.clutTriggers, count, allocator);
			new_trigger.triggerType = copy_array<short>(trigger.triggerType, count, allocator);
			new_trigger.origins = copy_array<vec3_t>(trigger.origins, count, allocator);
			new_trigger.scriptDelay = copy_array<float>(trigger.scriptDelay, count, allocator);
			new_trigger.audioTriggers = copy_array<short>(trigger.audioTriggers, count, allocator);
			new_trigger.blendLookup = copy_array<short>(trigger.blendLookup, count, allocator);
			new_trigger.npcTriggers = copy_array<short>(trigger.npcTriggers, count, allocator);
			new_trigger.contextTriggers = copy_array<short>(trigger.contextTriggers, count, allocator);
			new_trigger.waterTriggers = copy_array<short>(trigger.waterTriggers, count, allocator);
			new_trigger.unkTriggers = allocate_zeroed<short>(allocator, count);
		}

		std::string replace_entity_keys(const std::string& data)
		{
			std::string buffer{};
			for (const auto& line : utils::string::split(data.substr(0, data.find('\0')), '\n'))
			{
				const auto first_space = line.find_first_of(' ');
				const auto id = static_cast<std::uint32_t>(std::atoi(line.data()));
				const auto has_key_id = !line.starts_with("{") && !line.starts_with("}") && !line.starts_with("\"") && first_space != std::string::npos && id != 0;
				const auto key = has_key_id ? scriptfile::s2_context().token_name(id) : std::string{};
				if (key.starts_with("_id_"))
				{
					ZONETOOL_WARNING("map_ents: key \"%s\" has no name, dropping it", key.data());
					continue;
				}

				if (!buffer.empty())
				{
					buffer.append("\n");
				}

				buffer.append(has_key_id ? "\"" + key + "\"" + line.substr(first_space) : line);
			}

			return buffer;
		}

		zonetool::h1::ClientTriggerBlendNode* convert_blend_nodes(const ClientTriggerBlend& blend, utils::memory::allocator& allocator)
		{
			const auto nodes = copy_array<zonetool::h1::ClientTriggerBlendNode>(blend.blendNodes, blend.numClientTriggerBlendNodes, allocator);
			for (auto i = 0; nodes && i < blend.numClientTriggerBlendNodes; i++)
			{
				nodes[i].triggerA = nodes[i].triggerA == CLIENT_TRIGGER_BLEND_NO_TRIGGER ? zonetool::h1::CLIENT_TRIGGER_BLEND_NO_TRIGGER : nodes[i].triggerA;
				nodes[i].triggerB = nodes[i].triggerB == CLIENT_TRIGGER_BLEND_NO_TRIGGER ? zonetool::h1::CLIENT_TRIGGER_BLEND_NO_TRIGGER : nodes[i].triggerB;
			}

			return nodes;
		}

		std::string remove_turrets(const std::string& data) // TODO: h1 has no s2 turret weapons
		{
			std::string buffer{};
			std::string entity{};
			for (const auto& line : utils::string::split(data, '\n'))
			{
				entity.append(line + "\n");
				if (!line.starts_with("}"))
				{
					continue;
				}

				if (entity.find("\"classname\" \"misc_turret\"") == std::string::npos)
				{
					buffer.append(entity);
				}

				entity.clear();
			}

			buffer.append(entity);
			while (buffer.ends_with("\n"))
			{
				buffer.pop_back();
			}

			return buffer;
		}

		zonetool::h1::MapEnts* convert(MapEnts* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::h1::MapEnts>();
			new_asset->name = asset->name;
			const auto entity_string = remove_turrets(replace_entity_keys({asset->entityString, static_cast<std::size_t>(asset->numEntityChars)}));
			new_asset->numEntityChars = static_cast<int>(entity_string.size() + 1);
			new_asset->entityString = allocator.allocate_array<char>(new_asset->numEntityChars);
			std::memcpy(new_asset->entityString, entity_string.data(), entity_string.size());
			convert_triggers(asset->trigger, new_asset->trigger, allocator);
			convert_client_triggers(asset->clientTrigger, new_asset->clientTrigger, allocator);

			new_asset->clientTriggerBlend.numClientTriggerBlendNodes = asset->clientTriggerBlend.numClientTriggerBlendNodes;
			new_asset->clientTriggerBlend.blendNodes = convert_blend_nodes(asset->clientTriggerBlend, allocator);

			new_asset->spawnList.spawnsCount = asset->spawnList.spawnsCount;
			new_asset->spawnList.spawns = copy_array<zonetool::h1::SpawnPointEntityRecord>(asset->spawnList.spawns, asset->spawnList.spawnsCount, allocator);

			new_asset->splineList.splineCount = asset->splineList.splineCount;
			new_asset->splineList.splines = reinterpret_cast<zonetool::h1::SplinePointRecordList*>(asset->splineList.splines);
			return new_asset;
		}

		constexpr const char* level_net_const_string_tags[zonetool::h1::NETCONSTSTRINGTYPE_ASSET_COUNT]
		{
			"mdl", "mat", "rmb", "veh", "vfx", "loc", "snd", "sbx", "snl", "shk", "mnu", "tag",
			"hic", "nps", "mic", "sel", "wep", "att", "hnt", "anm", "fxt", "acl", "lui", "lsr",
		};

		constexpr const char* gametype_models[]
		{
			"com_bomb_objective",
			"com_bomb_objective_d",
			"com_cellphone_on",
			"com_plasticcase_beige_big",
			"h1_flag_mp_ctf_arab_blue",
			"h1_flag_mp_ctf_arab_red",
			"laptop_toughbook_01_open",
		};

		void dump_level_net_const_strings()
		{
			for (auto type = 0; type < zonetool::h1::NETCONSTSTRINGTYPE_ASSET_COUNT; type++)
			{
				const auto name = std::format("ncs_{}_level", level_net_const_string_tags[type]);

				zonetool::h1::NetConstStrings asset{};
				asset.name = name.data();
				asset.stringType = static_cast<zonetool::h1::NetConstStringType>(type);
				asset.sourceType = zonetool::h1::NETCONSTSTRINGSOURCE_MAP;
				if (type == zonetool::h1::NETCONSTSTRINGTYPE_XMODEL)
				{
					asset.entryCount = static_cast<unsigned int>(std::size(gametype_models));
					asset.stringList = const_cast<const char**>(gametype_models);
				}

				zonetool::h1::net_const_strings::dump(&asset);
				add_csv_row("netconststrings", name);
			}
		}
	}

	void dump(MapEnts* asset)
	{
		utils::memory::allocator allocator;
		const auto new_asset = convert(asset, allocator);
		zonetool::h1::map_ents::dump(new_asset);
		add_csv_row("map_ents", new_asset->name);
		const std::string name = new_asset->name;
		scriptfile::add_default_map_script(name.substr(0, name.find(".d3dbsp")) + ".gsc");
		dump_level_net_const_strings();
	}
}
