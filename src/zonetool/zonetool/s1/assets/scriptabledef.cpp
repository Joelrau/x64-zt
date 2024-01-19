#include <std_include.hpp>
#include "scriptabledef.hpp"

namespace zonetool::s1
{
	void scriptable_def::add_script_string(scr_string_t* ptr, const char* str)
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

	const char* scriptable_def::get_script_string(scr_string_t* ptr)
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

	void scriptable_def::parse_scriptable_event_def(ScriptableEventDef* event, assetmanager::reader& read, zone_memory* mem)
	{
		switch (event->type)
		{
		case SCRIPTABLE_EVENT_MODEL:
			event->data.setModel.model = read.read_asset<XModel>();
			break;
		case SCRIPTABLE_EVENT_FX:
			event->data.playFx.handle = read.read_asset<FxEffectDef>();
			this->add_script_string(&event->data.playFx.tagName, read.read_string());
			break;
		case SCRIPTABLE_EVENT_STOP_FX:
			event->data.stopFx.handle = read.read_asset<FxEffectDef>();
			this->add_script_string(&event->data.stopFx.tagName, read.read_string());
			break;
		case SCRIPTABLE_EVENT_SOUND:
			event->data.playSound.alias = read.read_asset<snd_alias_list_t>();
			break;
		case SCRIPTABLE_EVENT_ANIMATION:
			event->data.playAnim.animName = read.read_string();
			break;
		case SCRIPTABLE_EVENT_EXPLODE:
			break;
		case SCRIPTABLE_EVENT_HEALTHDRAIN:
			break;
		case SCRIPTABLE_EVENT_PHYSICSLAUNCH:
			event->data.physicsLaunch.model = read.read_asset<XModel>();
			break;
		case SCRIPTABLE_EVENT_LIGHTSETTINGS:
			event->data.lightSettings.noteworthy = read.read_string();
			break;
		case SCRIPTABLE_EVENT_SUNLIGHTSETTINGS:
			break;
		case SCRIPTABLE_EVENT_SHAKE:
			event->data.shake.rumbleName = read.read_string();
			break;
		case SCRIPTABLE_EVENT_TRANSLATE:
			event->data.translate.str = read.read_string();
			break;
		case SCRIPTABLE_EVENT_ROTATE:
			event->data.rotate.str = read.read_string();
			break;
		case SCRIPTABLE_EVENT_STATECHANGE:
			break;
		default:
			break;
		}
	}

	ScriptableDef* scriptable_def::parse(std::string name, zone_memory* mem)
	{
		const auto path = "scriptable\\"s + name;

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ScriptableDef* asset = read.read_single<ScriptableDef>();
		asset->name = read.read_string();

		asset->baseModel = read.read_asset<XModel>();
		asset->baseCollisionBrush = read.read_string();
		asset->destroyedCollisionBrush = read.read_string();

		asset->parts = read.read_array<ScriptablePartDef>();
		for (unsigned char i = 0; i < asset->partCount; i++)
		{
			asset->parts[i].states = read.read_array<ScriptableStateDef>();
			for (unsigned char j = 0; j < asset->parts[i].stateCount; j++)
			{
				this->add_script_string(&asset->parts[i].states[j].name, read.read_string());
				this->add_script_string(&asset->parts[i].states[j].tagName, read.read_string());
				asset->parts[i].states[j].onEnterEvents = read.read_array<ScriptableEventDef>();
				for (unsigned char k = 0; k < asset->parts[i].states[j].onEnterEventCount; k++)
				{
					parse_scriptable_event_def(&asset->parts[i].states[j].onEnterEvents[k], read, mem);
				}
			}
			this->add_script_string(&asset->parts[i].name, read.read_string());
		}

		asset->notetracks = read.read_array<ScriptableNotetrackDef>();
		for (unsigned char i = 0; i < asset->notetrackCount; i++)
		{
			this->add_script_string(&asset->notetracks[i].name, read.read_string());
			if (asset->notetracks[i].type == SCRIPTABLE_NT_FX)
			{
				asset->notetracks[i].data.playFx.handle = read.read_asset<FxEffectDef>();
				this->add_script_string(&asset->notetracks[i].data.playFx.tagName, read.read_string());
			}
			else if (asset->notetracks[i].type == SCRIPTABLE_NT_SOUND)
			{
				asset->notetracks[i].data.playSound.alias = read.read_asset<snd_alias_list_t>();
			}
		}

		read.close();

		return asset;
	}

	void scriptable_def::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not parse scriptable \"%s\"", name.data());
		}
	}

	void scriptable_def::prepare_scriptable_event_def(ScriptableEventDef* event, zone_buffer* buf, zone_memory* mem)
	{
		switch (event->type)
		{
		case SCRIPTABLE_EVENT_FX:
			event->data.playFx.tagName = static_cast<scr_string_t>(buf->write_scriptstring(this->get_script_string(&event->data.playFx.tagName)));
			break;
		case SCRIPTABLE_EVENT_STOP_FX:
			event->data.stopFx.tagName = static_cast<scr_string_t>(buf->write_scriptstring(this->get_script_string(&event->data.stopFx.tagName)));
			break;
		default:
			break;
		}
	}

	void scriptable_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;

		for (unsigned char i = 0; i < data->partCount; i++)
		{
			for (unsigned char j = 0; j < data->parts[i].stateCount; j++)
			{
				data->parts[i].states[j].name = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->parts[i].states[j].name)));

				data->parts[i].states[j].tagName = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->parts[i].states[j].tagName)));
				for (unsigned char k = 0; k < data->parts[i].states[j].onEnterEventCount; k++)
				{
					prepare_scriptable_event_def(&data->parts[i].states[j].onEnterEvents[k], buf, mem);
				}
			}
			data->parts[i].name = static_cast<scr_string_t>(buf->write_scriptstring(this->get_script_string(&data->parts[i].name)));
		}

		for (unsigned char i = 0; i < data->notetrackCount; i++)
		{
			data->notetracks[i].name = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->notetracks[i].name)));
			if (data->notetracks[i].type == SCRIPTABLE_NT_FX)
			{
				data->notetracks[i].data.playFx.tagName = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->notetracks[i].data.playFx.tagName)));
			}
		}
	}

	void scriptable_def::load_depending_scriptable_event_def(zone_base* zone, ScriptableEventDef* event)
	{
		switch (event->type)
		{
		case SCRIPTABLE_EVENT_MODEL:
			zone->add_asset_of_type(ASSET_TYPE_XMODEL, event->data.setModel.model->name);
			break;
		case SCRIPTABLE_EVENT_FX:
			zone->add_asset_of_type(ASSET_TYPE_FX, event->data.playFx.handle->name);
			break;
		case SCRIPTABLE_EVENT_STOP_FX:
			zone->add_asset_of_type(ASSET_TYPE_FX, event->data.stopFx.handle->name);
			break;
		case SCRIPTABLE_EVENT_SOUND:
			zone->add_asset_of_type(ASSET_TYPE_SOUND, event->data.playSound.alias->name);
			break;
		case SCRIPTABLE_EVENT_ANIMATION:
			zone->add_asset_of_type(ASSET_TYPE_XANIMPARTS, event->data.playAnim.animName);
			break;
		case SCRIPTABLE_EVENT_EXPLODE:
			break;
		case SCRIPTABLE_EVENT_HEALTHDRAIN:
			break;
		case SCRIPTABLE_EVENT_PHYSICSLAUNCH:
			zone->add_asset_of_type(ASSET_TYPE_XMODEL, event->data.physicsLaunch.model->name);
			break;
		case SCRIPTABLE_EVENT_LIGHTSETTINGS:
			break;
		case SCRIPTABLE_EVENT_SUNLIGHTSETTINGS:
			break;
		case SCRIPTABLE_EVENT_SHAKE:
			break;
		case SCRIPTABLE_EVENT_TRANSLATE:
			break;
		case SCRIPTABLE_EVENT_ROTATE:
			break;
		case SCRIPTABLE_EVENT_STATECHANGE:
			break;
		default:
			break;
		}
	}

	void scriptable_def::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;

		if (data->baseModel)
		{
			zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->baseModel->name);
		}

		if (data->parts)
		{
			for (unsigned char i = 0; i < data->partCount; i++)
			{
				if (data->parts[i].states)
				{
					for (unsigned char j = 0; j < data->parts[i].stateCount; j++)
					{
						if (data->parts[i].states[j].onEnterEvents)
						{
							for (unsigned char k = 0; k < data->parts[i].states[j].onEnterEventCount; k++)
							{
								load_depending_scriptable_event_def(zone, &data->parts[i].states[j].onEnterEvents[k]);
							}
						}
					}
				}
			}
		}

		if (data->notetracks)
		{
			for (unsigned char i = 0; i < data->notetrackCount; i++)
			{
				if (data->notetracks[i].type == SCRIPTABLE_NT_FX)
				{
					zone->add_asset_of_type(ASSET_TYPE_FX, data->notetracks[i].data.playFx.handle->name);
				}
				else if (data->notetracks[i].type == SCRIPTABLE_NT_SOUND)
				{
					zone->add_asset_of_type(ASSET_TYPE_SOUND, data->notetracks[i].data.playSound.alias->name);
				}
			}
		}
	}

	std::string scriptable_def::name()
	{
		return this->name_;
	}

	std::int32_t scriptable_def::type()
	{
		return ASSET_TYPE_SCRIPTABLE;
	}

	void scriptable_def::write_scriptable_event_def(zone_base* zone, zone_buffer* buf, ScriptableEventDef* data, ScriptableEventDef* dest)
	{
		switch (data->type)
		{
		case SCRIPTABLE_EVENT_MODEL:
			if (data->data.setModel.model)
			{
				dest->data.setModel.model = reinterpret_cast<XModel*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.setModel.model->name));
			}
			break;
		case SCRIPTABLE_EVENT_FX:
			if (data->data.playFx.handle)
			{
				dest->data.playFx.handle = reinterpret_cast<FxEffectDef*>(
					zone->get_asset_pointer(ASSET_TYPE_FX, data->data.playFx.handle->name));
			}
			break;
		case SCRIPTABLE_EVENT_STOP_FX:
			if (data->data.stopFx.handle)
			{
				dest->data.stopFx.handle = reinterpret_cast<FxEffectDef*>(
					zone->get_asset_pointer(ASSET_TYPE_FX, data->data.stopFx.handle->name));
			}
			break;
		case SCRIPTABLE_EVENT_SOUND:
			if (data->data.playSound.alias)
			{
				auto ptr = 0xFDFDFDFFFFFFFFFF;
				buf->align(7);
				buf->write(&ptr);
				buf->write_str(data->data.playSound.alias->name);
				buf->clear_pointer(&dest->data.playSound.alias);
			}
			break;
		case SCRIPTABLE_EVENT_ANIMATION:
			if (data->data.playAnim.animName)
			{
				dest->data.playAnim.animName = buf->write_str(data->data.playAnim.animName);
			}
			break;
		case SCRIPTABLE_EVENT_EXPLODE:
			break;
		case SCRIPTABLE_EVENT_HEALTHDRAIN:
			break;
		case SCRIPTABLE_EVENT_PHYSICSLAUNCH:
			if (data->data.physicsLaunch.model)
			{
				dest->data.physicsLaunch.model = reinterpret_cast<XModel*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.physicsLaunch.model->name));
			}
			break;
		case SCRIPTABLE_EVENT_LIGHTSETTINGS:
			if (data->data.lightSettings.noteworthy)
			{
				dest->data.lightSettings.noteworthy = buf->write_str(data->data.lightSettings.noteworthy);
			}
			break;
		case SCRIPTABLE_EVENT_SUNLIGHTSETTINGS:
			break;
		case SCRIPTABLE_EVENT_SHAKE:
			if (data->data.shake.rumbleName)
			{
				dest->data.shake.rumbleName = buf->write_str(data->data.shake.rumbleName);
			}
			break;
		case SCRIPTABLE_EVENT_TRANSLATE:
			if (data->data.translate.str)
			{
				dest->data.translate.str = buf->write_str(data->data.translate.str);
			}
			break;
		case SCRIPTABLE_EVENT_ROTATE:
			if (data->data.rotate.str)
			{
				dest->data.rotate.str = buf->write_str(data->data.rotate.str);
			}
			break;
		case SCRIPTABLE_EVENT_STATECHANGE:
			break;
		default:
			break;
		}
	}

	void scriptable_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->baseModel)
		{
			dest->baseModel = reinterpret_cast<XModel*>(zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->baseModel->name));
		}

		if (data->baseCollisionBrush)
		{
			dest->baseCollisionBrush = buf->write_str(data->baseCollisionBrush);
		}

		if (data->destroyedCollisionBrush)
		{
			dest->destroyedCollisionBrush = buf->write_str(data->destroyedCollisionBrush);
		}

		if (data->parts)
		{
			buf->align(3);
			auto destparts = buf->write(data->parts, data->partCount);
			for (unsigned char i = 0; i < data->partCount; i++)
			{
				if (data->parts[i].states)
				{
					buf->align(3);
					auto deststates = buf->write(data->parts[i].states, data->parts[i].stateCount);
					for (unsigned char j = 0; j < data->parts[i].stateCount; j++)
					{
						if (data->parts[i].states[j].onEnterEvents)
						{
							buf->align(3);
							auto destevents = buf->write(data->parts[i].states[j].onEnterEvents,
								data->parts[i].states[j].onEnterEventCount);
							for (unsigned char k = 0; k < data->parts[i].states[j].onEnterEventCount; k++)
							{
								write_scriptable_event_def(zone, buf, &data->parts[i].states[j].onEnterEvents[k],
									&destevents[k]);
							}
							buf->clear_pointer(&deststates[j].onEnterEvents);
						}
					}
					buf->clear_pointer(&destparts[i].states);
				}
			}
			buf->clear_pointer(&dest->parts);
		}

		if (data->notetracks)
		{
			buf->align(3);
			auto destnotetracks = buf->write(data->notetracks, data->notetrackCount);
			for (unsigned char i = 0; i < data->notetrackCount; i++)
			{
				if (data->notetracks[i].type)
				{
					if (data->notetracks[i].type == SCRIPTABLE_NT_SOUND)
					{
						if (data->notetracks[i].data.playSound.alias)
						{
							auto ptr = 0xFDFDFDFFFFFFFFFF;
							buf->align(7);
							buf->write(&ptr);
							buf->write_str(data->notetracks[i].data.playSound.alias->name);
							buf->clear_pointer(&destnotetracks[i].data.playSound.alias);
						}
					}
				}
				else
				{
					if (data->notetracks[i].data.playFx.handle)
					{
						destnotetracks[i].data.playFx.handle = reinterpret_cast<FxEffectDef*>(
							zone->get_asset_pointer(ASSET_TYPE_FX, data->notetracks[i].data.playFx.handle->name));
					}
				}
			}
			buf->clear_pointer(&dest->notetracks);
		}

		buf->pop_stream();
	}

	void scriptable_def::dump_scriptable_event_def(ScriptableEventDef* event, assetmanager::dumper& dump)
	{
		switch (event->type)
		{
		case SCRIPTABLE_EVENT_MODEL:
			dump.dump_asset(event->data.setModel.model);
			break;
		case SCRIPTABLE_EVENT_FX:
			dump.dump_asset(event->data.playFx.handle);
			dump.dump_string(SL_ConvertToString(event->data.playFx.tagName));
			break;
		case SCRIPTABLE_EVENT_STOP_FX:
			dump.dump_asset(event->data.stopFx.handle);
			dump.dump_string(SL_ConvertToString(event->data.stopFx.tagName));
			break;
		case SCRIPTABLE_EVENT_SOUND:
			dump.dump_asset(event->data.playSound.alias);
			break;
		case SCRIPTABLE_EVENT_ANIMATION:
			dump.dump_string(event->data.playAnim.animName);
			break;
		case SCRIPTABLE_EVENT_EXPLODE:
			break;
		case SCRIPTABLE_EVENT_HEALTHDRAIN:
			break;
		case SCRIPTABLE_EVENT_PHYSICSLAUNCH:
			dump.dump_asset(event->data.physicsLaunch.model);
			break;
		case SCRIPTABLE_EVENT_LIGHTSETTINGS:
			dump.dump_string(event->data.lightSettings.noteworthy);
			break;
		case SCRIPTABLE_EVENT_SUNLIGHTSETTINGS:
			break;
		case SCRIPTABLE_EVENT_SHAKE:
			dump.dump_string(event->data.shake.rumbleName);
			break;
		case SCRIPTABLE_EVENT_TRANSLATE:
			dump.dump_string(event->data.translate.str);
			break;
		case SCRIPTABLE_EVENT_ROTATE:
			dump.dump_string(event->data.rotate.str);
			break;
		case SCRIPTABLE_EVENT_STATECHANGE:
			break;
		default:
			break;
		}
	}

	void scriptable_def::dump(ScriptableDef* asset)
	{
		const auto path = "scriptable\\"s + asset->name;

		assetmanager::dumper dump;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		dump.dump_asset(asset->baseModel);
		dump.dump_string(asset->baseCollisionBrush);
		dump.dump_string(asset->destroyedCollisionBrush);

		dump.dump_array(asset->parts, asset->partCount);
		for (unsigned char i = 0; i < asset->partCount; i++)
		{
			dump.dump_array(asset->parts[i].states, asset->parts[i].stateCount);
			for (unsigned char j = 0; j < asset->parts[i].stateCount; j++)
			{
				dump.dump_string(SL_ConvertToString(asset->parts[i].states[j].name));
				dump.dump_string(SL_ConvertToString(asset->parts[i].states[j].tagName));
				dump.dump_array(asset->parts[i].states[j].onEnterEvents,
					asset->parts[i].states[j].onEnterEventCount);
				for (unsigned char k = 0; k < asset->parts[i].states[j].onEnterEventCount; k++)
				{
					dump_scriptable_event_def(&asset->parts[i].states[j].onEnterEvents[k], dump);
				}
			}
			dump.dump_string(SL_ConvertToString(asset->parts[i].name));
		}

		dump.dump_array(asset->notetracks, asset->notetrackCount);
		for (unsigned char i = 0; i < asset->notetrackCount; i++)
		{
			dump.dump_string(SL_ConvertToString(asset->notetracks[i].name));
			if (asset->notetracks[i].type == SCRIPTABLE_NT_FX)
			{
				dump.dump_asset(asset->notetracks[i].data.playFx.handle);
				dump.dump_string(SL_ConvertToString(asset->notetracks[i].data.playFx.tagName));
			}
			else if (asset->notetracks[i].type == SCRIPTABLE_NT_SOUND)
			{
				dump.dump_asset(asset->notetracks[i].data.playSound.alias);
			}
		}

		dump.close();
	}
}