#include "std_include.hpp"
#include "animclass.hpp"

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

	AnimationClass* anim_class::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);

		const auto path = "animclass\\"s + name;
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing animclass \"%s\"...", name.data());

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
							this->add_script_string(&asset->stateMachine->states[i].animEntries[entry_index].alias, read.read_string());
							this->add_script_string(&asset->stateMachine->states[i].animEntries[entry_index].animName, read.read_string());
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
			ZONETOOL_FATAL("animclass %s is missing...", name.data());
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

	void anim_class::dump(AnimationClass* asset)
	{
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
	}
}