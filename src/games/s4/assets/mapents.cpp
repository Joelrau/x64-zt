#include <std_include.hpp>
#include "mapents.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk>
		void walk_client_triggers(Walk walk, ClientTriggers& triggers)
		{
			auto& map_triggers = triggers.trigger;
			walk.array(map_triggers.models, map_triggers.count, 3);
			walk.array(map_triggers.hulls, map_triggers.hullCount, 3);
			walk.array(map_triggers.slabs, map_triggers.slabCount, 3);

			const auto count = map_triggers.count;
			walk.array(triggers.triggerString, triggers.triggerStringLength, 0);
			walk.array(triggers.visionSetTriggers, count, 1);
			walk.array(triggers.triggerType, count, 1);
			walk.array(triggers.origins, count, 3);
			walk.array(triggers.scriptDelay, count, 3);
			walk.array(triggers.priority, count, 3);
			walk.array(triggers.audioTriggers, count, 1);
			walk.array(triggers.blendLookup, count, 1);
			walk.array(triggers.npcTriggers, count, 1);
			walk.array(triggers.audioStateIds, count, 1);
			walk_asset_array(walk, ASSET_TYPE_SOUND_BANK_TRANSIENT, triggers.detailSoundBank, count);
			walk.array(triggers.audioRvbPanInfo, count, 3);
			walk.array(triggers.parent, count, 1);

			if (const auto links = walk.array(triggers.linkTo, count, 7))
			{
				for (auto i = 0u; i < count; i++)
				{
					walk.array(links[i], 1, 3);
				}
			}

			walk_spatial_tree(walk, triggers.spatialTree);
			walk.array(triggers.unknown0, count, 3);
		}

		template <typename Walk>
		void walk_map_ents_unknown0(Walk walk, MapEntsUnknown0& list)
		{
			if (const auto entries = walk.array(list.entries, list.count, 7))
			{
				for (auto i = 0u; i < list.count; i++)
				{
					walk.array(entries[i].linkTo, 1, 3);
				}
			}
		}

		template <typename Walk>
		void walk_spawn_points(Walk walk, SpawnPointRecordList& list)
		{
			if (const auto spawns = walk.array(list.spawns, list.spawnsCount, 3))
			{
				for (auto i = 0u; i < list.spawnsCount; i++)
				{
					walk.script_string(spawns[i].name);
					walk.script_string(spawns[i].target);
					walk.script_string(spawns[i].script_noteworthy);
				}
			}
		}

		template <typename Walk>
		void walk_splines(Walk walk, SplineRecordList& list)
		{
			const auto splines = walk.array(list.splines, list.splineCount, 7);
			if (!splines)
			{
				return;
			}

			for (auto i = 0u; i < list.splineCount; i++)
			{
				if (const auto points = walk.array(splines[i].splinePoints, splines[i].splinePointCount, 3))
				{
					for (auto j = 0u; j < splines[i].splinePointCount; j++)
					{
						walk.script_string(points[j].splineNodeLabel);
						walk.script_string(points[j].targetname);
						walk.script_string(points[j].target);
						walk.script_string(points[j].string);
					}
				}
			}
		}

		template <typename Walk>
		void walk_client_models(Walk walk, MapEnts& asset)
		{
			if (const auto models = walk.array(asset.clientModels, asset.numClientModels, 7))
			{
				for (auto i = 0u; i < asset.numClientModels; i++)
				{
					walk.asset(ASSET_TYPE_XMODEL, models[i].model);
					walk.script_string(models[i].animationTreeName);
					walk.asset(ASSET_TYPE_XANIMPARTS, models[i].animation);
					walk.script_string(models[i].name);
				}
			}
		}

		template <typename Walk>
		void walk_population_tree(Walk walk, SpatialPartition_Population_Tree& tree)
		{
			walk.array(tree.partitions, tree.partitionCount, 3);
			walk.array(tree.extents, tree.partitionCount, 3);
		}

		template <typename Walk>
		void walk_population(Walk walk, SpatialPartition_Population*& field)
		{
			const auto population = walk.array(field, 1, 7);
			if (!population)
			{
				return;
			}

			if (const auto tree = walk.array(population->tree, 1, 7))
			{
				walk_population_tree(walk, *tree);
			}

			walk.array(population->buckets, population->partitionCount, 3);
		}

		template <typename Walk>
		void walk_transient_map(Walk walk, SpatialPartition_Population_TransientMap*& field)
		{
			const auto map = walk.array(field, 1, 7);
			if (!map)
			{
				return;
			}

			if (const auto partitions = walk.array(map->partitionsForTransient, map->transientCount, 7))
			{
				for (auto i = 0u; i < map->transientCount; i++)
				{
					walk.array(partitions[i].array, partitions[i].wordCount, 3);
				}
			}
		}

		template <typename Walk>
		void walk_scriptable_map_ents(Walk walk, ScriptableMapEnts& ents)
		{
			if (const auto instances = walk.array(ents.mapInstances, ents.mapInstanceCount, 7))
			{
				for (auto i = 0u; i < ents.mapInstanceCount; i++)
				{
					auto& instance = instances[i];
					walk.asset(ASSET_TYPE_SCRIPTABLE, instance.initialDef);
					walk.script_string(instance.classname);
					walk.script_string(instance.target);
					walk.script_string(instance.targetname);
					walk.script_string(instance.script_linkname);
					walk.script_string(instance.script_linkto);
					walk.script_string(instance.script_noteworthy);
					walk.script_string(instance.unknown0);
				}
			}

			for (auto& dynents : ents.reservedDynents)
			{
				walk.array(dynents.reservedDynents, dynents.numReservedDynents, 3);
			}

			if (const auto tree = walk.array(ents.spatialPopulationTree, 1, 7))
			{
				walk_population_tree(walk, *tree);
			}

			walk.array(ents.unknown2, ents.unknown2Count, 3);
			walk.array(ents.unknown3.unknown0, ents.unknown3.unknown0Count, 3);
			walk.array(ents.unknown3.unknown1, ents.unknown3.unknown1Count, 3);
		}

		template <typename Walk>
		void walk_spawn_group(Walk walk, SpawnGroup& group)
		{
			walk.array(group.points, group.pointCount, 3);

			if (const auto clusters = walk.array(group.clusters, group.clusterCount, 7))
			{
				for (auto i = 0u; i < group.clusterCount; i++)
				{
					walk.array(clusters[i].points, clusters[i].pointCount, 1);
				}
			}

			if (const auto zones = walk.array(group.zones, group.zoneCount, 7))
			{
				for (auto i = 0u; i < group.zoneCount; i++)
				{
					walk.script_string(zones[i].name);
					walk.array(zones[i].points, zones[i].pointCount, 1);
					walk.array(zones[i].clusters, zones[i].clusterCount, 1);
				}
			}
		}

		template <typename Walk>
		void walk_client_side_effects(Walk walk, ClientSideEffects& effects)
		{
			if (const auto oneshots = walk.array(effects.oneshotEffects, effects.oneshotEffectCount, 7))
			{
				for (auto i = 0u; i < effects.oneshotEffectCount; i++)
				{
					walk.asset(ASSET_TYPE_VFX, oneshots[i].effect.particleSystemDef);
				}
			}

			walk_spatial_tree(walk, effects.oneshotEffectsTree);
			walk.array(effects.oneshotSoundIndices, effects.oneshotSoundCount, 3);

			if (const auto exploders = walk.array(effects.exploders, effects.exploderCount, 7))
			{
				for (auto i = 0u; i < effects.exploderCount; i++)
				{
					walk.asset(ASSET_TYPE_VFX, exploders[i].effect.particleSystemDef);
					walk_alias_list_pointer(walk, exploders[i].aliasList);
					walk.script_string(exploders[i].name);
				}
			}

			if (const auto sounds = walk.array(effects.intervalSounds, effects.intervalSoundCount, 7))
			{
				for (auto i = 0u; i < effects.intervalSoundCount; i++)
				{
					walk_alias_list_pointer(walk, sounds[i].aliasList);
				}
			}

			if (const auto sounds = walk.array(effects.loopSounds, effects.loopSoundCount, 7))
			{
				for (auto i = 0u; i < effects.loopSoundCount; i++)
				{
					walk_alias_list_pointer(walk, sounds[i].aliasList);
				}
			}

			if (const auto sounds = walk.array(effects.unknown0, effects.unknown0Count, 7))
			{
				for (auto i = 0u; i < effects.unknown0Count; i++)
				{
					walk_alias_list_pointer(walk, sounds[i].aliasList);
				}
			}

			if (const auto sounds = walk.array(effects.unknown1, effects.unknown1Count, 7))
			{
				for (auto i = 0u; i < effects.unknown1Count; i++)
				{
					walk_alias_list_pointer(walk, sounds[i].aliasList);
				}
			}

			walk_spatial_tree(walk, effects.soundsSpatialTree);

			if (const auto ents = walk.array(effects.reactiveEnts, effects.reactiveEntCount, 7))
			{
				for (auto i = 0u; i < effects.reactiveEntCount; i++)
				{
					walk.asset(ASSET_TYPE_VFX, ents[i].effect.particleSystemDef);
					walk_alias_list_pointer(walk, ents[i].aliasList);
				}
			}

			walk_spatial_tree(walk, effects.reactiveSpatialTree);

			if (const auto entries = walk.array(effects.unknown2, effects.unknown2Count, 3))
			{
				for (auto i = 0u; i < effects.unknown2Count; i++)
				{
					walk.script_string(entries[i].name);
				}
			}

			walk.array(effects.unknown3, effects.unknown3Count, 1);
		}

		template <typename Walk>
		void walk_create_fx(Walk walk, CreateFxAssetData& data)
		{
			if (const auto palette = walk.array(data.fxPalette, data.fxPaletteCount, 3))
			{
				for (auto i = 0u; i < data.fxPaletteCount; i++)
				{
					walk.script_string(palette[i].aliasName);
					walk.script_string(palette[i].effectPath);
				}
			}
		}

		template <typename Walk>
		void walk_server_side_effects(Walk walk, ServerSideEffects& effects)
		{
			if (const auto exploders = walk.array(effects.exploders, effects.exploderCount, 3))
			{
				for (auto i = 0u; i < effects.exploderCount; i++)
				{
					walk.script_string(exploders[i].name);
					walk.script_string(exploders[i].earthquakeName);
					walk.script_string(exploders[i].rumbleName);
				}
			}
		}

		template <typename Walk>
		void walk_mayhem_scenes(Walk walk, MapEnts& asset)
		{
			if (const auto scenes = walk.array(asset.mayhemScenes, asset.numMayhemScenes, 7))
			{
				for (auto i = 0u; i < asset.numMayhemScenes; i++)
				{
					walk.asset(ASSET_TYPE_MAYHEM, scenes[i].mayhem);
					walk.array(scenes[i].linkTo, 1, 3);
					walk.script_string(scenes[i].scriptName);
				}
			}
		}

		template <typename Walk>
		void walk_spawners(Walk walk, SpawnerList& list)
		{
			const auto spawners = walk.array(list.spawnerList, list.spawnerCount, 7);
			if (!spawners)
			{
				return;
			}

			for (auto i = 0u; i < list.spawnerCount; i++)
			{
				auto& spawner = spawners[i];
				if (const auto fields = walk.array(spawner.fields, spawner.numFields, 3))
				{
					for (auto j = 0u; j < spawner.numFields; j++)
					{
						walk.script_string(fields[j].key);
						walk.script_string(fields[j].value);
					}
				}

				walk.script_string(spawner.targetname);
				walk.script_string(spawner.classname);
			}
		}

		template <typename Walk>
		void walk_water_dynamics_entry(Walk walk, WaterDynamics& water)
		{
			walk.array(water.unknown1, water.unknown1Count, 3);
			walk.array(water.unknown2, water.unknown2Count, 3);
			walk.array(water.unknown3, water.unknown3Count, 3);

			if (const auto list = walk.array(water.unknown4, 1, 7))
			{
				walk.array(list->entries, list->count, 3);
			}

			walk.array(water.unknown6, water.unknown6Count, 3);
			walk.array(water.unknown9, water.unknown9Size, 0);

			if (const auto entries = walk.array(water.unknown8, water.unknown8Count, 7))
			{
				for (auto i = 0u; i < water.unknown8Count; i++)
				{
					walk.array(entries[i].unknown1, entries[i].unknown1Size, 0);
					walk.array(entries[i].unknown2, entries[i].unknown2Size, 0);
					walk.array(entries[i].unknown3, entries[i].unknown3Count, 3);
				}
			}

			walk.array(water.unknown12, water.unknown12Size, 0);

			if (const auto entries = walk.array(water.unknown11, water.unknown11Count, 7))
			{
				for (auto i = 0u; i < water.unknown11Count; i++)
				{
					walk.array(entries[i].unknown1, entries[i].unknown1Size, 0);
					walk.array(entries[i].unknown2, entries[i].unknown2Size, 0);
				}
			}
		}

		template <typename Walk>
		void walk_water_dynamics(Walk walk, MapEnts& asset)
		{
			if (const auto waters = walk.array(asset.waterDynamics, asset.waterDynamicsCount, 7))
			{
				for (auto i = 0u; i < asset.waterDynamicsCount; i++)
				{
					walk_water_dynamics_entry(walk, waters[i]);
				}
			}

			if (const auto entries = walk.array(asset.unknown1, asset.unknown1Count, 7))
			{
				for (auto i = 0u; i < asset.unknown1Count; i++)
				{
					walk.array(entries[i].entries, entries[i].count, 3);
				}
			}

			walk.array(asset.unknown2, asset.unknown2Count, 3);

			if (const auto systems = walk.array(asset.waterParticleSystems, asset.waterParticleSystemCount, 7))
			{
				for (auto i = 0u; i < asset.waterParticleSystemCount; i++)
				{
					walk.array(systems[i].entries, systems[i].count, 3);
				}
			}

			walk.array(asset.waterDynamicsGfx, asset.waterDynamicsCount, 3);
			walk.array(asset.unknown4, asset.waterDynamicsCount, 3);
			walk.array(asset.waterTileGfx, asset.waterTileCount, 3);
			walk.array(asset.waterTiles, asset.waterTileCount, 3);
			walk.array(asset.waterParticleInstancesGfx, asset.waterParticleInstanceCount, 3);
			walk.array(asset.waterParticleSystemsGfx, asset.waterParticleSystemCount, 3);

			if (const auto data = walk.array(asset.unknown5, 1, 7))
			{
				walk.array(data->unknown0, data->unknown0Count, 0);
				walk.array(data->unknown1, data->unknown1Count, 0);
				walk.array(data->unknown2, data->unknown2Count, 3);
			}
		}

		template <typename Walk>
		void walk_map_ents(Walk walk, MapEnts& asset)
		{
			walk.array(asset.entityString, asset.numEntityChars, 0);
			walk_map_triggers(walk, asset.trigger);
			walk_client_triggers(walk, asset.clientTrigger);
			walk.array(asset.clientTriggerBlend.blendNodes, asset.clientTriggerBlend.numClientTriggerBlendNodes, 3);
			walk_map_ents_unknown0(walk, asset.unknown0);
			walk_spawn_points(walk, asset.spawnList);
			walk_splines(walk, asset.splineList);
			walk.array(asset.havokEntsShapeData, asset.havokEntsShapeDataSize, 15);

			if (const auto cmodels = walk.array(asset.cmodels, asset.numSubModels, 7))
			{
				for (auto i = 0u; i < asset.numSubModels; i++)
				{
					walk.asset(ASSET_TYPE_PHYSICSASSET, cmodels[i].physicsAsset);
				}
			}

			walk_asset_array(walk, ASSET_TYPE_EDGE_LIST, asset.edgeLists, asset.numEdgeLists);
			walk_spatial_tree(walk, asset.edgeListSpatialTree);
			walk_client_models(walk, asset);
			walk.array(asset.dynEntListIds, asset.dynEntityListsCount, 1);

			for (auto& population : asset.dynEntSpatialPopulation)
			{
				walk_population(walk, population);
			}

			for (auto& map : asset.dynEntSpatialTransientMap)
			{
				walk_transient_map(walk, map);
			}

			if (const auto anchors = walk.array(asset.clientEntAnchors, asset.clientEntAnchorCount, 3))
			{
				for (auto i = 0u; i < asset.clientEntAnchorCount; i++)
				{
					walk.script_string(anchors[i].name);
				}
			}

			walk_scriptable_map_ents(walk, asset.scriptableMapEnts);
			walk_spawn_group(walk, asset.spawnGroupLoot);
			walk_client_side_effects(walk, asset.clientSideEffects);
			walk_create_fx(walk, asset.createFxAssetData);
			walk.script_strings(asset.exploderNames, asset.exploderNameTotal);
			walk_server_side_effects(walk, asset.serverSideEffects);
			walk_mayhem_scenes(walk, asset);
			walk_spawners(walk, asset.spawners);
			walk.array(asset.audioPASpeakers, asset.audioPASpeakerCount, 7);
			walk.array(asset.audioPropNodes, asset.numAudioPropNodes, 3);
			walk.array(asset.audioPropEdges, asset.numAudioPropEdges, 1);

			if (const auto lookups = walk.array(asset.collmapLookups, asset.numCollmaps, 7))
			{
				for (auto i = 0u; i < asset.numCollmaps; i++)
				{
					walk.string(lookups[i].targetName);
				}
			}

			walk_water_dynamics(walk, asset);
		}
	}

	void map_ents::read(zone_reader& reader, MapEnts* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_map_ents(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void map_ents::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_map_ents(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void map_ents::dump(MapEnts* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open(asset->name + ".mapents"s))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_map_ents(dump_walk{dump}, *asset);
		dump.close();
	}

	MapEnts* map_ents::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(name + ".mapents"s))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing mapents \"%s\"...", name.data());

		auto* asset = read.read_single<MapEnts>();
		asset->name = read.read_string();
		walk_map_ents(parse_script_walk{{read}, this->script_strings_}, *asset);
		read.close();
		return asset;
	}

	void map_ents::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<MapEnts>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void map_ents::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_map_ents(depend_walk{zone}, *this->asset_);
	}

	void map_ents::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (!this->referenced())
		{
			walk_map_ents(prepare_walk{buf, this->script_strings_}, *this->asset_);
		}
	}

	std::string map_ents::name()
	{
		return this->name_;
	}

	std::int32_t map_ents::type()
	{
		return ASSET_TYPE_MAP_ENTS;
	}
}
