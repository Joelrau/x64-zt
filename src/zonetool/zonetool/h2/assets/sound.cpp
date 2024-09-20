#include <std_include.hpp>
#include "sound.hpp"

namespace zonetool::h2
{
#define SND_MAX_VOLUME_GROUPS 126
	std::array<const char*, SND_MAX_VOLUME_GROUPS> volume_mod_groups =
	{
		"default",
		// User interface
		"hud",
		"frontend_sfx",
		"frontend_music",
		"sp_hit_alert_npc",
		// Music
		"music",
		"music2",
		"music_big",
		"music_so",
		// Ambience
		"amb_quad",
		"amb_dist",
		"amb_damb",
		"amb_emitter",
		"amb_emitter_int",
		"amb_emitter_ext",
		// Physics
		"physics",
		// Characters
		"bodyfall",
		"deaths_door",
		"foley_plr",
		"foley_npc",
		"footstep_plr",
		"footstep_plr_special",
		"footstep_water_plr",
		"footstep_npc",
		"melee_npc",
		"melee_hit",
		"mp_foley_npc",
		"mp_foley_plr",
		"mp_footstep_plr",
		"mp_footstep_npc",
		"plr_internal",
		"wpn_foley_plr",
		"wpn_foley_plr_beltfed",
		"wpn_plr_foley_h2",
		"wpn_foley_npc",
		"melee_plr",
		// Dialogue
		"voices_ally",
		"voices_enemy",
		"voices_enemy_death_kbp",
		"voices_dialog",
		"voices_dialog_radio",
		"voices_dialog_critical",
		"voices_pa",
		// Explosions & Destruction
		"destruct",
		"explosion",
		"explosion_grenade",
		"explosion_flashbang",
		"explosion_rocket",
		"explosion_car",
		"explosion_critical",
		"explosion_debris",
		// Bullet Impacts & Whizbys
		"bullet_impact_geo",
		"bullet_impact_geo_metal",
		"bullet_impact_plr",
		"bullet_impact_npc",
		"mp_bullet_impact_geo",
		"mp_bullet_impact_plr",
		"mp_bullet_impact_npc",
		"bullet_whizby",
		"mp_bullet_whizby",
		// Vehicles
		"vehicle_plr",
		"vehicle_plr_special",
		"vehicle_npc",
		"vehicle_avm_plr",
		"vehicle_avm_npc",
		"wpn_vehicle_plr",
		"wpn_vehicle_npc",
		"wpn_vehicle_npc_h2",
		"vehicle_aircraft",
		// Weapons
		"grenadebounce",
		"mp_grenadebounce",
		"shellcasings",
		"wpn_plr",
		"wpn_plr_special",
		"wpn_npc_special",
		"wpn_npc",
		"mp_wpn_npc",
		"wpn_projectile",
		// Weapons Detailed
		"iw4_wpn_plr_shot",
		"wpn_plr_shot_first",
		"wpn_plr_shot_special",
		"wpn_plr_shot",
		"wpn_plr_punch",
		"wpn_plr_mech",
		"wpn_plr_sub",
		"wpn_plr_lfe",
		"wpn_plr_tail",
		"wpn_plr_tail_int_med",
		"wpn_plr_tail_int_sml",
		"wpn_plr_tail_semi",
		//
		"iw4_wpn_npc_shot",
		"wpn_npc_shot",
		"wpn_npc_mech",
		"wpn_npc_sub",
		"wpn_npc_tail",
		"wpn_npc_dist",
		"h2_wpn_npc_shot_close",
		"h2_wpn_npc_shot_mid",
		"h2_wpn_npc_shot_far",
		"h2_wpn_npc_shot_tail",
		"h2_wpn_npc_shot_first",
		// Tactical
		"tactical_plr",
		"tactical_npc",
		"tactical_plr_special",
		"tactical_npc_special",
		// Attack Dogs
		"attack_dog",
		// Special use
		"fullvolume",
		"big_moment",
		"scripted1",
		"scripted2",
		"scripted3",
		"scripted4",
		"scripted5",
		"scripted6",
		"scripted7",
		"scripted8",
		"scripted9",
		"scripted10",
		"scripted11",
		"scripted12",
		"scripted13",
		"scripted14",
		"scripted15",
		// MP
		"mp_pre_event",
		"mp_splash_notify",
		"mp_hit_marker",
	};

#define SND_MAX_DSP_BUSES 32
	std::array<const char*, SND_MAX_DSP_BUSES> dsp_buses =
	{
		"default",
		"ambiance_quad",
		"animals",
		"emitters",
		"emitters_ext",
		"emitters_int",
		"emitters_unoccluded",
		"explosions",
		"explosions_critical",
		"foley",
		"interface",
		"music",
		"notimescale",
		"physics",
		"scripted1",
		"scripted2",
		"scripted3",
		"scripted4",
		"tactical",
		"unoccluded",
		"vehicles_air",
		"vehicles_land",
		"voices",
		"voices_env",
		"voices_critical",
		"voices_unfiltered",
		"weapons_npc",
		"weapons_npc_dist",
		"weapons_npc_mid",
		"weapons_npc_tail",
		"weapons_plr",
		"whizbys",
	};

	const char* get_vol_mod_name(short index)
	{
		return volume_mod_groups[index];
	}

	const char* get_dsp_bus_name(char index)
	{
		return dsp_buses[index];
	}

	short get_vol_mod_index_from_name(const char* name)
	{
		for (short i = 0; i < volume_mod_groups.size(); i++)
		{
			const char* vol_mod = volume_mod_groups[i];
			if (!_stricmp(vol_mod, name))
			{
				return i;
			}
		}
		return -1;
	}

	char get_dsp_bus_index_from_name(const char* name)
	{
		for (char i = 0; i < dsp_buses.size(); i++)
		{
			const char* dsp_bus = dsp_buses[i];
			if (!_stricmp(dsp_bus, name))
			{
				return i;
			}
		}
		return -1;
	}

#define SOUND_DUMP_SUBASSET(entry) \
	if (asset->entry) sound[#entry] = asset->entry->name;	\
	else sound[#entry] = nullptr;
#define SOUND_DUMP_STRING(entry) \
	if (asset->entry) sound[#entry] = std::string(asset->entry); \
	else sound[#entry] = nullptr;
#define SOUND_DUMP_FIELD(entry) \
	sound[#entry] = asset->entry;

#define SOUND_READ_SUBASSET(entry, __type__) \
	if (!snddata[#entry].is_null()) { \
		asset->entry = mem->allocate<__type__>(); \
		asset->entry->name = mem->duplicate_string(snddata[#entry].get<std::string>().data()); }
#define SOUND_READ_STRING(entry) \
	if (!snddata[#entry].is_null()) { \
		asset->entry = mem->duplicate_string(snddata[#entry].get<std::string>().data()); \
	} else { asset->entry = nullptr; }
#define SOUND_READ_FIELD(entry) \
	asset->entry = snddata[#entry].get<decltype(asset->entry)>()

	void sound::json_parse_snd_alias(snd_alias_t* asset, nlohmann::json snddata, zone_memory* mem)
	{
		SOUND_READ_STRING(aliasName);
		SOUND_READ_STRING(secondaryAliasName);
		SOUND_READ_STRING(chainAliasName);
		SOUND_READ_STRING(subtitle);
		SOUND_READ_STRING(squelchName);

		asset->soundFile = mem->allocate<SoundFile>();
		asset->soundFile->type = snddata["soundfile"]["type"].get<snd_alias_type_t>();
		asset->soundFile->exists = snddata["soundfile"]["exists"].get<bool>();

		if (asset->soundFile->type == SAT_LOADED)
		{
			asset->soundFile->u.loadSnd = mem->allocate<LoadedSound>();
			asset->soundFile->u.loadSnd->name = mem->duplicate_string(snddata["soundfile"]["name"].get<std::string>().data());
		}
		else if (asset->soundFile->type == SAT_STREAMED)
		{
			asset->soundFile->u.streamSnd.totalMsec = snddata["soundfile"]["totalMsec"].get<unsigned int>();
			asset->soundFile->u.streamSnd.filename.isLocalized = snddata["soundfile"]["isLocalized"].get<bool>();
			asset->soundFile->u.streamSnd.filename.isStreamed = snddata["soundfile"]["isStreamed"].get<bool>();
			asset->soundFile->u.streamSnd.filename.fileIndex = snddata["soundfile"]["fileIndex"].get<unsigned short>();
			if (asset->soundFile->u.streamSnd.filename.fileIndex)
			{
				asset->soundFile->u.streamSnd.filename.info.packed.offset = snddata["soundfile"]["packed"]["offset"].get<unsigned long long>();
				asset->soundFile->u.streamSnd.filename.info.packed.length = snddata["soundfile"]["packed"]["length"].get<unsigned long long>();
			}
			else
			{
				asset->soundFile->u.streamSnd.filename.info.raw.dir = mem->duplicate_string(snddata["soundfile"]["raw"]["dir"].get<std::string>().data());
				asset->soundFile->u.streamSnd.filename.info.raw.name = mem->duplicate_string(snddata["soundfile"]["raw"]["name"].get<std::string>().data());
			}
		}
		else if (asset->soundFile->type == SAT_PRIMED)
		{
			ZONETOOL_FATAL("SAT_PRIMED parsing is not supported yet.");
		}

		SOUND_READ_FIELD(flags);
		SOUND_READ_FIELD(variationType);
		SOUND_READ_FIELD(priority);
		asset->dspBusIndex = get_dsp_bus_index_from_name(snddata["dspBus"].get<std::string>().data()); //SOUND_CHAR(dspBusIndex);
		asset->volModIndex = get_vol_mod_index_from_name(snddata["volMod"].get<std::string>().data()); //SOUND_SHORT(volModIndex);
		SOUND_READ_FIELD(volMin);
		SOUND_READ_FIELD(volMax);
		SOUND_READ_FIELD(pitchMin);
		SOUND_READ_FIELD(pitchMax);
		SOUND_READ_FIELD(distMin);
		SOUND_READ_FIELD(distMax);
		SOUND_READ_FIELD(velocityMin);
		SOUND_READ_FIELD(probability);
		SOUND_READ_FIELD(sequence);
		SOUND_READ_FIELD(startDelay);

		SOUND_READ_FIELD(masterPriority);
		SOUND_READ_FIELD(masterPercentage);
		SOUND_READ_FIELD(slavePercentage);
		SOUND_READ_FIELD(playbackPercentage);

		SOUND_READ_FIELD(lfePercentage);
		SOUND_READ_FIELD(centerPercentage);

		SOUND_READ_FIELD(polyCount);
		SOUND_READ_FIELD(polyGlobalCount);
		SOUND_READ_FIELD(polyEntityType);
		SOUND_READ_FIELD(polyGlobalType);
		SOUND_READ_FIELD(polyClass);
		SOUND_READ_FIELD(playCount);
		SOUND_READ_FIELD(unk);

		SOUND_READ_FIELD(envelopMin);
		SOUND_READ_FIELD(envelopMax);

		SOUND_READ_FIELD(wetMixOverride);
		SOUND_READ_FIELD(focusPercentage);

		SOUND_READ_FIELD(smartpanDistance2d);
		SOUND_READ_FIELD(smartpanDistance3d);
		SOUND_READ_FIELD(smartpanAttenuation3d);
		SOUND_READ_FIELD(minSmartpan2dContribution);

		SOUND_READ_FIELD(stereo3DAngle);
		SOUND_READ_FIELD(stereo3DStart);
		SOUND_READ_FIELD(stereo3DEnd);

		SOUND_READ_FIELD(threshold);
		SOUND_READ_FIELD(lockedLoopTime);

		SOUND_READ_SUBASSET(sndContext, SndContext);
		SOUND_READ_SUBASSET(volumeFalloffCurve, SndCurve);
		SOUND_READ_SUBASSET(lpfCurve, SndCurve);
		SOUND_READ_SUBASSET(hpfCurve, SndCurve);
		SOUND_READ_SUBASSET(reverbSendCurve, SndCurve);

		if (!snddata["speakerMap"].is_null())
		{
			asset->speakerMap = mem->allocate<SpeakerMap>();
			json speakerMap = snddata["speakerMap"];

			asset->speakerMap->name = mem->duplicate_string(speakerMap["name"].get<std::string>().data());
			asset->speakerMap->isDefault = speakerMap["isDefault"].get<bool>();
			asset->speakerMap->orientation = speakerMap["orientation"].get<unsigned char>();

			if (!speakerMap["channelMaps"].is_null())
			{
				json channelMaps = speakerMap["channelMaps"];
				for (char x = 0; x < 2; x++)
				{
					for (char y = 0; y < 2; y++)
					{
						if (!channelMaps[(x & 0x01) << 1 | y & 0x01].is_null())
						{
							json channelMap = channelMaps[(x & 0x01) << 1 | y & 0x01];
							asset->speakerMap->channelMaps[x][y].speakerCount = channelMap["speakerCount"].get<int>();

							if (!channelMap["speakers"].is_null())
							{
								json speakers = channelMap["speakers"];

								for (int speaker = 0; speaker < asset->speakerMap->channelMaps[x][y].speakerCount;
									speaker++)
								{
									if (!speakers[speaker].is_null())
									{
										json jspeaker = speakers[speaker];
										asset->speakerMap->channelMaps[x][y].speakers[speaker].speaker =
											jspeaker["speaker"].get<char>();
										asset->speakerMap->channelMaps[x][y].speakers[speaker].numLevels =
											jspeaker["numLevels"].get<char>();
										asset->speakerMap->channelMaps[x][y].speakers[speaker].levels[0] =
											jspeaker["levels0"].get<float>();
										asset->speakerMap->channelMaps[x][y].speakers[speaker].levels[1] =
											jspeaker["levels1"].get<float>();
									}
								}
							}
						}
					}
				}
			}
		}

		SOUND_READ_FIELD(allowDoppler);
		SOUND_READ_SUBASSET(dopplerPreset, DopplerPreset);
	}

	snd_alias_list_t* sound::json_parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "sounds\\"s + name + ".json"s;
		auto file = filesystem::file(path);
		file.open("rb");

		if (file.get_fp())
		{
			ZONETOOL_INFO("Parsing sound \"%s\"...", name.data());

			auto size = file.size();
			auto bytes = file.read_bytes(size);
			file.close();

			json snddata = json::parse(bytes);

			snd_alias_list_t* asset = mem->allocate<snd_alias_list_t>();

			SOUND_READ_STRING(aliasName);
			SOUND_READ_FIELD(count);

			asset->head = mem->allocate<snd_alias_t>(asset->count);

			json heads = snddata["head"];
			for (unsigned char i = 0; i < asset->count; i++)
			{
				json_parse_snd_alias(&asset->head[i], heads[i], mem);
			}

			auto context_list = snddata["contextList"];
			if (context_list.is_array())
			{
				asset->contextListCount = static_cast<unsigned char>(context_list.size());
				asset->contextList = mem->allocate<snd_alias_context_list>(asset->contextListCount);

				for (unsigned char i = 0; i < asset->contextListCount; i++)
				{
					asset->contextList[i].aliasOffset = context_list[i]["aliasOffset"].get<unsigned char>();
					asset->contextList[i].count = context_list[i]["count"].get<unsigned char>();
				}
			}

			return asset;
		}

		return nullptr;
	}

	snd_alias_list_t* sound::parse(const std::string& name, zone_memory* mem)
	{
		return json_parse(name, mem);
	}

	void sound::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;
		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).sound;
		}
	}

	void sound::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void sound::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;

		for (unsigned char i = 0; i < data->count; i++)
		{
			auto* head = &data->head[i];

			if (head->secondaryAliasName)
			{
				zone->add_asset_of_type(ASSET_TYPE_SOUND, head->secondaryAliasName);
			}

			if (head->sndContext)
			{
				zone->add_asset_of_type(ASSET_TYPE_SOUND_CONTEXT, head->sndContext->name);
			}

			if (head->volumeFalloffCurve)
			{
				zone->add_asset_of_type(ASSET_TYPE_SOUND_CURVE, head->volumeFalloffCurve->name);
			}

			if (head->lpfCurve)
			{
				zone->add_asset_of_type(ASSET_TYPE_LPF_CURVE, head->lpfCurve->name);
			}

			if (head->hpfCurve)
			{
				zone->add_asset_of_type(ASSET_TYPE_LPF_CURVE, head->hpfCurve->name);
			}

			if (head->reverbSendCurve)
			{
				zone->add_asset_of_type(ASSET_TYPE_REVERB_CURVE, head->reverbSendCurve->name);
			}

			if (head->soundFile)
			{
				if (head->soundFile->type == SAT_LOADED)
				{
					zone->add_asset_of_type(ASSET_TYPE_LOADED_SOUND, head->soundFile->u.loadSnd->name);
				}
			}

			if (head->dopplerPreset)
			{
				zone->add_asset_of_type(ASSET_TYPE_DOPPLER_PRESET, head->dopplerPreset->name);
			}
		}
	}

	std::string sound::name()
	{
		return this->name_;
	}

	std::int32_t sound::type()
	{
		return ASSET_TYPE_SOUND;
	}

	void sound::write_soundfile(zone_base* zone, zone_buffer* buf, SoundFile* data)
	{
		auto* dest = buf->write(data);

		if (data->type == SAT_LOADED)
		{
			if (data->u.loadSnd)
			{
				dest->u.loadSnd = static_cast<LoadedSound*>(zone->get_asset_pointer(
					ASSET_TYPE_LOADED_SOUND, data->u.loadSnd->name));
			}
		}
		else if (data->type == SAT_STREAMED)
		{
			if (!data->u.streamSnd.filename.fileIndex)
			{
				if (data->u.streamSnd.filename.info.raw.dir)
				{
					dest->u.streamSnd.filename.info.raw.dir = buf->write_str(data->u.streamSnd.filename.info.raw.dir);
				}

				if (data->u.streamSnd.filename.info.raw.name)
				{
					dest->u.streamSnd.filename.info.raw.name = buf->write_str(data->u.streamSnd.filename.info.raw.name);
				}
			}
		}
		else if (data->type == SAT_PRIMED)
		{
			if (data->u.loadSnd)
			{
				dest->u.loadSnd = static_cast<LoadedSound*>(zone->get_asset_pointer(
					ASSET_TYPE_LOADED_SOUND, data->u.loadSnd->name));
			}

			if (!data->u.streamSnd.filename.fileIndex)
			{
				if (data->u.streamSnd.filename.info.raw.dir)
				{
					dest->u.streamSnd.filename.info.raw.dir = buf->write_str(data->u.streamSnd.filename.info.raw.dir);
				}

				if (data->u.streamSnd.filename.info.raw.name)
				{
					dest->u.streamSnd.filename.info.raw.name = buf->write_str(data->u.streamSnd.filename.info.raw.name);
				}
			}
		}
	}

	const char* aliasname_ptr = nullptr;

	void sound::write_head(zone_base* zone, zone_buffer* buf, snd_alias_t* dest)
	{
		auto* data = dest;

		// this needs to be the same PTR
		dest->aliasName = aliasname_ptr;

		if (data->subtitle)
		{
			dest->subtitle = buf->write_str(data->subtitle);
		}

		if (data->secondaryAliasName)
		{
			dest->secondaryAliasName = buf->write_str(data->secondaryAliasName);
		}

		if (data->chainAliasName)
		{
			dest->chainAliasName = buf->write_str(data->chainAliasName);
		}

		if (data->squelchName)
		{
			dest->squelchName = buf->write_str(data->squelchName);
		}

		if (data->soundFile)
		{
			buf->align(3);
			write_soundfile(zone, buf, data->soundFile);
			buf->clear_pointer(&dest->soundFile);
		}

		if (data->sndContext)
		{
			dest->sndContext = static_cast<SndContext*>(zone->get_asset_pointer(
				ASSET_TYPE_SOUND_CONTEXT, data->sndContext->name));
		}

		if (data->volumeFalloffCurve)
		{
			dest->volumeFalloffCurve = static_cast<SndCurve*>(zone->get_asset_pointer(
				ASSET_TYPE_SOUND_CURVE, data->volumeFalloffCurve->name));
		}

		if (data->lpfCurve)
		{
			dest->lpfCurve = static_cast<SndCurve*>(zone->get_asset_pointer(
				ASSET_TYPE_LPF_CURVE, data->lpfCurve->name));
		}

		if (data->hpfCurve)
		{
			dest->hpfCurve = static_cast<SndCurve*>(zone->get_asset_pointer(
				ASSET_TYPE_LPF_CURVE, data->hpfCurve->name));
		}

		if (data->reverbSendCurve)
		{
			dest->reverbSendCurve = static_cast<SndCurve*>(zone->get_asset_pointer(
				ASSET_TYPE_REVERB_CURVE, data->reverbSendCurve->name));
		}

		if (data->speakerMap)
		{
			buf->align(3);
			auto* speaker_map = buf->write(data->speakerMap);

			if (speaker_map->name)
			{
				speaker_map->name = buf->write_str(speaker_map->name);
			}

			buf->clear_pointer(&dest->speakerMap);
		}

		if (data->dopplerPreset)
		{
			dest->dopplerPreset = static_cast<DopplerPreset*>(zone->get_asset_pointer(
				ASSET_TYPE_DOPPLER_PRESET, data->dopplerPreset->name));
		}
	}

	void sound::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		aliasname_ptr = buf->get_zone_pointer<const char>(buf->current_stream(), buf->current_stream_offset());
		dest->name = buf->write_str(this->name());

		if (data->head)
		{
			buf->align(3);
			auto* dest_sound = buf->write(data->head, data->count);

			for (unsigned char i = 0; i < data->count; i++)
			{
				write_head(zone, buf, &dest_sound[i]);
			}

			buf->clear_pointer(&dest->head);
		}

		if (data->contextList)
		{
			buf->align(1);
			buf->write(data->contextList, data->contextListCount);
			buf->clear_pointer(&dest->contextList);
		}

		buf->pop_stream();
	}

	void sound::json_dump_snd_alias(ordered_json& sound, snd_alias_t* asset)
	{
		SOUND_DUMP_STRING(aliasName);
		SOUND_DUMP_STRING(secondaryAliasName);
		SOUND_DUMP_STRING(chainAliasName);
		SOUND_DUMP_STRING(subtitle);
		SOUND_DUMP_STRING(squelchName);

		// soundfile shit
		if (asset->soundFile)
		{
			sound["soundfile"]["type"] = asset->soundFile->type;
			sound["soundfile"]["exists"] = asset->soundFile->exists;

			auto insert_loaded = [&]()
			{
				sound["soundfile"]["name"] = asset->soundFile->u.loadSnd ?
					(asset->soundFile->u.loadSnd->name ? asset->soundFile->u.loadSnd->name : "") :
					"";
			};

			auto insert_streamed = [&]()
			{
				sound["soundfile"]["totalMsec"] = asset->soundFile->u.streamSnd.totalMsec;
				sound["soundfile"]["isLocalized"] = asset->soundFile->u.streamSnd.filename.isLocalized;
				sound["soundfile"]["isStreamed"] = asset->soundFile->u.streamSnd.filename.isStreamed;
				sound["soundfile"]["fileIndex"] = asset->soundFile->u.streamSnd.filename.fileIndex;

				sound["soundfile"]["packed"]["offset"] = 0;
				sound["soundfile"]["packed"]["length"] = 0;
				sound["soundfile"]["raw"]["dir"] = "";
				sound["soundfile"]["raw"]["name"] = "";

				if (asset->soundFile->u.streamSnd.filename.fileIndex)
				{
					sound["soundfile"]["packed"]["offset"] = asset->soundFile->u.streamSnd.filename.info.packed.offset;
					sound["soundfile"]["packed"]["length"] = asset->soundFile->u.streamSnd.filename.info.packed.length;
				}
				else
				{
					sound["soundfile"]["raw"]["dir"] = asset->soundFile->u.streamSnd.filename.info.raw.dir
						? asset->soundFile->u.streamSnd.filename.info.raw.dir
						: "";
					sound["soundfile"]["raw"]["name"] = asset->soundFile->u.streamSnd.filename.info.raw.name
						? asset->soundFile->u.streamSnd.filename.info.raw.name
						: "";
				}
			};

			auto insert_primed = [&]()
			{
				sound["soundfile"]["isLocalized"] = asset->soundFile->u.primedSnd.streamedPart.isLocalized;
				sound["soundfile"]["isStreamed"] = asset->soundFile->u.primedSnd.streamedPart.isStreamed;
				sound["soundfile"]["fileIndex"] = asset->soundFile->u.primedSnd.streamedPart.fileIndex;

				sound["soundfile"]["packed"]["offset"] = 0;
				sound["soundfile"]["packed"]["length"] = 0;
				sound["soundfile"]["raw"]["dir"] = "";
				sound["soundfile"]["raw"]["name"] = "";

				if (asset->soundFile->u.primedSnd.streamedPart.fileIndex)
				{
					sound["soundfile"]["packed"]["offset"] = asset->soundFile->u.primedSnd.streamedPart.info.packed.offset;
					sound["soundfile"]["packed"]["length"] = asset->soundFile->u.primedSnd.streamedPart.info.packed.length;
				}
				else
				{
					sound["soundfile"]["raw"]["dir"] = asset->soundFile->u.primedSnd.streamedPart.info.raw.dir
						? asset->soundFile->u.primedSnd.streamedPart.info.raw.dir
						: "";
					sound["soundfile"]["raw"]["name"] = asset->soundFile->u.primedSnd.streamedPart.info.raw.name
						? asset->soundFile->u.primedSnd.streamedPart.info.raw.name
						: "";
				}

				if (asset->soundFile->u.primedSnd.loadedPart)
				{
					sound["soundfile"]["name"] = asset->soundFile->u.primedSnd.loadedPart ?
						(asset->soundFile->u.primedSnd.loadedPart->name ? asset->soundFile->u.primedSnd.loadedPart->name : "") :
						"";
				}

				sound["soundfile"]["dataOffset"] = asset->soundFile->u.primedSnd.dataOffset;
				sound["soundfile"]["totalSize"] = asset->soundFile->u.primedSnd.totalSize;
				//sound["soundfile"]["primedCrc"] = asset->soundFile->u.primedSnd.primedCrc;
			};

			if (asset->soundFile->type == SAT_LOADED)
			{
				insert_loaded();
			}
			else if (asset->soundFile->type == SAT_STREAMED)
			{
				insert_streamed();
			}
			else if (asset->soundFile->type == SAT_PRIMED)
			{
				insert_primed();
			}
		}

		SOUND_DUMP_FIELD(flags);
		SOUND_DUMP_FIELD(variationType);
		SOUND_DUMP_FIELD(priority);
		sound["dspBus"] = get_dsp_bus_name(asset->dspBusIndex); //SOUND_DUMP_CHAR(dspBusIndex);
		sound["volMod"] = get_vol_mod_name(asset->volModIndex); //SOUND_DUMP_SHORT(volModIndex);
		SOUND_DUMP_FIELD(volMin);
		SOUND_DUMP_FIELD(volMax);
		SOUND_DUMP_FIELD(pitchMin);
		SOUND_DUMP_FIELD(pitchMax);
		SOUND_DUMP_FIELD(distMin);
		SOUND_DUMP_FIELD(distMax);
		SOUND_DUMP_FIELD(velocityMin);
		SOUND_DUMP_FIELD(probability);
		SOUND_DUMP_FIELD(sequence);
		SOUND_DUMP_FIELD(startDelay);

		SOUND_DUMP_FIELD(masterPriority);
		SOUND_DUMP_FIELD(masterPercentage);
		SOUND_DUMP_FIELD(slavePercentage);
		SOUND_DUMP_FIELD(playbackPercentage);

		SOUND_DUMP_FIELD(lfePercentage);
		SOUND_DUMP_FIELD(centerPercentage);

		SOUND_DUMP_FIELD(polyCount);
		SOUND_DUMP_FIELD(polyGlobalCount);
		SOUND_DUMP_FIELD(polyEntityType);
		SOUND_DUMP_FIELD(polyGlobalType);
		SOUND_DUMP_FIELD(polyClass);
		SOUND_DUMP_FIELD(playCount);
		SOUND_DUMP_FIELD(unk);

		SOUND_DUMP_FIELD(envelopMin);
		SOUND_DUMP_FIELD(envelopMax);

		SOUND_DUMP_FIELD(wetMixOverride);
		SOUND_DUMP_FIELD(focusPercentage);

		SOUND_DUMP_FIELD(smartpanDistance2d);
		SOUND_DUMP_FIELD(smartpanDistance3d);
		SOUND_DUMP_FIELD(smartpanAttenuation3d);
		SOUND_DUMP_FIELD(minSmartpan2dContribution);

		SOUND_DUMP_FIELD(stereo3DAngle);
		SOUND_DUMP_FIELD(stereo3DStart);
		SOUND_DUMP_FIELD(stereo3DEnd);

		SOUND_DUMP_FIELD(threshold);
		SOUND_DUMP_FIELD(lockedLoopTime);

		SOUND_DUMP_SUBASSET(sndContext);
		SOUND_DUMP_SUBASSET(volumeFalloffCurve);
		SOUND_DUMP_SUBASSET(lpfCurve);
		SOUND_DUMP_SUBASSET(hpfCurve);
		SOUND_DUMP_SUBASSET(reverbSendCurve);

		sound["speakerMap"] = nullptr;
		if (asset->speakerMap)
		{
			json speakerMap;
			speakerMap["name"] = asset->speakerMap->name;
			speakerMap["isDefault"] = asset->speakerMap->isDefault;
			speakerMap["orientation"] = asset->speakerMap->orientation;

			json channelMaps;
			for (char x = 0; x < 2; x++)
			{
				for (char y = 0; y < 2; y++)
				{
					json channelMap;
					channelMap["speakerCount"] = asset->speakerMap->channelMaps[x][y].speakerCount;

					json speakers;
					for (int speaker = 0; speaker < asset->speakerMap->channelMaps[x][y].speakerCount; speaker++)
					{
						json jspeaker;

						jspeaker["speaker"] = asset->speakerMap->channelMaps[x][y].speakers[speaker].speaker;
						jspeaker["numLevels"] = asset->speakerMap->channelMaps[x][y].speakers[speaker].numLevels;
						jspeaker["levels0"] = asset->speakerMap->channelMaps[x][y].speakers[speaker].levels[0];
						jspeaker["levels1"] = asset->speakerMap->channelMaps[x][y].speakers[speaker].levels[1];

						speakers[speaker] = jspeaker;
					}

					channelMap["speakers"] = speakers;

					channelMaps[(x & 0x01) << 1 | y & 0x01] = channelMap;
				}
			}

			speakerMap["channelMaps"] = channelMaps;

			sound["speakerMap"] = speakerMap;
		}

		SOUND_DUMP_FIELD(allowDoppler);
		SOUND_DUMP_SUBASSET(dopplerPreset);
	}

	void sound::json_dump(snd_alias_list_t* asset)
	{
		const auto path = "sounds\\"s + asset->name + ".json"s;

		ordered_json sound;

		SOUND_DUMP_STRING(aliasName);
		SOUND_DUMP_FIELD(count);

		for (unsigned char i = 0; i < asset->count; i++)
		{
			ordered_json alias;
			json_dump_snd_alias(alias, &asset->head[i]);
			sound["head"][i] = alias;
		}

		for (unsigned char i = 0; i < asset->contextListCount; i++)
		{
			sound["contextList"][i]["aliasOffset"] = asset->contextList[i].aliasOffset;
			sound["contextList"][i]["count"] = asset->contextList[i].count;
		}

		std::string assetstr = sound.dump(4);

		auto file = filesystem::file(path);
		file.open();
		file.write(assetstr.data(), assetstr.size(), 1);
		file.close();
	}

	void sound::dump(snd_alias_list_t* asset)
	{
		json_dump(asset);
	}
}