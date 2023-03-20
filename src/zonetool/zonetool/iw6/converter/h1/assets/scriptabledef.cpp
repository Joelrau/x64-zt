#include <std_include.hpp>
#include "zonetool/iw6/converter/include.hpp"
#include "zonetool/iw6/converter/h1/include.hpp"
#include "scriptabledef.hpp"

#include "zonetool/h1/assets/scriptabledef.hpp"

#include "zonetool/iw6/zonetool.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace scriptabledef
		{
			zonetool::h1::ScriptableDef* convert(ScriptableDef* asset, ZoneMemory* mem)
			{
				auto* new_asset = mem->Alloc<zonetool::h1::ScriptableDef>();

				// the only thing different about h1 scriptabledef is that there is extra type of ScriptableEventType
				// ScriptableEventSunlightSettingsDef also changed, that may cause issues...

				std::memcpy(new_asset, asset, sizeof(zonetool::h1::ScriptableDef)); // same struct

				new_asset->parts = mem->Alloc<zonetool::h1::ScriptablePartDef>(asset->partCount);
				for (unsigned char i = 0; i < asset->partCount; i++)
				{
					auto* part = &asset->parts[i];
					auto* new_part = &new_asset->parts[i];

					std::memcpy(new_part, part, sizeof(zonetool::h1::ScriptablePartDef)); // same struct

					new_part->states = mem->Alloc<zonetool::h1::ScriptableStateDef>(part->stateCount);
					for (unsigned char j = 0; j < part->stateCount; j++)
					{
						auto* state = &part->states[j];
						auto* new_state = &new_part->states[j];

						std::memcpy(new_state, state, sizeof(zonetool::h1::ScriptableStateDef)); // same struct

						new_state->onEnterEvents = mem->Alloc<zonetool::h1::ScriptableEventDef>(state->onEnterEventCount);
						for (unsigned char k = 0; k < state->onEnterEventCount; k++)
						{
							auto* eevent = &state->onEnterEvents[k];
							auto* new_eevent = &new_state->onEnterEvents[k];

							std::memcpy(new_eevent, eevent, sizeof(zonetool::h1::ScriptableEventDef)); // same stuff, mostly

							// 3 new types, skip them
							const auto type = static_cast<int>(new_eevent->type);
							auto new_type = static_cast<int>(new_eevent->type);
							if (type > 1) new_type++;
							if (type > 9) new_type += 2;

							new_eevent->type = static_cast<zonetool::h1::ScriptableEventType>(new_type);

							// lightSettings need fixing?
						}
					}
				}

				return new_asset;
			}

			void dump(ScriptableDef* asset, ZoneMemory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::IScriptableDef::dump(converted_asset, 
					reinterpret_cast<decltype(zonetool::h1::SL_ConvertToString.get())>(zonetool::iw6::SL_ConvertToString.get()));
			}
		}
	}
}