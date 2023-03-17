#include <std_include.hpp>
#include "sound.hpp"

namespace zonetool::h1
{
	std::array<const char*, SND_VOLMOD_COUNT> volume_mod_groups =
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
		"music_emitter",
		// Ambience
		"amb_quad",
		"amb_dist",
		"amb_damb",
		"amb_damb_big",
		"amb_emitter",
		"amb_emitter_int",
		"amb_emitter_ext",
		// Physics
		"physics",
		// Characters
		"bodyfall",
		"bodyfall_mech",
		"foley_plr",
		"foley_plr_special",
		"mp_foley_plr",
		"foley_npc",
		"foley_mech",
		"foleymp_npc",
		"wpn_foley_plr",
		"wpn_foley_npc",
		"wpn_foley_mech",
		"footstep_plr",
		"footstep_plr_special",
		"footstep_water_plr",
		"footstep_npc",
		"footstep_mech",
		"mp_footstep_plr",
		"mp_footstep_npc",
		"melee_plr",
		"melee_npc",
		"melee_hit",
		"plr_internal",
		"deaths_door",
		// Dialogue
		"chatter_friendly",
		"chatter_enemy",
		"chatter_atlas",
		"vox_friendly",
		"vox_enemy",
		"dialog",
		"dialog_radio",
		"dialog_critical",
		"dialog_amb",
		// Explosions & Destruction
		"destruct",
		"explosion",
		"explosion_grenade",
		"explosion_flashbang",
		"explosion_rocket",
		"explosion_car",
		"explosion_critical",
		"explosion_debris",
		"impact_critical",
		// Bullet Impacts & Whizbys
		"bullet_impact_geo",
		"bullet_impact_plr",
		"bullet_impact_npc",
		"mp_bullet_impact_geo",
		"mp_bullet_impact_plr",
		"mp_bullet_impact_npc",
		"bullet_whizby",
		"mp_bullet_whizby",
		"bullet_impact_special",
		// Vehicles
		"vehicle_plr",
		"vehicle_plr_special",
		"vehicle_npc",
		"vehicle_npc_special",
		"vehicle_avm_plr",
		"vehicle_avm_npc",
		"wpn_vehicle_plr",
		"wpn_vehicle_npc",
		"vehicle",
		"vehicle_aircraft",
		"vehicle_pdrone",
		"vehicle_drone_swarm",
		"vehicle_amb",
		// Weapons
		"grenadebounce",
		"mp_grenadebounce",
		"shellcasings",
		"mp_shellcasings",
		"wpn_plr",
		"wpn_plr_special",
		"wpn_npc_special",
		"mp_wpn_plr",
		"wpn_npc",
		"mp_wpn_npc",
		"wpn_projectile",
		"mp_wpn_projectile",
		// Weapons Detailed
		"wpn_plr_shot_special",
		"wpn_plr_shot",
		"wpn_plr_mech",
		"wpn_plr_sub",
		"wpn_plr_tail",
		"wpn_npc_shot",
		"wpn_npc_mech",
		"wpn_npc_sub",
		"wpn_npc_tail",
		"wpn_npc_dist",
		// Tactical
		"tactical_plr",
		"tactical_npc",
		"tactical_plr_special",
		"tactical_npc_special",
		// Attack Dogs
		"attack_dog",
		// Special use
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
		"fullvolume",
		"mp_pre_event",
		"mp_post_event",
		"mp_splash_notify",
		"mp_hit_marker",
		// Level specific and MP perks
		"mp_perk_quiet",
		// OLD
		"mw3_amb_damb",
		"mw3_amb_emitter",
		"mw3_amb_quad",
		"mw3_bodyfall",
		"mw3_bullet_impact_geo",
		"mw3_bullet_impact_npc",
		"mw3_bullet_impact_plr",
		"mw3_bullet_whizby",
		"mw3_chatter_enemy",
		"mw3_chatter_friendly",
		"mw3_DEPRECATED_default",
		"mw3_DEPRECATED_MAX",
		"mw3_DEPRECATED_na",
		"mw3_destruct",
		"mw3_dialog",
		"mw3_dialog_amb",
		"mw3_dialog_critical",
		"mw3_dialog_radio",
		"mw3_explosion",
		"mw3_explosion_car",
		"mw3_explosion_flashbang",
		"mw3_explosion_grenade",
		"mw3_explosion_rocket",
		"mw3_foley_npc",
		"mw3_footstep_npc",
		"mw3_frontend_sfx",
		"mw3_fullvolume",
		"mw3_grenadebounce",
		"mw3_melee_npc",
		"mw3_mp_bullet_impact_geo",
		"mw3_mp_bullet_impact_npc",
		"mw3_mp_foley_npc",
		"mw3_mp_footstep_npc",
		"mw3_mp_wpn_npc",
		"mw3_music",
		"mw3_physics",
		"mw3_scripted1",
		"mw3_scripted2",
		"mw3_scripted3",
		"mw3_scripted4",
		"mw3_scripted5",
		"mw3_shellcasings",
		"mw3_vehicle",
		"mw3_vehicle_plr",
		"mw3_vehicle_npc",
		"mw3_vox_enemy",
		"mw3_vox_friendly",
		"mw3_wpn_foley_npc",
		"mw3_wpn_foley_plr",
		"mw3_footstep_plr",
		"mw3_wpn_npc",
		"mw3_wpn_plr",
		"mw3_wpn_projectile",
		"mw3_wpn_vehicle_npc",
		"mw3_wpn_vehicle_plr",
	};

	std::array<const char*, SND_DSPBUS_COUNT> dsp_buses =
	{
		"default",
		"weapons",
		"weapons_dist",
		"ambience",
		"emitters",
		"quads",
		"music",
		"foley",
		"explosions",
		"explosions_critical",
		"tactical",
		"tactical_special",
		"vehicles",
		"air_vehicles",
		"int_vehicles_1",
		"int_vehicles_2",
		"voices",
		"voices_critical",
		"voices_unfiltered",
		"animals",
		"physics",
		"battlechatter",
		"interface",
		"scripted",
		"scripted1",
		"scripted2",
		"scripted3",
		"scripted4",
		"scripted5",
		"unoccluded",
		"whizbys",
		"notimescale",
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
#define SOUND_DUMP_CHAR(entry) \
	sound[#entry] = (char)asset->entry;
#define SOUND_DUMP_SHORT(entry) \
	sound[#entry] = (short)asset->entry;
#define SOUND_DUMP_INT(entry) \
	sound[#entry] = (int)asset->entry;
#define SOUND_DUMP_FLOAT(entry) \
	sound[#entry] = (float)asset->entry;

#define SOUND_SUBASSET(entry, __type__) \
	if (!snddata[#entry].is_null()) { \
		asset->entry = mem->Alloc<__type__>(); \
		asset->entry->name = mem->StrDup(snddata[#entry].get<std::string>().data()); }
#define SOUND_STRING(entry) \
	if (!snddata[#entry].is_null()) { \
		asset->entry = mem->StrDup(snddata[#entry].get<std::string>().data()); \
	} else { asset->entry = nullptr; }
#define SOUND_CHAR(entry) \
	asset->entry = snddata[#entry].get<char>()
#define SOUND_SHORT(entry) \
	asset->entry = snddata[#entry].get<short>()
#define SOUND_INT(entry) \
	asset->entry = snddata[#entry].get<int>()
#define SOUND_FLOAT(entry) \
	asset->entry = snddata[#entry].get<float>()

	void ISound::json_parse_snd_alias(snd_alias_t* asset, nlohmann::json snddata, ZoneMemory* mem)
	{
		SOUND_STRING(aliasName);
		SOUND_STRING(secondaryAliasName);
		SOUND_STRING(chainAliasName);
		SOUND_STRING(subtitle);
		SOUND_STRING(mixerGroup);

		asset->soundFile = mem->Alloc<SoundFile>();
		asset->soundFile->type = snddata["soundfile"]["type"].get<snd_alias_type_t>();
		asset->soundFile->exists = snddata["soundfile"]["exists"].get<bool>();

		if (asset->soundFile->type == SAT_LOADED)
		{
			asset->soundFile->u.loadSnd = mem->Alloc<LoadedSound>();
			asset->soundFile->u.loadSnd->name = mem->StrDup(snddata["soundfile"]["name"].get<std::string>().data());
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
				asset->soundFile->u.streamSnd.filename.info.raw.dir = mem->StrDup(snddata["soundfile"]["raw"]["dir"].get<std::string>().data());
				asset->soundFile->u.streamSnd.filename.info.raw.name = mem->StrDup(snddata["soundfile"]["raw"]["name"].get<std::string>().data());
			}
		}
		else if (asset->soundFile->type == SAT_PRIMED)
		{
			ZONETOOL_FATAL("SAT_PRIMED parsing is not supported yet.");
		}

		SOUND_INT(flags);
		SOUND_CHAR(priority);
		asset->dspBusIndex = get_dsp_bus_index_from_name(snddata["dspBus"].get<std::string>().data()); //SOUND_CHAR(dspBusIndex);
		asset->volModIndex = get_vol_mod_index_from_name(snddata["volMod"].get<std::string>().data()); //SOUND_SHORT(volModIndex);
		SOUND_FLOAT(volMin);
		SOUND_FLOAT(volMax);
		SOUND_FLOAT(pitchMin);
		SOUND_FLOAT(pitchMax);
		SOUND_FLOAT(distMin);
		SOUND_FLOAT(distMax);
		SOUND_FLOAT(velocityMin);
		SOUND_FLOAT(probability);
		SOUND_CHAR(sequence);
		SOUND_INT(startDelay);

		SOUND_CHAR(masterPriority);
		SOUND_FLOAT(masterPercentage);
		SOUND_FLOAT(slavePercentage);
		SOUND_CHAR(playbackPercentage);

		SOUND_FLOAT(lfePercentage);
		SOUND_FLOAT(centerPercentage);

		SOUND_SHORT(poly);
		SOUND_SHORT(polyGlobal);
		SOUND_CHAR(polyEntityType);
		SOUND_CHAR(polyGlobalType);

		SOUND_FLOAT(envelopMin);
		SOUND_FLOAT(envelopMax);
		SOUND_FLOAT(envelopPercentage);

		SOUND_FLOAT(reverbWetMixOverride);
		SOUND_FLOAT(reverbMultiplier);

		SOUND_FLOAT(smartPanDistance2d);
		SOUND_FLOAT(smartPanDistance3d);
		SOUND_FLOAT(smartPanAttenuation3d);

		SOUND_SHORT(stereo3dAngle);
		SOUND_FLOAT(stereo3dStart);
		SOUND_FLOAT(stereo3dEnd);

		SOUND_SUBASSET(sndContext, SndContext);
		SOUND_SUBASSET(sndCurve, SndCurve);
		SOUND_SUBASSET(lpfCurve, SndCurve);
		SOUND_SUBASSET(reverbSendCurve, SndCurve);

		if (!snddata["speakerMap"].is_null())
		{
			asset->speakerMap = mem->Alloc<SpeakerMap>();
			json speakerMap = snddata["speakerMap"];

			asset->speakerMap->name = mem->StrDup(speakerMap["name"].get<std::string>().data());
			asset->speakerMap->isDefault = speakerMap["isDefault"].get<bool>();

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

		SOUND_CHAR(allowDoppler);
		SOUND_SUBASSET(dopplerPreset, DopplerPreset);

		// parse all unknown things too
		json snd_unknown = snddata["unknown"];
		if (!snd_unknown.is_null())
		{
			auto pad0 = nlohmann::get_object_bytes(snd_unknown["pad"][0]);
			memcpy(asset->__pad0, pad0.data(), pad0.size());
			asset->u1 = snd_unknown["u1"].get<char>();
			asset->u2 = snd_unknown["u2"].get<float>();
		}
	}

	snd_alias_list_t* ISound::json_parse(const std::string& name, ZoneMemory* mem)
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

			snd_alias_list_t* asset = mem->Alloc<snd_alias_list_t>();

			SOUND_STRING(aliasName);
			SOUND_CHAR(count);

			asset->head = mem->Alloc<snd_alias_t>(asset->count);

			json heads = snddata["head"];
			for (unsigned char i = 0; i < asset->count; i++)
			{
				json_parse_snd_alias(&asset->head[i], heads[i], mem);
			}

			auto context_list = snddata["contextList"];
			if (context_list.is_array())
			{
				asset->contextListCount = static_cast<unsigned char>(context_list.size());
				asset->contextList = mem->Alloc<snd_alias_context_list>(asset->contextListCount);

				for (unsigned char i = 0; i < asset->contextListCount; i++)
				{
					asset->contextList[i].unk = context_list[i].get<short>();
				}
			}

			return asset;
		}

		return nullptr;
	}

	snd_alias_list_t* ISound::parse(const std::string& name, ZoneMemory* mem)
	{
		return json_parse(name, mem);
	}

	void ISound::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;
		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->StrDup(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name().data()).sound;
		}
	}

	void ISound::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void ISound::load_depending(IZone* zone)
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

			if (head->sndCurve)
			{
				zone->add_asset_of_type(ASSET_TYPE_SOUND_CURVE, head->sndCurve->name);
			}

			if (head->lpfCurve)
			{
				zone->add_asset_of_type(ASSET_TYPE_LPF_CURVE, head->lpfCurve->name);
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

	std::string ISound::name()
	{
		return this->name_;
	}

	std::int32_t ISound::type()
	{
		return ASSET_TYPE_SOUND;
	}

	void ISound::write_soundfile(IZone* zone, ZoneBuffer* buf, SoundFile* data)
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

	void ISound::write_head(IZone* zone, ZoneBuffer* buf, snd_alias_t* dest)
	{
		auto* data = dest;

		if (data->aliasName)
		{
			dest->aliasName = buf->write_str(data->aliasName);
		}

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

		if (data->mixerGroup)
		{
			dest->mixerGroup = buf->write_str(data->mixerGroup);
		}

		if (data->soundFile)
		{
			buf->align(3);
			write_soundfile(zone, buf, data->soundFile);
			ZoneBuffer::clear_pointer(&dest->soundFile);
		}

		if (data->sndContext)
		{
			dest->sndContext = static_cast<SndContext*>(zone->get_asset_pointer(
				ASSET_TYPE_SOUND_CONTEXT, data->sndContext->name));
		}

		if (data->sndCurve)
		{
			dest->sndCurve = static_cast<SndCurve*>(zone->get_asset_pointer(
				ASSET_TYPE_SOUND_CURVE, data->sndCurve->name));
		}

		if (data->lpfCurve)
		{
			dest->lpfCurve = static_cast<SndCurve*>(zone->get_asset_pointer(
				ASSET_TYPE_LPF_CURVE, data->lpfCurve->name));
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

			ZoneBuffer::clear_pointer(&dest->speakerMap);
		}

		if (data->dopplerPreset)
		{
			dest->dopplerPreset = static_cast<DopplerPreset*>(zone->get_asset_pointer(
				ASSET_TYPE_DOPPLER_PRESET, data->dopplerPreset->name));
		}
	}

	void ISound::write(IZone* zone, ZoneBuffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->head)
		{
			buf->align(3);
			auto* dest_sound = buf->write(data->head, data->count);

			for (unsigned char i = 0; i < data->count; i++)
			{
				write_head(zone, buf, &dest_sound[i]);
			}

			ZoneBuffer::clear_pointer(&dest->head);
		}

		if (data->contextList)
		{
			buf->align(1);
			buf->write(data->contextList, data->contextListCount);
			ZoneBuffer::clear_pointer(&dest->contextList);
		}

		buf->pop_stream();
	}

	void ISound::json_dump_snd_alias(ordered_json& sound, snd_alias_t* asset)
	{
		SOUND_DUMP_STRING(aliasName);
		SOUND_DUMP_STRING(secondaryAliasName);
		SOUND_DUMP_STRING(chainAliasName);
		SOUND_DUMP_STRING(subtitle);
		SOUND_DUMP_STRING(mixerGroup);

		// soundfile shit
		if (asset->soundFile)
		{
			sound["soundfile"]["type"] = asset->soundFile->type;
			sound["soundfile"]["exists"] = asset->soundFile->exists;

			auto insert_loaded = [&]()
			{
				sound["soundfile"]["name"] = asset->soundFile->u.loadSnd->name ? asset->soundFile->u.loadSnd->name : "";
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
				insert_loaded();
				insert_streamed();
				//ZONETOOL_FATAL("SAT_PRIMED dumping is not supported yet.");
			}
		}

		SOUND_DUMP_INT(flags);
		SOUND_DUMP_CHAR(priority);
		sound["dspBus"] = get_dsp_bus_name(asset->dspBusIndex); //SOUND_DUMP_CHAR(dspBusIndex);
		sound["volMod"] = get_vol_mod_name(asset->volModIndex); //SOUND_DUMP_SHORT(volModIndex);
		SOUND_DUMP_FLOAT(volMin);
		SOUND_DUMP_FLOAT(volMax);
		SOUND_DUMP_FLOAT(pitchMin);
		SOUND_DUMP_FLOAT(pitchMax);
		SOUND_DUMP_FLOAT(distMin);
		SOUND_DUMP_FLOAT(distMax);
		SOUND_DUMP_FLOAT(velocityMin);
		SOUND_DUMP_FLOAT(probability);
		SOUND_DUMP_INT(sequence);
		SOUND_DUMP_INT(startDelay);

		SOUND_DUMP_CHAR(masterPriority);
		SOUND_DUMP_FLOAT(masterPercentage);
		SOUND_DUMP_FLOAT(slavePercentage);
		SOUND_DUMP_CHAR(playbackPercentage);

		SOUND_DUMP_FLOAT(lfePercentage);
		SOUND_DUMP_FLOAT(centerPercentage);

		SOUND_DUMP_SHORT(poly);
		SOUND_DUMP_SHORT(polyGlobal);
		SOUND_DUMP_CHAR(polyEntityType);
		SOUND_DUMP_CHAR(polyGlobalType);

		SOUND_DUMP_FLOAT(envelopMin);
		SOUND_DUMP_FLOAT(envelopMax);
		SOUND_DUMP_FLOAT(envelopPercentage);

		SOUND_DUMP_FLOAT(reverbWetMixOverride);
		SOUND_DUMP_FLOAT(reverbMultiplier);

		SOUND_DUMP_FLOAT(smartPanDistance2d);
		SOUND_DUMP_FLOAT(smartPanDistance3d);
		SOUND_DUMP_FLOAT(smartPanAttenuation3d);

		SOUND_DUMP_SHORT(stereo3dAngle);
		SOUND_DUMP_FLOAT(stereo3dStart);
		SOUND_DUMP_FLOAT(stereo3dEnd);

		SOUND_DUMP_SUBASSET(sndContext);
		SOUND_DUMP_SUBASSET(sndCurve);
		SOUND_DUMP_SUBASSET(lpfCurve);
		SOUND_DUMP_SUBASSET(reverbSendCurve);

		sound["speakerMap"] = nullptr;
		if (asset->speakerMap)
		{
			json speakerMap;
			speakerMap["name"] = asset->speakerMap->name;
			speakerMap["isDefault"] = asset->speakerMap->isDefault;

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

		SOUND_DUMP_CHAR(allowDoppler);
		SOUND_DUMP_SUBASSET(dopplerPreset);

		// dump all unknown things too
		sound["unknown"]["pad"][0] = json::binary(std::vector<std::uint8_t>(asset->__pad0, asset->__pad0 + sizeof(asset->__pad0)));
		sound["unknown"]["u1"] = asset->u1;
		sound["unknown"]["u2"] = asset->u2;
	}

	void ISound::json_dump(snd_alias_list_t* asset)
	{
		const auto path = "sounds\\"s + asset->name + ".json"s;

		ordered_json sound;
		ordered_json aliases;
		ordered_json unknownArray;

		SOUND_DUMP_STRING(aliasName);
		SOUND_DUMP_CHAR(count);

		for (unsigned char i = 0; i < asset->count; i++)
		{
			ordered_json alias;
			json_dump_snd_alias(alias, &asset->head[i]);
			sound["head"][i] = alias;
		}

		for (unsigned char i = 0; i < asset->contextListCount; i++)
		{
			sound["contextList"][i] = asset->contextList[i].unk;
		}

		std::string assetstr = sound.dump(4);

		auto file = filesystem::file(path);
		file.open();
		file.write(assetstr.data(), assetstr.size(), 1);
		file.close();
	}

	void ISound::dump(snd_alias_list_t* asset)
	{
		json_dump(asset);
	}
}