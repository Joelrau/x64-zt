#include <std_include.hpp>
#include "mapents.hpp"

namespace zonetool::iw8
{
	void read_map_triggers(zone_reader& reader, MapTriggers& triggers)
	{
		if (const auto models = reader.read_array(triggers.models, 7, triggers.count))
		{
			for (auto i = 0u; i < triggers.count; i++)
			{
				reader.read_asset(ASSET_TYPE_PHYSICSASSET, models[i].physicsAsset);
			}
		}

		reader.read_array(triggers.hulls, 3, triggers.hullCount);
		reader.read_array(triggers.slabs, 3, triggers.slabCount);
		reader.read_array(triggers.windings, 3, triggers.windingCount);
		reader.read_array(triggers.windingPoints, 3, triggers.windingPointCount);
	}

	namespace
	{
		void read_client_triggers(zone_reader& reader, ClientTriggers& triggers)
		{
			auto& map_triggers = triggers.trigger;
			reader.read_array(map_triggers.models, 3, map_triggers.count);
			reader.read_array(map_triggers.hulls, 3, map_triggers.hullCount);
			reader.read_array(map_triggers.slabs, 3, map_triggers.slabCount);

			const auto count = map_triggers.count;
			reader.read_array(triggers.triggerString, 0, triggers.triggerStringLength);
			reader.read_array(triggers.visionSetTriggers, 1, count);
			reader.read_array(triggers.triggerType, 1, count);
			reader.read_array(triggers.origins, 3, count);
			reader.read_array(triggers.scriptDelay, 3, count);
			reader.read_array(triggers.priority, 3, count);
			reader.read_array(triggers.audioTriggers, 1, count);
			reader.read_array(triggers.blendLookup, 1, count);
			reader.read_array(triggers.npcTriggers, 1, count);
			reader.read_array(triggers.audioStateIds, 1, count);
			reader.read_asset_array(ASSET_TYPE_SOUND_BANK_TRANSIENT, triggers.detailSoundBank, count);
			reader.read_array(triggers.audioRvbPanInfo, 3, count);
			reader.read_array(triggers.parent, 1, count);

			if (const auto links = reader.read_array(triggers.linkTo, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					reader.read_single(links[i], 3);
				}
			}

			read_spatial_tree(reader, triggers.spatialTree);
		}

		void read_spawn_points(zone_reader& reader, SpawnPointRecordList& list)
		{
			if (const auto spawns = reader.read_array(list.spawns, 3, list.spawnsCount))
			{
				for (auto i = 0u; i < list.spawnsCount; i++)
				{
					reader.read_script_string(spawns[i].name);
					reader.read_script_string(spawns[i].target);
					reader.read_script_string(spawns[i].script_noteworthy);
				}
			}
		}

		void read_splines(zone_reader& reader, SplineRecordList& list)
		{
			const auto splines = reader.read_array(list.splines, 7, list.splineCount);
			if (!splines)
			{
				return;
			}

			for (auto i = 0u; i < list.splineCount; i++)
			{
				if (const auto points = reader.read_array(splines[i].splinePoints, 3, splines[i].splinePointCount))
				{
					for (auto j = 0u; j < splines[i].splinePointCount; j++)
					{
						reader.read_script_string(points[j].splineNodeLabel);
						reader.read_script_string(points[j].targetname);
						reader.read_script_string(points[j].target);
						reader.read_script_string(points[j].string);
					}
				}
			}
		}

		void read_client_models(zone_reader& reader, MapEnts* asset)
		{
			if (const auto models = reader.read_array(asset->clientModels, 7, asset->numClientModels))
			{
				for (auto i = 0u; i < asset->numClientModels; i++)
				{
					reader.read_asset(ASSET_TYPE_XMODEL, models[i].model);
					reader.read_script_string(models[i].animationTreeName);
					reader.read_asset(ASSET_TYPE_XANIMPARTS, models[i].animation);
					reader.read_script_string(models[i].name);
				}
			}
		}

		void read_population_tree(zone_reader& reader, SpatialPartition_Population_Tree& tree)
		{
			reader.read_array(tree.partitions, 3, tree.partitionCount);
			reader.read_array(tree.extents, 3, tree.partitionCount);
		}

		void read_population(zone_reader& reader, SpatialPartition_Population*& field)
		{
			const auto population = reader.read_single(field, 7);
			if (!population)
			{
				return;
			}

			if (const auto tree = reader.read_single(population->tree, 7))
			{
				read_population_tree(reader, *tree);
			}

			reader.read_array(population->buckets, 3, population->partitionCount);
		}

		void read_transient_map(zone_reader& reader, SpatialPartition_Population_TransientMap*& field)
		{
			const auto map = reader.read_single(field, 7);
			if (!map)
			{
				return;
			}

			if (const auto partitions = reader.read_array(map->partitionsForTransient, 7, map->transientCount))
			{
				for (auto i = 0u; i < map->transientCount; i++)
				{
					reader.read_array(partitions[i].array, 3, partitions[i].wordCount);
				}
			}
		}

		void read_scriptable_map_ents(zone_reader& reader, ScriptableMapEnts& ents)
		{
			if (const auto instances = reader.read_array(ents.mapInstances, 7, ents.mapInstanceCount))
			{
				for (auto i = 0u; i < ents.mapInstanceCount; i++)
				{
					auto& instance = instances[i];
					reader.read_asset(ASSET_TYPE_SCRIPTABLE, instance.initialDef);
					reader.read_script_string(instance.classname);
					reader.read_script_string(instance.target);
					reader.read_script_string(instance.targetname);
					reader.read_script_string(instance.script_linkname);
					reader.read_script_string(instance.script_linkto);
					reader.read_script_string(instance.script_noteworthy);
				}
			}

			for (auto& dynents : ents.reservedDynents)
			{
				reader.read_array(dynents.reservedDynents, 3, dynents.numReservedDynents);
			}

			if (const auto tree = reader.read_single(ents.spatialPopulationTree, 7))
			{
				read_population_tree(reader, *tree);
			}
		}

		void read_spawn_group(zone_reader& reader, SpawnGroup& group)
		{
			reader.read_array(group.points, 3, group.pointCount);

			if (const auto clusters = reader.read_array(group.clusters, 7, group.clusterCount))
			{
				for (auto i = 0u; i < group.clusterCount; i++)
				{
					reader.read_array(clusters[i].points, 1, clusters[i].pointCount);
				}
			}

			if (const auto zones = reader.read_array(group.zones, 7, group.zoneCount))
			{
				for (auto i = 0u; i < group.zoneCount; i++)
				{
					reader.read_script_string(zones[i].name);
					reader.read_array(zones[i].points, 1, zones[i].pointCount);
					reader.read_array(zones[i].clusters, 1, zones[i].clusterCount);
				}
			}
		}

		void read_alias_list_pointer(zone_reader& reader, SndAliasList*& field)
		{
			if (const auto list = reader.read_single(field, 7))
			{
				read_alias_list(reader, *list);
			}
		}

		void read_client_side_effects(zone_reader& reader, ClientSideEffects& effects)
		{
			if (const auto oneshots = reader.read_array(effects.oneshotEffects, 7, effects.oneshotEffectCount))
			{
				for (auto i = 0u; i < effects.oneshotEffectCount; i++)
				{
					reader.read_asset(ASSET_TYPE_VFX, oneshots[i].effect.particleSystemDef);
					reader.read_string(oneshots[i].effectSound.name);
				}
			}

			read_spatial_tree(reader, effects.oneshotEffectsTree);
			reader.read_array(effects.oneshotSoundIndices, 3, effects.oneshotSoundCount);

			if (const auto exploders = reader.read_array(effects.exploders, 7, effects.exploderCount))
			{
				for (auto i = 0u; i < effects.exploderCount; i++)
				{
					reader.read_asset(ASSET_TYPE_VFX, exploders[i].effect.particleSystemDef);
					reader.read_string(exploders[i].effectSound.name);
					read_alias_list_pointer(reader, exploders[i].aliasList);
					reader.read_script_string(exploders[i].name);
				}
			}

			if (const auto sounds = reader.read_array(effects.intervalSounds, 7, effects.intervalSoundCount))
			{
				for (auto i = 0u; i < effects.intervalSoundCount; i++)
				{
					reader.read_string(sounds[i].effectSound.name);
					read_alias_list_pointer(reader, sounds[i].aliasList);
				}
			}

			if (const auto sounds = reader.read_array(effects.loopSounds, 7, effects.loopSoundCount))
			{
				for (auto i = 0u; i < effects.loopSoundCount; i++)
				{
					reader.read_string(sounds[i].effectSound.name);
					read_alias_list_pointer(reader, sounds[i].aliasList);
				}
			}

			read_spatial_tree(reader, effects.soundsSpatialTree);

			if (const auto ents = reader.read_array(effects.reactiveEnts, 7, effects.reactiveEntCount))
			{
				for (auto i = 0u; i < effects.reactiveEntCount; i++)
				{
					reader.read_asset(ASSET_TYPE_VFX, ents[i].effect.particleSystemDef);
					reader.read_string(ents[i].effectSound.name);
					read_alias_list_pointer(reader, ents[i].aliasList);
				}
			}

			read_spatial_tree(reader, effects.reactiveSpatialTree);
		}

		void read_create_fx(zone_reader& reader, CreateFxAssetData& data)
		{
			if (const auto palette = reader.read_array(data.fxPalette, 3, data.fxPaletteCount))
			{
				for (auto i = 0u; i < data.fxPaletteCount; i++)
				{
					reader.read_script_string(palette[i].aliasName);
					reader.read_script_string(palette[i].effectPath);
				}
			}
		}

		void read_server_side_effects(zone_reader& reader, ServerSideEffects& effects)
		{
			if (const auto exploders = reader.read_array(effects.exploders, 3, effects.exploderCount))
			{
				for (auto i = 0u; i < effects.exploderCount; i++)
				{
					reader.read_script_string(exploders[i].name);
					reader.read_script_string(exploders[i].earthquakeName);
					reader.read_script_string(exploders[i].rumbleName);
				}
			}
		}

		void read_mayhem_scenes(zone_reader& reader, MapEnts* asset)
		{
			if (const auto scenes = reader.read_array(asset->mayhemScenes, 7, asset->numMayhemScenes))
			{
				for (auto i = 0u; i < asset->numMayhemScenes; i++)
				{
					reader.read_asset(ASSET_TYPE_MAYHEM, scenes[i].mayhem);
					reader.read_single(scenes[i].linkTo, 3);
					reader.read_script_string(scenes[i].scriptName);
				}
			}
		}

		void read_spawners(zone_reader& reader, SpawnerList& list)
		{
			const auto spawners = reader.read_array(list.spawnerList, 7, list.spawnerCount);
			if (!spawners)
			{
				return;
			}

			for (auto i = 0u; i < list.spawnerCount; i++)
			{
				auto& spawner = spawners[i];
				if (const auto fields = reader.read_array(spawner.fields, 3, spawner.numFields))
				{
					for (auto j = 0u; j < spawner.numFields; j++)
					{
						reader.read_script_string(fields[j].key);
						reader.read_script_string(fields[j].value);
					}
				}

				reader.read_script_string(spawner.targetname);
				reader.read_script_string(spawner.classname);
			}
		}
	}

	void map_ents::read(zone_reader& reader, MapEnts* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->entityString, 0, asset->numEntityChars);
		read_map_triggers(reader, asset->trigger);
		read_client_triggers(reader, asset->clientTrigger);
		reader.read_array(asset->clientTriggerBlend.blendNodes, 3, asset->clientTriggerBlend.numClientTriggerBlendNodes);
		read_spawn_points(reader, asset->spawnList);
		read_splines(reader, asset->splineList);
		reader.read_array(asset->havokEntsShapeData, 15, asset->havokEntsShapeDataSize);

		if (const auto cmodels = reader.read_array(asset->cmodels, 7, asset->numSubModels))
		{
			for (auto i = 0u; i < asset->numSubModels; i++)
			{
				reader.read_asset(ASSET_TYPE_PHYSICSASSET, cmodels[i].physicsAsset);
			}
		}

		reader.read_asset_array(ASSET_TYPE_EDGE_LIST, asset->edgeLists, asset->numEdgeLists);
		read_spatial_tree(reader, asset->edgeListSpatialTree);
		read_client_models(reader, asset);
		reader.read_array(asset->dynEntListIds, 1, asset->dynEntityListsCount);

		for (auto& population : asset->dynEntSpatialPopulation)
		{
			read_population(reader, population);
		}

		for (auto& map : asset->dynEntSpatialTransientMap)
		{
			read_transient_map(reader, map);
		}

		if (const auto anchors = reader.read_array(asset->clientEntAnchors, 3, asset->clientEntAnchorCount))
		{
			for (auto i = 0u; i < asset->clientEntAnchorCount; i++)
			{
				reader.read_script_string(anchors[i].name);
			}
		}

		read_scriptable_map_ents(reader, asset->scriptableMapEnts);
		read_spawn_group(reader, asset->spawnGroupLoot);
		read_client_side_effects(reader, asset->clientSideEffects);
		read_create_fx(reader, asset->createFxAssetData);
		reader.read_script_string_array(asset->exploderNames, asset->exploderNameTotal);
		read_server_side_effects(reader, asset->serverSideEffects);
		read_mayhem_scenes(reader, asset);
		read_spawners(reader, asset->spawners);
		reader.read_array(asset->audioPASpeakers, 3, asset->audioPASpeakerCount);
		reader.read_array(asset->audioPropNodes, 3, asset->numAudioPropNodes);
		reader.read_array(asset->audioPropEdges, 1, asset->numAudioPropEdges);

		if (const auto lookups = reader.read_array(asset->collmapLookups, 7, asset->numCollmaps))
		{
			for (auto i = 0u; i < asset->numCollmaps; i++)
			{
				reader.read_string(lookups[i].targetName);
			}
		}
		reader.pop_stream();
	}
}
