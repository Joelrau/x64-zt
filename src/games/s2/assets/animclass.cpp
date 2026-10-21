#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		void read_aim_set(zone_reader& reader, AnimationAimSet* aim_set)
		{
			reader.read_script_string(aim_set->name);
			reader.read_script_string(aim_set->rootName);
			reader.read_script_strings(aim_set->animName, std::size(aim_set->animName));
		}

		void read_state(zone_reader& reader, AnimationState* state)
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

			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, state->runtime, 7, state->entryCount);
		}

		void read_state_machine(zone_reader& reader, AnimationStateMachine* machine)
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

		void parse_aim_set(assetmanager::reader& read, AnimationAimSet* aim_set)
		{
			aim_set->name = parse_script_string(read);
			aim_set->rootName = parse_script_string(read);
			for (auto& anim_name : aim_set->animName)
			{
				anim_name = parse_script_string(read);
			}
		}

		void parse_state(assetmanager::reader& read, zone_memory* mem, AnimationState* state)
		{
			state->name = parse_script_string(read);
			state->notify = parse_script_string(read);
			if (const auto aim_set = parse_array(read, mem, state->aimSet, 1))
			{
				parse_aim_set(read, aim_set);
			}

			if (const auto entries = parse_array(read, mem, state->animEntries, state->entryCount))
			{
				for (auto i = 0; i < state->entryCount; i++)
				{
					entries[i].animName = parse_script_string(read);
					entries[i].alias = parse_script_string(read);
				}
			}

			parse_runtime_array(mem, state->runtime, state->entryCount);
		}

		void parse_state_machine(assetmanager::reader& read, zone_memory* mem, AnimationStateMachine* machine)
		{
			machine->name = parse_script_string(read);
			if (const auto states = parse_array(read, mem, machine->states, machine->stateCount))
			{
				for (auto i = 0; i < machine->stateCount; i++)
				{
					parse_state(read, mem, &states[i]);
				}
			}

			if (const auto aim_sets = parse_array(read, mem, machine->aimSets, machine->aimSetCount))
			{
				for (auto i = 0; i < machine->aimSetCount; i++)
				{
					parse_aim_set(read, &aim_sets[i]);
				}
			}
		}

		void prepare_aim_set(script_string_preparer& prepare, AnimationAimSet* aim_set)
		{
			prepare(aim_set->name);
			prepare(aim_set->rootName);
			prepare(aim_set->animName, std::size(aim_set->animName));
		}

		void prepare_state_machine(script_string_preparer& prepare, AnimationStateMachine* machine)
		{
			prepare(machine->name);
			for (auto i = 0; machine->states && i < machine->stateCount; i++)
			{
				auto* state = &machine->states[i];
				prepare(state->name);
				prepare(state->notify);
				if (state->aimSet)
				{
					prepare_aim_set(prepare, state->aimSet);
				}

				for (auto j = 0; state->animEntries && j < state->entryCount; j++)
				{
					prepare(state->animEntries[j].animName);
					prepare(state->animEntries[j].alias);
				}
			}

			for (auto i = 0; machine->aimSets && i < machine->aimSetCount; i++)
			{
				prepare_aim_set(prepare, &machine->aimSets[i]);
			}
		}

		void write_state_machine(zone_buffer* buf, AnimationStateMachine* machine, AnimationStateMachine* dest)
		{
			if (const auto dest_states = write_array(buf, 7, machine->states, machine->stateCount, dest->states))
			{
				for (auto i = 0; i < machine->stateCount; i++)
				{
					auto* state = &machine->states[i];
					write_array(buf, 7, state->aimSet, 1, dest_states[i].aimSet);
					write_array(buf, 3, state->animEntries, state->entryCount, dest_states[i].animEntries);
					write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 7, state->runtime, state->entryCount, dest_states[i].runtime);
				}
			}

			write_array(buf, 7, machine->aimSets, machine->aimSetCount, dest->aimSets);
		}

		void dump_aim_set(assetmanager::dumper& write, AnimationAimSet* aim_set)
		{
			dump_script_string(write, aim_set->name);
			dump_script_string(write, aim_set->rootName);
			dump_script_strings(write, aim_set->animName, std::size(aim_set->animName));
		}

		void dump_state_machine(assetmanager::dumper& write, AnimationStateMachine* machine)
		{
			dump_script_string(write, machine->name);
			write.dump_array(machine->states, machine->stateCount);
			for (auto i = 0; machine->states && i < machine->stateCount; i++)
			{
				auto* state = &machine->states[i];
				dump_script_string(write, state->name);
				dump_script_string(write, state->notify);
				write.dump_single(state->aimSet);
				if (state->aimSet)
				{
					dump_aim_set(write, state->aimSet);
				}

				write.dump_array(state->animEntries, state->entryCount);
				for (auto j = 0; state->animEntries && j < state->entryCount; j++)
				{
					dump_script_string(write, state->animEntries[j].animName);
					dump_script_string(write, state->animEntries[j].alias);
				}
			}

			write.dump_array(machine->aimSets, machine->aimSetCount);
			for (auto i = 0; machine->aimSets && i < machine->aimSetCount; i++)
			{
				dump_aim_set(write, &machine->aimSets[i]);
			}
		}
	}

	void anim_class::read(zone_reader& reader, AnimationClass* asset)
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

	AnimationClass* anim_class::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_ANIMCLASS, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing animclass \"%s\"...", name.data());

		auto* asset = read.read_single<AnimationClass>();
		asset->name = read.read_string();
		if (const auto machine = parse_array(read, mem, asset->stateMachine, 1))
		{
			parse_state_machine(read, mem, machine);
		}
		asset->animTree = parse_script_string(read);
		asset->scriptable = read.read_asset<ScriptableDef>();
		parse_script_string_array(read, mem, asset->soundNotes, asset->soundCount);
		parse_script_string_array(read, mem, asset->soundNames, asset->soundCount);
		parse_script_string_array(read, mem, asset->soundOptions, asset->soundCount);
		parse_script_string_array(read, mem, asset->effectNotes, asset->effectCount);
		parse_asset_array(read, mem, asset->effects, asset->effectCount);
		parse_script_string_array(read, mem, asset->effectTags, asset->effectCount);
		read.close();

		return asset;
	}

	void anim_class::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;
		script_string_preparer prepare(buf);

		if (data->stateMachine)
		{
			prepare_state_machine(prepare, data->stateMachine);
		}
		prepare(data->animTree);
		prepare(data->soundNotes, data->soundCount);
		prepare(data->soundNames, data->soundCount);
		prepare(data->soundOptions, data->soundCount);
		prepare(data->effectNotes, data->effectCount);
		prepare(data->effectTags, data->effectCount);
	}

	void anim_class::load_depending(zone_base* zone)
	{
		add_dependency(zone, ASSET_TYPE_SCRIPTABLE, this->asset_->scriptable);
		add_dependencies(zone, ASSET_TYPE_FX, this->asset_->effects, this->asset_->effectCount);
	}

	void anim_class::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = this->write_name(buf);
		if (const auto dest_machine = write_array(buf, 7, data->stateMachine, 1, dest->stateMachine))
		{
			write_state_machine(buf, data->stateMachine, dest_machine);
		}
		dest->scriptable = asset_reference(zone, ASSET_TYPE_SCRIPTABLE, data->scriptable);
		write_array(buf, 3, data->soundNotes, data->soundCount, dest->soundNotes);
		write_array(buf, 3, data->soundNames, data->soundCount, dest->soundNames);
		write_array(buf, 3, data->soundOptions, data->soundCount, dest->soundOptions);
		write_array(buf, 3, data->effectNotes, data->effectCount, dest->effectNotes);
		write_asset_array(zone, buf, ASSET_TYPE_FX, data->effects, data->effectCount, dest->effects);
		write_array(buf, 3, data->effectTags, data->effectCount, dest->effectTags);

		buf->pop_stream();
	}

	void anim_class::dump(AnimationClass* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_ANIMCLASS, asset->name)))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_single(asset->stateMachine);
		if (asset->stateMachine)
		{
			dump_state_machine(write, asset->stateMachine);
		}
		dump_script_string(write, asset->animTree);
		write.dump_asset(asset->scriptable);
		dump_script_string_array(write, asset->soundNotes, asset->soundCount);
		dump_script_string_array(write, asset->soundNames, asset->soundCount);
		dump_script_string_array(write, asset->soundOptions, asset->soundCount);
		dump_script_string_array(write, asset->effectNotes, asset->effectCount);
		dump_asset_array(write, asset->effects, asset->effectCount);
		dump_script_string_array(write, asset->effectTags, asset->effectCount);
		write.close();
	}
}
