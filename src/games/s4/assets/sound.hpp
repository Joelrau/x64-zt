#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	inline const std::string_view volmod_names[] =
	{
		"hud",
		"interface",
		"interface_music",
		"hitmarker",
		"music",
		"music_emitter",
		"music_emitter_pa",
		"ambience",
		"ambience_dist",
		"element",
		"emitter",
		"physics",
		"nuke_reactive_emitter",
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
		"chatteral",
		"chatterax",
		"reactional",
		"reactionax",
		"voiceover",
		"voiceover_radio",
		"voiceover_radio_3d",
		"voiceover_critical",
		"voiceover_amb",
		"voiceover_plr",
		"destruct",
		"explosion",
		"explosion_grenade",
		"explosion_flashbang",
		"explosion_rocket",
		"explosion_car",
		"rex_emitters",
		"impact",
		"impact_plr",
		"impact_npc",
		"impact_plr_to_npc",
		"impactmp",
		"impactmp_plr",
		"impactmp_npc",
		"impactmp_plr_to_npc",
		"whizby",
		"whizbymp",
		"vehicle_plr",
		"vehicle_npc",
		"vehicle_wpn_plr",
		"vehicle_wpn_npc",
		"vehicle",
		"grenadebounce",
		"grenadebouncemp",
		"shellcasings",
		"shellcasingsmp",
		"wpn_plr",
		"wpn_plr_mech",
		"wpn_plr_lfe",
		"wpn_plr_atmo",
		"wpnmp_plr",
		"wpn_npc",
		"wpnmp_npc",
		"wpn_projectile",
		"wpnmp_projectile",
		"wpn_reflections",
		"wpn_reflections_mid",
		"wpn_reflections_far",
		"wpn_reflections_dist",
		"na",
		"max",
		"scripted1",
		"scripted2",
		"scripted3",
		"scripted4",
		"scripted5",
		"fullvolume",
		"perkmp_quiet",
		"deathsdoor",
		"default",
	};

	inline const std::string_view entchannel_names[] =
	{
		"scn_fx_unres_3d",
		"scn_lfe_unres_3d",
		"scn_fx_unres_2d",
		"scn_lfe_unres_2d",
		"scn_fx_special_unres_3d",
		"scn_fx_special_unres_2d",
		"scn_fx_special_unres_prop_3d",
		"scn_fx_unres_3d_lim",
		"scn_fx_unres_2d_lim",
		"scn_fx_res_3d",
		"scn_fx_res_2d",
		"melee_plr_2d",
		"weap_plr_fire_1_2d",
		"weap_plr_fire_2_2d",
		"weap_plr_fire_3_2d",
		"weap_plr_fire_4_2d",
		"weap_plr_fire_overlap_2d",
		"weap_plr_fire_lfe_2d",
		"weap_casing_eject_3d",
		"weap_plr_fire_alt_1_2d",
		"weap_plr_fire_alt_2_2d",
		"weap_plr_fire_alt_3_2d",
		"weap_plr_fire_alt_4_2d",
		"weap_plr_fire_alt_overlap_2d",
		"weap_plr_fire_alt_lfe_2d",
		"foley_plr_step_2d",
		"foley_plr_step_unres_2d",
		"foley_plr_mvmt_unres_2d_lim",
		"foley_plr_weap_2d_lim",
		"reload_plr_res_2d",
		"reload_plr_res2_2d",
		"reload_plr_unres_2d",
		"spear_refl_front_unres_3d_lim",
		"spear_refl_rear_unres_3d_lim",
		"spear_refl_side_unres_3d_lim",
		"spear_refl_dist_unres_3d_lim",
		"spear_refl_npc_unres_3d_lim",
		"weap_npc_main_3d",
		"weap_npc_mech_3d",
		"weap_npc_atmo_3d",
		"weap_npc_atmo_alt_3d",
		"weap_npc_lfe_3d",
		"weap_npc_mid_3d",
		"weap_npc_mid_atmo_3d",
		"weap_npc_mid_atmo_alt_3d",
		"weap_npc_far_3d",
		"weap_npc_far_atmo_3d",
		"weap_npc_far_atmo_alt_3d",
		"weap_npc_dist_3d",
		"weap_npc_dist_atmo_3d",
		"weap_npc_dist_atmo_alt_3d",
		"weap_npc_sniper_mid_3d",
		"weap_npc_sniper_mid_atmo_3d",
		"weap_npc_sniper_mid_atmo_alt_3d",
		"weap_npc_sniper_far_3d",
		"weap_npc_sniper_far_atmo_3d",
		"weap_npc_sniper_far_atmo_alt_3d",
		"weap_npc_sniper_dist_3d",
		"weap_npc_sniper_dist_atmo_3d",
		"weap_npc_sniper_dist_atmo_alt_3d",
		"weap_npc_lo_3d",
		"melee_npc_3d",
		"plr_use_misc_unres_2d",
		"plr_ui_ingame_unres_2d",
		"plr_use_ps4spkr_res_2d",
		"equip_use_unres_3d",
		"equip_ui_ingame_unres_3d",
		"amb_plunder_loops_unres_3d",
		"amb_plunder_loops_close_unres_3d",
		"amb_plunder_loops_mech_3d",
		"pickup_plunder_unres_1_2d",
		"explo_main_3d",
		"explo_close_3d",
		"explo_body_3d",
		"explo_body2_3d",
		"explo_debris_3d",
		"explo_atmo_3d",
		"explo_atmo_alt_3d",
		"explo_lfe_3d",
		"explo_far_3d",
		"explo_dist_3d",
		"projectile_loop_close",
		"projectile_loop_mid",
		"projectile_loop_dist",
		"whizby_crack_unres_3d_lim",
		"whizby_in_unres_3d_lim",
		"whizby_out_unres_3d_lim",
		"bulletimpact_unres_3d_lim",
		"bulletimpact_lo_unres_3d_lim",
		"bullet_ricochets_unres_3d_lim",
		"bullet_rico_reflect_unres_3d_lim",
		"bullet_plr_1_unres_3d_lim",
		"bullet_plr_exit_unres_3d_lim",
		"hit_marker_unres_3d_lim",
		"hit_marker_final_res_2d_lim",
		"bulletflesh_1_unres_3d_lim",
		"bulletflesh_2_unres_3d_lim",
		"bulletflesh_lfe_unres_2d_lim",
		"bulletflesh_npc_1_unres_3d_lim",
		"bulletflesh_npc_2_unres_3d_lim",
		"bulletflesh_npcnpc1_unres_3d_lim",
		"bulletflesh_npcnpc2_unres_3d_lim",
		"melee_impact_3d",
		"foley_npc_step_3d",
		"foley_npc_step_unres_3d",
		"foley_npc_step_ceiling_3d",
		"foley_npc_parachute_land",
		"foley_npc_mvmt_unres_3d_lim",
		"foley_npc_weap_unres_3d_lim",
		"reload_npc_res_3d",
		"reload_npc_unres_3d",
		"voice_air_3d",
		"voice_no_occ_3d",
		"voice_radio_3d",
		"voice_radio_2d",
		"voice_narration_2d",
		"voice_special_2d",
		"voice_walla_1_3d",
		"voice_bchatter_1_3d",
		"voice_bchatter_lo_unres_3d_lim",
		"voice_plr_2d",
		"voice_plr_efforts_2d",
		"voice_plr_breath_2d",
		"voice_plr_breath_2d_alt",
		"voice_animal_1_3d",
		"amb_bed_2d",
		"amb_elm_unres_3d",
		"amb_elm_int_unres_3d",
		"amb_elm_ext_unres_3d",
		"amb_elm_unres_2d_lim",
		"amb_elm_unres_2d",
		"amb_elm_int_special_unres_3d",
		"amb_elm_ext_special_unres_3d",
		"rex_emitters_unres_3d_lim",
		"doors_3d",
		"vehicle_air_flyby_close_3d_lim",
		"vehicle_air_loops_3d_lim",
		"vehicle_air_flyby_midfar_3d_lim",
		"veh_weap_npc_main_3d",
		"veh_weap_npc_mech_3d",
		"veh_weap_npc_atmo_3d",
		"veh_weap_npc_atmo_alt_3d",
		"veh_weap_npc_lfe_3d",
		"vehicle_hi_res_3d",
		"vehicle_hi_unres_3d",
		"vehicle_lfe_3d",
		"vehicle_lo_3d_lim",
		"vehicle_unres_2d",
		"vehicle_unres_2d_lim",
		"shock_nofilter_2d",
		"hurt_nofilter_2d",
		"shock1_nofilter_3d",
		"shock2_nofilter_3d",
		"music_lr",
		"music_lsrs",
		"musicnopause_lr",
		"musicnopause_lsrs",
		"mus_emitter_3d",
		"menu_unres_2d",
		"menu_1_2d_lim",
		"menu_2_2d_lim",
		"physics_hi_unres_3d_lim",
		"physics_lo_unres_3d_lim",
		"bodyfalls_hi_unres_3d_lim",
		"bodyfalls_lo_unres_3d_lim",
		"special_hi_unres_1_3d",
		"special_hi_unres_1_2d",
		"special_mid_unres_1_3d",
		"special_lo_unres_1_3d",
		"special_lo_unres_1_2d",
		"pa_speaker",
		"pa_voice",
		"z_audio_test_music_unres_2d",
		"z_audio_test_wpn_unres_2d",
		"z_audio_test_wpn_lo_unres_2d",
		"z_audio_test_fx_unres_2d",
		"z_audio_test_voice_2d",
	};

	template <typename Walk>
	void walk_alias_list(Walk walk, SndAliasList& list);

	template <typename Walk>
	void walk_alias_list_pointer(Walk walk, SndAliasList*& field)
	{
		if (const auto list = walk.array(field, 1, 7))
		{
			walk_alias_list(walk, *list);
		}
	}

	template <typename Walk>
	void walk_alias_list(Walk walk, SndAliasList& list)
	{
		if (const auto aliases = walk.array(list.head, list.count, 7))
		{
			for (auto i = 0; i < list.count; i++)
			{
				walk.string(aliases[i].aliasName);
				walk.string(aliases[i].subtitle);
				walk_alias_list_pointer(walk, aliases[i].aliasList);
			}
		}
	}

	class sound_globals : public asset_interface
	{
	private:
		std::string name_;
		SndGlobals* asset_ = nullptr;

	public:
		SndGlobals* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, SndGlobals* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(SndGlobals* asset);
	};

	class sound_bank : public asset_interface
	{
	private:
		std::string name_;
		SndBankResident* asset_ = nullptr;

	public:
		SndBankResident* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, SndBankResident* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(SndBankResident* asset);
	};

	class sound_bank_transient : public asset_interface
	{
	private:
		std::string name_;
		SndBankTransient* asset_ = nullptr;

	public:
		SndBankTransient* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, SndBankTransient* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(SndBankTransient* asset);
	};

	class sfx_package : public asset_interface
	{
	private:
		std::string name_;
		WeaponSFXPackage* asset_ = nullptr;

	public:
		WeaponSFXPackage* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, WeaponSFXPackage* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(WeaponSFXPackage* asset);
	};

	class sound_bank_list
	{
	public:
		static void read(zone_reader& reader, SoundBankListDef* asset);
	};
}
