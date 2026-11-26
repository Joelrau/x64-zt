#include <std_include.hpp>

#include "../include.hpp"
#include "dynentitylist.hpp"
#include "mapents.hpp"
#include "xmodel.hpp"
#include "gfxworld.hpp"
#include "havok.hpp"
#include "material.hpp"

#include <t9/assets/triggerlist.hpp>

#include <iw8/assets/mapents.hpp>
#include <iw8/assets/physics.hpp>
#include <iw8/assets/scriptfile.hpp>

#include <utils/compression.hpp>

namespace zonetool::t9::converter::iw8::mapents
{
	namespace
	{
		namespace target = zonetool::iw8;

		const char* trigger_physics_asset = "triggermodeldummydefault";

		script_context& target_context()
		{
			static auto context = []
			{
				auto result = std::make_unique<script_context>();
				result->init(xsk::gsc::build::prod, {});
				return result;
			}();
			return *context;
		}

		std::string_view find_value(const entity& entity, const std::string_view key)
		{
			const auto found = std::ranges::find(entity, key, &std::pair<std::string, std::string>::first);
			return found != entity.end() ? std::string_view(found->second) : std::string_view{};
		}

		bool is_kept_entity(const entity& entity)
		{
			const auto classname = find_value(entity, "classname");
			if (classname == "script_model")
			{
				return !find_value(entity, "model").empty() && find_value(entity, "script_gameobjectname").empty();
			}

			return classname == "worldspawn" || classname == "mp_global_intermission" || classname == "script_origin"
				|| classname == "script_struct";
		}

		bool is_destructible_key(const std::pair<std::string, std::string>& key)
		{
			return key.first == "destructibledef" || (key.first == "script" && key.second.starts_with("destructibledef/"))
				|| (key.first == "targetname" && key.second == "destructible");
		}

		std::string make_entity_text(const entity& entity, unsigned int& dropped_keys)
		{
			auto text = std::string("{\n");
			for (const auto& [key, value] : entity)
			{
				const auto id = target_context().token_id(key);
				if (!id)
				{
					dropped_keys++;
					continue;
				}

				text += std::format("{} \"{}\"\n", id, value);
			}

			return text + "}\n";
		}

		entity read_entity(const TriggerGroup& group)
		{
			entity result;
			for (auto i = 0u; group.entries && i < group.entryCount; i++)
			{
				const auto& entry = group.entries[i];
				result.emplace_back(entry.key ? entry.key : "", trigger_entry_value(entry));
			}

			return result;
		}

		std::string make_entity_string(const std::vector<entity>& entities, const std::vector<trigger>& triggers, const std::string& map_name)
		{
			std::string world_text;
			std::string text;
			std::map<std::string, unsigned int> skipped;
			auto dropped_keys = 0u;
			auto destructibles = 0u;
			for (const auto& entity : entities)
			{
				const auto classname = find_value(entity, "classname");
				if (!is_kept_entity(entity))
				{
					skipped[std::string(classname)]++;
					continue;
				}

				if (classname == "script_model")
				{
					add_csv_row("xmodel", find_value(entity, "model"));
					auto converted = entity;
					destructibles += std::erase_if(converted, is_destructible_key) != 0;
					text += make_entity_text(converted, dropped_keys);
					continue;
				}

				(classname == "worldspawn" ? world_text : text) += make_entity_text(entity, dropped_keys);
			}

			for (auto i = 0u; i < triggers.size(); i++)
			{
				auto keys = triggers[i].keys;
				keys.emplace_back("model", std::format("?{}", i));
				keys.emplace_back("physasset", trigger_physics_asset);
				keys.emplace_back("footprint_mask_group", "default");
				text += make_entity_text(keys, dropped_keys);
			}

			for (const auto& [classname, count] : skipped)
			{
				ZONETOOL_INFO("mapents \"%s\": %u \"%s\" entities are not converted", map_name.data(), count, classname.data());
			}

			if (dropped_keys)
			{
				ZONETOOL_WARNING("mapents \"%s\": %u keys have no IW8 id", map_name.data(), dropped_keys);
			}

			if (destructibles)
			{
				ZONETOOL_WARNING("mapents \"%s\": %u script models lose their T9 destructible def, they are static models in IW8", map_name.data(), destructibles);
			}

			return world_text + text;
		}

		void read_vector(const std::string_view text, target::vec3_t& vector)
		{
			std::sscanf(std::string(text).data(), "%f %f %f", &vector[0], &vector[1], &vector[2]);
		}

		void make_spawn_points(const std::vector<entity>& entities, target::SpawnPointRecordList& converted, utils::memory::allocator& allocator)
		{
			std::vector<target::SpawnPointEntityRecord> spawns;
			const auto add_spawn = [&](const entity& entity, const char* classname)
			{
				auto& spawn = spawns.emplace_back();
				spawn.index = static_cast<unsigned short>(spawns.size() - 1); // TODO: stock index rule unknown
				spawn.name = static_cast<target::scr_string_t>(SL_GetString(classname));
				spawn.target = static_cast<target::scr_string_t>(SL_GetString(""));
				spawn.script_noteworthy = spawn.target;
				read_vector(find_value(entity, "origin"), spawn.origin);
				read_vector(find_value(entity, "angles"), spawn.angles);
			};

			for (const auto& entity : entities)
			{
				if (find_value(entity, "classname") != "mp_spawn_point")
				{
					continue;
				}

				if (find_value(entity, "useAsStartSpawn") == "1")
				{
					const auto group = find_value(entity, "group_index");
					add_spawn(entity, group == "2" ? "mp_tdm_spawn_axis_start" : "mp_tdm_spawn_allies_start");
					add_spawn(entity, "mp_dm_spawn_start");
				}

				if (find_value(entity, "tdm") == "1")
				{
					add_spawn(entity, "mp_tdm_spawn");
					add_spawn(entity, "mp_dm_spawn");
				}
			}

			converted.spawnsCount = static_cast<unsigned short>(spawns.size());
			converted.spawns = allocator.allocate_array<target::SpawnPointEntityRecord>(spawns.size());
			std::ranges::copy(spawns, converted.spawns);
		}

		void make_world_model(const target::Bounds& bounds, target::MapEnts* converted, utils::memory::allocator& allocator)
		{
			converted->numSubModels = 1;
			converted->cmodels = allocator.allocate<target::cmodel_t>();
			auto& model = converted->cmodels[0];
			model.bounds = bounds;
			model.radius = std::sqrt(bounds.halfSize[0] * bounds.halfSize[0] + bounds.halfSize[1] * bounds.halfSize[1]
				+ bounds.halfSize[2] * bounds.halfSize[2]);
			model.physicsShapeOverrideIdx = 0xFFFF;
		}

		void dump_trigger_physics_asset()
		{
			if (!mark_dumped(target::ASSET_TYPE_PHYSICSASSET, trigger_physics_asset))
			{
				return;
			}

			auto havok_data = havok::write_trigger_physics_asset();
			auto simulation_category = target::PhysicsAssetSimulationCategory_Custom;
			unsigned int body_contents = target::PHYSICS_CONTENTS_SOLID;
			target::PhysicsSFXEventAsset* sfx_event = nullptr;
			target::PhysicsVFXEventAsset* vfx_event = nullptr;

			target::PhysicsAsset asset{};
			asset.name = trigger_physics_asset;
			asset.havokData = havok_data.data();
			asset.havokDataSize = static_cast<unsigned int>(havok_data.size());
			asset.useCategory = target::PhysicsAssetUseCategory_Custom;
			asset.numRigidBodies = 1;
			asset.simulationCategories = &simulation_category;
			asset.bodyContents = &body_contents;
			asset.numSFXEventAssets = 1;
			asset.numVFXEventAssets = 1;
			asset.sfxEventAssets = &sfx_event;
			asset.vfxEventAssets = &vfx_event;
			target::physics_asset::dump(&asset);
			add_csv_row("physicsasset", trigger_physics_asset);
		}

		void make_trigger_models(const std::vector<trigger>& triggers, target::MapTriggers& converted, utils::memory::allocator& allocator)
		{
			if (triggers.empty())
			{
				return;
			}

			dump_trigger_physics_asset();
			auto* physics_asset = allocator.allocate<target::PhysicsAsset>();
			physics_asset->name = trigger_physics_asset;

			std::vector<target::TriggerHull> hulls;
			std::vector<target::TriggerSlab> slabs;
			converted.count = static_cast<unsigned int>(triggers.size());
			converted.models = allocator.allocate_array<target::TriggerModel>(triggers.size());
			for (auto i = 0u; i < triggers.size(); i++)
			{
				auto& model = converted.models[i];
				model.contents = target::PHYSICS_CONTENTS_SOLID;
				model.hullCount = static_cast<unsigned short>(triggers[i].hulls.size());
				model.firstHull = static_cast<unsigned short>(hulls.size());
				model.physicsAsset = physics_asset;
				model.physicsShapeOverrideIdx = static_cast<unsigned short>(i);

				for (auto hull : triggers[i].hulls)
				{
					hull.firstSlab = static_cast<unsigned short>(hull.firstSlab + slabs.size());
					hulls.emplace_back(hull);
				}

				slabs.insert(slabs.end(), triggers[i].slabs.begin(), triggers[i].slabs.end());
			}

			converted.hullCount = static_cast<unsigned int>(hulls.size());
			converted.hulls = allocator.allocate_array<target::TriggerHull>(hulls.size());
			std::ranges::copy(hulls, converted.hulls);
			converted.slabCount = static_cast<unsigned int>(slabs.size());
			converted.slabs = allocator.allocate_array<target::TriggerSlab>(slabs.size());
			std::ranges::copy(slabs, converted.slabs);
		}

		target::SpatialPartition_Population_Tree* make_leaf_tree(utils::memory::allocator& allocator)
		{
			auto* tree = allocator.allocate<target::SpatialPartition_Population_Tree>();
			tree->partitionCount = 1;
			tree->partitions = allocator.allocate<target::SpatialPartition_Population_Partition>();
			tree->extents = allocator.allocate<target::ExtentBounds>();
			return tree;
		}

		bool has_entity(const std::vector<entity>& entities, const std::string_view key, const std::string_view value)
		{
			return std::ranges::any_of(entities, [&](const entity& entity)
			{
				return find_value(entity, key) == value;
			});
		}

		std::string make_rotating_platform_script()
		{
			return R"(
rotate_platform()
{
    platform = getent( "rotating_platform", "targetname" );
    car = getent( "rotating_platform_car", "script_noteworthy" );
    if ( isdefined( car ) )
        car linkto( platform );

    for (;;)
    {
        platform rotateyaw( 360, 60 );
        wait 60;
    }
}
)";
		}

		std::string make_map_script(const std::string& map_name, const bool has_compass, const bool has_rotating_platform)
		{
			std::string script = R"(main()
{
    scripts\mp\load::main();
    level.outofboundstriggers = getentarray( "OutOfBounds", "targetname" );
)";
			if (has_compass)
			{
				script += std::format("    scripts\\mp\\compass::setupminimap( \"compass_map_{}\" );\n", map_name);
			}

			script += R"(    game["attackers"] = "allies";
    game["defenders"] = "axis";
    game["allies_outfit"] = "urban";
    game["axis_outfit"] = "woodland";
)";
			if (has_rotating_platform)
			{
				script += "    level thread rotate_platform();\n";
			}

			script += "}\n";
			if (has_rotating_platform)
			{
				ZONETOOL_WARNING("Map \"%s\": the rotating platform turns once each 60 seconds, the T9 speed is not known", map_name.data());
				script += make_rotating_platform_script();
			}

			return script;
		}

		bool dump_compass(const std::string& map_name)
		{
			const auto* entry = static_cast<const MapTableEntry*>(db_find_asset(ASSET_TYPE_MAPTABLEENTRY, xhash_name(xhash(map_name))));
			if (!entry || !entry->compassSetCount || !entry->compassSets[0].images[0])
			{
				ZONETOOL_WARNING("Map \"%s\" has no map table entry with a compass image, the minimap is empty", map_name.data());
				return false;
			}

			material::dump_2d(entry->compassSets[0].images[0], "compass_map_" + map_name);
			return true;
		}

		void dump_map_script(const std::vector<entity>& entities, const std::string& map_name)
		{
			const auto name = std::format("scripts/mp/maps/{}/{}.gsc", map_name, map_name); // GScr_MainMP_LoadScripts_shared (0xE94369)
			const auto has_compass = dump_compass(map_name);
			const auto source = make_map_script(map_name, has_compass, has_entity(entities, "targetname", "rotating_platform"));
			std::vector<std::uint8_t> data(source.begin(), source.end());

			auto& context = target_context();
			const auto assembly = context.compiler().compile(name, data);
			const auto [bytecode, stack, devmap] = context.assembler().assemble(*assembly);
			const auto buffer = utils::compression::zlib::compress(std::string(reinterpret_cast<const char*>(stack.data), stack.size));

			target::ScriptFile script{};
			script.name = name.data();
			script.compressedLen = static_cast<int>(buffer.size());
			script.len = static_cast<int>(stack.size);
			script.bytecodeLen = static_cast<int>(bytecode.size);
			script.buffer = buffer.data();
			script.bytecode = reinterpret_cast<char*>(const_cast<std::uint8_t*>(bytecode.data));
			target::scriptfile::dump(&script);
			add_csv_row("scriptfile", name);
		}
	}

	std::vector<entity> read_entities(const EntityList* asset)
	{
		std::vector<entity> entities;
		for (auto i = 0ull; asset && asset->groups && i < asset->groupCount; i++)
		{
			entities.emplace_back(read_entity(asset->groups[i]));
		}

		return entities;
	}

	std::vector<trigger> convert_triggers(const TriggerList* asset, const std::string& map_name)
	{
		std::vector<trigger> triggers;
		std::map<std::string, unsigned int> skipped;
		auto slab_hulls = 0u;
		for (auto i = 0ull; asset && asset->groups && i < asset->groupCount && i < asset->modelCount; i++)
		{
			const auto source = read_entity(asset->groups[i]);
			const auto classname = find_value(source, "classname");

			trigger converted{};
			if (classname == "trigger_out_of_bounds")
			{
				converted.keys = {{"classname", "trigger_multiple"}, {"targetname", "OutOfBounds"}};
			}
			else if (classname == "trigger_hurt")
			{
				converted.keys = {{"classname", "trigger_hurt"}, {"dmg", std::string(find_value(source, "dmg"))}};
			}
			else
			{
				skipped[std::string(classname)]++;
				continue;
			}

			const auto& model = asset->models[i];
			if (!model.hullCount || model.hullCount > 63) // shared vertex index is a byte
			{
				ZONETOOL_WARNING("mapents \"%s\": trigger %llu has %u hulls and is not converted", map_name.data(), i,
					static_cast<unsigned>(model.hullCount));
				continue;
			}

			target::vec3_t angles{};
			read_vector(find_value(source, "angles"), angles);
			if (angles[0] != 0.0f || angles[1] != 0.0f || angles[2] != 0.0f)
			{
				ZONETOOL_WARNING("mapents \"%s\": trigger %llu has angles, the converted trigger has no rotation", map_name.data(), i);
			}

			converted.keys.emplace_back("origin", find_value(source, "origin"));
			for (auto h = 0u; h < model.hullCount; h++)
			{
				const auto& hull = asset->hulls[model.firstHull + h];
				auto& converted_hull = converted.hulls.emplace_back();
				std::memcpy(converted_hull.bounds.midPoint, hull.midPoint, sizeof(hull.midPoint));
				std::memcpy(converted_hull.bounds.halfSize, hull.halfSize, sizeof(hull.halfSize));
				converted_hull.contents = target::PHYSICS_CONTENTS_SOLID;
				converted_hull.slabCount = hull.slabCount;
				converted_hull.firstSlab = static_cast<unsigned short>(converted.slabs.size());
				slab_hulls += hull.slabCount ? 1 : 0;

				for (auto s = 0u; s < hull.slabCount; s++)
				{
					const auto& slab = asset->slabs[hull.firstSlab + s];
					auto& converted_slab = converted.slabs.emplace_back();
					std::memcpy(converted_slab.dir, slab.dir, sizeof(slab.dir));
					converted_slab.midPoint = slab.midPoint;
					converted_slab.halfSize = slab.halfSize;
				}
			}

			triggers.emplace_back(std::move(converted));
		}

		for (const auto& [classname, count] : skipped)
		{
			ZONETOOL_INFO("mapents \"%s\": %u \"%s\" triggers are not converted", map_name.data(), count, classname.data());
		}

		if (slab_hulls)
		{
			ZONETOOL_WARNING("mapents \"%s\": %u trigger hulls have slabs, their Havok shape is the hull box", map_name.data(), slab_hulls);
		}

		ZONETOOL_INFO("mapents \"%s\": %zu triggers", map_name.data(), triggers.size());
		return triggers;
	}

	void dump(const std::vector<entity>& entities, const std::vector<trigger>& triggers, const zonetool::iw8::Bounds& world_bounds,
		const std::string& ents_shape_data, const unsigned int dyn_entity_count, const std::string& map_name)
	{
		utils::memory::allocator allocator;

		const auto world_name = std::format("maps/mp/{}.d3dbsp", map_name);
		auto* converted = allocator.allocate<target::MapEnts>();
		converted->name = allocator.duplicate_string(world_name);
		auto entity_string = make_entity_string(entities, triggers, map_name);
		converted->entityString = entity_string.data();
		converted->numEntityChars = static_cast<int>(entity_string.size() + 1);

		make_spawn_points(entities, converted->spawnList, allocator);
		ZONETOOL_INFO("mapents \"%s\": %u spawn points", map_name.data(), static_cast<unsigned>(converted->spawnList.spawnsCount));

		converted->havokEntsShapeDataSize = static_cast<unsigned int>(ents_shape_data.size());
		converted->havokEntsShapeData = allocator.allocate_array<char>(ents_shape_data.size());
		std::memcpy(converted->havokEntsShapeData, ents_shape_data.data(), ents_shape_data.size());

		make_world_model(world_bounds, converted, allocator);
		make_trigger_models(triggers, converted->trigger, allocator);
		dynentitylist::fill_map_ents(converted, dyn_entity_count, world_bounds, allocator);
		converted->scriptableMapEnts.spatialPopulationTree = make_leaf_tree(allocator);

		target::map_ents::dump(converted);
		add_csv_row("map_ents", world_name);

		dump_map_script(entities, map_name);
	}
}
