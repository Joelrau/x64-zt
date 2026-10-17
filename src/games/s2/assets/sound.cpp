#include <std_include.hpp>
#include "s2.hpp"
#include "json_fields.hpp"

namespace zonetool::s2
{
	template <typename J>
	void to_json(J& data, const SpeakerLevels& speaker)
	{
		data["speaker"] = speaker.speaker;
		data["numLevels"] = speaker.numLevels;
		data["levels"] = speaker.levels;
	}

	template <typename J>
	void from_json(const J& data, SpeakerLevels& speaker)
	{
		data.at("speaker").get_to(speaker.speaker);
		data.at("numLevels").get_to(speaker.numLevels);
		data.at("levels").get_to(speaker.levels);
	}

	namespace
	{
		void read_sound_file(zone_reader& reader, SoundFile* file)
		{
			switch (file->type)
			{
			case SAT_LOADED:
				reader.read_asset(ASSET_TYPE_LOADED_SOUND, file->u.loadSnd);
				break;
			case SAT_STREAMED:
				loaded_sound::read_stream_info(reader, file->u.streamSnd.info);
				file->u.streamSnd.info.streamKey = reader.next_shared_stream_key();
				break;
			case SAT_PRIMED:
				reader.read_asset(ASSET_TYPE_LOADED_SOUND, file->u.primedSnd.loadedPart);
				loaded_sound::read_stream_info(reader, file->u.primedSnd.streamedPart);
				file->u.primedSnd.streamedPart.streamKey = reader.next_shared_stream_key();
				break;
			default:
				break;
			}
		}

		void read_alias(zone_reader& reader, snd_alias_t* alias)
		{
			reader.read_string(alias->aliasName);
			reader.read_string(alias->subtitle);
			reader.read_string(alias->mixerGroup);
			reader.read_string(alias->secondaryAliasName);
			reader.read_string(alias->chainAliasName);
			reader.read_string(alias->squelchName);

			if (const auto file = reader.read_single(alias->soundFile, 7))
			{
				read_sound_file(reader, file);
			}

			reader.read_asset(ASSET_TYPE_SOUND_CONTEXT, alias->sndContext);
			reader.read_asset(ASSET_TYPE_SOUND_CURVE, alias->volumeFalloffCurve);
			reader.read_asset(ASSET_TYPE_DIST_CURVE, alias->lpfCurve);
			reader.read_asset(ASSET_TYPE_REVERB_CURVE, alias->reverbSendCurve);

			if (const auto speaker_map = reader.read_single(alias->speakerMap, 7))
			{
				reader.read_string(speaker_map->name);
			}

			for (auto& modifier : alias->parameterModifiers)
			{
				reader.read_asset(ASSET_TYPE_ALIAS_PARAMETER_MODIFIER, modifier);
			}

			reader.read_asset(ASSET_TYPE_ALIAS_COMBAT_CONE, alias->combatCone);
		}
	}

	void sound::read(zone_reader& reader, snd_alias_list_t* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto aliases = reader.read_array(asset->head, 7, asset->count))
		{
			for (auto i = 0; i < asset->count; i++)
			{
				read_alias(reader, &aliases[i]);
			}
		}

		reader.read_array(asset->contextList, 0, asset->contextListCount);
		reader.pop_stream();
	}

	namespace
	{
		void write_stream_info(zone_buffer* buf, const LoadedSoundStreamInfo& info, LoadedSoundStreamInfo& dest)
		{
			write_string(buf, info.name, dest.name);
			write_string(buf, info.fileName, dest.fileName);
			dest.streamKey = {};
			buf->write_shared_stream_key(info.streamKey);
		}

		void write_sound_file(zone_base* zone, zone_buffer* buf, SoundFile* file, SoundFile* dest)
		{
			switch (file->type)
			{
			case SAT_LOADED:
				dest->u.loadSnd = asset_reference(zone, ASSET_TYPE_LOADED_SOUND, file->u.loadSnd);
				break;
			case SAT_STREAMED:
				write_stream_info(buf, file->u.streamSnd.info, dest->u.streamSnd.info);
				break;
			case SAT_PRIMED:
				dest->u.primedSnd.loadedPart = asset_reference(zone, ASSET_TYPE_LOADED_SOUND, file->u.primedSnd.loadedPart);
				write_stream_info(buf, file->u.primedSnd.streamedPart, dest->u.primedSnd.streamedPart);
				break;
			default:
				break;
			}
		}

		void write_alias(zone_base* zone, zone_buffer* buf, snd_alias_t* alias, snd_alias_t* dest)
		{
			write_string(buf, alias->aliasName, dest->aliasName);
			write_string(buf, alias->subtitle, dest->subtitle);
			write_string(buf, alias->mixerGroup, dest->mixerGroup);
			write_string(buf, alias->secondaryAliasName, dest->secondaryAliasName);
			write_string(buf, alias->chainAliasName, dest->chainAliasName);
			write_string(buf, alias->squelchName, dest->squelchName);

			SoundFile* file{};
			dest->soundFile = buf->write_s(7, alias->soundFile, 1, sizeof(SoundFile), &file);
			if (file)
			{
				write_sound_file(zone, buf, alias->soundFile, file);
			}

			dest->sndContext = asset_reference(zone, ASSET_TYPE_SOUND_CONTEXT, alias->sndContext);
			dest->volumeFalloffCurve = asset_reference(zone, ASSET_TYPE_SOUND_CURVE, alias->volumeFalloffCurve);
			dest->lpfCurve = asset_reference(zone, ASSET_TYPE_DIST_CURVE, alias->lpfCurve);
			dest->reverbSendCurve = asset_reference(zone, ASSET_TYPE_REVERB_CURVE, alias->reverbSendCurve);

			SpeakerMap* speaker_map{};
			dest->speakerMap = buf->write_s(7, alias->speakerMap, 1, sizeof(SpeakerMap), &speaker_map);
			if (speaker_map)
			{
				write_string(buf, alias->speakerMap->name, speaker_map->name);
			}

			for (auto i = 0u; i < std::size(alias->parameterModifiers); i++)
			{
				dest->parameterModifiers[i] = asset_reference(zone, ASSET_TYPE_ALIAS_PARAMETER_MODIFIER, alias->parameterModifiers[i]);
			}

			dest->combatCone = asset_reference(zone, ASSET_TYPE_ALIAS_COMBAT_CONE, alias->combatCone);
		}

		void parse_stream_info(assetmanager::reader& read, LoadedSoundStreamInfo& info)
		{
			info.name = read.read_string();
			info.fileName = read.read_string();
		}

		void parse_alias(assetmanager::reader& read, snd_alias_t* alias)
		{
			alias->aliasName = read.read_string();
			alias->subtitle = read.read_string();
			alias->mixerGroup = read.read_string();
			alias->secondaryAliasName = read.read_string();
			alias->chainAliasName = read.read_string();
			alias->squelchName = read.read_string();

			alias->soundFile = read.read_single<SoundFile>();
			if (const auto file = alias->soundFile)
			{
				switch (file->type)
				{
				case SAT_LOADED:
					file->u.loadSnd = read.read_asset<LoadedSound>();
					break;
				case SAT_STREAMED:
					parse_stream_info(read, file->u.streamSnd.info);
					break;
				case SAT_PRIMED:
					file->u.primedSnd.loadedPart = read.read_asset<LoadedSound>();
					parse_stream_info(read, file->u.primedSnd.streamedPart);
					break;
				default:
					break;
				}
			}

			alias->sndContext = read.read_asset<SndContext>();
			alias->volumeFalloffCurve = read.read_asset<SndCurve>();
			alias->lpfCurve = read.read_asset<DistCurve>();
			alias->reverbSendCurve = read.read_asset<SndCurve>();

			alias->speakerMap = read.read_single<SpeakerMap>();
			if (alias->speakerMap)
			{
				alias->speakerMap->name = read.read_string();
			}

			for (auto& modifier : alias->parameterModifiers)
			{
				modifier = read.read_asset<SndAliasParameterModifier>();
			}

			alias->combatCone = read.read_asset<SndAliasCombatCone>();
		}

		std::string json_path(const std::string& name)
		{
			return "sounds\\" + name + ".json";
		}

#define SOUND_FIELD(field) #field, value.field
		template <typename J>
		void visit_sound_file_json(const J& j, SoundFile& value)
		{
			j.value(SOUND_FIELD(type));
			j.value(SOUND_FIELD(exists));
			switch (value.type)
			{
			case SAT_LOADED:
				j.asset("loadedSound", value.u.loadSnd);
				break;
			case SAT_STREAMED:
				j.object("streamedPart", value.u.streamSnd.info, loaded_sound::visit_stream_info_json<J>);
				j.object("audioFormat", value.u.streamSnd.audioFormat, loaded_sound::visit_audio_format_json<J>);
				break;
			case SAT_PRIMED:
				j.asset("loadedPart", value.u.primedSnd.loadedPart);
				j.object("streamedPart", value.u.primedSnd.streamedPart, loaded_sound::visit_stream_info_json<J>);
				j.value("unknown48", value.u.primedSnd.unknown48);
				break;
			default:
				break;
			}
		}

		template <typename J>
		void visit_context_list_json(const J& j, snd_alias_context_list& value)
		{
			j.value(SOUND_FIELD(aliasOffset));
			j.value(SOUND_FIELD(count));
		}

		template <typename J>
		void visit_speaker_map_json(const J& j, SpeakerMap& value)
		{
			j.string(SOUND_FIELD(name));
			j.value(SOUND_FIELD(channelMaps));
		}

		template <typename J>
		void visit_alias_json(const J& j, snd_alias_t& value)
		{
			j.string(SOUND_FIELD(aliasName));
			j.string(SOUND_FIELD(subtitle));
			j.string(SOUND_FIELD(mixerGroup));
			j.string(SOUND_FIELD(secondaryAliasName));
			j.string(SOUND_FIELD(chainAliasName));
			j.optional_object(SOUND_FIELD(soundFile), visit_sound_file_json<J>);
			j.string(SOUND_FIELD(squelchName));
			j.value(SOUND_FIELD(unknown56));
			j.value(SOUND_FIELD(polyCount));
			j.value(SOUND_FIELD(dspBusIndex));
			j.value(SOUND_FIELD(priority));
			j.value(SOUND_FIELD(playCount));
			j.value(SOUND_FIELD(polyClass));
			j.value(SOUND_FIELD(volMin));
			j.value(SOUND_FIELD(volMax));
			j.value(SOUND_FIELD(volModIndex));
			j.value(SOUND_FIELD(pitchMin));
			j.value(SOUND_FIELD(pitchMax));
			j.value(SOUND_FIELD(distMin));
			j.value(SOUND_FIELD(distMax));
			j.value(SOUND_FIELD(velocityMin));
			j.value(SOUND_FIELD(flags));
			j.value(SOUND_FIELD(masterPriority));
			j.value(SOUND_FIELD(masterPercentage));
			j.value(SOUND_FIELD(slavePercentage));
			j.value(SOUND_FIELD(playbackPercentage));
			j.value(SOUND_FIELD(probability));
			j.value(SOUND_FIELD(variationType));
			j.asset(SOUND_FIELD(sndContext));
			j.value(SOUND_FIELD(sequence));
			j.value(SOUND_FIELD(lfePercentage));
			j.value(SOUND_FIELD(centerPercentage));
			j.value(SOUND_FIELD(startDelay));
			j.asset(SOUND_FIELD(volumeFalloffCurve));
			j.asset(SOUND_FIELD(lpfCurve));
			j.asset(SOUND_FIELD(reverbSendCurve));
			j.optional_object(SOUND_FIELD(speakerMap), visit_speaker_map_json<J>);
			j.value(SOUND_FIELD(wetMixOverride));
			j.value(SOUND_FIELD(focusPercentage));
			j.value(SOUND_FIELD(smartpanDistance2d));
			j.value(SOUND_FIELD(smartpanDistance3d));
			j.value(SOUND_FIELD(smartpanAttenuation3d));
			j.value(SOUND_FIELD(minSmartpan2dContribution));
			j.value(SOUND_FIELD(stereo3DAngle));
			j.value(SOUND_FIELD(stereo3DStart));
			j.value(SOUND_FIELD(stereo3DEnd));
			j.value(SOUND_FIELD(threshold));
			j.value(SOUND_FIELD(unknown232));
			j.value(SOUND_FIELD(unknown236));
			j.value(SOUND_FIELD(lockedLoopTime));
			j.value(SOUND_FIELD(unknown244));
			j.assets(SOUND_FIELD(parameterModifiers));
			j.value(SOUND_FIELD(parameterModifierOps));
			j.value(SOUND_FIELD(length));
			j.value(SOUND_FIELD(unknown324));
			j.asset(SOUND_FIELD(combatCone));
		}

		template <typename J>
		void visit_sound_json(const J& j, snd_alias_list_t& value)
		{
			j.string(SOUND_FIELD(name));
			j.entries("aliases", value.head, value.count, visit_alias_json<J>);
			j.entries("contextList", value.contextList, value.contextListCount, visit_context_list_json<J>);
		}
#undef SOUND_FIELD
	}

	snd_alias_list_t* sound::parse(const std::string& name, zone_memory* mem)
	{
		auto file = filesystem::file(json_path(name));
		if (file.exists())
		{
			ZONETOOL_INFO("Parsing sound \"%s\"...", name.data());

			file.open("rb");
			const auto data = json::parse(file.read_bytes(file.size()));
			file.close();

			auto* asset = mem->allocate<snd_alias_list_t>();
			visit_sound_json(json_field_parser(data, mem), *asset);
			return asset;
		}

		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_SOUND, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing sound \"%s\"...", name.data());

		auto* asset = read.read_single<snd_alias_list_t>();
		asset->name = read.read_string();
		asset->head = read.read_array<snd_alias_t>();
		for (auto i = 0; asset->head && i < asset->count; i++)
		{
			parse_alias(read, &asset->head[i]);
		}
		asset->contextList = read.read_array<snd_alias_context_list>();
		read.close();

		return asset;
	}

	void sound::load_depending(zone_base* zone)
	{
		for (auto i = 0; this->asset_->head && i < this->asset_->count; i++)
		{
			const auto& alias = this->asset_->head[i];
			if (const auto file = alias.soundFile)
			{
				if (file->type == SAT_LOADED)
				{
					add_dependency(zone, ASSET_TYPE_LOADED_SOUND, file->u.loadSnd);
				}
				else if (file->type == SAT_PRIMED)
				{
					add_dependency(zone, ASSET_TYPE_LOADED_SOUND, file->u.primedSnd.loadedPart);
				}
			}

			add_dependency(zone, ASSET_TYPE_SOUND_CONTEXT, alias.sndContext);
			add_dependency(zone, ASSET_TYPE_SOUND_CURVE, alias.volumeFalloffCurve);
			add_dependency(zone, ASSET_TYPE_DIST_CURVE, alias.lpfCurve);
			add_dependency(zone, ASSET_TYPE_REVERB_CURVE, alias.reverbSendCurve);
			for (const auto* modifier : alias.parameterModifiers)
			{
				add_dependency(zone, ASSET_TYPE_ALIAS_PARAMETER_MODIFIER, modifier);
			}
			add_dependency(zone, ASSET_TYPE_ALIAS_COMBAT_CONE, alias.combatCone);
		}
	}

	void sound::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = this->write_name(buf);

		if (data->head)
		{
			buf->align(7);
			auto* aliases = buf->write(data->head, data->count);
			for (auto i = 0; i < data->count; i++)
			{
				write_alias(zone, buf, &data->head[i], &aliases[i]);
			}
			buf->clear_pointer(&dest->head);
		}

		write_inline(buf, 0, data->contextList, data->contextListCount, dest->contextList);

		buf->pop_stream();
	}

	void sound::dump(snd_alias_list_t* asset)
	{
		ordered_json data;
		visit_sound_json(json_field_dumper(data), *asset);
		const auto str = data.dump(4);

		auto file = filesystem::file(json_path(asset->name));
		file.open("wb");
		file.write(str.data(), str.size(), 1);
		file.close();
	}

	namespace
	{
		void read_sound_set(zone_reader& reader, EquipmentSoundSet* set)
		{
			reader.read_name_reference(set->unk_0x0);
			reader.read_name_reference(set->unk_0x8);
		}

		void read_sound_sets(zone_reader& reader, EquipmentSoundSet*& field, const unsigned int count)
		{
			if (const auto sets = reader.read_array(field, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					read_sound_set(reader, &sets[i]);
				}
			}
		}

		void read_sound_set_lists(zone_reader& reader, EquipmentSoundSet**& field, const unsigned int list_count, const unsigned int sound_count)
		{
			if (const auto lists = reader.read_array(field, 7, list_count))
			{
				for (auto i = 0u; i < list_count; i++)
				{
					read_sound_sets(reader, lists[i], sound_count);
				}
			}
		}

		void read_weights(zone_reader& reader, vec3_t***& field, const EquipmentSndTable* table)
		{
			if (const auto surfaces = reader.read_array(field, 7, table->surfaceCount))
			{
				for (auto i = 0u; i < table->surfaceCount; i++)
				{
					if (const auto equipment = reader.read_array(surfaces[i], 7, table->equipmentCount))
					{
						for (auto j = 0u; j < table->equipmentCount; j++)
						{
							reader.read_array(equipment[j], 3, table->soundCount);
						}
					}
				}
			}
		}

		void write_sound_set(zone_buffer* buf, const EquipmentSoundSet* set, EquipmentSoundSet* dest)
		{
			write_sound(buf, set->unk_0x0, dest->unk_0x0);
			write_sound(buf, set->unk_0x8, dest->unk_0x8);
		}

		void write_sound_sets(zone_buffer* buf, EquipmentSoundSet* sets, const unsigned int count, EquipmentSoundSet*& dest_field)
		{
			if (const auto dest = write_array(buf, 7, sets, count, dest_field))
			{
				for (auto i = 0u; i < count; i++)
				{
					write_sound_set(buf, &sets[i], &dest[i]);
				}
			}
		}

		void write_sound_set_lists(zone_buffer* buf, EquipmentSoundSet** lists, const unsigned int list_count, const unsigned int sound_count, EquipmentSoundSet**& dest_field)
		{
			if (const auto dest = write_array(buf, 7, lists, list_count, dest_field))
			{
				for (auto i = 0u; i < list_count; i++)
				{
					write_sound_sets(buf, lists[i], sound_count, dest[i]);
				}
			}
		}

		void write_weights(zone_buffer* buf, vec3_t*** weights, const EquipmentSndTable* table, vec3_t***& dest_field)
		{
			if (const auto dest_surfaces = write_array(buf, 7, weights, table->surfaceCount, dest_field))
			{
				for (auto i = 0u; i < table->surfaceCount; i++)
				{
					if (const auto dest_equipment = write_array(buf, 7, weights[i], table->equipmentCount, dest_surfaces[i]))
					{
						for (auto j = 0u; j < table->equipmentCount; j++)
						{
							write_array(buf, 3, weights[i][j], table->soundCount, dest_equipment[j]);
						}
					}
				}
			}
		}

		void parse_sound_set(assetmanager::reader& read, EquipmentSoundSet* set)
		{
			set->unk_0x0 = read.read_asset<snd_alias_list_t>();
			set->unk_0x8 = read.read_asset<snd_alias_list_t>();
		}

		void parse_sound_sets(assetmanager::reader& read, zone_memory* mem, EquipmentSoundSet*& field, const unsigned int count)
		{
			if (const auto sets = parse_array(read, mem, field, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					parse_sound_set(read, &sets[i]);
				}
			}
		}

		void parse_sound_set_lists(assetmanager::reader& read, zone_memory* mem, EquipmentSoundSet**& field, const unsigned int list_count, const unsigned int sound_count)
		{
			if (const auto lists = parse_array(read, mem, field, list_count))
			{
				for (auto i = 0u; i < list_count; i++)
				{
					parse_sound_sets(read, mem, lists[i], sound_count);
				}
			}
		}

		void parse_weights(assetmanager::reader& read, zone_memory* mem, vec3_t***& field, const EquipmentSndTable* table)
		{
			if (const auto surfaces = parse_array(read, mem, field, table->surfaceCount))
			{
				for (auto i = 0u; i < table->surfaceCount; i++)
				{
					if (const auto equipment = parse_array(read, mem, surfaces[i], table->equipmentCount))
					{
						for (auto j = 0u; j < table->equipmentCount; j++)
						{
							parse_array(read, mem, equipment[j], table->soundCount);
						}
					}
				}
			}
		}

		void dump_sound_set(assetmanager::dumper& write, const EquipmentSoundSet* set)
		{
			write.dump_asset(set->unk_0x0);
			write.dump_asset(set->unk_0x8);
		}

		void dump_sound_sets(assetmanager::dumper& write, EquipmentSoundSet* sets, const unsigned int count)
		{
			write.dump_array(sets, count);
			for (auto i = 0u; sets && i < count; i++)
			{
				dump_sound_set(write, &sets[i]);
			}
		}

		void dump_sound_set_lists(assetmanager::dumper& write, EquipmentSoundSet** lists, const unsigned int list_count, const unsigned int sound_count)
		{
			write.dump_array(lists, list_count);
			for (auto i = 0u; lists && i < list_count; i++)
			{
				dump_sound_sets(write, lists[i], sound_count);
			}
		}

		void dump_weights(assetmanager::dumper& write, vec3_t*** weights, const EquipmentSndTable* table)
		{
			write.dump_array(weights, table->surfaceCount);
			for (auto i = 0u; weights && i < table->surfaceCount; i++)
			{
				write.dump_array(weights[i], table->equipmentCount);
				for (auto j = 0u; weights[i] && j < table->equipmentCount; j++)
				{
					write.dump_array(weights[i][j], table->soundCount);
				}
			}
		}

		void add_sound_set_dependencies(zone_base* zone, const EquipmentSoundSet* sets, const std::size_t count)
		{
			for (std::size_t i = 0; sets && i < count; i++)
			{
				add_dependency(zone, ASSET_TYPE_SOUND, sets[i].unk_0x0);
				add_dependency(zone, ASSET_TYPE_SOUND, sets[i].unk_0x8);
			}
		}
	}

	void equipment_snd_table::read(zone_reader& reader, EquipmentSndTable* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto names = reader.read_array(asset->surfaceNames, 7, asset->surfaceCount))
		{
			for (auto i = 0u; i < asset->surfaceCount; i++)
			{
				reader.read_string(names[i]);
			}
		}
		if (const auto equipment = reader.read_array(asset->equipment, 7, asset->equipmentCount))
		{
			for (auto i = 0u; i < asset->equipmentCount; i++)
			{
				reader.read_string(equipment[i].name);
			}
		}
		read_weights(reader, asset->unk_0x28, asset);
		read_weights(reader, asset->unk_0x30, asset);
		read_sound_set_lists(reader, asset->surfaceSounds, asset->surfaceCount, asset->soundCount);
		read_sound_set_lists(reader, asset->equipmentSounds, asset->equipmentCount, asset->soundCount);
		read_sound_sets(reader, asset->sounds, asset->soundCount);
		if (const auto surface_sounds = reader.read_array(asset->unk_0x50, 7, asset->surfaceCount))
		{
			for (auto i = 0u; i < asset->surfaceCount; i++)
			{
				for (auto& set : surface_sounds[i].sets)
				{
					read_sound_set(reader, &set);
				}
			}
		}
		reader.pop_stream();
	}

	EquipmentSndTable* equipment_snd_table::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_EQUIPMENT_SND_TABLE, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing equipmentsndtable \"%s\"...", name.data());

		auto* asset = read.read_single<EquipmentSndTable>();
		asset->name = read.read_string();
		if (const auto names = parse_array(read, mem, asset->surfaceNames, asset->surfaceCount))
		{
			for (auto i = 0u; i < asset->surfaceCount; i++)
			{
				names[i] = read.read_string();
			}
		}
		if (const auto equipment = parse_array(read, mem, asset->equipment, asset->equipmentCount))
		{
			for (auto i = 0u; i < asset->equipmentCount; i++)
			{
				equipment[i].name = read.read_string();
			}
		}
		parse_weights(read, mem, asset->unk_0x28, asset);
		parse_weights(read, mem, asset->unk_0x30, asset);
		parse_sound_set_lists(read, mem, asset->surfaceSounds, asset->surfaceCount, asset->soundCount);
		parse_sound_set_lists(read, mem, asset->equipmentSounds, asset->equipmentCount, asset->soundCount);
		parse_sound_sets(read, mem, asset->sounds, asset->soundCount);
		if (const auto surface_sounds = parse_array(read, mem, asset->unk_0x50, asset->surfaceCount))
		{
			for (auto i = 0u; i < asset->surfaceCount; i++)
			{
				for (auto& set : surface_sounds[i].sets)
				{
					parse_sound_set(read, &set);
				}
			}
		}
		read.close();

		return asset;
	}

	void equipment_snd_table::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;
		for (auto i = 0u; data->surfaceSounds && i < data->surfaceCount; i++)
		{
			add_sound_set_dependencies(zone, data->surfaceSounds[i], data->soundCount);
		}
		for (auto i = 0u; data->equipmentSounds && i < data->equipmentCount; i++)
		{
			add_sound_set_dependencies(zone, data->equipmentSounds[i], data->soundCount);
		}
		add_sound_set_dependencies(zone, data->sounds, data->soundCount);
		for (auto i = 0u; data->unk_0x50 && i < data->surfaceCount; i++)
		{
			add_sound_set_dependencies(zone, data->unk_0x50[i].sets, std::size(data->unk_0x50[i].sets));
		}
	}

	void equipment_snd_table::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = this->write_name(buf);
		if (const auto dest_names = write_array(buf, 7, data->surfaceNames, data->surfaceCount, dest->surfaceNames))
		{
			for (auto i = 0u; i < data->surfaceCount; i++)
			{
				write_string(buf, data->surfaceNames[i], dest_names[i]);
			}
		}
		if (const auto dest_equipment = write_array(buf, 7, data->equipment, data->equipmentCount, dest->equipment))
		{
			for (auto i = 0u; i < data->equipmentCount; i++)
			{
				write_string(buf, data->equipment[i].name, dest_equipment[i].name);
			}
		}
		write_weights(buf, data->unk_0x28, data, dest->unk_0x28);
		write_weights(buf, data->unk_0x30, data, dest->unk_0x30);
		write_sound_set_lists(buf, data->surfaceSounds, data->surfaceCount, data->soundCount, dest->surfaceSounds);
		write_sound_set_lists(buf, data->equipmentSounds, data->equipmentCount, data->soundCount, dest->equipmentSounds);
		write_sound_sets(buf, data->sounds, data->soundCount, dest->sounds);
		if (const auto dest_surface_sounds = write_array(buf, 7, data->unk_0x50, data->surfaceCount, dest->unk_0x50))
		{
			for (auto i = 0u; i < data->surfaceCount; i++)
			{
				for (auto j = 0u; j < std::size(data->unk_0x50[i].sets); j++)
				{
					write_sound_set(buf, &data->unk_0x50[i].sets[j], &dest_surface_sounds[i].sets[j]);
				}
			}
		}
		buf->pop_stream();
	}

	void equipment_snd_table::dump(EquipmentSndTable* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_EQUIPMENT_SND_TABLE, asset->name)))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_array(asset->surfaceNames, asset->surfaceCount);
		for (auto i = 0u; asset->surfaceNames && i < asset->surfaceCount; i++)
		{
			write.dump_string(asset->surfaceNames[i]);
		}
		write.dump_array(asset->equipment, asset->equipmentCount);
		for (auto i = 0u; asset->equipment && i < asset->equipmentCount; i++)
		{
			write.dump_string(asset->equipment[i].name);
		}
		dump_weights(write, asset->unk_0x28, asset);
		dump_weights(write, asset->unk_0x30, asset);
		dump_sound_set_lists(write, asset->surfaceSounds, asset->surfaceCount, asset->soundCount);
		dump_sound_set_lists(write, asset->equipmentSounds, asset->equipmentCount, asset->soundCount);
		dump_sound_sets(write, asset->sounds, asset->soundCount);
		write.dump_array(asset->unk_0x50, asset->surfaceCount);
		for (auto i = 0u; asset->unk_0x50 && i < asset->surfaceCount; i++)
		{
			for (auto& set : asset->unk_0x50[i].sets)
			{
				dump_sound_set(write, &set);
			}
		}
		write.close();
	}
}
