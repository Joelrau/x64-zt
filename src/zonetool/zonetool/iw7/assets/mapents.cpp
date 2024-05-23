#include <std_include.hpp>
#include "mapents.hpp"

namespace zonetool::iw7
{
	void map_ents::add_script_string(scr_string_t* ptr, const char* str)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return;
			}
		}
		this->script_strings.push_back(std::pair<scr_string_t*, const char*>(ptr, str));
	}

	const char* map_ents::get_script_string(scr_string_t* ptr)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return this->script_strings[i].second;
			}
		}
		return nullptr;
	}

	void map_ents::parse_spawn_list(zone_memory* mem, std::string name, SpawnPointRecordList* spawnList)
	{
		const auto path = name + ".ents.spawnList"s;
		filesystem::file file(path);
		file.open("rb");
		if (!file.get_fp())
		{
			return;
		}

		const auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();

		auto data = json::parse(bytes);

		spawnList->spawnsCount = static_cast<unsigned short>(data.size());
		spawnList->spawns = mem->allocate<SpawnPointEntityRecord>(spawnList->spawnsCount);

		for (unsigned short i = 0; i < spawnList->spawnsCount; i++)
		{
			spawnList->spawns[i].index = i;
			add_script_string(&spawnList->spawns[i].name, mem->duplicate_string(data[i]["name"].get<std::string>()));
			add_script_string(&spawnList->spawns[i].target, mem->duplicate_string(data[i]["target"].get<std::string>()));
			add_script_string(&spawnList->spawns[i].script_noteworthy, mem->duplicate_string(data[i]["script_noteworthy"].get<std::string>()));
			for (auto j = 0; j < 3; j++)
			{
				spawnList->spawns[i].origin[j] = data[i]["origin"][j].get<float>();
				spawnList->spawns[i].angles[j] = data[i]["angles"][j].get<float>();
			}
		}
	}

	void map_ents::parse_spawners(zone_memory* mem, std::string name, SpawnerList* spawners)
	{
		const auto path = name + ".ents.spawners"s;
		filesystem::file file(path);
		file.open("rb");
		if (!file.get_fp())
		{
			return;
		}

		const auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();

		auto data = json::parse(bytes);

		spawners->spawnerCount = static_cast<unsigned int>(data.size());
		spawners->spawnerList = mem->allocate<Spawner>(spawners->spawnerCount);

		for (unsigned int i = 0; i < spawners->spawnerCount; i++)
		{
			add_script_string(&spawners->spawnerList[i].targetname, mem->duplicate_string(data[i]["targetname"].get<std::string>()));
			add_script_string(&spawners->spawnerList[i].classname, mem->duplicate_string(data[i]["classname"].get<std::string>()));
			spawners->spawnerList[i].count = data[i]["count"].get<int>();

			for (auto j = 0; j < 3; j++)
			{
				spawners->spawnerList[i].origin[j] = data[i]["origin"][j].get<float>();
				spawners->spawnerList[i].angles[j] = data[i]["angles"][j].get<float>();
			}

			spawners->spawnerList[i].numFields = static_cast<unsigned int>(data[i]["fields"].size());
			for (unsigned int j = 0; j < spawners->spawnerList[i].numFields; j++)
			{
				add_script_string(&spawners->spawnerList[i].fields[j].key, mem->duplicate_string(data[i]["fields"][j]["key"].get<std::string>()));
				add_script_string(&spawners->spawnerList[i].fields[j].value, mem->duplicate_string(data[i]["fields"][j]["value"].get<std::string>()));
				spawners->spawnerList[i].fields[j].type = data[i]["fields"][j]["type"].get<SpawnerFieldType>();

				spawners->spawnerList[i].fields[j].keyCanonical = 0; // this being 0 makes the game generate it at runtime
			}
		}
	}

	void map_ents::parse_entity_strings(zone_memory* mem, std::string name, char** entityStrings, int* numEntityChars)
	{
		const auto path = name + ".ents"s;
		auto file = filesystem::file(path);
		file.open("rb");
		if (!file.get_fp())
		{
			return;
		}

		*numEntityChars = static_cast<int>(file.size());

		*entityStrings = mem->allocate<char>(static_cast<size_t>(*numEntityChars + 1));
		memset(*entityStrings, 0, *numEntityChars);

		fread(*entityStrings, *numEntityChars, 1, file.get_fp());
		(*entityStrings)[*numEntityChars] = '\0';

		file.close();
	}

	MapEnts* map_ents::parse(std::string name, zone_memory* mem)
	{
		ZONETOOL_INFO("Parsing mapents \"%s\"...", name.data());

		assetmanager::reader reader(mem);
		const auto path = name + ".ents.data";
		if (!reader.open(path))
		{
			return nullptr;
		}

		MapEnts* asset = reader.read_single<MapEnts>();
		asset->name = mem->duplicate_string(name);

		parse_entity_strings(mem, name, &asset->entityString, &asset->numEntityChars);
		parse_spawn_list(mem, name, &asset->spawnList);
		parse_spawners(mem, name, &asset->spawners);

		asset->trigger.models = reader.read_array<TriggerModel>();
		for (unsigned int i = 0; i < asset->trigger.count; i++)
		{
			asset->trigger.models[i].physicsAsset = reader.read_asset<PhysicsAsset>();
		}
		asset->trigger.hulls = reader.read_array<TriggerHull>();
		asset->trigger.slabs = reader.read_array<TriggerSlab>();
		asset->trigger.windings = reader.read_array<TriggerWinding>();
		asset->trigger.windingPoints = reader.read_array<TriggerWindingPoint>();

		asset->clientTrigger.trigger.models = reader.read_array<TriggerModel>();
		for (unsigned int i = 0; i < asset->clientTrigger.trigger.count; i++)
		{
			asset->clientTrigger.trigger.models[i].physicsAsset = reader.read_asset<PhysicsAsset>();
		}
		asset->clientTrigger.trigger.hulls = reader.read_array<TriggerHull>();
		asset->clientTrigger.trigger.slabs = reader.read_array<TriggerSlab>();
		asset->clientTrigger.trigger.windings = reader.read_array<TriggerWinding>();
		asset->clientTrigger.trigger.windingPoints = reader.read_array<TriggerWindingPoint>();
		asset->clientTrigger.triggerString = reader.read_array<char>();
		asset->clientTrigger.visionSetTriggers = reader.read_array<short>();
		asset->clientTrigger.triggerType = reader.read_array<unsigned char>();
		asset->clientTrigger.origins = reader.read_array<vec3_t>();
		asset->clientTrigger.scriptDelay = reader.read_array<float>();
		asset->clientTrigger.audioTriggers = reader.read_array<short>();
		asset->clientTrigger.blendLookup = reader.read_array<short>();
		asset->clientTrigger.npcTriggers = reader.read_array<short>();
		asset->clientTrigger.audioStateIds = reader.read_array<short>();
		asset->clientTrigger.audioRvbPanInfo = reader.read_array<CTAudRvbPanInfo>();
		asset->clientTrigger.transientIndex = reader.read_array<short>();
		if (asset->clientTrigger.linkTo)
		{
			asset->clientTrigger.linkTo = reader.read_array<ClientEntityLinkToDef*>();
			for (unsigned int i = 0; i < asset->clientTrigger.trigger.count; i++)
			{
				asset->clientTrigger.linkTo[i] = reader.read_single<ClientEntityLinkToDef>();
			}
		}

		asset->clientTriggerBlend.blendNodes = reader.read_array<ClientTriggerBlendNode>();

		asset->splineList.splines = reader.read_array<SplinePointRecordList>();
		for (unsigned short i = 0; i < asset->splineList.splineCount; i++)
		{
			asset->splineList.splines[i].splinePoints = reader.read_array<SplinePointEntityRecord>();
			for (unsigned short j = 0; j < asset->splineList.splines[i].splinePointCount; j++)
			{
				add_script_string(&asset->splineList.splines[i].splinePoints[j].splineNodeLabel, reader.read_string());
				add_script_string(&asset->splineList.splines[i].splinePoints[j].targetname, reader.read_string());
				add_script_string(&asset->splineList.splines[i].splinePoints[j].target, reader.read_string());
				add_script_string(&asset->splineList.splines[i].splinePoints[j].string, reader.read_string());
			}
		}

		asset->havokEntsShapeData = reader.read_array<char>();

		asset->cmodels = reader.read_array<cmodel_t>();
		for (unsigned int i = 0; i < asset->numSubModels; i++)
		{
			asset->cmodels[i].info = reader.read_single<ClipInfo>();
			if (asset->cmodels[i].info)
			{
				clip_map::parse_info(asset->cmodels[i].info, reader, mem);
			}

			asset->cmodels[i].physicsAsset = reader.read_asset<PhysicsAsset>();
		}

		asset->dynEntDefList[0] = reader.read_array<DynEntityDef>();
		for (unsigned short i = 0; i < asset->dynEntCount[0]; i++)
		{
			asset->dynEntDefList[0][i].baseModel = reader.read_asset<XModel>();
			asset->dynEntDefList[0][i].linkTo = reader.read_single<DynEntityLinkToDef>();
		}

		asset->dynEntDefList[1] = reader.read_array<DynEntityDef>();
		for (unsigned short i = 0; i < asset->dynEntCount[1]; i++)
		{
			asset->dynEntDefList[1][i].baseModel = reader.read_asset<XModel>();
			asset->dynEntDefList[1][i].linkTo = reader.read_single<DynEntityLinkToDef>();
		}

		asset->dynEntPoseList[0][0] = reader.read_array<DynEntityPose>();
		for (unsigned short i = 0; i < asset->dynEntCount[0]; i++)
		{
			asset->dynEntPoseList[0][0][i].poses = reader.read_array<GfxPlacement>();
			asset->dynEntPoseList[0][0][i].unk = reader.read_array<char>();
		}

		asset->dynEntPoseList[1][0] = reader.read_array<DynEntityPose>();
		for (unsigned short i = 0; i < asset->dynEntCount[0]; i++)
		{
			asset->dynEntPoseList[1][0][i].poses = reader.read_array<GfxPlacement>();
			asset->dynEntPoseList[1][0][i].unk = reader.read_array<char>();
		}

		asset->dynEntPoseList[0][1] = reader.read_array<DynEntityPose>();
		for (unsigned short i = 0; i < asset->dynEntCount[1]; i++)
		{
			asset->dynEntPoseList[0][1][i].poses = reader.read_array<GfxPlacement>();
			asset->dynEntPoseList[0][1][i].unk = reader.read_array<char>();
		}

		asset->dynEntPoseList[1][1] = reader.read_array<DynEntityPose>();
		for (unsigned short i = 0; i < asset->dynEntCount[1]; i++)
		{
			asset->dynEntPoseList[1][1][i].poses = reader.read_array<GfxPlacement>();
			asset->dynEntPoseList[1][1][i].unk = reader.read_array<char>();
		}

		asset->dynEntClientList[0][0] = mem->allocate<DynEntityClient>(asset->dynEntCount[0]);
		asset->dynEntClientList[1][0] = mem->allocate<DynEntityClient>(asset->dynEntCount[0]);
		asset->dynEntClientList[0][1] = mem->allocate<DynEntityClient>(asset->dynEntCount[1]);
		asset->dynEntClientList[1][1] = mem->allocate<DynEntityClient>(asset->dynEntCount[1]);

		asset->dynEntGlobalIdList[0] = reader.read_array<DynEntityGlobalId>();
		asset->dynEntGlobalIdList[1] = reader.read_array<DynEntityGlobalId>();

		asset->unk2 = reader.read_array<unk_1453E4298>();
		asset->unk2_1[0] = reader.read_array<unk_1453E42A8>();
		asset->unk2_1[1] = reader.read_array<unk_1453E42A8>();
		asset->unk2_2[0] = reader.read_array<unk_1453E1130>();
		asset->unk2_2[1] = reader.read_array<unk_1453E1130>();

		asset->unk3 = reader.read_array<unsigned int>();

		asset->clientEntAnchors = reader.read_array<ClientEntAnchor>();
		for (unsigned int i = 0; i < asset->clientEntAnchorCount; i++)
		{
			add_script_string(&asset->clientEntAnchors[i].name, reader.read_string());
		}

		asset->scriptableMapEnts.instances = reader.read_array<ScriptableInstance>();
		for (unsigned int i = 0; i < asset->scriptableMapEnts.totalInstanceCount; i++)
		{
			asset->scriptableMapEnts.instances[i].unk01.unk01.def = reader.read_asset<ScriptableDef>();
			asset->scriptableMapEnts.instances[i].unk01.unk01.unk01.model = reader.read_asset<XModel>();
			asset->scriptableMapEnts.instances[i].unk01.unk01.eventStreamBuffer = reader.read_array<char>();

			asset->scriptableMapEnts.instances[i].unk02[0].unk01.def = reader.read_asset<ScriptableDef>();
			asset->scriptableMapEnts.instances[i].unk02[0].unk01.unk01.model = reader.read_asset<XModel>();
			asset->scriptableMapEnts.instances[i].unk02[0].unk01.eventStreamBuffer = reader.read_array<char>();

			asset->scriptableMapEnts.instances[i].unk02[1].unk01.def = reader.read_asset<ScriptableDef>();
			asset->scriptableMapEnts.instances[i].unk02[1].unk01.unk01.model = reader.read_asset<XModel>();
			asset->scriptableMapEnts.instances[i].unk02[1].unk01.eventStreamBuffer = reader.read_array<char>();

			add_script_string(&asset->scriptableMapEnts.instances[i].unk03, reader.read_string());
			asset->scriptableMapEnts.instances[i].unk04 = reader.read_string();
			add_script_string(&asset->scriptableMapEnts.instances[i].targetname, reader.read_string());
		}

		asset->scriptableMapEnts.unk.unk01 = reader.read_array<unk_1453E2558>();
		asset->scriptableMapEnts.unk.unk02_1 = reader.read_array<unk_1453E2558>();
		asset->scriptableMapEnts.unk.unk02_2 = reader.read_array<unk_1453E2558>();

		asset->scriptableMapEnts.reservedDynents[0].reservedDynents = reader.read_array<ScriptableReservedDynent>();
		asset->scriptableMapEnts.reservedDynents[1].reservedDynents = reader.read_array<ScriptableReservedDynent>();

		asset->mayhemScenes = reader.read_array<MayhemInstance>();
		for (unsigned int i = 0; i < asset->numMayhemScenes; i++)
		{
			asset->mayhemScenes[i].mayhem = reader.read_asset<MayhemData>();
			asset->mayhemScenes[i].linkTo = reader.read_single<ClientEntityLinkToDef>();
			add_script_string(&asset->mayhemScenes[i].scriptName, reader.read_string());
		}

		asset->audioPASpeakers = reader.read_array<AudioPASpeaker>();

		reader.close();

		return asset;
	}

	void map_ents::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = "maps/"s + (filesystem::get_fastfile().substr(0, 3) == "mp_" ? "mp/" : "") + filesystem::get_fastfile() + ".d3dbsp"; // name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not find mapents: \"%s\"", this->name_.data());
		}
	}

	void map_ents::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;

		for (int i = 0; i < data->spawnList.spawnsCount; i++)
		{
			data->spawnList.spawns[i].name = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->spawnList.spawns[i].name)));

			data->spawnList.spawns[i].target = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->spawnList.spawns[i].target)));

			data->spawnList.spawns[i].script_noteworthy = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->spawnList.spawns[i].script_noteworthy)));
		}

		for (unsigned int i = 0; i < data->spawners.spawnerCount; i++)
		{
			data->spawners.spawnerList[i].targetname = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->spawners.spawnerList[i].targetname)));
			data->spawners.spawnerList[i].classname = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->spawners.spawnerList[i].classname)));

			for (unsigned int j = 0; j < data->spawners.spawnerList[i].numFields; j++)
			{
				data->spawners.spawnerList[i].fields[j].key = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->spawners.spawnerList[i].fields[j].key)));
				data->spawners.spawnerList[i].fields[j].value = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->spawners.spawnerList[i].fields[j].value)));
			}
		}

		for (unsigned short i = 0; i < data->splineList.splineCount; i++)
		{
			for (unsigned short j = 0; j < data->splineList.splines[i].splinePointCount; j++)
			{
				data->splineList.splines[i].splinePoints[j].splineNodeLabel = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->splineList.splines[i].splinePoints[j].splineNodeLabel)));
				data->splineList.splines[i].splinePoints[j].targetname = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->splineList.splines[i].splinePoints[j].targetname)));
				data->splineList.splines[i].splinePoints[j].target = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->splineList.splines[i].splinePoints[j].target)));
				data->splineList.splines[i].splinePoints[j].string = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->splineList.splines[i].splinePoints[j].string)));
			}
		}

		for (unsigned int i = 0; i < data->clientEntAnchorCount; i++)
		{
			data->clientEntAnchors[i].name = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->clientEntAnchors[i].name)));
		}

		for (unsigned int i = 0; i < data->scriptableMapEnts.totalInstanceCount; i++)
		{
			data->scriptableMapEnts.instances[i].unk03 = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->scriptableMapEnts.instances[i].unk03)));
			data->scriptableMapEnts.instances[i].targetname = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->scriptableMapEnts.instances[i].targetname)));
		}

		for (unsigned int i = 0; i < data->numMayhemScenes; i++)
		{
			data->mayhemScenes[i].scriptName = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->mayhemScenes[i].scriptName)));
		}
	}

	void map_ents::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;

		if (data->trigger.models)
		{
			for (unsigned int i = 0; i < data->trigger.count; i++)
			{
				if (data->trigger.models[i].physicsAsset)
				{
					zone->add_asset_of_type(ASSET_TYPE_PHYSICSASSET, data->trigger.models[i].physicsAsset->name);
				}
			}
		}

		if (data->clientTrigger.trigger.models)
		{
			for (unsigned int i = 0; i < data->clientTrigger.trigger.count; i++)
			{
				if (data->clientTrigger.trigger.models[i].physicsAsset)
				{
					zone->add_asset_of_type(ASSET_TYPE_PHYSICSASSET, data->clientTrigger.trigger.models[i].physicsAsset->name);
				}
			}
		}
		
		for (unsigned int i = 0; i < data->numSubModels; i++)
		{
			if (data->cmodels[i].physicsAsset)
			{
				zone->add_asset_of_type(ASSET_TYPE_PHYSICSASSET, data->cmodels[i].physicsAsset->name);
			}
		}

		for (unsigned short i = 0; i < data->dynEntCount[0]; i++)
		{
			if (data->dynEntDefList[0][i].baseModel)
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->dynEntDefList[0][i].baseModel->name);
			}
		}

		for (unsigned short i = 0; i < data->dynEntCount[1]; i++)
		{
			if (data->dynEntDefList[1][i].baseModel)
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->dynEntDefList[1][i].baseModel->name);
			}
		}

		for (unsigned int i = 0; i < data->scriptableMapEnts.totalInstanceCount; i++)
		{
			if (data->scriptableMapEnts.instances[i].unk01.unk01.def)
			{
				zone->add_asset_of_type(ASSET_TYPE_SCRIPTABLE, data->scriptableMapEnts.instances[i].unk01.unk01.def->name);
			}

			if (data->scriptableMapEnts.instances[i].unk01.unk01.unk01.model)
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->scriptableMapEnts.instances[i].unk01.unk01.unk01.model->name);
			}

			if (data->scriptableMapEnts.instances[i].unk02[0].unk01.def)
			{
				zone->add_asset_of_type(ASSET_TYPE_SCRIPTABLE, data->scriptableMapEnts.instances[i].unk02[0].unk01.def->name);
			}

			if (data->scriptableMapEnts.instances[i].unk02[0].unk01.unk01.model)
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->scriptableMapEnts.instances[i].unk02[0].unk01.unk01.model->name);
			}

			if (data->scriptableMapEnts.instances[i].unk02[1].unk01.def)
			{
				zone->add_asset_of_type(ASSET_TYPE_SCRIPTABLE, data->scriptableMapEnts.instances[i].unk02[1].unk01.def->name);
			}

			if (data->scriptableMapEnts.instances[i].unk02[1].unk01.unk01.model)
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->scriptableMapEnts.instances[i].unk02[1].unk01.unk01.model->name);
			}
		}

		for (unsigned int i = 0; i < data->numMayhemScenes; i++)
		{
			if (data->mayhemScenes[i].mayhem)
			{
				zone->add_asset_of_type(ASSET_TYPE_MAYHEM, data->mayhemScenes[i].mayhem->name);
			}
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

	void map_ents::write_triggers(zone_base* zone, zone_buffer* buf, MapTriggers* dest)
	{
		if (dest->models)
		{
			TriggerModel* dest_model;
			dest->models = buf->write_s(7, dest->models, dest->count, sizeof(TriggerModel), &dest_model);
			if (reinterpret_cast<std::uint64_t>(dest_model) == buf->data_following)
			{
				if (dest_model->physicsAsset)
				{
					dest_model->physicsAsset = reinterpret_cast<PhysicsAsset*>(
						zone->get_asset_pointer(ASSET_TYPE_PHYSICSASSET, dest_model->physicsAsset->name));
				}
			}
		}

		if (dest->hulls)
		{
			dest->hulls = buf->write_s(3, dest->hulls, dest->hullCount);
		}

		if (dest->slabs)
		{
			dest->slabs = buf->write_s(3, dest->slabs, dest->slabCount);
		}

		if (dest->windings)
		{
			dest->windings = buf->write_s(3, dest->windings, dest->windingCount);
		}

		if (dest->windingPoints)
		{
			dest->windingPoints = buf->write_s(3, dest->windingPoints, dest->windingPointCount);
		}
	}

	void map_ents::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->entityString)
		{
			buf->align(0);
			buf->write(data->entityString, data->numEntityChars);
			buf->clear_pointer(&dest->entityString);
		}

		write_triggers(zone, buf, &dest->trigger);
		write_triggers(zone, buf, &dest->clientTrigger.trigger);

		if (data->clientTrigger.triggerString)
		{
			buf->align(0);
			buf->write(data->clientTrigger.triggerString, data->clientTrigger.triggerStringLength);
			buf->clear_pointer(&dest->clientTrigger.triggerString);
		}
		if (data->clientTrigger.visionSetTriggers)
		{
			buf->align(1);
			buf->write(data->clientTrigger.visionSetTriggers, data->clientTrigger.trigger.count);
			buf->clear_pointer(&dest->clientTrigger.visionSetTriggers);
		}
		if (data->clientTrigger.triggerType)
		{
			buf->align(0);
			buf->write(data->clientTrigger.triggerType, data->clientTrigger.trigger.count);
			buf->clear_pointer(&dest->clientTrigger.triggerType);
		}
		if (data->clientTrigger.origins)
		{
			buf->align(3);
			buf->write(data->clientTrigger.origins, data->clientTrigger.trigger.count);
			buf->clear_pointer(&dest->clientTrigger.origins);
		}
		if (data->clientTrigger.scriptDelay)
		{
			buf->align(3);
			buf->write(data->clientTrigger.scriptDelay, data->clientTrigger.trigger.count);
			buf->clear_pointer(&dest->clientTrigger.scriptDelay);
		}
		if (data->clientTrigger.audioTriggers)
		{
			buf->align(1);
			buf->write(data->clientTrigger.audioTriggers, data->clientTrigger.trigger.count);
			buf->clear_pointer(&dest->clientTrigger.audioTriggers);
		}
		if (data->clientTrigger.blendLookup)
		{
			buf->align(1);
			buf->write(data->clientTrigger.blendLookup, data->clientTrigger.trigger.count);
			buf->clear_pointer(&dest->clientTrigger.blendLookup);
		}
		if (data->clientTrigger.npcTriggers)
		{
			buf->align(1);
			buf->write(data->clientTrigger.npcTriggers, data->clientTrigger.trigger.count);
			buf->clear_pointer(&dest->clientTrigger.npcTriggers);
		}
		if (data->clientTrigger.audioStateIds)
		{
			buf->align(1);
			buf->write(data->clientTrigger.audioStateIds, data->clientTrigger.trigger.count);
			buf->clear_pointer(&dest->clientTrigger.audioStateIds);
		}
		if (data->clientTrigger.audioRvbPanInfo)
		{
			buf->align(3);
			buf->write(data->clientTrigger.audioRvbPanInfo, data->clientTrigger.trigger.count);
			buf->clear_pointer(&dest->clientTrigger.audioRvbPanInfo);
		}
		if (data->clientTrigger.transientIndex)
		{
			buf->align(1);
			buf->write(data->clientTrigger.transientIndex, data->clientTrigger.trigger.count);
			buf->clear_pointer(&dest->clientTrigger.transientIndex);
		}
		if (data->clientTrigger.linkTo)
		{
			buf->align(7);
			auto* dest_linkto = buf->write(data->clientTrigger.linkTo, data->clientTrigger.trigger.count);
			for (unsigned int i = 0; i < data->clientTrigger.trigger.count; i++)
			{
				if (data->clientTrigger.linkTo[i])
				{
					dest_linkto[i] = buf->write_s(3, data->clientTrigger.linkTo[i]);
				}
			}
			buf->clear_pointer(&dest->clientTrigger.linkTo);
		}

		if (data->clientTriggerBlend.blendNodes)
		{
			buf->align(3);
			buf->write(data->clientTriggerBlend.blendNodes, data->clientTriggerBlend.numClientTriggerBlendNodes);
			buf->clear_pointer(&dest->clientTriggerBlend.blendNodes);
		}

		if (data->spawnList.spawns)
		{
			buf->align(3);
			buf->write(data->spawnList.spawns, data->spawnList.spawnsCount);
			buf->clear_pointer(&dest->spawnList.spawns);
		}

		if (data->splineList.splines)
		{
			buf->align(7);
			auto destsplines = buf->write(data->splineList.splines, data->splineList.splineCount);
			for (unsigned short i = 0; i < data->splineList.splineCount; i++)
			{
				if (data->splineList.splines[i].splinePoints)
				{
					buf->align(3);
					buf->write(data->splineList.splines[i].splinePoints, data->splineList.splines[i].splinePointCount);
					buf->clear_pointer(&destsplines->splinePoints);
				}
			}
			buf->clear_pointer(&dest->splineList.splines);
		}

		if (data->havokEntsShapeData)
		{
			buf->align(15);
			buf->write(data->havokEntsShapeData, data->havokEntsShapeDataSize);
			buf->clear_pointer(&dest->havokEntsShapeData);
		}

		if (data->cmodels)
		{
			buf->align(7);
			auto* dest_cmodels = buf->write(data->cmodels, data->numSubModels);
			for (unsigned int i = 0; i < data->numSubModels; i++)
			{
				if (data->cmodels[i].info)
				{
					ClipInfo* destInfo;
					dest_cmodels[i].info = buf->write_s(7, data->cmodels[i].info, 1, sizeof(ClipInfo), &destInfo);
					if (reinterpret_cast<std::uint64_t>(dest_cmodels[i].info) == buf->data_following)
					{
						clip_map::write_info(zone, buf, data->cmodels[i].info, destInfo);
					}
				}

				if (data->cmodels[i].physicsAsset)
				{
					dest_cmodels[i].physicsAsset = reinterpret_cast<PhysicsAsset*>(
						zone->get_asset_pointer(ASSET_TYPE_PHYSICSASSET, data->cmodels[i].physicsAsset->name));
				}
			}
			buf->clear_pointer(&dest->cmodels);
		}

		if (data->dynEntDefList[0])
		{
			buf->align(7);
			dest->dynEntDefList[0] = buf->write(data->dynEntDefList[0], data->dynEntCount[0]);
			for (unsigned short i = 0; i < data->dynEntCount[0]; i++)
			{
				if (data->dynEntDefList[0][i].baseModel)
				{
					dest->dynEntDefList[0][i].baseModel = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->dynEntDefList[0][i].baseModel->name));
				}
				if (data->dynEntDefList[0][i].linkTo)
				{
					dest->dynEntDefList[0][i].linkTo = buf->write_s(3, data->dynEntDefList[0][i].linkTo);
				}
			}
			buf->clear_pointer(&dest->dynEntDefList[0]);
		}
		if (data->dynEntDefList[1])
		{
			buf->align(7);
			dest->dynEntDefList[1] = buf->write(data->dynEntDefList[1], data->dynEntCount[1]);
			for (unsigned short i = 0; i < data->dynEntCount[1]; i++)
			{
				if (data->dynEntDefList[1][i].baseModel)
				{
					dest->dynEntDefList[1][i].baseModel = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->dynEntDefList[1][i].baseModel->name));
				}
				if (data->dynEntDefList[1][i].linkTo)
				{
					dest->dynEntDefList[1][i].linkTo = buf->write_s(3, data->dynEntDefList[1][i].linkTo);
				}
			}
			buf->clear_pointer(&dest->dynEntDefList[1]);
		}
		if (data->dynEntPoseList[0][0])
		{
			buf->align(7);
			dest->dynEntPoseList[0][0] = buf->write(data->dynEntPoseList[0][0], data->dynEntCount[0]);
			for (unsigned short i = 0; i < data->dynEntCount[0]; i++)
			{
				if (data->dynEntPoseList[0][0][i].poses)
				{
					buf->align(3);
					buf->write(data->dynEntPoseList[0][0][i].poses, data->dynEntPoseList[0][0][i].numPoses);
					buf->clear_pointer(&dest->dynEntPoseList[0][0][i].poses);
				}
				if (data->dynEntPoseList[0][0][i].unk)
				{
					buf->align(0);
					buf->write(data->dynEntPoseList[0][0][i].unk, data->dynEntPoseList[0][0][i].numPoses);
					buf->clear_pointer(&dest->dynEntPoseList[0][0][i].unk);
				}
			}
			buf->clear_pointer(&dest->dynEntPoseList[0][0]);
		}
		if (data->dynEntPoseList[1][0])
		{
			buf->align(7);
			dest->dynEntPoseList[1][0] = buf->write(data->dynEntPoseList[1][0], data->dynEntCount[0]);
			for (unsigned short i = 0; i < data->dynEntCount[0]; i++)
			{
				if (data->dynEntPoseList[1][0][i].poses)
				{
					buf->align(3);
					buf->write(data->dynEntPoseList[1][0][i].poses, data->dynEntPoseList[1][0][i].numPoses);
					buf->clear_pointer(&dest->dynEntPoseList[1][0][i].poses);
				}
				if (data->dynEntPoseList[1][0][i].unk)
				{
					buf->align(0);
					buf->write(data->dynEntPoseList[1][0][i].unk, data->dynEntPoseList[1][0][i].numPoses);
					buf->clear_pointer(&dest->dynEntPoseList[1][0][i].unk);
				}
			}
			buf->clear_pointer(&dest->dynEntPoseList[1][0]);
		}
		if (data->dynEntPoseList[0][1])
		{
			buf->align(7);
			dest->dynEntPoseList[0][1] = buf->write(data->dynEntPoseList[0][1], data->dynEntCount[1]);
			for (unsigned short i = 0; i < data->dynEntCount[1]; i++)
			{
				if (data->dynEntPoseList[0][1][i].poses)
				{
					buf->align(3);
					buf->write(data->dynEntPoseList[0][1][i].poses, data->dynEntPoseList[0][1][i].numPoses);
					buf->clear_pointer(&dest->dynEntPoseList[0][1][i].poses);
				}
				if (data->dynEntPoseList[0][1][i].unk)
				{
					buf->align(0);
					buf->write(data->dynEntPoseList[0][1][i].unk, data->dynEntPoseList[0][1][i].numPoses);
					buf->clear_pointer(&dest->dynEntPoseList[0][1][i].unk);
				}
			}
			buf->clear_pointer(&dest->dynEntPoseList[0][1]);
		}
		if (data->dynEntPoseList[1][1])
		{
			buf->align(7);
			dest->dynEntPoseList[1][1] = buf->write(data->dynEntPoseList[1][1], data->dynEntCount[1]);
			for (unsigned short i = 0; i < data->dynEntCount[1]; i++)
			{
				if (data->dynEntPoseList[1][1][i].poses)
				{
					buf->align(3);
					buf->write(data->dynEntPoseList[1][1][i].poses, data->dynEntPoseList[1][1][i].numPoses);
					buf->clear_pointer(&dest->dynEntPoseList[1][1][i].poses);
				}
				if (data->dynEntPoseList[1][1][i].unk)
				{
					buf->align(0);
					buf->write(data->dynEntPoseList[1][1][i].unk, data->dynEntPoseList[1][1][i].numPoses);
					buf->clear_pointer(&dest->dynEntPoseList[1][1][i].unk);
				}
			}
			buf->clear_pointer(&dest->dynEntPoseList[1][1]);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->dynEntClientList[0][0])
		{
			buf->align(3);
			buf->write(data->dynEntClientList[0][0], data->dynEntCount[0]);
			buf->clear_pointer(&dest->dynEntClientList[0][0]);
		}
		if (data->dynEntClientList[1][0])
		{
			buf->align(3);
			buf->write(data->dynEntClientList[1][0], data->dynEntCount[0]);
			buf->clear_pointer(&dest->dynEntClientList[1][0]);
		}
		if (data->dynEntClientList[0][1])
		{
			buf->align(3);
			buf->write(data->dynEntClientList[0][1], data->dynEntCount[1]);
			buf->clear_pointer(&dest->dynEntClientList[0][1]);
		}
		if (data->dynEntClientList[1][1])
		{
			buf->align(3);
			buf->write(data->dynEntClientList[1][1], data->dynEntCount[1]);
			buf->clear_pointer(&dest->dynEntClientList[1][1]);
		}
		buf->pop_stream();

		if (data->dynEntGlobalIdList[0])
		{
			buf->align(3);
			buf->write(data->dynEntGlobalIdList[0], data->dynEntCountTotal);
			buf->clear_pointer(&dest->dynEntGlobalIdList[0]);
		}
		if (data->dynEntGlobalIdList[1])
		{
			buf->align(3);
			buf->write(data->dynEntGlobalIdList[1], data->dynEntCountTotal);
			buf->clear_pointer(&dest->dynEntGlobalIdList[1]);
		}

		if (data->unk2)
		{
			buf->align(7);
			dest->unk2 = buf->write(data->unk2, data->unk2Count);
			for (unsigned int i = 0; i < data->unk2Count; i++)
			{
				if (data->unk2[i].unk01)
				{
					buf->align(3);
					buf->write(data->unk2[i].unk01, data->unk2[i].unk01Count);
					buf->clear_pointer(&dest->unk2[i].unk01);
				}
			}
			buf->clear_pointer(&dest->unk2);
		}

		if (data->unk2_1[0])
		{
			buf->align(3);
			buf->write(data->unk2_1[0], data->unk2Count);
			buf->clear_pointer(&dest->unk2_1[0]);
		}
		if (data->unk2_1[1])
		{
			buf->align(3);
			buf->write(data->unk2_1[1], data->unk2Count);
			buf->clear_pointer(&dest->unk2_1[1]);
		}
		if (data->unk2_2[0])
		{
			buf->align(3);
			buf->write(data->unk2_2[0], data->unk2Count);
			buf->clear_pointer(&dest->unk2_2[0]);
		}
		if (data->unk2_2[1])
		{
			buf->align(3);
			buf->write(data->unk2_2[1], data->unk2Count);
			buf->clear_pointer(&dest->unk2_2[1]);
		}

		if (data->unk3)
		{
			buf->align(3);
			buf->write(data->unk3, data->unk3Count);
			buf->clear_pointer(&dest->unk3);
		}

		if (data->clientEntAnchors)
		{
			buf->align(3);
			buf->write(data->clientEntAnchors, data->clientEntAnchorCount);
			buf->clear_pointer(&dest->clientEntAnchors);
		}

		if (data->scriptableMapEnts.instances)
		{
			buf->align(7);
			dest->scriptableMapEnts.instances = buf->write(data->scriptableMapEnts.instances, data->scriptableMapEnts.totalInstanceCount);
			for (unsigned int i = 0; i < data->scriptableMapEnts.totalInstanceCount; i++)
			{
				if (data->scriptableMapEnts.instances[i].unk01.unk01.def)
				{
					dest->scriptableMapEnts.instances[i].unk01.unk01.def = reinterpret_cast<ScriptableDef*>(
						zone->get_asset_pointer(ASSET_TYPE_SCRIPTABLE, data->scriptableMapEnts.instances[i].unk01.unk01.def->name));
				}
				if (data->scriptableMapEnts.instances[i].unk01.unk01.unk01.model)
				{
					dest->scriptableMapEnts.instances[i].unk01.unk01.unk01.model = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->scriptableMapEnts.instances[i].unk01.unk01.unk01.model->name));
				}
				if (data->scriptableMapEnts.instances[i].unk01.unk01.eventStreamBuffer)
				{
					buf->align(0);
					buf->write(data->scriptableMapEnts.instances[i].unk01.unk01.eventStreamBuffer, data->scriptableMapEnts.instances[i].unk01.unk01.eventStreamBufferSize);
					buf->clear_pointer(&dest->scriptableMapEnts.instances[i].unk01.unk01.eventStreamBuffer);
				}
				//
				if (data->scriptableMapEnts.instances[i].unk02[0].unk01.def)
				{
					dest->scriptableMapEnts.instances[i].unk02[0].unk01.def = reinterpret_cast<ScriptableDef*>(
						zone->get_asset_pointer(ASSET_TYPE_SCRIPTABLE, data->scriptableMapEnts.instances[i].unk02[0].unk01.def->name));
				}
				if (data->scriptableMapEnts.instances[i].unk02[0].unk01.unk01.model)
				{
					dest->scriptableMapEnts.instances[i].unk02[0].unk01.unk01.model = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->scriptableMapEnts.instances[i].unk02[0].unk01.unk01.model->name));
				}
				if (data->scriptableMapEnts.instances[i].unk02[0].unk01.eventStreamBuffer)
				{
					buf->align(0);
					buf->write(data->scriptableMapEnts.instances[i].unk02[0].unk01.eventStreamBuffer, data->scriptableMapEnts.instances[i].unk02[0].unk01.eventStreamBufferSize);
					buf->clear_pointer(&dest->scriptableMapEnts.instances[i].unk02[0].unk01.eventStreamBuffer);
				}
				//
				if (data->scriptableMapEnts.instances[i].unk02[1].unk01.def)
				{
					dest->scriptableMapEnts.instances[i].unk02[1].unk01.def = reinterpret_cast<ScriptableDef*>(
						zone->get_asset_pointer(ASSET_TYPE_SCRIPTABLE, data->scriptableMapEnts.instances[i].unk02[1].unk01.def->name));
				}
				if (data->scriptableMapEnts.instances[i].unk02[1].unk01.unk01.model)
				{
					dest->scriptableMapEnts.instances[i].unk02[1].unk01.unk01.model = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->scriptableMapEnts.instances[i].unk02[1].unk01.unk01.model->name));
				}
				if (data->scriptableMapEnts.instances[i].unk02[1].unk01.eventStreamBuffer)
				{
					buf->align(0);
					buf->write(data->scriptableMapEnts.instances[i].unk02[1].unk01.eventStreamBuffer, data->scriptableMapEnts.instances[i].unk02[1].unk01.eventStreamBufferSize);
					buf->clear_pointer(&dest->scriptableMapEnts.instances[i].unk02[1].unk01.eventStreamBuffer);
				}
			}

			if (data->scriptableMapEnts.unk.unk01)
			{
				buf->align(3);
				buf->write(data->scriptableMapEnts.unk.unk01, data->scriptableMapEnts.unk.unk01Count);
				buf->clear_pointer(&dest->scriptableMapEnts.unk.unk01);
			}
			if (data->scriptableMapEnts.unk.unk02_1)
			{
				buf->align(3);
				buf->write(data->scriptableMapEnts.unk.unk02_1, data->scriptableMapEnts.unk.unk02Count);
				buf->clear_pointer(&dest->scriptableMapEnts.unk.unk02_1);
			}
			if (data->scriptableMapEnts.unk.unk02_2)
			{
				buf->align(3);
				buf->write(data->scriptableMapEnts.unk.unk02_2, data->scriptableMapEnts.unk.unk02Count);
				buf->clear_pointer(&dest->scriptableMapEnts.unk.unk02_2);
			}

			if (data->scriptableMapEnts.reservedDynents[0].reservedDynents)
			{
				buf->align(3);
				buf->write(data->scriptableMapEnts.reservedDynents[0].reservedDynents, data->scriptableMapEnts.reservedDynents[0].numReservedDynents);
				buf->clear_pointer(&dest->scriptableMapEnts.reservedDynents[0].reservedDynents);
			}
			if (data->scriptableMapEnts.reservedDynents[1].reservedDynents)
			{
				buf->align(3);
				buf->write(data->scriptableMapEnts.reservedDynents[1].reservedDynents, data->scriptableMapEnts.reservedDynents[1].numReservedDynents);
				buf->clear_pointer(&dest->scriptableMapEnts.reservedDynents[1].reservedDynents);
			}

			buf->clear_pointer(&dest->scriptableMapEnts.instances);
		}

		if (data->mayhemScenes)
		{
			MayhemInstance* destMayhems;
			dest->mayhemScenes = buf->write_s(7, data->mayhemScenes, data->numMayhemScenes, sizeof(MayhemInstance), &destMayhems);
			if ((std::uint64_t)dest->mayhemScenes == buf->data_following)
			{
				for (unsigned int i = 0; i < data->numMayhemScenes; i++)
				{
					if (data->mayhemScenes[i].mayhem)
					{
						destMayhems[i].mayhem = reinterpret_cast<MayhemData*>(
							zone->get_asset_pointer(ASSET_TYPE_MAYHEM, data->mayhemScenes[i].mayhem->name));
					}

					if (data->mayhemScenes[i].linkTo)
					{
						destMayhems[i].linkTo = buf->write_s(3, data->mayhemScenes[i].linkTo);
					}
				}
			}
		}

		if (data->spawners.spawnerList)
		{
			buf->align(7);
			dest->spawners.spawnerList = buf->write(data->spawners.spawnerList, data->spawners.spawnerCount);
			for (unsigned int i = 0; i < data->spawners.spawnerCount; i++)
			{
				if (data->spawners.spawnerList[i].fields)
				{
					buf->align(3);
					buf->write(data->spawners.spawnerList[i].fields, data->spawners.spawnerList[i].numFields);
					buf->clear_pointer(&dest->spawners.spawnerList[i].fields);
				}
			}
			buf->clear_pointer(&dest->spawners.spawnerList);
		}

		if (data->audioPASpeakers)
		{
			buf->align(3);
			buf->write(data->audioPASpeakers, data->audioPASpeakerCount);
			buf->clear_pointer(&dest->audioPASpeakers);
		}

		buf->pop_stream();
	}

	void map_ents::dump_spawn_list(const std::string& name, SpawnPointRecordList* spawnList)
	{
		const auto path = name + ".ents.spawnList"s;
		auto file = filesystem::file(path);
		file.open("wb");

		if (!file.get_fp())
		{
			return;
		}

		ordered_json data{};

		for (unsigned short i = 0; i < spawnList->spawnsCount; i++)
		{
			data[i]["name"] = SL_ConvertToString(spawnList->spawns[i].name);
			data[i]["target"] = SL_ConvertToString(spawnList->spawns[i].target);
			data[i]["script_noteworthy"] = SL_ConvertToString(spawnList->spawns[i].script_noteworthy);
			for (auto j = 0; j < 3; j++)
			{
				data[i]["origin"][j] = spawnList->spawns[i].origin[j];
				data[i]["angles"][j] = spawnList->spawns[i].angles[j];
			}
		}

		const auto json = data.dump(4);
		file.write(json.data(), json.size(), 1);

		file.close();
	}

	void map_ents::dump_spawners(const std::string& name, SpawnerList* spawners)
	{
		const auto path = name + ".ents.spawners"s;
		auto file = filesystem::file(path);
		file.open("wb");

		if (!file.get_fp())
		{
			return;
		}

		ordered_json data{};

		for (unsigned int i = 0; i < spawners->spawnerCount; i++)
		{
			data[i]["targetname"] = SL_ConvertToString(spawners->spawnerList[i].targetname);
			data[i]["classname"] = SL_ConvertToString(spawners->spawnerList[i].classname);
			data[i]["count"] = spawners->spawnerList[i].count;

			for (auto j = 0; j < 3; j++)
			{
				data[i]["origin"][j] = spawners->spawnerList[i].origin[j];
				data[i]["angles"][j] = spawners->spawnerList[i].angles[j];
			}

			data[i]["fields"] = {};
			for (unsigned int j = 0; j < spawners->spawnerList[i].numFields; j++)
			{
				data[i]["fields"][j]["key"] = SL_ConvertToString(spawners->spawnerList[i].fields[j].key);
				data[i]["fields"][j]["value"] = SL_ConvertToString(spawners->spawnerList[i].fields[j].value);
				data[i]["fields"][j]["type"] = spawners->spawnerList[i].fields[j].type;
			}
		}

		const auto json = data.dump(4);
		file.write(json.data(), json.size(), 1);

		file.close();
	}

	void map_ents::dump_entity_strings(const std::string& name, char* entityString, int numEntityChars)
	{
		const auto path = name + ".ents"s;
		auto file = filesystem::file(path);
		file.open("wb");

		if (!file.get_fp())
		{
			return;
		}

		file.write(entityString, numEntityChars, 1);
		file.close();
	}

	void map_ents::dump(MapEnts* asset)
	{
		assetmanager::dumper dumper;
		const auto path = asset->name + ".ents.data"s;
		if (!dumper.open(path))
		{
			return;
		}

		dumper.dump_single(asset);

		dump_entity_strings(asset->name, asset->entityString, asset->numEntityChars);
		dump_spawn_list(asset->name, &asset->spawnList);
		dump_spawners(asset->name, &asset->spawners);

		dumper.dump_array<TriggerModel>(asset->trigger.models, asset->trigger.count);
		for (unsigned int i = 0; i < asset->trigger.count; i++)
		{
			dumper.dump_asset(asset->trigger.models[i].physicsAsset);
		}
		dumper.dump_array<TriggerHull>(asset->trigger.hulls, asset->trigger.hullCount);
		dumper.dump_array<TriggerSlab>(asset->trigger.slabs, asset->trigger.slabCount);
		dumper.dump_array<TriggerWinding>(asset->trigger.windings, asset->trigger.windingCount);
		dumper.dump_array<TriggerWindingPoint>(asset->trigger.windingPoints, asset->trigger.windingPointCount);

		dumper.dump_array<TriggerModel>(asset->clientTrigger.trigger.models, asset->clientTrigger.trigger.count);
		for (unsigned int i = 0; i < asset->clientTrigger.trigger.count; i++)
		{
			dumper.dump_asset(asset->clientTrigger.trigger.models[i].physicsAsset);
		}
		dumper.dump_array<TriggerHull>(asset->clientTrigger.trigger.hulls, asset->clientTrigger.trigger.hullCount);
		dumper.dump_array<TriggerSlab>(asset->clientTrigger.trigger.slabs, asset->clientTrigger.trigger.slabCount);
		dumper.dump_array<TriggerWinding>(asset->clientTrigger.trigger.windings, asset->clientTrigger.trigger.windingCount);
		dumper.dump_array<TriggerWindingPoint>(asset->clientTrigger.trigger.windingPoints, asset->clientTrigger.trigger.windingPointCount);
		dumper.dump_array(asset->clientTrigger.triggerString, asset->clientTrigger.triggerStringLength);
		dumper.dump_array(asset->clientTrigger.visionSetTriggers, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.triggerType, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.origins, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.scriptDelay, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.audioTriggers, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.blendLookup, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.npcTriggers, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.audioStateIds, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.audioRvbPanInfo, asset->clientTrigger.trigger.count);
		dumper.dump_array(asset->clientTrigger.transientIndex, asset->clientTrigger.trigger.count);
		if (asset->clientTrigger.linkTo)
		{
			dumper.dump_array(asset->clientTrigger.linkTo, asset->clientTrigger.trigger.count);
			for (unsigned int i = 0; i < asset->clientTrigger.trigger.count; i++)
			{
				dumper.dump_single(asset->clientTrigger.linkTo[i]);
			}
		}
		dumper.dump_array(asset->clientTriggerBlend.blendNodes, asset->clientTriggerBlend.numClientTriggerBlendNodes);

		dumper.dump_array(asset->splineList.splines, asset->splineList.splineCount);
		for (unsigned short i = 0; i < asset->splineList.splineCount; i++)
		{
			dumper.dump_array(asset->splineList.splines[i].splinePoints, asset->splineList.splines[i].splinePointCount);
			for (unsigned short j = 0; j < asset->splineList.splines[i].splinePointCount; j++)
			{
				dumper.dump_string(SL_ConvertToString(asset->splineList.splines[i].splinePoints[j].splineNodeLabel));
				dumper.dump_string(SL_ConvertToString(asset->splineList.splines[i].splinePoints[j].targetname));
				dumper.dump_string(SL_ConvertToString(asset->splineList.splines[i].splinePoints[j].target));
				dumper.dump_string(SL_ConvertToString(asset->splineList.splines[i].splinePoints[j].string));
			}
		}

		dumper.dump_array(asset->havokEntsShapeData, asset->havokEntsShapeDataSize);

		dumper.dump_array(asset->cmodels, asset->numSubModels);
		for (unsigned int i = 0; i < asset->numSubModels; i++)
		{
			dumper.dump_single(asset->cmodels[i].info);
			if (asset->cmodels[i].info)
			{
				clip_map::dump_info(asset->cmodels[i].info, dumper);
			}
			dumper.dump_asset(asset->cmodels[i].physicsAsset);
		}

		dumper.dump_array(asset->dynEntDefList[0], asset->dynEntCount[0]);
		for (unsigned short i = 0; i < asset->dynEntCount[0]; i++)
		{
			dumper.dump_asset(asset->dynEntDefList[0][i].baseModel);
			dumper.dump_single(asset->dynEntDefList[0][i].linkTo);
		}

		dumper.dump_array(asset->dynEntDefList[1], asset->dynEntCount[1]);
		for (unsigned short i = 0; i < asset->dynEntCount[1]; i++)
		{
			dumper.dump_asset(asset->dynEntDefList[1][i].baseModel);
			dumper.dump_single(asset->dynEntDefList[1][i].linkTo);
		}

		dumper.dump_array(asset->dynEntPoseList[0][0], asset->dynEntCount[0]);
		for (unsigned short i = 0; i < asset->dynEntCount[0]; i++)
		{
			dumper.dump_array(asset->dynEntPoseList[0][0][i].poses, asset->dynEntPoseList[0][0][i].numPoses);
			dumper.dump_array(asset->dynEntPoseList[0][0][i].unk, asset->dynEntPoseList[0][0][i].numPoses);
		}

		dumper.dump_array(asset->dynEntPoseList[1][0], asset->dynEntCount[0]);
		for (unsigned short i = 0; i < asset->dynEntCount[0]; i++)
		{
			dumper.dump_array(asset->dynEntPoseList[1][0][i].poses, asset->dynEntPoseList[1][0][i].numPoses);
			dumper.dump_array(asset->dynEntPoseList[1][0][i].unk, asset->dynEntPoseList[1][0][i].numPoses);
		}

		dumper.dump_array(asset->dynEntPoseList[0][1], asset->dynEntCount[1]);
		for (unsigned short i = 0; i < asset->dynEntCount[1]; i++)
		{
			dumper.dump_array(asset->dynEntPoseList[0][1][i].poses, asset->dynEntPoseList[0][1][i].numPoses);
			dumper.dump_array(asset->dynEntPoseList[0][1][i].unk, asset->dynEntPoseList[0][1][i].numPoses);
		}

		dumper.dump_array(asset->dynEntPoseList[1][1], asset->dynEntCount[1]);
		for (unsigned short i = 0; i < asset->dynEntCount[1]; i++)
		{
			dumper.dump_array(asset->dynEntPoseList[1][1][i].poses, asset->dynEntPoseList[1][1][i].numPoses);
			dumper.dump_array(asset->dynEntPoseList[1][1][i].unk, asset->dynEntPoseList[1][1][i].numPoses);
		}

		dumper.dump_array(asset->dynEntGlobalIdList[0], asset->dynEntCountTotal);
		dumper.dump_array(asset->dynEntGlobalIdList[1], asset->dynEntCountTotal);

		dumper.dump_array(asset->unk2, asset->unk2Count);
		dumper.dump_array(asset->unk2_1[0], asset->unk2Count);
		dumper.dump_array(asset->unk2_1[1], asset->unk2Count);
		dumper.dump_array(asset->unk2_2[0], asset->unk2Count);
		dumper.dump_array(asset->unk2_2[1], asset->unk2Count);

		dumper.dump_array(asset->unk3, asset->unk3Count);

		dumper.dump_array(asset->clientEntAnchors, asset->clientEntAnchorCount);
		for (unsigned int i = 0; i < asset->clientEntAnchorCount; i++)
		{
			dumper.dump_string(SL_ConvertToString(asset->clientEntAnchors[i].name));
		}

		dumper.dump_array(asset->scriptableMapEnts.instances, asset->scriptableMapEnts.totalInstanceCount);
		for (unsigned int i = 0; i < asset->scriptableMapEnts.totalInstanceCount; i++)
		{
			dumper.dump_asset(asset->scriptableMapEnts.instances[i].unk01.unk01.def);
			dumper.dump_asset(asset->scriptableMapEnts.instances[i].unk01.unk01.unk01.model);
			dumper.dump_array(asset->scriptableMapEnts.instances[i].unk01.unk01.eventStreamBuffer, asset->scriptableMapEnts.instances[i].unk01.unk01.eventStreamBufferSize);

			dumper.dump_asset(asset->scriptableMapEnts.instances[i].unk02[0].unk01.def);
			dumper.dump_asset(asset->scriptableMapEnts.instances[i].unk02[0].unk01.unk01.model);
			dumper.dump_array(asset->scriptableMapEnts.instances[i].unk02[0].unk01.eventStreamBuffer, asset->scriptableMapEnts.instances[i].unk02[0].unk01.eventStreamBufferSize);

			dumper.dump_asset(asset->scriptableMapEnts.instances[i].unk02[1].unk01.def);
			dumper.dump_asset(asset->scriptableMapEnts.instances[i].unk02[1].unk01.unk01.model);
			dumper.dump_array(asset->scriptableMapEnts.instances[i].unk02[1].unk01.eventStreamBuffer, asset->scriptableMapEnts.instances[i].unk02[1].unk01.eventStreamBufferSize);

			dumper.dump_string(SL_ConvertToString(asset->scriptableMapEnts.instances[i].unk03));
			dumper.dump_string(asset->scriptableMapEnts.instances[i].unk04);
			dumper.dump_string(SL_ConvertToString(asset->scriptableMapEnts.instances[i].targetname));
		}

		dumper.dump_array(asset->scriptableMapEnts.unk.unk01, asset->scriptableMapEnts.unk.unk01Count);
		dumper.dump_array(asset->scriptableMapEnts.unk.unk02_1, asset->scriptableMapEnts.unk.unk02Count);
		dumper.dump_array(asset->scriptableMapEnts.unk.unk02_2, asset->scriptableMapEnts.unk.unk02Count);

		dumper.dump_array(asset->scriptableMapEnts.reservedDynents[0].reservedDynents, asset->scriptableMapEnts.reservedDynents[0].numReservedDynents);
		dumper.dump_array(asset->scriptableMapEnts.reservedDynents[1].reservedDynents, asset->scriptableMapEnts.reservedDynents[1].numReservedDynents);

		dumper.dump_array(asset->mayhemScenes, asset->numMayhemScenes);
		for (unsigned int i = 0; i < asset->numMayhemScenes; i++)
		{
			dumper.dump_asset(asset->mayhemScenes[i].mayhem);
			dumper.dump_single(asset->mayhemScenes[i].linkTo);
			dumper.dump_string(SL_ConvertToString(asset->mayhemScenes[i].scriptName));
		}

		dumper.dump_array(asset->audioPASpeakers, asset->audioPASpeakerCount);

		dumper.close();
	}
}