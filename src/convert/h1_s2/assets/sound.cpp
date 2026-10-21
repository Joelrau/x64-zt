#include <std_include.hpp>

#include "../include.hpp"
#include "sound.hpp"
#include "loadedsound.hpp"

namespace zonetool::h1::converter::s2::sound
{
	namespace
	{
		constexpr std::array<std::string_view, 145> s2_volume_mod_groups
		{
			"default", "hud", "frontend_sfx", "frontend_music", "frontend_music_force", "sp_hit_alert_npc", "music_big",
			"music_stinger", "music", "music2", "music_emitter", "music_obj_complete", "amb_quad", "amb_dist", "amb_damb",
			"amb_damb_big", "amb_damb_creepy", "amb_emitter", "amb_emitter_int", "amb_emitter_ext", "physics", "bodyfall",
			"bodyfall_mech", "foley_plr", "foley_plr_special", "mp_foley_plr", "foley_npc", "foley_mech", "foleymp_npc",
			"wpn_foley_plr", "wpn_foley_npc", "wpn_foley_mech", "footstep_plr", "footstep_plr_special", "footstep_water_plr",
			"footstep_npc", "footstep_npc_large", "footstep_npc_huge", "footstep_mech", "mp_footstep_plr", "mp_footstep_npc",
			"melee_plr", "melee_npc", "melee_hit", "plr_internal", "deaths_door", "chatter_friendly", "chatter_enemy",
			"chatter_atlas", "vox_friendly", "vox_enemy", "vox_zombie", "vox_zombie_grondo", "vox_plr", "vox_plr_breath",
			"dialog", "dialog_morale", "dialog_radio", "dialog_critical", "dialog_amb", "dialog_passthru", "destruct",
			"explosion", "explosion_grenade", "explosion_flashbang", "explosion_rocket", "explosion_car",
			"explosion_critical", "explosion_morale", "explosion_debris", "impact_critical", "bullet_impact_geo",
			"bullet_impact_plr", "bullet_impact_npc", "mp_bullet_impact_geo", "mp_bullet_impact_plr",
			"mp_bullet_impact_npc", "bullet_whizby", "mp_bullet_whizby", "bullet_impact_special", "vehicle_plr",
			"vehicle_plr_special", "vehicle_npc", "vehicle_npc_special", "vehicle_avm_plr", "vehicle_avm_npc",
			"wpn_vehicle_plr", "wpn_vehicle_npc", "vehicle", "vehicle_aircraft", "vehicle_pdrone", "vehicle_drone_swarm",
			"vehicle_amb", "grenadebounce", "mp_grenadebounce", "shellcasings", "mp_shellcasings", "wpn_plr", "mp_wpn_plr",
			"wpn_plr_special", "wpn_npc", "mp_wpn_npc", "wpn_npc_special", "wpn_projectile", "mp_wpn_projectile",
			"wpn_npc_mounted", "wpn_npc_mounted_alt", "wpn_plr_shot_special", "wpn_plr_shot", "wpn_plr_mech",
			"wpn_plr_sub", "wpn_plr_tail", "wpn_npc_shot", "wpn_npc_dist", "wpn_npc_mech", "wpn_npc_sub", "wpn_npc_tail",
			"tactical_plr", "tactical_npc", "tactical_plr_special", "tactical_npc_special", "prop", "attack_dog",
			"big_moment", "scripted1", "scripted2", "scripted3", "scripted4", "scripted5", "scripted6", "scripted7",
			"scripted8", "scripted9", "scripted10", "pa_wet", "pa_dry", "fullvolume", "mp_pre_event", "mp_post_event",
			"mp_splash_notify", "mp_hit_marker", "zmb_grondo", "zmb_horror_audio_vignette", "mp_perk_quiet", "mw3_legacy",
		};

		constexpr std::array<std::string_view, 35> s2_dsp_buses
		{
			"default", "weapons", "turret_weapons", "weapons_dist", "ambience", "emitters", "quads", "music", "foley",
			"explosions", "tactical", "tactical_special", "vehicles", "air_vehicles", "int_vehicles_1", "int_vehicles_2",
			"voices", "voices_critical", "voices_plr", "voices_unfiltered", "voices_soft_occlu", "voices_unoccluded",
			"animals", "physics", "battlechatter", "interface", "scripted", "scripted1", "scripted2", "scripted3",
			"scripted4", "scripted5", "unoccluded", "unocclu_no_timescale", "whizbys",
		};

		const std::unordered_map<std::string_view, std::string_view> h1_dsp_bus_renames
		{
			{"explosions_critical", "explosions"},
			{"notimescale", "unocclu_no_timescale"},
		};

		const char* alias_name(const snd_alias_t& alias)
		{
			return alias.aliasName ? alias.aliasName : "null";
		}

		template <std::size_t N>
		std::optional<std::size_t> find_name(const std::array<std::string_view, N>& names, const std::string_view name)
		{
			const auto entry = std::ranges::find(names, name);
			if (entry == names.end())
			{
				return {};
			}

			return static_cast<std::size_t>(entry - names.begin());
		}

		short convert_volume_mod(const snd_alias_t& alias)
		{
			if (alias.volModIndex >= 0 && alias.volModIndex < SND_VOLMOD_COUNT)
			{
				const std::string_view name = zonetool::h1::sound::get_vol_mod_name(alias.volModIndex);
				const auto s2_name = name.starts_with("mw3_") ? std::string_view("mw3_legacy") : name;
				if (const auto index = find_name(s2_volume_mod_groups, s2_name))
				{
					return static_cast<short>(*index);
				}
			}

			ZONETOOL_WARNING("%s: volume mod %d has no S2 group, the alias uses \"default\"", alias_name(alias), alias.volModIndex);
			return 0;
		}

		unsigned char convert_dsp_bus(const snd_alias_t& alias)
		{
			if (alias.dspBusIndex < SND_DSPBUS_COUNT)
			{
				std::string_view name = zonetool::h1::sound::get_dsp_bus_name(static_cast<char>(alias.dspBusIndex));
				if (const auto rename = h1_dsp_bus_renames.find(name); rename != h1_dsp_bus_renames.end())
				{
					name = rename->second;
				}

				if (const auto index = find_name(s2_dsp_buses, name))
				{
					return static_cast<unsigned char>(*index);
				}
			}

			ZONETOOL_WARNING("%s: dsp bus %u has no S2 bus, the alias uses \"default\"", alias_name(alias), alias.dspBusIndex);
			return 0;
		}

		int convert_flags(const int h1_flags, const zonetool::s2::SoundFile* file)
		{
			const auto source = static_cast<std::uint32_t>(h1_flags);

			// bits 0-5, 7, 8 and the file type match, h1 bits 10-15 are s2 bits 9-14
			auto flags = (source & 0x701BF) | ((source & 0xFC00) >> 1);
			flags &= ~zonetool::s2::SND_ALIAS_FLAG_USE_CONTEXT_LIST;

			if (file)
			{
				flags &= ~zonetool::s2::SND_ALIAS_FLAG_TYPE_MASK;
				flags |= file->type << 16;
			}

			return static_cast<int>(flags);
		}

		int convert_speaker(const int h1_speaker)
		{
			return h1_speaker < 4 ? h1_speaker : h1_speaker + 2;
		}

		void set_default_speaker_levels(zonetool::s2::SpeakerMap& map)
		{
			map.channelMaps[0][0].levels[0] = 0.5f;
			map.channelMaps[0][1].levels[0] = 0.5f;
			map.channelMaps[1][0].levels[0] = 1.0f;
			map.channelMaps[1][1].levels[1] = 1.0f;
		}

		zonetool::s2::SpeakerMap* convert_speaker_map(const SpeakerMap* asset, utils::memory::allocator& allocator)
		{
			auto* map = allocator.allocate<zonetool::s2::SpeakerMap>();
			map->name = asset && asset->name ? asset->name : "";
			const auto speaker_count = static_cast<int>(std::size(map->channelMaps[0]));

			for (auto input = 0; input < 2; input++)
			{
				for (auto speaker = 0; speaker < speaker_count; speaker++)
				{
					map->channelMaps[input][speaker].speaker = static_cast<unsigned char>(speaker);
					map->channelMaps[input][speaker].numLevels = static_cast<unsigned char>(input + 1);
				}
			}

			if (!asset)
			{
				set_default_speaker_levels(*map);
				return map;
			}

			for (auto input = 0; input < 2; input++)
			{
				const auto& source = asset->channelMaps[input][1];
				for (auto i = 0; i < source.speakerCount && i < static_cast<int>(std::size(source.speakers)); i++)
				{
					const auto& h1_levels = source.speakers[i];
					const auto speaker = convert_speaker(h1_levels.speaker);
					if (speaker < 0 || speaker >= speaker_count)
					{
						continue;
					}

					auto& levels = map->channelMaps[input][speaker];
					levels.levels[0] = h1_levels.levels[0];
					levels.levels[1] = h1_levels.levels[1];
				}
			}

			return map;
		}

		template <typename T>
		T* named_asset(const char* name, utils::memory::allocator& allocator)
		{
			if (!name)
			{
				return nullptr;
			}

			auto* asset = allocator.allocate<T>();
			asset->name = name;
			return asset;
		}

		const char* curve_name(const SndCurve* curve)
		{
			return curve ? curve->name : nullptr;
		}

		const StreamFileName* find_stream_file(const SoundFile* file)
		{
			if (file->type == SAT_STREAMED)
			{
				return &file->u.streamSnd.filename;
			}

			if (file->type == SAT_PRIMED)
			{
				return &file->u.primedSnd.streamedPart;
			}

			return nullptr;
		}

		const LoadedSound* find_loaded_sound(const SoundFile* file)
		{
			if (file->type == SAT_LOADED)
			{
				return file->u.loadSnd;
			}

			if (file->type == SAT_PRIMED)
			{
				return file->u.primedSnd.loadedPart;
			}

			return nullptr;
		}

		std::string stream_sound_name(const StreamFileName& filename, const snd_alias_t& alias, const int index)
		{
			if (filename.isLocalized)
			{
				return std::format("streamed/{}/{}_{}_{}", filesystem::get_language_name(), filesystem::get_language_code(),
					alias_name(alias), index);
			}

			return std::format("streamed/{}_{}", alias_name(alias), index);
		}

		const char* convert_stream_file(const StreamFileName& filename, const snd_alias_t& alias, const int index,
			utils::memory::allocator& allocator)
		{
			if (!filename.fileIndex)
			{
				return nullptr;
			}

			const auto data = zonetool::h1::loaded_sound::read_stream_file(filename);
			if (!data)
			{
				return nullptr;
			}

			const auto name = stream_sound_name(filename, alias, index);
			loaded_sound::write_audio(name, *data);
			return allocator.duplicate_string(name);
		}

		zonetool::s2::SoundFile* convert_sound_file(const snd_alias_t& alias, const int index, utils::memory::allocator& allocator)
		{
			const auto* source = alias.soundFile;
			if (!source)
			{
				return nullptr;
			}

			auto* file = allocator.allocate<zonetool::s2::SoundFile>();
			file->type = zonetool::s2::SAT_LOADED;
			file->exists = source->exists;

			const auto* loaded = find_loaded_sound(source);
			const char* loaded_name = loaded ? loaded->name : nullptr;

			if (const auto* stream = find_stream_file(source); stream && source->exists)
			{
				if (const auto* name = convert_stream_file(*stream, alias, index, allocator))
				{
					loaded_name = name;
				}
				else if (!loaded_name)
				{
					ZONETOOL_WARNING("%s: the streamed audio is missing", alias_name(alias));
					file->exists = false;
				}
			}

			file->u.loadSnd = named_asset<zonetool::s2::LoadedSound>(loaded_name, allocator);
			return file;
		}

		int sound_length_msec(const SoundFile* file)
		{
			if (!file)
			{
				return 0;
			}

			if (file->type == SAT_STREAMED)
			{
				return static_cast<int>(file->u.streamSnd.totalMsec);
			}

			const auto* loaded = find_loaded_sound(file);
			if (!loaded || !loaded->info.sampleRate)
			{
				return 0;
			}

			return static_cast<int>(static_cast<std::uint64_t>(loaded->info.numSamples) * 1000 / loaded->info.sampleRate);
		}

		void convert_alias(const snd_alias_t& alias, const int index, zonetool::s2::snd_alias_t& result,
			utils::memory::allocator& allocator)
		{
			result.aliasName = alias.aliasName;
			result.subtitle = alias.subtitle;
			result.secondaryAliasName = alias.secondaryAliasName;
			result.chainAliasName = alias.chainAliasName;
			result.soundFile = convert_sound_file(alias, index, allocator);
			result.squelchName = alias.squelchName;
			result.polyCount = alias.polyCount;
			result.dspBusIndex = convert_dsp_bus(alias);
			result.priority = alias.priority;
			result.playCount = alias.playCount;
			result.polyClass = alias.polyClass;
			result.volMin = alias.volMin;
			result.volMax = alias.volMax;
			result.volModIndex = convert_volume_mod(alias);
			result.pitchMin = alias.pitchMin;
			result.pitchMax = alias.pitchMax;
			result.distMin = alias.distMin;
			result.distMax = alias.distMax;
			result.velocityMin = alias.velocityMin;
			result.flags = convert_flags(alias.flags, result.soundFile);
			result.masterPriority = alias.masterPriority;
			result.masterPercentage = alias.masterPercentage;
			result.slavePercentage = alias.slavePercentage;
			result.playbackPercentage = alias.playbackPercentage;
			result.probability = alias.probability;
			result.variationType = static_cast<zonetool::s2::SndVariationType>(alias.variationType);
			result.sndContext = nullptr;
			result.sequence = static_cast<unsigned char>(alias.sequence);
			result.lfePercentage = alias.lfePercentage;
			result.centerPercentage = alias.centerPercentage;
			result.startDelay = alias.startDelay;
			result.volumeFalloffCurve = named_asset<zonetool::s2::SndCurve>(curve_name(alias.volumeFalloffCurve), allocator);
			result.lpfCurve = named_asset<zonetool::s2::DistCurve>("$default", allocator);
			result.reverbSendCurve = named_asset<zonetool::s2::SndCurve>(curve_name(alias.reverbSendCurve), allocator);
			result.speakerMap = convert_speaker_map(alias.speakerMap, allocator);
			result.wetMixOverride = alias.wetMixOverride;
			result.focusPercentage = alias.focusPercentage;
			result.smartpanDistance2d = alias.smartpanDistance2d;
			result.smartpanDistance3d = alias.smartpanDistance3d;
			result.smartpanAttenuation3d = alias.smartpanAttenuation3d;
			result.minSmartpan2dContribution = alias.minSmartpan2dContribution;
			result.stereo3DAngle = alias.stereo3DAngle;
			result.stereo3DStart = alias.stereo3DStart;
			result.stereo3DEnd = alias.stereo3DEnd;
			result.threshold = alias.threshold;
			result.unknown232 = 1.0f;
			result.lockedLoopTime = alias.lockedLoopTime;
			std::fill(std::begin(result.parameterModifierOps), std::end(result.parameterModifierOps) - 1, zonetool::s2::SND_MODIFIER_OP_NONE);
			result.parameterModifierOps[7] = zonetool::s2::SND_MODIFIER_OP_ADD;
			result.length = sound_length_msec(alias.soundFile);
		}

		zonetool::s2::SndCurve convert_curve(const SndCurve* asset)
		{
			zonetool::s2::SndCurve curve{};
			curve.isDefault = asset->isDefault;
			curve.name = asset->name;
			curve.knotCount = asset->knotCount;
			std::memcpy(curve.knots, asset->knots, sizeof(curve.knots));
			return curve;
		}
	}

	void dump(snd_alias_list_t* asset)
	{
		utils::memory::allocator allocator;

		zonetool::s2::snd_alias_list_t list{};
		list.name = asset->name;
		list.count = asset->count;
		list.head = allocator.allocate_array<zonetool::s2::snd_alias_t>(asset->count);
		for (auto i = 0; i < asset->count; i++)
		{
			convert_alias(asset->head[i], i, list.head[i], allocator);
		}

		zonetool::s2::sound::dump(&list);
		add_csv_row(zonetool::s2::type_to_string(zonetool::s2::ASSET_TYPE_SOUND), asset->name);
	}

	void dump_curve(SndCurve* asset)
	{
		auto curve = convert_curve(asset);
		zonetool::s2::snd_curve::dump(&curve);
		add_csv_row(zonetool::s2::type_to_string(zonetool::s2::ASSET_TYPE_SOUND_CURVE), asset->name);
	}

	void dump_reverb_curve(SndCurve* asset)
	{
		auto curve = convert_curve(asset);
		zonetool::s2::reverb_curve::dump(&curve);
		add_csv_row(zonetool::s2::type_to_string(zonetool::s2::ASSET_TYPE_REVERB_CURVE), asset->name);
	}
}
