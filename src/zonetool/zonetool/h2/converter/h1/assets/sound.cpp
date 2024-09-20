#include <std_include.hpp>

#include "zonetool/h2/converter/h1/include.hpp"
#include "sound.hpp"
#include "zonetool/h1/assets/sound.hpp"
#include "game/h2/game.hpp"

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace sound
		{
			namespace
			{
#define H1_SND_MAX_VOLUME_GROUPS 180
				std::array<const char*, H1_SND_MAX_VOLUME_GROUPS> h1_volume_mod_groups =
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

#define H1_SND_MAX_DSP_BUSES 32
				std::array<const char*, H1_SND_MAX_DSP_BUSES> h1_dsp_buses =
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

				std::unordered_map<std::string, const char*> mapped_volume_mod_groups =
				{
					{"default", "default"},
					{"hud", "hud"},
					{"frontend_sfx", "frontend_sfx"},
					{"frontend_music", "frontend_music"},
					{"sp_hit_alert_npc", "sp_hit_alert_npc"},
					{"music", "music"},
					{"music2", "music2"},
					{"music_big", "music_big"},
					{"music_so", "music"},
					{"amb_quad", "amb_quad"},
					{"amb_dist", "amb_dist"},
					{"amb_damb", "amb_damb"},
					{"amb_emitter", "amb_emitter"},
					{"amb_emitter_int", "amb_emitter_int"},
					{"amb_emitter_ext", "amb_emitter_ext"},
					{"physics", "physics"},
					{"bodyfall", "bodyfall"},
					{"deaths_door", "deaths_door"},
					{"foley_plr", "foley_plr"},
					{"foley_npc", "foley_npc"},
					{"footstep_plr", "footstep_plr"},
					{"footstep_plr_special", "footstep_plr_special"},
					{"footstep_water_plr", "footstep_water_plr"},
					{"footstep_npc", "footstep_npc"},
					{"melee_npc", "melee_npc"},
					{"melee_hit", "melee_hit"},
					{"mp_foley_npc", "mw3_mp_foley_npc"},
					{"mp_foley_plr", "mp_foley_plr"},
					{"mp_footstep_plr", "mp_footstep_plr"},
					{"mp_footstep_npc", "mp_footstep_npc"},
					{"plr_internal", "plr_internal"},
					{"wpn_foley_plr", "wpn_foley_plr"},
					{"wpn_foley_plr_beltfed", "wpn_foley_plr"},
					{"wpn_plr_foley_h2", "wpn_foley_plr"},
					{"wpn_foley_npc", "wpn_foley_npc"},
					{"melee_plr", "melee_plr"},
					{"voices_ally", "vox_friendly"},
					{"voices_enemy", "vox_enemy"},
					{"voices_enemy_death_kbp", "vox_enemy"},
					{"voices_dialog", "dialog"},
					{"voices_dialog_radio", "dialog_radio"},
					{"voices_dialog_critical", "dialog_critical"},
					{"voices_pa", "dialog_amb"},
					{"destruct", "destruct"},
					{"explosion", "explosion"},
					{"explosion_grenade", "explosion_grenade"},
					{"explosion_flashbang", "explosion_flashbang"},
					{"explosion_rocket", "explosion_rocket"},
					{"explosion_car", "explosion_car"},
					{"explosion_critical", "explosion_critical"},
					{"explosion_debris", "explosion_debris"},
					{"bullet_impact_geo", "bullet_impact_geo"},
					{"bullet_impact_geo_metal", "bullet_impact_geo"},
					{"bullet_impact_plr", "bullet_impact_plr"},
					{"bullet_impact_npc", "bullet_impact_npc"},
					{"mp_bullet_impact_geo", "mp_bullet_impact_geo"},
					{"mp_bullet_impact_plr", "mp_bullet_impact_plr"},
					{"mp_bullet_impact_npc", "mp_bullet_impact_npc"},
					{"bullet_whizby", "bullet_whizby"},
					{"mp_bullet_whizby", "mp_bullet_whizby"},
					{"vehicle_plr", "vehicle_plr"},
					{"vehicle_plr_special", "vehicle_plr_special"},
					{"vehicle_npc", "vehicle_npc"},
					{"vehicle_avm_plr", "vehicle_avm_plr"},
					{"vehicle_avm_npc", "vehicle_avm_npc"},
					{"wpn_vehicle_plr", "wpn_vehicle_plr"},
					{"wpn_vehicle_npc", "wpn_vehicle_npc"},
					{"wpn_vehicle_npc_h2", "wpn_vehicle_npc"},
					{"vehicle_aircraft", "vehicle_aircraft"},
					{"grenadebounce", "grenadebounce"},
					{"mp_grenadebounce", "mp_grenadebounce"},
					{"shellcasings", "shellcasings"},
					{"wpn_plr", "wpn_plr"},
					{"wpn_plr_special", "wpn_plr_special"},
					{"wpn_npc_special", "wpn_npc_special"},
					{"wpn_npc", "wpn_npc"},
					{"mp_wpn_npc", "mp_wpn_npc"},
					{"wpn_projectile", "wpn_projectile"},
					{"iw4_wpn_plr_shot", "wpn_plr_shot"},
					{"wpn_plr_shot_first", "wpn_plr_shot"},
					{"wpn_plr_shot_special", "wpn_plr_shot_special"},
					{"wpn_plr_shot", "wpn_plr_shot"},
					{"wpn_plr_punch", "wpn_plr_shot"},
					{"wpn_plr_mech", "wpn_plr_mech"},
					{"wpn_plr_sub", "wpn_plr_sub"},
					{"wpn_plr_lfe", "wpn_plr_shot"},
					{"wpn_plr_tail", "wpn_plr_tail"},
					{"wpn_plr_tail_int_med", "wpn_plr_tail"},
					{"wpn_plr_tail_int_sml", "wpn_plr_tail"},
					{"wpn_plr_tail_semi", "wpn_plr_tail"},
					{"iw4_wpn_npc_shot", "wpn_npc_shot"},
					{"wpn_npc_shot", "wpn_npc_shot"},
					{"wpn_npc_mech", "wpn_npc_mech"},
					{"wpn_npc_sub", "wpn_npc_sub"},
					{"wpn_npc_tail", "wpn_npc_tail"},
					{"wpn_npc_dist", "wpn_npc_dist"},
					{"h2_wpn_npc_shot_close", "wpn_npc_shot"},
					{"h2_wpn_npc_shot_mid", "wpn_npc_shot"},
					{"h2_wpn_npc_shot_far", "wpn_npc_shot"},
					{"h2_wpn_npc_shot_tail", "wpn_npc_shot"},
					{"h2_wpn_npc_shot_first", "wpn_npc_shot"},
					{"tactical_plr", "tactical_plr"},
					{"tactical_npc", "tactical_npc"},
					{"tactical_plr_special", "tactical_plr_special"},
					{"tactical_npc_special", "tactical_npc_special"},
					{"attack_dog", "attack_dog"},
					{"fullvolume", "fullvolume"},
					{"big_moment", "big_moment"},
					{"scripted1", "scripted1"},
					{"scripted2", "scripted2"},
					{"scripted3", "scripted3"},
					{"scripted4", "scripted4"},
					{"scripted5", "scripted5"},
					{"scripted6", "scripted6"},
					{"scripted7", "scripted7"},
					{"scripted8", "scripted8"},
					{"scripted9", "scripted9"},
					{"scripted10", "scripted10"},
					{"scripted11", "scripted10"},
					{"scripted12", "scripted10"},
					{"scripted13", "scripted10"},
					{"scripted14", "scripted10"},
					{"scripted15", "scripted10"},
					{"mp_pre_event", "mp_pre_event"},
					{"mp_splash_notify", "mp_splash_notify"},
					{"mp_hit_marker", "mp_hit_marker"},
				};

				std::unordered_map<std::string, const char*> mapped_dsp_buses =
				{
					{"default", "default"},
					{"ambiance_quad", "ambience"}, // not found
					{"animals", "animals"},
					{"emitters", "emitters"},
					{"emitters_ext", "emitters"}, // not found
					{"emitters_int", "emitters"}, // ^
					{"emitters_unoccluded", "emitters"}, // ^
					{"explosions", "explosions"},
					{"explosions_critical", "explosions_critical"},
					{"foley", "foley"},
					{"interface", "interface"},
					{"music", "music"},
					{"notimescale", "notimescale"},
					{"physics", "physics"},
					{"scripted1", "scripted1"},
					{"scripted2", "scripted2"},
					{"scripted3", "scripted3"},
					{"scripted4", "scripted4"},
					{"tactical", "tactical"},
					{"unoccluded", "unoccluded"},
					{"vehicles_air", "air_vehicles"},
					{"vehicles_land", "vehicles"}, // not found
					{"voices", "voices"},
					{"voices_env", "voices"}, // not found
					{"voices_critical", "voices_critical"},
					{"voices_unfiltered", "voices_unfiltered"},
					{"weapons_npc", "weapons"}, // not found
					{"weapons_npc_dist", "weapons_dist"}, // not found
					{"weapons_npc_mid", "weapons"}, // not found
					{"weapons_npc_tail", "weapons"}, // not found
					{"weapons_plr", "weapons"}, // not found
					{"whizbys", "whizbys"},
				};

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

				short get_h1_vol_mod_index_from_name(const char* name)
				{
					for (short i = 0; i < h1_volume_mod_groups.size(); i++)
					{
						const char* vol_mod = h1_volume_mod_groups[i];
						if (!_stricmp(vol_mod, name))
						{
							return i;
						}
					}
					return -1;
				}

				char get_h1_dsp_bus_index_from_name(const char* name)
				{
					for (char i = 0; i < h1_dsp_buses.size(); i++)
					{
						const char* dsp_bus = h1_dsp_buses[i];
						if (!_stricmp(dsp_bus, name))
						{
							return i;
						}
					}
					return -1;
				}

				short convert_vol_mod_group(short h2_vol_mod)
				{
					const auto h2_name = volume_mod_groups[h2_vol_mod];
					const auto name = mapped_volume_mod_groups[h2_name];
					return get_h1_vol_mod_index_from_name(name);
				}

				char convert_dsp_bus(short h2_dsp_bus)
				{
					const auto h2_name = dsp_buses[h2_dsp_bus];
					const auto name = mapped_dsp_buses[h2_name];
					return get_h1_dsp_bus_index_from_name(name);
				}
			}

			zonetool::h1::snd_alias_list_t* convert(zonetool::h2::snd_alias_list_t* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::snd_alias_list_t>();

				std::memcpy(new_asset, asset, sizeof(snd_alias_list_t));
				new_asset->head = allocator.allocate_array<zonetool::h1::snd_alias_t>(asset->count);
				for (auto i = 0u; i < asset->count; i++)
				{
					auto* head = &asset->head[i];
					auto* new_head = &new_asset->head[i];

					new_head->aliasName = head->aliasName;
					new_head->subtitle = head->subtitle;
					new_head->secondaryAliasName = head->secondaryAliasName;
					new_head->chainAliasName = head->chainAliasName;
					new_head->soundFile = allocator.allocate<zonetool::h1::SoundFile>();
					std::memcpy(new_head->soundFile, head->soundFile, sizeof(zonetool::h1::SoundFile));
					new_head->squelchName = head->squelchName;
					new_head->polyCount = head->polyCount;
					new_head->polyGlobalCount = head->polyGlobalCount;
					new_head->polyEntityType = head->polyEntityType;
					new_head->polyGlobalType = head->polyGlobalType;
					new_head->dspBusIndex = convert_dsp_bus(head->dspBusIndex);
					new_head->priority = head->priority;
					new_head->playCount = head->playCount;
					new_head->polyClass = head->polyClass;
					new_head->unk = head->unk;
					new_head->volMin = head->volMin;
					new_head->volMax = head->volMax;
					new_head->volModIndex = convert_vol_mod_group(head->volModIndex);
					new_head->pitchMin = head->pitchMin;
					new_head->pitchMax = head->pitchMax;
					new_head->distMin = head->distMin;
					new_head->distMax = head->distMax;
					new_head->velocityMin = head->velocityMin;
					new_head->flags = head->flags;
					new_head->masterPriority = head->masterPriority;
					new_head->masterPercentage = head->masterPercentage;
					new_head->slavePercentage = head->slavePercentage;
					new_head->playbackPercentage = head->playbackPercentage;
					new_head->probability = head->probability;
					new_head->variationType = head->variationType;
					new_head->sndContext = reinterpret_cast<zonetool::h1::SndContext*>(head->sndContext);
					new_head->sequence = head->sequence;
					new_head->lfePercentage = head->lfePercentage;
					new_head->centerPercentage = head->centerPercentage;
					new_head->startDelay = head->startDelay;
					new_head->volumeFalloffCurve = reinterpret_cast<zonetool::h1::SndCurve*>(head->volumeFalloffCurve);
					new_head->envelopMin = head->envelopMin;
					new_head->envelopMax = head->envelopMax;
					new_head->lpfCurve = reinterpret_cast<zonetool::h1::SndCurve*>(head->lpfCurve);

					new_head->reverbSendCurve = reinterpret_cast<zonetool::h1::SndCurve*>(head->reverbSendCurve);
					new_head->speakerMap = reinterpret_cast<zonetool::h1::SpeakerMap*>(head->speakerMap);
					new_head->wetMixOverride = head->wetMixOverride;
					new_head->focusPercentage = head->focusPercentage;
					new_head->smartpanDistance2d = head->smartpanDistance2d;
					new_head->smartpanDistance3d = head->smartpanDistance3d;
					new_head->smartpanAttenuation3d = head->smartpanAttenuation3d;
					new_head->minSmartpan2dContribution = head->minSmartpan2dContribution;
					new_head->stereo3DAngle = head->stereo3DAngle;
					new_head->stereo3DStart = head->stereo3DStart;
					new_head->stereo3DEnd = head->stereo3DEnd;
					new_head->allowDoppler = head->allowDoppler;
					new_head->dopplerPreset = reinterpret_cast<zonetool::h1::DopplerPreset*>(head->dopplerPreset);
					new_head->threshold = head->threshold;
					new_head->lockedLoopTime = head->lockedLoopTime;

					const auto convert_and_dump_streamed = [&]() -> bool
					{
						const auto alias_name = head->aliasName ? head->aliasName : "null";

						// dump soundfile
						if (!head->soundFile->exists)
						{
							ZONETOOL_ERROR("streamed file doesn't exist for sound %s", alias_name);
							return false;
						}

						const auto soundfile_index = head->soundFile->u.streamSnd.filename.fileIndex;
						if (!soundfile_index)
						{
							ZONETOOL_ERROR("Could not convert streamed data for sound %s", alias_name);
							return false;
						}

						std::string soundfile_path = utils::string::va("soundfile%d.pak", soundfile_index);
						if (head->soundFile->u.streamSnd.filename.isLocalized)
						{
							soundfile_path = utils::string::va("%s/%s_%s",
								::h2::game::SEH_GetCurrentLanguageName(),
								::h2::game::SEH_GetCurrentLanguageCode(),
								soundfile_path.data());
						}

						const auto db_fs = ::h2::game::DB_FSInitialize();
						const auto soundfile = db_fs->vftbl->OpenFile(db_fs, ::h2::game::Sys_Folder::SF_PAKFILE, soundfile_path.data());
						const auto _0 = gsl::finally([&]
						{
							db_fs->vftbl->Close(db_fs, soundfile);
						});

						if (!soundfile)
						{
							ZONETOOL_FATAL("could not open %s for read.", soundfile_path.data());
							return false;
						}

						const auto offset = head->soundFile->u.streamSnd.filename.info.packed.offset;
						const auto length = head->soundFile->u.streamSnd.filename.info.packed.length;

						std::vector<std::uint8_t> snd_data;
						snd_data.resize(length);

						// get data from offset
						db_fs->vftbl->Read(db_fs, soundfile, offset, length, snd_data.data());

						if (strncmp(reinterpret_cast<char*>(snd_data.data()), "fLaC", 4))
						{
							ZONETOOL_FATAL("streamed sound data for sound %s is not in fLaC format!", alias_name);
							return false;
						}

						std::string loaded_name = utils::string::va("%s_%d", alias_name, i);
						if (head->soundFile->u.streamSnd.filename.isLocalized)
						{
							loaded_name = utils::string::va("%s/%s_%s_%d",
								::h2::game::SEH_GetCurrentLanguageName(),
								::h2::game::SEH_GetCurrentLanguageCode(),
								alias_name,
								i);
						}
						loaded_name = utils::string::va("streamed/%s", loaded_name.data());

						auto path = utils::string::va("loaded_sound/%s%s", loaded_name.data(), ".flac");
						auto file = filesystem::file(path);
						file.open("wb");

						file.write(snd_data.data(), snd_data.size(), 1);

						file.close();

						// convert sound
						std::memset(&new_head->soundFile->u.streamSnd, 0, sizeof(new_head->soundFile->u.streamSnd));
						new_head->soundFile->type = zonetool::h1::SAT_LOADED;

						zonetool::h1::SoundAliasFlags flags{ 0 };
						flags.intValue = new_head->flags;
						flags.packed.type = zonetool::h1::SAT_LOADED;
						new_head->flags = flags.intValue;

						new_head->soundFile->u.loadSnd = allocator.allocate<zonetool::h1::LoadedSound>();
						auto* new_loaded = new_head->soundFile->u.loadSnd;
						new_loaded->name = allocator.duplicate_string(loaded_name);

						return true;
					};

					if (head->soundFile)
					{
						if (head->soundFile->type == SAT_STREAMED)
						{
							if (!convert_and_dump_streamed())
							{
								new_head->soundFile = allocator.allocate<zonetool::h1::SoundFile>();
								new_head->soundFile->exists = false;
								new_head->soundFile->type = zonetool::h1::SAT_STREAMED;
								new_head->soundFile->u.streamSnd.filename.info.raw.dir = "";
								new_head->soundFile->u.streamSnd.filename.info.raw.name = "";
							}
						}
					}
				}

				return new_asset;
			}

			void dump(zonetool::h2::snd_alias_list_t* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::sound::dump(converted_asset);
			}
		}
	}
}
