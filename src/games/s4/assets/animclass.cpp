#include <std_include.hpp>
#include "animclass.hpp"

namespace zonetool::s4
{
	namespace
	{
		void read_state(zone_reader& reader, AnimationState* state)
		{
			reader.read_script_string(state->name);
			reader.read_script_string(state->notify);
			if (const auto entries = reader.read_array(state->animEntries, 3, state->entryCount))
			{
				for (auto i = 0u; i < state->entryCount; i++)
				{
					reader.read_script_string(entries[i].animName);
				}
			}

			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(state->animIndices, 7, state->entryCount);
			reader.pop_stream();

			if (const auto aliases = reader.read_array(state->aliasList, 7, state->aliasCount))
			{
				for (auto i = 0u; i < state->aliasCount; i++)
				{
					reader.read_script_string(aliases[i].aliasName);
					reader.read_array(aliases[i].aliasInfo, 3, aliases[i].animCount);
				}
			}
		}

		void read_aim_set(zone_reader& reader, AnimationAimSet* aim_set)
		{
			reader.read_script_string(aim_set->name);
			reader.read_script_string(aim_set->rootName);
			reader.read_script_string_array(aim_set->animName, aim_set->animCount);

			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(aim_set->animIndices, 7, aim_set->animCount);
			reader.pop_stream();

			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(aim_set->aimNodeIndices, 7, aim_set->animCount);
			reader.pop_stream();
		}

		void read_state_machine(zone_reader& reader, AnimationStateMachine* state_machine)
		{
			reader.read_script_string(state_machine->name);
			if (const auto states = reader.read_array(state_machine->states, 7, state_machine->stateCount))
			{
				for (auto i = 0u; i < state_machine->stateCount; i++)
				{
					read_state(reader, &states[i]);
				}
			}

			if (const auto aim_sets = reader.read_array(state_machine->aimSets, 7, state_machine->aimSetCount))
			{
				for (auto i = 0u; i < state_machine->aimSetCount; i++)
				{
					read_aim_set(reader, &aim_sets[i]);
				}
			}
		}
	}

	void anim_class::read(zone_reader& reader, AnimationClass* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->className);
		if (const auto state_machine = reader.read_single(asset->stateMachine, 7))
		{
			read_state_machine(reader, state_machine);
		}

		reader.read_script_string(asset->animTree);
		reader.read_script_string_array(asset->soundNotes, asset->soundCount);
		reader.read_script_string_array(asset->soundNames, asset->soundCount);
		reader.read_script_string_array(asset->soundOptions, asset->soundCount);
		reader.read_script_string_array(asset->effectNotes, asset->effectCount);
		if (const auto effects = reader.read_array(asset->effectDefs, 7, asset->effectCount))
		{
			for (auto i = 0u; i < asset->effectCount; i++)
			{
				reader.read_asset(ASSET_TYPE_VFX, effects[i].particleSystemDef);
			}
		}

		reader.read_script_string_array(asset->effectTags, asset->effectCount);
		reader.pop_stream();
	}
}
