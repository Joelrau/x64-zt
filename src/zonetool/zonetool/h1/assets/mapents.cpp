#include <std_include.hpp>
#include "mapents.hpp"

namespace zonetool::h1
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

	std::string extra_entity_strings;

	void map_ents::add_entity_string(const std::string& line)
	{
		extra_entity_strings.append(line);
	}

	void map_ents::clear_entity_strings()
	{
		extra_entity_strings.clear();
	}

	void map_ents::parse_spline_list(zone_memory* mem, std::string name, SplineRecordList* splineList)
	{
		assetmanager::reader reader(mem);
		const auto path = name + ".ents.splineList"s;
		if (reader.open(path))
		{
			splineList->splineCount = reader.read_short();
			splineList->splines = reader.read_array<SplinePointRecordList>();
			for (unsigned short i = 0; i < splineList->splineCount; i++)
			{
				splineList->splines[i].splinePoints = reader.read_array<SplinePointEntityRecord>();
				for (unsigned short j = 0; j < splineList->splines[i].splinePointCount; j++)
				{
					splineList->splines[i].splinePoints[j].splineNodeLabel = reader.read_array<char>();
					splineList->splines[i].splinePoints[j].positionCubic = reader.read_array<vec3_t>();
					splineList->splines[i].splinePoints[j].tangentQuadratic = reader.read_array<vec3_t>();
				}
			}

			reader.close();
		}
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
			add_script_string(&spawnList->spawns[i].unknown, mem->duplicate_string(data[i]["unknown"].get<std::string>()));
			for (auto j = 0; j < 3; j++)
			{
				spawnList->spawns[i].origin[j] = data[i]["origin"][j].get<float>();
				spawnList->spawns[i].angles[j] = data[i]["angles"][j].get<float>();
			}
		}
	}

	void map_ents::parse_client_blend_triggers(zone_memory* mem, std::string name, ClientTriggerBlend* clientTriggerBlend)
	{
		assetmanager::reader reader(mem);
		const auto path = name + ".ents.clientBlendTriggers"s;
		if (reader.open(path))
		{
			clientTriggerBlend->numClientTriggerBlendNodes = reader.read_short();
			clientTriggerBlend->blendNodes = reader.read_array<ClientTriggerBlendNode>();

			reader.close();
		}
	}

	void map_ents::parse_client_triggers(zone_memory* mem, std::string name, ClientTriggers* clientTrigger)
	{
		assetmanager::reader reader(mem);
		const auto path = name + ".ents.clientTriggers";
		if (reader.open(path))
		{
			clientTrigger->trigger.count = reader.read_int();
			clientTrigger->trigger.models = reader.read_array<TriggerModel>();

			clientTrigger->trigger.hullCount = reader.read_int();
			clientTrigger->trigger.hulls = reader.read_array<TriggerHull>();

			clientTrigger->trigger.slabCount = reader.read_int();
			clientTrigger->trigger.slabs = reader.read_array<TriggerSlab>();

			clientTrigger->clientTriggerAabbTree = reader.read_array<ClientTriggerAabbNode>();

			clientTrigger->triggerStringLength = reader.read_int();
			clientTrigger->triggerString = reader.read_array<char>();

			clientTrigger->visionSetTriggers = reader.read_array<short>();
			clientTrigger->blendLookup = reader.read_array<short>();
			clientTrigger->unk1 = reader.read_array<short>();
			clientTrigger->triggerType = reader.read_array<short>();
			clientTrigger->origins = reader.read_array<vec3_t>();
			clientTrigger->scriptDelay = reader.read_array<float>();
			clientTrigger->audioTriggers = reader.read_array<short>();
			clientTrigger->unk2 = reader.read_array<short>();
			clientTrigger->unk3 = reader.read_array<short>();
			clientTrigger->unk4 = reader.read_array<short>();
			clientTrigger->unk5 = reader.read_array<short>();
			clientTrigger->unk6 = reader.read_array<short>();

			reader.close();
		}
	}

	void map_ents::parse_triggers(zone_memory* mem, std::string name, MapTriggers* trigger)
	{
		assetmanager::reader reader(mem);
		const auto path = name + ".ents.triggers"s;
		if (reader.open(path))
		{
			trigger->count = reader.read_int();
			trigger->models = reader.read_array<TriggerModel>();

			trigger->hullCount = reader.read_int();
			trigger->hulls = reader.read_array<TriggerHull>();

			trigger->slabCount = reader.read_int();
			trigger->slabs = reader.read_array<TriggerSlab>();

			reader.close();
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

		std::string entity_string = *entityStrings;
		if (!entity_string.ends_with("\n"))
		{
			entity_string.append("\n");
		}

		entity_string.append(extra_entity_strings);
		*entityStrings = mem->duplicate_string(entity_string);
		*numEntityChars = static_cast<int>(entity_string.size());

		file.close();
	}

	MapEnts* map_ents::parse(std::string name, zone_memory* mem)
	{
		const auto path = name + ".ents"s;
		if (!filesystem::file(path).exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing mapents \"%s\"...", name.data());

		MapEnts* ents = mem->allocate<MapEnts>();
		ents->name = mem->duplicate_string(name);

		parse_entity_strings(mem, name, &ents->entityString, &ents->numEntityChars);
		parse_triggers(mem, name, &ents->trigger);
		parse_client_triggers(mem, name, &ents->clientTrigger);
		parse_client_blend_triggers(mem, name, &ents->clientTriggerBlend);
		parse_spawn_list(mem, name, &ents->spawnList);
		parse_spline_list(mem, name, &ents->splineList);

		return ents;
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

		if (data->spawnList.spawns)
		{
			for (int i = 0; i < data->spawnList.spawnsCount; i++)
			{
				data->spawnList.spawns[i].name = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->spawnList.spawns[i].name)));

				data->spawnList.spawns[i].target = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->spawnList.spawns[i].target)));

				data->spawnList.spawns[i].script_noteworthy = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->spawnList.spawns[i].script_noteworthy)));

				data->spawnList.spawns[i].unknown = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->spawnList.spawns[i].unknown)));
			}
		}
	}

	void map_ents::load_depending(zone_base* zone)
	{
	}

	std::string map_ents::name()
	{
		return this->name_;
	}

	std::int32_t map_ents::type()
	{
		return ASSET_TYPE_MAP_ENTS;
	}

	void map_ents::write_triggers(zone_buffer* buf, MapTriggers* dest)
	{
		if (dest->models)
		{
			dest->models = buf->write_s(3, dest->models, dest->count);
		}

		if (dest->hulls)
		{
			dest->hulls = buf->write_s(3, dest->hulls, dest->hullCount);
		}

		if (dest->slabs)
		{
			dest->slabs = buf->write_s(3, dest->slabs, dest->slabCount);
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
			zone_buffer::clear_pointer(&dest->entityString);
		}

		write_triggers(buf, &dest->trigger);
		write_triggers(buf, &dest->clientTrigger.trigger);

		if (data->clientTrigger.clientTriggerAabbTree)
		{
			buf->align(3);
			buf->write(data->clientTrigger.clientTriggerAabbTree, data->clientTrigger.numClientTriggerNodes);
			zone_buffer::clear_pointer(&dest->clientTrigger.clientTriggerAabbTree);
		}
		if (data->clientTrigger.triggerString)
		{
			buf->align(0);
			buf->write(data->clientTrigger.triggerString, data->clientTrigger.triggerStringLength);
			zone_buffer::clear_pointer(&dest->clientTrigger.triggerString);
		}
		if (data->clientTrigger.visionSetTriggers)
		{
			buf->align(1);
			buf->write(data->clientTrigger.visionSetTriggers, data->clientTrigger.trigger.count);
			zone_buffer::clear_pointer(&dest->clientTrigger.visionSetTriggers);
		}
		if (data->clientTrigger.blendLookup)
		{
			buf->align(1);
			buf->write(data->clientTrigger.blendLookup, data->clientTrigger.trigger.count);
			zone_buffer::clear_pointer(&dest->clientTrigger.blendLookup);
		}
		if (data->clientTrigger.unk1)
		{
			buf->align(1);
			buf->write(data->clientTrigger.unk1, data->clientTrigger.trigger.count);
			zone_buffer::clear_pointer(&dest->clientTrigger.unk1);
		}
		if (data->clientTrigger.triggerType)
		{
			buf->align(1);
			buf->write(data->clientTrigger.triggerType, data->clientTrigger.trigger.count);
			zone_buffer::clear_pointer(&dest->clientTrigger.triggerType);
		}
		if (data->clientTrigger.origins)
		{
			buf->align(3);
			buf->write(data->clientTrigger.origins, data->clientTrigger.trigger.count);
			zone_buffer::clear_pointer(&dest->clientTrigger.origins);
		}
		if (data->clientTrigger.scriptDelay)
		{
			buf->align(3);
			buf->write(data->clientTrigger.scriptDelay, data->clientTrigger.trigger.count);
			zone_buffer::clear_pointer(&dest->clientTrigger.scriptDelay);
		}
		if (data->clientTrigger.audioTriggers)
		{
			buf->align(1);
			buf->write(data->clientTrigger.audioTriggers, data->clientTrigger.trigger.count);
			zone_buffer::clear_pointer(&dest->clientTrigger.audioTriggers);
		}
		if (data->clientTrigger.unk2)
		{
			buf->align(1);
			buf->write(data->clientTrigger.unk2, data->clientTrigger.trigger.count);
			zone_buffer::clear_pointer(&dest->clientTrigger.unk2);
		}
		if (data->clientTrigger.unk3)
		{
			buf->align(1);
			buf->write(data->clientTrigger.unk3, data->clientTrigger.trigger.count);
			zone_buffer::clear_pointer(&dest->clientTrigger.unk3);
		}
		if (data->clientTrigger.unk4)
		{
			buf->align(1);
			buf->write(data->clientTrigger.unk4, data->clientTrigger.trigger.count);
			zone_buffer::clear_pointer(&dest->clientTrigger.unk4);
		}
		if (data->clientTrigger.unk5)
		{
			buf->align(1);
			buf->write(data->clientTrigger.unk5, data->clientTrigger.trigger.count);
			zone_buffer::clear_pointer(&dest->clientTrigger.unk5);
		}
		if (data->clientTrigger.unk6)
		{
			buf->align(1);
			buf->write(data->clientTrigger.unk6, data->clientTrigger.trigger.count);
			zone_buffer::clear_pointer(&dest->clientTrigger.unk6);
		}

		if (data->clientTriggerBlend.blendNodes)
		{
			buf->align(3);
			buf->write(data->clientTriggerBlend.blendNodes, data->clientTriggerBlend.numClientTriggerBlendNodes);
			zone_buffer::clear_pointer(&dest->clientTriggerBlend.blendNodes);
		}

		if (data->spawnList.spawns)
		{
			buf->align(3);
			buf->write(data->spawnList.spawns, data->spawnList.spawnsCount);
			zone_buffer::clear_pointer(&dest->spawnList.spawns);
		}

		if (data->splineList.splines)
		{
			buf->align(3);
			auto destsplines = buf->write(data->splineList.splines, data->splineList.splineCount);
			for (unsigned short i = 0; i < data->splineList.splineCount; i++)
			{
				if (data->splineList.splines[i].splinePoints)
				{
					buf->align(3);
					auto destsplinepoints = buf->write(data->splineList.splines[i].splinePoints,
						data->splineList.splines[i].splinePointCount);
					for (unsigned short j = 0; j < data->splineList.splines[i].splinePointCount; j++)
					{
						if (data->splineList.splines[i].splinePoints[j].splineNodeLabel)
						{
							buf->align(0);
							buf->write(data->splineList.splines[i].splinePoints[j].splineNodeLabel, 64);
							zone_buffer::clear_pointer(destsplinepoints[j].splineNodeLabel);
						}
						if (data->splineList.splines[i].splinePoints[j].positionCubic)
						{
							buf->align(3);
							buf->write(data->splineList.splines[i].splinePoints[j].positionCubic, 4);
							zone_buffer::clear_pointer(&destsplinepoints[j].positionCubic);
						}
						if (data->splineList.splines[i].splinePoints[j].tangentQuadratic)
						{
							buf->align(3);
							buf->write(data->splineList.splines[i].splinePoints[j].tangentQuadratic, 3);
							zone_buffer::clear_pointer(&destsplinepoints[j].tangentQuadratic);
						}
					}
					zone_buffer::clear_pointer(&destsplines->splinePoints);
				}
			}
			zone_buffer::clear_pointer(&dest->splineList.splines);
		}

		buf->pop_stream();
	}

	void map_ents::dump_spline_list(const std::string& name, SplineRecordList* splineList)
	{
		assetmanager::dumper dumper;
		const auto path = name + ".ents.splineList"s;
		if (dumper.open(path))
		{
			dumper.dump_short(splineList->splineCount);
			dumper.dump_array(splineList->splines, splineList->splineCount);
			for (unsigned short i = 0; i < splineList->splineCount; i++)
			{
				dumper.dump_array(splineList->splines[i].splinePoints, splineList->splines[i].splinePointCount);
				for (unsigned short j = 0; j < splineList->splines[i].splinePointCount; j++)
				{
					dumper.dump_array(splineList->splines[i].splinePoints[j].splineNodeLabel, 64);
					dumper.dump_array(splineList->splines[i].splinePoints[j].positionCubic, 4);
					dumper.dump_array(splineList->splines[i].splinePoints[j].tangentQuadratic, 3);
				}
			}
		}
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

		ordered_json data;

		for (unsigned short i = 0; i < spawnList->spawnsCount; i++)
		{
			data[i]["name"] = spawnList->spawns[i].name ? SL_ConvertToString(spawnList->spawns[i].name) : "";
			data[i]["target"] = spawnList->spawns[i].target ? SL_ConvertToString(spawnList->spawns[i].target) : "";
			data[i]["script_noteworthy"] = spawnList->spawns[i].script_noteworthy ? SL_ConvertToString(spawnList->spawns[i].script_noteworthy) : "";
			data[i]["unknown"] = spawnList->spawns[i].unknown ? SL_ConvertToString(spawnList->spawns[i].unknown) : "";
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

	void map_ents::dump_client_blend_triggers(const std::string& name, ClientTriggerBlend* clientTriggerBlend)
	{
		assetmanager::dumper dumper;
		const auto path = name + ".ents.clientBlendTriggers"s;
		if (dumper.open(path))
		{
			dumper.dump_short(clientTriggerBlend->numClientTriggerBlendNodes);
			dumper.dump_array(clientTriggerBlend->blendNodes, clientTriggerBlend->numClientTriggerBlendNodes);
		}
	}

	void map_ents::dump_client_triggers(const std::string& name, ClientTriggers* clientTrigger)
	{
		assetmanager::dumper dumper;
		const auto path = name + ".ents.clientTriggers";
		if (dumper.open(path))
		{
			dumper.dump_int(clientTrigger->trigger.count);
			dumper.dump_array<TriggerModel>(clientTrigger->trigger.models, clientTrigger->trigger.count);

			dumper.dump_int(clientTrigger->trigger.hullCount);
			dumper.dump_array<TriggerHull>(clientTrigger->trigger.hulls, clientTrigger->trigger.hullCount);

			dumper.dump_int(clientTrigger->trigger.slabCount);
			dumper.dump_array<TriggerSlab>(clientTrigger->trigger.slabs, clientTrigger->trigger.slabCount);

			dumper.dump_array(clientTrigger->clientTriggerAabbTree, clientTrigger->numClientTriggerNodes);

			dumper.dump_int(clientTrigger->triggerStringLength);
			dumper.dump_array(clientTrigger->triggerString, clientTrigger->triggerStringLength);

			dumper.dump_array(clientTrigger->visionSetTriggers, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->blendLookup, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->unk1, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->triggerType, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->origins, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->scriptDelay, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->audioTriggers, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->unk2, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->unk3, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->unk4, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->unk5, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->unk6, clientTrigger->trigger.count);

			dumper.close();
		}
	}

	void map_ents::dump_triggers(const std::string& name, MapTriggers* trigger)
	{
		assetmanager::dumper dumper;
		const auto path = name + ".ents.triggers"s;
		if (dumper.open(path))
		{
			dumper.dump_int(trigger->count);
			dumper.dump_array<TriggerModel>(trigger->models, trigger->count);

			dumper.dump_int(trigger->hullCount);
			dumper.dump_array<TriggerHull>(trigger->hulls, trigger->hullCount);

			dumper.dump_int(trigger->slabCount);
			dumper.dump_array<TriggerSlab>(trigger->slabs, trigger->slabCount);

			dumper.close();
		}
	}

	void map_ents::dump_entity_strings(const std::string& name, char* entityString, int numEntityChars)
	{
		const auto path = name + ".ents"s;
		auto file = filesystem::file(path);
		file.open("wb");
		if (file.get_fp())
		{
			file.write(entityString, numEntityChars, 1);
			file.close();
		}
	}

	void map_ents::dump(MapEnts* asset)
	{
		dump_entity_strings(asset->name, asset->entityString, asset->numEntityChars);
		dump_triggers(asset->name, &asset->trigger);
		dump_client_triggers(asset->name, &asset->clientTrigger);
		dump_client_blend_triggers(asset->name, &asset->clientTriggerBlend);
		dump_spawn_list(asset->name, &asset->spawnList);
		dump_spline_list(asset->name, &asset->splineList);
	}
}