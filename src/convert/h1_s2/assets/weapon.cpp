#include <std_include.hpp>

#include "../include.hpp"
#include "attachment.hpp"
#include "fxeffectdef.hpp"
#include "material.hpp"
#include "physcollmap.hpp"
#include "physpreset.hpp"
#include "sound.hpp"
#include "tracer.hpp"
#include "weapon.hpp"
#include "xanim.hpp"
#include "xmodel.hpp"

namespace zonetool::h1::converter::s2::weapon
{
	namespace
	{
		constexpr std::size_t model_count = 2;
		constexpr std::size_t hide_tag_count = 32;
		constexpr std::size_t notetrack_sound_count = 36;
		constexpr std::size_t notetrack_map_count = 16;
		constexpr std::size_t h1_surface_type_count = 53;
		constexpr std::size_t s2_surface_type_count = 62;

		const std::multimap<weapAnimFiles_t, zonetool::s2::weapAnimFiles_t> anim_map =
		{
			{WEAP_ANIM_ROOT, zonetool::s2::WEAP_ANIM_ROOT},
			{WEAP_ANIM_IDLE, zonetool::s2::WEAP_ANIM_IDLE},
			{WEAP_ANIM_EMPTY_IDLE, zonetool::s2::WEAP_ANIM_EMPTY_IDLE},
			{WEAP_ANIM_FIRE, zonetool::s2::WEAP_ANIM_FIRE},
			{WEAP_ANIM_HOLD_FIRE, zonetool::s2::WEAP_ANIM_HOLD_FIRE},
			{WEAP_ANIM_LASTSHOT, zonetool::s2::WEAP_ANIM_LASTSHOT},
			{WEAP_ANIM_RECHAMBER, zonetool::s2::WEAP_ANIM_RECHAMBER},
			{WEAP_ANIM_GRENADE_PRIME, zonetool::s2::WEAP_ANIM_GRENADE_PRIME},
			{WEAP_ANIM_GRENADE_PRIME_READY_TO_THROW, zonetool::s2::WEAP_ANIM_GRENADE_PRIME_READY_TO_THROW},
			{WEAP_ANIM_MELEE_SWIPE, zonetool::s2::WEAP_ANIM_MELEE_SWIPE},
			// h1 lunges with its fatal anim whatever the victim does and never plays its other melee anims
			{WEAP_ANIM_MELEE_FATAL, zonetool::s2::WEAP_ANIM_MELEE_HIT},
			{WEAP_ANIM_MELEE_FATAL, zonetool::s2::WEAP_ANIM_MELEE_FATAL},
			{WEAP_ANIM_MELEE_FATAL, zonetool::s2::WEAP_ANIM_MELEE_VICTIM_CROUCHING_HIT},
			{WEAP_ANIM_MELEE_FATAL, zonetool::s2::WEAP_ANIM_MELEE_VICTIM_CROUCHING_FATAL},
			{WEAP_ANIM_MELEE_MISS, zonetool::s2::WEAP_ANIM_MELEE_MISS},
			{WEAP_ANIM_MELEE_MISS, zonetool::s2::WEAP_ANIM_MELEE_VICTIM_CROUCHING_MISS},
			{WEAP_ANIM_RELOAD, zonetool::s2::WEAP_ANIM_RELOAD},
			{WEAP_ANIM_RELOAD_EMPTY, zonetool::s2::WEAP_ANIM_RELOAD_EMPTY},
			{WEAP_ANIM_RELOAD_START, zonetool::s2::WEAP_ANIM_RELOAD_START},
			{WEAP_ANIM_RELOAD_END, zonetool::s2::WEAP_ANIM_RELOAD_END},
			{WEAP_ANIM_FAST_RELOAD, zonetool::s2::WEAP_ANIM_FAST_RELOAD},
			{WEAP_ANIM_FAST_RELOAD_EMPTY, zonetool::s2::WEAP_ANIM_FAST_RELOAD_EMPTY},
			{WEAP_ANIM_FAST_RELOAD_START, zonetool::s2::WEAP_ANIM_FAST_RELOAD_START},
			{WEAP_ANIM_FAST_RELOAD_END, zonetool::s2::WEAP_ANIM_FAST_RELOAD_END},
			{WEAP_ANIM_DUALMAG_RELOAD, zonetool::s2::WEAP_ANIM_DUALMAG_RELOAD},
			{WEAP_ANIM_DUALMAG_RELOAD_EMPTY, zonetool::s2::WEAP_ANIM_DUALMAG_RELOAD_EMPTY},
			{WEAP_ANIM_SPEED_RELOAD, zonetool::s2::WEAP_ANIM_SPEED_RELOAD},
			{WEAP_ANIM_RELOAD_FROM_ALT, zonetool::s2::WEAP_ANIM_RELOAD_FROM_ALT},
			{WEAP_ANIM_RAISE, zonetool::s2::WEAP_ANIM_RAISE},
			{WEAP_ANIM_FIRST_RAISE, zonetool::s2::WEAP_ANIM_FIRST_RAISE},
			{WEAP_ANIM_BREACH_RAISE, zonetool::s2::WEAP_ANIM_BREACH_RAISE},
			{WEAP_ANIM_DROP, zonetool::s2::WEAP_ANIM_DROP},
			{WEAP_ANIM_ALT_RAISE, zonetool::s2::WEAP_ANIM_ALT_RAISE},
			{WEAP_ANIM_ALT_DROP, zonetool::s2::WEAP_ANIM_ALT_DROP},
			{WEAP_ANIM_ALT_OVERRIDE, zonetool::s2::WEAP_ANIM_ALT_OVERRIDE},
			{WEAP_ANIM_QUICK_RAISE, zonetool::s2::WEAP_ANIM_QUICK_RAISE},
			{WEAP_ANIM_QUICK_DROP, zonetool::s2::WEAP_ANIM_QUICK_DROP},
			{WEAP_ANIM_EMPTY_RAISE, zonetool::s2::WEAP_ANIM_EMPTY_RAISE},
			{WEAP_ANIM_EMPTY_DROP, zonetool::s2::WEAP_ANIM_EMPTY_DROP},
			{WEAP_ANIM_HYBRID_SIGHT_ON, zonetool::s2::WEAP_ANIM_HYBRID_SIGHT_ON},
			{WEAP_ANIM_HYBRID_SIGHT_OFF, zonetool::s2::WEAP_ANIM_HYBRID_SIGHT_OFF},
			{WEAP_ANIM_SPRINT_IN, zonetool::s2::WEAP_ANIM_SPRINT_IN},
			{WEAP_ANIM_SPRINT_IN_CANCEL, zonetool::s2::WEAP_ANIM_SPRINT_IN_CANCEL},
			{WEAP_ANIM_SPRINT_LOOP, zonetool::s2::WEAP_ANIM_SPRINT_LOOP},
			{WEAP_ANIM_SPRINT_OUT, zonetool::s2::WEAP_ANIM_SPRINT_OUT},
			{WEAP_ANIM_SPRINTANDFIRE_IN, zonetool::s2::WEAP_ANIM_SPRINTANDFIRE_IN},
			{WEAP_ANIM_SPRINTANDFIRE_LOOP, zonetool::s2::WEAP_ANIM_SPRINTANDFIRE_LOOP},
			{WEAP_ANIM_SPRINTANDFIRE_OUT, zonetool::s2::WEAP_ANIM_SPRINTANDFIRE_OUT},
			{WEAP_ANIM_STUNNED_START, zonetool::s2::WEAP_ANIM_STUNNED_START},
			{WEAP_ANIM_STUNNED_LOOP, zonetool::s2::WEAP_ANIM_STUNNED_LOOP},
			{WEAP_ANIM_STUNNED_END, zonetool::s2::WEAP_ANIM_STUNNED_END},
			{WEAP_ANIM_DETONATE, zonetool::s2::WEAP_ANIM_DETONATE},
			{WEAP_ANIM_NIGHTVISION_WEAR, zonetool::s2::WEAP_ANIM_NIGHTVISION_WEAR},
			{WEAP_ANIM_NIGHTVISION_REMOVE, zonetool::s2::WEAP_ANIM_NIGHTVISION_REMOVE},
			{WEAP_ANIM_ADS_FIRE, zonetool::s2::WEAP_ANIM_ADS_FIRE},
			{WEAP_ANIM_ADS_LASTSHOT, zonetool::s2::WEAP_ANIM_ADS_LASTSHOT},
			{WEAP_ANIM_ADS_RECHAMBER, zonetool::s2::WEAP_ANIM_ADS_RECHAMBER},
			{WEAP_ANIM_BLAST_FRONT, zonetool::s2::WEAP_ANIM_BLAST_FRONT},
			{WEAP_ANIM_BLAST_RIGHT, zonetool::s2::WEAP_ANIM_BLAST_RIGHT},
			{WEAP_ANIM_BLAST_BACK, zonetool::s2::WEAP_ANIM_BLAST_BACK},
			{WEAP_ANIM_BLAST_LEFT, zonetool::s2::WEAP_ANIM_BLAST_LEFT},
			{WEAP_ANIM_SLIDE_IN, zonetool::s2::WEAP_ANIM_SLIDE_IN},
			{WEAP_ANIM_SLIDE_LOOP, zonetool::s2::WEAP_ANIM_SLIDE_LOOP},
			{WEAP_ANIM_SLIDE_OUT, zonetool::s2::WEAP_ANIM_SLIDE_OUT},
			{WEAP_ANIM_RECOIL_SETTLE, zonetool::s2::WEAP_ANIM_RECOIL_SETTLE},
			{WEAP_ANIM_SWIM_LOOP, zonetool::s2::WEAP_ANIM_SWIM_LOOP},
			{WEAP_ANIM_MANTLE_UP_64, zonetool::s2::WEAP_ANIM_MANTLE_UP_64},
			{WEAP_ANIM_MANTLE_UP_56, zonetool::s2::WEAP_ANIM_MANTLE_UP_56},
			{WEAP_ANIM_MANTLE_UP_48, zonetool::s2::WEAP_ANIM_MANTLE_UP_48},
			{WEAP_ANIM_MANTLE_UP_40, zonetool::s2::WEAP_ANIM_MANTLE_UP_40},
			{WEAP_ANIM_MANTLE_UP_32, zonetool::s2::WEAP_ANIM_MANTLE_UP_32},
			{WEAP_ANIM_MANTLE_UP_24, zonetool::s2::WEAP_ANIM_MANTLE_UP_24},
			{WEAP_ANIM_MANTLE_UP_16, zonetool::s2::WEAP_ANIM_MANTLE_UP_16},
			{WEAP_ANIM_MANTLE_OVER_64, zonetool::s2::WEAP_ANIM_MANTLE_OVER_64},
			{WEAP_ANIM_MANTLE_OVER_56, zonetool::s2::WEAP_ANIM_MANTLE_OVER_56},
			{WEAP_ANIM_MANTLE_OVER_48, zonetool::s2::WEAP_ANIM_MANTLE_OVER_48},
			{WEAP_ANIM_MANTLE_OVER_40, zonetool::s2::WEAP_ANIM_MANTLE_OVER_40},
			{WEAP_ANIM_MANTLE_OVER_32, zonetool::s2::WEAP_ANIM_MANTLE_OVER_32},
			{WEAP_ANIM_MANTLE_OVER_24, zonetool::s2::WEAP_ANIM_MANTLE_OVER_24},
			{WEAP_ANIM_MANTLE_OVER_16, zonetool::s2::WEAP_ANIM_MANTLE_OVER_16},
			{WEAP_ANIM_GOLIATH_ENTRY, zonetool::s2::WEAP_ANIM_GOLIATH_ENTRY},
			{WEAP_ANIM_OFFHAND_SWITCH, zonetool::s2::WEAP_ANIM_OFFHAND_SWITCH},
			{WEAP_ANIM_HEAT_COOLDOWN_IN, zonetool::s2::WEAP_ANIM_HEAT_COOLDOWN_IN},
			{WEAP_ANIM_HEAT_COOLDOWN_OUT, zonetool::s2::WEAP_ANIM_HEAT_COOLDOWN_OUT},
			{WEAP_ANIM_OVERHEAT_OUT, zonetool::s2::WEAP_ANIM_OVERHEAT_OUT},
			{WEAP_ANIM_SCRIPTED, zonetool::s2::WEAP_ANIM_SCRIPTED},
			{WEAP_ANIM_INSPECTION, zonetool::s2::WEAP_ANIM_INSPECTION},
			{WEAP_ANIM_RELOAD_MULTIPLE_1, zonetool::s2::WEAP_ANIM_RELOAD_MULTIPLE_1},
			{WEAP_ANIM_RELOAD_MULTIPLE_2, zonetool::s2::WEAP_ANIM_RELOAD_MULTIPLE_2},
			{WEAP_ANIM_RELOAD_MULTIPLE_3, zonetool::s2::WEAP_ANIM_RELOAD_MULTIPLE_3},
			{WEAP_ANIM_RELOAD_MULTIPLE_4, zonetool::s2::WEAP_ANIM_RELOAD_MULTIPLE_4},
			{WEAP_ANIM_RELOAD_MULTIPLE_5, zonetool::s2::WEAP_ANIM_RELOAD_MULTIPLE_5},
			{WEAP_ANIM_RELOAD_MULTIPLE_6, zonetool::s2::WEAP_ANIM_RELOAD_MULTIPLE_6},
			{WEAP_ANIM_RELOAD_MULTIPLE_7, zonetool::s2::WEAP_ANIM_RELOAD_MULTIPLE_7},
			{WEAP_ANIM_RELOAD_MULTIPLE_8, zonetool::s2::WEAP_ANIM_RELOAD_MULTIPLE_8},
			{WEAP_ANIM_RELOAD_MULTIPLE_FAST_1, zonetool::s2::WEAP_ANIM_RELOAD_MULTIPLE_FAST_1},
			{WEAP_ANIM_RELOAD_MULTIPLE_FAST_2, zonetool::s2::WEAP_ANIM_RELOAD_MULTIPLE_FAST_2},
			{WEAP_ANIM_RELOAD_MULTIPLE_FAST_3, zonetool::s2::WEAP_ANIM_RELOAD_MULTIPLE_FAST_3},
			{WEAP_ANIM_RELOAD_MULTIPLE_FAST_4, zonetool::s2::WEAP_ANIM_RELOAD_MULTIPLE_FAST_4},
			{WEAP_ANIM_RELOAD_MULTIPLE_FAST_5, zonetool::s2::WEAP_ANIM_RELOAD_MULTIPLE_FAST_5},
			{WEAP_ANIM_RELOAD_MULTIPLE_FAST_6, zonetool::s2::WEAP_ANIM_RELOAD_MULTIPLE_FAST_6},
			{WEAP_ANIM_RELOAD_MULTIPLE_FAST_7, zonetool::s2::WEAP_ANIM_RELOAD_MULTIPLE_FAST_7},
			{WEAP_ANIM_RELOAD_MULTIPLE_FAST_8, zonetool::s2::WEAP_ANIM_RELOAD_MULTIPLE_FAST_8},
			{WEAP_ANIM_ADS_UP, zonetool::s2::WEAP_ANIM_ADS_UP},
			{WEAP_ANIM_ADS_DOWN, zonetool::s2::WEAP_ANIM_ADS_DOWN},
			{WEAP_ANIM_RECOIL, zonetool::s2::WEAP_ANIM_RECOIL},
			{WEAP_ANIM_ADDITIVE_ADS_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_ADS_ROOT},
			{WEAP_ANIM_ADDITIVE_ADS_UP, zonetool::s2::WEAP_ANIM_ADDITIVE_ADS_UP},
			{WEAP_ANIM_ADDITIVE_DRAG_LEFT_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_DRAG_LEFT_ROOT},
			{WEAP_ANIM_ADDITIVE_DRAG_LEFT, zonetool::s2::WEAP_ANIM_ADDITIVE_DRAG_LEFT},
			{WEAP_ANIM_ADDITIVE_DRAG_RIGHT_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_DRAG_RIGHT_ROOT},
			{WEAP_ANIM_ADDITIVE_DRAG_RIGHT, zonetool::s2::WEAP_ANIM_ADDITIVE_DRAG_RIGHT},
			{WEAP_ANIM_ADDITIVE_DRAG_UP_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_DRAG_UP_ROOT},
			{WEAP_ANIM_ADDITIVE_DRAG_UP, zonetool::s2::WEAP_ANIM_ADDITIVE_DRAG_UP},
			{WEAP_ANIM_ADDITIVE_DRAG_DOWN_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_DRAG_DOWN_ROOT},
			{WEAP_ANIM_ADDITIVE_DRAG_DOWN, zonetool::s2::WEAP_ANIM_ADDITIVE_DRAG_DOWN},
			{WEAP_ANIM_ADDITIVE_JUMP_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_JUMP_ROOT},
			{WEAP_ANIM_ADDITIVE_JUMP, zonetool::s2::WEAP_ANIM_ADDITIVE_JUMP},
			{WEAP_ANIM_ADDITIVE_JUMP_BOOST, zonetool::s2::WEAP_ANIM_ADDITIVE_JUMP_BOOST},
			{WEAP_ANIM_ADDITIVE_JUMP_LAND_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_JUMP_LAND_ROOT},
			{WEAP_ANIM_ADDITIVE_JUMP_LAND, zonetool::s2::WEAP_ANIM_ADDITIVE_JUMP_LAND},
			{WEAP_ANIM_ADDITIVE_JUMP_LAND_HEAVY, zonetool::s2::WEAP_ANIM_ADDITIVE_JUMP_LAND_HEAVY},
			{WEAP_ANIM_ADDITIVE_WALK_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_WALK_ROOT},
			{WEAP_ANIM_ADDITIVE_WALK, zonetool::s2::WEAP_ANIM_ADDITIVE_WALK},
			{WEAP_ANIM_ADDITIVE_HEAT_COOLDOWN_LOOP_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_HEAT_COOLDOWN_LOOP_ROOT},
			{WEAP_ANIM_ADDITIVE_HEAT_COOLDOWN_LOOP, zonetool::s2::WEAP_ANIM_ADDITIVE_HEAT_COOLDOWN_LOOP},
			{WEAP_ANIM_ADDITIVE_OVERHEAT_IN_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_OVERHEAT_IN_ROOT},
			{WEAP_ANIM_ADDITIVE_OVERHEAT_IN, zonetool::s2::WEAP_ANIM_ADDITIVE_OVERHEAT_IN},
			{WEAP_ANIM_ADDITIVE_OVERHEAT_LOOP_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_OVERHEAT_LOOP_ROOT},
			{WEAP_ANIM_ADDITIVE_OVERHEAT_LOOP, zonetool::s2::WEAP_ANIM_ADDITIVE_OVERHEAT_LOOP},
			{WEAP_ANIM_ADDITIVE_CRAWL_IN_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_CRAWL_IN_ROOT},
			{WEAP_ANIM_ADDITIVE_CRAWL_IN, zonetool::s2::WEAP_ANIM_ADDITIVE_CRAWL_IN},
			{WEAP_ANIM_ADDITIVE_CRAWL_LOOP_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_ROOT},
			{WEAP_ANIM_ADDITIVE_CRAWL_LOOP, zonetool::s2::WEAP_ANIM_ADDITIVE_CRAWL_LOOP},
			{WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT_ROOT},
			{WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT, zonetool::s2::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT},
			{WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT_ROOT},
			{WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT, zonetool::s2::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT},
			{WEAP_ANIM_ADDITIVE_PRONE_DROP_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_PRONE_DROP_ROOT},
			{WEAP_ANIM_ADDITIVE_PRONE_DROP, zonetool::s2::WEAP_ANIM_ADDITIVE_PRONE_DROP},
			{WEAP_ANIM_ADDITIVE_EMPTY_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_EMPTY_ROOT},
			{WEAP_ANIM_ADDITIVE_EMPTY, zonetool::s2::WEAP_ANIM_ADDITIVE_EMPTY},
			{WEAP_ANIM_ADDITIVE_SCRIPTED_ROOT, zonetool::s2::WEAP_ANIM_ADDITIVE_SCRIPTED_ROOT},
			{WEAP_ANIM_ADDITIVE_SCRIPTED, zonetool::s2::WEAP_ANIM_ADDITIVE_SCRIPTED},
		};

		std::unordered_set<std::string> dumped_weapons;

		enum class weapon_profile
		{
			melee,
			sniper,
			mg,
			smg,
			spread,
			pistol,
			grenade,
			rocketlauncher,
			throwingknife,
			shield,
			rifle,
		};

		weapon_profile get_profile(const WeaponDef* asset)
		{
			if (asset->inventoryType == WEAPINVENTORY_MELEE)
			{
				return weapon_profile::melee;
			}

			switch (asset->weapClass)
			{
			case WEAPCLASS_SNIPER:
				return weapon_profile::sniper;
			case WEAPCLASS_MG:
				return weapon_profile::mg;
			case WEAPCLASS_SMG:
				return weapon_profile::smg;
			case WEAPCLASS_SPREAD:
				return weapon_profile::spread;
			case WEAPCLASS_PISTOL:
				return weapon_profile::pistol;
			case WEAPCLASS_GRENADE:
				return weapon_profile::grenade;
			case WEAPCLASS_ROCKETLAUNCHER:
				return weapon_profile::rocketlauncher;
			case WEAPCLASS_THROWINGKNIFE:
				return weapon_profile::throwingknife;
			case WEAPCLASS_SHIELD:
				return weapon_profile::shield;
			default:
				return weapon_profile::rifle;
			}
		}

		zonetool::s2::weapInventoryType_t convert_inventory_type(const weapInventoryType_t type)
		{
			switch (type)
			{
			case WEAPINVENTORY_OFFHAND:
				return zonetool::s2::WEAPINVENTORY_OFFHAND;
			case WEAPINVENTORY_ITEM:
				return zonetool::s2::WEAPINVENTORY_ITEM;
			case WEAPINVENTORY_EXCLUSIVE:
				return zonetool::s2::WEAPINVENTORY_EXCLUSIVE;
			case WEAPINVENTORY_SCAVENGER:
				return zonetool::s2::WEAPINVENTORY_SCAVENGER;
			default:
				return zonetool::s2::WEAPINVENTORY_PRIMARY; // s2 has no alt mode or melee inventory type, its melee weapons are primaries
			}
		}

		zonetool::s2::scr_string_t script_string(const char* value)
		{
			return static_cast<zonetool::s2::scr_string_t>(SL_GetString(value));
		}

		bool is_thrown(const weapon_profile profile)
		{
			return profile == weapon_profile::grenade || profile == weapon_profile::throwingknife;
		}

		void set_shared_defaults(zonetool::s2::WeaponCompleteDef* weapon, utils::memory::allocator& allocator)
		{
			weapon->reticleCenterEnemySize = 32;
			weapon->minimapRadiusScale = 1.0f;
			weapon->viewModelFov = 55.0f;
			weapon->lightweightSprintSpeedScale = 1.07f;
			weapon->showNoAmmoWarning = true;
			weapon->burnDuration = 0.4f;
			weapon->burnType = 1;
			weapon->aiBurnDuration = 0.4f;
			weapon->aiBurnType = 1;
			weapon->meleeFrontAngle = 45.0f;
			weapon->meleeBackAngle = 45.0f;
			weapon->meleeFrontDamageScale = 1.0f;
			weapon->meleeBackDamageScale = 1.1f;
			weapon->meleeSideDamageScale = 1.0f;
			weapon->combatRolePowerRechargeRate = 0.004f;
			weapon->combatRolePowerDrainRate = 0.1f;
			weapon->combatRoleCancelDrainsPower = true;
			weapon->combatRoleCancelPowerCost = 1.0f;
			weapon->combatRoleSwitchDrainsPower = true;
			weapon->combatRoleSwitchPowerCost = 1.0f;
			weapon->jogSprintLoopAnimWeight = 0.7f;
			weapon->jogSprintLoopAnimRate = 1.0f;
			weapon->jogSprintLoopAnimBlendTime = 2.0f;
			weapon->unknownRumble1040 = "defaultweapon_melee";
			weapon->unknown3832 = 480.0f;
			weapon->unknown4088 = 0.05f;
			weapon->handModel = make_stub<zonetool::s2::XModel>("player_usa_infantry_view_arms", allocator); // the weapon anims are retargeted to the s2 view arms
			weapon->fireRumble = "";
			weapon->meleeImpactRumble = "defaultweapon_melee";
			weapon->turretBarrelSpinRumble = "";
			weapon->playerAnimType = zonetool::s2::PLAYERANIMTYPE_AUTORIFLE;
			weapon->stowTag = script_string("tag_stow_back_mid_attach");
			weapon->stateTimers.rechamberTimeAlt = 100;
			weapon->stateTimers.reloadJamTime = 4000;
			weapon->stateTimers.activeReloadSuccessTime = 1000;
			weapon->stateTimers.activeReloadSuccessWindowStart = 250;
			weapon->stateTimers.activeReloadSuccessWindowEnd = 750;
			weapon->stateTimers.activeReloadJamWindowEnd = 1300;
			weapon->stateTimers.crawlInTime = 300;
			weapon->stateTimers.crawlLoopTime = 600;
			// activeReloadEffect and tagWeaponOffset fit only the s2 models and guid only the s2 stats table, they stay empty
		}

		void set_wielded_weapon_defaults(zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->meleeImpactType = zonetool::s2::IMPACT_TYPE_MELEE;
			weapon->quickMeleeDamage = 95;
			weapon->quickMeleeKeepsWeapon = true;
			weapon->meleeSwipeContinuous = true;
			weapon->aimAssistHoldBreathDelay = 0.5f;
			weapon->maxBreathHoldTime = 4.5f;
			weapon->firingMoveSpeedScale = 1.0f;
			weapon->firingAdsMoveSpeedScale = 1.0f;
			weapon->stalkerAdsMoveSpeedScale = 1.7f;
			weapon->firingStalkerAdsMoveSpeedScale = 1.5f;
			weapon->sprintSpeedScale = 1.0f;
			weapon->firingSprintSpeedScale = 1.0f;
			weapon->fastReloadTimeScale = 0.65f;
			weapon->hipSpreadStandDecay = 1.0f;
			weapon->hipSpreadSprintMin = 4.0f;
			weapon->hipSpreadSprintDecay = 1.5f;
			weapon->hipSpreadSlideMin = 3.0f;
			weapon->hipSpreadSlideDecay = 1.375f;
			weapon->bulletExplosionRadius = 256;
			weapon->bulletExplosionInnerDamage = 150;
			weapon->bulletExplosionOuterDamage = 50;
			weapon->adsViewKickAltPitchMin = 20.0f;
			weapon->adsViewKickAltPitchMax = 95.0f;
			weapon->adsViewKickAltYawMin = -30.0f;
			weapon->adsViewKickAltYawMax = 30.0f;
			weapon->adsViewKickAltCenterSpeed = 2000.0f;
			weapon->adsViewScatterAltMin = 10.0f;
			weapon->adsViewScatterAltMax = 10.0f;
			weapon->adsKickDuckedScale = 1.0f;
			weapon->adsKickProneScale = 1.0f;
			weapon->adsKickCenterSpeedDuckedScale = 1.0f;
			weapon->adsKickCenterSpeedProneScale = 1.0f;
			weapon->adsTurnRateScale = 1.0f;
			weapon->mountTurnRateScale = 1.0f;
			weapon->mountAdsTurnRateScale = 1.0f;
			weapon->mountViewOffsetBack = 20.0f;
			weapon->mountViewOffsetUp = 9.0f;
			weapon->hipViewKickAltPitchMin = 20.0f;
			weapon->hipViewKickAltPitchMax = 40.0f;
			weapon->hipViewKickAltYawMin = -15.0f;
			weapon->hipViewKickAltYawMax = 15.0f;
			weapon->hipViewKickAltCenterSpeed = 2000.0f;
			weapon->hipViewScatterAltMin = 10.0f;
			weapon->hipViewScatterAltMax = 10.0f;
			weapon->hipKickDuckedScale = 1.0f;
			weapon->hipKickProneScale = 1.0f;
			weapon->hipKickCenterSpeedDuckedScale = 1.0f;
			weapon->hipKickCenterSpeedProneScale = 1.0f;
			weapon->adsViewKickScale = 1.0f;
			weapon->adsFiringViewKickScale = 1.0f;
			weapon->kickAlignedInputScalar = 1.0f;
			weapon->kickOpposedInputScalar = 1.0f;
			weapon->autosimSpeedScaleThirdPerson = 1.0f;
			weapon->stateTimers.quickMeleeDelay = 250;
			weapon->stateTimers.quickMeleeTime = 700;
			weapon->akimboStateTimers.rechamberTimeAlt = 100;
			weapon->akimboStateTimers.reloadJamTime = 4000;
			weapon->akimboStateTimers.activeReloadSuccessTime = 1000;
			weapon->akimboStateTimers.activeReloadSuccessWindowStart = 250;
			weapon->akimboStateTimers.activeReloadSuccessWindowEnd = 750;
			weapon->akimboStateTimers.activeReloadJamWindowEnd = 1300;
			weapon->akimboStateTimers.sprintFireOutTime = 500;
			weapon->akimboStateTimers.crawlInTime = 300;
			weapon->akimboStateTimers.crawlLoopTime = 600;
		}

		void set_class_defaults(const weapon_profile profile, zonetool::s2::WeaponCompleteDef* weapon, utils::memory::allocator& allocator)
		{
			switch (profile)
			{
			case weapon_profile::melee:
				weapon->quickMeleeDamage = 135;
				weapon->viewModelFov = 65.0f;
				weapon->fastReloadTimeScale = 0.55f;
				weapon->meleeBackDamageScale = 1.0f;
				weapon->jogSprintLoopAnimWeight = 0.3f;
				weapon->jogSprintLoopAnimBlendTime = 0.8f;
				weapon->unknownRumble1040 = "";
				weapon->playerAnimType = zonetool::s2::PLAYERANIMTYPE_SHOVEL;
				weapon->stowTag = script_string("tag_stow_back_melee_attach");
				weapon->stateTimers.quickMeleeDelay = 130;
				weapon->stateTimers.sprintFireOutTime = 300;
				break;
			case weapon_profile::sniper:
				weapon->aimAssistRequiresHoldBreath = true;
				weapon->maxBreathHoldTime = 3.0f;
				weapon->firingMoveSpeedScale = 0.8f;
				weapon->stalkerAdsMoveSpeedScale = 1.6f;
				weapon->firingStalkerAdsMoveSpeedScale = 1.3f;
				weapon->hipSpreadSprintMin = 17.0f;
				weapon->idleFireRecoveryTime = 1500;
				weapon->adsTransSpread = 6.0f;
				weapon->adsKickCenterSpeedDuckedScale = 1.15f;
				weapon->adsKickCenterSpeedProneScale = 1.25f;
				weapon->allowProneAdsMovement = true;
				weapon->adsTurnRateScale = 1.4f;
				weapon->jogSprintLoopAnimWeight = 0.65f;
				weapon->jogSprintLoopAnimRate = 0.95f;
				weapon->fireRumble = "assault_fire";
				weapon->playerAnimType = zonetool::s2::PLAYERANIMTYPE_SNIPER;
				weapon->stateTimers.reloadRechamberTime = 950;
				weapon->stateTimers.sprintResumeDelay = 300;
				break;
			case weapon_profile::mg:
				weapon->firingMoveSpeedScale = 0.6f;
				weapon->firingAdsMoveSpeedScale = 0.4f;
				weapon->stalkerAdsMoveSpeedScale = 1.725f;
				weapon->firingStalkerAdsMoveSpeedScale = 2.35f;
				weapon->firingSprintSpeedScale = 0.948f;
				weapon->fastReloadTimeScale = 0.7f;
				weapon->hipSpreadSprintMin = 10.0f;
				weapon->idleFireRecoveryTime = 750;
				weapon->adsViewKickAltPitchMin = -45.0f;
				weapon->adsViewKickAltPitchMax = 45.0f;
				weapon->adsViewKickAltYawMin = -45.0f;
				weapon->adsViewKickAltYawMax = 45.0f;
				weapon->adsViewKickAltMagMin = 45.0f;
				weapon->adsViewKickAltCenterSpeed = 1500.0f;
				weapon->adsTransSpread = 2.0f;
				weapon->adsTurnRateScale = 0.95f;
				weapon->hipViewKickAltPitchMin = -45.0f;
				weapon->hipViewKickAltPitchMax = 45.0f;
				weapon->hipViewKickAltYawMin = -45.0f;
				weapon->hipViewKickAltYawMax = 45.0f;
				weapon->hipViewKickAltMagMin = 45.0f;
				weapon->hipViewKickAltCenterSpeed = 1500.0f;
				weapon->adsViewKickScale = 0.35f;
				weapon->adsFiringViewKickScale = 0.35f;
				weapon->kickOpposedInputScalar = 0.8f;
				weapon->autosimSpeedScaleThirdPerson = 1.5f;
				weapon->fireRumble = "heavygun_fire";
				weapon->stowTag = script_string("tag_stowed_back");
				break;
			case weapon_profile::smg:
				weapon->firingAdsMoveSpeedScale = 1.8f;
				weapon->stalkerAdsMoveSpeedScale = 1.25f;
				weapon->firingStalkerAdsMoveSpeedScale = 1.25f;
				weapon->fastReloadTimeScale = 0.6f;
				weapon->hipSpreadSprintMin = 8.0f;
				weapon->idleFireRecoveryTime = 250;
				weapon->adsViewKickAltPitchMin = -56.0f;
				weapon->adsViewKickAltPitchMax = 56.0f;
				weapon->adsViewKickAltYawMin = -80.0f;
				weapon->adsViewKickAltYawMax = 80.0f;
				weapon->adsViewKickAltMagMin = 56.0f;
				weapon->adsViewKickAltCenterSpeed = 1500.0f;
				weapon->adsTransSpread = 1.0f;
				weapon->hipViewKickAltPitchMin = -56.0f;
				weapon->hipViewKickAltPitchMax = 56.0f;
				weapon->hipViewKickAltYawMin = -80.0f;
				weapon->hipViewKickAltYawMax = 80.0f;
				weapon->hipViewKickAltMagMin = 56.0f;
				weapon->hipViewKickAltCenterSpeed = 1500.0f;
				weapon->adsViewKickScale = 0.55f;
				weapon->adsFiringViewKickScale = 0.55f;
				weapon->kickAlignedInputScalar = 1.1f;
				weapon->fireRumble = "smg_fire";
				weapon->playerAnimType = zonetool::s2::PLAYERANIMTYPE_SMG;
				break;
			case weapon_profile::spread:
				weapon->firingMoveSpeedScale = 0.95f;
				weapon->firingAdsMoveSpeedScale = 0.95f;
				weapon->stalkerAdsMoveSpeedScale = 1.25f;
				weapon->firingStalkerAdsMoveSpeedScale = 2.61f;
				weapon->firingSprintSpeedScale = 0.9785f;
				weapon->fastReloadTimeScale = 0.7f;
				weapon->hipSpreadSprintMin = 13.0f;
				weapon->adsTransSpread = 3.0f;
				weapon->adsTurnRateScale = 1.25f;
				weapon->adsViewKickScale = 0.55f;
				weapon->adsFiringViewKickScale = 0.55f;
				weapon->firstPelletBonusDamage = 94;
				weapon->firstPelletBonusMid1Damage = 92;
				weapon->firstPelletBonusMid2Damage = 46;
				weapon->firstPelletBonusMinDamage = 40;
				weapon->fireRumble = "shotgun_fire";
				weapon->playerAnimType = zonetool::s2::PLAYERANIMTYPE_OTHER;
				weapon->stateTimers.rechamberTimeAlt = 400;
				weapon->stateTimers.reloadRechamberTime = 600;
				weapon->stateTimers.sprintResumeDelay = 250;
				break;
			case weapon_profile::pistol:
				weapon->firingAdsMoveSpeedScale = 1.9f;
				weapon->stalkerAdsMoveSpeedScale = 1.315f;
				weapon->firingStalkerAdsMoveSpeedScale = 1.3f;
				weapon->fastReloadTimeScale = 0.6f;
				weapon->hipSpreadSprintMin = 7.5f;
				weapon->adsTransSpread = 1.0f;
				weapon->jogSprintLoopAnimRate = 0.9f;
				weapon->fireRumble = "pistol_fire";
				weapon->playerAnimType = zonetool::s2::PLAYERANIMTYPE_PISTOL;
				weapon->stateTimers.sprintResumeDelay = 500;
				weapon->akimboStateTimers.sprintFireOutTime = 0;
				break;
			case weapon_profile::grenade:
				weapon->grenadeIndicatorIcon = make_stub<zonetool::s2::Material>("grenade_indicator_frag_allies_hud", allocator);
				weapon->viewModelFov = 65.0f;
				weapon->meleeBackDamageScale = 1.0f;
				weapon->jogSprintLoopAnimWeight = 0.3f;
				weapon->jogSprintLoopAnimBlendTime = 0.8f;
				weapon->initialRoll = -45.0f;
				weapon->rotationPitchMin = -800.0f;
				weapon->rotationRollMin = -540.0f;
				weapon->rotationPitchMax = -320.0f;
				weapon->rotationRollMax = -180.0f;
				weapon->unknownRumble1040 = "";
				weapon->unknown3904 = 50;
				weapon->handModel = make_stub<zonetool::s2::XModel>("player_generic_view_arms", allocator);
				weapon->meleeImpactRumble = "";
				weapon->playerAnimType = zonetool::s2::PLAYERANIMTYPE_GRENADE;
				weapon->stowTag = script_string("tag_stowed_back");
				weapon->stateTimers.sprintFireOutTime = 500;
				break;
			case weapon_profile::rocketlauncher:
				weapon->quickMeleeDamage = 135;
				weapon->aimAssistHoldBreathDelay = 0.0f;
				weapon->maxBreathHoldTime = 0.0f;
				weapon->viewModelFov = 65.0f;
				weapon->fastReloadTimeScale = 0.8f;
				weapon->bulletExplosionRadius = 0;
				weapon->bulletExplosionInnerDamage = 0;
				weapon->bulletExplosionOuterDamage = 0;
				weapon->projectileSpinRate = 300.0f;
				weapon->killcamUpOffset = 10.0f;
				weapon->killcamBackOffset = 70.0f;
				weapon->killcamStopDistance = 350.0f;
				weapon->guidedForwardAccel = 500.0f;
				weapon->guidedPitchMin = 60.0f;
				weapon->guidedPitchMax = 90.0f;
				weapon->guidedYawLimit = 180.0f;
				weapon->guidedRollLimit = 180.0f;
				weapon->guidedPitchRate = 30.0f;
				weapon->guidedYawRate = 30.0f;
				weapon->adsViewKickAltPitchMin = 0.0f;
				weapon->adsViewKickAltPitchMax = 0.0f;
				weapon->adsViewKickAltYawMin = 0.0f;
				weapon->adsViewKickAltYawMax = 0.0f;
				weapon->adsViewKickAltCenterSpeed = 0.0f;
				weapon->adsViewScatterAltMin = 0.0f;
				weapon->adsViewScatterAltMax = 0.0f;
				weapon->hipViewKickAltPitchMin = 0.0f;
				weapon->hipViewKickAltPitchMax = 0.0f;
				weapon->hipViewKickAltYawMin = 0.0f;
				weapon->hipViewKickAltYawMax = 0.0f;
				weapon->hipViewKickAltCenterSpeed = 0.0f;
				weapon->hipViewScatterAltMin = 0.0f;
				weapon->hipViewScatterAltMax = 0.0f;
				weapon->kickAlignedInputScalar = 0.0f;
				weapon->kickOpposedInputScalar = 0.0f;
				weapon->autosimSpeedScaleThirdPerson = 0.0f;
				weapon->jogSprintLoopAnimWeight = 0.4f;
				weapon->unknownRumble1040 = "";
				weapon->fireRumble = "heavygun_fire";
				weapon->playerAnimType = zonetool::s2::PLAYERANIMTYPE_ROCKETLAUNCHER;
				break;
			case weapon_profile::throwingknife:
				weapon->grenadeIndicatorIconRatio = 2;
				weapon->viewModelFov = 65.0f;
				weapon->meleeBackDamageScale = 1.0f;
				weapon->jogSprintLoopAnimWeight = 0.3f;
				weapon->jogSprintLoopAnimBlendTime = 0.8f;
				weapon->unknownRumble1040 = "";
				weapon->unknown3904 = 50;
				weapon->handModel = make_stub<zonetool::s2::XModel>("player_generic_view_arms", allocator);
				weapon->meleeImpactRumble = "";
				weapon->playerAnimType = zonetool::s2::PLAYERANIMTYPE_THROWINGKNIFE;
				weapon->stowTag = script_string("tag_stowed_back");
				weapon->stateTimers.sprintFireOutTime = 500;
				break;
			case weapon_profile::shield:
				weapon->quickMeleeDamage = 135;
				weapon->meleeSwipeContinuous = false;
				weapon->fastReloadTimeScale = 0.55f;
				weapon->suppressAmmoPrimaryDisplay = true;
				weapon->jogSprintLoopAnimWeight = 0.3f;
				weapon->jogSprintLoopAnimBlendTime = 0.8f;
				weapon->unknownRumble1040 = "";
				weapon->fireRumble = "defaultweapon_fire";
				weapon->meleeImpactRumble = "riotshield_impact";
				weapon->playerAnimType = zonetool::s2::PLAYERANIMTYPE_RIOTSHIELD;
				weapon->stowTag = script_string("tag_stowed_back");
				weapon->stateTimers.sprintFireOutTime = 300;
				break;
			case weapon_profile::rifle:
				weapon->firingMoveSpeedScale = 0.9f;
				weapon->firingAdsMoveSpeedScale = 0.95f;
				weapon->stalkerAdsMoveSpeedScale = 1.925f;
				weapon->firingStalkerAdsMoveSpeedScale = 1.875f;
				weapon->viewModelFov = 65.0f;
				weapon->firingSprintSpeedScale = 0.9495f;
				weapon->hipSpreadSprintMin = 8.5f;
				weapon->idleFireRecoveryTime = 750;
				weapon->adsViewKickAltPitchMin = -50.0f;
				weapon->adsViewKickAltPitchMax = 50.0f;
				weapon->adsViewKickAltYawMin = -60.0f;
				weapon->adsViewKickAltYawMax = 60.0f;
				weapon->adsViewKickAltMagMin = 51.0f;
				weapon->adsViewKickAltCenterSpeed = 1500.0f;
				weapon->adsTransSpread = 1.0f;
				weapon->hipViewKickAltPitchMin = -50.0f;
				weapon->hipViewKickAltPitchMax = 50.0f;
				weapon->hipViewKickAltYawMin = -60.0f;
				weapon->hipViewKickAltYawMax = 60.0f;
				weapon->hipViewKickAltMagMin = 51.0f;
				weapon->hipViewKickAltCenterSpeed = 1500.0f;
				weapon->adsViewKickScale = 0.55f;
				weapon->adsFiringViewKickScale = 0.55f;
				weapon->kickOpposedInputScalar = 0.85f;
				weapon->jogSprintLoopAnimRate = 0.95f;
				weapon->fireRumble = "assault_fire";
				weapon->stateTimers.sprintResumeDelay = 500;
				break;
			}
		}

		void set_s2_only_defaults(const weapon_profile profile, zonetool::s2::WeaponCompleteDef* weapon, utils::memory::allocator& allocator)
		{
			set_shared_defaults(weapon, allocator);
			if (!is_thrown(profile))
			{
				set_wielded_weapon_defaults(weapon);
			}

			set_class_defaults(profile, weapon, allocator);
		}

		zonetool::s2::scr_string_t convert_script_string(const scr_string_t value)
		{
			return static_cast<zonetool::s2::scr_string_t>(value);
		}

		zonetool::s2::scr_string_t* convert_script_strings(const scr_string_t* values, const std::size_t count,
			utils::memory::allocator& allocator)
		{
			if (!values)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<zonetool::s2::scr_string_t>(count);
			for (auto i = 0u; i < count; i++)
			{
				result[i] = convert_script_string(values[i]);
			}

			return result;
		}

		zonetool::s2::XModel** convert_models(XModel* const* models, utils::memory::allocator& allocator)
		{
			if (!models)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<zonetool::s2::XModel*>(model_count);
			for (auto i = 0u; i < model_count; i++)
			{
				result[i] = xmodel::reference(models[i], allocator);
			}

			return result;
		}

		zonetool::s2::Material* convert_material(const Material* material, utils::memory::allocator& allocator)
		{
			if (!material || !material->name || !material::dump_used(material->name))
			{
				return nullptr;
			}

			return make_stub<zonetool::s2::Material>(material->name, allocator);
		}

		zonetool::s2::FxEffectDef* convert_effect(const FxEffectDef* effect, utils::memory::allocator& allocator)
		{
			if (!effect || !effect->name || !fxeffectdef::dump_used(effect->name))
			{
				return nullptr;
			}

			return make_stub<zonetool::s2::FxEffectDef>(effect->name, allocator);
		}

		zonetool::s2::TracerDef* convert_tracer(const TracerDef* tracer, utils::memory::allocator& allocator)
		{
			const auto* name = tracer && tracer->name ? tracer::dump_used(tracer->name) : nullptr;
			return name ? make_stub<zonetool::s2::TracerDef>(name, allocator) : nullptr;
		}

		std::vector<zonetool::s2::weapAnimFiles_t> convert_anim_slots(const std::uint32_t h1_anim)
		{
			std::vector<zonetool::s2::weapAnimFiles_t> slots;
			const auto [first, last] = anim_map.equal_range(static_cast<weapAnimFiles_t>(h1_anim));
			for (auto entry = first; entry != last; ++entry)
			{
				slots.emplace_back(entry->second);
			}

			return slots;
		}

		zonetool::s2::XAnimParts** convert_anims(XAnimParts* const* anims, utils::memory::allocator& allocator)
		{
			if (!anims)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<zonetool::s2::XAnimParts*>(zonetool::s2::NUM_WEAP_ANIMS);
			for (auto i = 0u; i < NUM_WEAP_ANIMS; i++)
			{
				if (!anims[i])
				{
					continue;
				}

				const auto slots = convert_anim_slots(i);
				if (slots.empty())
				{
					ZONETOOL_WARNING("weapon anim \"%s\" (%s) has no s2 slot", anim_names[i], anims[i]->name);
				}

				for (const auto slot : slots)
				{
					result[slot] = xanim::reference(anims[i], allocator);
				}
			}

			return result;
		}

		template <typename T, typename Convert>
		auto* convert_surface_array(const T* h1_values, utils::memory::allocator& allocator, const Convert& convert_value)
		{
			using value_type = std::remove_cvref_t<std::invoke_result_t<Convert, const T&>>;
			if (!h1_values)
			{
				return static_cast<value_type*>(nullptr);
			}

			const auto result = allocator.allocate_array<value_type>(s2_surface_type_count);
			std::vector<bool> has_source(s2_surface_type_count);
			for (auto i = 0u; i < h1_surface_type_count; i++)
			{
				const auto s2_type = convert_surface_type(i);
				if (!has_source[s2_type])
				{
					result[s2_type] = convert_value(h1_values[i]);
					has_source[s2_type] = true;
				}
			}

			for (auto i = 0u; i < s2_surface_type_count; i++)
			{
				if (!has_source[i])
				{
					result[i] = result[0];
				}
			}

			return result;
		}

		void dump_notetrack_sounds(const scr_string_t* values, const std::size_t count)
		{
			for (auto i = 0u; values && i < count; i++)
			{
				const auto* name = SL_ConvertToString(values[i]);
				if (name && *name && !sound::dump_used(name))
				{
					ZONETOOL_WARNING("notetrack sound \"%s\" not found in the loaded h1 zones", name);
				}
			}
		}

		void convert_attachments(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon, utils::memory::allocator& allocator)
		{
			weapon->numAttachments = 0;
			weapon->attachments = nullptr;
			std::memset(weapon->defaultAttachments, 0, sizeof(weapon->defaultAttachments));
			if (!asset->attachments || !asset->numAttachments)
			{
				return;
			}

			weapon->attachments = allocator.allocate_array<zonetool::s2::WeaponAttachment*>(asset->numAttachments);
			for (auto i = 0u; i < asset->numAttachments; i++)
			{
				weapon->attachments[i] = attachment::reference(asset->attachments[i], allocator);
			}

			weapon->numAttachments = asset->numAttachments;
		}

		std::optional<std::string> attachment_anim_name(const WAField& field, const XAnimParts* base_anim)
		{
			const std::string_view value = field.parm.string ? field.parm.string : "";
			if (field.code == FIELD_OP_STRING_SET)
			{
				return std::string(value);
			}

			if (!base_anim || !base_anim->name)
			{
				return std::nullopt;
			}

			std::string name = base_anim->name;
			if (field.code == FIELD_OP_STRING_APPEND)
			{
				return name + std::string(value);
			}

			const auto separator = value.find(' ');
			if (field.code != FIELD_OP_STRING_REPLACE || separator == std::string_view::npos)
			{
				return std::nullopt;
			}

			const auto from = value.substr(0, separator);
			const auto position = name.find(from);
			if (position == std::string::npos)
			{
				return std::nullopt;
			}

			return name.replace(position, from.size(), value.substr(separator + 1));
		}

		void add_attachment_anim_overrides(const WeaponDef* asset, std::vector<zonetool::s2::AnimOverrideEntry>& overrides,
			utils::memory::allocator& allocator)
		{
			for (auto i = 0u; asset->attachments && i < asset->numAttachments; i++)
			{
				const auto* attachment = asset->attachments[i] && asset->attachments[i]->name
					? db_find_x_asset_header_safe(ASSET_TYPE_ATTACHMENT, asset->attachments[i]->name).attachment
					: nullptr;
				for (auto f = 0u; attachment && attachment->fields && f < attachment->numFields; f++)
				{
					const auto& field = attachment->fields[f];
					if (field.type != WAFIELD_TYPE_ANIM || field.index >= NUM_WEAP_ANIMS || weapon_field_name(attachment->fieldOffsets[f]) != "szXAnims")
					{
						continue;
					}

					const auto* base_anim = asset->szXAnims ? asset->szXAnims[field.index] : nullptr;
					const auto name = attachment_anim_name(field, base_anim);
					if (name && name->empty())
					{
						continue;
					}

					const auto slots = convert_anim_slots(field.index);
					const auto* anim = name ? db_find_x_asset_header_safe(ASSET_TYPE_XANIMPARTS, name->data()).parts : nullptr;
					if (!anim || slots.empty())
					{
						ZONETOOL_WARNING("weapon \"%s\": attachment \"%s\" anim %s (\"%s\") is not converted",
							asset->name, attachment->name, anim_names[field.index], name ? name->data() : field.parm.string);
						continue;
					}

					for (const auto slot : slots)
					{
						auto& entry = overrides.emplace_back();
						entry.attachment1 = static_cast<unsigned char>(i + 1);
						entry.animTreeType = static_cast<unsigned char>(slot);
						entry.overrideAnim = xanim::reference(anim, allocator);
					}
				}
			}
		}

		void convert_overrides(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon, utils::memory::allocator& allocator)
		{
			std::vector<zonetool::s2::AnimOverrideEntry> anim_overrides;
			for (auto i = 0u; asset->animOverrides && i < asset->numAnimOverrides; i++)
			{
				const auto& entry = asset->animOverrides[i];
				for (const auto slot : convert_anim_slots(entry.animTreeType))
				{
					auto& override_entry = anim_overrides.emplace_back();
					override_entry.animHand = entry.animHand;
					override_entry.attachment1 = entry.attachment1;
					override_entry.attachment2 = entry.attachment2;
					override_entry.animTreeType = static_cast<unsigned char>(slot);
					override_entry.overrideAnim = xanim::reference(entry.overrideAnim, allocator);
					override_entry.altmodeAnim = xanim::reference(entry.altmodeAnim, allocator);
					override_entry.animTime = entry.animTime;
					override_entry.altTime = entry.altTime;
				}
			}

			add_attachment_anim_overrides(asset, anim_overrides, allocator);

			weapon->numAnimOverrides = static_cast<unsigned char>(anim_overrides.size());
			weapon->animOverrides = copy_array(anim_overrides.data(), anim_overrides.size(), allocator);

			weapon->numReloadStateTimerOverrides = asset->numReloadStateTimerOverrides;
			weapon->reloadOverrides = retype<zonetool::s2::ReloadStateTimerEntry>(asset->reloadOverrides);

			weapon->numNotetrackOverrides = asset->numNotetrackOverrides;
			weapon->notetrackOverrides = nullptr;
			if (asset->notetrackOverrides && asset->numNotetrackOverrides)
			{
				weapon->notetrackOverrides = allocator.allocate_array<zonetool::s2::NoteTrackToSoundEntry>(asset->numNotetrackOverrides);
				for (auto i = 0u; i < asset->numNotetrackOverrides; i++)
				{
					const auto& entry = asset->notetrackOverrides[i];
					auto& override_entry = weapon->notetrackOverrides[i];
					override_entry.attachment = entry.attachment;
					override_entry.notetrackSoundMapKeys = convert_script_strings(entry.notetrackSoundMapKeys, notetrack_sound_count, allocator);
					override_entry.notetrackSoundMapValues = convert_script_strings(entry.notetrackSoundMapValues, notetrack_sound_count, allocator);
					dump_notetrack_sounds(entry.notetrackSoundMapValues, notetrack_sound_count);
				}
			}

			if (asset->numSoundOverrides || asset->numFXOverrides)
			{
				ZONETOOL_WARNING("weapon \"%s\": %d sound and %d effect overrides are not converted",
					asset->name, asset->numSoundOverrides, asset->numFXOverrides);
			}

			weapon->numSoundOverrides = 0;
			weapon->soundOverrides = nullptr;
			weapon->numFXOverrides = 0;
			weapon->fxOverrides = nullptr;
		}

		void convert_notetrack_maps(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon, utils::memory::allocator& allocator)
		{
			weapon->notetrackSoundMapKeys = convert_script_strings(asset->notetrackSoundMapKeys, notetrack_sound_count, allocator);
			weapon->notetrackSoundMapValues = convert_script_strings(asset->notetrackSoundMapValues, notetrack_sound_count, allocator);
			dump_notetrack_sounds(asset->notetrackSoundMapValues, notetrack_sound_count);

			// TODO: h1 rumbles and effects are not converted
			weapon->notetrackRumbleMapKeys = allocator.allocate_array<zonetool::s2::scr_string_t>(notetrack_map_count);
			weapon->notetrackRumbleMapValues = allocator.allocate_array<zonetool::s2::scr_string_t>(notetrack_map_count);
			weapon->notetrackFXMapKeys = allocator.allocate_array<zonetool::s2::scr_string_t>(notetrack_map_count);
			weapon->notetrackFXMapValues = allocator.allocate_array<zonetool::s2::FxEffectDef*>(notetrack_map_count);
			weapon->notetrackFXMapTagValues = allocator.allocate_array<zonetool::s2::scr_string_t>(notetrack_map_count);
		}

		void convert_state_timers(const StateTimers& h1, zonetool::s2::StateTimers& s2)
		{
			s2.fireDelay = h1.fireDelay;
			s2.meleeDelay = h1.meleeDelay;
			s2.meleeChargeDelay = h1.meleeChargeDelay;
			s2.detonateDelay = h1.detonateDelay;
			s2.fireTime = h1.fireTime;
			s2.rechamberTime = h1.rechamberTime;
			s2.rechamberTimeOneHanded = h1.rechamberTimeOneHanded;
			s2.rechamberBoltTime = h1.rechamberBoltTime;
			s2.holdFireTime = h1.holdFireTime;
			s2.grenadePrimeReadyToThrowTime = h1.grenadePrimeReadyToThrowTime;
			s2.detonateTime = h1.detonateTime;
			s2.meleeTime = h1.meleeTime;
			s2.meleeChargeTime = h1.meleeChargeTime;
			s2.reloadTime = h1.reloadTime;
			s2.reloadShowRocketTime = h1.reloadShowRocketTime;
			s2.reloadEmptyTime = h1.reloadEmptyTime;
			s2.reloadAddTime = h1.reloadAddTime;
			s2.reloadEmptyAddTime = h1.reloadEmptyAddTime;
			s2.reloadStartTime = h1.reloadStartTime;
			s2.reloadStartAddTime = h1.reloadStartAddTime;
			s2.reloadEndTime = h1.reloadEndTime;
			s2.reloadTimeDualWield = h1.reloadTimeDualWield;
			s2.reloadAddTimeDualWield = h1.reloadAddTimeDualWield;
			s2.reloadEmptyDualMag = h1.reloadEmptyDualMag;
			s2.reloadEmptyAddTimeDualMag = h1.reloadEmptyAddTimeDualMag;
			s2.speedReloadTime = h1.speedReloadTime;
			s2.speedReloadAddTime = h1.speedReloadAddTime;
			s2.dropTime = h1.dropTime;
			s2.raiseTime = h1.raiseTime;
			s2.altDropTime = h1.altDropTime;
			s2.altRaiseTime = h1.altRaiseTime;
			s2.quickDropTime = h1.quickDropTime;
			s2.quickRaiseTime = h1.quickRaiseTime;
			s2.firstRaiseTime = h1.firstRaiseTime;
			s2.breachRaiseTime = h1.breachRaiseTime;
			s2.emptyRaiseTime = h1.emptyRaiseTime;
			s2.emptyDropTime = h1.emptyDropTime;
			s2.sprintInTime = h1.sprintInTime;
			s2.sprintLoopTime = h1.sprintLoopTime;
			s2.sprintOutTime = h1.sprintOutTime;
			s2.stunnedTimeBegin = h1.stunnedTimeBegin;
			s2.stunnedTimeLoop = h1.stunnedTimeLoop;
			s2.stunnedTimeEnd = h1.stunnedTimeEnd;
			s2.nightVisionWearTime = h1.nightVisionWearTime;
			s2.nightVisionWearTimeFadeOutEnd = h1.nightVisionWearTimeFadeOutEnd;
			s2.nightVisionWearTimePowerUp = h1.nightVisionWearTimePowerUp;
			s2.nightVisionRemoveTime = h1.nightVisionRemoveTime;
			s2.nightVisionRemoveTimePowerDown = h1.nightVisionRemoveTimePowerDown;
			s2.nightVisionRemoveTimeFadeInStart = h1.nightVisionRemoveTimeFadeInStart;
			s2.aiFuseTime = h1.aiFuseTime;
			s2.fuseTime = h1.fuseTime;
			s2.missileTime = h1.missileTime;
			s2.primeTime = h1.primeTime;
			s2.bHoldFullPrime = h1.bHoldFullPrime;
			s2.blastFrontTime = h1.blastFrontTime;
			s2.blastRightTime = h1.blastRightTime;
			s2.blastBackTime = h1.blastBackTime;
			s2.blastLeftTime = h1.blastLeftTime;
			s2.slideInTime = h1.slideInTime;
			s2.slideLoopTime = h1.slideLoopTime;
			s2.slideOutTime = h1.slideOutTime;
			s2.offhandSwitchTime = h1.offhandSwitchTime;
		}

		void convert_sounds(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon, utils::memory::allocator& allocator)
		{
			const auto convert = [&](const snd_alias_list_t* alias)
			{
				return sound::reference(alias, allocator);
			};

			weapon->pickupSound = convert(asset->pickupSound);
			weapon->pickupSoundPlayer = convert(asset->pickupSoundPlayer);
			weapon->ammoPickupSound = convert(asset->ammoPickupSound);
			weapon->ammoPickupSoundPlayer = convert(asset->ammoPickupSoundPlayer);
			weapon->projectileSound = convert(asset->projectileSound);
			weapon->pullbackSound = convert(asset->pullbackSound);
			weapon->pullbackSoundPlayer = convert(asset->pullbackSoundPlayer);
			weapon->fireSound = convert(asset->fireSound);
			weapon->fireSoundPlayer = convert(asset->fireSoundPlayer);
			weapon->fireSoundPlayerAkimbo = convert(asset->fireSoundPlayerAkimbo);
			weapon->fireLoopSound = convert(asset->fireLoopSound);
			weapon->fireLoopSoundPlayer = convert(asset->fireLoopSoundPlayer);
			weapon->fireLoopEndPointSound = convert(asset->fireLoopEndPointSound);
			weapon->fireLoopEndPointSoundPlayer = convert(asset->fireLoopEndPointSoundPlayer);
			weapon->fireStopSound = convert(asset->fireStopSound);
			weapon->fireStopSoundPlayer = convert(asset->fireStopSoundPlayer);
			weapon->fireLastSound = convert(asset->fireLastSound);
			weapon->fireLastSoundPlayer = convert(asset->fireLastSoundPlayer);
			weapon->fireFirstSound = convert(asset->fireFirstSound);
			weapon->fireFirstSoundPlayer = convert(asset->fireFirstSoundPlayer);
			weapon->fireCustomSound = convert(asset->fireCustomSound);
			weapon->fireCustomSoundPlayer = convert(asset->fireCustomSoundPlayer);
			weapon->emptyFireSound = convert(asset->emptyFireSound);
			weapon->emptyFireSoundPlayer = convert(asset->emptyFireSoundPlayer);
			weapon->adsRequiredFireSoundPlayer = convert(asset->adsRequiredFireSoundPlayer);
			weapon->meleeSwipeSound = convert(asset->meleeSwipeSound);
			weapon->meleeSwipeSoundPlayer = convert(asset->meleeSwipeSoundPlayer);
			weapon->meleeHitSound = convert(asset->meleeHitSound);
			weapon->meleeMissSound = convert(asset->meleeMissSound);
			weapon->meleeHitSoundPlayer = convert(asset->meleeHitSoundPlayer);
			weapon->meleeMissSoundPlayer = convert(asset->meleeMissSoundPlayer);
			weapon->rechamberSound = convert(asset->rechamberSound);
			weapon->rechamberSoundPlayer = convert(asset->rechamberSoundPlayer);
			weapon->reloadSound = convert(asset->reloadSound);
			weapon->reloadSoundPlayer = convert(asset->reloadSoundPlayer);
			weapon->reloadEmptySound = convert(asset->reloadEmptySound);
			weapon->reloadEmptySoundPlayer = convert(asset->reloadEmptySoundPlayer);
			weapon->reloadStartSound = convert(asset->reloadStartSound);
			weapon->reloadStartSoundPlayer = convert(asset->reloadStartSoundPlayer);
			weapon->reloadEndSound = convert(asset->reloadEndSound);
			weapon->reloadEndSoundPlayer = convert(asset->reloadEndSoundPlayer);
			weapon->detonateSound = convert(asset->detonateSound);
			weapon->detonateSoundPlayer = convert(asset->detonateSoundPlayer);
			weapon->nightVisionWearSound = convert(asset->nightVisionWearSound);
			weapon->nightVisionWearSoundPlayer = convert(asset->nightVisionWearSoundPlayer);
			weapon->nightVisionRemoveSound = convert(asset->nightVisionRemoveSound);
			weapon->nightVisionRemoveSoundPlayer = convert(asset->nightVisionRemoveSoundPlayer);
			weapon->altSwitchSound = convert(asset->altSwitchSound);
			weapon->altSwitchSoundPlayer = convert(asset->altSwitchSoundPlayer);
			weapon->raiseSound = convert(asset->raiseSound);
			weapon->raiseSoundPlayer = convert(asset->raiseSoundPlayer);
			weapon->firstRaiseSound = convert(asset->firstRaiseSound);
			weapon->firstRaiseSoundPlayer = convert(asset->firstRaiseSoundPlayer);
			weapon->putawaySound = convert(asset->putawaySound);
			weapon->putawaySoundPlayer = convert(asset->putawaySoundPlayer);
			weapon->scanSound = convert(asset->scanSound);
			weapon->changeVariableZoomSound = convert(asset->changeVariableZoomSound);
			weapon->adsUpSound = convert(asset->adsUpSound);
			weapon->adsDownSound = convert(asset->adsDownSound);
			weapon->adsCrosshairEnemySound = convert(asset->adsCrosshairEnemySound);
			weapon->bounceSound = convert_surface_array(asset->bounceSound, allocator, convert);
			weapon->rollingSound = convert_surface_array(asset->rollingSound, allocator, convert);
			weapon->projExplosionSound = convert(asset->projExplosionSound);
			weapon->projDudSound = convert(asset->projDudSound);
			weapon->projIgnitionSound = convert(asset->projIgnitionSound);
			weapon->turretOverheatSound = convert(asset->turretOverheatSound);
			weapon->turretBarrelSpinMaxSnd = convert(asset->turretBarrelSpinMaxSnd);
			for (auto i = 0u; i < std::size(weapon->turretBarrelSpinUpSnd); i++)
			{
				weapon->turretBarrelSpinUpSnd[i] = convert(asset->turretBarrelSpinUpSnd[i]);
				weapon->turretBarrelSpinDownSnd[i] = convert(asset->turretBarrelSpinDownSnd[i]);
			}
			weapon->missileConeSoundAlias = convert(asset->missileConeSoundAlias);
			weapon->missileConeSoundAliasAtBase = convert(asset->missileConeSoundAliasAtBase);
			weapon->missileConeSoundRadiusAtTop = asset->missileConeSoundRadiusAtTop;
			weapon->missileConeSoundRadiusAtBase = asset->missileConeSoundRadiusAtBase;
			weapon->missileConeSoundHeight = asset->missileConeSoundHeight;
			weapon->missileConeSoundOriginOffset = asset->missileConeSoundOriginOffset;
			weapon->missileConeSoundVolumescaleAtCore = asset->missileConeSoundVolumescaleAtCore;
			weapon->missileConeSoundVolumescaleAtEdge = asset->missileConeSoundVolumescaleAtEdge;
			weapon->missileConeSoundVolumescaleCoreSize = asset->missileConeSoundVolumescaleCoreSize;
			weapon->missileConeSoundPitchAtTop = asset->missileConeSoundPitchAtTop;
			weapon->missileConeSoundPitchAtBottom = asset->missileConeSoundPitchAtBottom;
			weapon->missileConeSoundPitchTopSize = asset->missileConeSoundPitchTopSize;
			weapon->missileConeSoundPitchBottomSize = asset->missileConeSoundPitchBottomSize;
			weapon->missileConeSoundCrossfadeTopSize = asset->missileConeSoundCrossfadeTopSize;
			weapon->missileConeSoundCrossfadeBottomSize = asset->missileConeSoundCrossfadeBottomSize;
			weapon->missileConeSoundEnabled = asset->missileConeSoundEnabled;
			weapon->missileConeSoundPitchshiftEnabled = asset->missileConeSoundPitchshiftEnabled;
			weapon->missileConeSoundCrossfadeEnabled = asset->missileConeSoundCrossfadeEnabled;
		}

		void convert_weapon_models(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon, utils::memory::allocator& allocator)
		{
			weapon->gunModel = convert_models(asset->gunModel, allocator);
			weapon->worldModel = convert_models(asset->worldModel, allocator);
			weapon->gunModelConditions = allocator.allocate_array<zonetool::s2::XModel*>(model_count); // stock weapons always have the arrays
			weapon->worldModelConditions = allocator.allocate_array<zonetool::s2::XModel*>(model_count);
			weapon->worldClipModel = xmodel::reference(asset->worldClipModel, allocator);
			weapon->rocketModel = xmodel::reference(asset->rocketModel, allocator);
			weapon->projectileModel = xmodel::reference(asset->projectileModel, allocator);
			weapon->stowOffsetModel = xmodel::reference(asset->stowOffsetModel, allocator);
			weapon->lobWorldModelName = asset->lobWorldModelName;
			weapon->physPreset = physpreset::reference(asset->physPreset, asset->worldModel ? asset->worldModel[0] : nullptr, allocator);
			weapon->physCollmap = physcollmap::reference(asset->physCollmap, allocator);
		}

		void convert_hud(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon, utils::memory::allocator& allocator)
		{
			const auto convert = [&](const Material* material)
			{
				return convert_material(material, allocator);
			};

			weapon->reticleCenter = convert(asset->reticleCenter);
			weapon->reticleSide = convert(asset->reticleSide);
			weapon->activeReticleType = static_cast<zonetool::s2::activeReticleType_t>(asset->activeReticleType);
			weapon->hudIcon = convert(asset->hudIcon);
			weapon->hudIconRatio = asset->hudIconRatio;
			weapon->pickupIcon = convert(asset->pickupIcon);
			weapon->pickupIconRatio = asset->pickupIconRatio;
			weapon->ammoCounterIcon = convert(asset->ammoCounterIcon);
			weapon->ammoCounterIconRatio = asset->ammoCounterIconRatio;
			weapon->ammoCounterClip = static_cast<zonetool::s2::ammoCounterClipType_t>(asset->ammoCounterClip);
			weapon->killIcon = convert(asset->killIcon);
			weapon->killIconRatio = asset->killIconRatio;
			weapon->dpadIcon = convert(asset->dpadIcon);
			weapon->dpadIconRatio = asset->dpadIconRatio;
			weapon->hudProximityWarningIcon = convert(asset->hudProximityWarningIcon);
			weapon->overlay.shader = convert(asset->overlay.shader);
			weapon->overlay.shaderLowRes = convert(asset->overlay.shaderLowRes);
			weapon->overlay.shaderEMP = convert(asset->overlay.shaderEMP);
			weapon->overlay.shaderEMPLowRes = convert(asset->overlay.shaderEMPLowRes);
			weapon->overlay.reticle = asset->overlay.reticle;
			weapon->overlay.enabled = weapon->overlay.shader != nullptr; // s2 only draws an overlay shader with enabled set, h1 draws any it has
			weapon->overlayInterface = static_cast<zonetool::s2::weapOverlayInterface_t>(asset->overlayInterface);
		}

		void convert_effects(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon, utils::memory::allocator& allocator)
		{
			const auto convert = [&](const FxEffectDef* effect)
			{
				return convert_effect(effect, allocator);
			};

			weapon->viewFlashEffect = convert(asset->viewFlashEffect);
			weapon->viewBodyFlashEffect = convert(asset->viewBodyFlashEffect);
			weapon->worldFlashEffect = convert(asset->worldFlashEffect);
			weapon->viewFlashADSEffect = convert(asset->viewFlashADSEffect);
			weapon->viewBodyFlashADSEffect = convert(asset->viewBodyFlashADSEffect);
			weapon->signatureViewFlashEffect = convert(asset->signatureViewFlashEffect);
			weapon->signatureViewBodyFlashEffect = convert(asset->signatureViewBodyFlashEffect);
			weapon->signatureWorldFlashEffect = convert(asset->signatureWorldFlashEffect);
			weapon->signatureViewFlashADSEffect = convert(asset->signatureViewFlashADSEffect);
			weapon->signatureViewBodyFlashADSEffect = convert(asset->signatureViewBodyFlashADSEffect);
			weapon->meleeHitEffect = convert(asset->meleeHitEffect);
			weapon->meleeMissEffect = convert(asset->meleeMissEffect);
			weapon->viewShellEjectEffect = convert(asset->viewShellEjectEffect);
			weapon->worldShellEjectEffect = convert(asset->worldShellEjectEffect);
			weapon->viewLastShotEjectEffect = convert(asset->viewLastShotEjectEffect);
			weapon->worldLastShotEjectEffect = convert(asset->worldLastShotEjectEffect);
			weapon->viewMagEjectEffect = convert(asset->viewMagEjectEffect);
			weapon->turretOverheatEffect = convert(asset->turretOverheatEffect);
			weapon->projExplosionEffect = convert(asset->projExplosionEffect);
			weapon->projDudEffect = convert(asset->projDudEffect);
			weapon->projTrailEffect = convert(asset->projTrailEffect);
			weapon->projBeaconEffect = convert(asset->projBeaconEffect);
			weapon->projIgnitionEffect = convert(asset->projIgnitionEffect);
			weapon->tracerType = convert_tracer(asset->tracerType, allocator);
			weapon->signatureTracerType = convert_tracer(asset->signatureTracerType, allocator);
		}

		void convert_damage(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->damageFalloffLerpNear = false;
			weapon->damageFalloffLerpFar = false;
			weapon->damage = asset->damage;
			weapon->playerDamage = asset->playerDamage;
			weapon->maxDamageRange = asset->maxDamageRange;
			weapon->mid1Damage = asset->midDamage;
			weapon->mid1PlayerDamage = asset->midPlayerDamage;
			weapon->mid1DamageRange = asset->minDamageRange;
			weapon->mid2Damage = asset->minDamage;
			weapon->mid2PlayerDamage = asset->minPlayerDamage;
			weapon->minDamage = asset->minDamage;
			weapon->minPlayerDamage = asset->minPlayerDamage;
			weapon->minDamageRange = asset->minDamageRange;
			weapon->meleeDamage = asset->meleeDamage;
			weapon->meleeChargeDamage = asset->meleeDamage; // the s2 alternate melee damage, h1 has one damage for every swing
			weapon->damageType = asset->damageType;
			weapon->signatureDamage = asset->signatureDamage;
			weapon->signatureMid1Damage = asset->signatureMidDamage;
			weapon->signatureMinDamage = asset->signatureMinDamage;
			weapon->signatureMaxDamageRange = asset->signatureMaxDamageRange;
			weapon->signatureMinDamageRange = asset->signatureMinDamageRange;
			weapon->locationDamageMultipliers = asset->locationDamageMultipliers;
			weapon->explosionRadius = asset->explosionRadius;
			weapon->explosionRadiusMin = asset->explosionRadiusMin;
			weapon->explosionInnerDamage = asset->explosionInnerDamage;
			weapon->explosionOuterDamage = asset->explosionOuterDamage;
			weapon->damageConeAngle = asset->damageConeAngle;
			weapon->riotShieldHealth = asset->riotShieldHealth;
			weapon->riotShieldDamageMult = asset->riotShieldDamageMult;
			weapon->riotShieldEnableDamage = asset->riotShieldEnableDamage;
		}

		void convert_movement(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			std::memcpy(weapon->standMove, asset->standMove, sizeof(weapon->standMove));
			std::memcpy(weapon->standRot, asset->standRot, sizeof(weapon->standRot));
			std::memcpy(weapon->strafeMove, asset->strafeMove, sizeof(weapon->strafeMove));
			std::memcpy(weapon->strafeRot, asset->strafeRot, sizeof(weapon->strafeRot));
			std::memcpy(weapon->duckedOfs, asset->duckedOfs, sizeof(weapon->duckedOfs));
			std::memcpy(weapon->duckedMove, asset->duckedMove, sizeof(weapon->duckedMove));
			std::memcpy(weapon->duckedRot, asset->duckedRot, sizeof(weapon->duckedRot));
			std::memcpy(weapon->proneOfs, asset->proneOfs, sizeof(weapon->proneOfs));
			std::memcpy(weapon->proneMove, asset->proneMove, sizeof(weapon->proneMove));
			std::memcpy(weapon->proneRot, asset->proneRot, sizeof(weapon->proneRot));
			weapon->posMoveRate = asset->posMoveRate;
			weapon->posProneMoveRate = asset->posProneMoveRate;
			weapon->standMoveMinSpeed = asset->standMoveMinSpeed;
			weapon->duckedMoveMinSpeed = asset->duckedMoveMinSpeed;
			weapon->proneMoveMinSpeed = asset->proneMoveMinSpeed;
			weapon->posRotRate = asset->posRotRate;
			weapon->posProneRotRate = asset->posProneRotRate;
			weapon->moveSpeedScale = asset->moveSpeedScale;
			weapon->adsMoveSpeedScale = asset->adsMoveSpeedScale;
			weapon->sprintDurationScale = asset->sprintDurationScale;
		}

		void convert_ads(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->adsZoomFov = asset->adsZoomFov;
			weapon->adsZoomInFrac = asset->adsZoomInFrac;
			weapon->adsZoomOutFrac = asset->adsZoomOutFrac;
			weapon->adsSceneBlur = asset->adsSceneBlurStrength;
			weapon->adsBobFactor = asset->adsBobFactor;
			weapon->adsViewBobMult = asset->adsViewBobMult;
			weapon->adsIdleAmount = asset->adsIdleAmount;
			weapon->hipIdleAmount = asset->hipIdleAmount;
			weapon->adsIdleSpeed = asset->adsIdleSpeed;
			weapon->hipIdleSpeed = asset->hipIdleSpeed;
			weapon->adsIdleLerpStartTime = asset->adsIdleLerpStartTime;
			weapon->adsIdleLerpTime = asset->adsIdleLerpTime;
			weapon->idleCrouchFactor = asset->idleCrouchFactor;
			weapon->idleProneFactor = asset->idleProneFactor;
			weapon->gunMaxPitch = asset->gunMaxPitch;
			weapon->gunMaxYaw = asset->gunMaxYaw;
			weapon->adsTransInTime = asset->adsTransInTime;
			weapon->adsTransInFromSprintTime = asset->adsTransInFromSprintTime;
			weapon->adsTransOutTime = asset->adsTransOutTime;
			weapon->swayMaxAngle = asset->swayMaxAngle;
			weapon->swayMaxAngleSteadyAim = asset->swayMaxAngleSteadyAim;
			weapon->swayLerpSpeed = asset->swayLerpSpeed;
			weapon->swayPitchScale = asset->swayPitchScale;
			weapon->swayYawScale = asset->swayYawScale;
			weapon->swayHorizScale = asset->swayHorizScale;
			weapon->swayVertScale = asset->swayVertScale;
			weapon->swayShellShockScale = asset->swayShellShockScale;
			weapon->adsSwayMaxAngle = asset->adsSwayMaxAngle;
			weapon->adsSwayLerpSpeed = asset->adsSwayLerpSpeed;
			weapon->adsSwayPitchScale = asset->adsSwayPitchScale;
			weapon->adsSwayYawScale = asset->adsSwayYawScale;
			weapon->adsSwayHorizScale = asset->adsSwayHorizScale;
			weapon->adsSwayVertScale = asset->adsSwayVertScale;
			weapon->adsViewErrorMin = asset->adsViewErrorMin;
			weapon->adsViewErrorMax = asset->adsViewErrorMax;
			weapon->adsFireAnimFrac = asset->adsFireAnimFrac;
			weapon->scopeDriftDelay = asset->scopeDriftDelay;
			weapon->scopeDriftLerpInTime = asset->scopeDriftLerpInTime;
			weapon->scopeDriftSteadyTime = asset->scopeDriftSteadyTime;
			weapon->scopeDriftLerpOutTime = asset->scopeDriftLerpOutTime;
			weapon->scopeDriftSteadyFactor = asset->scopeDriftSteadyFactor;
			weapon->scopeDriftUnsteadyFactor = asset->scopeDriftUnsteadyFactor;
			weapon->useScopeDrift = asset->useScopeDrift;
			weapon->bobVerticalFactor = asset->bobVerticalFactor;
			weapon->bobHorizontalFactor = asset->bobHorizontalFactor;
			weapon->bobViewVerticalFactor = asset->bobViewVerticalFactor;
			weapon->bobViewHorizontalFactor = asset->bobViewHorizontalFactor;
			weapon->stationaryZoomFov = asset->stationaryZoomFov;
			weapon->stationaryZoomDelay = asset->stationaryZoomDelay;
			weapon->stationaryZoomLerpInTime = asset->stationaryZoomLerpInTime;
			weapon->stationaryZoomLerpOutTime = asset->stationaryZoomLerpOutTime;
			weapon->adsDofStart = asset->adsDofStart;
			weapon->adsDofEnd = asset->adsDofEnd;
			weapon->adsDofPhysicalFstop = asset->adsDofPhysicalFstop;
			weapon->adsDofPhysicalFocusDistance = asset->adsDofPhysicalFocusDistance;
			weapon->adsAimPitch = asset->adsAimPitch;
			weapon->adsCrosshairInFrac = asset->adsCrosshairInFrac;
			weapon->adsCrosshairOutFrac = asset->adsCrosshairOutFrac;
			weapon->adsSpread = asset->adsSpread;
			weapon->adsShouldShowCrosshair = asset->adsShouldShowCrosshair;
			weapon->adsCrosshairShouldScale = asset->adsCrosshairShouldScale;
			weapon->aimDownSight = asset->aimDownSight;
			weapon->noAdsWhenMagEmpty = asset->noAdsWhenMagEmpty;
			weapon->canHoldBreath = asset->canHoldBreath;
			weapon->canVariableZoom = asset->canVariableZoom;
			weapon->usesSniperScope = asset->usesSniperScope;
			weapon->adsHideWeapon = asset->adsHideWeapon;
			weapon->rechamberWhileAds = asset->rechamberWhileAds;
			weapon->overlay.width = asset->overlay.width;
			weapon->overlay.height = asset->overlay.height;
			weapon->overlay.widthSplitscreen = asset->overlay.widthSplitscreen;
			weapon->overlay.heightSplitscreen = asset->overlay.heightSplitscreen;
		}

		void convert_spread(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->hipSpreadDecayRate = asset->hipSpreadDecayRate;
			weapon->hipSpreadStandMin = asset->hipSpreadStandMin;
			weapon->hipSpreadStandMax = asset->hipSpreadStandMax;
			weapon->hipSpreadDuckedMin = asset->hipSpreadDuckedMin;
			weapon->hipSpreadDuckedMax = asset->hipSpreadDuckedMax;
			weapon->hipSpreadDuckedDecay = asset->hipSpreadDuckedDecay;
			weapon->hipSpreadProneMin = asset->hipSpreadProneMin;
			weapon->hipSpreadProneMax = asset->hipSpreadProneMax;
			weapon->hipSpreadProneDecay = asset->hipSpreadProneDecay;
			weapon->hipSpreadSprintMax = asset->hipSpreadSprintMax;
			weapon->hipSpreadSlideMax = asset->hipSpreadSlideMax;
			weapon->hipSpreadMoveAdd = asset->hipSpreadMoveAdd;
			weapon->hipSpreadTurnAdd = asset->hipSpreadTurnAdd;
			weapon->hipSpreadFireAdd = asset->hipSpreadFireAdd;
			weapon->hipReticleSidePos = asset->hipReticleSidePos;
			weapon->doNotAllowAttachmentsToOverrideSpread = asset->doNotAllowAttachmentsToOverrideSpread;
		}

		void convert_kick(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->adsGunKickPitchMin = asset->adsGunKickPitchMin;
			weapon->adsGunKickPitchMax = asset->adsGunKickPitchMax;
			weapon->adsGunKickYawMin = asset->adsGunKickYawMin;
			weapon->adsGunKickYawMax = asset->adsGunKickYawMax;
			weapon->adsGunKickMagMin = asset->adsGunKickMagMin;
			weapon->adsGunKickAccel = asset->adsGunKickAccel;
			weapon->adsGunKickSpeedMax = asset->adsGunKickSpeedMax;
			weapon->adsGunKickSpeedDecay = asset->adsGunKickSpeedDecay;
			weapon->adsGunKickStaticDecay = asset->adsGunKickStaticDecay;
			weapon->adsGunKickReducedKickBullets = asset->adsGunKickReducedKickBullets;
			weapon->adsGunKickReducedKickPercent = asset->adsGunKickReducedKickPercent;
			weapon->adsViewKickPitchMin = asset->adsViewKickPitchMin;
			weapon->adsViewKickPitchMax = asset->adsViewKickPitchMax;
			weapon->adsViewKickYawMin = asset->adsViewKickYawMin;
			weapon->adsViewKickYawMax = asset->adsViewKickYawMax;
			weapon->adsViewKickMagMin = asset->adsViewKickMagMin;
			weapon->adsViewKickCenterSpeed = asset->adsViewKickCenterSpeed;
			weapon->adsViewScatterMin = asset->adsViewScatterMin;
			weapon->adsViewScatterMax = asset->adsViewScatterMax;
			weapon->hipGunKickPitchMin = asset->hipGunKickPitchMin;
			weapon->hipGunKickPitchMax = asset->hipGunKickPitchMax;
			weapon->hipGunKickYawMin = asset->hipGunKickYawMin;
			weapon->hipGunKickYawMax = asset->hipGunKickYawMax;
			weapon->hipGunKickMagMin = asset->hipGunKickMagMin;
			weapon->hipGunKickAccel = asset->hipGunKickAccel;
			weapon->hipGunKickSpeedMax = asset->hipGunKickSpeedMax;
			weapon->hipGunKickSpeedDecay = asset->hipGunKickSpeedDecay;
			weapon->hipGunKickStaticDecay = asset->hipGunKickStaticDecay;
			weapon->hipGunKickReducedKickBullets = asset->hipGunKickReducedKickBullets;
			weapon->hipGunKickReducedKickPercent = asset->hipGunKickReducedKickPercent;
			weapon->hipViewKickPitchMin = asset->hipViewKickPitchMin;
			weapon->hipViewKickPitchMax = asset->hipViewKickPitchMax;
			weapon->hipViewKickYawMin = asset->hipViewKickYawMin;
			weapon->hipViewKickYawMax = asset->hipViewKickYawMax;
			weapon->hipViewKickMagMin = asset->hipViewKickMagMin;
			weapon->hipViewKickCenterSpeed = asset->hipViewKickCenterSpeed;
			weapon->hipViewScatterMin = asset->hipViewScatterMin;
			weapon->hipViewScatterMax = asset->hipViewScatterMax;
			weapon->viewKickScale = asset->viewKickScale;
			weapon->decreasingKick = asset->decreasingKick;
			weapon->viewKickPitchUpOnly = false; // s2 makes every pitch kick positive when this is set, h1 kicks can go down

			// s2 switches to the alt kick after adsViewKickAltBullets
			weapon->adsViewKickAltBullets = 0;
			weapon->hipViewKickAltBullets = 0;
		}

		void convert_projectile(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon, utils::memory::allocator& allocator)
		{
			weapon->projectileName = asset->projectileName;
			weapon->projectileSpeed = asset->projectileSpeed;
			weapon->projectileSpeedUp = asset->projectileSpeedUp;
			weapon->projectileSpeedForward = asset->projectileSpeedForward;
			weapon->projectileActivateDist = asset->projectileActivateDist;
			weapon->projLifetime = asset->projLifetime;
			weapon->timeToAccelerate = asset->timeToAccelerate;
			weapon->projectileCurvature = asset->projectileCurvature;
			weapon->projExplosion = asset->projExplosion;
			weapon->stickiness = asset->stickiness;
			weapon->ricochetChance = asset->ricochetChance;
			weapon->parallelBounce = convert_surface_array(asset->parallelBounce, allocator, std::identity{});
			weapon->perpendicularBounce = convert_surface_array(asset->perpendicularBounce, allocator, std::identity{});
			std::memcpy(weapon->projectileColor, asset->projectileColor, sizeof(weapon->projectileColor));
			weapon->guidedMissileType = asset->guidedMissileType;
			weapon->maxSteeringAccel = asset->maxSteeringAccel;
			weapon->projIgnitionDelay = asset->projIgnitionDelay;
			weapon->destabilizationRateTime = asset->destabilizationRateTime;
			weapon->destabilizationCurvatureMax = asset->destabilizationCurvatureMax;
			weapon->destabilizeDistance = asset->destabilizeDistance;
			weapon->isRollingGrenade = asset->isRollingGrenade;
			weapon->projExplosionEffectForceNormalUp = asset->projExplosionEffectForceNormalUp;
			weapon->projExplosionEffectInheritParentDirection = asset->projExplosionEffectInheritParentDirection;
			weapon->projImpactExplode = asset->projImpactExplode;
			weapon->stickToPlayers = asset->stickToPlayers;
			weapon->stickToVehicles = asset->stickToVehicles;
			weapon->stickToTurrets = asset->stickToTurrets;
			weapon->thrownSideways = asset->thrownSideways;
			weapon->detonatesOnEmptyThrow = asset->hasDetonatorEmptyThrow;
			weapon->detonatesOnDoubleTap = asset->hasDetonatorDoubleTap;
			weapon->timedDetonation = asset->timedDetonation;
			weapon->fuseLitAfterImpact = asset->fuseLitAfterImpact;
			weapon->rotate = asset->rotate;
			weapon->holdButtonToThrow = asset->holdButtonToThrow;
			weapon->bigExplosion = asset->bigExplosion;
			weapon->isAirburstWeapon = asset->isAirburstWeapon;
			weapon->projSuppressedByEMP = asset->projSuppressedByEMP;
			weapon->projDisabledByEMP = asset->projDisabledByEMP;
			weapon->projPlayTrailEffectForOwnerOnly = asset->projPlayTrailEffectForOwnerOnly;
			weapon->projPlayBeaconEffectForOwnerOnly = asset->projPlayBeaconEffectForOwnerOnly;
			weapon->projKillTrailEffectOnDeath = asset->projKillTrailEffectOnDeath;
			weapon->projKillBeaconEffectOnDeath = asset->projKillBeaconEffectOnDeath;
			weapon->projTrajectoryEvents = asset->projTrajectoryEvents;
			weapon->projWhizByEnabled = asset->projWhizByEnabled;
			weapon->noCrumpleMissile = asset->noCrumpleMissile;
			weapon->notifyOnPlayerImpact = asset->notifyOnPlayerImpact;
		}

		void convert_ammo(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->szAmmoName = asset->szAmmoName;
			weapon->szClipName = asset->szClipName;
			weapon->szSharedAmmoCapName = asset->szSharedAmmoCapName;
			weapon->maxAmmo = asset->maxAmmo;
			weapon->minAmmoReq = asset->minAmmoReq;
			weapon->startAmmo = asset->startAmmo;
			weapon->clipSize = asset->clipSize;
			weapon->shotCount = asset->shotCount;
			weapon->sharedAmmoCapIndex = asset->sharedAmmoCapIndex;
			weapon->sharedAmmoCap = asset->sharedAmmoCap;
			weapon->reloadAmmoAdd = asset->reloadAmmoAdd;
			weapon->reloadStartAdd = asset->reloadStartAdd;
			weapon->ammoDropStockMin = asset->ammoDropStockMin;
			weapon->ammoDropStockMax = asset->ammoDropStockMax;
			weapon->ammoDropClipPercentMin = asset->ammoDropClipPercentMin;
			weapon->ammoDropClipPercentMax = asset->ammoDropClipPercentMax;
			weapon->lowAmmoWarningThreshold = asset->lowAmmoWarningThreshold;
			weapon->generateAmmo = asset->generateAmmo;
			weapon->ammoPerShot = asset->ammoPerShot;
			weapon->explodeCount = asset->explodeCount;
			weapon->clipOnly = asset->clipOnly;
			weapon->noAmmoPickup = asset->noAmmoPickup;
			weapon->noPartialReload = asset->noPartialReload;
			weapon->segmentedReload = asset->segmentedReload;
			weapon->multipleReload = asset->multipleReload;
			weapon->useDualMagReloadAnims = asset->dualMagReloadSupported;
			weapon->useFastReloadAnims = asset->useFastReloadAnims;
			weapon->reloadStopsAlt = asset->reloadStopsAlt;
			weapon->sharedAmmo = asset->sharedAmmo;
			weapon->useBattery = asset->useBattery;
			weapon->disableSwitchToWhenEmpty = asset->disableSwitchToWhenEmpty;
			weapon->suppressAmmoReserveDisplay = asset->suppressAmmoReserveDisplay;
			weapon->signatureAmmoAlternate = asset->signatureAmmoAlternate;

			// s2 gives the ammo as clips when these are set, h1 gives it as rounds
			weapon->maxAmmoClips = 0;
			weapon->startAmmoClips = 0;
		}

		void convert_turret(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->leftArc = asset->leftArc;
			weapon->rightArc = asset->rightArc;
			weapon->topArc = asset->topArc;
			weapon->bottomArc = asset->bottomArc;
			weapon->turretADSTime = asset->turretADSTime;
			weapon->turretFov = asset->turretFov;
			weapon->turretFovADS = asset->turretFovADS;
			weapon->turretScopeZoomRate = asset->turretScopeZoomRate;
			weapon->turretScopeZoomMin = asset->turretScopeZoomMin;
			weapon->turretScopeZoomMax = asset->turretScopeZoomMax;
			weapon->overheatUpRate = asset->overheatUpRate;
			weapon->overheatDownRate = asset->overheatDownRate;
			weapon->overheatCooldownRate = asset->overheatCooldownRate;
			weapon->overheatPenalty = asset->overheatPenalty;
			weapon->turretBarrelSpinSpeed = asset->turretBarrelSpinSpeed;
			weapon->turretBarrelSpinUpTime = asset->turretBarrelSpinUpTime;
			weapon->turretADSEnabled = asset->turretADSEnabled;
			weapon->turretBarrelSpinEnabled = asset->turretBarrelSpinEnabled;
			weapon->turretHydraulicSettings = nullptr;
		}

		void convert_flags(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->rattleSoundType = asset->rattleSoundType;
			weapon->meleeOverrideValues = asset->meleeOverrideValues;
			weapon->lockonSupported = asset->lockonSupported;
			weapon->requireLockonToFire = asset->requireLockonToFire;
			weapon->inheritsPerks = asset->inheritsPerks;
			weapon->crosshairColorChange = asset->crosshairColorChange;
			weapon->rifleBullet = asset->rifleBullet;
			weapon->armorPiercing = asset->armorPiercing;
			weapon->boltAction = asset->boltAction;
			weapon->bulletExplosiveDamage = asset->bulletExplosiveDamage;
			weapon->cookOffHold = asset->cookOffHold;
			weapon->reticleSpin45 = asset->reticleSpin45;
			weapon->motionTracker = asset->motionTracker;
			weapon->markableViewmodel = asset->markableViewmodel;
			weapon->flipKillIcon = asset->flipKillIcon;
			weapon->dpadIconShowsAmmo = asset->actionSlotShowAmmo;
			weapon->blocksProne = asset->blocksProne;
			weapon->silenced = asset->silenced;
			weapon->disableFiring = asset->disableFiring;
			weapon->freezeMovementWhenFiring = asset->freezeMovementWhenFiring;
			weapon->offhandHoldIsCancelable = asset->offhandHoldIsCancelable;
			weapon->alwaysShatterGlassOnImpact = asset->alwaysShatterGlassOnImpact;
			weapon->counterSilencer = asset->counterSilencer;
			weapon->cloaked = asset->cloaked;
			weapon->useScriptCallbackForHit = asset->useScriptCallbackForHit;
			weapon->knifeAlwaysAttached = asset->knifeAlwaysAttached;
			weapon->allowPrimaryWeaponPickup = asset->allowPrimaryWeaponPickup;
			weapon->avoidDropCleanup = asset->avoidDropCleanup;
			weapon->noDualWield = asset->noDualWield;
			weapon->outlineEnemies = asset->outlineEnemies;
			weapon->altModeSameWeapon = asset->altModeSameWeapon;
			weapon->reticleDetonateHide = asset->reticleDetonateHide;
			weapon->autosimDisableVariableRate = asset->autosimDisableVariableRate;
			weapon->autosimSpeedScale = asset->autosimSpeedScale;
			weapon->hasTransientModels = false; // TODO: tr fastfile support later on will change this
		}

		void convert_aim(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			weapon->autoAimRange = asset->autoAimRange;
			weapon->aimAssistRange = asset->aimAssistRange;
			weapon->aimAssistRangeAds = asset->aimAssistRangeAds;
			weapon->aimPadding = asset->aimPadding;
			weapon->enemyCrosshairRange = asset->enemyCrosshairRange;
			weapon->reticleCenterSize = asset->reticleCenterSize;
			weapon->reticleSideSize = asset->reticleSideSize;
			weapon->reticleMinOfs = asset->reticleMinOfs;
			weapon->accuracy = asset->accuracy;
			weapon->aiSpread = asset->aiSpread;
			weapon->playerSpread = asset->playerSpread;
			std::memcpy(weapon->minTurnSpeed, asset->minTurnSpeed, sizeof(weapon->minTurnSpeed));
			std::memcpy(weapon->maxTurnSpeed, asset->maxTurnSpeed, sizeof(weapon->maxTurnSpeed));
			weapon->pitchConvergenceTime = asset->pitchConvergenceTime;
			weapon->yawConvergenceTime = asset->yawConvergenceTime;
			weapon->suppressTime = asset->suppressTime;
			weapon->maxRange = asset->maxRange;
			weapon->fightDist = asset->fightDist;
			weapon->maxDist = asset->maxDist;
			weapon->animHorRotateInc = asset->animHorRotateInc;
			weapon->playerPositionDist = asset->playerPositionDist;
			weapon->horizViewJitter = asset->horizViewJitter;
			weapon->vertViewJitter = asset->vertViewJitter;
			weapon->scanSpeed = asset->scanSpeed;
			weapon->scanAccel = asset->scanAccel;
			weapon->scanPauseTime = asset->scanPauseTime;
			weapon->aim_automelee_lerp = asset->aim_automelee_lerp;
			weapon->aim_automelee_range = asset->aim_automelee_range;
			weapon->aim_automelee_region_width = asset->aim_automelee_region_width;
			weapon->aim_automelee_region_height = asset->aim_automelee_region_height;
			weapon->player_meleeHeight = asset->player_meleeHeight;
			weapon->player_meleeRange = asset->player_meleeRange;
			weapon->player_meleeWidth = asset->player_meleeWidth;
			weapon->accuracyGraphName[0] = asset->accuracyGraphName[0];
			weapon->accuracyGraphName[1] = asset->accuracyGraphName[1];
			for (auto i = 0u; i < std::size(weapon->accuracyGraphKnots); i++)
			{
				weapon->accuracyGraphKnots[i] = asset->accuracyGraphKnots[i];
				weapon->originalAccuracyGraphKnots[i] = asset->originalAccuracyGraphKnots[i];
				weapon->accuracyGraphKnotCount[i] = asset->accuracyGraphKnotCount[i];
				weapon->originalAccuracyGraphKnotCount[i] = asset->originalAccuracyGraphKnotCount[i];
			}
		}

		void convert_type(const WeaponDef* asset, zonetool::s2::WeaponCompleteDef* weapon)
		{
			if (asset->inventoryType == WEAPINVENTORY_MELEE)
			{
				weapon->weapType = zonetool::s2::WEAPTYPE_MELEE; // h1 marks melee weapons by inventory type, s2 by weapon type and class
				weapon->weapClass = zonetool::s2::WEAPCLASS_KNIFE;
			}
			else
			{
				weapon->weapType = convert_weap_type(asset->weapType);
				if (const auto weap_class = convert_weap_class(asset->weapClass))
				{
					weapon->weapClass = *weap_class;
				}
			}

			weapon->inventoryType = convert_inventory_type(asset->inventoryType);
			weapon->fireType = static_cast<zonetool::s2::weapFireType_t>(asset->fireType);
			weapon->fireBarrels = asset->fireBarrels;
			weapon->adsFireMode = asset->adsFireMode;
			weapon->burstFireCooldown = asset->burstFireCooldown;
			weapon->greebleType = static_cast<zonetool::s2::weapGreebleType_t>(asset->greebleType);
			weapon->autoReloadType = asset->autoReloadType;
			weapon->autoHolsterType = asset->autoHolsterType;
			weapon->offhandClass = asset->offhandClass;
			weapon->stance = asset->stance;
			weapon->penetrateType = static_cast<zonetool::s2::PenetrateType>(asset->penetrateType);
			weapon->penetrateDepth = asset->penetrateDepth;
			weapon->impactType = weapon->weapType == zonetool::s2::WEAPTYPE_MELEE
				? zonetool::s2::IMPACT_TYPE_MELEE : static_cast<zonetool::s2::ImpactType>(asset->impactType); // s2 melee weapons have their own impact type
			weapon->fireAnimLength = asset->fireAnimLength;
			weapon->fireAnimLengthAkimbo = asset->fireAnimLengthAkimbo;
			weapon->positionReloadTransTime = asset->positionReloadTransTime;
			weapon->changedFireTime = asset->changedFireTime;
			weapon->changedFireTimeNumBullets = asset->changedFireTimeNumBullets;
			weapon->fireTimeInterpolationType = asset->fireTimeInterpolationType;
		}
	}

	zonetool::s2::weapType_t convert_weap_type(const weapType_t type)
	{
		switch (type)
		{
		case WEAPTYPE_BULLET:
			return zonetool::s2::WEAPTYPE_BULLET;
		case WEAPTYPE_GRENADE:
			return zonetool::s2::WEAPTYPE_GRENADE;
		case WEAPTYPE_PROJECTILE:
			return zonetool::s2::WEAPTYPE_PROJECTILE;
		case WEAPTYPE_RIOTSHIELD:
			return zonetool::s2::WEAPTYPE_RIOTSHIELD;
		case WEAPTYPE_ENERGY:
			return zonetool::s2::WEAPTYPE_ENERGY;
		default:
			return zonetool::s2::WEAPTYPE_NONE;
		}
	}

	std::optional<zonetool::s2::weapClass_t> convert_weap_class(const weapClass_t weap_class)
	{
		switch (weap_class)
		{
		case WEAPCLASS_ITEM:
			return zonetool::s2::WEAPCLASS_ITEM;
		case WEAPCLASS_CONE:
			return std::nullopt; // s2 has no cone class
		default:
			if (weap_class < 0 || weap_class >= WEAPCLASS_NUM)
			{
				return std::nullopt;
			}

			return static_cast<zonetool::s2::weapClass_t>(weap_class); // the other h1 classes have the same s2 values
		}
	}

	void dump(WeaponDef* asset)
	{
		if (!dumped_weapons.insert(asset->name).second)
		{
			return;
		}

		utils::memory::allocator allocator;
		auto* weapon = allocator.allocate<zonetool::s2::WeaponCompleteDef>();
		set_s2_only_defaults(get_profile(asset), weapon, allocator);
		weapon->szInternalName = asset->szInternalName;
		weapon->szDisplayName = asset->szDisplayName;
		weapon->szAdsrBaseSetting = asset->szAdsrBaseSetting;
		weapon->szUseHintString = asset->szUseHintString;
		weapon->dropHintString = asset->dropHintString;
		weapon->useHintStringIndex = asset->useHintStringIndex;
		weapon->dropHintStringIndex = asset->dropHintStringIndex;
		weapon->szScript = asset->szScript;
		weapon->hideTags = convert_script_strings(asset->hideTags, hide_tag_count, allocator);
		weapon->szXAnimsRightHanded = convert_anims(asset->szXAnimsRightHanded, allocator);
		weapon->szXAnimsLeftHanded = convert_anims(asset->szXAnimsLeftHanded, allocator);
		weapon->szXAnims = convert_anims(asset->szXAnims, allocator);

		convert_type(asset, weapon);
		convert_weapon_models(asset, weapon, allocator);
		convert_hud(asset, weapon, allocator);
		convert_effects(asset, weapon, allocator);
		convert_attachments(asset, weapon, allocator);
		convert_overrides(asset, weapon, allocator);
		convert_notetrack_maps(asset, weapon, allocator);
		convert_sounds(asset, weapon, allocator);
		convert_state_timers(asset->stateTimers, weapon->stateTimers);
		convert_state_timers(asset->akimboStateTimers, weapon->akimboStateTimers);
		weapon->stateTimers.inspectTime = asset->inspectAnimTime;
		weapon->akimboStateTimers.inspectTime = asset->inspectAnimTime;
		convert_damage(asset, weapon);
		convert_movement(asset, weapon);
		convert_ads(asset, weapon);
		convert_spread(asset, weapon);
		convert_kick(asset, weapon);
		convert_projectile(asset, weapon, allocator);
		convert_ammo(asset, weapon);
		convert_turret(asset, weapon);
		convert_flags(asset, weapon);
		convert_aim(asset, weapon);

		zonetool::s2::weapon_def::dump(weapon);
		add_csv_row(zonetool::s2::type_to_string(zonetool::s2::ASSET_TYPE_WEAPON), asset->name);
	}

	void clear()
	{
		dumped_weapons.clear();
	}
}
