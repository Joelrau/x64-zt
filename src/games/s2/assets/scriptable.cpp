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

		void read_event(zone_reader& reader, ScriptableEventDef* event)
		{
			auto& data = event->data;
			switch (event->type)
			{
			case SCRIPTABLE_EVENT_STATE_MODEL:
				reader.read_asset(ASSET_TYPE_XMODEL, data.stateModel.model);
				reader.read_string(data.stateModel.name);
				break;
			case SCRIPTABLE_EVENT_MODEL:
			case SCRIPTABLE_EVENT_PHYSICSLAUNCH:
				reader.read_asset(ASSET_TYPE_XMODEL, data.model);
				break;
			case SCRIPTABLE_EVENT_FX:
			case SCRIPTABLE_EVENT_STOP_FX:
				reader.read_asset(ASSET_TYPE_FX, data.fx.effect);
				reader.read_script_string(data.fx.tag);
				break;
			case SCRIPTABLE_EVENT_SOUND:
				reader.read_name_reference(data.sound.alias);
				break;
			case SCRIPTABLE_EVENT_ANIMATION:
			case SCRIPTABLE_EVENT_STRING_11:
			case SCRIPTABLE_EVENT_STRING_15:
				reader.read_string(data.string.name);
				break;
			case SCRIPTABLE_EVENT_NAMED_SPLINE:
				reader.read_string(data.namedSpline.name);
				reader.read_asset(ASSET_TYPE_SPLINES, data.namedSpline.spline);
				break;
			case SCRIPTABLE_EVENT_LATE_SPLINE:
				reader.read_asset(ASSET_TYPE_SPLINES, data.lateSpline.spline);
				break;
			case SCRIPTABLE_EVENT_LATE_STRING_12:
			case SCRIPTABLE_EVENT_LATE_STRING_13:
				reader.read_string(data.lateString.name);
				break;
			case SCRIPTABLE_EVENT_MAYHEM:
				reader.read_asset(ASSET_TYPE_MAYHEM, data.mayhem.mayhem);
				break;
			case SCRIPTABLE_EVENT_SPLINE:
				reader.read_asset(ASSET_TYPE_SPLINES, data.spline.spline);
				break;
			default:
				break;
			}
		}

		void read_state(zone_reader& reader, ScriptableStateDef* state)
		{
			reader.read_script_string(state->name);
			reader.read_script_string(state->tagName);
			if (const auto events = reader.read_array(state->events, 7, state->eventCount))
			{
				for (auto i = 0; i < state->eventCount; i++)
				{
					read_event(reader, &events[i]);
				}
			}
		}

		void read_part(zone_reader& reader, ScriptablePartDef* part)
		{
			if (const auto states = reader.read_array(part->states, 7, part->stateCount))
			{
				for (auto i = 0; i < part->stateCount; i++)
				{
					read_state(reader, &states[i]);
				}
			}
			reader.read_script_string(part->name);
			reader.read_array(part->flags, 0, part->flagCount);
		}

		void read_notetrack(zone_reader& reader, ScriptableNotetrackDef* notetrack)
		{
			reader.read_script_string(notetrack->name);
			if (notetrack->type == 0)
			{
				reader.read_asset(ASSET_TYPE_FX, notetrack->data.fx.effect);
				reader.read_script_string(notetrack->data.fx.tag);
			}
			else if (notetrack->type == 1)
			{
				reader.read_name_reference(notetrack->data.sound.alias);
			}
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
				read_part(reader, &parts[i]);
			}
		}

		if (const auto notetracks = reader.read_array(asset->notetracks, 7, asset->notetrackCount))
		{
			for (auto i = 0; i < asset->notetrackCount; i++)
			{
				read_notetrack(reader, &notetracks[i]);
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
}
