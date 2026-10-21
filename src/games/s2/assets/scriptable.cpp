#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		enum scriptable_event_type
		{
			SCRIPTABLE_EVENT_STATE_MODEL = 0,
			SCRIPTABLE_EVENT_MODEL = 1,
			SCRIPTABLE_EVENT_FX = 2,
			SCRIPTABLE_EVENT_STOP_FX = 3,
			SCRIPTABLE_EVENT_SOUND = 4,
			SCRIPTABLE_EVENT_ANIMATION = 5,
			SCRIPTABLE_EVENT_EXPLODE = 6,
			SCRIPTABLE_EVENT_HEALTHDRAIN = 7,
			SCRIPTABLE_EVENT_PHYSICSLAUNCH = 8,
			SCRIPTABLE_EVENT_NAMED_SPLINE = 9,
			SCRIPTABLE_EVENT_LATE_SPLINE = 10,
			SCRIPTABLE_EVENT_STRING_11 = 11,
			SCRIPTABLE_EVENT_LATE_STRING_12 = 12,
			SCRIPTABLE_EVENT_LATE_STRING_13 = 13,
			SCRIPTABLE_EVENT_MAYHEM = 14,
			SCRIPTABLE_EVENT_STRING_15 = 15,
			SCRIPTABLE_EVENT_SPLINE = 16,
			SCRIPTABLE_EVENT_NONE_17 = 17,
		};

		enum scriptable_notetrack_type
		{
			SCRIPTABLE_NOTETRACK_FX = 0,
			SCRIPTABLE_NOTETRACK_SOUND = 1,
		};

		void read_event(zone_reader& reader, ScriptableEventDef* event)
		{
			auto* data = &event->data;
			switch (event->type)
			{
			case SCRIPTABLE_EVENT_STATE_MODEL:
				reader.read_asset(ASSET_TYPE_XMODEL, data->stateModel.model);
				reader.read_string(data->stateModel.name);
				break;
			case SCRIPTABLE_EVENT_MODEL:
			case SCRIPTABLE_EVENT_PHYSICSLAUNCH:
				reader.read_asset(ASSET_TYPE_XMODEL, data->model);
				break;
			case SCRIPTABLE_EVENT_FX:
			case SCRIPTABLE_EVENT_STOP_FX:
				reader.read_asset(ASSET_TYPE_FX, data->fx.effect);
				reader.read_script_string(data->fx.tag);
				break;
			case SCRIPTABLE_EVENT_SOUND:
				reader.read_name_reference(data->sound.alias);
				break;
			case SCRIPTABLE_EVENT_ANIMATION:
			case SCRIPTABLE_EVENT_STRING_11:
			case SCRIPTABLE_EVENT_STRING_15:
				reader.read_string(data->string.name);
				break;
			case SCRIPTABLE_EVENT_NAMED_SPLINE:
				reader.read_string(data->namedSpline.name);
				reader.read_asset(ASSET_TYPE_SPLINES, data->namedSpline.spline);
				break;
			case SCRIPTABLE_EVENT_LATE_SPLINE:
				reader.read_asset(ASSET_TYPE_SPLINES, data->lateSpline.spline);
				break;
			case SCRIPTABLE_EVENT_LATE_STRING_12:
			case SCRIPTABLE_EVENT_LATE_STRING_13:
				reader.read_string(data->lateString.name);
				break;
			case SCRIPTABLE_EVENT_MAYHEM:
				reader.read_asset(ASSET_TYPE_MAYHEM, data->mayhem.mayhem);
				break;
			case SCRIPTABLE_EVENT_SPLINE:
				reader.read_asset(ASSET_TYPE_SPLINES, data->spline.spline);
				break;
			default:
				break;
			}
		}

		void parse_event(assetmanager::reader& read, ScriptableEventDef* event)
		{
			auto* data = &event->data;
			switch (event->type)
			{
			case SCRIPTABLE_EVENT_STATE_MODEL:
				data->stateModel.model = read.read_asset<XModel>();
				data->stateModel.name = read.read_string();
				break;
			case SCRIPTABLE_EVENT_MODEL:
			case SCRIPTABLE_EVENT_PHYSICSLAUNCH:
				data->model = read.read_asset<XModel>();
				break;
			case SCRIPTABLE_EVENT_FX:
			case SCRIPTABLE_EVENT_STOP_FX:
				data->fx.effect = read.read_asset<FxEffectDef>();
				data->fx.tag = parse_script_string(read);
				break;
			case SCRIPTABLE_EVENT_SOUND:
				data->sound.alias = read.read_asset<snd_alias_list_t>();
				break;
			case SCRIPTABLE_EVENT_ANIMATION:
			case SCRIPTABLE_EVENT_STRING_11:
			case SCRIPTABLE_EVENT_STRING_15:
				data->string.name = read.read_string();
				break;
			case SCRIPTABLE_EVENT_NAMED_SPLINE:
				data->namedSpline.name = read.read_string();
				data->namedSpline.spline = read.read_asset<Spline>();
				break;
			case SCRIPTABLE_EVENT_LATE_SPLINE:
				data->lateSpline.spline = read.read_asset<Spline>();
				break;
			case SCRIPTABLE_EVENT_LATE_STRING_12:
			case SCRIPTABLE_EVENT_LATE_STRING_13:
				data->lateString.name = read.read_string();
				break;
			case SCRIPTABLE_EVENT_MAYHEM:
				data->mayhem.mayhem = read.read_asset<Mayhem>();
				break;
			case SCRIPTABLE_EVENT_SPLINE:
				data->spline.spline = read.read_asset<Spline>();
				break;
			default:
				break;
			}
		}

		void write_event(zone_base* zone, zone_buffer* buf, ScriptableEventDef* event, ScriptableEventDef* dest)
		{
			auto* data = &event->data;
			auto* dest_data = &dest->data;
			switch (event->type)
			{
			case SCRIPTABLE_EVENT_STATE_MODEL:
				dest_data->stateModel.model = asset_reference(zone, ASSET_TYPE_XMODEL, data->stateModel.model);
				write_string(buf, data->stateModel.name, dest_data->stateModel.name);
				break;
			case SCRIPTABLE_EVENT_MODEL:
			case SCRIPTABLE_EVENT_PHYSICSLAUNCH:
				dest_data->model = asset_reference(zone, ASSET_TYPE_XMODEL, data->model);
				break;
			case SCRIPTABLE_EVENT_FX:
			case SCRIPTABLE_EVENT_STOP_FX:
				dest_data->fx.effect = asset_reference(zone, ASSET_TYPE_FX, data->fx.effect);
				break;
			case SCRIPTABLE_EVENT_SOUND:
				write_sound(buf, data->sound.alias, dest_data->sound.alias);
				break;
			case SCRIPTABLE_EVENT_ANIMATION:
			case SCRIPTABLE_EVENT_STRING_11:
			case SCRIPTABLE_EVENT_STRING_15:
				write_string(buf, data->string.name, dest_data->string.name);
				break;
			case SCRIPTABLE_EVENT_NAMED_SPLINE:
				write_string(buf, data->namedSpline.name, dest_data->namedSpline.name);
				dest_data->namedSpline.spline = asset_reference(zone, ASSET_TYPE_SPLINES, data->namedSpline.spline);
				break;
			case SCRIPTABLE_EVENT_LATE_SPLINE:
				dest_data->lateSpline.spline = asset_reference(zone, ASSET_TYPE_SPLINES, data->lateSpline.spline);
				break;
			case SCRIPTABLE_EVENT_LATE_STRING_12:
			case SCRIPTABLE_EVENT_LATE_STRING_13:
				write_string(buf, data->lateString.name, dest_data->lateString.name);
				break;
			case SCRIPTABLE_EVENT_MAYHEM:
				dest_data->mayhem.mayhem = asset_reference(zone, ASSET_TYPE_MAYHEM, data->mayhem.mayhem);
				break;
			case SCRIPTABLE_EVENT_SPLINE:
				dest_data->spline.spline = asset_reference(zone, ASSET_TYPE_SPLINES, data->spline.spline);
				break;
			default:
				break;
			}
		}

		void dump_event(assetmanager::dumper& write, ScriptableEventDef* event)
		{
			auto* data = &event->data;
			switch (event->type)
			{
			case SCRIPTABLE_EVENT_STATE_MODEL:
				write.dump_asset(data->stateModel.model);
				write.dump_string(data->stateModel.name);
				break;
			case SCRIPTABLE_EVENT_MODEL:
			case SCRIPTABLE_EVENT_PHYSICSLAUNCH:
				write.dump_asset(data->model);
				break;
			case SCRIPTABLE_EVENT_FX:
			case SCRIPTABLE_EVENT_STOP_FX:
				write.dump_asset(data->fx.effect);
				dump_script_string(write, data->fx.tag);
				break;
			case SCRIPTABLE_EVENT_SOUND:
				write.dump_asset(data->sound.alias);
				break;
			case SCRIPTABLE_EVENT_ANIMATION:
			case SCRIPTABLE_EVENT_STRING_11:
			case SCRIPTABLE_EVENT_STRING_15:
				write.dump_string(data->string.name);
				break;
			case SCRIPTABLE_EVENT_NAMED_SPLINE:
				write.dump_string(data->namedSpline.name);
				write.dump_asset(data->namedSpline.spline);
				break;
			case SCRIPTABLE_EVENT_LATE_SPLINE:
				write.dump_asset(data->lateSpline.spline);
				break;
			case SCRIPTABLE_EVENT_LATE_STRING_12:
			case SCRIPTABLE_EVENT_LATE_STRING_13:
				write.dump_string(data->lateString.name);
				break;
			case SCRIPTABLE_EVENT_MAYHEM:
				write.dump_asset(data->mayhem.mayhem);
				break;
			case SCRIPTABLE_EVENT_SPLINE:
				write.dump_asset(data->spline.spline);
				break;
			default:
				break;
			}
		}

		void add_event_dependencies(zone_base* zone, ScriptableEventDef* event)
		{
			auto* data = &event->data;
			switch (event->type)
			{
			case SCRIPTABLE_EVENT_STATE_MODEL:
				add_dependency(zone, ASSET_TYPE_XMODEL, data->stateModel.model);
				break;
			case SCRIPTABLE_EVENT_MODEL:
			case SCRIPTABLE_EVENT_PHYSICSLAUNCH:
				add_dependency(zone, ASSET_TYPE_XMODEL, data->model);
				break;
			case SCRIPTABLE_EVENT_FX:
			case SCRIPTABLE_EVENT_STOP_FX:
				add_dependency(zone, ASSET_TYPE_FX, data->fx.effect);
				break;
			case SCRIPTABLE_EVENT_SOUND:
				add_dependency(zone, ASSET_TYPE_SOUND, data->sound.alias);
				break;
			case SCRIPTABLE_EVENT_NAMED_SPLINE:
				add_dependency(zone, ASSET_TYPE_SPLINES, data->namedSpline.spline);
				break;
			case SCRIPTABLE_EVENT_LATE_SPLINE:
				add_dependency(zone, ASSET_TYPE_SPLINES, data->lateSpline.spline);
				break;
			case SCRIPTABLE_EVENT_MAYHEM:
				add_dependency(zone, ASSET_TYPE_MAYHEM, data->mayhem.mayhem);
				break;
			case SCRIPTABLE_EVENT_SPLINE:
				add_dependency(zone, ASSET_TYPE_SPLINES, data->spline.spline);
				break;
			default:
				break;
			}
		}

		bool has_fx_tag(const ScriptableEventDef* event)
		{
			return event->type == SCRIPTABLE_EVENT_FX || event->type == SCRIPTABLE_EVENT_STOP_FX;
		}
	}

	void scriptable_def::read(zone_reader& reader, ScriptableDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->baseModel);
		reader.read_string(asset->baseCollision);

		if (const auto parts = reader.read_array(asset->parts, 7, asset->partCount))
		{
			for (auto i = 0; i < asset->partCount; i++)
			{
				auto* part = &parts[i];
				if (const auto states = reader.read_array(part->states, 7, part->stateCount))
				{
					for (auto j = 0; j < part->stateCount; j++)
					{
						reader.read_script_string(states[j].name);
						reader.read_script_string(states[j].tagName);
						if (const auto events = reader.read_array(states[j].events, 7, states[j].eventCount))
						{
							for (auto k = 0; k < states[j].eventCount; k++)
							{
								read_event(reader, &events[k]);
							}
						}
					}
				}
				reader.read_script_string(part->name);
				reader.read_array(part->flags, 0, part->flagCount);
			}
		}

		if (const auto notetracks = reader.read_array(asset->notetracks, 7, asset->notetrackCount))
		{
			for (auto i = 0; i < asset->notetrackCount; i++)
			{
				auto* notetrack = &notetracks[i];
				reader.read_script_string(notetrack->name);
				if (notetrack->type == SCRIPTABLE_NOTETRACK_FX)
				{
					reader.read_asset(ASSET_TYPE_FX, notetrack->data.fx.effect);
					reader.read_script_string(notetrack->data.fx.tag);
				}
				else if (notetrack->type == SCRIPTABLE_NOTETRACK_SOUND)
				{
					reader.read_name_reference(notetrack->data.sound.alias);
				}
			}
		}

		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->models, asset->modelCount);

		if (const auto tags = reader.read_array(asset->modelTags, 7, asset->modelTagCount))
		{
			for (auto i = 0; i < asset->modelTagCount; i++)
			{
				reader.read_asset(ASSET_TYPE_XMODEL, tags[i].model);
				reader.read_script_string(tags[i].tag);
			}
		}

		reader.pop_stream();
	}

	ScriptableDef* scriptable_def::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_SCRIPTABLE, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing scriptable \"%s\"...", name.data());

		auto* asset = read.read_single<ScriptableDef>();
		asset->name = read.read_string();
		asset->baseModel = read.read_asset<XModel>();
		asset->baseCollision = read.read_string();

		if (const auto parts = parse_array(read, mem, asset->parts, asset->partCount))
		{
			for (auto i = 0; i < asset->partCount; i++)
			{
				auto* part = &parts[i];
				if (const auto states = parse_array(read, mem, part->states, part->stateCount))
				{
					for (auto j = 0; j < part->stateCount; j++)
					{
						states[j].name = parse_script_string(read);
						states[j].tagName = parse_script_string(read);
						if (const auto events = parse_array(read, mem, states[j].events, states[j].eventCount))
						{
							for (auto k = 0; k < states[j].eventCount; k++)
							{
								parse_event(read, &events[k]);
							}
						}
					}
				}
				part->name = parse_script_string(read);
				parse_array(read, mem, part->flags, part->flagCount);
			}
		}

		if (const auto notetracks = parse_array(read, mem, asset->notetracks, asset->notetrackCount))
		{
			for (auto i = 0; i < asset->notetrackCount; i++)
			{
				auto* notetrack = &notetracks[i];
				notetrack->name = parse_script_string(read);
				if (notetrack->type == SCRIPTABLE_NOTETRACK_FX)
				{
					notetrack->data.fx.effect = read.read_asset<FxEffectDef>();
					notetrack->data.fx.tag = parse_script_string(read);
				}
				else if (notetrack->type == SCRIPTABLE_NOTETRACK_SOUND)
				{
					notetrack->data.sound.alias = read.read_asset<snd_alias_list_t>();
				}
			}
		}

		parse_asset_array(read, mem, asset->models, asset->modelCount);

		if (const auto tags = parse_array(read, mem, asset->modelTags, asset->modelTagCount))
		{
			for (auto i = 0; i < asset->modelTagCount; i++)
			{
				tags[i].model = read.read_asset<XModel>();
				tags[i].tag = parse_script_string(read);
			}
		}
		read.close();

		return asset;
	}

	void scriptable_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;
		script_string_preparer prepare(buf);

		for (auto i = 0; data->parts && i < data->partCount; i++)
		{
			auto* part = &data->parts[i];
			for (auto j = 0; part->states && j < part->stateCount; j++)
			{
				auto* state = &part->states[j];
				prepare(state->name);
				prepare(state->tagName);
				for (auto k = 0; state->events && k < state->eventCount; k++)
				{
					if (has_fx_tag(&state->events[k]))
					{
						prepare(state->events[k].data.fx.tag);
					}
				}
			}
			prepare(part->name);
		}

		for (auto i = 0; data->notetracks && i < data->notetrackCount; i++)
		{
			auto* notetrack = &data->notetracks[i];
			prepare(notetrack->name);
			if (notetrack->type == SCRIPTABLE_NOTETRACK_FX)
			{
				prepare(notetrack->data.fx.tag);
			}
		}

		for (auto i = 0; data->modelTags && i < data->modelTagCount; i++)
		{
			prepare(data->modelTags[i].tag);
		}
	}

	void scriptable_def::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;
		add_dependency(zone, ASSET_TYPE_XMODEL, data->baseModel);

		for (auto i = 0; data->parts && i < data->partCount; i++)
		{
			auto* part = &data->parts[i];
			for (auto j = 0; part->states && j < part->stateCount; j++)
			{
				auto* state = &part->states[j];
				for (auto k = 0; state->events && k < state->eventCount; k++)
				{
					add_event_dependencies(zone, &state->events[k]);
				}
			}
		}

		for (auto i = 0; data->notetracks && i < data->notetrackCount; i++)
		{
			auto* notetrack = &data->notetracks[i];
			if (notetrack->type == SCRIPTABLE_NOTETRACK_FX)
			{
				add_dependency(zone, ASSET_TYPE_FX, notetrack->data.fx.effect);
			}
			else if (notetrack->type == SCRIPTABLE_NOTETRACK_SOUND)
			{
				add_dependency(zone, ASSET_TYPE_SOUND, notetrack->data.sound.alias);
			}
		}

		add_dependencies(zone, ASSET_TYPE_XMODEL, data->models, data->modelCount);

		for (auto i = 0; data->modelTags && i < data->modelTagCount; i++)
		{
			add_dependency(zone, ASSET_TYPE_XMODEL, data->modelTags[i].model);
		}
	}

	void scriptable_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = this->write_name(buf);
		dest->baseModel = asset_reference(zone, ASSET_TYPE_XMODEL, data->baseModel);
		write_string(buf, data->baseCollision, dest->baseCollision);

		if (const auto dest_parts = write_array(buf, 7, data->parts, data->partCount, dest->parts))
		{
			for (auto i = 0; i < data->partCount; i++)
			{
				auto* part = &data->parts[i];
				if (const auto dest_states = write_array(buf, 7, part->states, part->stateCount, dest_parts[i].states))
				{
					for (auto j = 0; j < part->stateCount; j++)
					{
						auto* state = &part->states[j];
						if (const auto dest_events = write_array(buf, 7, state->events, state->eventCount, dest_states[j].events))
						{
							for (auto k = 0; k < state->eventCount; k++)
							{
								write_event(zone, buf, &state->events[k], &dest_events[k]);
							}
						}
					}
				}
				write_array(buf, 0, part->flags, part->flagCount, dest_parts[i].flags);
			}
		}

		if (const auto dest_notetracks = write_array(buf, 7, data->notetracks, data->notetrackCount, dest->notetracks))
		{
			for (auto i = 0; i < data->notetrackCount; i++)
			{
				auto* notetrack = &data->notetracks[i];
				auto* dest_notetrack = &dest_notetracks[i];
				if (notetrack->type == SCRIPTABLE_NOTETRACK_FX)
				{
					dest_notetrack->data.fx.effect = asset_reference(zone, ASSET_TYPE_FX, notetrack->data.fx.effect);
				}
				else if (notetrack->type == SCRIPTABLE_NOTETRACK_SOUND)
				{
					write_sound(buf, notetrack->data.sound.alias, dest_notetrack->data.sound.alias);
				}
			}
		}

		write_asset_array(zone, buf, ASSET_TYPE_XMODEL, data->models, data->modelCount, dest->models);

		if (const auto dest_tags = write_array(buf, 7, data->modelTags, data->modelTagCount, dest->modelTags))
		{
			for (auto i = 0; i < data->modelTagCount; i++)
			{
				dest_tags[i].model = asset_reference(zone, ASSET_TYPE_XMODEL, data->modelTags[i].model);
			}
		}

		buf->pop_stream();
	}

	void scriptable_def::dump(ScriptableDef* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_SCRIPTABLE, asset->name)))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_asset(asset->baseModel);
		write.dump_string(asset->baseCollision);

		write.dump_array(asset->parts, asset->partCount);
		for (auto i = 0; asset->parts && i < asset->partCount; i++)
		{
			auto* part = &asset->parts[i];
			write.dump_array(part->states, part->stateCount);
			for (auto j = 0; part->states && j < part->stateCount; j++)
			{
				auto* state = &part->states[j];
				dump_script_string(write, state->name);
				dump_script_string(write, state->tagName);
				write.dump_array(state->events, state->eventCount);
				for (auto k = 0; state->events && k < state->eventCount; k++)
				{
					dump_event(write, &state->events[k]);
				}
			}
			dump_script_string(write, part->name);
			write.dump_array(part->flags, part->flagCount);
		}

		write.dump_array(asset->notetracks, asset->notetrackCount);
		for (auto i = 0; asset->notetracks && i < asset->notetrackCount; i++)
		{
			auto* notetrack = &asset->notetracks[i];
			dump_script_string(write, notetrack->name);
			if (notetrack->type == SCRIPTABLE_NOTETRACK_FX)
			{
				write.dump_asset(notetrack->data.fx.effect);
				dump_script_string(write, notetrack->data.fx.tag);
			}
			else if (notetrack->type == SCRIPTABLE_NOTETRACK_SOUND)
			{
				write.dump_asset(notetrack->data.sound.alias);
			}
		}

		dump_asset_array(write, asset->models, asset->modelCount);

		write.dump_array(asset->modelTags, asset->modelTagCount);
		for (auto i = 0; asset->modelTags && i < asset->modelTagCount; i++)
		{
			write.dump_asset(asset->modelTags[i].model);
			dump_script_string(write, asset->modelTags[i].tag);
		}
		write.close();
	}
}
