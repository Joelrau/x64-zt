#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "scriptabledef.hpp"

#include "zonetool/h1/assets/scriptabledef.hpp"

#include "zonetool/iw6/functions.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace
		{
			std::unordered_map<zonetool::iw6::ScriptableEventType, zonetool::h1::ScriptableEventType> scriptable_event_type_map =
			{
				{zonetool::iw6::SCRIPTABLE_EVENT_MODEL, zonetool::h1::SCRIPTABLE_EVENT_MODEL},
				{zonetool::iw6::SCRIPTABLE_EVENT_FX, zonetool::h1::SCRIPTABLE_EVENT_FX},
				//{zonetool::iw6::SCRIPTABLE_EVENT_STOP_FX, zonetool::h1::SCRIPTABLE_EVENT_STOP_FX},
				{zonetool::iw6::SCRIPTABLE_EVENT_SOUND, zonetool::h1::SCRIPTABLE_EVENT_SOUND},
				{zonetool::iw6::SCRIPTABLE_EVENT_ANIMATION, zonetool::h1::SCRIPTABLE_EVENT_ANIMATION},
				{zonetool::iw6::SCRIPTABLE_EVENT_EXPLODE, zonetool::h1::SCRIPTABLE_EVENT_EXPLODE},
				{zonetool::iw6::SCRIPTABLE_EVENT_HEALTHDRAIN, zonetool::h1::SCRIPTABLE_EVENT_HEALTHDRAIN},
				{zonetool::iw6::SCRIPTABLE_EVENT_PHYSICSLAUNCH, zonetool::h1::SCRIPTABLE_EVENT_PHYSICSLAUNCH},
				{zonetool::iw6::SCRIPTABLE_EVENT_LIGHTSETTINGS, zonetool::h1::SCRIPTABLE_EVENT_LIGHTSETTINGS},
				{zonetool::iw6::SCRIPTABLE_EVENT_SUNLIGHTSETTINGS, zonetool::h1::SCRIPTABLE_EVENT_SUNLIGHTSETTINGS},
				{zonetool::iw6::SCRIPTABLE_EVENT_SHAKE, zonetool::h1::SCRIPTABLE_EVENT_SHAKE},
				//{zonetool::iw6::SCRIPTABLE_EVENT_TRANSLATE, zonetool::h1::SCRIPTABLE_EVENT_TRANSLATE},
				//{zonetool::iw6::SCRIPTABLE_EVENT_ROTATE, zonetool::h1::SCRIPTABLE_EVENT_ROTATE},
				{zonetool::iw6::SCRIPTABLE_EVENT_STATECHANGE, zonetool::h1::SCRIPTABLE_EVENT_STATECHANGE},
			};
		}

		namespace scriptabledef
		{
			zonetool::h1::ScriptableDef* convert(ScriptableDef* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::ScriptableDef>();

				// the only thing different about h1 scriptabledef is that there is extra type of ScriptableEventType
				// ScriptableEventSunlightSettingsDef also changed, that may cause issues...

				std::memcpy(new_asset, asset, sizeof(zonetool::h1::ScriptableDef)); // same struct

				new_asset->parts = allocator.allocate_array<zonetool::h1::ScriptablePartDef>(asset->partCount);
				for (unsigned char i = 0; i < asset->partCount; i++)
				{
					const auto part = &asset->parts[i];
					const auto new_part = &new_asset->parts[i];

					std::memcpy(new_part, part, sizeof(zonetool::h1::ScriptablePartDef)); // same struct

					new_part->states = allocator.allocate_array<zonetool::h1::ScriptableStateDef>(part->stateCount);
					for (unsigned char j = 0; j < part->stateCount; j++)
					{
						const auto state = &part->states[j];
						const auto new_state = &new_part->states[j];

						std::memcpy(new_state, state, sizeof(zonetool::h1::ScriptableStateDef)); // same struct

						new_state->onEnterEvents = allocator.allocate_array<zonetool::h1::ScriptableEventDef>(state->onEnterEventCount);
						for (unsigned char k = 0; k < state->onEnterEventCount; k++)
						{
							const auto event = &state->onEnterEvents[k];
							const auto new_event = &new_state->onEnterEvents[k];

							std::memcpy(new_event, event, sizeof(zonetool::h1::ScriptableEventDef)); // same stuff, mostly

							new_event->type = scriptable_event_type_map[event->type];

							if (event->type == SCRIPTABLE_EVENT_LIGHTSETTINGS)
							{
								new_event->data.lightSettings.color[0] = event->data.lightSettings.color[0];
								new_event->data.lightSettings.color[1] = event->data.lightSettings.color[1];
								new_event->data.lightSettings.color[2] = event->data.lightSettings.color[2];
								new_event->data.lightSettings.color[3] = event->data.lightSettings.color[3];
								new_event->data.lightSettings.lightIndexConstIndex = event->data.lightSettings.lightIndexConstIndex;
								new_event->data.lightSettings.transStateStreamIndex = event->data.lightSettings.transStateStreamIndex;
								new_event->data.lightSettings.useColor = event->data.lightSettings.useColor;
								new_event->data.lightSettings.useStateTransitionTime = event->data.lightSettings.useStateTransitionTime;
								new_event->data.lightSettings.intensityScaleMin = event->data.lightSettings.intensityScaleMin;
								new_event->data.lightSettings.intensityScaleMax = event->data.lightSettings.intensityScaleMax;
								new_event->data.lightSettings.radiusScaleMin = event->data.lightSettings.radiusScaleMin;
								new_event->data.lightSettings.radiusScaleMax = event->data.lightSettings.radiusScaleMax;
								new_event->data.lightSettings.transitionTimeMin = event->data.lightSettings.transitionTimeMin;
								new_event->data.lightSettings.transitionTimeMax = event->data.lightSettings.transitionTimeMax;
								new_event->data.lightSettings.noteworthy = event->data.lightSettings.noteworthy;
							}
							else if (event->type == SCRIPTABLE_EVENT_SUNLIGHTSETTINGS)
							{
								new_event->data.sunlightSettings.color[0] = event->data.sunlightSettings.color[0];
								new_event->data.sunlightSettings.color[1] = event->data.sunlightSettings.color[1];
								new_event->data.sunlightSettings.color[2] = event->data.sunlightSettings.color[2];
								new_event->data.sunlightSettings.color[3] = event->data.sunlightSettings.color[3];
								new_event->data.sunlightSettings.transStateStreamIndex = event->data.sunlightSettings.transStateStreamIndex;
								new_event->data.sunlightSettings.flags = event->data.sunlightSettings.flags;
								new_event->data.sunlightSettings.intensityScaleMin = static_cast<float>(event->data.sunlightSettings.intensityScaleMin);
								new_event->data.sunlightSettings.intensityScaleMax = static_cast<float>(event->data.sunlightSettings.intensityScaleMax);
								new_event->data.sunlightSettings.pitchMin = event->data.sunlightSettings.pitchMin;
								new_event->data.sunlightSettings.pitchMax = event->data.sunlightSettings.pitchMax;
								new_event->data.sunlightSettings.headingMin = event->data.sunlightSettings.headingMin;
								new_event->data.sunlightSettings.headingMax = event->data.sunlightSettings.headingMax;
								new_event->data.sunlightSettings.transitionTimeMin = event->data.sunlightSettings.transitionTimeMin;
								new_event->data.sunlightSettings.transitionTimeMax = event->data.sunlightSettings.transitionTimeMax;
							}
						}
					}
				}

				return new_asset;
			}

			void dump(ScriptableDef* asset)
			{
				utils::memory::allocator allocator;
				auto* converted_asset = convert(asset, allocator);
				zonetool::h1::scriptable_def::dump(converted_asset);
			}
		}
	}
}