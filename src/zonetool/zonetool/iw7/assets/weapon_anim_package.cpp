#include "std_include.hpp"
#include "weapon_anim_package.hpp"

namespace zonetool::iw7
{
	const char* weapon_anim_package::anim_names[NUM_WEAP_ANIMS] =
	{
		"WEAP_ANIM_ROOT",
		"WEAP_ANIM_INNER_ROOT",
		"WEAP_ANIM_GESTURE_IK_TARGET_1",
		"WEAP_ANIM_GESTURE_IK_TARGET_2",
		"WEAP_ANIM_GESTURE_IK_TARGET_3",
		"WEAP_ANIM_GESTURE_IK_TARGET_4",
		"WEAP_ANIM_IK_NODE",
		"WEAP_ANIM_PROC_NODE",
		"WEAP_ANIM_IK_NODE_VOID",
		"WEAP_ANIM_PROC_NODE_VOID",
		"WEAP_ANIM_RELATIVE_ROOT",
		"WEAP_ANIM_ADDITIVE_ADS_ROOT",
		"WEAP_ANIM_ADDITIVE_ADS_UP",
		"WEAP_ANIM_ADDITIVE_DRAG_LEFT_ROOT",
		"WEAP_ANIM_ADDITIVE_DRAG_LEFT",
		"WEAP_ANIM_ADDITIVE_DRAG_RIGHT_ROOT",
		"WEAP_ANIM_ADDITIVE_DRAG_RIGHT",
		"WEAP_ANIM_ADDITIVE_DRAG_UP_ROOT",
		"WEAP_ANIM_ADDITIVE_DRAG_UP",
		"WEAP_ANIM_ADDITIVE_DRAG_DOWN_ROOT",
		"WEAP_ANIM_ADDITIVE_DRAG_DOWN",
		"WEAP_ANIM_ADDITIVE_SWIM_FORWARD_ROOT",
		"WEAP_ANIM_ADDITIVE_SWIM_FORWARD",
		"WEAP_ANIM_ADDITIVE_SWIM_BACKWARD_ROOT",
		"WEAP_ANIM_ADDITIVE_SWIM_BACKWARD",
		"WEAP_ANIM_ADDITIVE_JUMP_ROOT",
		"WEAP_ANIM_ADDITIVE_JUMP",
		"WEAP_ANIM_ADDITIVE_JUMP_BOOST",
		"WEAP_ANIM_ADDITIVE_JUMP_LAND_ROOT",
		"WEAP_ANIM_ADDITIVE_JUMP_LAND",
		"WEAP_ANIM_ADDITIVE_WALK_ROOT",
		"WEAP_ANIM_ADDITIVE_WALK",
		"WEAP_ANIM_ADDITIVE_WALK_GESTURE_1",
		"WEAP_ANIM_ADDITIVE_WALK_GESTURE_2",
		"WEAP_ANIM_ADDITIVE_SPRINT_ROOT",
		"WEAP_ANIM_ADDITIVE_SPRINT_IN",
		"WEAP_ANIM_ADDITIVE_SPRINT_LOOP",
		"WEAP_ANIM_ADDITIVE_SUPER_SPRINT_LOOP",
		"WEAP_ANIM_ADDITIVE_SPRINT_OUT",
		"WEAP_ANIM_ADDITIVE_SPRINT_OFFSET_ROOT",
		"WEAP_ANIM_ADDITIVE_SPRINT_OFFSET",
		"WEAP_ANIM_ADDITIVE_CRAWL_IN_ROOT",
		"WEAP_ANIM_ADDITIVE_CRAWL_IN",
		"WEAP_ANIM_ADDITIVE_CRAWL_LOOP_ROOT",
		"WEAP_ANIM_ADDITIVE_CRAWL_LOOP",
		"WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT_ROOT",
		"WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT",
		"WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT_ROOT",
		"WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT",
		"WEAP_ANIM_ADDITIVE_PRONE_DROP_ROOT",
		"WEAP_ANIM_ADDITIVE_PRONE_DROP",
		"WEAP_ANIM_ADDITIVE_FINGER_POSE_LEFT_ROOT",
		"WEAP_ANIM_ADDITIVE_FINGER_POSE_LEFT",
		"WEAP_ANIM_ADDITIVE_FINGER_POSE_RIGHT_ROOT",
		"WEAP_ANIM_ADDITIVE_FINGER_POSE_RIGHT",
		"WEAP_ANIM_ADDITIVE_DEMEANOR_OFFSET_ROOT",
		"WEAP_ANIM_ADDITIVE_DEMEANOR_OFFSET_SAFE",
		"WEAP_ANIM_ADDITIVE_GESTURE_1",
		"WEAP_ANIM_ADDITIVE_GESTURE_2",
		"WEAP_ANIM_ADDITIVE_GESTURE_3",
		"WEAP_ANIM_ADDITIVE_GESTURE_4",
		"WEAP_ANIM_ADDITIVE_GESTURE_5",
		"WEAP_ANIM_ADDITIVE_GESTURE_6",
		"WEAP_ANIM_ADDITIVE_GESTURE_7",
		"WEAP_ANIM_ADDITIVE_GESTURE_8",
		"WEAP_ANIM_ADDITIVE_GESTURE_9",
		"WEAP_ANIM_ADDITIVE_GESTURE_10",
		"WEAP_ANIM_ADDITIVE_GESTURE_11",
		"WEAP_ANIM_ADDITIVE_GESTURE_12",
		"WEAP_ANIM_ADDITIVE_GESTURE_13",
		"WEAP_ANIM_ADDITIVE_GESTURE_14",
		"WEAP_ANIM_ADDITIVE_GESTURE_15",
		"WEAP_ANIM_ADDITIVE_GESTURE_16",
		"WEAP_ANIM_ADDITIVE_GESTURE_17",
		"WEAP_ANIM_ADDITIVE_GESTURE_18",
		"WEAP_ANIM_ADDITIVE_GESTURE_19",
		"WEAP_ANIM_ADDITIVE_GESTURE_20",
		"WEAP_ANIM_ADDITIVE_GESTURE_21",
		"WEAP_ANIM_ADDITIVE_GESTURE_22",
		"WEAP_ANIM_ADDITIVE_GESTURE_23",
		"WEAP_ANIM_ADDITIVE_GESTURE_24",
		"WEAP_ANIM_ADDITIVE_GESTURE_25",
		"WEAP_ANIM_ADDITIVE_GESTURE_26",
		"WEAP_ANIM_ADDITIVE_GESTURE_27",
		"WEAP_ANIM_ADDITIVE_GESTURE_28",
		"WEAP_ANIM_ADDITIVE_GESTURE_29",
		"WEAP_ANIM_ADDITIVE_GESTURE_30",
		"WEAP_ANIM_ADDITIVE_GESTURE_31",
		"WEAP_ANIM_ADDITIVE_GESTURE_32",
		"WEAP_ANIM_ADDITIVE_GESTURE_33",
		"WEAP_ANIM_ADDITIVE_GESTURE_34",
		"WEAP_ANIM_ADDITIVE_GESTURE_35",
		"WEAP_ANIM_ADDITIVE_GESTURE_36",
		"WEAP_ANIM_ADDITIVE_GESTURE_37",
		"WEAP_ANIM_ADDITIVE_GESTURE_38",
		"WEAP_ANIM_ADDITIVE_GESTURE_39",
		"WEAP_ANIM_ADDITIVE_GESTURE_40",
		"WEAP_ANIM_ADDITIVE_SETTLE_ROOT",
		"WEAP_ANIM_ADDITIVE_SETTLE_ADS",
		"WEAP_ANIM_ADDITIVE_REGENERATE_ROOT",
		"WEAP_ANIM_ADDITIVE_REGENERATE",
		"WEAP_ANIM_ADDITIVE_MAGAZINE_BULLETS_ROOT",
		"WEAP_ANIM_ADDITIVE_MAGAZINE_BULLETS",
		"WEAP_ANIM_ADDITIVE_RECOIL_ROOT",
		"WEAP_ANIM_ADDITIVE_RECOIL",
		"WEAP_ANIM_ADDITIVE_EMPTY_ROOT",
		"WEAP_ANIM_ADDITIVE_EMPTY",
		"WEAP_ANIM_IDLE",
		"WEAP_ANIM_EMPTY_IDLE",
		"WEAP_ANIM_FIRE",
		"WEAP_ANIM_HOLD_FIRE",
		"WEAP_ANIM_LASTSHOT",
		"WEAP_ANIM_RECHAMBER",
		"WEAP_ANIM_GRENADE_PRIME",
		"WEAP_ANIM_GRENADE_PRIME_READY_TO_THROW",
		"WEAP_ANIM_MELEE_FATAL",
		"WEAP_ANIM_MELEE_HIT",
		"WEAP_ANIM_MELEE_MISS",
		"WEAP_ANIM_MELEE_SWIPE",
		"WEAP_ANIM_MELEE_VICTIM_CROUCHING_FATAL",
		"WEAP_ANIM_MELEE_VICTIM_CROUCHING_HIT",
		"WEAP_ANIM_MELEE_VICTIM_CROUCHING_MISS",
		"WEAP_ANIM_MELEE_FATAL_2",
		"WEAP_ANIM_MELEE_HIT_2",
		"WEAP_ANIM_MELEE_MISS_2",
		"WEAP_ANIM_MELEE_SWIPE_2",
		"WEAP_ANIM_MELEE_VICTIM_CROUCHING_FATAL_2",
		"WEAP_ANIM_MELEE_VICTIM_CROUCHING_HIT_2",
		"WEAP_ANIM_MELEE_VICTIM_CROUCHING_MISS_2",
		"WEAP_ANIM_MELEE_FATAL_3",
		"WEAP_ANIM_MELEE_HIT_3",
		"WEAP_ANIM_MELEE_MISS_3",
		"WEAP_ANIM_MELEE_SWIPE_3",
		"WEAP_ANIM_MELEE_VICTIM_CROUCHING_FATAL_3",
		"WEAP_ANIM_MELEE_VICTIM_CROUCHING_HIT_3",
		"WEAP_ANIM_MELEE_VICTIM_CROUCHING_MISS_3",
		"WEAP_ANIM_MELEE_FATAL_4",
		"WEAP_ANIM_MELEE_HIT_4",
		"WEAP_ANIM_MELEE_MISS_4",
		"WEAP_ANIM_MELEE_SWIPE_4",
		"WEAP_ANIM_MELEE_VICTIM_CROUCHING_FATAL_4",
		"WEAP_ANIM_MELEE_VICTIM_CROUCHING_HIT_4",
		"WEAP_ANIM_MELEE_VICTIM_CROUCHING_MISS_4",
		"WEAP_ANIM_ALT_MELEE_FATAL",
		"WEAP_ANIM_ALT_MELEE_HIT",
		"WEAP_ANIM_ALT_MELEE_MISS",
		"WEAP_ANIM_ALT_MELEE_SWIPE",
		"WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_FATAL",
		"WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_HIT",
		"WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_MISS",
		"WEAP_ANIM_ALT_MELEE_FATAL_2",
		"WEAP_ANIM_ALT_MELEE_HIT_2",
		"WEAP_ANIM_ALT_MELEE_MISS_2",
		"WEAP_ANIM_ALT_MELEE_SWIPE_2",
		"WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_FATAL_2",
		"WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_HIT_2",
		"WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_MISS_2",
		"WEAP_ANIM_ALT_MELEE_FATAL_3",
		"WEAP_ANIM_ALT_MELEE_HIT_3",
		"WEAP_ANIM_ALT_MELEE_MISS_3",
		"WEAP_ANIM_ALT_MELEE_SWIPE_3",
		"WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_FATAL_3",
		"WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_HIT_3",
		"WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_MISS_3",
		"WEAP_ANIM_ALT_MELEE_FATAL_4",
		"WEAP_ANIM_ALT_MELEE_HIT_4",
		"WEAP_ANIM_ALT_MELEE_MISS_4",
		"WEAP_ANIM_ALT_MELEE_SWIPE_4",
		"WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_FATAL_4",
		"WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_HIT_4",
		"WEAP_ANIM_ALT_MELEE_VICTIM_CROUCHING_MISS_4",
		"WEAP_ANIM_RELOAD",
		"WEAP_ANIM_RELOAD_EMPTY",
		"WEAP_ANIM_RELOAD_START",
		"WEAP_ANIM_RELOAD_END",
		"WEAP_ANIM_FAST_RELOAD",
		"WEAP_ANIM_FAST_RELOAD_EMPTY",
		"WEAP_ANIM_FAST_RELOAD_START",
		"WEAP_ANIM_FAST_RELOAD_END",
		"WEAP_ANIM_RAISE",
		"WEAP_ANIM_FIRST_RAISE",
		"WEAP_ANIM_BREACH_RAISE",
		"WEAP_ANIM_DROP",
		"WEAP_ANIM_ALT_RAISE",
		"WEAP_ANIM_ALT_RAISE_ADS",
		"WEAP_ANIM_ALT_RAISE_AKIMBO",
		"WEAP_ANIM_ALT_DROP",
		"WEAP_ANIM_ALT_DROP_ADS",
		"WEAP_ANIM_ALT_DROP_AKIMBO",
		"WEAP_ANIM_ALT_OVERRIDE",
		"WEAP_ANIM_TOGGLE_OVERRIDE",
		"WEAP_ANIM_SCOPE_TOGGLE_ON",
		"WEAP_ANIM_SCOPE_TOGGLE_OFF",
		"WEAP_ANIM_QUICK_RAISE",
		"WEAP_ANIM_QUICK_DROP",
		"WEAP_ANIM_EMPTY_RAISE",
		"WEAP_ANIM_EMPTY_DROP",
		"WEAP_ANIM_SPRINT_IN",
		"WEAP_ANIM_SPRINT_IN_CANCEL",
		"WEAP_ANIM_SPRINT_LOOP",
		"WEAP_ANIM_SPRINT_OUT",
		"WEAP_ANIM_STUNNED_START",
		"WEAP_ANIM_STUNNED_LOOP",
		"WEAP_ANIM_STUNNED_END",
		"WEAP_ANIM_DETONATE",
		"WEAP_ANIM_NIGHTVISION_WEAR",
		"WEAP_ANIM_NIGHTVISION_REMOVE",
		"WEAP_ANIM_ADS_FIRE",
		"WEAP_ANIM_ADS_LASTSHOT",
		"WEAP_ANIM_ADS_RECHAMBER",
		"WEAP_ANIM_BLAST_FRONT",
		"WEAP_ANIM_BLAST_RIGHT",
		"WEAP_ANIM_BLAST_BACK",
		"WEAP_ANIM_BLAST_LEFT",
		"WEAP_ANIM_DODGE_GROUND_BACK",
		"WEAP_ANIM_DODGE_GROUND_LEFT",
		"WEAP_ANIM_DODGE_GROUND_RIGHT",
		"WEAP_ANIM_DODGE_AIR_FORWARD",
		"WEAP_ANIM_DODGE_AIR_BACK",
		"WEAP_ANIM_DODGE_AIR_LEFT",
		"WEAP_ANIM_DODGE_AIR_RIGHT",
		"WEAP_ANIM_LEAP_IN",
		"WEAP_ANIM_LEAP_LOOP",
		"WEAP_ANIM_LEAP_CANCEL",
		"WEAP_ANIM_LEAP_OUT",
		"WEAP_ANIM_CHARGE_IN",
		"WEAP_ANIM_CHARGE_LOOP",
		"WEAP_ANIM_CHARGE_OUT",
		"WEAP_ANIM_ADS_CHARGE_IN",
		"WEAP_ANIM_ADS_CHARGE_LOOP",
		"WEAP_ANIM_ADS_CHARGE_OUT",
		"WEAP_ANIM_RECOIL_SETTLE",
		"WEAP_ANIM_SWIM_LOOP",
		"WEAP_ANIM_RELOAD_MULTIPLE_1",
		"WEAP_ANIM_RELOAD_MULTIPLE_2",
		"WEAP_ANIM_RELOAD_MULTIPLE_3",
		"WEAP_ANIM_RELOAD_MULTIPLE_4",
		"WEAP_ANIM_RELOAD_MULTIPLE_5",
		"WEAP_ANIM_RELOAD_MULTIPLE_6",
		"WEAP_ANIM_RELOAD_MULTIPLE_7",
		"WEAP_ANIM_RELOAD_MULTIPLE_8",
		"WEAP_ANIM_RELOAD_MULTIPLE_FAST_1",
		"WEAP_ANIM_RELOAD_MULTIPLE_FAST_2",
		"WEAP_ANIM_RELOAD_MULTIPLE_FAST_3",
		"WEAP_ANIM_RELOAD_MULTIPLE_FAST_4",
		"WEAP_ANIM_RELOAD_MULTIPLE_FAST_5",
		"WEAP_ANIM_RELOAD_MULTIPLE_FAST_6",
		"WEAP_ANIM_RELOAD_MULTIPLE_FAST_7",
		"WEAP_ANIM_RELOAD_MULTIPLE_FAST_8",
		"WEAP_ANIM_ADS_UP",
		"WEAP_ANIM_ADS_DOWN",
		"WEAP_ANIM_SHIELD_UP",
		"WEAP_ANIM_SHIELD_IDLE",
		"WEAP_ANIM_SHIELD_DOWN",
		"WEAP_ANIM_RECOIL",
		"WEAP_ALT_ANIM_ADJUST",
		"WEAP_ANIM_UNUSED",
		"WEAP_ANIM_3P_FINGER_POSE_LEFT",
		"WEAP_ANIM_3P_FINGER_POSE_RIGHT",
		"WEAP_ANIM_3P_ADDITIVE_ALT_OVERRIDE",
	};

	const char* get_anim_name_from_index(weapAnimFiles_t index)
	{
		return weapon_anim_package::anim_names[index];
	}

	const char* get_anim_name_from_index(int index)
	{
		return weapon_anim_package::anim_names[index];
	}

	weapAnimFiles_t get_anim_index_from_name(const char* name)
	{
		for (unsigned int i = 0; i < NUM_WEAP_ANIMS; i++)
		{
			const char* anim_name = weapon_anim_package::anim_names[i];
			if (!_stricmp(anim_name, name))
			{
				return static_cast<weapAnimFiles_t>(i);
			}
		}

		ZONETOOL_FATAL("Invalid animation index returned for name: \"%s\"", name);
		//return WEAP_ANIM_INVALID;
	}

#define PARSE_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	!data[#__field__].is_null() && !data[#__field__].empty() ? asset->__field__ = mem->duplicate_string(data[#__field__].get<std::string>()) : asset->__field__ = nullptr;

#define PARSE_FIELD(__field__) \
	if (!data[#__field__].is_null()) asset->__field__ = data[#__field__].get<decltype(asset->__field__)>();

#define PARSE_FIELD_ARR(__field__, __size__) \
	if(!data[#__field__].is_null()) \
	{ \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			asset->__field__[idx##__field__] = data[#__field__][idx##__field__].get<typename std::remove_reference<decltype(asset->__field__[idx##__field__])>::type>(); \
		} \
	}

	void parse_anims(XAnimParts** anims, ordered_json& data, zone_memory* mem)
	{
		for (auto i = 0; i < NUM_WEAP_ANIMS; i++)
		{
			if (!data[get_anim_name_from_index(i)].empty())
			{
				anims[i] = mem->allocate<XAnimParts>();
				anims[i]->name = mem->duplicate_string(data[get_anim_name_from_index(i)].get<std::string>());
			}
		}
	}

	void parse_state_timers(WeaponAnimPackageStateTimers* asset, ordered_json& data)
	{
		PARSE_FIELD(iFireDelay);
		PARSE_FIELD(iMeleeDelay);
		PARSE_FIELD(iDetonateDelay);
		PARSE_FIELD(iFirstRaiseTime);
		PARSE_FIELD(iAltRaiseTime);
		PARSE_FIELD(iAltRaiseADSTime);
		PARSE_FIELD(iAltRaiseAkimboTime);
		PARSE_FIELD(iRechamberTime);
		PARSE_FIELD(rechamberTimeOneHanded);
		PARSE_FIELD(iRechamberBoltTime);
		PARSE_FIELD(iHoldFireTime);
		PARSE_FIELD(iHoldPrimeTime);
		PARSE_FIELD(iDetonateTime);
		PARSE_FIELD(iMeleeTime);
		PARSE_FIELD(meleeChargeTime);
		PARSE_FIELD(meleeChargeDelay);
		PARSE_FIELD(altMeleeTime);
		PARSE_FIELD(altMeleeChargeTime);
		PARSE_FIELD(altMeleeChargeDelay);
		PARSE_FIELD(iReloadTime);
		PARSE_FIELD(iFastReloadTime);
		PARSE_FIELD(reloadShowRocketTime);
		PARSE_FIELD(iReloadEmptyTime);
		PARSE_FIELD(iFastReloadEmptyTime);
		PARSE_FIELD(iReloadAddTime);
		PARSE_FIELD(iFastReloadAddTime);
		PARSE_FIELD(iReloadEmptyAddTime);
		PARSE_FIELD(iFastReloadEmptyAddTime);
		PARSE_FIELD(iReloadStartTime);
		PARSE_FIELD(iAltOverride3PInTime);
		PARSE_FIELD(iAltOverride3POutTime);
		PARSE_FIELD(iReloadStartAddTime);
		PARSE_FIELD(iFastReloadStartAddTime);
		PARSE_FIELD(iReloadEndTime);
		PARSE_FIELD(iFastReloadEndTime);
		PARSE_FIELD(iDropTime);
		PARSE_FIELD(iRaiseTime);
		PARSE_FIELD(iAltDropTime);
		PARSE_FIELD(iAltDropADSTime);
		PARSE_FIELD(iAltDropAkimboTime);
		PARSE_FIELD(quickDropTime);
		PARSE_FIELD(quickRaiseTime);
		PARSE_FIELD(iBreachRaiseTime);
		PARSE_FIELD(iEmptyRaiseTime);
		PARSE_FIELD(iEmptyDropTime);
		PARSE_FIELD(sprintInTime);
		PARSE_FIELD(sprintLoopTime);
		PARSE_FIELD(sprintOutTime);
		PARSE_FIELD(walkTime);
		PARSE_FIELD(gungHoTime);
		PARSE_FIELD(crawlTime);
		PARSE_FIELD(stunnedTimeBegin);
		PARSE_FIELD(stunnedTimeLoop);
		PARSE_FIELD(stunnedTimeEnd);
		PARSE_FIELD(nightVisionWearTime);
		PARSE_FIELD(nightVisionWearTimeFadeOutEnd);
		PARSE_FIELD(nightVisionWearTimePowerUp);
		PARSE_FIELD(nightVisionRemoveTime);
		PARSE_FIELD(nightVisionRemoveTimePowerDown);
		PARSE_FIELD(nightVisionRemoveTimeFadeInStart);
		PARSE_FIELD(primeTime);
		PARSE_FIELD(bHoldFullPrime);
		PARSE_FIELD(bAltOverride3PADSOnly);
		PARSE_FIELD(blastFrontTime);
		PARSE_FIELD(blastRightTime);
		PARSE_FIELD(blastBackTime);
		PARSE_FIELD(blastLeftTime);
		PARSE_FIELD(slideInTime);
		PARSE_FIELD(slideLoopTime);
		PARSE_FIELD(slideOutTime);
		PARSE_FIELD(dodgeTime);
		PARSE_FIELD(wallRunInTime);
		PARSE_FIELD(wallRunOutTime);
		PARSE_FIELD(leapInTime);
		PARSE_FIELD(leapLoopTime);
		PARSE_FIELD(leapOutTime);
		PARSE_FIELD(leapCancelTime);
		PARSE_FIELD(chargeInTime);
		PARSE_FIELD(chargeLoopTime);
		PARSE_FIELD(chargeOutTime);
		PARSE_FIELD(iRegenerationTime);
		PARSE_FIELD(iRegenerationAddTime);
		PARSE_FIELD(iReloadInterruptTime);
		PARSE_FIELD(iReloadEmptyInterruptTime);
		PARSE_FIELD(iReloadEndInterruptTime);
		PARSE_FIELD(iReloadMultiple1InterruptTime);
		PARSE_FIELD(iReloadMultiple2InterruptTime);
		PARSE_FIELD(iReloadMultiple3InterruptTime);
		PARSE_FIELD(iReloadMultiple4InterruptTime);
		PARSE_FIELD(iReloadMultiple5InterruptTime);
		PARSE_FIELD(iReloadMultiple6InterruptTime);
		PARSE_FIELD(iReloadMultiple7InterruptTime);
		PARSE_FIELD(iReloadMultiple8InterruptTime);
		PARSE_FIELD(iFastReloadInterruptTime);
		PARSE_FIELD(iFastReloadEmptyInterruptTime);
		PARSE_FIELD(iFastReloadEndInterruptTime);
		PARSE_FIELD(iFastReloadMultiple1InterruptTime);
		PARSE_FIELD(iFastReloadMultiple2InterruptTime);
		PARSE_FIELD(iFastReloadMultiple3InterruptTime);
		PARSE_FIELD(iFastReloadMultiple4InterruptTime);
		PARSE_FIELD(iFastReloadMultiple5InterruptTime);
		PARSE_FIELD(iFastReloadMultiple6InterruptTime);
		PARSE_FIELD(iFastReloadMultiple7InterruptTime);
		PARSE_FIELD(iFastReloadMultiple8InterruptTime);
	}

	void parse(MovementTime* asset, ordered_json& data)
	{
		PARSE_FIELD(time);
		PARSE_FIELD(isLeadIn);
	}

	void parse(FootstepTime* asset, ordered_json& data)
	{
		PARSE_FIELD(time);
		PARSE_FIELD(isLeft);
	}

	void parse(FootstepAnim* asset, ordered_json& data)
	{
		PARSE_FIELD(leftCount);
		for (auto i = 0; i < 4; i++)
		{
			parse(&asset->step[i], data["step"][i]);
		}
		for (auto i = 0; i < 4; i++)
		{
			parse(&asset->movement[i], data["movement"][i]);
		}
	}

	void parse_footsteps(FootstepAnims* asset, ordered_json& data)
	{
		for (auto i = 0; i < 9; i++)
		{
			parse(&asset->footstep[i], data["footstep"][i]);
		}
	}

	WeaponAnimPackage* weapon_anim_package::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "animpkg\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing animpkg \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		ordered_json data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto asset = mem->allocate<WeaponAnimPackage>();

		asset->name = mem->duplicate_string(data["name"].get<std::string>());
		
		PARSE_STRING(name);
		PARSE_FIELD(meleeAnimType);
		PARSE_FIELD(meleeAnimPrimaryCount);
		PARSE_FIELD(meleeAnimAltCount);
		PARSE_FIELD_ARR(fireAnimTimesMs, 4);

		if (!data["anims"].is_null())
		{
			asset->anims = mem->allocate<XAnimParts*>(NUM_WEAP_ANIMS);
			parse_anims(asset->anims, data["anims"], mem);
		}
		else
		{
			asset->anims = nullptr;
		}

		if (!data["timers"].is_null())
		{
			asset->timers = mem->allocate<WeaponAnimPackageStateTimers>();
			parse_state_timers(asset->timers, data["timers"]);
		}
		else
		{
			asset->timers = nullptr;
		}

		if (!data["footsteps"].is_null())
		{
			asset->footstep = mem->allocate<FootstepAnims>();
			parse_footsteps(asset->footstep, data["footsteps"]);
		}
		else
		{
			asset->footstep = nullptr;
		}

		return asset;
	}

	void weapon_anim_package::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).weaponAnimPackage;
		}
	}

	void weapon_anim_package::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void weapon_anim_package::load_depending(zone_base* zone)
	{
		auto* asset = this->asset_;
		if (asset->anims)
		{
			for (auto i = 0; i < NUM_WEAP_ANIMS; i++)
			{
				if (asset->anims[i])
				{
					zone->add_asset_of_type(ASSET_TYPE_XANIMPARTS, asset->anims[i]->name);
				}
			}
		}
	}

	std::string weapon_anim_package::name()
	{
		return this->name_;
	}

	std::int32_t weapon_anim_package::type()
	{
		return ASSET_TYPE_ANIM_PACKAGE;
	}

	void weapon_anim_package::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->anims)
		{
			buf->align(7);
			auto* dest_anims = buf->write(data->anims, NUM_WEAP_ANIMS);
			for (auto i = 0; i < NUM_WEAP_ANIMS; i++)
			{
				if (data->anims[i])
				{
					dest_anims[i] = reinterpret_cast<XAnimParts*>(zone->get_asset_pointer(ASSET_TYPE_XANIMPARTS, data->anims[i]->name));
				}
			}
			buf->clear_pointer(&dest->anims);
		}

		if (data->timers)
		{
			buf->align(3);
			buf->write(data->timers);
			buf->clear_pointer(&dest->timers);
		}

		if (data->footstep)
		{
			buf->align(3);
			buf->write(data->footstep);
			buf->clear_pointer(&dest->footstep);
		}

		buf->pop_stream();
	}

#define DUMP_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	asset->__field__ ? data[#__field__] = asset->__field__ : data[#__field__] = "";

#define DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__;

#define DUMP_FIELD_ARR(__field__, __size__) \
	for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
	{ \
		data[#__field__][idx##__field__] = asset->__field__[idx##__field__]; \
	}

	void dump_anims(XAnimParts** anims, ordered_json& data)
	{
		for (auto i = 0; i < NUM_WEAP_ANIMS; i++)
		{
			data[get_anim_name_from_index(i)] = anims[i] ? anims[i]->name : "";
		}
	}

	void dump_state_timers(WeaponAnimPackageStateTimers* asset, ordered_json& data)
	{
		DUMP_FIELD(iFireDelay);
		DUMP_FIELD(iMeleeDelay);
		DUMP_FIELD(iDetonateDelay);
		DUMP_FIELD(iFirstRaiseTime);
		DUMP_FIELD(iAltRaiseTime);
		DUMP_FIELD(iAltRaiseADSTime);
		DUMP_FIELD(iAltRaiseAkimboTime);
		DUMP_FIELD(iRechamberTime);
		DUMP_FIELD(rechamberTimeOneHanded);
		DUMP_FIELD(iRechamberBoltTime);
		DUMP_FIELD(iHoldFireTime);
		DUMP_FIELD(iHoldPrimeTime);
		DUMP_FIELD(iDetonateTime);
		DUMP_FIELD(iMeleeTime);
		DUMP_FIELD(meleeChargeTime);
		DUMP_FIELD(meleeChargeDelay);
		DUMP_FIELD(altMeleeTime);
		DUMP_FIELD(altMeleeChargeTime);
		DUMP_FIELD(altMeleeChargeDelay);
		DUMP_FIELD(iReloadTime);
		DUMP_FIELD(iFastReloadTime);
		DUMP_FIELD(reloadShowRocketTime);
		DUMP_FIELD(iReloadEmptyTime);
		DUMP_FIELD(iFastReloadEmptyTime);
		DUMP_FIELD(iReloadAddTime);
		DUMP_FIELD(iFastReloadAddTime);
		DUMP_FIELD(iReloadEmptyAddTime);
		DUMP_FIELD(iFastReloadEmptyAddTime);
		DUMP_FIELD(iReloadStartTime);
		DUMP_FIELD(iAltOverride3PInTime);
		DUMP_FIELD(iAltOverride3POutTime);
		DUMP_FIELD(iReloadStartAddTime);
		DUMP_FIELD(iFastReloadStartAddTime);
		DUMP_FIELD(iReloadEndTime);
		DUMP_FIELD(iFastReloadEndTime);
		DUMP_FIELD(iDropTime);
		DUMP_FIELD(iRaiseTime);
		DUMP_FIELD(iAltDropTime);
		DUMP_FIELD(iAltDropADSTime);
		DUMP_FIELD(iAltDropAkimboTime);
		DUMP_FIELD(quickDropTime);
		DUMP_FIELD(quickRaiseTime);
		DUMP_FIELD(iBreachRaiseTime);
		DUMP_FIELD(iEmptyRaiseTime);
		DUMP_FIELD(iEmptyDropTime);
		DUMP_FIELD(sprintInTime);
		DUMP_FIELD(sprintLoopTime);
		DUMP_FIELD(sprintOutTime);
		DUMP_FIELD(walkTime);
		DUMP_FIELD(gungHoTime);
		DUMP_FIELD(crawlTime);
		DUMP_FIELD(stunnedTimeBegin);
		DUMP_FIELD(stunnedTimeLoop);
		DUMP_FIELD(stunnedTimeEnd);
		DUMP_FIELD(nightVisionWearTime);
		DUMP_FIELD(nightVisionWearTimeFadeOutEnd);
		DUMP_FIELD(nightVisionWearTimePowerUp);
		DUMP_FIELD(nightVisionRemoveTime);
		DUMP_FIELD(nightVisionRemoveTimePowerDown);
		DUMP_FIELD(nightVisionRemoveTimeFadeInStart);
		DUMP_FIELD(primeTime);
		DUMP_FIELD(bHoldFullPrime);
		DUMP_FIELD(bAltOverride3PADSOnly);
		DUMP_FIELD(blastFrontTime);
		DUMP_FIELD(blastRightTime);
		DUMP_FIELD(blastBackTime);
		DUMP_FIELD(blastLeftTime);
		DUMP_FIELD(slideInTime);
		DUMP_FIELD(slideLoopTime);
		DUMP_FIELD(slideOutTime);
		DUMP_FIELD(dodgeTime);
		DUMP_FIELD(wallRunInTime);
		DUMP_FIELD(wallRunOutTime);
		DUMP_FIELD(leapInTime);
		DUMP_FIELD(leapLoopTime);
		DUMP_FIELD(leapOutTime);
		DUMP_FIELD(leapCancelTime);
		DUMP_FIELD(chargeInTime);
		DUMP_FIELD(chargeLoopTime);
		DUMP_FIELD(chargeOutTime);
		DUMP_FIELD(iRegenerationTime);
		DUMP_FIELD(iRegenerationAddTime);
		DUMP_FIELD(iReloadInterruptTime);
		DUMP_FIELD(iReloadEmptyInterruptTime);
		DUMP_FIELD(iReloadEndInterruptTime);
		DUMP_FIELD(iReloadMultiple1InterruptTime);
		DUMP_FIELD(iReloadMultiple2InterruptTime);
		DUMP_FIELD(iReloadMultiple3InterruptTime);
		DUMP_FIELD(iReloadMultiple4InterruptTime);
		DUMP_FIELD(iReloadMultiple5InterruptTime);
		DUMP_FIELD(iReloadMultiple6InterruptTime);
		DUMP_FIELD(iReloadMultiple7InterruptTime);
		DUMP_FIELD(iReloadMultiple8InterruptTime);
		DUMP_FIELD(iFastReloadInterruptTime);
		DUMP_FIELD(iFastReloadEmptyInterruptTime);
		DUMP_FIELD(iFastReloadEndInterruptTime);
		DUMP_FIELD(iFastReloadMultiple1InterruptTime);
		DUMP_FIELD(iFastReloadMultiple2InterruptTime);
		DUMP_FIELD(iFastReloadMultiple3InterruptTime);
		DUMP_FIELD(iFastReloadMultiple4InterruptTime);
		DUMP_FIELD(iFastReloadMultiple5InterruptTime);
		DUMP_FIELD(iFastReloadMultiple6InterruptTime);
		DUMP_FIELD(iFastReloadMultiple7InterruptTime);
		DUMP_FIELD(iFastReloadMultiple8InterruptTime);
	}

	void dump(MovementTime* asset, ordered_json& data)
	{
		DUMP_FIELD(time);
		DUMP_FIELD(isLeadIn);
	}

	void dump(FootstepTime* asset, ordered_json& data)
	{
		DUMP_FIELD(time);
		DUMP_FIELD(isLeft);
	}

	void dump(FootstepAnim* asset, ordered_json& data)
	{
		DUMP_FIELD(leftCount);
		for (auto i = 0; i < 4; i++)
		{
			dump(&asset->step[i], data["step"][i]);
		}
		for (auto i = 0; i < 4; i++)
		{
			dump(&asset->movement[i], data["movement"][i]);
		}
	}

	void dump_footsteps(FootstepAnims* asset, ordered_json& data)
	{
		for (auto i = 0; i < 9; i++)
		{
			dump(&asset->footstep[i], data["footstep"][i]);
		}
	}

	void weapon_anim_package::dump(WeaponAnimPackage* asset)
	{
		const auto path = "animpkg\\"s + asset->name + ".json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		DUMP_STRING(name);
		DUMP_FIELD(meleeAnimType);
		DUMP_FIELD(meleeAnimPrimaryCount);
		DUMP_FIELD(meleeAnimAltCount);
		DUMP_FIELD_ARR(fireAnimTimesMs, 4);

		if (asset->anims)
		{
			dump_anims(asset->anims, data["anims"]);
		}
		else
		{
			data["anims"] = nullptr;
		}

		if (asset->timers)
		{
			dump_state_timers(asset->timers, data["timers"]);
		}
		else
		{
			data["timers"] = nullptr;
		}

		if (asset->footstep)
		{
			dump_footsteps(asset->footstep, data["footsteps"]);
		}
		else
		{
			data["footsteps"] = nullptr;
		}

		auto str = data.dump(4);
		data.clear();
		file.write(str);
		file.close();
	}
}