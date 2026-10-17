#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		const ScriptableDef* find_scriptable_def(zone_base* zone, const ScriptableDef* def)
		{
			if (!def || !def->name)
			{
				return nullptr;
			}

			if (auto* entry = zone->find_asset(ASSET_TYPE_SCRIPTABLE, def->name); entry && !entry->referenced())
			{
				return static_cast<const ScriptableDef*>(entry->pointer());
			}

			return static_cast<const ScriptableDef*>(db_find_asset(ASSET_TYPE_SCRIPTABLE, def->name));
		}

		unsigned int flags_size(const ScriptableDef* def)
		{
			return def ? def->flagsSize : 0;
		}

		unsigned int part_count(const ScriptableDef* def)
		{
			return def ? def->partCount : 0;
		}

		unsigned int event_stream_size(const ScriptableDef* def)
		{
			return def ? def->eventStreamSize : 0;
		}

		unsigned int model_tag_count(const ScriptableDef* def)
		{
			return def ? def->modelTagCount : 0;
		}

		unsigned int part_stream_count(const ScriptableDef* def)
		{
			return def ? def->partStreamCount : 0;
		}

		void read_client_triggers(zone_reader& reader, ClientTriggers* trigger)
		{
			map_ents::read_triggers(reader, &trigger->trigger);

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

		void parse_client_triggers(assetmanager::reader& read, zone_memory* mem, ClientTriggers* trigger)
		{
			map_ents::parse_triggers(read, mem, &trigger->trigger);

			const auto count = trigger->trigger.count;
			parse_array(read, mem, trigger->clientTriggerAabbTree, trigger->numClientTriggerNodes);
			parse_array(read, mem, trigger->triggerString, trigger->triggerStringLength);
			parse_array(read, mem, trigger->visionSetTriggers, count);
			parse_array(read, mem, trigger->lightSetTriggers, count);
			parse_array(read, mem, trigger->clutTriggers, count);
			parse_array(read, mem, trigger->triggerType, count);
			parse_array(read, mem, trigger->origins, count);
			parse_array(read, mem, trigger->scriptDelay, count);
			parse_array(read, mem, trigger->audioTriggers, count);
			parse_array(read, mem, trigger->blendLookup, count);
			parse_array(read, mem, trigger->npcTriggers, count);
			parse_array(read, mem, trigger->contextTriggers, count);
			parse_array(read, mem, trigger->waterTriggers, count);
			if (const auto lists = parse_array(read, mem, trigger->leafLists, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					parse_array(read, mem, lists[i].leafs, lists[i].leafCount);
				}
			}
		}

		void write_client_triggers(zone_buffer* buf, ClientTriggers* trigger, ClientTriggers* dest)
		{
			map_ents::write_triggers(buf, &trigger->trigger, &dest->trigger);

			const auto count = trigger->trigger.count;
			write_array(buf, 3, trigger->clientTriggerAabbTree, trigger->numClientTriggerNodes, dest->clientTriggerAabbTree);
			write_array(buf, 0, trigger->triggerString, trigger->triggerStringLength, dest->triggerString);
			write_array(buf, 1, trigger->visionSetTriggers, count, dest->visionSetTriggers);
			write_array(buf, 1, trigger->lightSetTriggers, count, dest->lightSetTriggers);
			write_array(buf, 1, trigger->clutTriggers, count, dest->clutTriggers);
			write_array(buf, 1, trigger->triggerType, count, dest->triggerType);
			write_array(buf, 3, trigger->origins, count, dest->origins);
			write_array(buf, 3, trigger->scriptDelay, count, dest->scriptDelay);
			write_array(buf, 1, trigger->audioTriggers, count, dest->audioTriggers);
			write_array(buf, 1, trigger->blendLookup, count, dest->blendLookup);
			write_array(buf, 1, trigger->npcTriggers, count, dest->npcTriggers);
			write_array(buf, 1, trigger->contextTriggers, count, dest->contextTriggers);
			write_array(buf, 1, trigger->waterTriggers, count, dest->waterTriggers);
			if (const auto dest_lists = write_array(buf, 7, trigger->leafLists, count, dest->leafLists))
			{
				for (auto i = 0u; i < count; i++)
				{
					write_array(buf, 3, trigger->leafLists[i].leafs, trigger->leafLists[i].leafCount, dest_lists[i].leafs);
				}
			}
		}

		void dump_client_triggers(assetmanager::dumper& write, ClientTriggers* trigger)
		{
			map_ents::dump_triggers(write, &trigger->trigger);

			const auto count = trigger->trigger.count;
			write.dump_array(trigger->clientTriggerAabbTree, trigger->numClientTriggerNodes);
			write.dump_array(trigger->triggerString, trigger->triggerStringLength);
			write.dump_array(trigger->visionSetTriggers, count);
			write.dump_array(trigger->lightSetTriggers, count);
			write.dump_array(trigger->clutTriggers, count);
			write.dump_array(trigger->triggerType, count);
			write.dump_array(trigger->origins, count);
			write.dump_array(trigger->scriptDelay, count);
			write.dump_array(trigger->audioTriggers, count);
			write.dump_array(trigger->blendLookup, count);
			write.dump_array(trigger->npcTriggers, count);
			write.dump_array(trigger->contextTriggers, count);
			write.dump_array(trigger->waterTriggers, count);
			write.dump_array(trigger->leafLists, count);
			for (auto i = 0u; trigger->leafLists && i < count; i++)
			{
				write.dump_array(trigger->leafLists[i].leafs, trigger->leafLists[i].leafCount);
			}
		}

		void read_spline_list(zone_reader& reader, SplineRecordList* list)
		{
			if (const auto splines = reader.read_array(list->splines, 7, list->splineCount))
			{
				for (auto i = 0; i < list->splineCount; i++)
				{
					if (const auto points = reader.read_array(splines[i].splinePoints, 7, splines[i].splinePointCount))
					{
						for (auto j = 0; j < splines[i].splinePointCount; j++)
						{
							reader.read_array(points[j].splineNodeLabel, 0, 64);
							reader.read_array(points[j].positionCubic, 3, 4);
							reader.read_array(points[j].tangentQuadratic, 3, 3);
						}
					}
				}
			}
		}

		void parse_spline_list(assetmanager::reader& read, zone_memory* mem, SplineRecordList* list)
		{
			if (const auto splines = parse_array(read, mem, list->splines, list->splineCount))
			{
				for (auto i = 0; i < list->splineCount; i++)
				{
					if (const auto points = parse_array(read, mem, splines[i].splinePoints, splines[i].splinePointCount))
					{
						for (auto j = 0; j < splines[i].splinePointCount; j++)
						{
							parse_array(read, mem, points[j].splineNodeLabel, 64);
							parse_array(read, mem, points[j].positionCubic, 4);
							parse_array(read, mem, points[j].tangentQuadratic, 3);
						}
					}
				}
			}
		}

		void write_spline_list(zone_buffer* buf, SplineRecordList* list, SplineRecordList* dest)
		{
			if (const auto dest_splines = write_array(buf, 7, list->splines, list->splineCount, dest->splines))
			{
				for (auto i = 0; i < list->splineCount; i++)
				{
					auto* spline = &list->splines[i];
					if (const auto dest_points = write_array(buf, 7, spline->splinePoints, spline->splinePointCount, dest_splines[i].splinePoints))
					{
						for (auto j = 0; j < spline->splinePointCount; j++)
						{
							auto* point = &spline->splinePoints[j];
							write_array(buf, 0, point->splineNodeLabel, 64, dest_points[j].splineNodeLabel);
							write_array(buf, 3, point->positionCubic, 4, dest_points[j].positionCubic);
							write_array(buf, 3, point->tangentQuadratic, 3, dest_points[j].tangentQuadratic);
						}
					}
				}
			}
		}

		void dump_spline_list(assetmanager::dumper& write, SplineRecordList* list)
		{
			write.dump_array(list->splines, list->splineCount);
			for (auto i = 0; list->splines && i < list->splineCount; i++)
			{
				auto* spline = &list->splines[i];
				write.dump_array(spline->splinePoints, spline->splinePointCount);
				for (auto j = 0; spline->splinePoints && j < spline->splinePointCount; j++)
				{
					auto* point = &spline->splinePoints[j];
					write.dump_array(point->splineNodeLabel, 64);
					write.dump_array(point->positionCubic, 4);
					write.dump_array(point->tangentQuadratic, 3);
				}
			}
		}

		void read_scriptable_instance(zone_reader& reader, ScriptableInstance* instance)
		{
			reader.read_asset(ASSET_TYPE_SCRIPTABLE, instance->def);
			const auto* def = instance->def;

			reader.read_array(instance->flags, 0, flags_size(def));
			if (const auto states = reader.read_array(instance->partStates, 3, instance->partStateCount))
			{
				for (auto i = 0; i < instance->partStateCount; i++)
				{
					reader.read_script_string(states[i].name);
				}
			}
			reader.read_script_string(instance->name);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, instance->partRuntime, 1, part_count(def));
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, instance->eventStreamBuffer, 0, event_stream_size(def));
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, instance->modelTagRuntime, 0, model_tag_count(def));
			reader.read_array(instance->partStreams, 1, part_stream_count(def));
			reader.read_array(instance->linkedObjects, 1, instance->linkedObjectCount);
		}

		void parse_scriptable_instance(assetmanager::reader& read, zone_memory* mem, ScriptableInstance* instance)
		{
			instance->def = read.read_asset<ScriptableDef>();
			const auto* def = instance->def;

			parse_array(read, mem, instance->flags, flags_size(def));
			if (const auto states = parse_array(read, mem, instance->partStates, instance->partStateCount))
			{
				for (auto i = 0; i < instance->partStateCount; i++)
				{
					states[i].name = parse_script_string(read);
				}
			}
			instance->name = parse_script_string(read);
			parse_runtime_array(mem, instance->partRuntime, part_count(def));
			parse_runtime_array(mem, instance->eventStreamBuffer, event_stream_size(def));
			parse_runtime_array(mem, instance->modelTagRuntime, model_tag_count(def));
			parse_array(read, mem, instance->partStreams, part_stream_count(def));
			parse_array(read, mem, instance->linkedObjects, instance->linkedObjectCount);
		}

		void write_scriptable_instance(zone_base* zone, zone_buffer* buf, ScriptableInstance* instance, ScriptableInstance* dest)
		{
			dest->def = asset_reference(zone, ASSET_TYPE_SCRIPTABLE, instance->def);
			const auto* def = find_scriptable_def(zone, instance->def);

			write_array(buf, 0, instance->flags, flags_size(def), dest->flags);
			write_array(buf, 3, instance->partStates, instance->partStateCount, dest->partStates);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 1, instance->partRuntime, part_count(def), dest->partRuntime);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 0, instance->eventStreamBuffer, event_stream_size(def), dest->eventStreamBuffer);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 0, instance->modelTagRuntime, model_tag_count(def), dest->modelTagRuntime);
			write_array(buf, 1, instance->partStreams, part_stream_count(def), dest->partStreams);
			write_array(buf, 1, instance->linkedObjects, instance->linkedObjectCount, dest->linkedObjects);
		}

		void dump_scriptable_instance(assetmanager::dumper& write, ScriptableInstance* instance)
		{
			write.dump_asset(instance->def);
			const auto* def = instance->def;

			write.dump_array(instance->flags, flags_size(def));
			write.dump_array(instance->partStates, instance->partStateCount);
			for (auto i = 0; instance->partStates && i < instance->partStateCount; i++)
			{
				dump_script_string(write, instance->partStates[i].name);
			}
			dump_script_string(write, instance->name);
			write.dump_array(instance->partStreams, part_stream_count(def));
			write.dump_array(instance->linkedObjects, instance->linkedObjectCount);
		}

		void write_info_array(zone_base* zone, zone_buffer* buf, ClipInfo* info, ClipInfo*& dest_field)
		{
			if (const auto dest = write_array(buf, 7, info, 1, dest_field))
			{
				clip_map::write_info(zone, buf, info, dest);
			}
		}

		void read_info_array(zone_reader& reader, ClipInfo*& field)
		{
			if (const auto info = reader.read_array(field, 7))
			{
				clip_map::read_info(reader, info);
			}
		}

		void parse_info_array(assetmanager::reader& read, zone_memory* mem, ClipInfo*& field)
		{
			if (const auto info = parse_array(read, mem, field, 1))
			{
				clip_map::parse_info(read, mem, info);
			}
		}

		void dump_info_array(assetmanager::dumper& write, ClipInfo* info)
		{
			write.dump_single(info);
			if (info)
			{
				clip_map::dump_info(write, info);
			}
		}
	}

	void map_ents::read_triggers(zone_reader& reader, MapTriggers* trigger)
	{
		reader.read_array(trigger->models, 7, trigger->count);
		reader.read_array(trigger->hulls, 7, trigger->hullCount);
		reader.read_array(trigger->slabs, 3, trigger->slabCount);
	}

	void map_ents::parse_triggers(assetmanager::reader& read, zone_memory* mem, MapTriggers* trigger)
	{
		parse_array(read, mem, trigger->models, trigger->count);
		parse_array(read, mem, trigger->hulls, trigger->hullCount);
		parse_array(read, mem, trigger->slabs, trigger->slabCount);
	}

	void map_ents::write_triggers(zone_buffer* buf, MapTriggers* trigger, MapTriggers* dest)
	{
		write_array(buf, 7, trigger->models, trigger->count, dest->models);
		write_array(buf, 7, trigger->hulls, trigger->hullCount, dest->hulls);
		write_array(buf, 3, trigger->slabs, trigger->slabCount, dest->slabs);
	}

	void map_ents::dump_triggers(assetmanager::dumper& write, MapTriggers* trigger)
	{
		write.dump_array(trigger->models, trigger->count);
		write.dump_array(trigger->hulls, trigger->hullCount);
		write.dump_array(trigger->slabs, trigger->slabCount);
	}

	void map_ents::read(zone_reader& reader, MapEnts* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->entityString, 0, asset->numEntityChars);
		read_triggers(reader, &asset->trigger);
		read_client_triggers(reader, &asset->clientTrigger);
		reader.read_array(asset->clientTriggerBlend.blendNodes, 3, asset->clientTriggerBlend.numClientTriggerBlendNodes);

		auto* spawn_list = &asset->spawnList;
		if (const auto spawns = reader.read_array(spawn_list->spawns, 3, spawn_list->spawnsCount))
		{
			for (auto i = 0; i < spawn_list->spawnsCount; i++)
			{
				reader.read_script_string(spawns[i].name);
				reader.read_script_string(spawns[i].target);
				reader.read_script_string(spawns[i].script_noteworthy);
				reader.read_script_string(spawns[i].targetname);
			}
		}
		read_spline_list(reader, &asset->splineList);

		auto* scriptable = &asset->scriptableMapEnts;
		if (const auto instances = reader.read_array(scriptable->instances, 7, scriptable->instanceCount))
		{
			for (auto i = 0u; i < scriptable->instanceCount; i++)
			{
				read_scriptable_instance(reader, &instances[i]);
			}
		}
		if (const auto reserved = reader.read_array(scriptable->reservedDefs, 7, scriptable->reservedDefCount))
		{
			for (auto i = 0u; i < scriptable->reservedDefCount; i++)
			{
				reader.read_string(reserved[i].name);
			}
		}

		if (const auto entries = reader.read_array(asset->unk344.entries, 3, asset->unk344.count))
		{
			for (auto i = 0; i < asset->unk344.count; i++)
			{
				reader.read_script_strings(entries[i].names, std::size(entries[i].names));
			}
		}
		reader.read_array(asset->unk360.entries, 3, asset->unk360.count);

		reader.pop_stream();
	}

	MapEnts* map_ents::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_MAP_ENTS, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing mapents \"%s\"...", name.data());

		auto* asset = read.read_single<MapEnts>();
		asset->name = read.read_string();
		parse_array(read, mem, asset->entityString, asset->numEntityChars);
		parse_triggers(read, mem, &asset->trigger);
		parse_client_triggers(read, mem, &asset->clientTrigger);
		parse_array(read, mem, asset->clientTriggerBlend.blendNodes, asset->clientTriggerBlend.numClientTriggerBlendNodes);

		auto* spawn_list = &asset->spawnList;
		if (const auto spawns = parse_array(read, mem, spawn_list->spawns, spawn_list->spawnsCount))
		{
			for (auto i = 0; i < spawn_list->spawnsCount; i++)
			{
				spawns[i].name = parse_script_string(read);
				spawns[i].target = parse_script_string(read);
				spawns[i].script_noteworthy = parse_script_string(read);
				spawns[i].targetname = parse_script_string(read);
			}
		}
		parse_spline_list(read, mem, &asset->splineList);

		auto* scriptable = &asset->scriptableMapEnts;
		if (const auto instances = parse_array(read, mem, scriptable->instances, scriptable->instanceCount))
		{
			for (auto i = 0u; i < scriptable->instanceCount; i++)
			{
				parse_scriptable_instance(read, mem, &instances[i]);
			}
		}
		if (const auto reserved = parse_array(read, mem, scriptable->reservedDefs, scriptable->reservedDefCount))
		{
			for (auto i = 0u; i < scriptable->reservedDefCount; i++)
			{
				reserved[i].name = read.read_string();
			}
		}

		if (const auto entries = parse_array(read, mem, asset->unk344.entries, asset->unk344.count))
		{
			for (auto i = 0; i < asset->unk344.count; i++)
			{
				for (auto& entry_name : entries[i].names)
				{
					entry_name = parse_script_string(read);
				}
			}
		}
		parse_array(read, mem, asset->unk360.entries, asset->unk360.count);
		read.close();

		return asset;
	}

	void map_ents::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;
		script_string_preparer prepare(buf);

		for (auto i = 0; data->spawnList.spawns && i < data->spawnList.spawnsCount; i++)
		{
			auto* spawn = &data->spawnList.spawns[i];
			prepare(spawn->name);
			prepare(spawn->target);
			prepare(spawn->script_noteworthy);
			prepare(spawn->targetname);
		}

		auto* scriptable = &data->scriptableMapEnts;
		for (auto i = 0u; scriptable->instances && i < scriptable->instanceCount; i++)
		{
			auto* instance = &scriptable->instances[i];
			for (auto j = 0; instance->partStates && j < instance->partStateCount; j++)
			{
				prepare(instance->partStates[j].name);
			}
			prepare(instance->name);
		}

		for (auto i = 0; data->unk344.entries && i < data->unk344.count; i++)
		{
			prepare(data->unk344.entries[i].names, std::size(data->unk344.entries[i].names));
		}
	}

	void map_ents::load_depending(zone_base* zone)
	{
		auto* scriptable = &this->asset_->scriptableMapEnts;
		for (auto i = 0u; scriptable->instances && i < scriptable->instanceCount; i++)
		{
			add_dependency(zone, ASSET_TYPE_SCRIPTABLE, scriptable->instances[i].def);
		}
	}

	void map_ents::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = this->write_name(buf);
		write_array(buf, 0, data->entityString, data->numEntityChars, dest->entityString);
		write_triggers(buf, &data->trigger, &dest->trigger);
		write_client_triggers(buf, &data->clientTrigger, &dest->clientTrigger);
		write_array(buf, 3, data->clientTriggerBlend.blendNodes, data->clientTriggerBlend.numClientTriggerBlendNodes, dest->clientTriggerBlend.blendNodes);
		write_array(buf, 3, data->spawnList.spawns, data->spawnList.spawnsCount, dest->spawnList.spawns);
		write_spline_list(buf, &data->splineList, &dest->splineList);

		auto* scriptable = &data->scriptableMapEnts;
		auto* dest_scriptable = &dest->scriptableMapEnts;
		if (const auto dest_instances = write_array(buf, 7, scriptable->instances, scriptable->instanceCount, dest_scriptable->instances))
		{
			for (auto i = 0u; i < scriptable->instanceCount; i++)
			{
				write_scriptable_instance(zone, buf, &scriptable->instances[i], &dest_instances[i]);
			}
		}
		if (const auto dest_reserved = write_array(buf, 7, scriptable->reservedDefs, scriptable->reservedDefCount, dest_scriptable->reservedDefs))
		{
			for (auto i = 0u; i < scriptable->reservedDefCount; i++)
			{
				write_string(buf, scriptable->reservedDefs[i].name, dest_reserved[i].name);
			}
		}

		write_array(buf, 3, data->unk344.entries, data->unk344.count, dest->unk344.entries);
		write_array(buf, 3, data->unk360.entries, data->unk360.count, dest->unk360.entries);

		buf->pop_stream();
	}

	void map_ents::dump(MapEnts* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_MAP_ENTS, asset->name)))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_array(asset->entityString, asset->numEntityChars);
		dump_triggers(write, &asset->trigger);
		dump_client_triggers(write, &asset->clientTrigger);
		write.dump_array(asset->clientTriggerBlend.blendNodes, asset->clientTriggerBlend.numClientTriggerBlendNodes);

		auto* spawn_list = &asset->spawnList;
		write.dump_array(spawn_list->spawns, spawn_list->spawnsCount);
		for (auto i = 0; spawn_list->spawns && i < spawn_list->spawnsCount; i++)
		{
			dump_script_string(write, spawn_list->spawns[i].name);
			dump_script_string(write, spawn_list->spawns[i].target);
			dump_script_string(write, spawn_list->spawns[i].script_noteworthy);
			dump_script_string(write, spawn_list->spawns[i].targetname);
		}
		dump_spline_list(write, &asset->splineList);

		auto* scriptable = &asset->scriptableMapEnts;
		write.dump_array(scriptable->instances, scriptable->instanceCount);
		for (auto i = 0u; scriptable->instances && i < scriptable->instanceCount; i++)
		{
			dump_scriptable_instance(write, &scriptable->instances[i]);
		}
		write.dump_array(scriptable->reservedDefs, scriptable->reservedDefCount);
		for (auto i = 0u; scriptable->reservedDefs && i < scriptable->reservedDefCount; i++)
		{
			write.dump_string(scriptable->reservedDefs[i].name);
		}

		write.dump_array(asset->unk344.entries, asset->unk344.count);
		for (auto i = 0; asset->unk344.entries && i < asset->unk344.count; i++)
		{
			dump_script_strings(write, asset->unk344.entries[i].names, std::size(asset->unk344.entries[i].names));
		}
		write.dump_array(asset->unk360.entries, asset->unk360.count);
		write.close();
	}

	void addon_map_ents::read(zone_reader& reader, AddonMapEnts* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->entityString, 0, asset->numEntityChars);
		map_ents::read_triggers(reader, &asset->trigger);
		read_info_array(reader, asset->info);

		if (const auto models = reader.read_array(asset->cmodels, 7, asset->numSubModels))
		{
			for (auto i = 0u; i < asset->numSubModels; i++)
			{
				read_info_array(reader, models[i].info);
			}
		}
		reader.read_array(asset->models, 3, asset->numSubModels);
		reader.read_array(asset->modelBounds, 3, asset->numSubModels);

		if (const auto collisions = reader.read_array(asset->collisions, 7, asset->collisionCount))
		{
			for (auto i = 0u; i < asset->collisionCount; i++)
			{
				auto* collision = &collisions[i];
				reader.read_array(collision->vertices, 15, collision->vertexCount);
				reader.read_array(collision->normals, 15, collision->normalCount);
				reader.read_array(collision->indices, 0, collision->indexCount);
				reader.read_array(collision->data, 0, collision->normalCount);
			}
		}
		if (const auto shapes = reader.read_array(asset->shapes, 7, asset->shapeCount))
		{
			for (auto i = 0u; i < asset->shapeCount; i++)
			{
				auto* shape = &shapes[i];
				reader.read_array(shape->planes, 15, shape->planeCount);
				reader.read_array(shape->points, 15, shape->pointCount);
				reader.read_array(shape->edges, 3, shape->edgeCount);
			}
		}

		reader.pop_stream();
	}

	AddonMapEnts* addon_map_ents::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_ADDON_MAP_ENTS, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing addon_map_ents \"%s\"...", name.data());

		auto* asset = read.read_single<AddonMapEnts>();
		asset->name = read.read_string();
		parse_array(read, mem, asset->entityString, asset->numEntityChars);
		map_ents::parse_triggers(read, mem, &asset->trigger);
		parse_info_array(read, mem, asset->info);

		if (const auto models = parse_array(read, mem, asset->cmodels, asset->numSubModels))
		{
			for (auto i = 0u; i < asset->numSubModels; i++)
			{
				parse_info_array(read, mem, models[i].info);
			}
		}
		parse_array(read, mem, asset->models, asset->numSubModels);
		parse_array(read, mem, asset->modelBounds, asset->numSubModels);

		if (const auto collisions = parse_array(read, mem, asset->collisions, asset->collisionCount))
		{
			for (auto i = 0u; i < asset->collisionCount; i++)
			{
				auto* collision = &collisions[i];
				parse_array(read, mem, collision->vertices, collision->vertexCount);
				parse_array(read, mem, collision->normals, collision->normalCount);
				parse_array(read, mem, collision->indices, collision->indexCount);
				parse_array(read, mem, collision->data, collision->normalCount);
			}
		}
		if (const auto shapes = parse_array(read, mem, asset->shapes, asset->shapeCount))
		{
			for (auto i = 0u; i < asset->shapeCount; i++)
			{
				auto* shape = &shapes[i];
				parse_array(read, mem, shape->planes, shape->planeCount);
				parse_array(read, mem, shape->points, shape->pointCount);
				parse_array(read, mem, shape->edges, shape->edgeCount);
			}
		}
		read.close();

		return asset;
	}

	void addon_map_ents::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;
		if (data->info)
		{
			clip_map::add_info_dependencies(zone, data->info);
		}
		for (auto i = 0u; data->cmodels && i < data->numSubModels; i++)
		{
			if (data->cmodels[i].info)
			{
				clip_map::add_info_dependencies(zone, data->cmodels[i].info);
			}
		}
	}

	void addon_map_ents::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = this->write_name(buf);
		write_array(buf, 0, data->entityString, data->numEntityChars, dest->entityString);
		map_ents::write_triggers(buf, &data->trigger, &dest->trigger);
		write_info_array(zone, buf, data->info, dest->info);

		if (const auto dest_models = write_array(buf, 7, data->cmodels, data->numSubModels, dest->cmodels))
		{
			for (auto i = 0u; i < data->numSubModels; i++)
			{
				write_info_array(zone, buf, data->cmodels[i].info, dest_models[i].info);
			}
		}
		write_array(buf, 3, data->models, data->numSubModels, dest->models);
		write_array(buf, 3, data->modelBounds, data->numSubModels, dest->modelBounds);

		if (const auto dest_collisions = write_array(buf, 7, data->collisions, data->collisionCount, dest->collisions))
		{
			for (auto i = 0u; i < data->collisionCount; i++)
			{
				auto* collision = &data->collisions[i];
				auto* dest_collision = &dest_collisions[i];
				write_array(buf, 15, collision->vertices, collision->vertexCount, dest_collision->vertices);
				write_array(buf, 15, collision->normals, collision->normalCount, dest_collision->normals);
				write_array(buf, 0, collision->indices, collision->indexCount, dest_collision->indices);
				write_array(buf, 0, collision->data, collision->normalCount, dest_collision->data);
			}
		}
		if (const auto dest_shapes = write_array(buf, 7, data->shapes, data->shapeCount, dest->shapes))
		{
			for (auto i = 0u; i < data->shapeCount; i++)
			{
				auto* shape = &data->shapes[i];
				auto* dest_shape = &dest_shapes[i];
				write_array(buf, 15, shape->planes, shape->planeCount, dest_shape->planes);
				write_array(buf, 15, shape->points, shape->pointCount, dest_shape->points);
				write_array(buf, 3, shape->edges, shape->edgeCount, dest_shape->edges);
			}
		}

		buf->pop_stream();
	}

	void addon_map_ents::dump(AddonMapEnts* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_ADDON_MAP_ENTS, asset->name)))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_array(asset->entityString, asset->numEntityChars);
		map_ents::dump_triggers(write, &asset->trigger);
		dump_info_array(write, asset->info);

		write.dump_array(asset->cmodels, asset->numSubModels);
		for (auto i = 0u; asset->cmodels && i < asset->numSubModels; i++)
		{
			dump_info_array(write, asset->cmodels[i].info);
		}
		write.dump_array(asset->models, asset->numSubModels);
		write.dump_array(asset->modelBounds, asset->numSubModels);

		write.dump_array(asset->collisions, asset->collisionCount);
		for (auto i = 0u; asset->collisions && i < asset->collisionCount; i++)
		{
			auto* collision = &asset->collisions[i];
			write.dump_array(collision->vertices, collision->vertexCount);
			write.dump_array(collision->normals, collision->normalCount);
			write.dump_array(collision->indices, collision->indexCount);
			write.dump_array(collision->data, collision->normalCount);
		}
		write.dump_array(asset->shapes, asset->shapeCount);
		for (auto i = 0u; asset->shapes && i < asset->shapeCount; i++)
		{
			auto* shape = &asset->shapes[i];
			write.dump_array(shape->planes, shape->planeCount);
			write.dump_array(shape->points, shape->pointCount);
			write.dump_array(shape->edges, shape->edgeCount);
		}
		write.close();
	}
}
