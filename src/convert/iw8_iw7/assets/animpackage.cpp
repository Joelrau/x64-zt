#include <std_include.hpp>

#include "../include.hpp"
#include "animpackage.hpp"
#include "xanim.hpp"

#include <iw7/assets/weapon_anim_package.hpp>

namespace zonetool::iw8::converter::iw7::animpackage
{
	namespace
	{
		namespace target = zonetool::iw7;

		const std::array<std::pair<weapAnimFiles_t, target::weapAnimFiles_t>, 35> anim_slots
		{{
			{WEAP_ANIM_ADDITIVE_ADS_UP, target::WEAP_ANIM_ADDITIVE_ADS_UP},
			{WEAP_ANIM_SPRINT_LOOP, target::WEAP_ANIM_SPRINT_LOOP},
			{WEAP_ANIM_ADDITIVE_JUMP, target::WEAP_ANIM_ADDITIVE_JUMP},
			{WEAP_ANIM_ADDITIVE_JUMP_LAND, target::WEAP_ANIM_ADDITIVE_JUMP_LAND},
			{WEAP_ANIM_ADDITIVE_CRAWL_IN, target::WEAP_ANIM_ADDITIVE_CRAWL_IN},
			{WEAP_ANIM_ADDITIVE_CRAWL_LOOP, target::WEAP_ANIM_ADDITIVE_CRAWL_LOOP},
			{WEAP_ANIM_ADDITIVE_PRONE_DROP, target::WEAP_ANIM_ADDITIVE_PRONE_DROP},
			{WEAP_ANIM_ADDITIVE_FINGER_POSE_LEFT, target::WEAP_ANIM_ADDITIVE_FINGER_POSE_LEFT},
			{WEAP_ANIM_ADDITIVE_FINGER_POSE_RIGHT, target::WEAP_ANIM_ADDITIVE_FINGER_POSE_RIGHT},
			{WEAP_ANIM_ADDITIVE_DEMEANOR_OFFSET_SAFE, target::WEAP_ANIM_ADDITIVE_DEMEANOR_OFFSET_SAFE},
			{WEAP_ANIM_ADDITIVE_SETTLE_ADS, target::WEAP_ANIM_ADDITIVE_SETTLE_ADS},
			{WEAP_ANIM_ADDITIVE_RECOIL, target::WEAP_ANIM_ADDITIVE_RECOIL},
			{WEAP_ANIM_IDLE, target::WEAP_ANIM_IDLE},
			{WEAP_ANIM_EMPTY_IDLE, target::WEAP_ANIM_EMPTY_IDLE},
			{WEAP_ANIM_FIRE, target::WEAP_ANIM_FIRE},
			{WEAP_ANIM_LASTSHOT, target::WEAP_ANIM_LASTSHOT},
			{WEAP_ANIM_MELEE_MISS, target::WEAP_ANIM_MELEE_MISS},
			{WEAP_ANIM_MELEE_HIT, target::WEAP_ANIM_MELEE_HIT},
			{WEAP_ANIM_MELEE_FATAL, target::WEAP_ANIM_MELEE_FATAL},
			{WEAP_ANIM_RELOAD, target::WEAP_ANIM_RELOAD},
			{WEAP_ANIM_RELOAD_EMPTY, target::WEAP_ANIM_RELOAD_EMPTY},
			{WEAP_ANIM_FAST_RELOAD, target::WEAP_ANIM_FAST_RELOAD},
			{WEAP_ANIM_FAST_RELOAD_EMPTY, target::WEAP_ANIM_FAST_RELOAD_EMPTY},
			{WEAP_ANIM_RAISE, target::WEAP_ANIM_RAISE},
			{WEAP_ANIM_FIRST_RAISE, target::WEAP_ANIM_FIRST_RAISE},
			{WEAP_ANIM_DROP, target::WEAP_ANIM_DROP},
			{WEAP_ANIM_QUICK_RAISE, target::WEAP_ANIM_QUICK_RAISE},
			{WEAP_ANIM_QUICK_DROP, target::WEAP_ANIM_QUICK_DROP},
			{WEAP_ANIM_EMPTY_RAISE, target::WEAP_ANIM_EMPTY_RAISE},
			{WEAP_ANIM_EMPTY_DROP, target::WEAP_ANIM_EMPTY_DROP},
			{WEAP_ANIM_ADS_FIRE, target::WEAP_ANIM_ADS_FIRE},
			{WEAP_ANIM_ADS_LASTSHOT, target::WEAP_ANIM_ADS_LASTSHOT},
			{WEAP_ANIM_RECOIL_SETTLE, target::WEAP_ANIM_RECOIL_SETTLE},
			{WEAP_ANIM_ADS_UP, target::WEAP_ANIM_ADS_UP},
			{WEAP_ANIM_ADS_DOWN, target::WEAP_ANIM_ADS_DOWN},
		}};

		target::XAnimParts** convert_anims(XAnimParts** anims, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate_array<target::XAnimParts*>(target::NUM_WEAP_ANIMS);
			for (const auto& [source, slot] : anim_slots)
			{
				if (!anims[source])
				{
					continue;
				}

				xanim::dump(anims[source]);
				result[slot] = allocator.allocate<target::XAnimParts>();
				result[slot]->name = anims[source]->name;
			}

			return result;
		}

		int anim_time_ms(const XAnimParts* anim)
		{
			if (!anim || anim->framerate <= 0.0f)
			{
				return 0;
			}

			return static_cast<int>(anim->numframes * 1000.0f / anim->framerate);
		}

		target::WeaponAnimPackageStateTimers* convert_timers(const WeaponAnimPackageStateTimers* asset, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::WeaponAnimPackageStateTimers>();
			result->iFireDelay = 0;
			result->iMeleeDelay = 0;
			result->iDetonateDelay = asset->iDetonateDelay;
			result->iFirstRaiseTime = asset->iFirstRaiseTime;
			result->iAltRaiseTime = asset->iAltRaiseTime;
			result->iAltRaiseADSTime = asset->iAltRaiseADSTime;
			result->iAltRaiseAkimboTime = asset->iAltRaiseAkimboTime;
			result->iRechamberTime = asset->iRechamberTime;
			result->rechamberTimeOneHanded = asset->rechamberTimeOneHanded;
			result->iRechamberBoltTime = asset->iRechamberBoltTime;
			result->iHoldFireTime = asset->iHoldFireTime;
			result->iHoldPrimeTime = asset->iHoldPrimeTime;
			result->iDetonateTime = asset->iDetonateTime;
			result->iMeleeTime = asset->iMeleeTime;
			result->meleeChargeTime = asset->meleeChargeTime;
			result->meleeChargeDelay = 0;
			result->altMeleeTime = asset->altMeleeTime;
			result->altMeleeChargeTime = asset->altMeleeChargeTime;
			result->altMeleeChargeDelay = 0;
			result->iReloadTime = asset->iReloadTime;
			result->iFastReloadTime = asset->iFastReloadTime;
			result->reloadShowRocketTime = asset->reloadShowRocketTime;
			result->iReloadEmptyTime = asset->iReloadEmptyTime;
			result->iFastReloadEmptyTime = asset->iFastReloadEmptyTime;
			result->iReloadAddTime = asset->iReloadAddTime;
			result->iFastReloadAddTime = asset->iFastReloadAddTime;
			result->iReloadEmptyAddTime = asset->iReloadEmptyAddTime;
			result->iFastReloadEmptyAddTime = asset->iFastReloadEmptyAddTime;
			result->iReloadStartTime = asset->iReloadStartTime;
			result->iAltOverride3PInTime = asset->iAltOverride3PInTime;
			result->iAltOverride3POutTime = asset->iAltOverride3POutTime;
			result->iReloadStartAddTime = asset->iReloadStartAddTime;
			result->iFastReloadStartAddTime = asset->iFastReloadStartAddTime;
			result->iReloadEndTime = asset->iReloadEndTime;
			result->iFastReloadEndTime = asset->iFastReloadEndTime;
			result->iDropTime = asset->iDropTime;
			result->iRaiseTime = asset->iRaiseTime;
			result->iAltDropTime = asset->iAltDropTime;
			result->iAltDropADSTime = asset->iAltDropADSTime;
			result->iAltDropAkimboTime = asset->iAltDropAkimboTime;
			result->quickDropTime = asset->quickDropTime;
			result->quickRaiseTime = asset->quickRaiseTime;
			result->iBreachRaiseTime = asset->iBreachRaiseTime;
			result->iEmptyRaiseTime = asset->iEmptyRaiseTime;
			result->iEmptyDropTime = asset->iEmptyDropTime;
			result->sprintInTime = asset->sprintInTime;
			result->sprintLoopTime = asset->sprintLoopTime;
			result->sprintOutTime = asset->sprintOutTime;
			result->walkTime = asset->walkTime;
			result->gungHoTime = asset->gungHoTime;
			result->crawlTime = asset->crawlTime;
			result->stunnedTimeBegin = asset->stunnedTimeBegin;
			result->stunnedTimeLoop = asset->stunnedTimeLoop;
			result->stunnedTimeEnd = asset->stunnedTimeEnd;
			result->nightVisionWearTime = 0;
			result->nightVisionWearTimeFadeOutEnd = 0;
			result->nightVisionWearTimePowerUp = 0;
			result->nightVisionRemoveTime = 0;
			result->nightVisionRemoveTimePowerDown = 0;
			result->nightVisionRemoveTimeFadeInStart = 0;
			result->primeTime = asset->primeTime;
			result->bHoldFullPrime = asset->bHoldFullPrime;
			result->bAltOverride3PADSOnly = asset->bAltOverride3PADSOnly;
			result->blastFrontTime = asset->blastFrontTime;
			result->blastRightTime = asset->blastRightTime;
			result->blastBackTime = asset->blastBackTime;
			result->blastLeftTime = asset->blastLeftTime;
			result->slideInTime = asset->slideInTime;
			result->slideLoopTime = asset->slideLoopTime;
			result->slideOutTime = asset->slideOutTime;
			result->dodgeTime = asset->dodgeTime;
			result->wallRunInTime = asset->wallRunInTime;
			result->wallRunOutTime = asset->wallRunOutTime;
			result->leapInTime = asset->leapInTime;
			result->leapLoopTime = asset->leapLoopTime;
			result->leapOutTime = asset->leapOutTime;
			result->leapCancelTime = asset->leapCancelTime;
			result->chargeInTime = asset->chargeInTime;
			result->chargeLoopTime = asset->chargeLoopTime;
			result->chargeOutTime = asset->chargeOutTime;
			result->iRegenerationTime = asset->iRegenerationTime;
			result->iRegenerationAddTime = asset->iRegenerationAddTime;
			result->iReloadInterruptTime = asset->iReloadInterruptTime;
			result->iReloadEmptyInterruptTime = asset->iReloadEmptyInterruptTime;
			result->iReloadEndInterruptTime = asset->iReloadEndInterruptTime;
			result->iReloadMultiple1InterruptTime = asset->iReloadMultiple1InterruptTime;
			result->iReloadMultiple2InterruptTime = asset->iReloadMultiple2InterruptTime;
			result->iReloadMultiple3InterruptTime = asset->iReloadMultiple3InterruptTime;
			result->iReloadMultiple4InterruptTime = asset->iReloadMultiple4InterruptTime;
			result->iReloadMultiple5InterruptTime = asset->iReloadMultiple5InterruptTime;
			result->iReloadMultiple6InterruptTime = asset->iReloadMultiple6InterruptTime;
			result->iReloadMultiple7InterruptTime = asset->iReloadMultiple7InterruptTime;
			result->iReloadMultiple8InterruptTime = asset->iReloadMultiple8InterruptTime;
			result->iFastReloadInterruptTime = asset->iFastReloadInterruptTime;
			result->iFastReloadEmptyInterruptTime = asset->iFastReloadEmptyInterruptTime;
			result->iFastReloadEndInterruptTime = asset->iFastReloadEndInterruptTime;
			result->iFastReloadMultiple1InterruptTime = asset->iFastReloadMultiple1InterruptTime;
			result->iFastReloadMultiple2InterruptTime = asset->iFastReloadMultiple2InterruptTime;
			result->iFastReloadMultiple3InterruptTime = asset->iFastReloadMultiple3InterruptTime;
			result->iFastReloadMultiple4InterruptTime = asset->iFastReloadMultiple4InterruptTime;
			result->iFastReloadMultiple5InterruptTime = asset->iFastReloadMultiple5InterruptTime;
			result->iFastReloadMultiple6InterruptTime = asset->iFastReloadMultiple6InterruptTime;
			result->iFastReloadMultiple7InterruptTime = asset->iFastReloadMultiple7InterruptTime;
			result->iFastReloadMultiple8InterruptTime = asset->iFastReloadMultiple8InterruptTime;
			return result;
		}

		target::WeaponAnimPackage* convert(const WeaponAnimPackage* asset, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::WeaponAnimPackage>();
			result->name = asset->name;
			result->anims = asset->anims ? convert_anims(asset->anims, allocator) : nullptr;
			result->timers = asset->timers ? convert_timers(asset->timers, allocator) : nullptr;
			result->meleeAnimType = asset->meleeAnimPrimaryType;
			result->meleeAnimPrimaryCount = asset->meleeAnimPrimaryCount;
			result->meleeAnimAltCount = asset->meleeAnimAltCount;
			if (asset->anims)
			{
				result->fireAnimTimesMs[0] = anim_time_ms(asset->anims[WEAP_ANIM_FIRE]);
				result->fireAnimTimesMs[1] = anim_time_ms(asset->anims[WEAP_ANIM_LASTSHOT]);
				result->fireAnimTimesMs[2] = anim_time_ms(asset->anims[WEAP_ANIM_ADS_FIRE]);
				result->fireAnimTimesMs[3] = anim_time_ms(asset->anims[WEAP_ANIM_ADS_LASTSHOT]);
			}
			result->footstep = allocator.allocate<target::FootstepAnims>();
			return result;
		}
	}

	std::vector<XAnimParts*> converted_anims(const WeaponAnimPackage* asset)
	{
		std::vector<XAnimParts*> result;
		if (!asset || !asset->anims)
		{
			return result;
		}

		for (const auto& [source, slot] : anim_slots)
		{
			if (asset->anims[source])
			{
				result.emplace_back(asset->anims[source]);
			}
		}

		return result;
	}

	void dump(WeaponAnimPackage* asset)
	{
		if (!asset || !mark_dumped(ASSET_TYPE_ANIM_PACKAGE, asset->name))
		{
			return;
		}

		utils::memory::allocator allocator;
		target::weapon_anim_package::dump(convert(asset, allocator));
		add_csv_row("animpkg", asset->name);
	}
}
