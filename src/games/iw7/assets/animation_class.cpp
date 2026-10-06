#include <std_include.hpp>
#include "animation_class.hpp"

namespace zonetool::iw7
{
	namespace
	{
		void read_runtime_indices(zone_reader& reader, unsigned __int64*& indices, const std::size_t count)
		{
			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(indices, 3, count);
			reader.pop_stream();
		}

		void read_aliases(zone_reader& reader, AnimationState& state)
		{
			if (const auto aliases = reader.read_array(state.aliasList, 7, state.aliasCount))
			{
				for (auto i = 0; i < state.aliasCount; i++)
				{
					reader.read_script_string(aliases[i].aliasName);
					reader.read_array(aliases[i].aliasInfo, 3, aliases[i].animCount);
				}
			}
		}

		void read_states(zone_reader& reader, AnimationStateMachine& machine)
		{
			const auto states = reader.read_array(machine.states, 7, machine.stateCount);
			if (!states)
			{
				return;
			}

			for (auto i = 0; i < machine.stateCount; i++)
			{
				auto& state = states[i];
				reader.read_script_string(state.name);
				reader.read_script_string(state.notify);
				if (const auto entries = reader.read_array(state.animEntries, 3, state.entryCount))
				{
					for (auto j = 0; j < state.entryCount; j++)
					{
						reader.read_script_string(entries[j].animName);
					}
				}
				read_runtime_indices(reader, state.animIndices, state.entryCount);
				read_aliases(reader, state);
			}
		}

		void read_aim_sets(zone_reader& reader, AnimationStateMachine& machine)
		{
			const auto aim_sets = reader.read_array(machine.aimSets, 7, machine.aimSetCount);
			if (!aim_sets)
			{
				return;
			}

			for (auto i = 0; i < machine.aimSetCount; i++)
			{
				auto& aim_set = aim_sets[i];
				reader.read_script_string(aim_set.name);
				reader.read_script_string(aim_set.rootName);
				reader.read_script_string_array(aim_set.animName, aim_set.animCount);
				read_runtime_indices(reader, aim_set.animIndices, aim_set.animCount);
				read_runtime_indices(reader, aim_set.aimNodeIndices, aim_set.animCount);
			}
		}
	}

	void animation_class::read(zone_reader& reader, AnimationClass* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->className);
		if (const auto machine = reader.read_single(asset->stateMachine, 7))
		{
			reader.read_script_string(machine->name);
			read_states(reader, *machine);
			read_aim_sets(reader, *machine);
		}
		reader.read_script_string(asset->animTree);
		reader.read_asset(ASSET_TYPE_SCRIPTABLE, asset->scriptable);
		reader.read_script_string_array(asset->soundNotes, asset->soundCount);
		reader.read_script_string_array(asset->soundNames, asset->soundCount);
		reader.read_script_string_array(asset->soundOptions, asset->soundCount);
		reader.read_script_string_array(asset->effectNotes, asset->effectCount);
		if (const auto effects = reader.read_array(asset->effectDefs, 7, asset->effectCount))
		{
			for (auto i = 0; i < asset->effectCount; i++)
			{
				read_fx_combined(reader, effects[i]);
			}
		}
		reader.read_script_string_array(asset->effectTags, asset->effectCount);
		reader.pop_stream();
	}
}
