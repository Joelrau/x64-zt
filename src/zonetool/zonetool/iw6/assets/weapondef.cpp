#include "std_include.hpp"
#include "weapondef.hpp"

namespace zonetool::iw6
{
	constexpr const char* anim_names[NUM_WEAP_ANIMS] =
	{
		"ROOT",
		"IDLE",
		"EMPTY_IDLE",
		"FIRE",
		"HOLD_FIRE",
		"LASTSHOT",
		"RECHAMBER",
		"GRENADE_PRIME",
		"GRENADE_PRIME_READY_TO_THROW",
		"MELEE_SWIPE",
		"MELEE_HIT",
		"MELEE_FATAL",
		"MELEE_MISS",
		"MELEE_VICTIM_CROUCHING_HIT",
		"MELEE_VICTIM_CROUCHING_FATAL",
		"MELEE_VICTIM_CROUCHING_MISS",
		"RELOAD",
		"RELOAD_EMPTY",
		"RELOAD_START",
		"RELOAD_END",
		"FAST_RELOAD",
		"FAST_RELOAD_EMPTY",
		"FAST_RELOAD_START",
		"FAST_RELOAD_END",
		"RAISE",
		"FIRST_RAISE",
		"BREACH_RAISE",
		"DROP",
		"ALT_RAISE",
		"ALT_DROP",
		"ALT_OVERRIDE",
		"TOGGLE_OVERRIDE",
		"SCOPE_TOGGLE_ON",
		"SCOPE_TOGGLE_OFF",
		"QUICK_RAISE",
		"QUICK_DROP",
		"EMPTY_RAISE",
		"EMPTY_DROP",
		"SPRINT_IN",
		"SPRINT_IN_CANCEL",
		"SPRINT_LOOP",
		"SPRINT_OUT",
		"STUNNED_START",
		"STUNNED_LOOP",
		"STUNNED_END",
		"DETONATE",
		"NIGHTVISION_WEAR",
		"NIGHTVISION_REMOVE",
		"ADS_FIRE",
		"ADS_LASTSHOT",
		"ADS_RECHAMBER",
		"BLAST_FRONT",
		"BLAST_RIGHT",
		"BLAST_BACK",
		"BLAST_LEFT",
		"SLIDE_IN",
		"SLIDE_LOOP",
		"SLIDE_OUT",
		"RECOIL_SETTLE",
		"SWIM_LOOP",
		"MANTLE_UP_64",
		"MANTLE_UP_56",
		"MANTLE_UP_48",
		"MANTLE_UP_40",
		"MANTLE_UP_32",
		"MANTLE_UP_24",
		"MANTLE_UP_16",
		"MANTLE_OVER_64",
		"MANTLE_OVER_56",
		"MANTLE_OVER_48",
		"MANTLE_OVER_40",
		"MANTLE_OVER_32",
		"MANTLE_OVER_24",
		"MANTLE_OVER_16",
		"RELOAD_MULTIPLE_1",
		"RELOAD_MULTIPLE_2",
		"RELOAD_MULTIPLE_3",
		"RELOAD_MULTIPLE_4",
		"RELOAD_MULTIPLE_5",
		"RELOAD_MULTIPLE_6",
		"RELOAD_MULTIPLE_7",
		"RELOAD_MULTIPLE_8",
		"RELOAD_MULTIPLE_FAST_1",
		"RELOAD_MULTIPLE_FAST_2",
		"RELOAD_MULTIPLE_FAST_3",
		"RELOAD_MULTIPLE_FAST_4",
		"RELOAD_MULTIPLE_FAST_5",
		"RELOAD_MULTIPLE_FAST_6",
		"RELOAD_MULTIPLE_FAST_7",
		"RELOAD_MULTIPLE_FAST_8",
		"ADS_UP",
		"ADS_DOWN",
		"RECOIL",
		"WEAP_ALT_ANIM_ADJUST",
		"ADDITIVE_ADS_ROOT",
		"ADDITIVE_ADS_UP",
		"ADDITIVE_DRAG_LEFT_ROOT",
		"ADDITIVE_DRAG_LEFT",
		"ADDITIVE_DRAG_RIGHT_ROOT",
		"ADDITIVE_DRAG_RIGHT",
		"ADDITIVE_DRAG_UP_ROOT",
		"ADDITIVE_DRAG_UP",
		"ADDITIVE_DRAG_DOWN_ROOT",
		"ADDITIVE_DRAG_DOWN",
		"ADDITIVE_SWIM_FORWARD_ROOT",
		"ADDITIVE_SWIM_FORWARD",
		"ADDITIVE_SWIM_BACKWARD_ROOT",
		"ADDITIVE_SWIM_BACKWARD",
		"ADDITIVE_JUMP_ROOT",
		"ADDITIVE_JUMP",
		"ADDITIVE_JUMP_LAND_ROOT",
		"ADDITIVE_JUMP_LAND",
		"ADDITIVE_WALK_ROOT",
		"ADDITIVE_WALK",
		"ADDITIVE_CRAWL_IN_ROOT",
		"ADDITIVE_CRAWL_IN",
		"ADDITIVE_CRAWL_LOOP_ROOT",
		"ADDITIVE_CRAWL_LOOP",
		"ADDITIVE_CRAWL_LOOP_LEFT_ROOT",
		"ADDITIVE_CRAWL_LOOP_LEFT",
		"ADDITIVE_CRAWL_LOOP_RIGHT_ROOT",
		"ADDITIVE_CRAWL_LOOP_RIGHT",
		"ADDITIVE_PRONE_DROP_ROOT",
		"ADDITIVE_PRONE_DROP",
	};

	const char* get_anim_name_from_index(weapAnimFiles_t index)
	{
		return anim_names[index];
	}

	const char* get_anim_name_from_index(int index)
	{
		return anim_names[index];
	}

	weapAnimFiles_t get_anim_index_from_name(const char* name)
	{
		for (unsigned int i = 0; i < NUM_WEAP_ANIMS; i++)
		{
			const char* anim_name = anim_names[i];
			if (!_stricmp(anim_name, name))
			{
				return static_cast<weapAnimFiles_t>(i);
			}
		}

		ZONETOOL_FATAL("Invalid animation index returned for name: \"%s\"", name);
		//return WEAP_ANIM_INVALID;
	}

	void weapon_def::add_script_string(scr_string_t* ptr, const char* str)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return;
			}
		}
		this->script_strings.push_back(std::pair<scr_string_t*, const char*>(ptr, str));
	}

	const char* weapon_def::get_script_string(scr_string_t* ptr)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return this->script_strings[i].second;
			}
		}
		return nullptr;
	}

#define WEAPON_READ_FIELD(__type__, __field__) \
	if (!data[#__field__].is_null()) *(__type__*)(&weapon->__field__) = (__type__)data[#__field__].get<__type__>()

#define WEAPON_READ_FIELD_ARR(__type__, __field__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		if constexpr (std::is_pointer<decltype(weapon->__field__)>::value) \
		{ \
			*reinterpret_cast<__type__**>(reinterpret_cast<size_t>(&weapon->__field__)) = mem->allocate<__type__>(__size__); \
		} \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			*(__type__*)(&weapon->__field__[idx##__field__]) = (__type__)data[#__field__][idx##__field__].get<__type__>(); \
		} \
	}

#define WEAPON_READ_STRING(__field__) \
	if (!data[#__field__].is_null()) weapon->__field__ = mem->duplicate_string(data[#__field__].get<std::string>())

#define WEAPON_READ_ASSET(__type__, __datafield__, __field__) \
	if (!data[#__field__].is_null() && data[#__field__].is_string()) \
	{ \
		auto asset##__field__ = data[#__field__].get<std::string>(); \
		if (asset##__field__.empty()) \
		{ \
			weapon->__field__ = nullptr; \
		} \
		else \
		{ \
			weapon->__field__ = db_find_x_asset_header(XAssetType::__type__, asset##__field__.data(), 1).__datafield__; \
		} \
	} \
	else \
	{ \
		weapon->__field__ = nullptr; \
	}

#define WEAPON_READ_ASSET_ARR(__type__, __datafield__, __field__, __struct__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		weapon->__field__ = mem->allocate<__struct__*>(__size__); \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			auto asset##__field__ = data[#__field__][idx##__field__].get<std::string>(); \
			if (asset##__field__.empty()) \
			{ \
				weapon->__field__[idx##__field__] = nullptr; \
			} \
			else \
			{ \
				weapon->__field__[idx##__field__] = db_find_x_asset_header(XAssetType::__type__, asset##__field__.data(), 1).__datafield__; \
			} \
		} \
	} \
	else \
	{ \
		weapon->__field__ = nullptr; \
	}

#define WEAPON_READ_ANIM_ARR(__field__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		weapon->__field__ = mem->allocate<XAnimParts*>(__size__); \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			auto asset##__field__ = data[#__field__][get_anim_name_from_index(idx##__field__)].get<std::string>(); \
			if (asset##__field__.empty()) \
			{ \
				weapon->__field__[idx##__field__] = nullptr; \
			} \
			else \
			{ \
				weapon->__field__[idx##__field__] = db_find_x_asset_header(XAssetType::ASSET_TYPE_XANIMPARTS, asset##__field__.data(), 1).parts; \
			} \
		} \
	} \
	else \
	{ \
		weapon->__field__ = nullptr; \
	}

#define WEAPON_READ_SOUND(__field__) \
	if (!data["sounds"][#__field__].is_null() && data["sounds"][#__field__].is_string()) \
	{ \
		auto asset##__field__ = data["sounds"][#__field__].get<std::string>(); \
		if (asset##__field__.empty()) \
		{ \
			weapon->__field__ = nullptr; \
		} \
		else \
		{ \
			weapon->__field__ = db_find_x_asset_header(XAssetType::ASSET_TYPE_SOUND, asset##__field__.data(), 1).sound; \
		} \
	} \
	else \
	{ \
		weapon->__field__ = nullptr; \
	}

	void parse_overlay(ADSOverlay* weapon, json& data)
	{
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, shader);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, shaderLowRes);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, shaderEMP);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, shaderEMPLowRes);
		WEAPON_READ_FIELD(int, reticle);
		WEAPON_READ_FIELD(float, width);
		WEAPON_READ_FIELD(float, height);
		WEAPON_READ_FIELD(float, widthSplitscreen);
		WEAPON_READ_FIELD(float, heightSplitscreen);
	}

	void parse_accuracy_graph(WeaponDef* def, json& data, zone_memory* mem)
	{
		
	}

	void parse_statetimers(StateTimers* weapon, json& data)
	{
		WEAPON_READ_FIELD(int, iFireDelay);
		WEAPON_READ_FIELD(int, iMeleeDelay);
		WEAPON_READ_FIELD(int, meleeChargeDelay);
		WEAPON_READ_FIELD(int, iDetonateDelay);
		WEAPON_READ_FIELD(int, iRechamberTime);
		WEAPON_READ_FIELD(int, rechamberTimeOneHanded);
		WEAPON_READ_FIELD(int, iRechamberBoltTime);
		WEAPON_READ_FIELD(int, iHoldFireTime);
		WEAPON_READ_FIELD(int, iHoldPrimeTime);
		WEAPON_READ_FIELD(int, iDetonateTime);
		WEAPON_READ_FIELD(int, iMeleeTime);
		WEAPON_READ_FIELD(int, meleeChargeTime);
		WEAPON_READ_FIELD(int, iReloadTime);
		WEAPON_READ_FIELD(int, reloadShowRocketTime);
		WEAPON_READ_FIELD(int, iReloadEmptyTime);
		WEAPON_READ_FIELD(int, iReloadAddTime);
		WEAPON_READ_FIELD(int, iReloadEmptyAddTime);
		WEAPON_READ_FIELD(int, iReloadStartTime);
		WEAPON_READ_FIELD(int, iReloadStartAddTime);
		WEAPON_READ_FIELD(int, iReloadEndTime);
		WEAPON_READ_FIELD(int, iDropTime);
		WEAPON_READ_FIELD(int, iRaiseTime);
		WEAPON_READ_FIELD(int, iAltDropTime);
		WEAPON_READ_FIELD(int, quickDropTime);
		WEAPON_READ_FIELD(int, quickRaiseTime);
		WEAPON_READ_FIELD(int, iBreachRaiseTime);
		WEAPON_READ_FIELD(int, iEmptyRaiseTime);
		WEAPON_READ_FIELD(int, iEmptyDropTime);
		WEAPON_READ_FIELD(int, sprintInTime);
		WEAPON_READ_FIELD(int, sprintLoopTime);
		WEAPON_READ_FIELD(int, sprintOutTime);
		WEAPON_READ_FIELD(int, stunnedTimeBegin);
		WEAPON_READ_FIELD(int, stunnedTimeLoop);
		WEAPON_READ_FIELD(int, stunnedTimeEnd);
		WEAPON_READ_FIELD(int, nightVisionWearTime);
		WEAPON_READ_FIELD(int, nightVisionWearTimeFadeOutEnd);
		WEAPON_READ_FIELD(int, nightVisionWearTimePowerUp);
		WEAPON_READ_FIELD(int, nightVisionRemoveTime);
		WEAPON_READ_FIELD(int, nightVisionRemoveTimePowerDown);
		WEAPON_READ_FIELD(int, nightVisionRemoveTimeFadeInStart);
		WEAPON_READ_FIELD(int, fuseTime);
		WEAPON_READ_FIELD(int, aiFuseTime);
		WEAPON_READ_FIELD(bool, bHoldFullPrime);
		WEAPON_READ_FIELD(int, blastFrontTime);
		WEAPON_READ_FIELD(int, blastRightTime);
		WEAPON_READ_FIELD(int, blastBackTime);
		WEAPON_READ_FIELD(int, blastLeftTime);
		WEAPON_READ_FIELD(int, slideInTime);
		WEAPON_READ_FIELD(int, slideLoopTime);
		WEAPON_READ_FIELD(int, slideOutTime);
	}

	WeaponDef* weapon_def::parse_weapondef(json& data, WeaponCompleteDef* baseAsset, zone_memory* mem)
	{
		auto weapon = mem->allocate<WeaponDef>();

		if (baseAsset)
		{
			memcpy(weapon, baseAsset->weapDef, sizeof WeaponDef);
		}

		WEAPON_READ_STRING(szOverlayName);
		WEAPON_READ_ASSET(ASSET_TYPE_XMODEL, model, handXModel);
		WEAPON_READ_ASSET(ASSET_TYPE_XMODEL, model, camoWorldModel);
		WEAPON_READ_ASSET(ASSET_TYPE_XMODEL, model, camoViewModel);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, camoWorldModelMaterialOverride);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, camoViewModelMaterialOverride);
		WEAPON_READ_STRING(szModeName);
		WEAPON_READ_FIELD(int, playerAnimType);
		WEAPON_READ_FIELD(int, weapType);
		WEAPON_READ_FIELD(int, weapClass);
		WEAPON_READ_FIELD(int, penetrateType);
		WEAPON_READ_FIELD(int, inventoryType);
		WEAPON_READ_FIELD(int, fireType);
		WEAPON_READ_FIELD(float, burstFireCooldown);
		WEAPON_READ_FIELD(int, offhandClass);
		WEAPON_READ_FIELD(int, stance);
		WEAPON_READ_FIELD(short, rattleSoundType);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, viewFlashEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, worldFlashEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, viewFlashADSEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, viewShellEjectEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, worldShellEjectEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, viewLastShotEjectEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, worldLastShotEjectEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, reticleCenter);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, reticleSide);
		WEAPON_READ_FIELD(int, iReticleCenterSize);
		WEAPON_READ_FIELD(int, iReticleSideSize);
		WEAPON_READ_FIELD(int, iReticleMinOfs);
		WEAPON_READ_FIELD(int, activeReticleType);
		WEAPON_READ_FIELD_ARR(float, vStandMove, 3);
		WEAPON_READ_FIELD_ARR(float, vStandRot, 3);
		WEAPON_READ_FIELD_ARR(float, strafeMove, 3);
		WEAPON_READ_FIELD_ARR(float, strafeRot, 3);
		WEAPON_READ_FIELD_ARR(float, vDuckedOfs, 3);
		WEAPON_READ_FIELD_ARR(float, vDuckedMove, 3);
		WEAPON_READ_FIELD_ARR(float, vDuckedRot, 3);
		WEAPON_READ_FIELD_ARR(float, vProneOfs, 3);
		WEAPON_READ_FIELD_ARR(float, vProneMove, 3);
		WEAPON_READ_FIELD_ARR(float, vProneRot, 3);
		WEAPON_READ_FIELD(float, fPosMoveRate);
		WEAPON_READ_FIELD(float, fPosProneMoveRate);
		WEAPON_READ_FIELD(float, fStandMoveMinSpeed);
		WEAPON_READ_FIELD(float, fDuckedMoveMinSpeed);
		WEAPON_READ_FIELD(float, fProneMoveMinSpeed);
		WEAPON_READ_FIELD(float, fPosRotRate);
		WEAPON_READ_FIELD(float, fPosProneRotRate);
		WEAPON_READ_FIELD(float, fStandRotMinSpeed);
		WEAPON_READ_FIELD(float, fDuckedRotMinSpeed);
		WEAPON_READ_FIELD(float, fProneRotMinSpeed);
		WEAPON_READ_ASSET(ASSET_TYPE_XMODEL, model, worldClipModel);
		WEAPON_READ_ASSET(ASSET_TYPE_XMODEL, model, rocketModel);
		WEAPON_READ_ASSET(ASSET_TYPE_XMODEL, model, knifeModel);
		WEAPON_READ_ASSET(ASSET_TYPE_XMODEL, model, worldKnifeModel);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, hudIcon);
		WEAPON_READ_FIELD(int, hudIconRatio);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, pickupIcon);
		WEAPON_READ_FIELD(int, pickupIconRatio);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, ammoCounterIcon);
		WEAPON_READ_FIELD(int, ammoCounterIconRatio);
		WEAPON_READ_FIELD(int, ammoCounterClip);
		WEAPON_READ_FIELD(int, iStartAmmo);
		WEAPON_READ_STRING(szAmmoName);
		WEAPON_READ_FIELD(int, iAmmoIndex);
		WEAPON_READ_STRING(szClipName);
		WEAPON_READ_FIELD(int, iClipIndex);
		WEAPON_READ_FIELD(int, iMaxAmmo);
		WEAPON_READ_FIELD(int, shotCount);
		WEAPON_READ_STRING(szSharedAmmoCapName);
		WEAPON_READ_FIELD(int, iSharedAmmoCapIndex);
		WEAPON_READ_FIELD(int, iSharedAmmoCap);
		WEAPON_READ_FIELD(int, damage);
		WEAPON_READ_FIELD(int, playerDamage);
		WEAPON_READ_FIELD(int, iMeleeDamage);
		WEAPON_READ_FIELD(int, iDamageType);
		parse_statetimers(&weapon->stateTimers, data["stateTimers"]);
		parse_statetimers(&weapon->akimboStateTimers, data["akimboStateTimers"]);
		WEAPON_READ_FIELD(float, autoAimRange);
		WEAPON_READ_FIELD(float, aimAssistRange);
		WEAPON_READ_FIELD(float, aimAssistRangeAds);
		WEAPON_READ_FIELD(float, aimPadding);
		WEAPON_READ_FIELD(float, enemyCrosshairRange);
		WEAPON_READ_FIELD(float, moveSpeedScale);
		WEAPON_READ_FIELD(float, adsMoveSpeedScale);
		WEAPON_READ_FIELD(float, sprintDurationScale);
		WEAPON_READ_FIELD(float, fAdsZoomInFrac);
		WEAPON_READ_FIELD(float, fAdsZoomOutFrac);
		parse_overlay(&weapon->overlay, data["overlay"]);
		WEAPON_READ_FIELD(int, overlayInterface);
		WEAPON_READ_FIELD(float, fAdsBobFactor);
		WEAPON_READ_FIELD(float, fAdsViewBobMult);
		WEAPON_READ_FIELD(float, fHipSpreadStandMin);
		WEAPON_READ_FIELD(float, fHipSpreadDuckedMin);
		WEAPON_READ_FIELD(float, fHipSpreadProneMin);
		WEAPON_READ_FIELD(float, hipSpreadStandMax);
		WEAPON_READ_FIELD(float, hipSpreadDuckedMax);
		WEAPON_READ_FIELD(float, hipSpreadProneMax);
		WEAPON_READ_FIELD(float, fHipSpreadDecayRate);
		WEAPON_READ_FIELD(float, fHipSpreadFireAdd);
		WEAPON_READ_FIELD(float, fHipSpreadTurnAdd);
		WEAPON_READ_FIELD(float, fHipSpreadMoveAdd);
		WEAPON_READ_FIELD(float, fHipSpreadDuckedDecay);
		WEAPON_READ_FIELD(float, fHipSpreadProneDecay);
		WEAPON_READ_FIELD(float, fHipReticleSidePos);
		WEAPON_READ_FIELD(float, fAdsIdleAmount);
		WEAPON_READ_FIELD(float, fHipIdleAmount);
		WEAPON_READ_FIELD(float, adsIdleSpeed);
		WEAPON_READ_FIELD(float, hipIdleSpeed);
		WEAPON_READ_FIELD(float, fIdleCrouchFactor);
		WEAPON_READ_FIELD(float, fIdleProneFactor);
		WEAPON_READ_FIELD(float, fGunMaxPitch);
		WEAPON_READ_FIELD(float, fGunMaxYaw);
		WEAPON_READ_FIELD(float, adsIdleLerpStartTime);
		WEAPON_READ_FIELD(float, adsIdleLerpTime);
		WEAPON_READ_FIELD(float, swayMaxAngleSteadyAim);
		WEAPON_READ_FIELD(float, swayMaxAngle);
		WEAPON_READ_FIELD(float, swayLerpSpeed);
		WEAPON_READ_FIELD(float, swayPitchScale);
		WEAPON_READ_FIELD(float, swayYawScale);
		WEAPON_READ_FIELD(float, swayHorizScale);
		WEAPON_READ_FIELD(float, swayVertScale);
		WEAPON_READ_FIELD(float, swayShellShockScale);
		WEAPON_READ_FIELD(float, adsSwayMaxAngle);
		WEAPON_READ_FIELD(float, adsSwayLerpSpeed);
		WEAPON_READ_FIELD(float, adsSwayPitchScale);
		WEAPON_READ_FIELD(float, adsSwayYawScale);
		WEAPON_READ_FIELD(float, adsSwayHorizScale);
		WEAPON_READ_FIELD(float, adsSwayVertScale);
		WEAPON_READ_FIELD(float, adsFireRateScale);
		WEAPON_READ_FIELD(float, adsDamageRangeScale);
		WEAPON_READ_FIELD(float, adsFireAnimFrac);
		//WEAPON_READ_ASSET(ASSET_TYPE_PHYSCOLLMAP, phys_collmap, physCollmap);
		WEAPON_READ_FIELD(float, dualWieldViewModelOffset);
		WEAPON_READ_FIELD(float, fScopeDriftDelay);
		WEAPON_READ_FIELD(float, fScopeDriftLerpInTime);
		WEAPON_READ_FIELD(float, fScopeDriftSteadyTime);
		WEAPON_READ_FIELD(float, fScopeDriftLerpOutTime);
		WEAPON_READ_FIELD(float, fScopeDriftSteadyFactor);
		WEAPON_READ_FIELD(float, fScopeDriftUnsteadyFactor);
		WEAPON_READ_FIELD(int, killIconRatio);
		WEAPON_READ_FIELD(int, iReloadAmmoAdd);
		WEAPON_READ_FIELD(int, iReloadStartAdd);
		WEAPON_READ_FIELD(int, ammoDropStockMin);
		WEAPON_READ_FIELD(int, ammoDropClipPercentMin);
		WEAPON_READ_FIELD(int, ammoDropClipPercentMax);
		WEAPON_READ_FIELD(int, iExplosionRadius);
		WEAPON_READ_FIELD(int, iExplosionRadiusMin);
		WEAPON_READ_FIELD(int, iExplosionInnerDamage);
		WEAPON_READ_FIELD(int, iExplosionOuterDamage);
		WEAPON_READ_FIELD(float, damageConeAngle);
		WEAPON_READ_FIELD(float, bulletExplDmgMult);
		WEAPON_READ_FIELD(float, bulletExplRadiusMult);
		WEAPON_READ_FIELD(int, iProjectileSpeed);
		WEAPON_READ_FIELD(int, iProjectileSpeedUp);
		WEAPON_READ_FIELD(int, iProjectileSpeedForward);
		WEAPON_READ_FIELD(int, iProjectileActivateDist);
		WEAPON_READ_FIELD(float, projLifetime);
		WEAPON_READ_FIELD(float, timeToAccelerate);
		WEAPON_READ_FIELD(float, projectileCurvature);
		WEAPON_READ_STRING(projectileName);
		WEAPON_READ_ASSET(ASSET_TYPE_XMODEL, model, projectileModel);
		WEAPON_READ_FIELD(int, projExplosion);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, projExplosionEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, projDudEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_SOUND, sound, projExplosionSound);
		WEAPON_READ_ASSET(ASSET_TYPE_SOUND, sound, projDudSound);
		WEAPON_READ_FIELD(int, stickiness);
		WEAPON_READ_FIELD(float, lowAmmoWarningThreshold);
		WEAPON_READ_FIELD(float, ricochetChance);
		WEAPON_READ_FIELD(bool, riotShieldEnableDamage);
		WEAPON_READ_FIELD(int, riotShieldHealth);
		WEAPON_READ_FIELD(float, riotShieldDamageMult);
		//WEAPON_READ_FIELD_ARR(float, parallelBounce, 0);
		//WEAPON_READ_FIELD_ARR(float, perpendicularBounce, 0);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, projTrailEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, projBeaconEffect);
		WEAPON_READ_FIELD(float, vProjectileColor[3]);
		WEAPON_READ_FIELD(int, guidedMissileType);
		WEAPON_READ_FIELD(float, maxSteeringAccel);
		WEAPON_READ_FIELD(int, projIgnitionDelay);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, projIgnitionEffect);
		WEAPON_READ_ASSET(ASSET_TYPE_SOUND, sound, projIgnitionSound);
		WEAPON_READ_FIELD(float, fAdsAimPitch);
		WEAPON_READ_FIELD(float, fAdsCrosshairInFrac);
		WEAPON_READ_FIELD(float, fAdsCrosshairOutFrac);
		WEAPON_READ_FIELD(bool, adsShouldShowCrosshair);
		WEAPON_READ_FIELD(int, adsGunKickReducedKickBullets);
		WEAPON_READ_FIELD(float, adsGunKickReducedKickPercent);
		WEAPON_READ_FIELD(float, fAdsGunKickPitchMin);
		WEAPON_READ_FIELD(float, fAdsGunKickPitchMax);
		WEAPON_READ_FIELD(float, fAdsGunKickYawMin);
		WEAPON_READ_FIELD(float, fAdsGunKickYawMax);
		WEAPON_READ_FIELD(float, fAdsGunKickMagMin);
		WEAPON_READ_FIELD(float, fAdsGunKickAccel);
		WEAPON_READ_FIELD(float, fAdsGunKickSpeedMax);
		WEAPON_READ_FIELD(float, fAdsGunKickSpeedDecay);
		WEAPON_READ_FIELD(float, fAdsGunKickStaticDecay);
		WEAPON_READ_FIELD(float, fAdsViewKickPitchMin);
		WEAPON_READ_FIELD(float, fAdsViewKickPitchMax);
		WEAPON_READ_FIELD(float, fAdsViewKickYawMin);
		WEAPON_READ_FIELD(float, fAdsViewKickYawMax);
		WEAPON_READ_FIELD(float, fAdsViewScatterMin);
		WEAPON_READ_FIELD(float, fAdsViewScatterMax);
		WEAPON_READ_FIELD(float, fAdsSpread);
		WEAPON_READ_FIELD(int, hipGunKickReducedKickBullets);
		WEAPON_READ_FIELD(float, hipGunKickReducedKickPercent);
		WEAPON_READ_FIELD(float, fHipGunKickPitchMin);
		WEAPON_READ_FIELD(float, fHipGunKickPitchMax);
		WEAPON_READ_FIELD(float, fHipGunKickYawMin);
		WEAPON_READ_FIELD(float, fHipGunKickYawMax);
		WEAPON_READ_FIELD(float, fHipGunKickMagMin);
		WEAPON_READ_FIELD(float, fHipGunKickAccel);
		WEAPON_READ_FIELD(float, fHipGunKickSpeedMax);
		WEAPON_READ_FIELD(float, fHipGunKickSpeedDecay);
		WEAPON_READ_FIELD(float, fHipGunKickStaticDecay);
		WEAPON_READ_FIELD(float, fHipViewKickPitchMin);
		WEAPON_READ_FIELD(float, fHipViewKickPitchMax);
		WEAPON_READ_FIELD(float, fHipViewKickYawMin);
		WEAPON_READ_FIELD(float, fHipViewKickYawMax);
		WEAPON_READ_FIELD(float, fHipViewKickMagMin);
		WEAPON_READ_FIELD(float, fHipViewScatterMin);
		WEAPON_READ_FIELD(float, fHipViewScatterMax);
		WEAPON_READ_FIELD(float, fightDist);
		WEAPON_READ_FIELD(float, maxDist);
		//WEAPON_READ_STRING(accuracyGraphName[2]);
		//WEAPON_READ_FIELD(float, originalAccuracyGraphKnots);
		//WEAPON_READ_FIELD(short, originalAccuracyGraphKnotCount);
		WEAPON_READ_FIELD(int, iPositionReloadTransTime);
		WEAPON_READ_FIELD(float, leftArc);
		WEAPON_READ_FIELD(float, rightArc);
		WEAPON_READ_FIELD(float, topArc);
		WEAPON_READ_FIELD(float, bottomArc);
		WEAPON_READ_FIELD(float, accuracy);
		WEAPON_READ_FIELD(float, aiSpread);
		WEAPON_READ_FIELD(float, playerSpread);
		WEAPON_READ_FIELD(float, minTurnSpeed[2]);
		WEAPON_READ_FIELD(float, maxTurnSpeed[2]);
		WEAPON_READ_FIELD(float, pitchConvergenceTime);
		WEAPON_READ_FIELD(float, yawConvergenceTime);
		WEAPON_READ_FIELD(float, suppressTime);
		WEAPON_READ_FIELD(float, maxRange);
		WEAPON_READ_FIELD(float, fAnimHorRotateInc);
		WEAPON_READ_FIELD(float, fPlayerPositionDist);
		WEAPON_READ_STRING(szUseHintString);
		WEAPON_READ_STRING(dropHintString);
		WEAPON_READ_FIELD(int, iUseHintStringIndex);
		WEAPON_READ_FIELD(int, dropHintStringIndex);
		WEAPON_READ_FIELD(float, horizViewJitter);
		WEAPON_READ_FIELD(float, vertViewJitter);
		WEAPON_READ_FIELD(float, scanSpeed);
		WEAPON_READ_FIELD(float, scanAccel);
		WEAPON_READ_FIELD(int, scanPauseTime);
		WEAPON_READ_STRING(szScript);
		WEAPON_READ_FIELD_ARR(float, fOOPosAnimLength, 2);
		WEAPON_READ_FIELD(int, minDamage);
		WEAPON_READ_FIELD(int, minPlayerDamage);
		WEAPON_READ_FIELD(float, fMaxDamageRange);
		WEAPON_READ_FIELD(float, fMinDamageRange);
		WEAPON_READ_FIELD(float, destabilizationRateTime);
		WEAPON_READ_FIELD(float, destabilizationCurvatureMax);
		WEAPON_READ_FIELD(int, destabilizeDistance);
		WEAPON_READ_FIELD_ARR(float, locationDamageMultipliers, 22);
		WEAPON_READ_STRING(fireRumble);
		WEAPON_READ_STRING(meleeImpactRumble);
		WEAPON_READ_ASSET(ASSET_TYPE_TRACER, tracerDef, tracerType);
		WEAPON_READ_FIELD(bool, turretADSEnabled);
		WEAPON_READ_FIELD(float, turretADSTime);
		WEAPON_READ_FIELD(float, turretFov);
		WEAPON_READ_FIELD(float, turretFovADS);
		WEAPON_READ_FIELD(float, turretScopeZoomRate);
		WEAPON_READ_FIELD(float, turretScopeZoomMin);
		WEAPON_READ_FIELD(float, turretScopeZoomMax);
		WEAPON_READ_FIELD(float, turretOverheatUpRate);
		WEAPON_READ_FIELD(float, turretOverheatDownRate);
		WEAPON_READ_FIELD(float, turretOverheatPenalty);
		WEAPON_READ_ASSET(ASSET_TYPE_SOUND, sound, turretOverheatSound);
		WEAPON_READ_ASSET(ASSET_TYPE_FX, fx, turretOverheatEffect);
		WEAPON_READ_STRING(turretBarrelSpinRumble);
		WEAPON_READ_FIELD(float, turretBarrelSpinSpeed);
		WEAPON_READ_FIELD(float, turretBarrelSpinUpTime);
		WEAPON_READ_FIELD(float, turretBarrelSpinDownTime);
		WEAPON_READ_ASSET(ASSET_TYPE_SOUND, sound, turretBarrelSpinMaxSnd);
		WEAPON_READ_ASSET(ASSET_TYPE_SOUND, sound, missileConeSoundAlias);
		WEAPON_READ_ASSET(ASSET_TYPE_SOUND, sound, missileConeSoundAliasAtBase);
		WEAPON_READ_FIELD(float, missileConeSoundRadiusAtTop);
		WEAPON_READ_FIELD(float, missileConeSoundRadiusAtBase);
		WEAPON_READ_FIELD(float, missileConeSoundHeight);
		WEAPON_READ_FIELD(float, missileConeSoundOriginOffset);
		WEAPON_READ_FIELD(float, missileConeSoundVolumescaleAtCore);
		WEAPON_READ_FIELD(float, missileConeSoundVolumescaleAtEdge);
		WEAPON_READ_FIELD(float, missileConeSoundVolumescaleCoreSize);
		WEAPON_READ_FIELD(float, missileConeSoundPitchAtTop);
		WEAPON_READ_FIELD(float, missileConeSoundPitchAtBottom);
		WEAPON_READ_FIELD(float, missileConeSoundPitchTopSize);
		WEAPON_READ_FIELD(float, missileConeSoundPitchBottomSize);
		WEAPON_READ_FIELD(float, missileConeSoundCrossfadeTopSize);
		WEAPON_READ_FIELD(float, missileConeSoundCrossfadeBottomSize);

		// parse knifeAttachTagOverride
		if (!data["knifeAttachTagOverride"].is_null())
		{
			auto knifeAttachTagOverride = data["knifeAttachTagOverride"].get<std::string>();
			this->add_script_string(&weapon->knifeAttachTagOverride, mem->duplicate_string(knifeAttachTagOverride));
		}

		WEAPON_READ_FIELD(bool, knifeAlwaysAttached);
		WEAPON_READ_FIELD(bool, meleeOverrideValues);
		WEAPON_READ_FIELD(float, aim_automelee_lerp);
		WEAPON_READ_FIELD(float, aim_automelee_range);
		WEAPON_READ_FIELD(float, aim_automelee_region_height);
		WEAPON_READ_FIELD(float, aim_automelee_region_width);
		WEAPON_READ_FIELD(float, player_meleeHeight);
		WEAPON_READ_FIELD(float, player_meleeRange);
		WEAPON_READ_FIELD(float, player_meleeWidth);
		WEAPON_READ_FIELD(bool, sharedAmmo);
		WEAPON_READ_FIELD(bool, lockonSupported);
		WEAPON_READ_FIELD(bool, requireLockonToFire);
		WEAPON_READ_FIELD(bool, isAirburstWeapon);
		WEAPON_READ_FIELD(bool, bigExplosion);
		WEAPON_READ_FIELD(bool, noAdsWhenMagEmpty);
		WEAPON_READ_FIELD(bool, avoidDropCleanup);
		WEAPON_READ_FIELD(bool, inheritsPerks);
		WEAPON_READ_FIELD(bool, crosshairColorChange);
		WEAPON_READ_FIELD(bool, bRifleBullet);
		WEAPON_READ_FIELD(bool, armorPiercing);
		WEAPON_READ_FIELD(bool, bBoltAction);
		WEAPON_READ_FIELD(bool, aimDownSight);
		WEAPON_READ_FIELD(bool, canHoldBreath);
		WEAPON_READ_FIELD(bool, canVariableZoom);
		WEAPON_READ_FIELD(bool, bRechamberWhileAds);
		WEAPON_READ_FIELD(bool, bBulletExplosiveDamage);
		WEAPON_READ_FIELD(bool, bCookOffHold);
		WEAPON_READ_FIELD(bool, reticleSpin45);
		WEAPON_READ_FIELD(bool, bClipOnly);
		WEAPON_READ_FIELD(bool, noAmmoPickup);
		WEAPON_READ_FIELD(bool, adsFireOnly);
		WEAPON_READ_FIELD(bool, cancelAutoHolsterWhenEmpty);
		WEAPON_READ_FIELD(bool, disableSwitchToWhenEmpty);
		WEAPON_READ_FIELD(bool, suppressAmmoReserveDisplay);
		WEAPON_READ_FIELD(bool, laserSightDuringNightvision);
		WEAPON_READ_FIELD(bool, markableViewmodel);
		WEAPON_READ_FIELD(bool, noDualWield);
		WEAPON_READ_FIELD(bool, flipKillIcon);
		WEAPON_READ_FIELD(bool, bNoPartialReload);
		WEAPON_READ_FIELD(bool, bSegmentedReload);
		WEAPON_READ_FIELD(bool, bMultipleReload);
		WEAPON_READ_FIELD(bool, blocksProne);
		WEAPON_READ_FIELD(bool, silenced);
		WEAPON_READ_FIELD(bool, isRollingGrenade);
		WEAPON_READ_FIELD(bool, projExplosionEffectForceNormalUp);
		WEAPON_READ_FIELD(bool, projExplosionEffectInheritParentDirection);
		WEAPON_READ_FIELD(bool, bProjImpactExplode);
		WEAPON_READ_FIELD(bool, bProjTrajectoryEvents);
		WEAPON_READ_FIELD(bool, bProjWhizByEnabled);
		WEAPON_READ_FIELD(bool, stickToPlayers);
		WEAPON_READ_FIELD(bool, stickToVehicles);
		WEAPON_READ_FIELD(bool, stickToTurrets);
		WEAPON_READ_FIELD(bool, thrownSideways);
		WEAPON_READ_FIELD(bool, hasDetonator);
		WEAPON_READ_FIELD(bool, disableFiring);
		WEAPON_READ_FIELD(bool, timedDetonation);
		WEAPON_READ_FIELD(bool, rotate);
		WEAPON_READ_FIELD(bool, holdButtonToThrow);
		WEAPON_READ_FIELD(bool, freezeMovementWhenFiring);
		WEAPON_READ_FIELD(bool, thermalScope);
		WEAPON_READ_FIELD(bool, thermalToggle);
		WEAPON_READ_FIELD(bool, outlineEnemies);
		WEAPON_READ_FIELD(bool, altModeSameWeapon);
		WEAPON_READ_FIELD(bool, turretBarrelSpinEnabled);
		WEAPON_READ_FIELD(bool, missileConeSoundEnabled);
		WEAPON_READ_FIELD(bool, missileConeSoundPitchshiftEnabled);
		WEAPON_READ_FIELD(bool, missileConeSoundCrossfadeEnabled);
		WEAPON_READ_FIELD(bool, offhandHoldIsCancelable);
		WEAPON_READ_FIELD(bool, doNotAllowAttachmentsToOverrideSpread);
		WEAPON_READ_FIELD(bool, useFastReloadAnims);
		WEAPON_READ_FIELD(bool, useScopeDrift);
		WEAPON_READ_FIELD(bool, alwaysShatterGlassOnImpact);
		WEAPON_READ_FIELD(bool, oldWeapon);
		WEAPON_READ_ASSET(ASSET_TYPE_XMODEL, model, stowOffsetModel);

		// parse stowtag
		if (!data["stowTag"].is_null())
		{
			auto stowTag = data["stowTag"].get<std::string>();
			this->add_script_string(&weapon->stowTag, mem->duplicate_string(stowTag));
		}

		weapon->accuracyGraphName[0] = nullptr;
		weapon->accuracyGraphName[1] = nullptr;
		weapon->originalAccuracyGraphKnots[0] = nullptr;
		weapon->originalAccuracyGraphKnots[1] = nullptr;
		weapon->originalAccuracyGraphKnotCount[0] = 0;
		weapon->originalAccuracyGraphKnotCount[1] = 0;
		weapon->parallelBounce = nullptr;
		weapon->perpendicularBounce = nullptr;

		WEAPON_READ_ASSET_ARR(ASSET_TYPE_XMODEL, model, gunXModel, XModel, 64);
		WEAPON_READ_ASSET_ARR(ASSET_TYPE_XMODEL, model, worldModel, XModel, 64);
		WEAPON_READ_ASSET_ARR(ASSET_TYPE_MATERIAL, material, camoMaterialTarget, Material, 63);
		WEAPON_READ_ANIM_ARR(szXAnimsRightHanded, 124);
		WEAPON_READ_ANIM_ARR(szXAnimsLeftHanded, 124);

		weapon->notetrackSoundMapKeys = mem->allocate<scr_string_t>(24);
		weapon->notetrackSoundMapValues = mem->allocate<scr_string_t>(24);
		for (auto i = 0u; i < 24; i++)
		{
			auto notetrack = data["notetrackSoundMapKeys"][i].get<std::string>();
			this->add_script_string(&weapon->notetrackSoundMapKeys[i], mem->duplicate_string(notetrack));
		}
		for (auto i = 0u; i < 24; i++)
		{
			auto notetrack = data["notetrackSoundMapValues"][i].get<std::string>();
			this->add_script_string(&weapon->notetrackSoundMapValues[i], mem->duplicate_string(notetrack));
		}

		weapon->notetrackRumbleMapKeys = mem->allocate<scr_string_t>(16);
		weapon->notetrackRumbleMapValues = mem->allocate<scr_string_t>(16);
		weapon->notetrackFXMapKeys = mem->allocate<scr_string_t>(16);
		weapon->notetrackFXMapTagValues = mem->allocate<scr_string_t>(16);
		for (auto i = 0u; i < 16; i++)
		{
			auto notetrack = data["notetrackRumbleMapKeys"][i].get<std::string>();
			this->add_script_string(&weapon->notetrackRumbleMapKeys[i], mem->duplicate_string(notetrack));
		}
		for (auto i = 0u; i < 16; i++)
		{
			auto notetrack = data["notetrackRumbleMapValues"][i].get<std::string>();
			this->add_script_string(&weapon->notetrackRumbleMapValues[i], mem->duplicate_string(notetrack));
		}
		for (auto i = 0u; i < 16; i++)
		{
			auto notetrack = data["notetrackFXMapKeys"][i].get<std::string>();
			this->add_script_string(&weapon->notetrackFXMapKeys[i], mem->duplicate_string(notetrack));
		}
		for (auto i = 0u; i < 16; i++)
		{
			auto notetrack = data["notetrackFXMapTagValues"][i].get<std::string>();
			this->add_script_string(&weapon->notetrackFXMapTagValues[i], mem->duplicate_string(notetrack));
		}

		weapon->notetrackFXMapValues = mem->allocate<FxEffectDef*>(16);
		for (auto i = 0u; i < 16; i++)
		{
			auto notetrack = data["notetrackFXMapValues"][i].get<std::string>();
			weapon->notetrackFXMapValues[i] = db_find_x_asset_header(ASSET_TYPE_FX, notetrack.data(), 1).fx;
		}

		WEAPON_READ_SOUND(pickupSound);
		WEAPON_READ_SOUND(pickupSoundPlayer);
		WEAPON_READ_SOUND(ammoPickupSound);
		WEAPON_READ_SOUND(ammoPickupSoundPlayer);
		WEAPON_READ_SOUND(projectileSound);
		WEAPON_READ_SOUND(pullbackSound);
		WEAPON_READ_SOUND(pullbackSoundPlayer);
		WEAPON_READ_SOUND(fireSound);
		WEAPON_READ_SOUND(fireSoundPlayer);
		WEAPON_READ_SOUND(fireSoundPlayerAkimbo);
		WEAPON_READ_SOUND(fireLoopSound);
		WEAPON_READ_SOUND(fireLoopSoundPlayer);
		WEAPON_READ_SOUND(fireStopSound);
		WEAPON_READ_SOUND(fireStopSoundPlayer);
		WEAPON_READ_SOUND(fireLastSound);
		WEAPON_READ_SOUND(fireLastSoundPlayer);
		WEAPON_READ_SOUND(emptyFireSound);
		WEAPON_READ_SOUND(emptyFireSoundPlayer);
		WEAPON_READ_SOUND(meleeSwipeSound);
		WEAPON_READ_SOUND(meleeSwipeSoundPlayer);
		WEAPON_READ_SOUND(meleeHitSound);
		WEAPON_READ_SOUND(meleeMissSound);
		WEAPON_READ_SOUND(rechamberSound);
		WEAPON_READ_SOUND(rechamberSoundPlayer);
		WEAPON_READ_SOUND(reloadSound);
		WEAPON_READ_SOUND(reloadSoundPlayer);
		WEAPON_READ_SOUND(reloadEmptySound);
		WEAPON_READ_SOUND(reloadEmptySoundPlayer);
		WEAPON_READ_SOUND(reloadStartSound);
		WEAPON_READ_SOUND(reloadStartSoundPlayer);
		WEAPON_READ_SOUND(reloadEndSound);
		WEAPON_READ_SOUND(reloadEndSoundPlayer);
		WEAPON_READ_SOUND(detonateSound);
		WEAPON_READ_SOUND(detonateSoundPlayer);
		WEAPON_READ_SOUND(nightVisionWearSound);
		WEAPON_READ_SOUND(nightVisionWearSoundPlayer);
		WEAPON_READ_SOUND(nightVisionRemoveSound);
		WEAPON_READ_SOUND(nightVisionRemoveSoundPlayer);
		WEAPON_READ_SOUND(altSwitchSound);
		WEAPON_READ_SOUND(altSwitchSoundPlayer);
		WEAPON_READ_SOUND(raiseSound);
		WEAPON_READ_SOUND(raiseSoundPlayer);
		WEAPON_READ_SOUND(firstRaiseSound);
		WEAPON_READ_SOUND(firstRaiseSoundPlayer);
		WEAPON_READ_SOUND(putawaySound);
		WEAPON_READ_SOUND(putawaySoundPlayer);
		WEAPON_READ_SOUND(scanSound);
		WEAPON_READ_SOUND(changeVariableZoomSound);

		WEAPON_READ_ASSET_ARR(ASSET_TYPE_SOUND, sound, bounceSound, snd_alias_list_t, 31);
		WEAPON_READ_ASSET_ARR(ASSET_TYPE_SOUND, sound, rollingSound, snd_alias_list_t, 31);

		for (auto i = 0u; i < 4; i++)
		{
			auto sound = data["turretBarrelSpinUpSnd"][i].get<std::string>();
			if (!sound.empty())
			{
				weapon->turretBarrelSpinUpSnd[i] = db_find_x_asset_header(ASSET_TYPE_SOUND, sound.data(), 1).sound;
			}
		}
		for (auto i = 0u; i < 4; i++)
		{
			auto sound = data["turretBarrelSpinDownSnd"][i].get<std::string>();
			if (!sound.empty())
			{
				weapon->turretBarrelSpinDownSnd[i] = db_find_x_asset_header(ASSET_TYPE_SOUND, sound.data(), 1).sound;
			}
		}

		return weapon;
	}

	WeaponCompleteDef* weapon_def::parse(const std::string& name, zone_memory* mem)
	{
		auto path = "weapons\\" + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing weapon \"%s\"...", name.data());

		auto weapon = mem->allocate<WeaponCompleteDef>();

		// parse json file
		file.open("rb");
		auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();
		json data = json::parse(bytes);

		// base asset
		auto base = data["baseAsset"].get<std::string>();
		WeaponCompleteDef* baseAsset = nullptr;

		if (!base.empty())
		{
			baseAsset = db_find_x_asset_header(ASSET_TYPE_WEAPON, base.data(), 1).weapon;
			memcpy(weapon, baseAsset, sizeof WeaponCompleteDef);
		}
		else
		{
			ZONETOOL_WARNING("No base asset is defined for weapon %s, stuff might go wrong!", name.data());
		}

		WEAPON_READ_STRING(szInternalName);
		WEAPON_READ_STRING(szDisplayName);
		WEAPON_READ_FIELD(unsigned int, numAnimOverrides);
		WEAPON_READ_FIELD(unsigned int, numSoundOverrides);
		WEAPON_READ_FIELD(unsigned int, numFXOverrides);
		WEAPON_READ_FIELD(unsigned int, numReloadStateTimerOverrides);
		WEAPON_READ_FIELD(unsigned int, numNotetrackOverrides);
		WEAPON_READ_FIELD(float, fAdsZoomFov);
		WEAPON_READ_FIELD(int, iAdsTransInTime);
		WEAPON_READ_FIELD(int, iAdsTransOutTime);
		WEAPON_READ_FIELD(int, iClipSize);
		WEAPON_READ_FIELD(int, impactType);
		WEAPON_READ_FIELD(int, iFireTime);
		WEAPON_READ_FIELD(int, iFireTimeAkimbo);
		WEAPON_READ_FIELD(int, dpadIconRatio);
		WEAPON_READ_FIELD(float, penetrateMultiplier);
		WEAPON_READ_FIELD(float, fAdsViewKickCenterSpeed);
		WEAPON_READ_FIELD(float, fHipViewKickCenterSpeed);
		WEAPON_READ_STRING(szAltWeaponName);
		WEAPON_READ_FIELD(int, altWeapon);
		WEAPON_READ_FIELD(int, iAltRaiseTime);
		WEAPON_READ_FIELD(int, iAltRaiseTimeAkimbo);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, killIcon);
		WEAPON_READ_ASSET(ASSET_TYPE_MATERIAL, material, dpadIcon);
		WEAPON_READ_FIELD(int, fireAnimLength);
		WEAPON_READ_FIELD(int, fireAnimLengthAkimbo);
		WEAPON_READ_FIELD(int, iFirstRaiseTime);
		WEAPON_READ_FIELD(int, iFirstRaiseTimeAkimbo);
		WEAPON_READ_FIELD(int, ammoDropStockMax);
		WEAPON_READ_FIELD(float, adsDofStart);
		WEAPON_READ_FIELD(float, adsDofEnd);
		WEAPON_READ_FIELD_ARR(unsigned __int16, accuracyGraphKnotCount, 2);
		WEAPON_READ_FIELD(bool, motionTracker);
		WEAPON_READ_FIELD(bool, enhanced);
		WEAPON_READ_FIELD(bool, dpadIconShowsAmmo);
		WEAPON_READ_STRING(szAdsrBaseSetting);

		// parse weapondef
		weapon->weapDef = parse_weapondef(data["weapDef"], baseAsset, mem);

		weapon->hideTags = mem->allocate<scr_string_t>(32);
		for (auto i = 0u; i < 32; i++)
		{
			auto script_str = data["hideTags"][i].get<std::string>();
			this->add_script_string(&weapon->hideTags[i], mem->duplicate_string(script_str));
		}

		WEAPON_READ_ASSET_ARR(ASSET_TYPE_ATTACHMENT, attachment, scopes, WeaponAttachment, 6);
		WEAPON_READ_ASSET_ARR(ASSET_TYPE_ATTACHMENT, attachment, underBarrels, WeaponAttachment, 3);
		WEAPON_READ_ASSET_ARR(ASSET_TYPE_ATTACHMENT, attachment, others, WeaponAttachment, 8);

		WEAPON_READ_ANIM_ARR(szXAnims, 124);

		if (weapon->numAnimOverrides)
		{
			weapon->animOverrides = mem->allocate<AnimOverrideEntry>(weapon->numAnimOverrides);
			for (auto i = 0u; i < weapon->numAnimOverrides; i++)
			{
				auto altmodeAnim = data["animOverrides"][i]["altmodeAnim"].get<std::string>();
				if (!altmodeAnim.empty())
				{
					weapon->animOverrides[i].altmodeAnim = db_find_x_asset_header(ASSET_TYPE_XANIMPARTS, altmodeAnim.data(), 1).parts;
				}
				auto overrideAnim = data["animOverrides"][i]["overrideAnim"].get<std::string>();
				if (!overrideAnim.empty())
				{
					weapon->animOverrides[i].overrideAnim = db_find_x_asset_header(ASSET_TYPE_XANIMPARTS, overrideAnim.data(), 1).parts;
				}
				weapon->animOverrides[i].attachment1 = data["animOverrides"][i]["attachment1"].get<unsigned short>();
				weapon->animOverrides[i].attachment2 = data["animOverrides"][i]["attachment2"].get<unsigned short>();
				weapon->animOverrides[i].altTime = data["animOverrides"][i]["altTime"].get<int>();
				weapon->animOverrides[i].animTime = data["animOverrides"][i]["animTime"].get<int>();
				weapon->animOverrides[i].animTreeType = data["animOverrides"][i]["animTreeType"].get<unsigned int>();
			}
		}

		if (weapon->numSoundOverrides)
		{
			weapon->soundOverrides = mem->allocate<SoundOverrideEntry>(weapon->numSoundOverrides);
			for (auto i = 0u; i < weapon->numSoundOverrides; i++)
			{
				auto altmodeSound = data["soundOverrides"][i]["altmodeSound"].get<std::string>();
				if (!altmodeSound.empty())
				{
					weapon->soundOverrides[i].altmodeSound = db_find_x_asset_header(ASSET_TYPE_SOUND, altmodeSound.data(), 1).sound;
				}
				auto overrideSound = data["soundOverrides"][i]["overrideSound"].get<std::string>();
				if (!overrideSound.empty())
				{
					weapon->soundOverrides[i].overrideSound = db_find_x_asset_header(ASSET_TYPE_SOUND, overrideSound.data(), 1).sound;
				}
				weapon->soundOverrides[i].attachment1 = data["soundOverrides"][i]["attachment1"].get<unsigned short>();
				weapon->soundOverrides[i].attachment2 = data["soundOverrides"][i]["attachment2"].get<unsigned short>();
				weapon->soundOverrides[i].soundType = data["soundOverrides"][i]["soundType"].get<unsigned int>();
			}
		}

		if (weapon->numFXOverrides)
		{
			weapon->fxOverrides = mem->allocate<FXOverrideEntry>(weapon->numFXOverrides);
			for (auto i = 0u; i < weapon->numFXOverrides; i++)
			{
				auto altmodeFX = data["fxOverrides"][i]["altmodeFX"].get<std::string>();
				if (!altmodeFX.empty())
				{
					weapon->fxOverrides[i].altmodeFX = db_find_x_asset_header(ASSET_TYPE_FX, altmodeFX.data(), 1).fx;
				}
				auto overrideFX = data["fxOverrides"][i]["overrideFX"].get<std::string>();
				if (!overrideFX.empty())
				{
					weapon->fxOverrides[i].overrideFX = db_find_x_asset_header(ASSET_TYPE_SOUND, overrideFX.data(), 1).fx;
				}
				weapon->fxOverrides[i].attachment1 = data["fxOverrides"][i]["attachment1"].get<unsigned short>();
				weapon->fxOverrides[i].attachment2 = data["fxOverrides"][i]["attachment2"].get<unsigned short>();
				weapon->fxOverrides[i].fxType = data["fxOverrides"][i]["fxType"].get<unsigned int>();
			}
		}

		if (weapon->numReloadStateTimerOverrides)
		{
			weapon->reloadOverrides = mem->allocate<ReloadStateTimerEntry>(weapon->numReloadStateTimerOverrides);
			for (auto i = 0u; i < weapon->numReloadStateTimerOverrides; i++)
			{
				weapon->reloadOverrides[i].attachment = data["reloadOverrides"][i]["attachment"].get<int>();
				weapon->reloadOverrides[i].reloadAddTime = data["reloadOverrides"][i]["reloadAddTime"].get<int>();
				weapon->reloadOverrides[i].reloadEmptyAddTime = data["reloadOverrides"][i]["reloadEmptyAddTime"].get<int>();
				weapon->reloadOverrides[i].reloadStartAddTime = data["reloadOverrides"][i]["reloadStartAddTime"].get<int>();
			}
		}

		if (weapon->numNotetrackOverrides)
		{
			weapon->notetrackOverrides = mem->allocate<NoteTrackToSoundEntry>(weapon->numNotetrackOverrides);
			for (auto i = 0u; i < weapon->numNotetrackOverrides; i++)
			{
				weapon->notetrackOverrides[i].attachment = data["notetrackOverrides"][i]["attachment"].get<int>();
				for (auto j = 0u; j < 24; j++)
				{
					auto notetrackSoundMapKey = data["notetrackOverrides"][i]["notetrackSoundMapKeys"][j].get<std::string>();
					if (!notetrackSoundMapKey.empty())
					{
						this->add_script_string(&weapon->notetrackOverrides[i].notetrackSoundMapKeys[j], mem->duplicate_string(notetrackSoundMapKey));
					}
					auto notetrackSoundMapValue = data["notetrackOverrides"][i]["notetrackSoundMapValues"][j].get<std::string>();
					if (!notetrackSoundMapValue.empty())
					{
						this->add_script_string(&weapon->notetrackOverrides[i].notetrackSoundMapValues[j], mem->duplicate_string(notetrackSoundMapValue));
					}
				}
			}
		}

		return weapon;
	}

	void weapon_def::init(const std::string& name, zone_memory* mem)
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
			ZONETOOL_FATAL("Weapon file \"%s\" not found.", name.data());
		}
	}

#define WEAPON_SCRIPTSTRING_ARRAY_PREPARE(__field__,__count__) \
		if (weapon->__field__) \
		{ \
			for (int nt = 0; nt < __count__; nt++) \
			{ \
				weapon->__field__[nt] = static_cast<scr_string_t>(buf->write_scriptstring(this->get_script_string(&weapon->__field__[nt]))); \
			} \
		}

	void weapon_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* weapon = this->asset_;

		if (weapon->hideTags)
		{
			for (int i = 0; i < 32; i++)
			{
				weapon->hideTags[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&weapon->hideTags[i])));
			}
		}

		if (weapon->notetrackOverrides)
		{
			for (auto i = 0u; i < weapon->numNotetrackOverrides; i++)
			{
				WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackOverrides[i].notetrackSoundMapKeys, 24);
				WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackOverrides[i].notetrackSoundMapValues, 24);
			}
		}

		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(weapDef->notetrackSoundMapKeys, 24);
		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(weapDef->notetrackSoundMapValues, 24);
		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(weapDef->notetrackRumbleMapKeys, 16);
		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(weapDef->notetrackRumbleMapValues, 16);
		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(weapDef->notetrackFXMapKeys, 16);
		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(weapDef->notetrackFXMapTagValues, 16);

		weapon->weapDef->knifeAttachTagOverride = static_cast<scr_string_t>(buf->write_scriptstring(
			this->get_script_string(&weapon->weapDef->knifeAttachTagOverride)));

		weapon->weapDef->stowTag = static_cast<scr_string_t>(buf->write_scriptstring(
			this->get_script_string(&weapon->weapDef->stowTag)));
	}

#define WEAPON_SUBASSET_DEPENDING(__field__,__type__,__struct__) \
		if (data->__field__) \
		{ \
			zone->add_asset_of_type(__type__, data->__field__->name); \
		}

	void weapon_def::load_depending_weapondef(zone_base* zone, WeaponDef* data)
	{
		for (auto i = 0u; i < 64; i++)
		{
			if (data->worldModel && data->worldModel[i])
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->worldModel[i]->name);
			}
			if (data->gunXModel && data->gunXModel[i])
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->gunXModel[i]->name);
			}
		}

		if (data->handXModel)
		{
			zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->handXModel->name);
		}

		if (data->camoWorldModel)
		{
			zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->camoWorldModel->name);
		}

		if (data->camoViewModel)
		{
			zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->camoViewModel->name);
		}

		if (data->camoWorldModelMaterialOverride)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->camoViewModelMaterialOverride->name);
		}

		if (data->camoWorldModelMaterialOverride)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->camoWorldModelMaterialOverride->name);
		}

		for (auto i = 0u; i < 63; i++)
		{
			if (data->camoMaterialTarget && data->camoMaterialTarget[i])
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->camoMaterialTarget[i]->name);
			}
		}

		for (auto i = 0u; i < NUM_WEAP_ANIMS; i++)
		{
			if (data->szXAnimsRightHanded && data->szXAnimsRightHanded[i])
			{
				zone->add_asset_of_type(ASSET_TYPE_XANIMPARTS, data->szXAnimsRightHanded[i]->name);
			}
			if (data->szXAnimsLeftHanded && data->szXAnimsLeftHanded[i])
			{
				zone->add_asset_of_type(ASSET_TYPE_XANIMPARTS, data->szXAnimsLeftHanded[i]->name);
			}
		}

		for (auto i = 0; i < 24; i++)
		{
			auto str = this->get_script_string(&data->notetrackSoundMapValues[i]);
			if (str != nullptr)
			{
				zone->add_asset_of_type(ASSET_TYPE_SOUND, str);
			}
		}

		for (auto i = 0u; i < 16; i++)
		{
			if (data->notetrackFXMapValues && data->notetrackFXMapValues[i])
			{
				zone->add_asset_of_type(ASSET_TYPE_FX, data->notetrackFXMapValues[i]->name);
			}
		}

		WEAPON_SUBASSET_DEPENDING(viewFlashEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET_DEPENDING(worldFlashEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET_DEPENDING(viewFlashADSEffect, ASSET_TYPE_FX, FxEffectDef);

		WEAPON_SUBASSET_DEPENDING(pickupSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(pickupSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(ammoPickupSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(ammoPickupSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(projectileSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(pullbackSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(pullbackSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(fireSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(fireSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(fireSoundPlayerAkimbo, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(fireLoopSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(fireLoopSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(fireStopSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(fireStopSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(fireLastSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(fireLastSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(emptyFireSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(emptyFireSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(meleeSwipeSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(meleeSwipeSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(meleeHitSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(meleeMissSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(rechamberSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(rechamberSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(reloadSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(reloadSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(reloadEmptySound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(reloadEmptySoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(reloadStartSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(reloadStartSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(reloadEndSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(reloadEndSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(detonateSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(detonateSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(nightVisionWearSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(nightVisionWearSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(nightVisionRemoveSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(nightVisionRemoveSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(altSwitchSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(altSwitchSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(raiseSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(raiseSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(firstRaiseSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(firstRaiseSoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(putawaySound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(putawaySoundPlayer, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(scanSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(changeVariableZoomSound, ASSET_TYPE_SOUND, snd_alias_list_t);

		if (data->bounceSound)
		{
			for (auto i = 0u; i < 31; i++)
			{
				if (data->bounceSound[i])
				{
					zone->add_asset_of_type(ASSET_TYPE_SOUND, data->bounceSound[i]->name);
				}
			}
		}

		if (data->rollingSound)
		{
			for (auto i = 0u; i < 31; i++)
			{
				if (data->rollingSound[i])
				{
					zone->add_asset_of_type(ASSET_TYPE_SOUND, data->rollingSound[i]->name);
				}
			}
		}

		WEAPON_SUBASSET_DEPENDING(viewShellEjectEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET_DEPENDING(worldShellEjectEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET_DEPENDING(viewLastShotEjectEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET_DEPENDING(worldLastShotEjectEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET_DEPENDING(viewMagEjectEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET_DEPENDING(reticleCenter, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET_DEPENDING(reticleSide, ASSET_TYPE_MATERIAL, Material);

		WEAPON_SUBASSET_DEPENDING(worldClipModel, ASSET_TYPE_XMODEL, XModel);
		WEAPON_SUBASSET_DEPENDING(rocketModel, ASSET_TYPE_XMODEL, XModel);
		WEAPON_SUBASSET_DEPENDING(knifeModel, ASSET_TYPE_XMODEL, XModel);
		WEAPON_SUBASSET_DEPENDING(worldKnifeModel, ASSET_TYPE_XMODEL, XModel);

		WEAPON_SUBASSET_DEPENDING(hudIcon, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET_DEPENDING(pickupIcon, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET_DEPENDING(ammoCounterIcon, ASSET_TYPE_MATERIAL, Material);

		WEAPON_SUBASSET_DEPENDING(overlay.shader, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET_DEPENDING(overlay.shaderLowRes, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET_DEPENDING(overlay.shaderEMP, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET_DEPENDING(overlay.shaderEMPLowRes, ASSET_TYPE_MATERIAL, Material);

		WEAPON_SUBASSET_DEPENDING(physCollmap, ASSET_TYPE_PHYSCOLLMAP, PhysCollmap);

		WEAPON_SUBASSET_DEPENDING(projectileModel, ASSET_TYPE_XMODEL, XModel);

		WEAPON_SUBASSET_DEPENDING(projExplosionEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET_DEPENDING(projDudEffect, ASSET_TYPE_FX, FxEffectDef);

		WEAPON_SUBASSET_DEPENDING(projExplosionSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(projDudSound, ASSET_TYPE_SOUND, snd_alias_list_t);

		WEAPON_SUBASSET_DEPENDING(projTrailEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET_DEPENDING(projBeaconEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET_DEPENDING(projIgnitionEffect, ASSET_TYPE_FX, FxEffectDef);

		WEAPON_SUBASSET_DEPENDING(projIgnitionSound, ASSET_TYPE_SOUND, snd_alias_list_t);

		WEAPON_SUBASSET_DEPENDING(tracerType, ASSET_TYPE_TRACER, TracerDef);

		WEAPON_SUBASSET_DEPENDING(turretOverheatSound, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(turretOverheatEffect, ASSET_TYPE_FX, FxEffectDef);

		WEAPON_SUBASSET_DEPENDING(turretBarrelSpinMaxSnd, ASSET_TYPE_SOUND, snd_alias_list_t);

		for (int i = 0; i < 4; i++)
		{
			WEAPON_SUBASSET_DEPENDING(turretBarrelSpinUpSnd[i], ASSET_TYPE_SOUND, snd_alias_list_t);
			WEAPON_SUBASSET_DEPENDING(turretBarrelSpinDownSnd[i], ASSET_TYPE_SOUND, snd_alias_list_t);
		}

		WEAPON_SUBASSET_DEPENDING(missileConeSoundAlias, ASSET_TYPE_SOUND, snd_alias_list_t);
		WEAPON_SUBASSET_DEPENDING(missileConeSoundAliasAtBase, ASSET_TYPE_SOUND, snd_alias_list_t);

		WEAPON_SUBASSET_DEPENDING(stowOffsetModel, ASSET_TYPE_XMODEL, XModel);
	}

	void weapon_def::load_depending(zone_base* zone)
	{
		auto data = this->asset_;

		if (data->scopes)
		{
			for (auto i = 0u; i < 6; i++)
			{
				if (data->scopes[i])
				{
					zone->add_asset_of_type(ASSET_TYPE_ATTACHMENT, data->scopes[i]->szInternalName);
				}
			}
		}

		if (data->underBarrels)
		{
			for (auto i = 0u; i < 3; i++)
			{
				if (data->underBarrels[i])
				{
					zone->add_asset_of_type(ASSET_TYPE_ATTACHMENT, data->underBarrels[i]->szInternalName);
				}
			}
		}

		if (data->others)
		{
			for (auto i = 0u; i < 8; i++)
			{
				if (data->others[i])
				{
					zone->add_asset_of_type(ASSET_TYPE_ATTACHMENT, data->others[i]->szInternalName);
				}
			}
		}

		if (data->szXAnims)
		{
			for (int i = 0; i < NUM_WEAP_ANIMS; i++)
			{
				if (data->szXAnims[i])
				{
					zone->add_asset_of_type(ASSET_TYPE_XANIMPARTS, data->szXAnims[i]->name);
				}
			}
		}

		if (data->animOverrides)
		{
			for (auto i = 0u; i < data->numAnimOverrides; i++)
			{
				if (data->animOverrides[i].overrideAnim)
				{
					zone->add_asset_of_type(ASSET_TYPE_XANIMPARTS, data->animOverrides[i].overrideAnim->name);
				}

				if (data->animOverrides[i].altmodeAnim)
				{
					zone->add_asset_of_type(ASSET_TYPE_XANIMPARTS, data->animOverrides[i].altmodeAnim->name);
				}
			}
		}

		if (data->soundOverrides)
		{
			for (auto i = 0u; i < data->numSoundOverrides; i++)
			{
				if (data->soundOverrides[i].overrideSound)
				{
					zone->add_asset_of_type(ASSET_TYPE_SOUND, data->soundOverrides[i].overrideSound->name);
				}

				if (data->soundOverrides[i].altmodeSound)
				{
					zone->add_asset_of_type(ASSET_TYPE_SOUND, data->soundOverrides[i].altmodeSound->name);
				}
			}
		}

		if (data->fxOverrides)
		{
			for (auto i = 0u; i < data->numFXOverrides; i++)
			{
				if (data->fxOverrides[i].overrideFX)
				{
					zone->add_asset_of_type(ASSET_TYPE_FX, data->fxOverrides[i].overrideFX->name);
				}

				if (data->fxOverrides[i].altmodeFX)
				{
					zone->add_asset_of_type(ASSET_TYPE_FX, data->fxOverrides[i].altmodeFX->name);
				}
			}
		}

		if (data->dpadIcon)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->dpadIcon->name);
		}

		if (data->killIcon)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->killIcon->name);
		}

		load_depending_weapondef(zone, data->weapDef);
	}

	std::string weapon_def::name()
	{
		return this->name_;
	}

	std::int32_t weapon_def::type()
	{
		return ASSET_TYPE_WEAPON;
	}

#define WEAPON_SOUND_CUSTOM(__field__) \
		if (data->__field__) \
		{ \
			auto ptr = 0xFDFDFDFFFFFFFFFF; \
			buf->align(7); \
			buf->write(&ptr); \
			buf->write_str(data->__field__->name); \
			zone_buffer::clear_pointer(&dest->__field__); \
		}

#define WEAPON_SUBASSET(__field__,__type__,__struct__) \
		if (data->__field__) \
		{ \
			dest->__field__ = reinterpret_cast<__struct__*>(zone->get_asset_pointer(__type__, data->__field__->name)); \
		}

#define WEAPON_SCRIPTSTRING_ARRAY(__field__,__count__) \
		if (data->__field__) \
		{ \
			buf->align(3); \
			buf->write(data->__field__,__count__); \
			zone_buffer::clear_pointer(&dest->__field__); \
		}

#define WEAPON_ATTACHMENT(__field__,__max__) \
		if (data->__field__) \
		{ \
			buf->align(7); \
			auto destAttachments = buf->write(data->__field__,__max__); \
			 \
			for (auto i = 0u; i < __max__; i++) \
			{ \
				if (destAttachments && destAttachments[i]) \
				{ \
					destAttachments[i] = reinterpret_cast<WeaponAttachment*>( \
						zone->get_asset_pointer(ASSET_TYPE_ATTACHMENT, data->__field__[i]->szInternalName) \
					); \
				} \
			} \
			 \
			zone_buffer::clear_pointer(&dest->__field__); \
		}

	void weapon_def::write_weapondef(zone_base* zone, zone_buffer* buf, WeaponCompleteDef* complete, WeaponDef* data)
	{
		auto dest = buf->write(data);

		if (data->szOverlayName)
		{
			dest->szOverlayName = buf->write_str(data->szOverlayName);
		}

		if (data->gunXModel)
		{
			buf->align(7);
			auto destModels = buf->write(data->gunXModel, 64);

			for (auto i = 0u; i < 64; i++)
			{
				if (destModels[i])
				{
					destModels[i] = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->gunXModel[i]->name)
						);
				}
			}

			zone_buffer::clear_pointer(&dest->gunXModel);
		}

		if (data->handXModel)
		{
			dest->handXModel = reinterpret_cast<XModel*>(
				zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->handXModel->name)
				);
		}

		if (data->camoWorldModel)
		{
			dest->camoWorldModel = reinterpret_cast<XModel*>(
				zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->camoWorldModel->name)
				);
		}

		if (data->camoViewModel)
		{
			dest->camoViewModel = reinterpret_cast<XModel*>(
				zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->camoViewModel->name)
				);
		}

		if (data->camoWorldModelMaterialOverride)
		{
			dest->camoWorldModelMaterialOverride = reinterpret_cast<Material*>(
				zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->camoWorldModelMaterialOverride->name)
				);
		}

		if (data->camoViewModelMaterialOverride)
		{
			dest->camoViewModelMaterialOverride = reinterpret_cast<Material*>(
				zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->camoViewModelMaterialOverride->name)
				);
		}

		if (data->camoMaterialTarget)
		{
			buf->align(7);
			auto destMaterials = buf->write(data->camoMaterialTarget, 63);

			for (auto i = 0u; i < 63; i++)
			{
				if (destMaterials[i])
				{
					destMaterials[i] = reinterpret_cast<Material*>(
						zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->camoMaterialTarget[i]->name)
						);
				}
			}

			zone_buffer::clear_pointer(&dest->camoMaterialTarget);
		}

		if (data->szXAnimsRightHanded)
		{
			buf->align(7);
			auto destAnims = buf->write(data->szXAnimsRightHanded, NUM_WEAP_ANIMS);

			for (auto i = 0u; i < NUM_WEAP_ANIMS; i++)
			{
				if (destAnims[i])
				{
					destAnims[i] = reinterpret_cast<XAnimParts*>(
						zone->get_asset_pointer(ASSET_TYPE_XANIMPARTS, data->szXAnimsRightHanded[i]->name)
						);
				}
			}

			zone_buffer::clear_pointer(&dest->szXAnimsRightHanded);
		}

		if (data->szXAnimsLeftHanded)
		{
			buf->align(7);
			auto destAnims = buf->write(data->szXAnimsLeftHanded, NUM_WEAP_ANIMS);

			for (auto i = 0u; i < NUM_WEAP_ANIMS; i++)
			{
				if (destAnims[i])
				{
					destAnims[i] = reinterpret_cast<XAnimParts*>(
						zone->get_asset_pointer(ASSET_TYPE_XANIMPARTS, data->szXAnimsLeftHanded[i]->name)
						);
				}
			}

			zone_buffer::clear_pointer(&dest->szXAnimsLeftHanded);
		}

		if (data->szModeName)
		{
			dest->szModeName = buf->write_str(data->szModeName);
		}

		WEAPON_SCRIPTSTRING_ARRAY(notetrackSoundMapKeys, 24);
		WEAPON_SCRIPTSTRING_ARRAY(notetrackSoundMapValues, 24);
		WEAPON_SCRIPTSTRING_ARRAY(notetrackRumbleMapKeys, 16);
		WEAPON_SCRIPTSTRING_ARRAY(notetrackRumbleMapValues, 16);
		WEAPON_SCRIPTSTRING_ARRAY(notetrackFXMapKeys, 16);

		if (data->notetrackFXMapValues)
		{
			buf->align(7);
			auto destNotetrackFXMapValues = buf->write(data->notetrackFXMapValues, 16);

			for (auto i = 0u; i < 16; i++)
			{
				if (destNotetrackFXMapValues[i])
				{
					destNotetrackFXMapValues[i] = reinterpret_cast<FxEffectDef*>(
						zone->get_asset_pointer(ASSET_TYPE_FX, data->notetrackFXMapValues[i]->name)
						);
				}
			}

			zone_buffer::clear_pointer(&dest->notetrackFXMapValues);
		}

		WEAPON_SCRIPTSTRING_ARRAY(notetrackFXMapTagValues, 16);

		WEAPON_SUBASSET(viewFlashEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(worldFlashEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(viewFlashADSEffect, ASSET_TYPE_FX, FxEffectDef);

		WEAPON_SOUND_CUSTOM(pickupSound);
		WEAPON_SOUND_CUSTOM(pickupSoundPlayer);
		WEAPON_SOUND_CUSTOM(ammoPickupSound);
		WEAPON_SOUND_CUSTOM(ammoPickupSoundPlayer);
		WEAPON_SOUND_CUSTOM(projectileSound);
		WEAPON_SOUND_CUSTOM(pullbackSound);
		WEAPON_SOUND_CUSTOM(pullbackSoundPlayer);
		WEAPON_SOUND_CUSTOM(fireSound);
		WEAPON_SOUND_CUSTOM(fireSoundPlayer);
		WEAPON_SOUND_CUSTOM(fireSoundPlayerAkimbo);
		WEAPON_SOUND_CUSTOM(fireLoopSound);
		WEAPON_SOUND_CUSTOM(fireLoopSoundPlayer);
		WEAPON_SOUND_CUSTOM(fireStopSound);
		WEAPON_SOUND_CUSTOM(fireStopSoundPlayer);
		WEAPON_SOUND_CUSTOM(fireLastSound);
		WEAPON_SOUND_CUSTOM(fireLastSoundPlayer);
		WEAPON_SOUND_CUSTOM(emptyFireSound);
		WEAPON_SOUND_CUSTOM(emptyFireSoundPlayer);
		WEAPON_SOUND_CUSTOM(meleeSwipeSound);
		WEAPON_SOUND_CUSTOM(meleeSwipeSoundPlayer);
		WEAPON_SOUND_CUSTOM(meleeHitSound);
		WEAPON_SOUND_CUSTOM(meleeMissSound);
		WEAPON_SOUND_CUSTOM(rechamberSound);
		WEAPON_SOUND_CUSTOM(rechamberSoundPlayer);
		WEAPON_SOUND_CUSTOM(reloadSound);
		WEAPON_SOUND_CUSTOM(reloadSoundPlayer);
		WEAPON_SOUND_CUSTOM(reloadEmptySound);
		WEAPON_SOUND_CUSTOM(reloadEmptySoundPlayer);
		WEAPON_SOUND_CUSTOM(reloadStartSound);
		WEAPON_SOUND_CUSTOM(reloadStartSoundPlayer);
		WEAPON_SOUND_CUSTOM(reloadEndSound);
		WEAPON_SOUND_CUSTOM(reloadEndSoundPlayer);
		WEAPON_SOUND_CUSTOM(detonateSound);
		WEAPON_SOUND_CUSTOM(detonateSoundPlayer);
		WEAPON_SOUND_CUSTOM(nightVisionWearSound);
		WEAPON_SOUND_CUSTOM(nightVisionWearSoundPlayer);
		WEAPON_SOUND_CUSTOM(nightVisionRemoveSound);
		WEAPON_SOUND_CUSTOM(nightVisionRemoveSoundPlayer);
		WEAPON_SOUND_CUSTOM(altSwitchSound);
		WEAPON_SOUND_CUSTOM(altSwitchSoundPlayer);
		WEAPON_SOUND_CUSTOM(raiseSound);
		WEAPON_SOUND_CUSTOM(raiseSoundPlayer);
		WEAPON_SOUND_CUSTOM(firstRaiseSound);
		WEAPON_SOUND_CUSTOM(firstRaiseSoundPlayer);
		WEAPON_SOUND_CUSTOM(putawaySound);
		WEAPON_SOUND_CUSTOM(putawaySoundPlayer);
		WEAPON_SOUND_CUSTOM(scanSound);
		WEAPON_SOUND_CUSTOM(changeVariableZoomSound);

		if (data->bounceSound)
		{
			buf->align(7);
			dest->bounceSound = buf->write(data->bounceSound, 31);

			for (auto i = 0u; i < 31; i++)
			{
				WEAPON_SOUND_CUSTOM(bounceSound[i]);
			}

			zone_buffer::clear_pointer(&dest->bounceSound);
		}

		if (data->rollingSound)
		{
			buf->align(7);
			dest->rollingSound = buf->write(data->rollingSound, 31);

			for (auto i = 0u; i < 31; i++)
			{
				WEAPON_SOUND_CUSTOM(rollingSound[i]);
			}

			zone_buffer::clear_pointer(&dest->rollingSound);
		}

		WEAPON_SUBASSET(viewShellEjectEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(worldShellEjectEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(viewLastShotEjectEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(worldLastShotEjectEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(viewMagEjectEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(reticleCenter, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(reticleSide, ASSET_TYPE_MATERIAL, Material);

		if (data->worldModel)
		{
			buf->align(7);
			auto destModels = buf->write(data->worldModel, 64);

			for (auto i = 0u; i < 64; i++)
			{
				if (destModels[i])
				{
					destModels[i] = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->worldModel[i]->name)
						);
				}
			}

			zone_buffer::clear_pointer(&dest->worldModel);
		}

		WEAPON_SUBASSET(worldClipModel, ASSET_TYPE_XMODEL, XModel);
		WEAPON_SUBASSET(rocketModel, ASSET_TYPE_XMODEL, XModel);
		WEAPON_SUBASSET(knifeModel, ASSET_TYPE_XMODEL, XModel);
		WEAPON_SUBASSET(worldKnifeModel, ASSET_TYPE_XMODEL, XModel);
		WEAPON_SUBASSET(hudIcon, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(pickupIcon, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(ammoCounterIcon, ASSET_TYPE_MATERIAL, Material);

		if (data->szAmmoName)
		{
			dest->szAmmoName = buf->write_str(data->szAmmoName);
		}

		if (data->szClipName)
		{
			dest->szClipName = buf->write_str(data->szClipName);
		}

		if (data->szSharedAmmoCapName)
		{
			dest->szSharedAmmoCapName = buf->write_str(data->szSharedAmmoCapName);
		}

		WEAPON_SUBASSET(overlay.shader, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(overlay.shaderLowRes, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(overlay.shaderEMP, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(overlay.shaderEMPLowRes, ASSET_TYPE_MATERIAL, Material);

		WEAPON_SUBASSET(physCollmap, ASSET_TYPE_PHYSCOLLMAP, PhysCollmap);

		if (data->projectileName)
		{
			dest->projectileName = buf->write_str(data->projectileName);
		}

		WEAPON_SUBASSET(projectileModel, ASSET_TYPE_XMODEL, XModel);

		WEAPON_SUBASSET(projExplosionEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(projDudEffect, ASSET_TYPE_FX, FxEffectDef);

		WEAPON_SOUND_CUSTOM(projExplosionSound);
		WEAPON_SOUND_CUSTOM(projDudSound);

		if (data->parallelBounce)
		{
			// maybe fix this?
			dest->parallelBounce = nullptr;
		}

		if (data->perpendicularBounce)
		{
			// maybe fix this?
			dest->perpendicularBounce = nullptr;
		}

		WEAPON_SUBASSET(projTrailEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(projBeaconEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(projIgnitionEffect, ASSET_TYPE_FX, FxEffectDef);

		WEAPON_SOUND_CUSTOM(projIgnitionSound);

		if (data->accuracyGraphName[0])
		{
			// maybe fix this?
			dest->accuracyGraphName[0] = nullptr;
		}

		if (data->originalAccuracyGraphKnots[0])
		{
			// maybe fix this?
			dest->originalAccuracyGraphKnots[0] = nullptr;
		}

		if (data->accuracyGraphName[1])
		{
			// maybe fix this?
			dest->accuracyGraphName[1] = nullptr;
		}

		if (data->originalAccuracyGraphKnots[1])
		{
			// maybe fix this?
			dest->originalAccuracyGraphKnots[1] = nullptr;
		}

		if (data->szUseHintString)
		{
			dest->szUseHintString = buf->write_str(data->szUseHintString);
		}

		if (data->dropHintString)
		{
			dest->dropHintString = buf->write_str(data->dropHintString);
		}

		if (data->szScript)
		{
			dest->szScript = buf->write_str(data->szScript);
		}

		if (data->locationDamageMultipliers)
		{
			buf->align(3);
			buf->write(data->locationDamageMultipliers, 22);
			zone_buffer::clear_pointer(&dest->locationDamageMultipliers);
		}

		if (data->fireRumble)
		{
			dest->fireRumble = buf->write_str(data->fireRumble);
		}

		if (data->meleeImpactRumble)
		{
			dest->meleeImpactRumble = buf->write_str(data->meleeImpactRumble);
		}

		WEAPON_SUBASSET(tracerType, ASSET_TYPE_TRACER, TracerDef);

		WEAPON_SOUND_CUSTOM(turretOverheatSound);
		WEAPON_SUBASSET(turretOverheatEffect, ASSET_TYPE_FX, FxEffectDef);

		if (data->turretBarrelSpinRumble)
		{
			dest->turretBarrelSpinRumble = buf->write_str(data->turretBarrelSpinRumble);
		}

		WEAPON_SOUND_CUSTOM(turretBarrelSpinMaxSnd);

		for (int i = 0; i < 4; i++)
		{
			WEAPON_SOUND_CUSTOM(turretBarrelSpinUpSnd[i]);
			WEAPON_SOUND_CUSTOM(turretBarrelSpinDownSnd[i]);
		}

		WEAPON_SOUND_CUSTOM(missileConeSoundAlias);
		WEAPON_SOUND_CUSTOM(missileConeSoundAliasAtBase);

		WEAPON_SUBASSET(stowOffsetModel, ASSET_TYPE_XMODEL, XModel);
	}

	void weapon_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->szInternalName = buf->write_str(this->name());

		if (data->weapDef)
		{
			buf->align(3);
			write_weapondef(zone, buf, data, data->weapDef);
			zone_buffer::clear_pointer(&dest->weapDef);
		}

		if (data->szDisplayName)
		{
			dest->szDisplayName = buf->write_str(data->szDisplayName);
		}

		if (data->hideTags)
		{
			buf->align(3);
			buf->write(data->hideTags, 32);
			zone_buffer::clear_pointer(&dest->hideTags);
		}

		WEAPON_ATTACHMENT(scopes, 6);
		WEAPON_ATTACHMENT(underBarrels, 3);
		WEAPON_ATTACHMENT(others, 8);

		if (data->szXAnims)
		{
			buf->align(7);
			auto destStrings = buf->write(data->szXAnims, NUM_WEAP_ANIMS);

			for (auto i = 0u; i < NUM_WEAP_ANIMS; i++)
			{
				if (destStrings[i])
				{
					destStrings[i] = reinterpret_cast<XAnimParts*>(
						zone->get_asset_pointer(ASSET_TYPE_XANIMPARTS, data->szXAnims[i]->name)
						);
				}
			}

			zone_buffer::clear_pointer(&dest->szXAnims);
		}

		if (data->animOverrides)
		{
			buf->align(3);
			auto destAnimOverrides = buf->write(data->animOverrides, data->numAnimOverrides);

			for (auto i = 0u; i < data->numAnimOverrides; i++)
			{
				if (destAnimOverrides[i].overrideAnim)
				{
					destAnimOverrides[i].overrideAnim = reinterpret_cast<XAnimParts*>(
						zone->get_asset_pointer(ASSET_TYPE_XANIMPARTS, data->animOverrides[i].overrideAnim->name)
						);
				}

				if (destAnimOverrides[i].altmodeAnim)
				{
					destAnimOverrides[i].altmodeAnim = reinterpret_cast<XAnimParts*>(
						zone->get_asset_pointer(ASSET_TYPE_XANIMPARTS, data->animOverrides[i].altmodeAnim->name)
						);
				}
			}

			zone_buffer::clear_pointer(&dest->animOverrides);
		}

		if (data->soundOverrides)
		{
			buf->align(3);
			dest->soundOverrides = buf->write(data->soundOverrides, data->numSoundOverrides);

			for (auto i = 0u; i < data->numSoundOverrides; i++)
			{
				if (dest->soundOverrides[i].overrideSound)
				{
					WEAPON_SOUND_CUSTOM(soundOverrides[i].overrideSound);
				}

				if (dest->soundOverrides[i].altmodeSound)
				{
					WEAPON_SOUND_CUSTOM(soundOverrides[i].altmodeSound);
				}
			}

			zone_buffer::clear_pointer(&dest->soundOverrides);
		}

		if (data->fxOverrides)
		{
			buf->align(3);
			auto destFxOverrides = buf->write(data->fxOverrides, data->numFXOverrides);

			for (auto i = 0u; i < data->numFXOverrides; i++)
			{
				if (destFxOverrides[i].overrideFX)
				{
					destFxOverrides[i].overrideFX = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(
						ASSET_TYPE_FX, data->fxOverrides[i].overrideFX->name
					));
				}

				if (destFxOverrides[i].altmodeFX)
				{
					destFxOverrides[i].altmodeFX = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(
						ASSET_TYPE_FX, data->fxOverrides[i].altmodeFX->name
					));
				}
			}

			zone_buffer::clear_pointer(&dest->fxOverrides);
		}

		if (data->reloadOverrides)
		{
			buf->align(3);
			buf->write(data->reloadOverrides, data->numReloadStateTimerOverrides);
			zone_buffer::clear_pointer(&dest->reloadOverrides);
		}

		if (data->notetrackOverrides)
		{
			buf->align(3);
			auto destNoteTrackOverrides = buf->write(data->notetrackOverrides, data->numNotetrackOverrides);

			for (auto i = 0u; i < data->numNotetrackOverrides; i++)
			{
				if (destNoteTrackOverrides[i].notetrackSoundMapKeys)
				{
					buf->align(3);
					buf->write(data->notetrackOverrides[i].notetrackSoundMapKeys, 24);
					zone_buffer::clear_pointer(&destNoteTrackOverrides[i].notetrackSoundMapKeys);
				}

				if (destNoteTrackOverrides[i].notetrackSoundMapValues)
				{
					buf->align(3);
					buf->write(data->notetrackOverrides[i].notetrackSoundMapValues, 24);
					zone_buffer::clear_pointer(&destNoteTrackOverrides[i].notetrackSoundMapValues);
				}
			}

			zone_buffer::clear_pointer(&dest->notetrackOverrides);
		}

		if (data->szAltWeaponName)
		{
			dest->szAltWeaponName = buf->write_str(data->szAltWeaponName);
		}

		if (data->killIcon)
		{
			dest->killIcon = reinterpret_cast<Material*>(
				zone->get_asset_pointer(ASSET_TYPE_MATERIAL, dest->killIcon->name)
				);
		}

		if (data->dpadIcon)
		{
			dest->dpadIcon = reinterpret_cast<Material*>(
				zone->get_asset_pointer(ASSET_TYPE_MATERIAL, dest->dpadIcon->name)
				);
		}

		if (data->accuracyGraphKnots[0])
		{
			buf->align(3);
			buf->write(data->accuracyGraphKnots[0], data->accuracyGraphKnotCount[0]);
			zone_buffer::clear_pointer(&dest->accuracyGraphKnots[0]);
		}

		if (data->accuracyGraphKnots[1])
		{
			buf->align(3);
			buf->write(data->accuracyGraphKnots[1], data->accuracyGraphKnotCount[1]);
			zone_buffer::clear_pointer(&dest->accuracyGraphKnots[1]);
		}

		if (data->szAdsrBaseSetting)
		{
			dest->szAdsrBaseSetting = buf->write_str(data->szAdsrBaseSetting);
		}

		buf->pop_stream();
	}

#define WEAPON_DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__

#define WEAPON_DUMP_STRING WEAPON_DUMP_FIELD

#define WEAPON_DUMP_FIELD_ARR(__field__, __size__) \
	for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
	{ \
		data[#__field__][idx##__field__] = asset->__field__[idx##__field__]; \
	}

#define WEAPON_DUMP_FIELD_ARR_STR(__field__, __size__) \
	for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
	{ \
		if (asset->__field__[idx##__field__] != nullptr) \
			data[#__field__][idx##__field__] = asset->__field__[idx##__field__]; \
		else \
			data[#__field__][idx##__field__] = ""; \
	}

#define WEAPON_DUMP_ASSET(__field__) \
	if (asset->__field__) \
	{ \
		data[#__field__] = asset->__field__->name; \
	} \
	else \
	{ \
		data[#__field__] = ""; \
	}

#define WEAPON_DUMP_ASSET_ARR(__field__, __size__) \
	if (asset->__field__) \
	{ \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			if (asset->__field__[idx##__field__]) \
			{ \
				data[#__field__][idx##__field__] = asset->__field__[idx##__field__]->name; \
			} \
			else \
			{ \
				data[#__field__][idx##__field__] = ""; \
			} \
		} \
	} \
	else \
	{ \
		data[#__field__] = nullptr; \
	}

#define WEAPON_DUMP_ANIM_ARR(__field__, __size__) \
	if (asset->__field__) \
	{ \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			auto name##__field__ = get_anim_name_from_index(idx##__field__); \
			if (asset->__field__[idx##__field__]) \
			{ \
				data[#__field__][name##__field__] = asset->__field__[idx##__field__]->name; \
			} \
			else \
			{ \
				data[#__field__][name##__field__] = ""; \
			} \
		} \
	} \
	else \
	{ \
		data[#__field__] = nullptr; \
	}

#define WEAPON_DUMP_SOUND(__field__) \
	if (asset->__field__) \
	{ \
		data["sounds"][#__field__] = asset->__field__->name; \
	} \
	else \
	{ \
		data["sounds"][#__field__] = ""; \
	}

	ordered_json dump_overlay(ADSOverlay* asset)
	{
		ordered_json data;

		WEAPON_DUMP_ASSET(shader);
		WEAPON_DUMP_ASSET(shaderLowRes);
		WEAPON_DUMP_ASSET(shaderEMP);
		WEAPON_DUMP_ASSET(shaderEMPLowRes);
		WEAPON_DUMP_FIELD(reticle);
		WEAPON_DUMP_FIELD(width);
		WEAPON_DUMP_FIELD(height);
		WEAPON_DUMP_FIELD(widthSplitscreen);
		WEAPON_DUMP_FIELD(heightSplitscreen);

		return data;
	}

	json dump_accuracy_graph(WeaponDef* asset)
	{
		json data;

		

		return data;
	}

	json dump_statetimers(StateTimers* asset)
	{
		json data;

		WEAPON_DUMP_FIELD(iFireDelay);
		WEAPON_DUMP_FIELD(iMeleeDelay);
		WEAPON_DUMP_FIELD(meleeChargeDelay);
		WEAPON_DUMP_FIELD(iDetonateDelay);
		WEAPON_DUMP_FIELD(iRechamberTime);
		WEAPON_DUMP_FIELD(rechamberTimeOneHanded);
		WEAPON_DUMP_FIELD(iRechamberBoltTime);
		WEAPON_DUMP_FIELD(iHoldFireTime);
		WEAPON_DUMP_FIELD(iHoldPrimeTime);
		WEAPON_DUMP_FIELD(iDetonateTime);
		WEAPON_DUMP_FIELD(iMeleeTime);
		WEAPON_DUMP_FIELD(meleeChargeTime);
		WEAPON_DUMP_FIELD(iReloadTime);
		WEAPON_DUMP_FIELD(reloadShowRocketTime);
		WEAPON_DUMP_FIELD(iReloadEmptyTime);
		WEAPON_DUMP_FIELD(iReloadAddTime);
		WEAPON_DUMP_FIELD(iReloadEmptyAddTime);
		WEAPON_DUMP_FIELD(iReloadStartTime);
		WEAPON_DUMP_FIELD(iReloadStartAddTime);
		WEAPON_DUMP_FIELD(iReloadEndTime);
		WEAPON_DUMP_FIELD(iDropTime);
		WEAPON_DUMP_FIELD(iRaiseTime);
		WEAPON_DUMP_FIELD(iAltDropTime);
		WEAPON_DUMP_FIELD(quickDropTime);
		WEAPON_DUMP_FIELD(quickRaiseTime);
		WEAPON_DUMP_FIELD(iBreachRaiseTime);
		WEAPON_DUMP_FIELD(iEmptyRaiseTime);
		WEAPON_DUMP_FIELD(iEmptyDropTime);
		WEAPON_DUMP_FIELD(sprintInTime);
		WEAPON_DUMP_FIELD(sprintLoopTime);
		WEAPON_DUMP_FIELD(sprintOutTime);
		WEAPON_DUMP_FIELD(stunnedTimeBegin);
		WEAPON_DUMP_FIELD(stunnedTimeLoop);
		WEAPON_DUMP_FIELD(stunnedTimeEnd);
		WEAPON_DUMP_FIELD(nightVisionWearTime);
		WEAPON_DUMP_FIELD(nightVisionWearTimeFadeOutEnd);
		WEAPON_DUMP_FIELD(nightVisionWearTimePowerUp);
		WEAPON_DUMP_FIELD(nightVisionRemoveTime);
		WEAPON_DUMP_FIELD(nightVisionRemoveTimePowerDown);
		WEAPON_DUMP_FIELD(nightVisionRemoveTimeFadeInStart);
		WEAPON_DUMP_FIELD(fuseTime);
		WEAPON_DUMP_FIELD(aiFuseTime);
		WEAPON_DUMP_FIELD(bHoldFullPrime);
		WEAPON_DUMP_FIELD(blastFrontTime);
		WEAPON_DUMP_FIELD(blastRightTime);
		WEAPON_DUMP_FIELD(blastBackTime);
		WEAPON_DUMP_FIELD(blastLeftTime);
		WEAPON_DUMP_FIELD(slideInTime);
		WEAPON_DUMP_FIELD(slideLoopTime);
		WEAPON_DUMP_FIELD(slideOutTime);

		return data;
	}

	json weapon_def::dump_weapondef(WeaponDef* asset)
	{
		assert(sizeof(WeaponDef) == 2712);

		json data;

		WEAPON_DUMP_ASSET_ARR(gunXModel, 64);
		WEAPON_DUMP_ASSET_ARR(worldModel, 64);

		WEAPON_DUMP_ASSET_ARR(camoMaterialTarget, 63);

		WEAPON_DUMP_ANIM_ARR(szXAnimsRightHanded, 124);
		WEAPON_DUMP_ANIM_ARR(szXAnimsLeftHanded, 124);

		for (auto i = 0u; i < 24; i++)
		{
			if (asset->notetrackSoundMapKeys && asset->notetrackSoundMapKeys[i])
			{
				data["notetrackSoundMapKeys"][i] = SL_ConvertToString(asset->notetrackSoundMapKeys[i]);
			}
			else
			{
				data["notetrackSoundMapKeys"][i] = "";
			}

			if (asset->notetrackSoundMapValues && asset->notetrackSoundMapValues[i])
			{
				data["notetrackSoundMapValues"][i] = SL_ConvertToString(asset->notetrackSoundMapValues[i]);
			}
			else
			{
				data["notetrackSoundMapValues"][i] = "";
			}
		}

		for (auto i = 0u; i < 16; i++)
		{
			if (asset->notetrackRumbleMapKeys && asset->notetrackRumbleMapKeys[i])
			{
				data["notetrackRumbleMapKeys"][i] = SL_ConvertToString(asset->notetrackRumbleMapKeys[i]);
			}
			else
			{
				data["notetrackRumbleMapKeys"][i] = "";
			}

			if (asset->notetrackRumbleMapValues && asset->notetrackRumbleMapValues[i])
			{
				data["notetrackRumbleMapValues"][i] = SL_ConvertToString(asset->notetrackRumbleMapValues[i]);
			}
			else
			{
				data["notetrackRumbleMapValues"][i] = "";
			}

			if (asset->notetrackFXMapKeys && asset->notetrackFXMapKeys[i])
			{
				data["notetrackFXMapKeys"][i] = SL_ConvertToString(asset->notetrackFXMapKeys[i]);
			}
			else
			{
				data["notetrackFXMapKeys"][i] = "";
			}

			if (asset->notetrackFXMapValues && asset->notetrackFXMapValues[i])
			{
				data["notetrackFXMapValues"][i] = asset->notetrackFXMapValues[i]->name;
			}
			else
			{
				data["notetrackFXMapValues"][i] = "";
			}

			if (asset->notetrackFXMapTagValues && asset->notetrackFXMapTagValues[i])
			{
				data["notetrackFXMapTagValues"][i] = SL_ConvertToString(asset->notetrackFXMapTagValues[i]);
			}
			else
			{
				data["notetrackFXMapTagValues"][i] = "";
			}
		}

		WEAPON_DUMP_SOUND(pickupSound);
		WEAPON_DUMP_SOUND(pickupSoundPlayer);
		WEAPON_DUMP_SOUND(ammoPickupSound);
		WEAPON_DUMP_SOUND(ammoPickupSoundPlayer);
		WEAPON_DUMP_SOUND(projectileSound);
		WEAPON_DUMP_SOUND(pullbackSound);
		WEAPON_DUMP_SOUND(pullbackSoundPlayer);
		WEAPON_DUMP_SOUND(fireSound);
		WEAPON_DUMP_SOUND(fireSoundPlayer);
		WEAPON_DUMP_SOUND(fireSoundPlayerAkimbo);
		WEAPON_DUMP_SOUND(fireLoopSound);
		WEAPON_DUMP_SOUND(fireLoopSoundPlayer);
		WEAPON_DUMP_SOUND(fireStopSound);
		WEAPON_DUMP_SOUND(fireStopSoundPlayer);
		WEAPON_DUMP_SOUND(fireLastSound);
		WEAPON_DUMP_SOUND(fireLastSoundPlayer);
		WEAPON_DUMP_SOUND(emptyFireSound);
		WEAPON_DUMP_SOUND(emptyFireSoundPlayer);
		WEAPON_DUMP_SOUND(meleeSwipeSound);
		WEAPON_DUMP_SOUND(meleeSwipeSoundPlayer);
		WEAPON_DUMP_SOUND(meleeHitSound);
		WEAPON_DUMP_SOUND(meleeMissSound);
		WEAPON_DUMP_SOUND(rechamberSound);
		WEAPON_DUMP_SOUND(rechamberSoundPlayer);
		WEAPON_DUMP_SOUND(reloadSound);
		WEAPON_DUMP_SOUND(reloadSoundPlayer);
		WEAPON_DUMP_SOUND(reloadEmptySound);
		WEAPON_DUMP_SOUND(reloadEmptySoundPlayer);
		WEAPON_DUMP_SOUND(reloadStartSound);
		WEAPON_DUMP_SOUND(reloadStartSoundPlayer);
		WEAPON_DUMP_SOUND(reloadEndSound);
		WEAPON_DUMP_SOUND(reloadEndSoundPlayer);
		WEAPON_DUMP_SOUND(detonateSound);
		WEAPON_DUMP_SOUND(detonateSoundPlayer);
		WEAPON_DUMP_SOUND(nightVisionWearSound);
		WEAPON_DUMP_SOUND(nightVisionWearSoundPlayer);
		WEAPON_DUMP_SOUND(nightVisionRemoveSound);
		WEAPON_DUMP_SOUND(nightVisionRemoveSoundPlayer);
		WEAPON_DUMP_SOUND(altSwitchSound);
		WEAPON_DUMP_SOUND(altSwitchSoundPlayer);
		WEAPON_DUMP_SOUND(raiseSound);
		WEAPON_DUMP_SOUND(raiseSoundPlayer);
		WEAPON_DUMP_SOUND(firstRaiseSound);
		WEAPON_DUMP_SOUND(firstRaiseSoundPlayer);
		WEAPON_DUMP_SOUND(putawaySound);
		WEAPON_DUMP_SOUND(putawaySoundPlayer);
		WEAPON_DUMP_SOUND(scanSound);
		WEAPON_DUMP_SOUND(changeVariableZoomSound);

		WEAPON_DUMP_ASSET_ARR(bounceSound, 31);
		WEAPON_DUMP_ASSET_ARR(rollingSound, 31);

		WEAPON_DUMP_ASSET_ARR(turretBarrelSpinUpSnd, 4);
		WEAPON_DUMP_ASSET_ARR(turretBarrelSpinDownSnd, 4);

		WEAPON_DUMP_FIELD(szOverlayName);
		WEAPON_DUMP_ASSET(handXModel);
		WEAPON_DUMP_ASSET(camoWorldModel);
		WEAPON_DUMP_ASSET(camoViewModel);
		WEAPON_DUMP_ASSET(camoWorldModelMaterialOverride);
		WEAPON_DUMP_ASSET(camoViewModelMaterialOverride);
		WEAPON_DUMP_FIELD(szModeName);
		WEAPON_DUMP_FIELD(playerAnimType);
		WEAPON_DUMP_FIELD(weapType);
		WEAPON_DUMP_FIELD(weapClass);
		WEAPON_DUMP_FIELD(penetrateType);
		WEAPON_DUMP_FIELD(inventoryType);
		WEAPON_DUMP_FIELD(fireType);
		WEAPON_DUMP_FIELD(burstFireCooldown);
		WEAPON_DUMP_FIELD(offhandClass);
		WEAPON_DUMP_FIELD(stance);
		WEAPON_DUMP_FIELD(rattleSoundType);
		WEAPON_DUMP_ASSET(viewFlashEffect);
		WEAPON_DUMP_ASSET(worldFlashEffect);
		WEAPON_DUMP_ASSET(viewFlashADSEffect);
		WEAPON_DUMP_ASSET(viewShellEjectEffect);
		WEAPON_DUMP_ASSET(worldShellEjectEffect);
		WEAPON_DUMP_ASSET(viewLastShotEjectEffect);
		WEAPON_DUMP_ASSET(worldLastShotEjectEffect);
		WEAPON_DUMP_ASSET(viewMagEjectEffect);
		WEAPON_DUMP_ASSET(reticleCenter);
		WEAPON_DUMP_ASSET(reticleSide);
		WEAPON_DUMP_FIELD(iReticleCenterSize);
		WEAPON_DUMP_FIELD(iReticleSideSize);
		WEAPON_DUMP_FIELD(iReticleMinOfs);
		WEAPON_DUMP_FIELD(activeReticleType);

		WEAPON_DUMP_FIELD_ARR(vStandMove, 3);
		WEAPON_DUMP_FIELD_ARR(vStandRot, 3);
		WEAPON_DUMP_FIELD_ARR(strafeMove, 3);
		WEAPON_DUMP_FIELD_ARR(strafeRot, 3);
		WEAPON_DUMP_FIELD_ARR(vDuckedOfs, 3);
		WEAPON_DUMP_FIELD_ARR(vDuckedMove, 3);
		WEAPON_DUMP_FIELD_ARR(vDuckedRot, 3);
		WEAPON_DUMP_FIELD_ARR(vProneOfs, 3);
		WEAPON_DUMP_FIELD_ARR(vProneMove, 3);
		WEAPON_DUMP_FIELD_ARR(vProneRot, 3);

		WEAPON_DUMP_FIELD(fPosMoveRate);
		WEAPON_DUMP_FIELD(fPosProneMoveRate);
		WEAPON_DUMP_FIELD(fStandMoveMinSpeed);
		WEAPON_DUMP_FIELD(fDuckedMoveMinSpeed);
		WEAPON_DUMP_FIELD(fProneMoveMinSpeed);
		WEAPON_DUMP_FIELD(fPosRotRate);
		WEAPON_DUMP_FIELD(fPosProneRotRate);
		WEAPON_DUMP_FIELD(fStandRotMinSpeed);
		WEAPON_DUMP_FIELD(fDuckedRotMinSpeed);
		WEAPON_DUMP_FIELD(fProneRotMinSpeed);
		WEAPON_DUMP_ASSET(worldClipModel);
		WEAPON_DUMP_ASSET(rocketModel);
		WEAPON_DUMP_ASSET(knifeModel);
		WEAPON_DUMP_ASSET(worldKnifeModel);
		WEAPON_DUMP_ASSET(hudIcon);
		WEAPON_DUMP_FIELD(hudIconRatio);
		WEAPON_DUMP_ASSET(pickupIcon);
		WEAPON_DUMP_FIELD(pickupIconRatio);
		WEAPON_DUMP_ASSET(ammoCounterIcon);
		WEAPON_DUMP_FIELD(ammoCounterIconRatio);
		WEAPON_DUMP_FIELD(ammoCounterClip);
		WEAPON_DUMP_FIELD(iStartAmmo);
		WEAPON_DUMP_FIELD(szAmmoName);
		WEAPON_DUMP_FIELD(iAmmoIndex);
		WEAPON_DUMP_FIELD(szClipName);
		WEAPON_DUMP_FIELD(iClipIndex);
		WEAPON_DUMP_FIELD(iMaxAmmo);
		WEAPON_DUMP_FIELD(shotCount);
		WEAPON_DUMP_FIELD(szSharedAmmoCapName);
		WEAPON_DUMP_FIELD(iSharedAmmoCapIndex);
		WEAPON_DUMP_FIELD(iSharedAmmoCap);
		WEAPON_DUMP_FIELD(damage);
		WEAPON_DUMP_FIELD(playerDamage);
		WEAPON_DUMP_FIELD(iMeleeDamage);
		WEAPON_DUMP_FIELD(iDamageType);

		data["stateTimers"] = dump_statetimers(&asset->stateTimers);
		data["akimboStateTimers"] = dump_statetimers(&asset->akimboStateTimers);

		WEAPON_DUMP_FIELD(autoAimRange);
		WEAPON_DUMP_FIELD(aimAssistRange);
		WEAPON_DUMP_FIELD(aimAssistRangeAds);
		WEAPON_DUMP_FIELD(aimPadding);
		WEAPON_DUMP_FIELD(enemyCrosshairRange);
		WEAPON_DUMP_FIELD(moveSpeedScale);
		WEAPON_DUMP_FIELD(adsMoveSpeedScale);
		WEAPON_DUMP_FIELD(sprintDurationScale);
		WEAPON_DUMP_FIELD(fAdsZoomInFrac);
		WEAPON_DUMP_FIELD(fAdsZoomOutFrac);

		data["overlay"] = dump_overlay(&asset->overlay);

		WEAPON_DUMP_FIELD(overlayInterface);
		WEAPON_DUMP_FIELD(fAdsBobFactor);
		WEAPON_DUMP_FIELD(fAdsViewBobMult);
		WEAPON_DUMP_FIELD(fHipSpreadStandMin);
		WEAPON_DUMP_FIELD(fHipSpreadDuckedMin);
		WEAPON_DUMP_FIELD(fHipSpreadProneMin);
		WEAPON_DUMP_FIELD(hipSpreadStandMax);
		WEAPON_DUMP_FIELD(hipSpreadDuckedMax);
		WEAPON_DUMP_FIELD(hipSpreadProneMax);
		WEAPON_DUMP_FIELD(fHipSpreadDecayRate);
		WEAPON_DUMP_FIELD(fHipSpreadFireAdd);
		WEAPON_DUMP_FIELD(fHipSpreadTurnAdd);
		WEAPON_DUMP_FIELD(fHipSpreadMoveAdd);
		WEAPON_DUMP_FIELD(fHipSpreadDuckedDecay);
		WEAPON_DUMP_FIELD(fHipSpreadProneDecay);
		WEAPON_DUMP_FIELD(fHipReticleSidePos);
		WEAPON_DUMP_FIELD(fAdsIdleAmount);
		WEAPON_DUMP_FIELD(fHipIdleAmount);
		WEAPON_DUMP_FIELD(adsIdleSpeed);
		WEAPON_DUMP_FIELD(hipIdleSpeed);
		WEAPON_DUMP_FIELD(fIdleCrouchFactor);
		WEAPON_DUMP_FIELD(fIdleProneFactor);
		WEAPON_DUMP_FIELD(fGunMaxPitch);
		WEAPON_DUMP_FIELD(fGunMaxYaw);
		WEAPON_DUMP_FIELD(adsIdleLerpStartTime);
		WEAPON_DUMP_FIELD(adsIdleLerpTime);
		WEAPON_DUMP_FIELD(swayMaxAngleSteadyAim);
		WEAPON_DUMP_FIELD(swayMaxAngle);
		WEAPON_DUMP_FIELD(swayLerpSpeed);
		WEAPON_DUMP_FIELD(swayPitchScale);
		WEAPON_DUMP_FIELD(swayYawScale);
		WEAPON_DUMP_FIELD(swayHorizScale);
		WEAPON_DUMP_FIELD(swayVertScale);
		WEAPON_DUMP_FIELD(swayShellShockScale);
		WEAPON_DUMP_FIELD(adsSwayMaxAngle);
		WEAPON_DUMP_FIELD(adsSwayLerpSpeed);
		WEAPON_DUMP_FIELD(adsSwayPitchScale);
		WEAPON_DUMP_FIELD(adsSwayYawScale);
		WEAPON_DUMP_FIELD(adsSwayHorizScale);
		WEAPON_DUMP_FIELD(adsSwayVertScale);
		WEAPON_DUMP_FIELD(adsFireRateScale);
		WEAPON_DUMP_FIELD(adsDamageRangeScale);
		WEAPON_DUMP_FIELD(adsFireAnimFrac);
		WEAPON_DUMP_ASSET(physCollmap);
		WEAPON_DUMP_FIELD(dualWieldViewModelOffset);
		WEAPON_DUMP_FIELD(fScopeDriftDelay);
		WEAPON_DUMP_FIELD(fScopeDriftLerpInTime);
		WEAPON_DUMP_FIELD(fScopeDriftSteadyTime);
		WEAPON_DUMP_FIELD(fScopeDriftLerpOutTime);
		WEAPON_DUMP_FIELD(fScopeDriftSteadyFactor);
		WEAPON_DUMP_FIELD(fScopeDriftUnsteadyFactor);
		WEAPON_DUMP_FIELD(killIconRatio);
		WEAPON_DUMP_FIELD(iReloadAmmoAdd);
		WEAPON_DUMP_FIELD(iReloadStartAdd);
		WEAPON_DUMP_FIELD(ammoDropStockMin);
		WEAPON_DUMP_FIELD(ammoDropClipPercentMin);
		WEAPON_DUMP_FIELD(ammoDropClipPercentMax);
		WEAPON_DUMP_FIELD(iExplosionRadius);
		WEAPON_DUMP_FIELD(iExplosionRadiusMin);
		WEAPON_DUMP_FIELD(iExplosionInnerDamage);
		WEAPON_DUMP_FIELD(iExplosionOuterDamage);
		WEAPON_DUMP_FIELD(damageConeAngle);
		WEAPON_DUMP_FIELD(bulletExplDmgMult);
		WEAPON_DUMP_FIELD(bulletExplRadiusMult);
		WEAPON_DUMP_FIELD(iProjectileSpeed);
		WEAPON_DUMP_FIELD(iProjectileSpeedUp);
		WEAPON_DUMP_FIELD(iProjectileSpeedForward);
		WEAPON_DUMP_FIELD(iProjectileActivateDist);
		WEAPON_DUMP_FIELD(projLifetime);
		WEAPON_DUMP_FIELD(timeToAccelerate);
		WEAPON_DUMP_FIELD(projectileCurvature);
		WEAPON_DUMP_FIELD(projectileName);
		WEAPON_DUMP_ASSET(projectileModel);
		WEAPON_DUMP_FIELD(projExplosion);
		WEAPON_DUMP_ASSET(projExplosionEffect);
		WEAPON_DUMP_ASSET(projDudEffect);
		WEAPON_DUMP_ASSET(projExplosionSound);
		WEAPON_DUMP_ASSET(projDudSound);
		WEAPON_DUMP_FIELD(stickiness);
		WEAPON_DUMP_FIELD(lowAmmoWarningThreshold);
		WEAPON_DUMP_FIELD(ricochetChance);
		WEAPON_DUMP_FIELD(riotShieldEnableDamage);
		WEAPON_DUMP_FIELD(riotShieldHealth);
		WEAPON_DUMP_FIELD(riotShieldDamageMult);

		//float* parallelBounce;
		//float* perpendicularBounce;

		WEAPON_DUMP_ASSET(projTrailEffect);
		WEAPON_DUMP_ASSET(projBeaconEffect);

		WEAPON_DUMP_FIELD_ARR(vProjectileColor, 3);
		WEAPON_DUMP_FIELD(guidedMissileType);
		WEAPON_DUMP_FIELD(maxSteeringAccel);
		WEAPON_DUMP_FIELD(projIgnitionDelay);
		WEAPON_DUMP_ASSET(projIgnitionEffect);
		WEAPON_DUMP_ASSET(projIgnitionSound);
		WEAPON_DUMP_FIELD(fAdsAimPitch);
		WEAPON_DUMP_FIELD(fAdsCrosshairInFrac);
		WEAPON_DUMP_FIELD(fAdsCrosshairOutFrac);
		WEAPON_DUMP_FIELD(adsShouldShowCrosshair);
		WEAPON_DUMP_FIELD(adsGunKickReducedKickBullets);
		WEAPON_DUMP_FIELD(adsGunKickReducedKickPercent);
		WEAPON_DUMP_FIELD(fAdsGunKickPitchMin);
		WEAPON_DUMP_FIELD(fAdsGunKickPitchMax);
		WEAPON_DUMP_FIELD(fAdsGunKickYawMin);
		WEAPON_DUMP_FIELD(fAdsGunKickYawMax);
		WEAPON_DUMP_FIELD(fAdsGunKickMagMin);
		WEAPON_DUMP_FIELD(fAdsGunKickAccel);
		WEAPON_DUMP_FIELD(fAdsGunKickSpeedMax);
		WEAPON_DUMP_FIELD(fAdsGunKickSpeedDecay);
		WEAPON_DUMP_FIELD(fAdsGunKickStaticDecay);
		WEAPON_DUMP_FIELD(fAdsViewKickPitchMin);
		WEAPON_DUMP_FIELD(fAdsViewKickPitchMax);
		WEAPON_DUMP_FIELD(fAdsViewKickYawMin);
		WEAPON_DUMP_FIELD(fAdsViewKickYawMax);
		WEAPON_DUMP_FIELD(fAdsViewKickMagMin);
		WEAPON_DUMP_FIELD(fAdsViewScatterMin);
		WEAPON_DUMP_FIELD(fAdsViewScatterMax);
		WEAPON_DUMP_FIELD(fAdsSpread);
		WEAPON_DUMP_FIELD(hipGunKickReducedKickBullets);
		WEAPON_DUMP_FIELD(hipGunKickReducedKickPercent);
		WEAPON_DUMP_FIELD(fHipGunKickPitchMin);
		WEAPON_DUMP_FIELD(fHipGunKickPitchMax);
		WEAPON_DUMP_FIELD(fHipGunKickYawMin);
		WEAPON_DUMP_FIELD(fHipGunKickYawMax);
		WEAPON_DUMP_FIELD(fHipGunKickMagMin);
		WEAPON_DUMP_FIELD(fHipGunKickAccel);
		WEAPON_DUMP_FIELD(fHipGunKickSpeedMax);
		WEAPON_DUMP_FIELD(fHipGunKickSpeedDecay);
		WEAPON_DUMP_FIELD(fHipGunKickStaticDecay);
		WEAPON_DUMP_FIELD(fHipViewKickPitchMin);
		WEAPON_DUMP_FIELD(fHipViewKickPitchMax);
		WEAPON_DUMP_FIELD(fHipViewKickYawMin);
		WEAPON_DUMP_FIELD(fHipViewKickYawMax);
		WEAPON_DUMP_FIELD(fHipViewKickMagMin);
		WEAPON_DUMP_FIELD(fHipViewScatterMin);
		WEAPON_DUMP_FIELD(fHipViewScatterMax);
		WEAPON_DUMP_FIELD(fightDist);
		WEAPON_DUMP_FIELD(maxDist);
		WEAPON_DUMP_FIELD_ARR_STR(accuracyGraphName, 2);
		//float(*originalAccuracyGraphKnots[2])[2];
		//unsigned __int16 originalAccuracyGraphKnotCount[2];
		WEAPON_DUMP_FIELD(iPositionReloadTransTime);
		WEAPON_DUMP_FIELD(leftArc);
		WEAPON_DUMP_FIELD(rightArc);
		WEAPON_DUMP_FIELD(topArc);
		WEAPON_DUMP_FIELD(bottomArc);
		WEAPON_DUMP_FIELD(accuracy);
		WEAPON_DUMP_FIELD(aiSpread);
		WEAPON_DUMP_FIELD(playerSpread);
		WEAPON_DUMP_FIELD_ARR(minTurnSpeed, 2);
		WEAPON_DUMP_FIELD_ARR(maxTurnSpeed, 2);
		WEAPON_DUMP_FIELD(pitchConvergenceTime);
		WEAPON_DUMP_FIELD(yawConvergenceTime);
		WEAPON_DUMP_FIELD(suppressTime);
		WEAPON_DUMP_FIELD(maxRange);
		WEAPON_DUMP_FIELD(fAnimHorRotateInc);
		WEAPON_DUMP_FIELD(fPlayerPositionDist);
		WEAPON_DUMP_FIELD(szUseHintString);
		WEAPON_DUMP_FIELD(dropHintString);
		WEAPON_DUMP_FIELD(iUseHintStringIndex);
		WEAPON_DUMP_FIELD(dropHintStringIndex);
		WEAPON_DUMP_FIELD(horizViewJitter);
		WEAPON_DUMP_FIELD(vertViewJitter);
		WEAPON_DUMP_FIELD(scanSpeed);
		WEAPON_DUMP_FIELD(scanAccel);
		WEAPON_DUMP_FIELD(scanPauseTime);
		WEAPON_DUMP_FIELD(szScript);
		WEAPON_DUMP_FIELD_ARR(fOOPosAnimLength, 2);
		WEAPON_DUMP_FIELD(minDamage);
		WEAPON_DUMP_FIELD(minPlayerDamage);
		WEAPON_DUMP_FIELD(fMaxDamageRange);
		WEAPON_DUMP_FIELD(fMinDamageRange);
		WEAPON_DUMP_FIELD(destabilizationRateTime);
		WEAPON_DUMP_FIELD(destabilizationCurvatureMax);
		WEAPON_DUMP_FIELD(destabilizeDistance);
		WEAPON_DUMP_FIELD_ARR(locationDamageMultipliers, 22);
		WEAPON_DUMP_FIELD(fireRumble);
		WEAPON_DUMP_FIELD(meleeImpactRumble);
		WEAPON_DUMP_ASSET(tracerType);
		WEAPON_DUMP_FIELD(turretADSEnabled);
		WEAPON_DUMP_FIELD(turretADSTime);
		WEAPON_DUMP_FIELD(turretFov);
		WEAPON_DUMP_FIELD(turretFovADS);
		WEAPON_DUMP_FIELD(turretScopeZoomRate);
		WEAPON_DUMP_FIELD(turretScopeZoomMin);
		WEAPON_DUMP_FIELD(turretScopeZoomMax);
		WEAPON_DUMP_FIELD(turretOverheatUpRate);
		WEAPON_DUMP_FIELD(turretOverheatDownRate);
		WEAPON_DUMP_FIELD(turretOverheatPenalty);
		WEAPON_DUMP_ASSET(turretOverheatSound);
		WEAPON_DUMP_ASSET(turretOverheatEffect);
		WEAPON_DUMP_FIELD(turretBarrelSpinRumble);
		WEAPON_DUMP_FIELD(turretBarrelSpinSpeed);
		WEAPON_DUMP_FIELD(turretBarrelSpinUpTime);
		WEAPON_DUMP_FIELD(turretBarrelSpinDownTime);
		WEAPON_DUMP_ASSET(turretBarrelSpinMaxSnd);
		WEAPON_DUMP_ASSET(missileConeSoundAlias);
		WEAPON_DUMP_ASSET(missileConeSoundAliasAtBase);
		WEAPON_DUMP_FIELD(missileConeSoundRadiusAtTop);
		WEAPON_DUMP_FIELD(missileConeSoundRadiusAtBase);
		WEAPON_DUMP_FIELD(missileConeSoundHeight);
		WEAPON_DUMP_FIELD(missileConeSoundOriginOffset);
		WEAPON_DUMP_FIELD(missileConeSoundVolumescaleAtCore);
		WEAPON_DUMP_FIELD(missileConeSoundVolumescaleAtEdge);
		WEAPON_DUMP_FIELD(missileConeSoundVolumescaleCoreSize);
		WEAPON_DUMP_FIELD(missileConeSoundPitchAtTop);
		WEAPON_DUMP_FIELD(missileConeSoundPitchAtBottom);
		WEAPON_DUMP_FIELD(missileConeSoundPitchTopSize);
		WEAPON_DUMP_FIELD(missileConeSoundPitchBottomSize);
		WEAPON_DUMP_FIELD(missileConeSoundCrossfadeTopSize);
		WEAPON_DUMP_FIELD(missileConeSoundCrossfadeBottomSize);

		data["knifeAttachTagOverride"] = (asset->knifeAttachTagOverride) ?
			SL_ConvertToString(asset->knifeAttachTagOverride) :
			"";

		WEAPON_DUMP_FIELD(knifeAlwaysAttached);
		WEAPON_DUMP_FIELD(meleeOverrideValues);
		WEAPON_DUMP_FIELD(aim_automelee_lerp);
		WEAPON_DUMP_FIELD(aim_automelee_range);
		WEAPON_DUMP_FIELD(aim_automelee_region_height);
		WEAPON_DUMP_FIELD(aim_automelee_region_width);
		WEAPON_DUMP_FIELD(player_meleeHeight);
		WEAPON_DUMP_FIELD(player_meleeRange);
		WEAPON_DUMP_FIELD(player_meleeWidth);
		WEAPON_DUMP_FIELD(sharedAmmo);
		WEAPON_DUMP_FIELD(lockonSupported);
		WEAPON_DUMP_FIELD(requireLockonToFire);
		WEAPON_DUMP_FIELD(isAirburstWeapon);
		WEAPON_DUMP_FIELD(bigExplosion);
		WEAPON_DUMP_FIELD(noAdsWhenMagEmpty);
		WEAPON_DUMP_FIELD(avoidDropCleanup);
		WEAPON_DUMP_FIELD(inheritsPerks);
		WEAPON_DUMP_FIELD(crosshairColorChange);
		WEAPON_DUMP_FIELD(bRifleBullet);
		WEAPON_DUMP_FIELD(armorPiercing);
		WEAPON_DUMP_FIELD(bBoltAction);
		WEAPON_DUMP_FIELD(aimDownSight);
		WEAPON_DUMP_FIELD(canHoldBreath);
		WEAPON_DUMP_FIELD(meleeOnly);
		WEAPON_DUMP_FIELD(canVariableZoom);
		WEAPON_DUMP_FIELD(bRechamberWhileAds);
		WEAPON_DUMP_FIELD(bBulletExplosiveDamage);
		WEAPON_DUMP_FIELD(bCookOffHold);
		WEAPON_DUMP_FIELD(reticleSpin45);
		WEAPON_DUMP_FIELD(bClipOnly);
		WEAPON_DUMP_FIELD(noAmmoPickup);
		WEAPON_DUMP_FIELD(adsFireOnly);
		WEAPON_DUMP_FIELD(cancelAutoHolsterWhenEmpty);
		WEAPON_DUMP_FIELD(disableSwitchToWhenEmpty);
		WEAPON_DUMP_FIELD(suppressAmmoReserveDisplay);
		WEAPON_DUMP_FIELD(laserSightDuringNightvision);
		WEAPON_DUMP_FIELD(markableViewmodel);
		WEAPON_DUMP_FIELD(noDualWield);
		WEAPON_DUMP_FIELD(flipKillIcon);
		WEAPON_DUMP_FIELD(bNoPartialReload);
		WEAPON_DUMP_FIELD(bSegmentedReload);
		WEAPON_DUMP_FIELD(bMultipleReload);
		WEAPON_DUMP_FIELD(blocksProne);
		WEAPON_DUMP_FIELD(silenced);
		WEAPON_DUMP_FIELD(isRollingGrenade);
		WEAPON_DUMP_FIELD(projExplosionEffectForceNormalUp);
		WEAPON_DUMP_FIELD(projExplosionEffectInheritParentDirection);
		WEAPON_DUMP_FIELD(bProjImpactExplode);
		WEAPON_DUMP_FIELD(bProjTrajectoryEvents);
		WEAPON_DUMP_FIELD(bProjWhizByEnabled);
		WEAPON_DUMP_FIELD(stickToPlayers);
		WEAPON_DUMP_FIELD(stickToVehicles);
		WEAPON_DUMP_FIELD(stickToTurrets);
		WEAPON_DUMP_FIELD(thrownSideways);
		WEAPON_DUMP_FIELD(hasDetonator);
		WEAPON_DUMP_FIELD(disableFiring);
		WEAPON_DUMP_FIELD(timedDetonation);
		WEAPON_DUMP_FIELD(rotate);
		WEAPON_DUMP_FIELD(holdButtonToThrow);
		WEAPON_DUMP_FIELD(freezeMovementWhenFiring);
		WEAPON_DUMP_FIELD(thermalScope);
		WEAPON_DUMP_FIELD(thermalToggle);
		WEAPON_DUMP_FIELD(outlineEnemies);
		WEAPON_DUMP_FIELD(altModeSameWeapon);
		WEAPON_DUMP_FIELD(turretBarrelSpinEnabled);
		WEAPON_DUMP_FIELD(missileConeSoundEnabled);
		WEAPON_DUMP_FIELD(missileConeSoundPitchshiftEnabled);
		WEAPON_DUMP_FIELD(missileConeSoundCrossfadeEnabled);
		WEAPON_DUMP_FIELD(offhandHoldIsCancelable);
		WEAPON_DUMP_FIELD(doNotAllowAttachmentsToOverrideSpread);
		WEAPON_DUMP_FIELD(useFastReloadAnims);
		WEAPON_DUMP_FIELD(useScopeDrift);
		WEAPON_DUMP_FIELD(alwaysShatterGlassOnImpact);
		WEAPON_DUMP_FIELD(oldWeapon);

		data["stowTag"] = (asset->stowTag) ?
			SL_ConvertToString(asset->stowTag)
			: "";

		WEAPON_DUMP_ASSET(stowOffsetModel);

		return data;
	}

	json weapon_def::dump_complete(WeaponCompleteDef* asset)
	{
		assert(sizeof(WeaponCompleteDef) == 296);

		json data;
		data["baseAsset"] = asset->szInternalName;
		data["weapDef"] = dump_weapondef(asset->weapDef);

		for (auto i = 0; i < 32; i++)
		{
			if (asset->hideTags && asset->hideTags[i])
			{
				data["hideTags"][i] = SL_ConvertToString(asset->hideTags[i]);
			}
			else
			{
				data["hideTags"][i] = "";
			}
		}

		WEAPON_DUMP_ASSET_ARR(scopes, 6);
		WEAPON_DUMP_ASSET_ARR(underBarrels, 3);
		WEAPON_DUMP_ASSET_ARR(others, 8);

		WEAPON_DUMP_ANIM_ARR(szXAnims, 124);

		for (auto i = 0u; i < asset->numAnimOverrides; i++)
		{
			data["animOverrides"][i]["altmodeAnim"] = (asset->animOverrides[i].altmodeAnim)
				? asset->animOverrides[i].altmodeAnim->name
				: "";
			data["animOverrides"][i]["overrideAnim"] = (asset->animOverrides[i].overrideAnim)
				? asset->animOverrides[i].overrideAnim->name
				: "";
			data["animOverrides"][i]["attachment1"] = asset->animOverrides[i].attachment1;
			data["animOverrides"][i]["attachment2"] = asset->animOverrides[i].attachment2;
			data["animOverrides"][i]["altTime"] = asset->animOverrides[i].altTime;
			data["animOverrides"][i]["animTime"] = asset->animOverrides[i].animTime;
			data["animOverrides"][i]["animTreeType"] = asset->animOverrides[i].animTreeType;
		}

		for (auto i = 0u; i < asset->numSoundOverrides; i++)
		{
			data["soundOverrides"][i]["altmodeSound"] = (asset->soundOverrides[i].altmodeSound)
				? asset->soundOverrides[i].altmodeSound->name
				: "";
			data["soundOverrides"][i]["attachment1"] = asset->soundOverrides[i].attachment1;
			data["soundOverrides"][i]["attachment2"] = asset->soundOverrides[i].attachment2;
			data["soundOverrides"][i]["overrideSound"] = (asset->soundOverrides[i].overrideSound)
				? asset->soundOverrides[i].overrideSound->name
				: "";
			data["soundOverrides"][i]["soundType"] = asset->soundOverrides[i].soundType;
		}

		for (auto i = 0u; i < asset->numFXOverrides; i++)
		{
			data["fxOverrides"][i]["altmodeFX"] = (asset->fxOverrides[i].altmodeFX)
				? asset->fxOverrides[i].altmodeFX->name
				: "";
			data["fxOverrides"][i]["attachment1"] = asset->fxOverrides[i].attachment1;
			data["fxOverrides"][i]["attachment2"] = asset->fxOverrides[i].attachment2;
			data["fxOverrides"][i]["fxType"] = asset->fxOverrides[i].fxType;
			data["fxOverrides"][i]["overrideFX"] = (asset->fxOverrides[i].overrideFX)
				? asset->fxOverrides[i].overrideFX->name
				: "";
		}

		for (auto i = 0u; i < asset->numReloadStateTimerOverrides; i++)
		{
			data["reloadOverrides"][i]["attachment"] = asset->reloadOverrides[i].attachment;
			data["reloadOverrides"][i]["reloadAddTime"] = asset->reloadOverrides[i].reloadAddTime;
			data["reloadOverrides"][i]["reloadEmptyAddTime"] = asset->reloadOverrides[i].reloadEmptyAddTime;
			data["reloadOverrides"][i]["reloadStartAddTime"] = asset->reloadOverrides[i].reloadStartAddTime;
		}

		for (auto i = 0u; i < asset->numNotetrackOverrides; i++)
		{
			data["notetrackOverrides"][i]["attachment"] = asset->notetrackOverrides[i].attachment;

			for (auto j = 0u; j < 24; j++)
			{
				data["notetrackOverrides"][i]["notetrackSoundMapKeys"][j] = (asset->notetrackOverrides[i].notetrackSoundMapKeys[j])
					? SL_ConvertToString(asset->notetrackOverrides[i].notetrackSoundMapKeys[j])
					: "";

				data["notetrackOverrides"][i]["notetrackSoundMapValues"][j] = (asset->notetrackOverrides[i].notetrackSoundMapValues[j])
					? SL_ConvertToString(asset->notetrackOverrides[i].notetrackSoundMapValues[j])
					: "";
			}
		}

		WEAPON_DUMP_FIELD(szInternalName);
		WEAPON_DUMP_FIELD(szDisplayName);
		WEAPON_DUMP_FIELD(numAnimOverrides);
		WEAPON_DUMP_FIELD(numSoundOverrides);
		WEAPON_DUMP_FIELD(numFXOverrides);
		WEAPON_DUMP_FIELD(numReloadStateTimerOverrides);
		WEAPON_DUMP_FIELD(numNotetrackOverrides);
		WEAPON_DUMP_FIELD(fAdsZoomFov);
		WEAPON_DUMP_FIELD(iAdsTransInTime);
		WEAPON_DUMP_FIELD(iAdsTransOutTime);
		WEAPON_DUMP_FIELD(iClipSize);
		WEAPON_DUMP_FIELD(impactType);
		WEAPON_DUMP_FIELD(iFireTime);
		WEAPON_DUMP_FIELD(iFireTimeAkimbo);
		WEAPON_DUMP_FIELD(dpadIconRatio);
		WEAPON_DUMP_FIELD(penetrateMultiplier);
		WEAPON_DUMP_FIELD(fAdsViewKickCenterSpeed);
		WEAPON_DUMP_FIELD(fHipViewKickCenterSpeed);
		WEAPON_DUMP_FIELD(szAltWeaponName);
		WEAPON_DUMP_FIELD(altWeapon);
		WEAPON_DUMP_FIELD(iAltRaiseTime);
		WEAPON_DUMP_FIELD(iAltRaiseTimeAkimbo);
		WEAPON_DUMP_ASSET(killIcon);
		WEAPON_DUMP_ASSET(dpadIcon);
		WEAPON_DUMP_FIELD(fireAnimLength);
		WEAPON_DUMP_FIELD(fireAnimLengthAkimbo);
		WEAPON_DUMP_FIELD(iFirstRaiseTime);
		WEAPON_DUMP_FIELD(iFirstRaiseTimeAkimbo);
		WEAPON_DUMP_FIELD(ammoDropStockMax);
		WEAPON_DUMP_FIELD(adsDofStart);
		WEAPON_DUMP_FIELD(adsDofEnd);
		WEAPON_DUMP_FIELD_ARR(accuracyGraphKnotCount, 2);
		// float(*accuracyGraphKnots[2])[2]);
		WEAPON_DUMP_FIELD(motionTracker);
		WEAPON_DUMP_FIELD(enhanced);
		WEAPON_DUMP_FIELD(dpadIconShowsAmmo);
		WEAPON_DUMP_FIELD(szAdsrBaseSetting);

		return data;
	}

	void weapon_def::dump(WeaponCompleteDef* asset)
	{
		const auto path = "weapons\\"s + asset->name + ".json"s;
		std::string json = dump_complete(asset).dump(4);

		auto file = filesystem::file(path);
		file.open("wb");
		file.write(json.data(), json.size(), 1);
		file.close();
	}
}