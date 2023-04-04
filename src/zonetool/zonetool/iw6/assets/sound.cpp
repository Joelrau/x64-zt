#include <std_include.hpp>
#include "sound.hpp"

namespace zonetool::iw6
{
	std::array<const char*, SND_VOLMOD_COUNT> volume_mod_groups =
	{
		// OLD AND TO BE DELETED
		"foley",
		// searched ready to remove
		"wpnai",
		"wpnplyr",
		// User interface
		"hud",
		"interface",
		"interface_music",
		// Music
		"music",
		"music_emitter",
		// Ambience
		"ambience",
		"ambience_dist",
		"element",
		"emitter",
		"physics",
		// Character sounds
		"bodyfall",
		"foley_plr",
		"foleymp_plr",
		"foley_npc",
		"foleymp_npc",
		"foley_wpn_plr",
		"foley_wpn_npc",
		"footstep_plr",
		"footstep_npc",
		"footstepmp_plr",
		"footstepmp_npc",
		"melee_plr",
		"melee_npc",
		// Dialogue
		"chatteral",
		"chatterax",
		"reactional",
		"reactionax",
		"voiceover",
		"voiceover_radio",
		"voiceover_critical",
		"voiceover_amb",
		// Explosions & Destruction
		"destruct",
		"explosion",
		"explosion_grenade",
		"explosion_flashbang",
		"explosion_rocket",
		"explosion_car",
		"rex_emitters",
		// Bullet Impacts & Whizbys
		"impact",
		"impact_plr",
		"impact_npc",
		"impactmp",
		"impactmp_plr",
		"impactmp_npc",
		"whizby",
		"whizbymp",
		// Vehicle sounds
		"vehicle_plr",
		"vehicle_npc",
		"vehicle_wpn_plr",
		"vehicle_wpn_npc",
		"vehicle",
		// Weapons
		"grenadebounce",
		"grenadebouncemp",
		"shellcasings",
		"shellcasingsmp",
		"wpn_plr",
		"wpnmp_plr",
		"wpn_npc",
		"wpnmp_npc",
		"wpn_projectile",
		"wpnmp_projectile",
		// Special use
		"na",
		"max",
		"scripted1",
		"scripted2",
		"scripted3",
		"scripted4",
		"scripted5",
		"fullvolume",
		// Level specific and MP perks
		"perkmp_quiet",
		"deathsdoor",
		// Default if left blank
		"default",
	};

	std::array<const char*, SND_CHANNEL_COUNT> channels =
	{
		"physics",
		"ambdist1",
		"ambdist2",
		"alarm",
		"auto",
		"auto2",
		"auto2d",
		"autodog",
		"explosiondist1",
		"explosiondist2",
		"explosiveimpact",
		"element",
		"element_int",
		"element_ext",
		"foley_plr_mvmt",
		"foley_plr_weap",
		"foley_npc_mvmt",
		"foley_npc_weap",
		"foley_dog_mvmt",
		"element_lim",
		"element2d",
		"voice_dog_dist",
		"bulletflesh1npc_npc",
		"bulletflesh2npc_npc",
		"bulletimpact",
		"bulletflesh1npc",
		"bulletflesh2npc",
		"bulletflesh1",
		"bulletflesh2",
		"vehicle",
		"vehiclelimited",
		"menu",
		"menulim1",
		"menulim2",
		"menulim2",
		"bulletwhizbyout",
		"body",
		"body2d",
		"reload",
		"reload2d",
		"foley_plr_step",
		"foley_plr_step_unres",
		"foley_npc_step",
		"foley_dog_step",
		"item",
		"weapon_drone",
		"explosion1",
		"explosion2",
		"explosion3",
		"explosion4",
		"explosion5",
		"effects1",
		"effects2",
		"effects3",
		"effects2d1",
		"effects2d2",
		"norestrict",
		"norestrict2d",
		"aircraft",
		"vehicle2d",
		"weapon_dist",
		"weapon_mid",
		"weapon",
		"weapon2d",
		"nonshock",
		"nonshock2",
		"effects2dlim",
		"voice_dog",
		"music_emitter",
		"voice_dog_attack",
		"voice",
		"local",
		"local2",
		"local3",
		"ambient",
		"plr_weap_fire_2d",
		"plr_weap_mech_2d",
		"hurt",
		"player1",
		"player2",
		"music",
		"musicnopause",
		"mission",
		"missionfx",
		"announcer",
		"shellshock",
	};

	const char* get_vol_mod_name(int index)
	{
		return volume_mod_groups[index];
	}

	const char* get_channel_name(int index)
	{
		return channels[index];
	}

	int get_vol_mod_index_from_name(const char* name)
	{
		for (int i = 0; i < volume_mod_groups.size(); i++)
		{
			const char* vol_mod = volume_mod_groups[i];
			if (!_stricmp(vol_mod, name))
			{
				return i;
			}
		}
		return -1;
	}

	int get_channel_index_from_name(const char* name)
	{
		for (int i = 0; i < channels.size(); i++)
		{
			const char* channel = channels[i];
			if (!_stricmp(channel, name))
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
		asset->entry = mem->allocate<__type__>(); \
		asset->entry->name = mem->duplicate_string(snddata[#entry].get<std::string>().data()); }
#define SOUND_STRING(entry) \
	if (!snddata[#entry].is_null()) { \
		asset->entry = mem->duplicate_string(snddata[#entry].get<std::string>().data()); \
	} else { asset->entry = nullptr; }
#define SOUND_CHAR(entry) \
	asset->entry = snddata[#entry].get<char>()
#define SOUND_SHORT(entry) \
	asset->entry = snddata[#entry].get<short>()
#define SOUND_INT(entry) \
	asset->entry = snddata[#entry].get<int>()
#define SOUND_FLOAT(entry) \
	asset->entry = snddata[#entry].get<float>()

	void sound::json_parse_snd_alias(snd_alias_t* asset, nlohmann::json snddata, zone_memory* mem)
	{
		SOUND_STRING(aliasName);
		SOUND_STRING(secondaryAliasName);
		SOUND_STRING(chainAliasName);
		SOUND_STRING(subtitle);
		SOUND_STRING(mixerGroup);

		asset->soundFile = mem->allocate<SoundFile>();
		asset->soundFile->type = snddata["soundfile"]["type"].get<snd_alias_type_t>();
		asset->soundFile->exists = snddata["soundfile"]["exists"].get<unsigned char>();

		if (asset->soundFile->type == SAT_LOADED)
		{
			asset->soundFile->u.loadSnd = mem->allocate<LoadedSound>();
			asset->soundFile->u.loadSnd->name = mem->duplicate_string(snddata["soundfile"]["name"].get<std::string>().data());
		}
		else if (asset->soundFile->type == SAT_STREAMED)
		{
			asset->soundFile->u.streamSnd.totalMsec = snddata["soundfile"]["totalMsec"].get<unsigned int>();
			asset->soundFile->u.streamSnd.filename.isLocalized = snddata["soundfile"]["isLocalized"].get<unsigned short>();
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

		if (snddata["flags"].is_object())
		{
			SoundAliasFlags flags{ 0 };
			flags.packed.looping = snddata["flags"]["looping"].get<int>();
			flags.packed.isMaster = snddata["flags"]["isMaster"].get<int>();
			flags.packed.isSlave = snddata["flags"]["isSlave"].get<int>();
			flags.packed.fullDryLevel = snddata["flags"]["fullDryLevel"].get<int>();
			flags.packed.noWetLevel = snddata["flags"]["noWetLevel"].get<int>();
			flags.packed.unknown1 = snddata["flags"]["unknown1"].get<int>();
			flags.packed.unknown2 = snddata["flags"]["unknown2"].get<int>();
			flags.packed.type = snddata["flags"]["type"].get<int>();
			flags.packed.channel = static_cast<SoundChannel>(get_channel_index_from_name(snddata["flags"]["channel"].get<std::string>().data()));
		}
		else if (snddata["flags"].is_number_integer())
		{
			asset->flags = snddata["flags"].get<int>();
		}

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

		SOUND_FLOAT(lfePercentage);
		SOUND_FLOAT(centerPercentage);

		SOUND_FLOAT(envelopMin);
		SOUND_FLOAT(envelopMax);
		SOUND_FLOAT(envelopPercentage);

		SOUND_FLOAT(wetMixOverride);

		SOUND_SUBASSET(volumeFalloffCurve, SndCurve);
		SOUND_SUBASSET(lpfCurve, SndCurve);
		SOUND_SUBASSET(reverbSendCurve, SndCurve);

		if (!snddata["speakerMap"].is_null())
		{
			asset->speakerMap = mem->allocate<SpeakerMap>();
			json speakerMap = snddata["speakerMap"];

			asset->speakerMap->name = mem->duplicate_string(speakerMap["name"].get<std::string>().data());
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

			SOUND_STRING(aliasName);
			SOUND_CHAR(count);

			asset->head = mem->allocate<snd_alias_t>(asset->count);

			json heads = snddata["head"];
			for (unsigned char i = 0; i < asset->count; i++)
			{
				json_parse_snd_alias(&asset->head[i], heads[i], mem);
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

			if (head->volumeFalloffCurve)
			{
				zone->add_asset_of_type(ASSET_TYPE_SOUND_CURVE, head->volumeFalloffCurve->name);
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

	void sound::write_head(zone_base* zone, zone_buffer* buf, snd_alias_t* dest)
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
			zone_buffer::clear_pointer(&dest->soundFile);
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

			zone_buffer::clear_pointer(&dest->speakerMap);
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

			zone_buffer::clear_pointer(&dest->head);
		}

		buf->pop_stream();
	}

	void sound::json_dump_snd_alias(ordered_json& sound, snd_alias_t* asset)
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
				sound["soundfile"]["isLocalized"] = asset->soundFile->u.primedSnd.filename.isLocalized;
				sound["soundfile"]["fileIndex"] = asset->soundFile->u.primedSnd.filename.fileIndex;

				sound["soundfile"]["packed"]["offset"] = 0;
				sound["soundfile"]["packed"]["length"] = 0;
				sound["soundfile"]["raw"]["dir"] = "";
				sound["soundfile"]["raw"]["name"] = "";

				if (asset->soundFile->u.primedSnd.filename.fileIndex)
				{
					sound["soundfile"]["packed"]["offset"] = asset->soundFile->u.primedSnd.filename.info.packed.offset;
					sound["soundfile"]["packed"]["length"] = asset->soundFile->u.primedSnd.filename.info.packed.length;
				}
				else
				{
					sound["soundfile"]["raw"]["dir"] = asset->soundFile->u.primedSnd.filename.info.raw.dir
						? asset->soundFile->u.primedSnd.filename.info.raw.dir
						: "";
					sound["soundfile"]["raw"]["name"] = asset->soundFile->u.primedSnd.filename.info.raw.name
						? asset->soundFile->u.primedSnd.filename.info.raw.name
						: "";
				}

				if (asset->soundFile->u.primedSnd.loadedPart)
				{
					sound["soundfile"]["name"] = asset->soundFile->u.primedSnd.loadedPart->name ? asset->soundFile->u.primedSnd.loadedPart->name : "";
				}

				sound["soundfile"]["dataOffset"] = asset->soundFile->u.primedSnd.dataOffset;
				sound["soundfile"]["totalSize"] = asset->soundFile->u.primedSnd.totalSize;
				sound["soundfile"]["primedCrc"] = asset->soundFile->u.primedSnd.primedCrc;
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

		SoundAliasFlags flags{};
		flags.intValue = asset->flags;

		sound["flags"] = {};
		sound["flags"]["looping"] = static_cast<int>(flags.packed.looping);
		sound["flags"]["isMaster"] = static_cast<int>(flags.packed.isMaster);
		sound["flags"]["isSlave"] = static_cast<int>(flags.packed.isSlave);
		sound["flags"]["fullDryLevel"] = static_cast<int>(flags.packed.fullDryLevel);
		sound["flags"]["noWetLevel"] = static_cast<int>(flags.packed.noWetLevel);
		sound["flags"]["unknown1"] = static_cast<int>(flags.packed.unknown1);
		sound["flags"]["unknown2"] = static_cast<int>(flags.packed.unknown2);
		sound["flags"]["type"] = static_cast<int>(flags.packed.type);
		sound["flags"]["channel"] = get_channel_name(flags.packed.channel);

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

		SOUND_DUMP_FLOAT(lfePercentage);
		SOUND_DUMP_FLOAT(centerPercentage);

		SOUND_DUMP_FLOAT(envelopMin);
		SOUND_DUMP_FLOAT(envelopMax);
		SOUND_DUMP_FLOAT(envelopPercentage);

		SOUND_DUMP_FLOAT(wetMixOverride);

		SOUND_DUMP_SUBASSET(volumeFalloffCurve);
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
	}

	void sound::json_dump(snd_alias_list_t* asset)
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