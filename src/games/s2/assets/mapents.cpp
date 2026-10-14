#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		void read_triggers(zone_reader& reader, MapTriggers* trigger)
		{
			reader.read_array(trigger->models, 7, trigger->count);
			reader.read_array(trigger->hulls, 7, trigger->hullCount);
			reader.read_array(trigger->slabs, 3, trigger->slabCount);
		}

		void read_client_triggers(zone_reader& reader, ClientTriggers* trigger)
		{
			read_triggers(reader, &trigger->trigger);

			const auto count = trigger->trigger.count;
			reader.read_array(trigger->clientTriggerAabbTree, 3, trigger->numClientTriggerNodes);
			reader.read_array(trigger->triggerString, 0, trigger->triggerStringLength);
			reader.read_array(trigger->visionSetTriggers, 1, count);
			reader.read_array(trigger->lightSetTriggers, 1, count);
			reader.read_array(trigger->clutTriggers, 1, count);
			reader.read_array(trigger->triggerType, 1, count);
			reader.read_array(trigger->origins, 3, count);
			reader.read_array(trigger->scriptDelay, 3, count);
			reader.read_array(trigger->audioTriggers, 1, count);
			reader.read_array(trigger->blendLookup, 1, count);
			reader.read_array(trigger->npcTriggers, 1, count);
			reader.read_array(trigger->contextTriggers, 1, count);
			reader.read_array(trigger->waterTriggers, 1, count);

			if (const auto lists = reader.read_array(trigger->leafLists, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					reader.read_array(lists[i].leafs, 3, lists[i].leafCount);
				}
			}
		}

		void read_spawn_list(zone_reader& reader, SpawnPointRecordList* list)
		{
			if (const auto spawns = reader.read_array(list->spawns, 3, list->spawnsCount))
			{
				for (auto i = 0; i < list->spawnsCount; i++)
				{
					reader.read_script_string(spawns[i].name);
					reader.read_script_string(spawns[i].target);
					reader.read_script_string(spawns[i].script_noteworthy);
					reader.read_script_string(spawns[i].targetname);
				}
			}
		}

		void read_spline_list(zone_reader& reader, SplineRecordList* list)
		{
			const auto splines = reader.read_array(list->splines, 7, list->splineCount);
			if (!splines)
			{
				return;
			}

			for (auto i = 0; i < list->splineCount; i++)
			{
				const auto points = reader.read_array(splines[i].splinePoints, 7, splines[i].splinePointCount);
				if (!points)
				{
					continue;
				}

				for (auto j = 0; j < splines[i].splinePointCount; j++)
				{
					reader.read_array(points[j].splineNodeLabel, 0, 64);
					reader.read_array(points[j].positionCubic, 3, 4);
					reader.read_array(points[j].tangentQuadratic, 3, 3);
				}
			}
		}

		void read_scriptable_instance(zone_reader& reader, ScriptableInstance* instance)
		{
			reader.read_asset(ASSET_TYPE_SCRIPTABLE, instance->def);

			const auto def = instance->def;
			const auto part_count = def ? def->partCount : 0;
			const auto event_stream_size = def ? def->eventStreamSize : 0;
			const auto flags_size = def ? def->flagsSize : 0;
			const auto part_stream_count = def ? def->partStreamCount : 0;
			const auto model_tag_count = def ? def->modelTagCount : 0;

			reader.read_array(instance->flags, 0, flags_size);
			if (const auto states = reader.read_array(instance->partStates, 3, instance->partStateCount))
			{
				for (auto i = 0; i < instance->partStateCount; i++)
				{
					reader.read_script_string(states[i].name);
				}
			}
			reader.read_script_string(instance->name);

			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_stream(instance->partRuntime, 1, 4, part_count);
			reader.pop_stream();

			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(instance->eventStreamBuffer, 0, event_stream_size);
			reader.pop_stream();

			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(instance->modelTagRuntime, 0, model_tag_count);
			reader.pop_stream();

			reader.read_array(instance->partStreams, 1, part_stream_count);
			reader.read_array(instance->linkedObjects, 1, instance->linkedObjectCount);
		}

		void read_scriptable_map_ents(zone_reader& reader, ScriptableMapEnts* ents)
		{
			if (const auto instances = reader.read_array(ents->instances, 7, ents->instanceCount))
			{
				for (auto i = 0u; i < ents->instanceCount; i++)
				{
					read_scriptable_instance(reader, &instances[i]);
				}
			}

			if (const auto reserved = reader.read_array(ents->reservedDefs, 7, ents->reservedDefCount))
			{
				for (auto i = 0u; i < ents->reservedDefCount; i++)
				{
					reader.read_string(reserved[i].name);
				}
			}
		}
	}

	void map_ents::read(zone_reader& reader, MapEnts* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->entityString, 0, asset->numEntityChars);
		read_triggers(reader, &asset->trigger);
		read_client_triggers(reader, &asset->clientTrigger);
		reader.read_array(asset->clientTriggerBlend.blendNodes, 3, asset->clientTriggerBlend.numClientTriggerBlendNodes);
		read_spawn_list(reader, &asset->spawnList);
		read_spline_list(reader, &asset->splineList);
		read_scriptable_map_ents(reader, &asset->scriptableMapEnts);

		if (const auto entries = reader.read_array(asset->unk344.entries, 3, asset->unk344.count))
		{
			for (auto i = 0; i < asset->unk344.count; i++)
			{
				reader.read_script_strings(entries[i].names, 4);
			}
		}

		reader.read_array(asset->unk360.entries, 3, asset->unk360.count);
		reader.pop_stream();
	}

	void addon_map_ents::read(zone_reader& reader, AddonMapEnts* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->entityString, 0, asset->numEntityChars);
		read_triggers(reader, &asset->trigger);

		if (const auto info = reader.read_single(asset->info, 7))
		{
			clip_map::read_info(reader, info);
		}

		if (const auto cmodels = reader.read_array(asset->cmodels, 7, asset->numSubModels))
		{
			for (auto i = 0u; i < asset->numSubModels; i++)
			{
				if (const auto info = reader.read_single(cmodels[i].info, 7))
				{
					clip_map::read_info(reader, info);
				}
			}
		}

		reader.read_array(asset->models, 3, asset->numSubModels);
		reader.read_array(asset->modelBounds, 3, asset->numSubModels);

		if (const auto collisions = reader.read_array(asset->collisions, 7, asset->collisionCount))
		{
			for (auto i = 0u; i < asset->collisionCount; i++)
			{
				auto& collision = collisions[i];
				reader.read_array(collision.vertices, 15, collision.vertexCount);
				reader.read_array(collision.normals, 15, collision.normalCount);
				reader.read_array(collision.indices, 0, collision.indexCount);
				reader.read_array(collision.data, 0, collision.normalCount);
			}
		}

		if (const auto shapes = reader.read_array(asset->shapes, 7, asset->shapeCount))
		{
			for (auto i = 0u; i < asset->shapeCount; i++)
			{
				auto& shape = shapes[i];
				reader.read_array(shape.planes, 15, shape.planeCount);
				reader.read_array(shape.points, 15, shape.pointCount);
				reader.read_array(shape.edges, 3, shape.edgeCount);
			}
		}

		reader.pop_stream();
	}
}
