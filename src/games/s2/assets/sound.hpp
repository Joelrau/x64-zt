#pragma once

namespace zonetool::s2
{
	class sound : public asset_base<snd_alias_list_t, ASSET_TYPE_SOUND>
	{
	public:
		static void read(zone_reader& reader, snd_alias_list_t* asset);

		snd_alias_list_t* parse(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(snd_alias_list_t* asset);
	};

	class sound_submix : public asset_base<SoundSubmix, ASSET_TYPE_SOUNDSUBMIX>
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

		SoundSubmix* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_SOUNDSUBMIX, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing soundsubmix \"%s\"...", name.data());

			auto* asset = read.read_single<SoundSubmix>();
			asset->name = read.read_string();
			asset->entries = read.read_array<SoundSubmixEntry>();
			for (auto i = 0; asset->entries && i < asset->entryCount; i++)
			{
				asset->entries[i].name = read.read_string();
			}
			read.close();

			return asset;
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);
			if (data->entries)
			{
				buf->align(7);
				auto* entries = buf->write(data->entries, data->entryCount);
				for (auto i = 0; i < data->entryCount; i++)
				{
					write_string(buf, data->entries[i].name, entries[i].name);
				}
				buf->clear_pointer(&dest->entries);
			}

			buf->pop_stream();
		}

		static void dump(SoundSubmix* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_SOUNDSUBMIX, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_array(asset->entries, asset->entryCount);
			for (auto i = 0; asset->entries && i < asset->entryCount; i++)
			{
				write.dump_string(asset->entries[i].name);
			}
			write.close();
		}
	};

	class snd_music_set : public asset_base<SndMusicSet, ASSET_TYPE_SNDMUSICSET>
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

		SndMusicSet* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_SNDMUSICSET, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing sndmusicset \"%s\"...", name.data());

			auto* asset = read.read_single<SndMusicSet>();
			asset->name = read.read_string();
			asset->states = read.read_array<SndMusicState>();
			for (auto i = 0u; asset->states && i < asset->stateCount; i++)
			{
				auto& state = asset->states[i];
				state.name = read.read_string();
				state.intro.name = read.read_string();
				state.outro.name = read.read_string();
				state.loops = read.read_array<SndMusicSubState>();
				for (auto j = 0u; state.loops && j < state.loopCount; j++)
				{
					state.loops[j].name = read.read_string();
				}
			}
			read.close();

			return asset;
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);
			if (data->states)
			{
				buf->align(7);
				auto* states = buf->write(data->states, data->stateCount);
				for (auto i = 0u; i < data->stateCount; i++)
				{
					const auto& state = data->states[i];
					write_string(buf, state.name, states[i].name);
					write_string(buf, state.intro.name, states[i].intro.name);
					write_string(buf, state.outro.name, states[i].outro.name);
					if (state.loops)
					{
						buf->align(7);
						auto* loops = buf->write(state.loops, state.loopCount);
						for (auto j = 0u; j < state.loopCount; j++)
						{
							write_string(buf, state.loops[j].name, loops[j].name);
						}
						buf->clear_pointer(&states[i].loops);
					}
				}
				buf->clear_pointer(&dest->states);
			}

			buf->pop_stream();
		}

		static void dump(SndMusicSet* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_SNDMUSICSET, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_array(asset->states, asset->stateCount);
			for (auto i = 0u; asset->states && i < asset->stateCount; i++)
			{
				const auto& state = asset->states[i];
				write.dump_string(state.name);
				write.dump_string(state.intro.name);
				write.dump_string(state.outro.name);
				write.dump_array(state.loops, state.loopCount);
				for (auto j = 0u; state.loops && j < state.loopCount; j++)
				{
					write.dump_string(state.loops[j].name);
				}
			}
			write.close();
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

	class equipment_snd_table : public asset_base<EquipmentSndTable, ASSET_TYPE_EQUIPMENT_SND_TABLE>
	{
	public:
		static void read(zone_reader& reader, EquipmentSndTable* asset);
		EquipmentSndTable* parse(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(EquipmentSndTable* asset);
	};
}
