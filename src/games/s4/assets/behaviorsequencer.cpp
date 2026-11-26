#include <std_include.hpp>
#include "behaviorsequencer.hpp"

namespace zonetool::s4
{
	namespace
	{
		void read_value(zone_reader& reader, BehaviorSequencerValue& value)
		{
			if (value.type == 1)
			{
				reader.read_script_string(value.string);
			}
		}

		void read_values(zone_reader& reader, BehaviorSequencerValue*& field, const int count)
		{
			if (const auto values = reader.read_array(field, 3, count))
			{
				for (auto i = 0; i < count; i++)
				{
					read_value(reader, values[i]);
				}
			}
		}

		void read_action(zone_reader& reader, BehaviorSequencerAction& action)
		{
			auto& data = action.data;
			switch (action.type)
			{
			case 2:
				for (auto& list : data.type2.lists)
				{
					reader.read_script_string(list.name);
					read_values(reader, list.values, list.valueCount);
				}
				break;
			case 3:
				reader.read_script_string(data.type3.name);
				break;
			case 4:
				reader.read_script_string(data.type4.strings[0].name);
				reader.read_script_string(data.type4.strings[1].name);
				reader.read_script_string(data.type4.name0);
				reader.read_script_string(data.type4.name1);
				break;
			case 5:
				reader.read_script_string(data.type5.name0);
				reader.read_script_string(data.type5.name1);
				break;
			case 6:
				for (auto& list : data.type6.lists)
				{
					read_values(reader, list.values, list.valueCount);
					reader.read_script_string(list.name);
				}
				break;
			}
		}
	}

	void behavior_sequencer::read(zone_reader& reader, BehaviorSequencer* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		reader.read_script_string(asset->unknown0);

		if (const auto entries = reader.read_array(asset->entries, 3, asset->entryCount))
		{
			for (auto i = 0; i < asset->entryCount; i++)
			{
				reader.read_script_string(entries[i].name);
				read_value(reader, entries[i].value);
			}
		}

		if (const auto strings = reader.read_array(asset->strings, 3, asset->stringCount))
		{
			for (auto i = 0; i < asset->stringCount; i++)
			{
				reader.read_script_string(strings[i].name);
			}
		}

		if (const auto states = reader.read_array(asset->states, 7, asset->stateCount))
		{
			for (auto i = 0; i < asset->stateCount; i++)
			{
				reader.read_script_strings(states[i].keys.keys, 4);
				read_action(reader, states[i].action);
				reader.read_script_string(states[i].name);
			}
		}

		reader.read_array(asset->unknown3, 3, asset->unknown3Count);
		reader.read_array(asset->vectors, 3, asset->vectorCount);

		if (const auto events = reader.read_array(asset->events, 3, asset->eventCount))
		{
			for (auto i = 0; i < asset->eventCount; i++)
			{
				if (events[i].type == 0 || events[i].type == 1 || events[i].type == 2)
				{
					reader.read_script_string(events[i].name.name);
				}
			}
		}

		reader.read_script_string(asset->unknown1);
		reader.read_script_string(asset->unknown2);
		reader.pop_stream();
	}
}
