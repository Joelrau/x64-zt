#include "std_include.hpp"
#include "animclass.hpp"

#define DUMP_JSON

namespace zonetool::h1
{
	void anim_class::add_script_string(scr_string_t* ptr, const char* str)
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

	const char* anim_class::get_script_string(scr_string_t* ptr)
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

#define PARSE_VAR_SCRIPT_STR(var, __field__, __json_field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), scr_string_t>, "Field is not scr_string_t"); \
	if (!var[__json_field__].is_null() && !var[__json_field__].empty()) \
	{ \
		const auto str = mem->duplicate_string(var[__json_field__].get<std::string>()); \
		this->add_script_string(&asset->__field__, str); \
	} \
	else \
	{ \
		asset->__field__ = 0; /* null scr_string */ \
	}

	AnimationClass* anim_class::parse_json(const std::string& name, zone_memory* mem)
	{
		const auto path = "animclass\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing animclass json \"%s\"...", name.c_str());

		// parse json file
		file.open("rb");
		ordered_json data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto* asset = mem->allocate<AnimationClass>();
		asset->name = mem->duplicate_string(name);

		if (!data["stateMachine"].is_null())
		{
			auto& j_sm = data["stateMachine"];
			asset->stateMachine = mem->allocate<AnimationStateMachine>();

			auto asset_name = mem->duplicate_string(j_sm["name"].get<std::string>());
			this->add_script_string(&asset->stateMachine->name, asset_name);

			if (!j_sm["states"].is_null())
			{
				asset->stateMachine->stateCount = static_cast<std::uint16_t>(j_sm["states"].size());
				asset->stateMachine->states = mem->allocate<AnimationState>(asset->stateMachine->stateCount);

				for (int i = 0; i < asset->stateMachine->stateCount; i++)
				{
					auto& j_state = j_sm["states"][i];
					auto& state = asset->stateMachine->states[i];

					PARSE_VAR_SCRIPT_STR(j_state, stateMachine->states[i].name, "name");
					PARSE_VAR_SCRIPT_STR(j_state, stateMachine->states[i].notify, "notify");

					if (!j_state["aimSet"].is_null())
					{
						auto& j_aim = j_state["aimSet"];
						state.aimSet = mem->allocate<AnimationAimSet>();

						PARSE_VAR_SCRIPT_STR(j_aim, stateMachine->states[i].aimSet->name, "name");
						PARSE_VAR_SCRIPT_STR(j_aim, stateMachine->states[i].aimSet->rootName, "rootName");

						for (int a = 0; a < 8; a++)
						{
							// i think
							if (!j_aim["animName"][a].is_null() && !j_aim["animName"][a].empty())
							{
								const auto str = mem->duplicate_string(j_aim["animName"][a].get<std::string>());
								this->add_script_string(&state.aimSet->animName[a], str);
							}
						}
					}

					if (!j_state["animEntries"].is_null())
					{
						state.entryCount = static_cast<std::uint16_t>(j_state["animEntries"].size());
						state.animEntries = mem->allocate<AnimationEntry>(state.entryCount);

						for (int e = 0; e < state.entryCount; e++)
						{
							auto& j_entry = j_state["animEntries"][e];

							PARSE_VAR_SCRIPT_STR(j_entry, stateMachine->states[i].animEntries[e].alias, "alias");
							PARSE_VAR_SCRIPT_STR(j_entry, stateMachine->states[i].animEntries[e].animName, "animName");
						}
					}

					if (!j_state["animIndices"].is_null())
					{
						state.animIndices = mem->allocate<unsigned __int64>(state.entryCount);
						for (int e = 0; e < state.entryCount; e++)
						{
							state.animIndices[e] = j_state["animIndices"][e].get<unsigned __int64>();
						}
					}
				}
			}

			if (!j_sm["aimSets"].is_null())
			{
				asset->stateMachine->aimSetCount = static_cast<std::uint16_t>(j_sm["aimSets"].size());
				asset->stateMachine->aimSets = mem->allocate<AnimationAimSet>(asset->stateMachine->aimSetCount);

				for (int i = 0; i < asset->stateMachine->aimSetCount; i++)
				{
					auto& j_aim = j_sm["aimSets"][i];

					PARSE_VAR_SCRIPT_STR(j_aim, stateMachine->aimSets[i].name, "name");
					PARSE_VAR_SCRIPT_STR(j_aim, stateMachine->aimSets[i].rootName, "rootName");

					for (int a = 0; a < 8; a++)
					{
						if (!j_aim["animName"][a].is_null() && !j_aim["animName"][a].empty())
						{
							const auto str = mem->duplicate_string(j_aim["animName"][a].get<std::string>());
							this->add_script_string(&asset->stateMachine->aimSets[i].animName[a], str);
						}
					}
				}
			}
		}

		PARSE_VAR_SCRIPT_STR(data, animTree, "animTree");

		if (!data["scriptable"].is_null() && !data["scriptable"].get<std::string>().empty())
		{
			asset->scriptable->name = mem->duplicate_string(data["scriptable"].get<std::string>());
		}

		const auto soundCount = data["soundNotes"].size();
		asset->soundCount = static_cast<std::uint16_t>(soundCount);

		if (soundCount > 0)
		{
			asset->soundNotes = mem->allocate<scr_string_t>(soundCount);
			asset->soundNames = mem->allocate<scr_string_t>(soundCount);
			asset->soundOptions = mem->allocate<scr_string_t>(soundCount);

			for (int i = 0; i < soundCount; i++)
			{
				if (!data["soundNotes"][i].is_null() && !data["soundNotes"][i].empty())
				{
					const auto str = mem->duplicate_string(data["soundNotes"][i].get<std::string>());
					this->add_script_string(&asset->soundNotes[i], str);
				}

				if (!data["soundNames"][i].is_null() && !data["soundNames"][i].empty())
				{
					const auto str = mem->duplicate_string(data["soundNames"][i].get<std::string>());
					this->add_script_string(&asset->soundNames[i], str);
				}

				if (!data["soundOptions"][i].is_null() && !data["soundOptions"][i].empty())
				{
					const auto str = mem->duplicate_string(data["soundOptions"][i].get<std::string>());
					this->add_script_string(&asset->soundOptions[i], str);
				}
			}
		}

		const auto effectCount = data["effectNotes"].size();
		asset->effectCount = static_cast<std::uint16_t>(effectCount);

		if (effectCount > 0)
		{
			asset->effectNotes = mem->allocate<scr_string_t>(effectCount);
			asset->effectDefs = mem->allocate<FxEffectDef*>(effectCount);
			asset->effectTags = mem->allocate<scr_string_t>(effectCount);

			for (int i = 0; i < effectCount; i++)
			{
				if (!data["effectNotes"][i].is_null() && !data["effectNotes"][i].empty())
				{
					const auto str = mem->duplicate_string(data["effectNotes"][i].get<std::string>());
					this->add_script_string(&asset->effectNotes[i], str);
				}

				if (!data["effectTags"][i].is_null() && !data["effectTags"][i].empty())
				{
					const auto str = mem->duplicate_string(data["effectTags"][i].get<std::string>());
					this->add_script_string(&asset->effectTags[i], str);
				}

				const auto def_name = data["effectDefs"][i].get<std::string>();
				if (!def_name.empty())
				{
					asset->effectDefs[i]->name = mem->duplicate_string(def_name);
				}
			}
		}

		return asset;
	}

	AnimationClass* anim_class::parse(const std::string& name, zone_memory* mem)
	{
		if (auto* asset = parse_json(name, mem))
		{
			return asset;
		}

		assetmanager::reader read(mem);

		const auto path = "animclass\\"s + name;
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing animclass \"%s\"...", name.c_str());

		const auto asset = read.read_single<AnimationClass>();
		asset->name = read.read_string();

		if (asset->stateMachine)
		{
			asset->stateMachine = read.read_single<AnimationStateMachine>();
			this->add_script_string(&asset->stateMachine->name, read.read_string());
			if (asset->stateMachine->states)
			{
				asset->stateMachine->states = read.read_array<AnimationState>();
				for (auto i = 0; i < asset->stateMachine->stateCount; i++)
				{
					this->add_script_string(&asset->stateMachine->states[i].name, read.read_string());
					this->add_script_string(&asset->stateMachine->states[i].notify, read.read_string());
					if (asset->stateMachine->states[i].aimSet)
					{
						asset->stateMachine->states[i].aimSet = read.read_single<AnimationAimSet>();
						this->add_script_string(&asset->stateMachine->states[i].aimSet->name, read.read_string());
						this->add_script_string(&asset->stateMachine->states[i].aimSet->rootName, read.read_string());
						for (auto anim_index = 0; anim_index < 8; anim_index++)
						{
							this->add_script_string(&asset->stateMachine->states[i].aimSet->animName[anim_index], read.read_string());
						}
					}
					if (asset->stateMachine->states[i].animEntries)
					{
						asset->stateMachine->states[i].animEntries = read.read_array<AnimationEntry>();
						for (auto entry_index = 0; entry_index < asset->stateMachine->states[i].entryCount; entry_index++)
						{
							auto alias = read.read_string();
							this->add_script_string(&asset->stateMachine->states[i].animEntries[entry_index].alias, alias);
							auto anim_name = read.read_string();
							this->add_script_string(&asset->stateMachine->states[i].animEntries[entry_index].animName, anim_name);

							printf("adding alias %s for anim name %s\n", alias, anim_name);
						}
					}
					if (asset->stateMachine->states[i].animIndices)
					{
						asset->stateMachine->states[i].animIndices = read.read_array<unsigned __int64>();
					}
				}
			}
			if (asset->stateMachine->aimSets)
			{
				asset->stateMachine->aimSets = read.read_array<AnimationAimSet>();
				for (auto i = 0; i < asset->stateMachine->aimSetCount; i++)
				{
					this->add_script_string(&asset->stateMachine->aimSets[i].name, read.read_string());
					this->add_script_string(&asset->stateMachine->aimSets[i].rootName, read.read_string());
					for (auto anim_index = 0; anim_index < 8; anim_index++)
					{
						this->add_script_string(&asset->stateMachine->aimSets[i].animName[anim_index], read.read_string());
					}
				}
			}
		}

		this->add_script_string(&asset->animTree, read.read_string());
		asset->scriptable = read.read_asset<ScriptableDef>();

		if (asset->soundNotes)
		{
			asset->soundNotes = read.read_array<scr_string_t>();
			for (auto i = 0; i < asset->soundCount; i++)
			{
				this->add_script_string(&asset->soundNotes[i], read.read_string());
			}
		}
		if (asset->soundNames)
		{
			asset->soundNames = read.read_array<scr_string_t>();
			for (auto i = 0; i < asset->soundCount; i++)
			{
				this->add_script_string(&asset->soundNames[i], read.read_string());
			}
		}
		if (asset->soundOptions)
		{
			asset->soundOptions = read.read_array<scr_string_t>();
			for (auto i = 0; i < asset->soundCount; i++)
			{
				this->add_script_string(&asset->soundOptions[i], read.read_string());
			}
		}

		if (asset->effectNotes)
		{
			asset->effectNotes = read.read_array<scr_string_t>();
			for (auto i = 0; i < asset->effectCount; i++)
			{
				this->add_script_string(&asset->effectNotes[i], read.read_string());
			}
		}
		if (asset->effectDefs)
		{
			asset->effectDefs = read.read_array<FxEffectDef*>();
			for (auto i = 0; i < asset->effectCount; i++)
			{
				asset->effectDefs[i] = read.read_asset<FxEffectDef>();
			}
		}
		if (asset->effectTags)
		{
			asset->effectTags = read.read_array<scr_string_t>();
			for (auto i = 0; i < asset->effectCount; i++)
			{
				this->add_script_string(&asset->effectTags[i], read.read_string());
			}
		}

		return asset;
	}

	void anim_class::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			ZONETOOL_FATAL("animclass %s is missing...", name.c_str());
		}
	}

	void anim_class::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* asset = this->asset_;

		if (asset->stateMachine)
		{
			asset->stateMachine->name = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&asset->stateMachine->name)));
			if (asset->stateMachine->states)
			{
				for (auto i = 0; i < asset->stateMachine->stateCount; i++)
				{
					asset->stateMachine->states[i].name = static_cast<scr_string_t>(buf->write_scriptstring(
						this->get_script_string(&asset->stateMachine->states[i].name)));
					asset->stateMachine->states[i].notify = static_cast<scr_string_t>(buf->write_scriptstring(
						this->get_script_string(&asset->stateMachine->states[i].notify)));
					if (asset->stateMachine->states[i].aimSet)
					{
						asset->stateMachine->states[i].aimSet->name = static_cast<scr_string_t>(buf->write_scriptstring(
							this->get_script_string(&asset->stateMachine->states[i].aimSet->name)));
						asset->stateMachine->states[i].aimSet->rootName = static_cast<scr_string_t>(buf->write_scriptstring(
							this->get_script_string(&asset->stateMachine->states[i].aimSet->rootName)));
						for (auto anim_index = 0; anim_index < 8; anim_index++)
						{
							asset->stateMachine->states[i].aimSet->animName[anim_index] = static_cast<scr_string_t>(buf->write_scriptstring(
								this->get_script_string(&asset->stateMachine->states[i].aimSet->animName[anim_index])));
						}
					}
					if (asset->stateMachine->states[i].animEntries)
					{
						for (auto entry_index = 0; entry_index < asset->stateMachine->states[i].entryCount; entry_index++)
						{
							asset->stateMachine->states[i].animEntries[entry_index].alias = static_cast<scr_string_t>(buf->write_scriptstring(
								this->get_script_string(&asset->stateMachine->states[i].animEntries[entry_index].alias)));
							asset->stateMachine->states[i].animEntries[entry_index].animName = static_cast<scr_string_t>(buf->write_scriptstring(
								this->get_script_string(&asset->stateMachine->states[i].animEntries[entry_index].animName)));
						}
					}
				}
			}
			if (asset->stateMachine->aimSets)
			{
				for (auto i = 0; i < asset->stateMachine->aimSetCount; i++)
				{
					asset->stateMachine->aimSets[i].name = static_cast<scr_string_t>(buf->write_scriptstring(
						this->get_script_string(&asset->stateMachine->aimSets[i].name)));
					asset->stateMachine->aimSets[i].rootName = static_cast<scr_string_t>(buf->write_scriptstring(
						this->get_script_string(&asset->stateMachine->aimSets[i].rootName)));
					for (auto anim_index = 0; anim_index < 8; anim_index++)
					{
						asset->stateMachine->aimSets[i].animName[anim_index] = static_cast<scr_string_t>(buf->write_scriptstring(
							this->get_script_string(&asset->stateMachine->aimSets[i].animName[anim_index])));
					}
				}
			}
		}

		asset->animTree = static_cast<scr_string_t>(buf->write_scriptstring(
			this->get_script_string(&asset->animTree)));

		if (asset->soundNotes)
		{
			for (auto i = 0; i < asset->soundCount; i++)
			{
				asset->soundNotes[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&asset->soundNotes[i])));
			}
		}
		if (asset->soundNames)
		{
			for (auto i = 0; i < asset->soundCount; i++)
			{
				asset->soundNames[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&asset->soundNames[i])));
			}
		}
		if (asset->soundOptions)
		{
			for (auto i = 0; i < asset->soundCount; i++)
			{
				asset->soundOptions[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&asset->soundOptions[i])));
			}
		}
		if(asset->effectNotes)
		{
			for (auto i = 0; i < asset->effectCount; i++)
			{
				asset->effectNotes[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&asset->effectNotes[i])));
			}
		}
		if (asset->effectTags)
		{
			for (auto i = 0; i < asset->effectCount; i++)
			{
				asset->effectTags[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&asset->effectTags[i])));
			}
		}
	}

	void anim_class::load_depending(zone_base* zone)
	{
		auto* asset = this->asset_;

		if (asset->scriptable)
		{
			zone->add_asset_of_type(ASSET_TYPE_SCRIPTABLE, asset->scriptable->name);
		}
		
		for (auto i = 0; i < asset->effectCount; i++)
		{
			if (asset->effectDefs[i])
			{
				zone->add_asset_of_type(ASSET_TYPE_FX, asset->effectDefs[i]->name);
			}
		}
	}

	std::string anim_class::name()
	{
		return this->name_;
	}

	std::int32_t anim_class::type()
	{
		return ASSET_TYPE_ANIMCLASS;
	}

	void anim_class::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		
		dest->name = buf->write_str(this->name());

		if (data->stateMachine)
		{
			buf->align(3);

			auto* dest_state_machine = buf->write(data->stateMachine);
			auto* data_state_machine = data->stateMachine;
			if (data_state_machine->states)
			{
				buf->align(3);

				auto* dest_states = buf->write(data_state_machine->states, data_state_machine->stateCount);
				auto* data_states = data_state_machine->states;
				for (auto i = 0; i < data_state_machine->stateCount; i++)
				{
					if (data_states[i].aimSet)
					{
						buf->align(3);
						buf->write(data_states[i].aimSet);
						buf->clear_pointer(&dest_states[i].aimSet);
					}

					if (data_states[i].animEntries)
					{
						buf->align(3);
						buf->write(data_states[i].animEntries, data_states[i].entryCount);
						buf->clear_pointer(&dest_states[i].animEntries);
					}

					buf->push_stream(XFILE_BLOCK_RUNTIME);
					if (data_states[i].animIndices)
					{
						buf->align(3);
						buf->write(data_states[i].animIndices, data_states[i].entryCount);
						buf->clear_pointer(&dest_states[i].animIndices);
					}
					buf->pop_stream();
				}

				buf->clear_pointer(&dest_state_machine->states);
			}

			if (data_state_machine->aimSets)
			{
				buf->align(3);
				buf->write(data_state_machine->aimSets, data_state_machine->aimSetCount);
				buf->clear_pointer(&dest_state_machine->aimSets);
			}

			buf->clear_pointer(&dest->stateMachine);
		}

		if (data->scriptable)
		{
			dest->scriptable = reinterpret_cast<ScriptableDef*>(zone->get_asset_pointer(
				ASSET_TYPE_SCRIPTABLE, data->scriptable->name));
		}

		if (data->soundNotes)
		{
			buf->align(3);
			buf->write(data->soundNotes, data->soundCount);
			buf->clear_pointer(&dest->soundNotes);
		}

		if (data->soundNames)
		{
			buf->align(3);
			buf->write(data->soundNames, data->soundCount);
			buf->clear_pointer(&dest->soundNames);
		}

		if (data->soundOptions)
		{
			buf->align(3);
			buf->write(data->soundOptions, data->soundCount);
			buf->clear_pointer(&dest->soundOptions);
		}

		if (data->effectNotes)
		{
			buf->align(3);
			buf->write(data->effectNotes, data->effectCount);
			buf->clear_pointer(&dest->effectNotes);
		}

		if (data->effectDefs)
		{
			buf->align(7);
			auto* dest_defs = buf->write(data->effectDefs, data->effectCount);
			for (auto i = 0; i < data->effectCount; i++)
			{
				dest_defs[i] = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(
					ASSET_TYPE_FX, data->effectDefs[i]->name));
			}
			buf->clear_pointer(&dest->effectDefs);
		}

		if (data->effectTags)
		{
			buf->align(3);
			buf->write(data->effectTags, data->effectCount);
			buf->clear_pointer(&dest->effectTags);
		}

		buf->pop_stream();
	}

	void dump_json(AnimationClass* asset)
	{
		const auto path = "animclass\\"s + asset->name + ".json";
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		data["name"] = asset->name;

		if (asset->stateMachine)
		{
			auto& j_sm = data["stateMachine"];
			j_sm["name"] = SL_ConvertToString(asset->stateMachine->name);

			if (asset->stateMachine->states)
			{
				for (int i = 0; i < asset->stateMachine->stateCount; i++)
				{
					auto& j_state = j_sm["states"][i];
					const auto& state = asset->stateMachine->states[i];

					j_state["name"] = SL_ConvertToString(state.name);
					j_state["notify"] = SL_ConvertToString(state.notify);

					if (state.aimSet)
					{
						auto& j_aim = j_state["aimSet"];
						j_aim["name"] = SL_ConvertToString(state.aimSet->name);
						j_aim["rootName"] = SL_ConvertToString(state.aimSet->rootName);
						for (int a = 0; a < 8; a++)
						{
							j_aim["animName"][a] = SL_ConvertToString(state.aimSet->animName[a]);
						}
					}

					if (state.animEntries)
					{
						for (int e = 0; e < state.entryCount; e++)
						{
							auto& j_entry = j_state["animEntries"][e];
							j_entry["alias"] = state.animEntries[e].alias != 0
								? SL_ConvertToString(state.animEntries[e].alias)
								: "";

							j_entry["animName"] = state.animEntries[e].animName != 0
								? SL_ConvertToString(state.animEntries[e].animName)
								: "";
						}
					}

					if (state.animIndices)
					{
						for (int e = 0; e < state.entryCount; e++)
						{
							j_state["animIndices"][e] = state.animIndices[e];
						}
					}
				}
			}

			if (asset->stateMachine->aimSets)
			{
				for (int i = 0; i < asset->stateMachine->aimSetCount; i++)
				{
					auto& j_aim = j_sm["aimSets"][i];
					j_aim["name"] = SL_ConvertToString(asset->stateMachine->aimSets[i].name);
					j_aim["rootName"] = SL_ConvertToString(asset->stateMachine->aimSets[i].rootName);
					for (int a = 0; a < 8; a++)
					{
						j_aim["animName"][a] = SL_ConvertToString(asset->stateMachine->aimSets[i].animName[a]);
					}
				}
			}
		}

		data["animTree"] = SL_ConvertToString(asset->animTree);

		data["scriptable"] = asset->scriptable ? asset->scriptable->name : "";

		for (int i = 0; i < asset->soundCount; i++)
		{
			data["soundNotes"][i] = SL_ConvertToString(asset->soundNotes[i]);
			data["soundNames"][i] = SL_ConvertToString(asset->soundNames[i]);

			auto options = asset->soundOptions[i];
			data["soundOptions"][i] = options != 0
				? SL_ConvertToString(options)
				: "";
		}

		for (int i = 0; i < asset->effectCount; i++)
		{
			data["effectNotes"][i] = SL_ConvertToString(asset->effectNotes[i]);
			data["effectDefs"][i] = asset->effectDefs[i] ? asset->effectDefs[i]->name : "";
			data["effectTags"][i] = SL_ConvertToString(asset->effectTags[i]);
		}

		std::string str = data.dump(4);
		file.write(str);
		file.close();
	}

	void anim_class::dump(AnimationClass* asset)
	{
#ifdef DUMP_JSON
		dump_json(asset);
		return;
#else
		const auto path = "animclass\\"s + asset->name;

		assetmanager::dumper dumper;
		if (!dumper.open(path))
		{
			return;
		}

		dumper.dump_single(asset);
		dumper.dump_string(asset->name);
		
		if (asset->stateMachine)
		{
			dumper.dump_single(asset->stateMachine);
			dumper.dump_string(SL_ConvertToString(asset->stateMachine->name));
			if (asset->stateMachine->states)
			{
				dumper.dump_array(asset->stateMachine->states, asset->stateMachine->stateCount);
				for (auto i = 0; i < asset->stateMachine->stateCount; i++)
				{
					dumper.dump_string(SL_ConvertToString(asset->stateMachine->states[i].name));
					dumper.dump_string(SL_ConvertToString(asset->stateMachine->states[i].notify));
					if (asset->stateMachine->states[i].aimSet)
					{
						dumper.dump_single(asset->stateMachine->states[i].aimSet);
						dumper.dump_string(SL_ConvertToString(asset->stateMachine->states[i].aimSet->name));
						dumper.dump_string(SL_ConvertToString(asset->stateMachine->states[i].aimSet->rootName));
						for (auto anim_index = 0; anim_index < 8; anim_index++)
						{
							dumper.dump_string(SL_ConvertToString(asset->stateMachine->states[i].aimSet->animName[anim_index]));
						}
					}
					if (asset->stateMachine->states[i].animEntries)
					{
						dumper.dump_array(asset->stateMachine->states[i].animEntries, asset->stateMachine->states[i].entryCount);
						for (auto entry_index = 0; entry_index < asset->stateMachine->states[i].entryCount; entry_index++)
						{
							dumper.dump_string(SL_ConvertToString(asset->stateMachine->states[i].animEntries[entry_index].alias));
							dumper.dump_string(SL_ConvertToString(asset->stateMachine->states[i].animEntries[entry_index].animName));
						}
					}
					if (asset->stateMachine->states[i].animIndices)
					{
						dumper.dump_array(asset->stateMachine->states[i].animIndices, asset->stateMachine->states[i].entryCount);
					}
				}
			}
			if (asset->stateMachine->aimSets)
			{
				dumper.dump_array(asset->stateMachine->aimSets, asset->stateMachine->aimSetCount);
				for (auto i = 0; i < asset->stateMachine->aimSetCount; i++)
				{
					dumper.dump_string(SL_ConvertToString(asset->stateMachine->aimSets[i].name));
					dumper.dump_string(SL_ConvertToString(asset->stateMachine->aimSets[i].rootName));
					for (auto anim_index = 0; anim_index < 8; anim_index++)
					{
						dumper.dump_string(SL_ConvertToString(asset->stateMachine->aimSets[i].animName[anim_index]));
					}
				}
			}
		}

		dumper.dump_string(SL_ConvertToString(asset->animTree));
		dumper.dump_asset(asset->scriptable);
		
		if (asset->soundNotes)
		{
			dumper.dump_array(asset->soundNotes, asset->soundCount);
			for (auto i = 0; i < asset->soundCount; i++)
			{
				dumper.dump_string(SL_ConvertToString(asset->soundNotes[i]));
			}
		}
		if (asset->soundNames)
		{
			dumper.dump_array(asset->soundNames, asset->soundCount);
			for (auto i = 0; i < asset->soundCount; i++)
			{
				dumper.dump_string(SL_ConvertToString(asset->soundNames[i]));
			}
		}
		if (asset->soundOptions)
		{
			dumper.dump_array(asset->soundOptions, asset->soundCount);
			for (auto i = 0; i < asset->soundCount; i++)
			{
				dumper.dump_string(SL_ConvertToString(asset->soundOptions[i]));
			}
		}

		if (asset->effectNotes)
		{
			dumper.dump_array(asset->effectNotes, asset->effectCount);
			for (auto i = 0; i < asset->effectCount; i++)
			{
				dumper.dump_string(SL_ConvertToString(asset->effectNotes[i]));
			}
		}
		if (asset->effectDefs)
		{
			dumper.dump_array(asset->effectDefs, asset->effectCount);
			for (auto i = 0; i < asset->effectCount; i++)
			{
				dumper.dump_asset(asset->effectDefs[i]);
			}
		}
		if (asset->effectTags)
		{
			dumper.dump_array(asset->effectTags, asset->effectCount);
			for (auto i = 0; i < asset->effectCount; i++)
			{
				dumper.dump_string(SL_ConvertToString(asset->effectTags[i]));
			}
		}

		dumper.close();
#endif
	}
}