#pragma once

namespace zonetool::s2
{
	class sound
	{
	public:
		static void read(zone_reader& reader, snd_alias_list_t* asset);
	};

	class sound_submix
	{
	public:
		static void read(zone_reader& reader, SoundSubmix* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto entries = reader.read_array(asset->entries, 7, asset->entryCount))
			{
				for (auto i = 0; i < asset->entryCount; i++)
				{
					reader.read_string(entries[i].name);
				}
			}
			reader.pop_stream();
		}
	};

	class snd_music_set
	{
	public:
		static void read(zone_reader& reader, SndMusicSet* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto states = reader.read_array(asset->states, 7, asset->stateCount))
			{
				for (auto i = 0u; i < asset->stateCount; i++)
				{
					read_state(reader, &states[i]);
				}
			}
			reader.pop_stream();
		}

	private:
		static void read_state(zone_reader& reader, SndMusicState* state)
		{
			reader.read_string(state->name);
			reader.read_string(state->intro.name);
			reader.read_string(state->outro.name);
			if (const auto loops = reader.read_array(state->loops, 7, state->loopCount))
			{
				for (auto i = 0u; i < state->loopCount; i++)
				{
					reader.read_string(loops[i].name);
				}
			}
		}
	};

	class equipment_snd_table
	{
	public:
		static void read(zone_reader& reader, EquipmentSndTable* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);

			const field_reader table(reader, asset);
			const auto surface_count = table.at<std::uint32_t>(8);
			const auto equipment_count = table.at<std::uint32_t>(12);
			const auto sound_count = table.at<std::uint32_t>(16);

			const auto read_sounds = [&](const field_reader& parent, const std::size_t offset)
			{
				parent.entries(offset, 16, sound_count, [](const field_reader& e, std::size_t)
				{
					e.sounds(0, 2);
				});
			};
			const auto read_sound_lists = [&](const std::size_t offset, const std::size_t count)
			{
				table.entries(offset, 8, count, [&](const field_reader& e, std::size_t)
				{
					read_sounds(e, 0);
				});
			};
			const auto read_weights = [&](const std::size_t offset)
			{
				table.entries(offset, 8, surface_count, [&](const field_reader& surface, std::size_t)
				{
					surface.entries(0, 8, equipment_count, [&](const field_reader& e, std::size_t)
					{
						e.bytes(0, 3, 12ull * sound_count);
					});
				});
			};

			table.string(0);
			table.entries(24, 8, surface_count, [](const field_reader& e, std::size_t)
			{
				e.string(0);
			});
			table.entries(32, 16, equipment_count, [](const field_reader& e, std::size_t)
			{
				e.string(0);
			});
			read_weights(40);
			read_weights(48);
			read_sound_lists(56, surface_count);
			read_sound_lists(64, equipment_count);
			read_sounds(table, 72);
			table.entries(80, 224, surface_count, [](const field_reader& e, std::size_t)
			{
				for (auto i = 0u; i < 14; i++)
				{
					e.sounds(i * 16, 2);
				}
			});

			reader.pop_stream();
		}
	};
}
