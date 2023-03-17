#include <std_include.hpp>
#include "mapents.hpp"

namespace zonetool::iw6
{
	void IMapEnts::add_script_string(scr_string_t* ptr, const char* str)
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

	const char* IMapEnts::get_script_string(scr_string_t* ptr)
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

	void IMapEnts::parse_splineList(ZoneMemory* mem, std::string name, SplineRecordList* splineList)
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

	void IMapEnts::parse_spawnList(ZoneMemory* mem, std::string name, SpawnPointRecordList* spawnList)
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
		spawnList->spawns = mem->Alloc<SpawnPointEntityRecord>(spawnList->spawnsCount);

		for (unsigned short i = 0; i < spawnList->spawnsCount; i++)
		{
			spawnList->spawns[i].index = i;
			add_script_string(&spawnList->spawns[i].name, mem->StrDup(data[i]["name"].get<std::string>()));
			add_script_string(&spawnList->spawns[i].target, mem->StrDup(data[i]["target"].get<std::string>()));
			add_script_string(&spawnList->spawns[i].script_noteworthy, mem->StrDup(data[i]["script_noteworthy"].get<std::string>()));
			for (auto j = 0; j < 3; j++)
			{
				spawnList->spawns[i].origin[j] = data[i]["origin"][j].get<float>();
				spawnList->spawns[i].angles[j] = data[i]["angles"][j].get<float>();
			}
		}
	}

	void IMapEnts::parse_clientBlendTriggers(ZoneMemory* mem, std::string name, ClientTriggerBlend* clientTriggerBlend)
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

	void IMapEnts::parse_clientTriggers(ZoneMemory* mem, std::string name, ClientTriggers* clientTrigger)
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
			clientTrigger->triggerType = reader.read_array<char>();
			clientTrigger->origins = reader.read_array<vec3_t>();
			clientTrigger->scriptDelay = reader.read_array<float>();
			clientTrigger->audioTriggers = reader.read_array<short>();
			clientTrigger->blendLookup = reader.read_array<short>();
			clientTrigger->npcTriggers = reader.read_array<short>();

			reader.close();
		}
	}

	void IMapEnts::parse_triggers(ZoneMemory* mem, std::string name, MapTriggers* trigger)
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

	void IMapEnts::parse_entityStrings(ZoneMemory* mem, std::string name, char** entityStrings, int* numEntityChars)
	{
		const auto path = name + ".ents"s;
		auto file = filesystem::file(path);
		file.open("rb");
		if (!file.get_fp())
		{
			return;
		}

		*numEntityChars = static_cast<int>(file.size());

		*entityStrings = mem->Alloc<char>(static_cast<size_t>(*numEntityChars + 1));
		memset(*entityStrings, 0, *numEntityChars);

		fread(*entityStrings, *numEntityChars, 1, file.get_fp());
		(*entityStrings)[*numEntityChars] = '\0';

		file.close();
	}

	MapEnts* IMapEnts::parse(std::string name, ZoneMemory* mem)
	{
		const auto path = name + ".ents"s;
		if (!filesystem::file(path).exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing mapents \"%s\"...", name.data());

		MapEnts* ents = mem->Alloc<MapEnts>();
		ents->name = mem->StrDup(name);

		parse_entityStrings(mem, name, &ents->entityString, &ents->numEntityChars);
		parse_triggers(mem, name, &ents->trigger);
		parse_clientTriggers(mem, name, &ents->clientTrigger);
		parse_clientBlendTriggers(mem, name, &ents->clientTriggerBlend);
		parse_spawnList(mem, name, &ents->spawnList);
		parse_splineList(mem, name, &ents->splineList);

		return ents;
	}

	void IMapEnts::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = "maps/"s + (filesystem::get_fastfile().substr(0, 3) == "mp_" ? "mp/" : "") + filesystem::get_fastfile() + ".d3dbsp"; // name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->StrDup(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not find mapents: \"%s\"", this->name_.data());
		}
	}

	void IMapEnts::prepare(ZoneBuffer* buf, ZoneMemory* mem)
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
			}
		}
	}

	void IMapEnts::load_depending(IZone* zone)
	{
	}

	std::string IMapEnts::name()
	{
		return this->name_;
	}

	std::int32_t IMapEnts::type()
	{
		return ASSET_TYPE_MAP_ENTS;
	}

	void IMapEnts::write_triggers(ZoneBuffer* buf, MapTriggers* dest)
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

	void IMapEnts::write(IZone* zone, ZoneBuffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->entityString)
		{
			buf->align(0);
			buf->write(data->entityString, data->numEntityChars);
			ZoneBuffer::clear_pointer(&dest->entityString);
		}

		write_triggers(buf, &dest->trigger);
		write_triggers(buf, &dest->clientTrigger.trigger);

		if (data->clientTrigger.clientTriggerAabbTree)
		{
			buf->align(3);
			buf->write(data->clientTrigger.clientTriggerAabbTree, data->clientTrigger.numClientTriggerNodes);
			ZoneBuffer::clear_pointer(&dest->clientTrigger.clientTriggerAabbTree);
		}
		if (data->clientTrigger.triggerString)
		{
			buf->align(0);
			buf->write(data->clientTrigger.triggerString, data->clientTrigger.triggerStringLength);
			ZoneBuffer::clear_pointer(&dest->clientTrigger.triggerString);
		}
		if (data->clientTrigger.visionSetTriggers)
		{
			buf->align(1);
			buf->write(data->clientTrigger.visionSetTriggers, data->clientTrigger.trigger.count);
			ZoneBuffer::clear_pointer(&dest->clientTrigger.visionSetTriggers);
		}
		if (data->clientTrigger.triggerType)
		{
			buf->align(0);
			buf->write(data->clientTrigger.triggerType, data->clientTrigger.trigger.count);
			ZoneBuffer::clear_pointer(&dest->clientTrigger.triggerType);
		}
		if (data->clientTrigger.origins)
		{
			buf->align(3);
			buf->write(data->clientTrigger.origins, data->clientTrigger.trigger.count);
			ZoneBuffer::clear_pointer(&dest->clientTrigger.origins);
		}
		if (data->clientTrigger.scriptDelay)
		{
			buf->align(3);
			buf->write(data->clientTrigger.scriptDelay, data->clientTrigger.trigger.count);
			ZoneBuffer::clear_pointer(&dest->clientTrigger.scriptDelay);
		}
		if (data->clientTrigger.audioTriggers)
		{
			buf->align(1);
			buf->write(data->clientTrigger.audioTriggers, data->clientTrigger.trigger.count);
			ZoneBuffer::clear_pointer(&dest->clientTrigger.audioTriggers);
		}
		if (data->clientTrigger.blendLookup)
		{
			buf->align(1);
			buf->write(data->clientTrigger.blendLookup, data->clientTrigger.trigger.count);
			ZoneBuffer::clear_pointer(&dest->clientTrigger.blendLookup);
		}
		if (data->clientTrigger.npcTriggers)
		{
			buf->align(1);
			buf->write(data->clientTrigger.npcTriggers, data->clientTrigger.trigger.count);
			ZoneBuffer::clear_pointer(&dest->clientTrigger.npcTriggers);
		}

		if (data->clientTriggerBlend.blendNodes)
		{
			buf->align(3);
			buf->write(data->clientTriggerBlend.blendNodes, data->clientTriggerBlend.numClientTriggerBlendNodes);
			ZoneBuffer::clear_pointer(&dest->clientTriggerBlend.blendNodes);
		}

		if (data->spawnList.spawns)
		{
			buf->align(3);
			buf->write(data->spawnList.spawns, data->spawnList.spawnsCount);
			ZoneBuffer::clear_pointer(&dest->spawnList.spawns);
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
							ZoneBuffer::clear_pointer(destsplinepoints[j].splineNodeLabel);
						}
						if (data->splineList.splines[i].splinePoints[j].positionCubic)
						{
							buf->align(3);
							buf->write(data->splineList.splines[i].splinePoints[j].positionCubic, 4);
							ZoneBuffer::clear_pointer(&destsplinepoints[j].positionCubic);
						}
						if (data->splineList.splines[i].splinePoints[j].tangentQuadratic)
						{
							buf->align(3);
							buf->write(data->splineList.splines[i].splinePoints[j].tangentQuadratic, 3);
							ZoneBuffer::clear_pointer(&destsplinepoints[j].tangentQuadratic);
						}
					}
					ZoneBuffer::clear_pointer(&destsplines->splinePoints);
				}
			}
			ZoneBuffer::clear_pointer(&dest->splineList.splines);
		}

		buf->pop_stream();
	}

	void IMapEnts::dump_splineList(const std::string& name, SplineRecordList* splineList)
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

	void IMapEnts::dump_spawnList(const std::string& name, SpawnPointRecordList* spawnList)
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

	void IMapEnts::dump_clientBlendTriggers(const std::string& name, ClientTriggerBlend* clientTriggerBlend)
	{
		assetmanager::dumper dumper;
		const auto path = name + ".ents.clientBlendTriggers"s;
		if (dumper.open(path))
		{
			dumper.dump_short(clientTriggerBlend->numClientTriggerBlendNodes);
			dumper.dump_array(clientTriggerBlend->blendNodes, clientTriggerBlend->numClientTriggerBlendNodes);
		}
	}

	void IMapEnts::dump_clientTriggers(const std::string& name, ClientTriggers* clientTrigger)
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
			dumper.dump_array(clientTrigger->triggerType, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->origins, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->scriptDelay, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->audioTriggers, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->blendLookup, clientTrigger->trigger.count);
			dumper.dump_array(clientTrigger->npcTriggers, clientTrigger->trigger.count);

			dumper.close();
		}
	}

	void IMapEnts::dump_triggers(const std::string& name, MapTriggers* trigger)
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

	void IMapEnts::dump_entityStrings(const std::string& name, char* entityString, int numEntityChars)
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

	void IMapEnts::dump(MapEnts* asset)
	{
		dump_entityStrings(asset->name, asset->entityString, asset->numEntityChars);
		dump_triggers(asset->name, &asset->trigger);
		dump_clientTriggers(asset->name, &asset->clientTrigger);
		dump_clientBlendTriggers(asset->name, &asset->clientTriggerBlend);
		dump_spawnList(asset->name, &asset->spawnList);
		dump_splineList(asset->name, &asset->splineList);
	}
}