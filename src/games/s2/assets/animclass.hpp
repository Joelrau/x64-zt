#pragma once

namespace zonetool::s2
{
	class anim_class
	{
	public:
		static void read(zone_reader& reader, AnimationClass* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto machine = reader.read_single(asset->stateMachine, 7))
			{
				read_state_machine(reader, machine);
			}
			reader.read_script_string(asset->animTree);
			reader.read_asset(ASSET_TYPE_SCRIPTABLE, asset->scriptable);
			reader.read_script_string_array(asset->soundNotes, asset->soundCount);
			reader.read_script_string_array(asset->soundNames, asset->soundCount);
			reader.read_script_string_array(asset->soundOptions, asset->soundCount);
			reader.read_script_string_array(asset->effectNotes, asset->effectCount);
			reader.read_asset_array(ASSET_TYPE_FX, asset->effects, asset->effectCount);
			reader.read_script_string_array(asset->effectTags, asset->effectCount);
			reader.pop_stream();
		}

	private:
		static void read_aim_set(zone_reader& reader, AnimationAimSet* aim_set)
		{
			reader.read_script_string(aim_set->name);
			reader.read_script_string(aim_set->rootName);
			reader.read_script_strings(aim_set->animName, 8);
		}

		static void read_state(zone_reader& reader, AnimationState* state)
		{
			reader.read_script_string(state->name);
			reader.read_script_string(state->notify);
			if (const auto aim_set = reader.read_single(state->aimSet, 7))
			{
				read_aim_set(reader, aim_set);
			}
			if (const auto entries = reader.read_array(state->animEntries, 3, state->entryCount))
			{
				for (auto i = 0; i < state->entryCount; i++)
				{
					reader.read_script_string(entries[i].animName);
					reader.read_script_string(entries[i].alias);
				}
			}
			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(state->runtime, 7, state->entryCount);
			reader.pop_stream();
		}

		static void read_state_machine(zone_reader& reader, AnimationStateMachine* machine)
		{
			reader.read_script_string(machine->name);
			if (const auto states = reader.read_array(machine->states, 7, machine->stateCount))
			{
				for (auto i = 0; i < machine->stateCount; i++)
				{
					read_state(reader, &states[i]);
				}
			}
			if (const auto aim_sets = reader.read_array(machine->aimSets, 7, machine->aimSetCount))
			{
				for (auto i = 0; i < machine->aimSetCount; i++)
				{
					read_aim_set(reader, &aim_sets[i]);
				}
			}
		}
	};
}
