#include <std_include.hpp>

#include "zonetool/h1/converter/h2/include.hpp"
#include "sound.hpp"
#include "zonetool/h2/assets/weapondef.hpp"
#include "game/h1/game.hpp"

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace weapondef
		{
			namespace
			{
				std::unordered_map<std::uint32_t, std::uint32_t> anim_type_map =
				{
					{zonetool::h1::WEAP_ANIM_INVALID, zonetool::h2::WEAP_ANIM_INVALID},
					{zonetool::h1::WEAP_ANIM_ROOT, zonetool::h2::WEAP_ANIM_ROOT},
					{zonetool::h1::WEAP_ANIM_IDLE, zonetool::h2::WEAP_ANIM_IDLE},
					{zonetool::h1::WEAP_ANIM_EMPTY_IDLE, zonetool::h2::WEAP_ANIM_EMPTY_IDLE},
					{zonetool::h1::WEAP_ANIM_FIRE, zonetool::h2::WEAP_ANIM_FIRE},
					{zonetool::h1::WEAP_ANIM_HOLD_FIRE, zonetool::h2::WEAP_ANIM_HOLD_FIRE},
					{zonetool::h1::WEAP_ANIM_LASTSHOT, zonetool::h2::WEAP_ANIM_LASTSHOT},
					{zonetool::h1::WEAP_ANIM_RECHAMBER, zonetool::h2::WEAP_ANIM_RECHAMBER},
					{zonetool::h1::WEAP_ANIM_GRENADE_PRIME, zonetool::h2::WEAP_ANIM_GRENADE_PRIME},
					{zonetool::h1::WEAP_ANIM_GRENADE_PRIME_READY_TO_THROW, zonetool::h2::WEAP_ANIM_GRENADE_PRIME_READY_TO_THROW},
					{zonetool::h1::WEAP_ANIM_MELEE_SWIPE, zonetool::h2::WEAP_ANIM_MELEE_SWIPE},
					{zonetool::h1::WEAP_ANIM_MELEE_HIT, zonetool::h2::WEAP_ANIM_MELEE_HIT},
					{zonetool::h1::WEAP_ANIM_MELEE_FATAL, zonetool::h2::WEAP_ANIM_MELEE_FATAL},
					{zonetool::h1::WEAP_ANIM_MELEE_MISS, zonetool::h2::WEAP_ANIM_MELEE_MISS},
					{zonetool::h1::WEAP_ANIM_MELEE_VICTIM_CROUCHING_HIT, zonetool::h2::WEAP_ANIM_MELEE_VICTIM_CROUCHING_HIT},
					{zonetool::h1::WEAP_ANIM_MELEE_VICTIM_CROUCHING_FATAL, zonetool::h2::WEAP_ANIM_MELEE_VICTIM_CROUCHING_FATAL},
					{zonetool::h1::WEAP_ANIM_MELEE_VICTIM_CROUCHING_MISS, zonetool::h2::WEAP_ANIM_MELEE_VICTIM_CROUCHING_MISS},
					{zonetool::h1::WEAP_ANIM_MELEE_ALT_STANDING, zonetool::h2::WEAP_ANIM_MELEE_ALT_STANDING},
					{zonetool::h1::WEAP_ANIM_MELEE_ALT_CROUCHING, zonetool::h2::WEAP_ANIM_MELEE_ALT_CROUCHING},
					{zonetool::h1::WEAP_ANIM_MELEE_ALT_PRONE, zonetool::h2::WEAP_ANIM_MELEE_ALT_PRONE},
					{zonetool::h1::WEAP_ANIM_MELEE_ALT_JUMPING, zonetool::h2::WEAP_ANIM_MELEE_ALT_JUMPING},
					{zonetool::h1::WEAP_ANIM_MELEE_ALT_STANDING_VICTIM_CROUCHING, zonetool::h2::WEAP_ANIM_MELEE_ALT_STANDING_VICTIM_CROUCHING},
					{zonetool::h1::WEAP_ANIM_MELEE_ALT_CROUCHING_VICTIM_CROUCHING, zonetool::h2::WEAP_ANIM_MELEE_ALT_CROUCHING_VICTIM_CROUCHING},
					{zonetool::h1::WEAP_ANIM_MELEE_ALT_PRONE_VICTIM_CROUCHING, zonetool::h2::WEAP_ANIM_MELEE_ALT_PRONE_VICTIM_CROUCHING},
					{zonetool::h1::WEAP_ANIM_MELEE_ALT_JUMPING_VICTIM_CROUCHING, zonetool::h2::WEAP_ANIM_MELEE_ALT_JUMPING_VICTIM_CROUCHING},
					{zonetool::h1::WEAP_ANIM_RELOAD, zonetool::h2::WEAP_ANIM_RELOAD},
					{zonetool::h1::WEAP_ANIM_RELOAD_EMPTY, zonetool::h2::WEAP_ANIM_RELOAD_EMPTY},
					{zonetool::h1::WEAP_ANIM_RELOAD_START, zonetool::h2::WEAP_ANIM_RELOAD_START},
					{zonetool::h1::WEAP_ANIM_RELOAD_END, zonetool::h2::WEAP_ANIM_RELOAD_END},
					{zonetool::h1::WEAP_ANIM_FAST_RELOAD, zonetool::h2::WEAP_ANIM_FAST_RELOAD},
					{zonetool::h1::WEAP_ANIM_FAST_RELOAD_EMPTY, zonetool::h2::WEAP_ANIM_FAST_RELOAD_EMPTY},
					{zonetool::h1::WEAP_ANIM_FAST_RELOAD_START, zonetool::h2::WEAP_ANIM_FAST_RELOAD_START},
					{zonetool::h1::WEAP_ANIM_FAST_RELOAD_END, zonetool::h2::WEAP_ANIM_FAST_RELOAD_END},
					{zonetool::h1::WEAP_ANIM_DUALMAG_RELOAD, zonetool::h2::WEAP_ANIM_DUALMAG_RELOAD},
					{zonetool::h1::WEAP_ANIM_DUALMAG_RELOAD_EMPTY, zonetool::h2::WEAP_ANIM_DUALMAG_RELOAD_EMPTY},
					{zonetool::h1::WEAP_ANIM_SPEED_RELOAD, zonetool::h2::WEAP_ANIM_SPEED_RELOAD},
					{zonetool::h1::WEAP_ANIM_RELOAD_FROM_ALT, zonetool::h2::WEAP_ANIM_RELOAD_FROM_ALT},
					{zonetool::h1::WEAP_ANIM_RAISE, zonetool::h2::WEAP_ANIM_RAISE},
					{zonetool::h1::WEAP_ANIM_FIRST_RAISE, zonetool::h2::WEAP_ANIM_FIRST_RAISE},
					{zonetool::h1::WEAP_ANIM_BREACH_RAISE, zonetool::h2::WEAP_ANIM_BREACH_RAISE},
					{zonetool::h1::WEAP_ANIM_DROP, zonetool::h2::WEAP_ANIM_DROP},
					{zonetool::h1::WEAP_ANIM_ALT_RAISE, zonetool::h2::WEAP_ANIM_ALT_RAISE},
					{zonetool::h1::WEAP_ANIM_ALT_DROP, zonetool::h2::WEAP_ANIM_ALT_DROP},
					{zonetool::h1::WEAP_ANIM_ALT_OVERRIDE, zonetool::h2::WEAP_ANIM_ALT_OVERRIDE},
					{zonetool::h1::WEAP_ANIM_QUICK_RAISE, zonetool::h2::WEAP_ANIM_QUICK_RAISE},
					{zonetool::h1::WEAP_ANIM_QUICK_DROP, zonetool::h2::WEAP_ANIM_QUICK_DROP},
					{zonetool::h1::WEAP_ANIM_EMPTY_RAISE, zonetool::h2::WEAP_ANIM_EMPTY_RAISE},
					{zonetool::h1::WEAP_ANIM_EMPTY_DROP, zonetool::h2::WEAP_ANIM_EMPTY_DROP},
					{zonetool::h1::WEAP_ANIM_HYBRID_SIGHT_ON, zonetool::h2::WEAP_ANIM_HYBRID_SIGHT_ON},
					{zonetool::h1::WEAP_ANIM_HYBRID_SIGHT_OFF, zonetool::h2::WEAP_ANIM_HYBRID_SIGHT_OFF},
					{zonetool::h1::WEAP_ANIM_SPRINT_IN, zonetool::h2::WEAP_ANIM_SPRINT_IN},
					{zonetool::h1::WEAP_ANIM_SPRINT_IN_FROM_SLIDE, zonetool::h2::WEAP_ANIM_SPRINT_IN_FROM_SLIDE},
					{zonetool::h1::WEAP_ANIM_SPRINT_IN_CANCEL, zonetool::h2::WEAP_ANIM_SPRINT_IN_CANCEL},
					{zonetool::h1::WEAP_ANIM_SPRINT_LOOP, zonetool::h2::WEAP_ANIM_SPRINT_LOOP},
					{zonetool::h1::WEAP_ANIM_SPRINT_OUT, zonetool::h2::WEAP_ANIM_SPRINT_OUT},
					{zonetool::h1::WEAP_ANIM_SPRINTANDFIRE_IN, zonetool::h2::WEAP_ANIM_SPRINTANDFIRE_IN},
					{zonetool::h1::WEAP_ANIM_SPRINTANDFIRE_LOOP, zonetool::h2::WEAP_ANIM_SPRINTANDFIRE_LOOP},
					{zonetool::h1::WEAP_ANIM_SPRINTANDFIRE_OUT, zonetool::h2::WEAP_ANIM_SPRINTANDFIRE_OUT},
					{zonetool::h1::WEAP_ANIM_STUNNED_START, zonetool::h2::WEAP_ANIM_STUNNED_START},
					{zonetool::h1::WEAP_ANIM_STUNNED_LOOP, zonetool::h2::WEAP_ANIM_STUNNED_LOOP},
					{zonetool::h1::WEAP_ANIM_STUNNED_END, zonetool::h2::WEAP_ANIM_STUNNED_END},
					{zonetool::h1::WEAP_ANIM_THROWBACK, zonetool::h2::WEAP_ANIM_THROWBACK},
					{zonetool::h1::WEAP_ANIM_DETONATE, zonetool::h2::WEAP_ANIM_DETONATE},
					{zonetool::h1::WEAP_ANIM_NIGHTVISION_WEAR, zonetool::h2::WEAP_ANIM_NIGHTVISION_WEAR},
					{zonetool::h1::WEAP_ANIM_NIGHTVISION_REMOVE, zonetool::h2::WEAP_ANIM_NIGHTVISION_REMOVE},
					{zonetool::h1::WEAP_ANIM_ADS_FIRE, zonetool::h2::WEAP_ANIM_ADS_FIRE},
					{zonetool::h1::WEAP_ANIM_ADS_LASTSHOT, zonetool::h2::WEAP_ANIM_ADS_LASTSHOT},
					{zonetool::h1::WEAP_ANIM_ADS_RECHAMBER, zonetool::h2::WEAP_ANIM_ADS_RECHAMBER},
					{zonetool::h1::WEAP_ANIM_BLAST_FRONT, zonetool::h2::WEAP_ANIM_BLAST_FRONT},
					{zonetool::h1::WEAP_ANIM_BLAST_RIGHT, zonetool::h2::WEAP_ANIM_BLAST_RIGHT},
					{zonetool::h1::WEAP_ANIM_BLAST_BACK, zonetool::h2::WEAP_ANIM_BLAST_BACK},
					{zonetool::h1::WEAP_ANIM_BLAST_LEFT, zonetool::h2::WEAP_ANIM_BLAST_LEFT},
					{zonetool::h1::WEAP_ANIM_SLIDE_IN, zonetool::h2::WEAP_ANIM_SLIDE_IN},
					{zonetool::h1::WEAP_ANIM_SLIDE_LOOP, zonetool::h2::WEAP_ANIM_SLIDE_LOOP},
					{zonetool::h1::WEAP_ANIM_SLIDE_OUT_TO_SPRINT, zonetool::h2::WEAP_ANIM_SLIDE_OUT_TO_SPRINT},
					{zonetool::h1::WEAP_ANIM_SLIDE_OUT, zonetool::h2::WEAP_ANIM_SLIDE_OUT},
					{zonetool::h1::WEAP_ANIM_SLIDE_AND_FIRE_IN, zonetool::h2::WEAP_ANIM_SLIDE_AND_FIRE_IN},
					{zonetool::h1::WEAP_ANIM_SLIDE_AND_FIRE_LOOP, zonetool::h2::WEAP_ANIM_SLIDE_AND_FIRE_LOOP},
					{zonetool::h1::WEAP_ANIM_SLIDE_AND_FIRE_OUT, zonetool::h2::WEAP_ANIM_SLIDE_AND_FIRE_OUT},
					{zonetool::h1::WEAP_ANIM_HIGH_JUMP_IN, zonetool::h2::WEAP_ANIM_HIGH_JUMP_IN},
					{zonetool::h1::WEAP_ANIM_HIGH_JUMP_DROP_IN, zonetool::h2::WEAP_ANIM_HIGH_JUMP_DROP_IN},
					{zonetool::h1::WEAP_ANIM_HIGH_JUMP_DROP_LOOP, zonetool::h2::WEAP_ANIM_HIGH_JUMP_DROP_LOOP},
					{zonetool::h1::WEAP_ANIM_HIGH_JUMP_DROP_LAND, zonetool::h2::WEAP_ANIM_HIGH_JUMP_DROP_LAND},
					{zonetool::h1::WEAP_ANIM_DODGE_GROUND_BACK, zonetool::h2::WEAP_ANIM_DODGE_GROUND_BACK},
					{zonetool::h1::WEAP_ANIM_DODGE_GROUND_LEFT, zonetool::h2::WEAP_ANIM_DODGE_GROUND_LEFT},
					{zonetool::h1::WEAP_ANIM_DODGE_GROUND_RIGHT, zonetool::h2::WEAP_ANIM_DODGE_GROUND_RIGHT},
					{zonetool::h1::WEAP_ANIM_DODGE_AIR_FORWARD, zonetool::h2::WEAP_ANIM_DODGE_AIR_FORWARD},
					{zonetool::h1::WEAP_ANIM_DODGE_AIR_BACK, zonetool::h2::WEAP_ANIM_DODGE_AIR_BACK},
					{zonetool::h1::WEAP_ANIM_DODGE_AIR_LEFT, zonetool::h2::WEAP_ANIM_DODGE_AIR_LEFT},
					{zonetool::h1::WEAP_ANIM_DODGE_AIR_RIGHT, zonetool::h2::WEAP_ANIM_DODGE_AIR_RIGHT},
					{zonetool::h1::WEAP_ANIM_LAND_DIP, zonetool::h2::WEAP_ANIM_LAND_DIP},
					{zonetool::h1::WEAP_ANIM_RECOIL_SETTLE, zonetool::h2::WEAP_ANIM_RECOIL_SETTLE},
					{zonetool::h1::WEAP_ANIM_SWIM_LOOP, zonetool::h2::WEAP_ANIM_SWIM_LOOP},
					{zonetool::h1::WEAP_ANIM_MANTLE_UP_64, zonetool::h2::WEAP_ANIM_MANTLE_UP_64},
					{zonetool::h1::WEAP_ANIM_MANTLE_UP_56, zonetool::h2::WEAP_ANIM_MANTLE_UP_56},
					{zonetool::h1::WEAP_ANIM_MANTLE_UP_48, zonetool::h2::WEAP_ANIM_MANTLE_UP_48},
					{zonetool::h1::WEAP_ANIM_MANTLE_UP_40, zonetool::h2::WEAP_ANIM_MANTLE_UP_40},
					{zonetool::h1::WEAP_ANIM_MANTLE_UP_32, zonetool::h2::WEAP_ANIM_MANTLE_UP_32},
					{zonetool::h1::WEAP_ANIM_MANTLE_UP_24, zonetool::h2::WEAP_ANIM_MANTLE_UP_24},
					{zonetool::h1::WEAP_ANIM_MANTLE_UP_16, zonetool::h2::WEAP_ANIM_MANTLE_UP_16},
					{zonetool::h1::WEAP_ANIM_MANTLE_OVER_64, zonetool::h2::WEAP_ANIM_MANTLE_OVER_64},
					{zonetool::h1::WEAP_ANIM_MANTLE_OVER_56, zonetool::h2::WEAP_ANIM_MANTLE_OVER_56},
					{zonetool::h1::WEAP_ANIM_MANTLE_OVER_48, zonetool::h2::WEAP_ANIM_MANTLE_OVER_48},
					{zonetool::h1::WEAP_ANIM_MANTLE_OVER_40, zonetool::h2::WEAP_ANIM_MANTLE_OVER_40},
					{zonetool::h1::WEAP_ANIM_MANTLE_OVER_32, zonetool::h2::WEAP_ANIM_MANTLE_OVER_32},
					{zonetool::h1::WEAP_ANIM_MANTLE_OVER_24, zonetool::h2::WEAP_ANIM_MANTLE_OVER_24},
					{zonetool::h1::WEAP_ANIM_MANTLE_OVER_16, zonetool::h2::WEAP_ANIM_MANTLE_OVER_16},
					{zonetool::h1::WEAP_ANIM_GOLIATH_ENTRY, zonetool::h2::WEAP_ANIM_GOLIATH_ENTRY},
					{zonetool::h1::WEAP_ANIM_OFFHAND_SWITCH, zonetool::h2::WEAP_ANIM_OFFHAND_SWITCH},
					{zonetool::h1::WEAP_ANIM_HEAT_COOLDOWN_IN, zonetool::h2::WEAP_ANIM_HEAT_COOLDOWN_IN},
					{zonetool::h1::WEAP_ANIM_HEAT_COOLDOWN_OUT, zonetool::h2::WEAP_ANIM_HEAT_COOLDOWN_OUT},
					{zonetool::h1::WEAP_ANIM_OVERHEAT_OUT, zonetool::h2::WEAP_ANIM_OVERHEAT_OUT},
					{zonetool::h1::WEAP_ANIM_SCRIPTED, zonetool::h2::WEAP_ANIM_SCRIPTED},
					{zonetool::h1::WEAP_ANIM_INSPECTION, zonetool::h2::WEAP_ANIM_INSPECTION},
					{zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_1, zonetool::h2::WEAP_ANIM_RELOAD_MULTIPLE_1},
					{zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_2, zonetool::h2::WEAP_ANIM_RELOAD_MULTIPLE_2},
					{zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_3, zonetool::h2::WEAP_ANIM_RELOAD_MULTIPLE_3},
					{zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_4, zonetool::h2::WEAP_ANIM_RELOAD_MULTIPLE_4},
					{zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_5, zonetool::h2::WEAP_ANIM_RELOAD_MULTIPLE_5},
					{zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_6, zonetool::h2::WEAP_ANIM_RELOAD_MULTIPLE_6},
					{zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_7, zonetool::h2::WEAP_ANIM_RELOAD_MULTIPLE_7},
					{zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_8, zonetool::h2::WEAP_ANIM_RELOAD_MULTIPLE_8},
					{zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_FAST_1, zonetool::h2::WEAP_ANIM_RELOAD_MULTIPLE_FAST_1},
					{zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_FAST_2, zonetool::h2::WEAP_ANIM_RELOAD_MULTIPLE_FAST_2},
					{zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_FAST_3, zonetool::h2::WEAP_ANIM_RELOAD_MULTIPLE_FAST_3},
					{zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_FAST_4, zonetool::h2::WEAP_ANIM_RELOAD_MULTIPLE_FAST_4},
					{zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_FAST_5, zonetool::h2::WEAP_ANIM_RELOAD_MULTIPLE_FAST_5},
					{zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_FAST_6, zonetool::h2::WEAP_ANIM_RELOAD_MULTIPLE_FAST_6},
					{zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_FAST_7, zonetool::h2::WEAP_ANIM_RELOAD_MULTIPLE_FAST_7},
					{zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_FAST_8, zonetool::h2::WEAP_ANIM_RELOAD_MULTIPLE_FAST_8},
					{zonetool::h1::WEAP_ANIM_ADS_UP, zonetool::h2::WEAP_ANIM_ADS_UP},
					{zonetool::h1::WEAP_ANIM_ADS_DOWN, zonetool::h2::WEAP_ANIM_ADS_DOWN},
					{zonetool::h1::WEAP_ANIM_RECOIL, zonetool::h2::WEAP_ANIM_RECOIL},
					{zonetool::h1::WEAP_ALT_ANIM_ADJUST, zonetool::h2::WEAP_ALT_ANIM_ADJUST},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_ADS_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_ADS_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_ADS_UP, zonetool::h2::WEAP_ANIM_ADDITIVE_ADS_UP},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_HYBRID_SIGHT_UP_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_HYBRID_SIGHT_UP_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_HYBRID_SIGHT_UP, zonetool::h2::WEAP_ANIM_ADDITIVE_HYBRID_SIGHT_UP},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_DRAG_LEFT_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_DRAG_LEFT_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_DRAG_LEFT, zonetool::h2::WEAP_ANIM_ADDITIVE_DRAG_LEFT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_DRAG_RIGHT_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_DRAG_RIGHT_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_DRAG_RIGHT, zonetool::h2::WEAP_ANIM_ADDITIVE_DRAG_RIGHT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_DRAG_UP_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_DRAG_UP_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_DRAG_UP, zonetool::h2::WEAP_ANIM_ADDITIVE_DRAG_UP},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_DRAG_DOWN_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_DRAG_DOWN_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_DRAG_DOWN, zonetool::h2::WEAP_ANIM_ADDITIVE_DRAG_DOWN},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_SWIM_FORWARD_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_SWIM_FORWARD_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_SWIM_FORWARD, zonetool::h2::WEAP_ANIM_ADDITIVE_SWIM_FORWARD},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_SWIM_BACKWARD_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_SWIM_BACKWARD_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_SWIM_BACKWARD, zonetool::h2::WEAP_ANIM_ADDITIVE_SWIM_BACKWARD},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_JUMP_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_JUMP_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_JUMP, zonetool::h2::WEAP_ANIM_ADDITIVE_JUMP},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_JUMP_BOOST, zonetool::h2::WEAP_ANIM_ADDITIVE_JUMP_BOOST},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_JUMP_LAND_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_JUMP_LAND_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_JUMP_LAND, zonetool::h2::WEAP_ANIM_ADDITIVE_JUMP_LAND},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_JUMP_LAND_HEAVY, zonetool::h2::WEAP_ANIM_ADDITIVE_JUMP_LAND_HEAVY},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_WALK_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_WALK_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_WALK, zonetool::h2::WEAP_ANIM_ADDITIVE_WALK},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_HEAT_COOLDOWN_LOOP_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_HEAT_COOLDOWN_LOOP_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_HEAT_COOLDOWN_LOOP, zonetool::h2::WEAP_ANIM_ADDITIVE_HEAT_COOLDOWN_LOOP},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_OVERHEAT_IN_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_OVERHEAT_IN_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_OVERHEAT_IN, zonetool::h2::WEAP_ANIM_ADDITIVE_OVERHEAT_IN},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_OVERHEAT_LOOP_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_OVERHEAT_LOOP_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_OVERHEAT_LOOP, zonetool::h2::WEAP_ANIM_ADDITIVE_OVERHEAT_LOOP},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_IN_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_CRAWL_IN_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_IN, zonetool::h2::WEAP_ANIM_ADDITIVE_CRAWL_IN},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_LOOP, zonetool::h2::WEAP_ANIM_ADDITIVE_CRAWL_LOOP},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_BACK_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_BACK_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_BACK, zonetool::h2::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_BACK},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT, zonetool::h2::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT, zonetool::h2::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_PRONE_DROP_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_PRONE_DROP_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_PRONE_DROP, zonetool::h2::WEAP_ANIM_ADDITIVE_PRONE_DROP},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_EMPTY_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_EMPTY_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_EMPTY, zonetool::h2::WEAP_ANIM_ADDITIVE_EMPTY},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_MANTLE_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_MANTLE_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_MANTLE, zonetool::h2::WEAP_ANIM_ADDITIVE_MANTLE},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_LOW_MANTLE_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_LOW_MANTLE_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_MANTLE_UP_24, zonetool::h2::WEAP_ANIM_ADDITIVE_MANTLE_UP_24},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_MANTLE_UP_16, zonetool::h2::WEAP_ANIM_ADDITIVE_MANTLE_UP_16},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_MANTLE_OVER_24, zonetool::h2::WEAP_ANIM_ADDITIVE_MANTLE_OVER_24},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_MANTLE_OVER_16, zonetool::h2::WEAP_ANIM_ADDITIVE_MANTLE_OVER_16},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_SHOT_CHARGE_IN_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_SHOT_CHARGE_IN_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_SHOT_CHARGE_IN, zonetool::h2::WEAP_ANIM_ADDITIVE_SHOT_CHARGE_IN},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_SHOT_CHARGE_LOOP_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_SHOT_CHARGE_LOOP_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_SHOT_CHARGE_LOOP, zonetool::h2::WEAP_ANIM_ADDITIVE_SHOT_CHARGE_LOOP},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_SCRIPTED_ROOT, zonetool::h2::WEAP_ANIM_ADDITIVE_SCRIPTED_ROOT},
					{zonetool::h1::WEAP_ANIM_ADDITIVE_SCRIPTED, zonetool::h2::WEAP_ANIM_ADDITIVE_SCRIPTED},
				};
			}

			zonetool::h2::WeaponDef* convert(zonetool::h1::WeaponDef* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h2::WeaponDef>();

				COPY_VALUE(name);
				COPY_VALUE(szDisplayName);
				new_asset->szOverlayName = "";
				new_asset->szAttachmentName = "";
				COPY_VALUE(szAltWeaponName);
				COPY_ASSET_ARR(gunModel, 2);
				COPY_ASSET(handModel);
				COPY_ASSET(persistentArmXModel);
				COPY_ASSET_ARR(reticleViewModels, 64);
				COPY_VALUE(lobWorldModelName);
				new_asset->szXAnimsRightHanded = allocator.allocate_array<zonetool::h2::XAnimParts*>(zonetool::h2::NUM_WEAP_ANIMS);
				new_asset->szXAnimsLeftHanded = allocator.allocate_array<zonetool::h2::XAnimParts*>(zonetool::h2::NUM_WEAP_ANIMS);
				new_asset->szXAnims = allocator.allocate_array<zonetool::h2::XAnimParts*>(zonetool::h2::NUM_WEAP_ANIMS);
				for (auto i = 0; i < zonetool::h1::NUM_WEAP_ANIMS; i++)
				{
					const auto index = anim_type_map[i];

					if (asset->szXAnimsRightHanded[i] != nullptr)
					{
						new_asset->szXAnimsRightHanded[index] = allocator.allocate<zonetool::h2::XAnimParts>();
						new_asset->szXAnimsRightHanded[index]->name = asset->szXAnimsRightHanded[i]->name;
					}

					if (asset->szXAnimsLeftHanded[i] != nullptr)
					{
						new_asset->szXAnimsLeftHanded[index] = allocator.allocate<zonetool::h2::XAnimParts>();
						new_asset->szXAnimsLeftHanded[index]->name = asset->szXAnimsLeftHanded[i]->name;
					}

					if (asset->szXAnims[i] != nullptr)
					{
						new_asset->szXAnims[index] = allocator.allocate<zonetool::h2::XAnimParts>();
						new_asset->szXAnims[index]->name = asset->szXAnims[i]->name;
					}
				}
				COPY_VALUE(hideTags);
				COPY_ASSET_ARR(attachments, asset->numAttachments);
				new_asset->animOverrides = allocator.allocate_array<zonetool::h2::AnimOverrideEntry>(asset->numAnimOverrides);
				for (auto i = 0; i < asset->numAnimOverrides; i++)
				{
					COPY_VALUE(animOverrides[i].animHand);
					COPY_VALUE(animOverrides[i].attachment1);
					COPY_VALUE(animOverrides[i].attachment2);
					COPY_VALUE(animOverrides[i].animTreeType);
					COPY_ASSET(animOverrides[i].overrideAnim);
					COPY_ASSET(animOverrides[i].altmodeAnim);
					COPY_VALUE(animOverrides[i].animTime);
					COPY_VALUE(animOverrides[i].altTime);
				}
				new_asset->soundOverrides = allocator.allocate_array<zonetool::h2::SoundOverrideEntry>(asset->numSoundOverrides);
				for (auto i = 0; i < asset->numSoundOverrides; i++)
				{
					COPY_VALUE(soundOverrides[i].attachment1);
					COPY_VALUE(soundOverrides[i].attachment2);
					COPY_VALUE(soundOverrides[i].soundType);
					COPY_ASSET(soundOverrides[i].overrideSound);
					COPY_ASSET(soundOverrides[i].altmodeSound);
				}
				new_asset->fxOverrides = allocator.allocate_array<zonetool::h2::FXOverrideEntry>(asset->numFXOverrides);
				for (auto i = 0; i < asset->numFXOverrides; i++)
				{
					COPY_VALUE(fxOverrides[i].attachment1);
					COPY_VALUE(fxOverrides[i].attachment2);
					COPY_VALUE(fxOverrides[i].fxType);
					COPY_ASSET(fxOverrides[i].overrideFX);
					COPY_ASSET(fxOverrides[i].altmodeFX);
				}
				new_asset->reloadOverrides = allocator.allocate_array<zonetool::h2::ReloadStateTimerEntry>(asset->numReloadStateTimerOverrides);
				for (auto i = 0; i < asset->numReloadStateTimerOverrides; i++)
				{
					COPY_VALUE(reloadOverrides[i].attachment);
					COPY_VALUE(reloadOverrides[i].reloadAddTime);
					COPY_VALUE(reloadOverrides[i].reloadEmptyAddTime);
					COPY_VALUE(reloadOverrides[i].reloadStartAddTime);
				}
				new_asset->notetrackOverrides = allocator.allocate_array<zonetool::h2::NoteTrackToSoundEntry>(asset->numNotetrackOverrides);
				for (auto i = 0; i < asset->numNotetrackOverrides; i++)
				{
					COPY_VALUE(notetrackOverrides[i].attachment);
					COPY_VALUE(notetrackOverrides[i].notetrackSoundMapKeys);
					COPY_VALUE(notetrackOverrides[i].notetrackSoundMapValues);
				}
				COPY_VALUE(notetrackSoundMapKeys);
				COPY_VALUE(notetrackSoundMapValues);
				COPY_VALUE(notetrackRumbleMapKeys);
				COPY_VALUE(notetrackRumbleMapValues);
				COPY_VALUE(notetrackFXMapKeys);
				COPY_ASSET_ARR(notetrackFXMapValues, 16);
				COPY_VALUE(notetrackFXMapTagValues);
				COPY_VALUE(notetrackHideTagKeys);
				COPY_VALUE(notetrackHideTagValues);
				COPY_VALUE(notetrackHideTagTagValues);
				COPY_VALUE(szAdsrBaseSetting);
				COPY_ASSET(viewFlashEffect);
				COPY_ASSET(viewBodyFlashEffect);
				COPY_ASSET(worldFlashEffect);
				COPY_ASSET(viewFlashADSEffect);
				COPY_ASSET(viewBodyFlashADSEffect);
				COPY_ASSET(signatureViewFlashEffect);
				COPY_ASSET(signatureViewBodyFlashEffect);
				COPY_ASSET(signatureWorldFlashEffect);
				COPY_ASSET(signatureViewFlashADSEffect);
				COPY_ASSET(signatureViewBodyFlashADSEffect);
				COPY_ASSET(meleeHitEffect);
				COPY_ASSET(meleeMissEffect);
				COPY_ASSET(pickupSound);
				COPY_ASSET(pickupSoundPlayer);
				COPY_ASSET(ammoPickupSound);
				COPY_ASSET(ammoPickupSoundPlayer);
				COPY_ASSET(projectileSound);
				COPY_ASSET(pullbackSound);
				COPY_ASSET(pullbackSoundPlayer);
				COPY_ASSET(pullbackSoundQuick);
				COPY_ASSET(pullbackSoundQuickPlayer);
				COPY_ASSET(fireSound);
				COPY_ASSET(fireSoundPlayer);
				//new_asset->fireSoundPlayerLeft = new_asset->fireSoundPlayer;
				//new_asset->fireSoundPlayerRight = new_asset->fireSoundPlayer;
				new_asset->sound14 = nullptr;
				new_asset->sound15 = nullptr;
				new_asset->sound16 = nullptr;
				new_asset->sound17 = nullptr;
				new_asset->sound18 = nullptr;
				COPY_ASSET(fireLoopSound);
				COPY_ASSET(fireLoopSoundPlayer);
				new_asset->sound21 = nullptr;
				new_asset->sound22 = nullptr;
				new_asset->sound23 = nullptr;
				new_asset->sound24 = nullptr;
				new_asset->sound25 = nullptr;
				new_asset->sound26 = nullptr;
				COPY_ASSET(fireStopSound);
				COPY_ASSET(fireStopSoundPlayer);
				new_asset->sound29 = nullptr;
				new_asset->sound30 = nullptr;
				new_asset->sound31 = nullptr;
				new_asset->sound32 = nullptr;
				new_asset->sound33 = nullptr;
				COPY_ASSET(fireFirstSound);
				COPY_ASSET(fireFirstSoundPlayer);
				//new_asset->fireSound2 = new_asset->fireSound;
				//new_asset->fireSoundPlayer2 = new_asset->fireSoundPlayer;
				//new_asset->fireSpecialSound = new_asset->fireSound;
				//new_asset->fireSpecialSoundPlayer = new_asset->fireSoundPlayer;
				COPY_ASSET(emptyFireSound);
				COPY_ASSET(emptyFireSoundPlayer);
				//new_asset->emptyFireSoundPlayerLeft = new_asset->emptyFireSoundPlayer;
				//new_asset->emptyFireSoundPlayerRight = new_asset->emptyFireSoundPlayer;
				//new_asset->emptyFireSoundReleasePlayer = new_asset->emptyFireSoundPlayer;
				//new_asset->emptyFireSoundReleasePlayerLeft = new_asset->emptyFireSoundPlayer;
				//new_asset->emptyFireSoundReleasePlayerRight = new_asset->emptyFireSoundPlayer;
				new_asset->sound47 = nullptr;
				COPY_ASSET(meleeSwipeSound);
				COPY_ASSET(meleeSwipeSoundPlayer);
				COPY_ASSET(meleeHitSound);
				COPY_ASSET(meleeHitSoundPlayer);
				COPY_ASSET(meleeMissSound);
				COPY_ASSET(meleeMissSoundPlayer);
				new_asset->sound54 = nullptr;
				new_asset->sound55 = nullptr;
				new_asset->sound56 = nullptr;
				new_asset->sound57 = nullptr;
				new_asset->sound58 = nullptr;
				new_asset->sound59 = nullptr;
				new_asset->sound60 = nullptr;
				new_asset->sound61 = nullptr;
				new_asset->sound62 = nullptr;
				new_asset->sound63 = nullptr;
				new_asset->sound64 = nullptr;
				new_asset->sound65 = nullptr;
				COPY_ASSET(nightVisionWearSound);
				COPY_ASSET(nightVisionWearSoundPlayer);
				COPY_ASSET(nightVisionRemoveSound);
				COPY_ASSET(nightVisionRemoveSoundPlayer);
				COPY_ASSET(raiseSound);
				COPY_ASSET(raiseSoundPlayer);
				//new_asset->raiseSoundPlayerLeft = new_asset->raiseSoundPlayer;
				//new_asset->raiseSoundPlayerRight = new_asset->raiseSoundPlayer;
				new_asset->sound74 = nullptr;
				//new_asset->quickRaiseSoundPlayer = new_asset->raiseSoundPlayer;
				//new_asset->quickRaiseSoundPlayerLeft = new_asset->raiseSoundPlayer;
				//new_asset->quickRaiseSoundPlayerRight = new_asset->raiseSoundPlayer;
				//new_asset->raiseSound2 = new_asset->raiseSound;
				new_asset->sound79 = nullptr;
				new_asset->sound80 = nullptr;
				new_asset->sound81 = nullptr;
				COPY_ASSET(putawaySound);
				COPY_ASSET(putawaySoundPlayer);
				//new_asset->putawaySoundPlayerLeft = new_asset->putawaySoundPlayer;
				//new_asset->putawaySoundPlayerRight = new_asset->putawaySoundPlayer;
				new_asset->sound86 = nullptr;
				new_asset->sound87 = nullptr;
				new_asset->adsEnterSoundPlayer = nullptr;
				new_asset->adsLeaveSoundPlayer = nullptr;
				COPY_ASSET(adsCrosshairEnemySound);
				COPY_ASSET_ARR(bounceSound, 53);
				COPY_ASSET_ARR(rollingSound, 53);
				COPY_ASSET(viewShellEjectEffect);
				COPY_ASSET(worldShellEjectEffect);
				COPY_ASSET(viewLastShotEjectEffect);
				COPY_ASSET(worldLastShotEjectEffect);
				COPY_ASSET(viewMagEjectEffect);
				COPY_ASSET(reticleCenter);
				COPY_ASSET(reticleSide);
				COPY_ASSET_ARR(worldModel, 2);
				COPY_ASSET(worldClipModel);
				COPY_ASSET(rocketModel);
				COPY_ASSET(knifeModel);
				COPY_ASSET(worldKnifeModel);
				COPY_ASSET(hudIcon);
				COPY_ASSET(pickupIcon);
				COPY_ASSET(minimapIconFriendly);
				COPY_ASSET(minimapIconEnemy);
				COPY_ASSET(minimapIconNeutral);
				COPY_ASSET(ammoCounterIcon);
				COPY_VALUE(szAmmoName);
				COPY_VALUE(szClipName);
				COPY_VALUE(szSharedAmmoCapName);
				COPY_ASSET(physCollmap);
				COPY_ASSET(physPreset);
				COPY_VALUE(szUseHintString);
				COPY_VALUE(dropHintString);
				COPY_VALUE(locationDamageMultipliers);
				COPY_VALUE(fireRumble);
				COPY_VALUE(fireMedRumble);
				COPY_VALUE(fireHighRumble);
				COPY_VALUE(meleeImpactRumble);
				COPY_ASSET(tracerType);
				COPY_ASSET(signatureTracerType);
				COPY_ASSET(laserType);
				COPY_ASSET(turretOverheatSound);
				COPY_ASSET(turretOverheatEffect);
				COPY_VALUE(turretBarrelSpinRumble);
				COPY_ASSET(turretBarrelSpinMaxSnd);
				COPY_ASSET(turretBarrelSpinUpSnd[0]);
				COPY_ASSET(turretBarrelSpinUpSnd[1]);
				COPY_ASSET(turretBarrelSpinUpSnd[2]);
				COPY_ASSET(turretBarrelSpinUpSnd[3]);
				COPY_ASSET(turretBarrelSpinDownSnd[0]);
				COPY_ASSET(turretBarrelSpinDownSnd[1]);
				COPY_ASSET(turretBarrelSpinDownSnd[2]);
				COPY_ASSET(turretBarrelSpinDownSnd[3]);
				COPY_ASSET(missileConeSoundAlias);
				COPY_ASSET(missileConeSoundAliasAtBase);
				COPY_ASSET(stowOffsetModel);
				COPY_ARR(turretHydraulicSettings);
				COPY_VALUE(altWeapon);
				COPY_VALUE(numAttachments);
				COPY_VALUE(numAnimOverrides);
				COPY_VALUE(numSoundOverrides);
				COPY_VALUE(numFXOverrides);
				COPY_VALUE(numReloadStateTimerOverrides);
				COPY_VALUE(numNotetrackOverrides);
				COPY_VALUE_CAST(playerAnimType);
				COPY_VALUE_CAST(weapType);
				COPY_VALUE_CAST(weapClass);
				COPY_VALUE_CAST(penetrateType);
				COPY_VALUE(penetrateDepth);
				COPY_VALUE_CAST(impactType);
				COPY_VALUE_CAST(inventoryType);
				COPY_VALUE_CAST(fireType);
				COPY_VALUE_CAST(fireBarrels);
				COPY_VALUE_CAST(adsFireMode);
				COPY_VALUE(burstFireCooldown);
				COPY_VALUE_CAST(greebleType);
				COPY_VALUE_CAST(autoReloadType);
				COPY_VALUE_CAST(autoHolsterType);
				COPY_VALUE_CAST(offhandClass);
				COPY_VALUE_CAST(stance);
				COPY_VALUE(reticleCenterSize);
				COPY_VALUE(reticleSideSize);
				COPY_VALUE(reticleMinOfs);
				COPY_VALUE_CAST(activeReticleType);
				COPY_VALUE(standMove[0]);
				COPY_VALUE(standMove[1]);
				COPY_VALUE(standMove[2]);
				COPY_VALUE(standRot[0]);
				COPY_VALUE(standRot[1]);
				COPY_VALUE(standRot[2]);
				COPY_VALUE(strafeMove[0]);
				COPY_VALUE(strafeMove[1]);
				COPY_VALUE(strafeMove[2]);
				COPY_VALUE(strafeRot[0]);
				COPY_VALUE(strafeRot[1]);
				COPY_VALUE(strafeRot[2]);
				COPY_VALUE(duckedOfs[0]);
				COPY_VALUE(duckedOfs[1]);
				COPY_VALUE(duckedOfs[2]);
				COPY_VALUE(duckedMove[0]);
				COPY_VALUE(duckedMove[1]);
				COPY_VALUE(duckedMove[2]);
				COPY_VALUE(duckedRot[0]);
				COPY_VALUE(duckedRot[1]);
				COPY_VALUE(duckedRot[2]);
				COPY_VALUE(proneOfs[0]);
				COPY_VALUE(proneOfs[1]);
				COPY_VALUE(proneOfs[2]);
				COPY_VALUE(proneMove[0]);
				COPY_VALUE(proneMove[1]);
				COPY_VALUE(proneMove[2]);
				COPY_VALUE(proneRot[0]);
				COPY_VALUE(proneRot[1]);
				COPY_VALUE(proneRot[2]);
				new_asset->unkVec1[0] = 0.f;
				new_asset->unkVec1[1] = 0.f;
				new_asset->unkVec1[2] = 0.f;
				new_asset->unkVec2[0] = 0.f;
				new_asset->unkVec2[1] = 0.f;
				new_asset->unkVec2[2] = 0.f;
				COPY_VALUE(posMoveRate);
				COPY_VALUE(posProneMoveRate);
				COPY_VALUE(standMoveMinSpeed);
				COPY_VALUE(duckedMoveMinSpeed);
				COPY_VALUE(proneMoveMinSpeed);
				COPY_VALUE(posRotRate);
				COPY_VALUE(posProneRotRate);
				COPY_VALUE_CAST(hudIconRatio);
				COPY_VALUE_CAST(pickupIconRatio);
				COPY_VALUE_CAST(ammoCounterIconRatio);
				COPY_VALUE(ammoCounterClip);
				COPY_VALUE(startAmmo);
				//COPY_VALUE(iAmmoIndex);
				//COPY_VALUE(iClipIndex);
				COPY_VALUE(maxAmmo);
				COPY_VALUE(minAmmoReq);
				COPY_VALUE(clipSize);
				COPY_VALUE(shotCount);
				COPY_VALUE(sharedAmmoCapIndex);
				COPY_VALUE(sharedAmmoCap);
				COPY_VALUE(damage);
				COPY_VALUE(playerDamage);
				COPY_VALUE(meleeDamage);
				COPY_VALUE(damageType);
				COPY_VALUE(stateTimers.fireDelay);
				COPY_VALUE(stateTimers.meleeDelay);
				COPY_VALUE(stateTimers.meleeChargeDelay);
				COPY_VALUE(stateTimers.detonateDelay);
				COPY_VALUE(stateTimers.fireTime);
				COPY_VALUE(stateTimers.rechamberTime);
				COPY_VALUE(stateTimers.rechamberTimeOneHanded);
				COPY_VALUE(stateTimers.rechamberBoltTime);
				COPY_VALUE(stateTimers.holdFireTime);
				COPY_VALUE(stateTimers.grenadePrimeReadyToThrowTime);
				COPY_VALUE(stateTimers.detonateTime);
				COPY_VALUE(stateTimers.meleeTime);
				COPY_VALUE(stateTimers.meleeChargeTime);
				COPY_VALUE(stateTimers.reloadTime);
				COPY_VALUE(stateTimers.reloadShowRocketTime);
				COPY_VALUE(stateTimers.reloadEmptyTime);
				COPY_VALUE(stateTimers.reloadAddTime);
				COPY_VALUE(stateTimers.reloadEmptyAddTime);
				COPY_VALUE(stateTimers.reloadStartTime);
				COPY_VALUE(stateTimers.reloadStartAddTime);
				COPY_VALUE(stateTimers.reloadEndTime);
				COPY_VALUE(stateTimers.reloadTimeDualWield);
				COPY_VALUE(stateTimers.reloadAddTimeDualWield);
				COPY_VALUE(stateTimers.reloadEmptyDualMag);
				COPY_VALUE(stateTimers.reloadEmptyAddTimeDualMag);
				COPY_VALUE(stateTimers.speedReloadTime);
				COPY_VALUE(stateTimers.speedReloadAddTime);
				COPY_VALUE(stateTimers.dropTime);
				COPY_VALUE(stateTimers.raiseTime);
				COPY_VALUE(stateTimers.altDropTime);
				COPY_VALUE(stateTimers.altRaiseTime);
				COPY_VALUE(stateTimers.quickDropTime);
				COPY_VALUE(stateTimers.quickRaiseTime);
				COPY_VALUE(stateTimers.firstRaiseTime);
				COPY_VALUE(stateTimers.breachRaiseTime);
				COPY_VALUE(stateTimers.emptyRaiseTime);
				COPY_VALUE(stateTimers.emptyDropTime);
				COPY_VALUE(stateTimers.sprintInTime);
				COPY_VALUE(stateTimers.sprintLoopTime);
				COPY_VALUE(stateTimers.sprintOutTime);
				COPY_VALUE(stateTimers.stunnedTimeBegin);
				COPY_VALUE(stateTimers.stunnedTimeLoop);
				COPY_VALUE(stateTimers.stunnedTimeEnd);
				COPY_VALUE(stateTimers.nightVisionWearTime);
				COPY_VALUE(stateTimers.nightVisionWearTimeFadeOutEnd);
				COPY_VALUE(stateTimers.nightVisionWearTimePowerUp);
				COPY_VALUE(stateTimers.nightVisionRemoveTime);
				COPY_VALUE(stateTimers.nightVisionRemoveTimePowerDown);
				COPY_VALUE(stateTimers.nightVisionRemoveTimeFadeInStart);
				COPY_VALUE(stateTimers.aiFuseTime);
				COPY_VALUE(stateTimers.fuseTime);
				COPY_VALUE(stateTimers.missileTime);
				COPY_VALUE(stateTimers.primeTime);
				COPY_VALUE(stateTimers.bHoldFullPrime);
				COPY_VALUE(stateTimers.blastFrontTime);
				COPY_VALUE(stateTimers.blastRightTime);
				COPY_VALUE(stateTimers.blastBackTime);
				COPY_VALUE(stateTimers.blastLeftTime);
				COPY_VALUE(stateTimers.slideInTime);
				COPY_VALUE(stateTimers.slideLoopTime);
				COPY_VALUE(stateTimers.slideOutTime);
				COPY_VALUE(stateTimers.highJumpInTime);
				COPY_VALUE(stateTimers.highJumpDropInTime);
				COPY_VALUE(stateTimers.highJumpDropLoopTime);
				COPY_VALUE(stateTimers.highJumpDropLandTime);
				COPY_VALUE(stateTimers.dodgeTime);
				COPY_VALUE(stateTimers.landDipTime);
				COPY_VALUE(stateTimers.hybridSightInTime);
				COPY_VALUE(stateTimers.hybridSightOutTime);
				COPY_VALUE(stateTimers.offhandSwitchTime);
				COPY_VALUE(stateTimers.heatCooldownInTime);
				COPY_VALUE(stateTimers.heatCooldownOutTime);
				COPY_VALUE(stateTimers.heatCooldownOutReadyTime);
				COPY_VALUE(stateTimers.overheatOutTime);
				COPY_VALUE(stateTimers.overheatOutReadyTime);
				//COPY_VALUE(stateTimers.u75);
				//COPY_VALUE(stateTimers.u76);
				//COPY_VALUE(stateTimers.u77);
				//COPY_VALUE(stateTimers.u78);
				COPY_VALUE(akimboStateTimers.fireDelay);
				COPY_VALUE(akimboStateTimers.meleeDelay);
				COPY_VALUE(akimboStateTimers.meleeChargeDelay);
				COPY_VALUE(akimboStateTimers.detonateDelay);
				COPY_VALUE(akimboStateTimers.fireTime);
				COPY_VALUE(akimboStateTimers.rechamberTime);
				COPY_VALUE(akimboStateTimers.rechamberTimeOneHanded);
				COPY_VALUE(akimboStateTimers.rechamberBoltTime);
				COPY_VALUE(akimboStateTimers.holdFireTime);
				COPY_VALUE(akimboStateTimers.grenadePrimeReadyToThrowTime);
				COPY_VALUE(akimboStateTimers.detonateTime);
				COPY_VALUE(akimboStateTimers.meleeTime);
				COPY_VALUE(akimboStateTimers.meleeChargeTime);
				COPY_VALUE(akimboStateTimers.reloadTime);
				COPY_VALUE(akimboStateTimers.reloadShowRocketTime);
				COPY_VALUE(akimboStateTimers.reloadEmptyTime);
				COPY_VALUE(akimboStateTimers.reloadAddTime);
				COPY_VALUE(akimboStateTimers.reloadEmptyAddTime);
				COPY_VALUE(akimboStateTimers.reloadStartTime);
				COPY_VALUE(akimboStateTimers.reloadStartAddTime);
				COPY_VALUE(akimboStateTimers.reloadEndTime);
				COPY_VALUE(akimboStateTimers.reloadTimeDualWield);
				COPY_VALUE(akimboStateTimers.reloadAddTimeDualWield);
				COPY_VALUE(akimboStateTimers.reloadEmptyDualMag);
				COPY_VALUE(akimboStateTimers.reloadEmptyAddTimeDualMag);
				COPY_VALUE(akimboStateTimers.speedReloadTime);
				COPY_VALUE(akimboStateTimers.speedReloadAddTime);
				COPY_VALUE(akimboStateTimers.dropTime);
				COPY_VALUE(akimboStateTimers.raiseTime);
				COPY_VALUE(akimboStateTimers.altDropTime);
				COPY_VALUE(akimboStateTimers.altRaiseTime);
				COPY_VALUE(akimboStateTimers.quickDropTime);
				COPY_VALUE(akimboStateTimers.quickRaiseTime);
				COPY_VALUE(akimboStateTimers.firstRaiseTime);
				COPY_VALUE(akimboStateTimers.breachRaiseTime);
				COPY_VALUE(akimboStateTimers.emptyRaiseTime);
				COPY_VALUE(akimboStateTimers.emptyDropTime);
				COPY_VALUE(akimboStateTimers.sprintInTime);
				COPY_VALUE(akimboStateTimers.sprintLoopTime);
				COPY_VALUE(akimboStateTimers.sprintOutTime);
				COPY_VALUE(akimboStateTimers.stunnedTimeBegin);
				COPY_VALUE(akimboStateTimers.stunnedTimeLoop);
				COPY_VALUE(akimboStateTimers.stunnedTimeEnd);
				COPY_VALUE(akimboStateTimers.nightVisionWearTime);
				COPY_VALUE(akimboStateTimers.nightVisionWearTimeFadeOutEnd);
				COPY_VALUE(akimboStateTimers.nightVisionWearTimePowerUp);
				COPY_VALUE(akimboStateTimers.nightVisionRemoveTime);
				COPY_VALUE(akimboStateTimers.nightVisionRemoveTimePowerDown);
				COPY_VALUE(akimboStateTimers.nightVisionRemoveTimeFadeInStart);
				COPY_VALUE(akimboStateTimers.aiFuseTime);
				COPY_VALUE(akimboStateTimers.fuseTime);
				COPY_VALUE(akimboStateTimers.missileTime);
				COPY_VALUE(akimboStateTimers.primeTime);
				COPY_VALUE(akimboStateTimers.bHoldFullPrime);
				COPY_VALUE(akimboStateTimers.blastFrontTime);
				COPY_VALUE(akimboStateTimers.blastRightTime);
				COPY_VALUE(akimboStateTimers.blastBackTime);
				COPY_VALUE(akimboStateTimers.blastLeftTime);
				COPY_VALUE(akimboStateTimers.slideInTime);
				COPY_VALUE(akimboStateTimers.slideLoopTime);
				COPY_VALUE(akimboStateTimers.slideOutTime);
				COPY_VALUE(akimboStateTimers.highJumpInTime);
				COPY_VALUE(akimboStateTimers.highJumpDropInTime);
				COPY_VALUE(akimboStateTimers.highJumpDropLoopTime);
				COPY_VALUE(akimboStateTimers.highJumpDropLandTime);
				COPY_VALUE(akimboStateTimers.dodgeTime);
				COPY_VALUE(akimboStateTimers.landDipTime);
				COPY_VALUE(akimboStateTimers.hybridSightInTime);
				COPY_VALUE(akimboStateTimers.hybridSightOutTime);
				COPY_VALUE(akimboStateTimers.offhandSwitchTime);
				COPY_VALUE(akimboStateTimers.heatCooldownInTime);
				COPY_VALUE(akimboStateTimers.heatCooldownOutTime);
				COPY_VALUE(akimboStateTimers.heatCooldownOutReadyTime);
				COPY_VALUE(akimboStateTimers.overheatOutTime);
				COPY_VALUE(akimboStateTimers.overheatOutReadyTime);
				//COPY_VALUE(akimboStateTimers.u75);
				//COPY_VALUE(akimboStateTimers.u76);
				//COPY_VALUE(akimboStateTimers.u77);
				//COPY_VALUE(akimboStateTimers.u78);
				COPY_VALUE(autoAimRange);
				COPY_VALUE(aimAssistRange);
				COPY_VALUE(aimAssistRangeAds);
				COPY_VALUE(aimPadding);
				COPY_VALUE(enemyCrosshairRange);
				COPY_VALUE(moveSpeedScale);
				COPY_VALUE(adsMoveSpeedScale);
				COPY_VALUE(sprintDurationScale);
				COPY_VALUE(adsZoomFov);
				COPY_VALUE(adsZoomInFrac);
				COPY_VALUE(adsZoomOutFrac);
				COPY_VALUE(adsSceneBlurStrength);
				COPY_VALUE(adsSceneBlurPhysicalScale);
				COPY_VALUE(pad3);
				COPY_ASSET(overlay.shader);
				COPY_ASSET(overlay.shaderLowRes);
				COPY_ASSET(overlay.shaderEMP);
				COPY_ASSET(overlay.shaderEMPLowRes);
				COPY_VALUE_CAST(overlay.reticle);
				new_asset->overlay.xU_01 = 0.f;
				new_asset->overlay.xU_02 = 0.f;
				COPY_VALUE(overlay.width);
				COPY_VALUE(overlay.height);
				COPY_VALUE(overlay.widthSplitscreen);
				COPY_VALUE(overlay.heightSplitscreen);
				COPY_VALUE_CAST(overlayInterface);
				COPY_VALUE(adsBobFactor);
				COPY_VALUE(adsViewBobMult);
				COPY_VALUE(hipSpreadStandMin);
				COPY_VALUE(hipSpreadDuckedMin);
				COPY_VALUE(hipSpreadProneMin);
				COPY_VALUE(hipSpreadStandMax);
				COPY_VALUE(hipSpreadSprintMax);
				COPY_VALUE(hipSpreadSlideMax);
				COPY_VALUE(hipSpreadDuckedMax);
				COPY_VALUE(hipSpreadProneMax);
				COPY_VALUE(hipSpreadDecayRate);
				COPY_VALUE(hipSpreadFireAdd);
				COPY_VALUE(hipSpreadTurnAdd);
				COPY_VALUE(hipSpreadMoveAdd);
				COPY_VALUE(hipSpreadDuckedDecay);
				COPY_VALUE(hipSpreadProneDecay);
				COPY_VALUE(hipReticleSidePos);
				COPY_VALUE(adsIdleAmount);
				COPY_VALUE(hipIdleAmount);
				COPY_VALUE(adsIdleSpeed);
				COPY_VALUE(hipIdleSpeed);
				COPY_VALUE(idleCrouchFactor);
				COPY_VALUE(idleProneFactor);
				COPY_VALUE(gunMaxPitch);
				COPY_VALUE(gunMaxYaw);
				COPY_VALUE(adsIdleLerpStartTime);
				COPY_VALUE(adsIdleLerpTime);
				COPY_VALUE(adsTransInTime);
				COPY_VALUE(adsTransInFromSprintTime);
				COPY_VALUE(adsTransOutTime);
				COPY_VALUE(swayMaxAngleSteadyAim);
				COPY_VALUE(swayMaxAngle);
				COPY_VALUE(swayLerpSpeed);
				COPY_VALUE(swayPitchScale);
				COPY_VALUE(swayYawScale);
				COPY_VALUE(swayVertScale);
				COPY_VALUE(swayHorizScale);
				COPY_VALUE(swayShellShockScale);
				COPY_VALUE(adsSwayMaxAngle);
				COPY_VALUE(adsSwayLerpSpeed);
				COPY_VALUE(adsSwayPitchScale);
				COPY_VALUE(adsSwayYawScale);
				COPY_VALUE(adsSwayHorizScale);
				COPY_VALUE(adsSwayVertScale);
				COPY_VALUE(adsViewErrorMin);
				COPY_VALUE(adsViewErrorMax);
				COPY_VALUE(adsFireAnimFrac);
				COPY_VALUE(dualWieldViewModelOffset);
				COPY_VALUE(scopeDriftDelay);
				COPY_VALUE(scopeDriftLerpInTime);
				COPY_VALUE(scopeDriftSteadyTime);
				COPY_VALUE(scopeDriftLerpOutTime);
				COPY_VALUE(scopeDriftSteadyFactor);
				COPY_VALUE(scopeDriftUnsteadyFactor);
				COPY_VALUE(bobVerticalFactor);
				COPY_VALUE(bobHorizontalFactor);
				COPY_VALUE(bobViewVerticalFactor);
				COPY_VALUE(bobViewHorizontalFactor);
				COPY_VALUE(stationaryZoomFov);
				COPY_VALUE(stationaryZoomDelay);
				COPY_VALUE(stationaryZoomLerpInTime);
				COPY_VALUE(stationaryZoomLerpOutTime);
				COPY_VALUE(adsDofStart);
				COPY_VALUE(adsDofEnd);
				new_asset->xU_020 = 0.f;
				COPY_VALUE_CAST(killIcon);
				COPY_VALUE_CAST(dpadIcon);
				COPY_VALUE_CAST(hudProximityWarningIcon);
				COPY_VALUE_CAST(killIconRatio);
				COPY_VALUE_CAST(dpadIconRatio);
				COPY_VALUE(fireAnimLength);
				COPY_VALUE(fireAnimLengthAkimbo);
				COPY_VALUE(inspectAnimTime);
				COPY_VALUE(reloadAmmoAdd);
				COPY_VALUE(reloadStartAdd);
				COPY_VALUE(ammoDropStockMin);
				COPY_VALUE(ammoDropStockMax);
				COPY_VALUE(ammoDropClipPercentMin);
				COPY_VALUE(ammoDropClipPercentMax);
				COPY_VALUE(explosionRadius);
				COPY_VALUE(explosionRadiusMin);
				COPY_VALUE(explosionInnerDamage);
				COPY_VALUE(explosionOuterDamage);
				COPY_VALUE(damageConeAngle);
				COPY_VALUE(bulletExplDmgMult);
				COPY_VALUE(bulletExplRadiusMult);
				COPY_VALUE(projectileSpeed);
				COPY_VALUE(projectileSpeedUp);
				COPY_VALUE(projectileSpeedForward);
				COPY_VALUE(projectileActivateDist);
				COPY_VALUE(projLifetime);
				COPY_VALUE(timeToAccelerate);
				COPY_VALUE(projectileCurvature);
				new_asset->xU_021 = 0.f;
				COPY_VALUE(projectileName);
				COPY_ASSET(projectileModel);
				COPY_ASSET(projExplosionEffect);
				COPY_ASSET(projDudEffect);
				COPY_ASSET(projExplosionSound);
				COPY_ASSET(projDudSound);
				COPY_VALUE_CAST(projExplosion);
				COPY_VALUE_CAST(stickiness);
				COPY_VALUE(lowAmmoWarningThreshold);
				COPY_VALUE(ricochetChance);
				COPY_VALUE(riotShieldHealth);
				COPY_VALUE(riotShieldDamageMult);
				COPY_VALUE(parallelBounce);
				COPY_VALUE(perpendicularBounce);
				COPY_ASSET(projTrailEffect);
				COPY_ASSET(projBeaconEffect);
				COPY_VALUE(projectileColor[0]);
				COPY_VALUE(projectileColor[1]);
				COPY_VALUE(projectileColor[2]);
				COPY_VALUE_CAST(guidedMissileType);
				COPY_VALUE(maxSteeringAccel);
				COPY_VALUE(projIgnitionDelay);
				COPY_ASSET(projIgnitionEffect);
				COPY_ASSET(projIgnitionSound);
				COPY_VALUE(adsAimPitch);
				COPY_VALUE(adsCrosshairInFrac);
				COPY_VALUE(adsCrosshairOutFrac);
				COPY_VALUE(adsGunKickReducedKickBullets);
				COPY_VALUE(adsGunKickReducedKickPercent);
				COPY_VALUE(adsGunKickPitchMin);
				COPY_VALUE(adsGunKickPitchMax);
				COPY_VALUE(adsGunKickYawMin);
				COPY_VALUE(adsGunKickYawMax);
				COPY_VALUE(adsGunKickMagMin);
				COPY_VALUE(adsGunKickAccel);
				COPY_VALUE(adsGunKickSpeedMax);
				COPY_VALUE(adsGunKickSpeedDecay);
				COPY_VALUE(adsGunKickStaticDecay);
				COPY_VALUE(adsViewKickPitchMin);
				COPY_VALUE(adsViewKickPitchMax);
				COPY_VALUE(adsViewKickYawMin);
				COPY_VALUE(adsViewKickYawMax);
				COPY_VALUE(adsViewKickMagMin);
				COPY_VALUE(adsViewKickCenterSpeed);
				COPY_VALUE(adsViewScatterMin);
				COPY_VALUE(adsViewScatterMax);
				COPY_VALUE(adsSpread);
				COPY_VALUE(hipGunKickReducedKickBullets);
				COPY_VALUE(hipGunKickReducedKickPercent);
				COPY_VALUE(hipGunKickPitchMin);
				COPY_VALUE(hipGunKickPitchMax);
				COPY_VALUE(hipGunKickYawMin);
				COPY_VALUE(hipGunKickYawMax);
				COPY_VALUE(hipGunKickMagMin);
				COPY_VALUE(hipGunKickAccel);
				COPY_VALUE(hipGunKickSpeedMax);
				COPY_VALUE(hipGunKickSpeedDecay);
				COPY_VALUE(hipGunKickStaticDecay);
				COPY_VALUE(hipViewKickPitchMin);
				COPY_VALUE(hipViewKickPitchMax);
				COPY_VALUE(hipViewKickYawMin);
				COPY_VALUE(hipViewKickYawMax);
				COPY_VALUE(hipViewKickMagMin);
				COPY_VALUE(hipViewKickCenterSpeed);
				COPY_VALUE(hipViewScatterMin);
				COPY_VALUE(hipViewScatterMax);
				COPY_VALUE(viewKickScale);
				COPY_VALUE(positionReloadTransTime);
				COPY_VALUE(fightDist);
				COPY_VALUE(maxDist);
				COPY_VALUE(accuracyGraphName[0]);
				COPY_VALUE(accuracyGraphName[1]);
				COPY_VALUE(accuracyGraphKnots[0]);
				COPY_VALUE(accuracyGraphKnots[1]);
				COPY_VALUE(originalAccuracyGraphKnots[0]);
				COPY_VALUE(originalAccuracyGraphKnots[1]);
				COPY_VALUE(accuracyGraphKnotCount[0]);
				COPY_VALUE(accuracyGraphKnotCount[1]);
				COPY_VALUE(originalAccuracyGraphKnotCount[0]);
				COPY_VALUE(originalAccuracyGraphKnotCount[1]);
				COPY_VALUE(leftArc);
				COPY_VALUE(rightArc);
				COPY_VALUE(topArc);
				COPY_VALUE(bottomArc);
				//COPY_VALUE(__pad[0]);
				//COPY_VALUE(__pad[1]);
				//COPY_VALUE(__pad[2]);
				//COPY_VALUE(__pad[3]);
				//COPY_VALUE(__pad[4]);
				//COPY_VALUE(__pad[5]);
				//COPY_VALUE(__pad[6]);
				//COPY_VALUE(__pad[7]);
				//COPY_VALUE(__pad[8]);
				//COPY_VALUE(__pad[9]);
				//COPY_VALUE(__pad[10]);
				//COPY_VALUE(__pad[11]);
				//COPY_VALUE(__pad[12]);
				//COPY_VALUE(__pad[13]);
				//COPY_VALUE(__pad[14]);
				//COPY_VALUE(__pad[15]);
				COPY_VALUE(accuracy);
				COPY_VALUE(aiSpread);
				COPY_VALUE(playerSpread);
				COPY_VALUE(minTurnSpeed[0]);
				COPY_VALUE(minTurnSpeed[1]);
				COPY_VALUE(maxTurnSpeed[0]);
				COPY_VALUE(maxTurnSpeed[1]);
				COPY_VALUE(pitchConvergenceTime);
				COPY_VALUE(yawConvergenceTime);
				COPY_VALUE(suppressTime);
				COPY_VALUE(maxRange);
				COPY_VALUE(animHorRotateInc);
				COPY_VALUE(playerPositionDist);
				COPY_VALUE(useHintStringIndex);
				COPY_VALUE(dropHintStringIndex);
				COPY_VALUE(horizViewJitter);
				COPY_VALUE(vertViewJitter);
				COPY_VALUE(scanSpeed);
				COPY_VALUE(scanAccel);
				COPY_VALUE(scanPauseTime);
				COPY_VALUE(szScript);
				COPY_VALUE(minDamage);
				COPY_VALUE(midDamage);
				COPY_VALUE(minPlayerDamage);
				COPY_VALUE(midPlayerDamage);
				COPY_VALUE(maxDamageRange);
				COPY_VALUE(minDamageRange);
				COPY_VALUE(signatureAmmoInClip);
				COPY_VALUE(signatureDamage);
				COPY_VALUE(signatureMidDamage);
				COPY_VALUE(signatureMinDamage);
				COPY_VALUE(signatureMaxDamageRange);
				COPY_VALUE(signatureMinDamageRange);
				COPY_VALUE(destabilizationRateTime);
				COPY_VALUE(destabilizationCurvatureMax);
				COPY_VALUE(destabilizeDistance);
				COPY_VALUE(turretADSTime);
				COPY_VALUE(turretFov);
				COPY_VALUE(turretFovADS);
				COPY_VALUE(turretScopeZoomRate);
				COPY_VALUE(turretScopeZoomMin);
				COPY_VALUE(turretScopeZoomMax);
				COPY_VALUE(overheatUpRate);
				COPY_VALUE(overheatDownRate);
				COPY_VALUE(overheatCooldownRate);
				COPY_VALUE(overheatPenalty);
				COPY_VALUE(turretBarrelSpinSpeed);
				COPY_VALUE(turretBarrelSpinUpTime);
				COPY_VALUE(turretBarrelSpinDownTime);
				COPY_VALUE(missileConeSoundRadiusAtTop);
				COPY_VALUE(missileConeSoundRadiusAtBase);
				COPY_VALUE(missileConeSoundHeight);
				COPY_VALUE(missileConeSoundOriginOffset);
				COPY_VALUE(missileConeSoundVolumescaleAtCore);
				COPY_VALUE(missileConeSoundVolumescaleAtEdge);
				COPY_VALUE(missileConeSoundVolumescaleCoreSize);
				COPY_VALUE(missileConeSoundPitchAtTop);
				COPY_VALUE(missileConeSoundPitchAtBottom);
				COPY_VALUE(missileConeSoundPitchTopSize);
				COPY_VALUE(missileConeSoundPitchBottomSize);
				COPY_VALUE(missileConeSoundCrossfadeTopSize);
				COPY_VALUE(missileConeSoundCrossfadeBottomSize);
				COPY_VALUE(aim_automelee_lerp);
				COPY_VALUE(aim_automelee_range);
				COPY_VALUE(aim_automelee_region_height);
				COPY_VALUE(aim_automelee_region_width);
				COPY_VALUE(player_meleeHeight);
				COPY_VALUE(player_meleeRange);
				COPY_VALUE(player_meleeWidth);
				COPY_VALUE(changedFireTime);
				COPY_VALUE(changedFireTimeNumBullets);
				COPY_VALUE_CAST(fireTimeInterpolationType);
				COPY_VALUE(generateAmmo);
				COPY_VALUE(ammoPerShot);
				COPY_VALUE(explodeCount);
				COPY_VALUE(batteryDischargeRate);
				COPY_VALUE(extendedBattery);
				COPY_VALUE(bulletsPerTag);
				COPY_VALUE(maxTags);
				COPY_VALUE(stowTag);
				COPY_VALUE(rattleSoundType);
				COPY_VALUE(adsShouldShowCrosshair);
				COPY_VALUE(adsCrosshairShouldScale);
				COPY_VALUE(turretADSEnabled);
				COPY_VALUE(knifeAttachTagLeft);
				COPY_VALUE(knifeAlwaysAttached);
				COPY_VALUE(meleeOverrideValues);
				COPY_VALUE(riotShieldEnableDamage);
				COPY_VALUE(allowPrimaryWeaponPickup);
				COPY_VALUE(sharedAmmo);
				COPY_VALUE(lockonSupported);
				COPY_VALUE(requireLockonToFire);
				COPY_VALUE(isAirburstWeapon);
				COPY_VALUE(bigExplosion);
				COPY_VALUE(noAdsWhenMagEmpty);
				COPY_VALUE(avoidDropCleanup);
				COPY_VALUE(inheritsPerks);
				COPY_VALUE(crosshairColorChange);
				COPY_VALUE(rifleBullet);
				COPY_VALUE(armorPiercing);
				COPY_VALUE(boltAction);
				COPY_VALUE(aimDownSight);
				COPY_VALUE(canHoldBreath);
				COPY_VALUE(meleeOnly);
				COPY_VALUE(quickMelee);
				COPY_VALUE(bU_086);
				COPY_VALUE(canVariableZoom);
				COPY_VALUE(rechamberWhileAds);
				COPY_VALUE(bulletExplosiveDamage);
				COPY_VALUE(cookOffHold);
				COPY_VALUE(useBattery);
				COPY_VALUE(reticleSpin45);
				COPY_VALUE(clipOnly);
				COPY_VALUE(noAmmoPickup);
				COPY_VALUE(disableSwitchToWhenEmpty);
				COPY_VALUE(suppressAmmoReserveDisplay);
				COPY_VALUE(motionTracker);
				COPY_VALUE(markableViewmodel);
				COPY_VALUE(noDualWield);
				COPY_VALUE(flipKillIcon);
				COPY_VALUE(actionSlotShowAmmo);
				COPY_VALUE(noPartialReload);
				COPY_VALUE(segmentedReload);
				COPY_VALUE(multipleReload);
				COPY_VALUE(blocksProne);
				COPY_VALUE(silenced);
				COPY_VALUE(isRollingGrenade);
				COPY_VALUE(projExplosionEffectForceNormalUp);
				COPY_VALUE(projExplosionEffectInheritParentDirection);
				COPY_VALUE(projImpactExplode);
				COPY_VALUE(projTrajectoryEvents);
				COPY_VALUE(projWhizByEnabled);
				COPY_VALUE(stickToPlayers);
				COPY_VALUE(stickToVehicles);
				COPY_VALUE(stickToTurrets);
				COPY_VALUE(thrownSideways);
				COPY_VALUE(hasDetonatorEmptyThrow);
				COPY_VALUE(hasDetonatorDoubleTap);
				COPY_VALUE(disableFiring);
				COPY_VALUE(timedDetonation);
				COPY_VALUE(noCrumpleMissile);
				COPY_VALUE(fuseLitAfterImpact);
				COPY_VALUE(rotate);
				COPY_VALUE(holdButtonToThrow);
				COPY_VALUE(freezeMovementWhenFiring);
				COPY_VALUE(thermalScope);
				COPY_VALUE(thermalToggle);
				COPY_VALUE(outlineEnemies);
				COPY_VALUE(altModeSameWeapon);
				COPY_VALUE(turretBarrelSpinEnabled);
				COPY_VALUE(missileConeSoundEnabled);
				COPY_VALUE(missileConeSoundPitchshiftEnabled);
				COPY_VALUE(missileConeSoundCrossfadeEnabled);
				COPY_VALUE(offhandHoldIsCancelable);
				COPY_VALUE(doNotAllowAttachmentsToOverrideSpread);
				COPY_VALUE(useFastReloadAnims);
				COPY_VALUE(dualMagReloadSupported);
				COPY_VALUE(reloadStopsAlt);
				COPY_VALUE(useScopeDrift);
				COPY_VALUE(alwaysShatterGlassOnImpact);
				new_asset->bU_093 = false;
				COPY_VALUE(oldWeapon);
				COPY_VALUE(raiseToHold);
				COPY_VALUE(notifyOnPlayerImpact);
				COPY_VALUE(decreasingKick);
				COPY_VALUE(counterSilencer);
				COPY_VALUE(projSuppressedByEMP);
				COPY_VALUE(projDisabledByEMP);
				COPY_VALUE(autosimDisableVariableRate);
				COPY_VALUE(projPlayTrailEffectForOwnerOnly);
				COPY_VALUE(projPlayBeaconEffectForOwnerOnly);
				COPY_VALUE(projKillTrailEffectOnDeath);
				COPY_VALUE(projKillBeaconEffectOnDeath);
				COPY_VALUE(reticleDetonateHide);
				COPY_VALUE(cloaked);
				COPY_VALUE(adsHideWeapon);
				COPY_VALUE(adsHideHands);
				COPY_VALUE(bU_108);
				COPY_VALUE(adsSceneBlur);
				COPY_VALUE(usesSniperScope);
				COPY_VALUE(hasTransientModels);
				COPY_VALUE(signatureAmmoAlternate);
				COPY_VALUE(useScriptCallbackForHit);
				COPY_VALUE(useBulletTagSystem);
				COPY_VALUE(hideBulletTags);
				COPY_VALUE(adsDofPhysicalFstop);
				COPY_VALUE(adsDofPhysicalFocusDistance);
				COPY_VALUE(autosimSpeedScale);
				COPY_VALUE(reactiveMotionRadiusScale);
				COPY_VALUE(reactiveMotionFrequencyScale);
				COPY_VALUE(reactiveMotionAmplitudeScale);
				COPY_VALUE(reactiveMotionFalloff);
				COPY_VALUE(reactiveMotionLifetime);
				COPY_VALUE(fU_3604[0]);
				COPY_VALUE(fU_3604[1]);
				COPY_VALUE(fU_3604[2]);

				return new_asset;
			}

			void dump(zonetool::h1::WeaponDef* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h2::weapon_def::dump(converted_asset);
			}
		}
	}
}
