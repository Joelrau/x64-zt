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

		template <typename V>
		void visit_event(const V& event)
		{
			constexpr auto data = offsetof(ScriptableEventDef, data);
			switch (event.template at<int>(offsetof(ScriptableEventDef, type)))
			{
			case SCRIPTABLE_EVENT_STATE_MODEL:
				event.asset(ASSET_TYPE_XMODEL, data + offsetof(ScriptableEventDataUnion, stateModel.model));
				event.string(data + offsetof(ScriptableEventDataUnion, stateModel.name));
				break;
			case SCRIPTABLE_EVENT_MODEL:
			case SCRIPTABLE_EVENT_PHYSICSLAUNCH:
				event.asset(ASSET_TYPE_XMODEL, data + offsetof(ScriptableEventDataUnion, model));
				break;
			case SCRIPTABLE_EVENT_FX:
			case SCRIPTABLE_EVENT_STOP_FX:
				event.asset(ASSET_TYPE_FX, data + offsetof(ScriptableEventDataUnion, fx.effect));
				event.script_string(data + offsetof(ScriptableEventDataUnion, fx.tag));
				break;
			case SCRIPTABLE_EVENT_SOUND:
				event.sound(data + offsetof(ScriptableEventDataUnion, sound.alias));
				break;
			case SCRIPTABLE_EVENT_ANIMATION:
			case SCRIPTABLE_EVENT_STRING_11:
			case SCRIPTABLE_EVENT_STRING_15:
				event.string(data + offsetof(ScriptableEventDataUnion, string.name));
				break;
			case SCRIPTABLE_EVENT_NAMED_SPLINE:
				event.string(data + offsetof(ScriptableEventDataUnion, namedSpline.name));
				event.asset(ASSET_TYPE_SPLINES, data + offsetof(ScriptableEventDataUnion, namedSpline.spline));
				break;
			case SCRIPTABLE_EVENT_LATE_SPLINE:
				event.asset(ASSET_TYPE_SPLINES, data + offsetof(ScriptableEventDataUnion, lateSpline.spline));
				break;
			case SCRIPTABLE_EVENT_LATE_STRING_12:
			case SCRIPTABLE_EVENT_LATE_STRING_13:
				event.string(data + offsetof(ScriptableEventDataUnion, lateString.name));
				break;
			case SCRIPTABLE_EVENT_MAYHEM:
				event.asset(ASSET_TYPE_MAYHEM, data + offsetof(ScriptableEventDataUnion, mayhem.mayhem));
				break;
			case SCRIPTABLE_EVENT_SPLINE:
				event.asset(ASSET_TYPE_SPLINES, data + offsetof(ScriptableEventDataUnion, spline.spline));
				break;
			default:
				break;
			}
		}

		template <typename V>
		void visit_state(const V& state)
		{
			state.script_string(offsetof(ScriptableStateDef, name));
			state.script_string(offsetof(ScriptableStateDef, tagName));
			state.entries(offsetof(ScriptableStateDef, events), sizeof(ScriptableEventDef),
				state.template at<unsigned char>(offsetof(ScriptableStateDef, eventCount)), [](const auto& event, std::size_t)
			{
				visit_event(event);
			});
		}

		template <typename V>
		void visit_part(const V& part)
		{
			part.entries(offsetof(ScriptablePartDef, states), sizeof(ScriptableStateDef),
				part.template at<unsigned char>(offsetof(ScriptablePartDef, stateCount)), [](const auto& state, std::size_t)
			{
				visit_state(state);
			});
			part.script_string(offsetof(ScriptablePartDef, name));
			part.bytes(offsetof(ScriptablePartDef, flags), 0, part.template at<unsigned char>(offsetof(ScriptablePartDef, flagCount)));
		}

		template <typename V>
		void visit_notetrack(const V& notetrack)
		{
			notetrack.script_string(offsetof(ScriptableNotetrackDef, name));
			constexpr auto data = offsetof(ScriptableNotetrackDef, data);
			const auto type = notetrack.template at<int>(offsetof(ScriptableNotetrackDef, type));
			if (type == 0)
			{
				notetrack.asset(ASSET_TYPE_FX, data + offsetof(ScriptableNotetrackDataUnion, fx.effect));
				notetrack.script_string(data + offsetof(ScriptableNotetrackDataUnion, fx.tag));
			}
			else if (type == 1)
			{
				notetrack.sound(data + offsetof(ScriptableNotetrackDataUnion, sound.alias));
			}
		}
	}

	template <typename V>
	void scriptable_layout::visit(const V& w)
	{
		w.string(offsetof(ScriptableDef, name));
		w.asset(ASSET_TYPE_XMODEL, offsetof(ScriptableDef, baseModel));
		w.string(offsetof(ScriptableDef, baseCollision));
		w.entries(offsetof(ScriptableDef, parts), sizeof(ScriptablePartDef),
			w.template at<unsigned short>(offsetof(ScriptableDef, partCount)), [](const auto& part, std::size_t)
		{
			visit_part(part);
		});
		w.entries(offsetof(ScriptableDef, notetracks), sizeof(ScriptableNotetrackDef),
			w.template at<unsigned char>(offsetof(ScriptableDef, notetrackCount)), [](const auto& notetrack, std::size_t)
		{
			visit_notetrack(notetrack);
		});
		w.asset_array(ASSET_TYPE_XMODEL, offsetof(ScriptableDef, models), w.template at<unsigned char>(offsetof(ScriptableDef, modelCount)));
		w.entries(offsetof(ScriptableDef, modelTags), sizeof(ScriptableModelTag),
			w.template at<unsigned char>(offsetof(ScriptableDef, modelTagCount)), [](const auto& tag, std::size_t)
		{
			tag.asset(ASSET_TYPE_XMODEL, offsetof(ScriptableModelTag, model));
			tag.script_string(offsetof(ScriptableModelTag, tag));
		});
	}

	template void scriptable_layout::visit(const field_reader&);
	template void scriptable_layout::visit(const field_writer&);
	template void scriptable_layout::visit(const field_dumper&);
	template void scriptable_layout::visit(const field_parser&);
	template void scriptable_layout::visit(const field_dependencies&);
	template void scriptable_layout::visit(const field_preparer&);
}
