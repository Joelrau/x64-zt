#include <std_include.hpp>
#include "s2.hpp"
#include "common/json_fields.hpp"

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
			reader.read_string(alias->subtitleCharacterName);
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

		void write_alias(zone_base* zone, zone_buffer* buf, snd_alias_t* alias, snd_alias_t* dest, const char* list_name, const char* list_name_ptr)
		{
			// stock zones point every alias name at the list name
			if (alias->aliasName && list_name && !std::strcmp(alias->aliasName, list_name))
			{
				dest->aliasName = list_name_ptr;
			}
			else
			{
				write_string(buf, alias->aliasName, dest->aliasName);
			}

			write_string(buf, alias->subtitle, dest->subtitle);
			write_string(buf, alias->subtitleCharacterName, dest->subtitleCharacterName);
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
			alias->subtitleCharacterName = read.read_string();
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
				j.value("dataOffset", value.u.primedSnd.dataOffset);
				j.value("unused52", value.u.primedSnd.unused52);
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
			j.string(SOUND_FIELD(subtitleCharacterName));
			j.string(SOUND_FIELD(secondaryAliasName));
			j.string(SOUND_FIELD(chainAliasName));
			j.optional_object(SOUND_FIELD(soundFile), visit_sound_file_json<J>);
			j.string(SOUND_FIELD(squelchName));
			j.value(SOUND_FIELD(subtitleFlags));
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
			j.value(SOUND_FIELD(normalizeLevel));
			j.value(SOUND_FIELD(normalize));
			j.value(SOUND_FIELD(lockedLoopTime));
			j.value(SOUND_FIELD(unused244));
			j.assets(SOUND_FIELD(parameterModifiers));
			j.value(SOUND_FIELD(parameterModifierOps));
			j.value(SOUND_FIELD(length));
			j.value(SOUND_FIELD(unused324));
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

		const auto* name_ptr = buf->get_zone_pointer<const char>(buf->current_stream(), buf->current_stream_offset());
		dest->name = this->write_name(buf);
		if (dest->name != reinterpret_cast<const char*>(buf->data_following))
		{
			name_ptr = dest->name; // the name was already in the zone
		}

		if (data->head)
		{
			buf->align(7);
			auto* aliases = buf->write(data->head, data->count);
			for (auto i = 0; i < data->count; i++)
			{
				write_alias(zone, buf, &data->head[i], &aliases[i], this->name_.data(), name_ptr);
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
			reader.read_name_reference(set->soundPLR);
			reader.read_name_reference(set->soundNPC);
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

		void read_move_types_array(zone_reader& reader, EquipSoundSetMoveTypes*& field, const unsigned int count, const unsigned int move_count)
		{
			if (const auto types = reader.read_array(field, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					read_sound_sets(reader, types[i].soundSets, move_count);
				}
			}
		}

		void read_chances(zone_reader& reader, EquipmentChanceRattleTypes*& field, const EquipmentSoundTable* table)
		{
			if (const auto cloth_types = reader.read_array(field, 7, table->numClothTypes))
			{
				for (auto i = 0u; i < table->numClothTypes; i++)
				{
					if (const auto rattle_types = reader.read_array(cloth_types[i].chances, 7, table->numWeaponRattleTypes))
					{
						for (auto j = 0u; j < table->numWeaponRattleTypes; j++)
						{
							reader.read_array(rattle_types[j].chances, 3, table->numMoveTypes);
						}
					}
				}
			}
		}

		void write_sound_set(zone_buffer* buf, const EquipmentSoundSet* set, EquipmentSoundSet* dest)
		{
			write_sound(buf, set->soundPLR, dest->soundPLR);
			write_sound(buf, set->soundNPC, dest->soundNPC);
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

		void write_move_types_array(zone_buffer* buf, EquipSoundSetMoveTypes* types, const unsigned int count, const unsigned int move_count,
			EquipSoundSetMoveTypes*& dest_field)
		{
			if (const auto dest = write_array(buf, 7, types, count, dest_field))
			{
				for (auto i = 0u; i < count; i++)
				{
					write_sound_sets(buf, types[i].soundSets, move_count, dest[i].soundSets);
				}
			}
		}

		void write_chances(zone_buffer* buf, EquipmentChanceRattleTypes* chances, const EquipmentSoundTable* table, EquipmentChanceRattleTypes*& dest_field)
		{
			if (const auto dest_cloth_types = write_array(buf, 7, chances, table->numClothTypes, dest_field))
			{
				for (auto i = 0u; i < table->numClothTypes; i++)
				{
					if (const auto dest_rattle_types = write_array(buf, 7, chances[i].chances, table->numWeaponRattleTypes, dest_cloth_types[i].chances))
					{
						for (auto j = 0u; j < table->numWeaponRattleTypes; j++)
						{
							write_array(buf, 3, chances[i].chances[j].chances, table->numMoveTypes, dest_rattle_types[j].chances);
						}
					}
				}
			}
		}

		void parse_sound_set(assetmanager::reader& read, EquipmentSoundSet* set)
		{
			set->soundPLR = read.read_asset<snd_alias_list_t>();
			set->soundNPC = read.read_asset<snd_alias_list_t>();
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

		void parse_move_types_array(assetmanager::reader& read, zone_memory* mem, EquipSoundSetMoveTypes*& field, const unsigned int count,
			const unsigned int move_count)
		{
			if (const auto types = parse_array(read, mem, field, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					parse_sound_sets(read, mem, types[i].soundSets, move_count);
				}
			}
		}

		void parse_chances(assetmanager::reader& read, zone_memory* mem, EquipmentChanceRattleTypes*& field, const EquipmentSoundTable* table)
		{
			if (const auto cloth_types = parse_array(read, mem, field, table->numClothTypes))
			{
				for (auto i = 0u; i < table->numClothTypes; i++)
				{
					if (const auto rattle_types = parse_array(read, mem, cloth_types[i].chances, table->numWeaponRattleTypes))
					{
						for (auto j = 0u; j < table->numWeaponRattleTypes; j++)
						{
							parse_array(read, mem, rattle_types[j].chances, table->numMoveTypes);
						}
					}
				}
			}
		}

		void dump_sound_set(assetmanager::dumper& write, const EquipmentSoundSet* set)
		{
			write.dump_asset(set->soundPLR);
			write.dump_asset(set->soundNPC);
		}

		void dump_sound_sets(assetmanager::dumper& write, EquipmentSoundSet* sets, const unsigned int count)
		{
			write.dump_array(sets, count);
			for (auto i = 0u; sets && i < count; i++)
			{
				dump_sound_set(write, &sets[i]);
			}
		}

		void dump_move_types_array(assetmanager::dumper& write, EquipSoundSetMoveTypes* types, const unsigned int count, const unsigned int move_count)
		{
			write.dump_array(types, count);
			for (auto i = 0u; types && i < count; i++)
			{
				dump_sound_sets(write, types[i].soundSets, move_count);
			}
		}

		void dump_chances(assetmanager::dumper& write, EquipmentChanceRattleTypes* chances, const EquipmentSoundTable* table)
		{
			write.dump_array(chances, table->numClothTypes);
			for (auto i = 0u; chances && i < table->numClothTypes; i++)
			{
				write.dump_array(chances[i].chances, table->numWeaponRattleTypes);
				for (auto j = 0u; chances[i].chances && j < table->numWeaponRattleTypes; j++)
				{
					write.dump_array(chances[i].chances[j].chances, table->numMoveTypes);
				}
			}
		}

		void add_sound_set_dependencies(zone_base* zone, const EquipmentSoundSet* sets, const std::size_t count)
		{
			for (std::size_t i = 0; sets && i < count; i++)
			{
				add_dependency(zone, ASSET_TYPE_SOUND, sets[i].soundPLR);
				add_dependency(zone, ASSET_TYPE_SOUND, sets[i].soundNPC);
			}
		}
	}

	void equipment_snd_table::read(zone_reader& reader, EquipmentSoundTable* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto cloth_types = reader.read_array(asset->clothTypes, 7, asset->numClothTypes))
		{
			for (auto i = 0u; i < asset->numClothTypes; i++)
			{
				reader.read_string(cloth_types[i].szName);
			}
		}
		if (const auto rattle_types = reader.read_array(asset->weaponRattleTypes, 7, asset->numWeaponRattleTypes))
		{
			for (auto i = 0u; i < asset->numWeaponRattleTypes; i++)
			{
				reader.read_string(rattle_types[i].szName);
			}
		}
		read_chances(reader, asset->chancesPLR, asset);
		read_chances(reader, asset->chancesNPC, asset);
		read_move_types_array(reader, asset->mvmtClothSoundSets, asset->numClothTypes, asset->numMoveTypes);
		read_move_types_array(reader, asset->mvmtRattleSoundSets, asset->numWeaponRattleTypes, asset->numMoveTypes);
		read_sound_sets(reader, asset->mvmtAccentSoundSets.soundSets, asset->numMoveTypes);
		if (const auto mantle_types = reader.read_array(asset->mvmtMantleSoundSets, 7, asset->numClothTypes))
		{
			for (auto i = 0u; i < asset->numClothTypes; i++)
			{
				for (auto& set : mantle_types[i].soundSets)
				{
					read_sound_set(reader, &set);
				}
			}
		}
		reader.pop_stream();
	}

	EquipmentSoundTable* equipment_snd_table::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_EQUIPMENT_SND_TABLE, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing equipmentsndtable \"%s\"...", name.data());

		auto* asset = read.read_single<EquipmentSoundTable>();
		asset->name = read.read_string();
		if (const auto cloth_types = parse_array(read, mem, asset->clothTypes, asset->numClothTypes))
		{
			for (auto i = 0u; i < asset->numClothTypes; i++)
			{
				cloth_types[i].szName = read.read_string();
			}
		}
		if (const auto rattle_types = parse_array(read, mem, asset->weaponRattleTypes, asset->numWeaponRattleTypes))
		{
			for (auto i = 0u; i < asset->numWeaponRattleTypes; i++)
			{
				rattle_types[i].szName = read.read_string();
			}
		}
		parse_chances(read, mem, asset->chancesPLR, asset);
		parse_chances(read, mem, asset->chancesNPC, asset);
		parse_move_types_array(read, mem, asset->mvmtClothSoundSets, asset->numClothTypes, asset->numMoveTypes);
		parse_move_types_array(read, mem, asset->mvmtRattleSoundSets, asset->numWeaponRattleTypes, asset->numMoveTypes);
		parse_sound_sets(read, mem, asset->mvmtAccentSoundSets.soundSets, asset->numMoveTypes);
		if (const auto mantle_types = parse_array(read, mem, asset->mvmtMantleSoundSets, asset->numClothTypes))
		{
			for (auto i = 0u; i < asset->numClothTypes; i++)
			{
				for (auto& set : mantle_types[i].soundSets)
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
		for (auto i = 0u; data->mvmtClothSoundSets && i < data->numClothTypes; i++)
		{
			add_sound_set_dependencies(zone, data->mvmtClothSoundSets[i].soundSets, data->numMoveTypes);
		}
		for (auto i = 0u; data->mvmtRattleSoundSets && i < data->numWeaponRattleTypes; i++)
		{
			add_sound_set_dependencies(zone, data->mvmtRattleSoundSets[i].soundSets, data->numMoveTypes);
		}
		add_sound_set_dependencies(zone, data->mvmtAccentSoundSets.soundSets, data->numMoveTypes);
		for (auto i = 0u; data->mvmtMantleSoundSets && i < data->numClothTypes; i++)
		{
			add_sound_set_dependencies(zone, data->mvmtMantleSoundSets[i].soundSets, std::size(data->mvmtMantleSoundSets[i].soundSets));
		}
	}

	void equipment_snd_table::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = this->write_name(buf);
		if (const auto dest_cloth_types = write_array(buf, 7, data->clothTypes, data->numClothTypes, dest->clothTypes))
		{
			for (auto i = 0u; i < data->numClothTypes; i++)
			{
				write_string(buf, data->clothTypes[i].szName, dest_cloth_types[i].szName);
			}
		}
		if (const auto dest_rattle_types = write_array(buf, 7, data->weaponRattleTypes, data->numWeaponRattleTypes, dest->weaponRattleTypes))
		{
			for (auto i = 0u; i < data->numWeaponRattleTypes; i++)
			{
				write_string(buf, data->weaponRattleTypes[i].szName, dest_rattle_types[i].szName);
			}
		}
		write_chances(buf, data->chancesPLR, data, dest->chancesPLR);
		write_chances(buf, data->chancesNPC, data, dest->chancesNPC);
		write_move_types_array(buf, data->mvmtClothSoundSets, data->numClothTypes, data->numMoveTypes, dest->mvmtClothSoundSets);
		write_move_types_array(buf, data->mvmtRattleSoundSets, data->numWeaponRattleTypes, data->numMoveTypes, dest->mvmtRattleSoundSets);
		write_sound_sets(buf, data->mvmtAccentSoundSets.soundSets, data->numMoveTypes, dest->mvmtAccentSoundSets.soundSets);
		if (const auto dest_mantle_types = write_array(buf, 7, data->mvmtMantleSoundSets, data->numClothTypes, dest->mvmtMantleSoundSets))
		{
			for (auto i = 0u; i < data->numClothTypes; i++)
			{
				for (auto j = 0u; j < std::size(data->mvmtMantleSoundSets[i].soundSets); j++)
				{
					write_sound_set(buf, &data->mvmtMantleSoundSets[i].soundSets[j], &dest_mantle_types[i].soundSets[j]);
				}
			}
		}
		buf->pop_stream();
	}

	void equipment_snd_table::dump(EquipmentSoundTable* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_EQUIPMENT_SND_TABLE, asset->name)))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_array(asset->clothTypes, asset->numClothTypes);
		for (auto i = 0u; asset->clothTypes && i < asset->numClothTypes; i++)
		{
			write.dump_string(asset->clothTypes[i].szName);
		}
		write.dump_array(asset->weaponRattleTypes, asset->numWeaponRattleTypes);
		for (auto i = 0u; asset->weaponRattleTypes && i < asset->numWeaponRattleTypes; i++)
		{
			write.dump_string(asset->weaponRattleTypes[i].szName);
		}
		dump_chances(write, asset->chancesPLR, asset);
		dump_chances(write, asset->chancesNPC, asset);
		dump_move_types_array(write, asset->mvmtClothSoundSets, asset->numClothTypes, asset->numMoveTypes);
		dump_move_types_array(write, asset->mvmtRattleSoundSets, asset->numWeaponRattleTypes, asset->numMoveTypes);
		dump_sound_sets(write, asset->mvmtAccentSoundSets.soundSets, asset->numMoveTypes);
		write.dump_array(asset->mvmtMantleSoundSets, asset->numClothTypes);
		for (auto i = 0u; asset->mvmtMantleSoundSets && i < asset->numClothTypes; i++)
		{
			for (auto& set : asset->mvmtMantleSoundSets[i].soundSets)
			{
				dump_sound_set(write, &set);
			}
		}
		write.close();
	}
}
