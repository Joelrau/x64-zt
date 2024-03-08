#include <std_include.hpp>
#include "weapondef.hpp"

namespace zonetool::iw7
{
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

#define MODVAL(__field__, __size__) \
	if (__size__) assert(__size__ == static_cast<decltype(__size__)>(data[#__field__].size())); \
	__size__ = static_cast<decltype(__size__)>(data[#__field__].size()); \

#define WEAPON_PARSE_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	!data[#__field__].is_null() ? asset->__field__ = mem->duplicate_string(data[#__field__].get<std::string>()) : asset->__field__ = nullptr;

#define WEAPON_PARSE_STRING_ARR(__field__, __size__) \
	if(!data[#__field__].is_null()) \
	{ \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			!data[#__field__][idx##__field__].is_null() && !data[#__field__][idx##__field__].get<std::string>().empty() ? asset->__field__[idx##__field__] = mem->duplicate_string(data[#__field__][idx##__field__].get<std::string>()) : asset->__field__[idx##__field__] = nullptr; \
		} \
	}
#define WEAPON_PARSE_STRING_ARR_MODVAL(__field__, __size__) \
	MODVAL(__field__, __size__) \
	WEAPON_PARSE_STRING_ARR(__field__, __size__)

#define WEAPON_PARSE_SCRIPT_STRING(__field__) \
	if (!data[#__field__].is_null() && !data[#__field__].get<std::string>().empty()) this->add_script_string(&asset->__field__, mem->duplicate_string(data[#__field__].get<std::string>())); asset->__field__ = scr_string_t(0);

#define WEAPON_PARSE_SCRIPT_STRING_ALLOC_ARR(__field__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		asset->__field__ = mem->allocate<scr_string_t>(__size__); \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			if (!data[#__field__][idx##__field__].is_null() && !data[#__field__][idx##__field__].get<std::string>().empty()) this->add_script_string(&asset->__field__[idx##__field__], mem->duplicate_string(data[#__field__][idx##__field__].get<std::string>())); asset->__field__[idx##__field__] = scr_string_t(0); \
		} \
	}
#define WEAPON_PARSE_SCRIPT_STRING_ALLOC_ARR_MODVAL(__field__, __size__) \
	MODVAL(__field__, __size__) \
	WEAPON_PARSE_SCRIPT_STRING_ALLOC_ARR(__field__, __size__)

#define WEAPON_PARSE_FIELD(__field__) \
	if (!data[#__field__].is_null()) asset->__field__ = data[#__field__].get<decltype(asset->__field__)>();

#define WEAPON_PARSE_FIELD_ARR(__field__, __size__) \
	if(!data[#__field__].is_null()) \
	{ \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			asset->__field__[idx##__field__] = data[#__field__][idx##__field__].get<typename std::remove_reference<decltype(asset->__field__[idx##__field__])>::type>(); \
		} \
	}
#define WEAPON_PARSE_FIELD_ARR_MODVAL(__field__, __size__) \
	MODVAL(__field__, __size__) \
	WEAPON_PARSE_FIELD_ARR(__field__, __size__)

#define WEAPON_PARSE_FIELD_ALLOC_ARR(__field__, __size__) \
	if(!data[#__field__].is_null()) \
	{ \
		asset->__field__ = mem->allocate<typename std::remove_reference<decltype(*asset->__field__)>::type>(__size__); \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			asset->__field__[idx##__field__] = data[#__field__][idx##__field__].get<typename std::remove_reference<decltype(asset->__field__[idx##__field__])>::type>(); \
		} \
	} \
	else \
	{ \
		asset->__field__ = nullptr; \
	}
#define WEAPON_PARSE_FIELD_ALLOC_ARR_MODVAL(__field__, __size__) \
	MODVAL(__field__, __size__) \
	WEAPON_PARSE_FIELD_ALLOC_ARR(__field__, __size__)

#define WEAPON_PARSE_ASSET_ALLOC_ARR(__field__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		asset->__field__ = mem->allocate<typename std::remove_reference<decltype(*asset->__field__)>::type>(__size__); \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			if (!data[#__field__][idx##__field__].is_null() && !data[#__field__][idx##__field__].get<std::string>().empty()) \
			{ \
				asset->__field__[idx##__field__] = mem->manual_allocate<typename std::remove_reference<decltype(*asset->__field__[idx##__field__])>::type>(sizeof(const char*)); \
				asset->__field__[idx##__field__]->name = mem->duplicate_string(data[#__field__][idx##__field__].get<std::string>()); \
			} \
			else \
			{ \
				asset->__field__[idx##__field__] = nullptr; \
			} \
		} \
	} \
	else \
	{ \
		asset->__field__ = nullptr; \
	}
#define WEAPON_PARSE_ASSET_ALLOC_ARR_MODVAL(__field__, __size__) \
	MODVAL(__field__, __size__) \
	WEAPON_PARSE_ASSET_ALLOC_ARR(__field__, __size__)

#define WEAPON_PARSE_ASSET_FIELD_ARR(__field__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			if (!data[#__field__][idx##__field__].is_null() && !data[#__field__][idx##__field__].get<std::string>().empty()) \
			{ \
				asset->__field__[idx##__field__] = mem->manual_allocate<typename std::remove_reference<decltype(*asset->__field__[idx##__field__])>::type>(sizeof(const char*)); \
				asset->__field__[idx##__field__]->name = mem->duplicate_string(data[#__field__][idx##__field__].get<std::string>()); \
			} \
			else \
			{ \
				asset->__field__[idx##__field__] = nullptr; \
			} \
		} \
	}
#define WEAPON_PARSE_ASSET_ARR_MODVAL(__field__, __size__) \
	MODVAL(__field__, __size__) \
	WEAPON_PARSE_ASSET_ARR(__field__, __size__)

#define WEAPON_PARSE_ASSET(__field__) \
	if (!data[#__field__].is_null() && !data[#__field__].get<std::string>().empty()) \
	{ \
		asset->__field__ = mem->manual_allocate<typename std::remove_reference<decltype(*asset->__field__)>::type>(sizeof(const char*)); \
		asset->__field__->name = mem->duplicate_string(data[#__field__].get<std::string>()); \
	} \
	else \
	{ \
		asset->__field__ = nullptr; \
	}

#define WEAPON_PARSE_FXCOMBINED(__field__) \
	if (!data[#__field__].is_null()) \
	{ \
		asset->__field__.u.data = mem->manual_allocate<char>(sizeof(const char*)); \
		asset->__field__.type = data[#__field__]["type"].get<FxCombinedType>(); \
		asset->__field__.u.fx->name = mem->duplicate_string(data[#__field__]["fx"].get<std::string>()); \
	} \
	else \
	{ \
		asset->__field__.u.data = nullptr; \
	}

#define WEAPON_PARSE_FXCOMBINED_ALLOC_ARR(__field__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		asset->__field__ = mem->allocate<FxCombinedDef>(__size__); \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			if (!data[#__field__][idx##__field__].is_null()) \
			{ \
				asset->__field__[idx##__field__].u.data = mem->manual_allocate<char>(sizeof(const char*)); \
				asset->__field__[idx##__field__].type = data[#__field__][idx##__field__]["type"].get<FxCombinedType>(); \
				asset->__field__[idx##__field__].u.fx->name = mem->duplicate_string(data[#__field__][idx##__field__]["fx"].get<std::string>()); \
			} \
			else \
			{ \
				asset->__field__[idx##__field__].u.data = nullptr; \
			} \
		} \
	} \
	else \
	{ \
		asset->__field__ = nullptr; \
	}
#define WEAPON_PARSE_FXCOMBINED_ALLOC_ARR_MODVAL(__field__, __size__) \
	MODVAL(__field__, __size__) \
	WEAPON_PARSE_FXCOMBINED_ALLOC_ARR(__field__, __size__)

	void weapon_def::parse(AnimOverride* asset, ordered_json& data, zone_memory* mem)
	{
		WEAPON_PARSE_SCRIPT_STRING(attachment1);
		WEAPON_PARSE_SCRIPT_STRING(attachment2);
		WEAPON_PARSE_ASSET(overrides);
		WEAPON_PARSE_ASSET(overridesAlt);
	}

	void weapon_def::parse(SFXOverride* asset, ordered_json& data, zone_memory* mem)
	{
		WEAPON_PARSE_SCRIPT_STRING(attachment1);
		WEAPON_PARSE_SCRIPT_STRING(attachment2);
		WEAPON_PARSE_ASSET(overrides);
		WEAPON_PARSE_ASSET(overridesAlt);
	}

	void weapon_def::parse(VFXOverride* asset, ordered_json& data, zone_memory* mem)
	{
		WEAPON_PARSE_SCRIPT_STRING(attachment1);
		WEAPON_PARSE_SCRIPT_STRING(attachment2);
		WEAPON_PARSE_ASSET(overrides);
		WEAPON_PARSE_ASSET(overridesAlt);
	}

	void weapon_def::parse(GrenadeRotationParams* asset, ordered_json& data, zone_memory* mem)
	{
		WEAPON_PARSE_FIELD(initialPitch);
		WEAPON_PARSE_FIELD(initialYaw);
		WEAPON_PARSE_FIELD(initialRoll);

		WEAPON_PARSE_FIELD(rotationPitchDir);
		WEAPON_PARSE_FIELD(rotationPitchMin);
		WEAPON_PARSE_FIELD(rotationPitchMax);

		WEAPON_PARSE_FIELD(rotationYawDir);
		WEAPON_PARSE_FIELD(rotationYawMin);
		WEAPON_PARSE_FIELD(rotationYawMax);

		WEAPON_PARSE_FIELD(rotationRollDir);
		WEAPON_PARSE_FIELD(rotationRollMin);
		WEAPON_PARSE_FIELD(rotationRollMax);

		WEAPON_PARSE_FIELD(rotate);
	}

	void weapon_def::parse(ADSOverlay* asset, ordered_json& data, zone_memory* mem)
	{
		WEAPON_PARSE_ASSET(shader);
		WEAPON_PARSE_ASSET(shaderLowRes);
		WEAPON_PARSE_ASSET(shaderEMP);
		WEAPON_PARSE_ASSET(shaderEMPLowRes);
		WEAPON_PARSE_FIELD(reticle);
		WEAPON_PARSE_FIELD(width);
		WEAPON_PARSE_FIELD(height);
		WEAPON_PARSE_FIELD(widthSplitscreen);
		WEAPON_PARSE_FIELD(heightSplitscreen);
	}

	void weapon_def::parse_weapondef(WeaponDef* asset, WeaponCompleteDef* parent, ordered_json& data, zone_memory* mem)
	{
		WEAPON_PARSE_STRING(szOverlayName);
		WEAPON_PARSE_STRING(szModeName);
		WEAPON_PARSE_STRING(szAmmoName);
		WEAPON_PARSE_STRING(szSharedAmmoCapName);
		WEAPON_PARSE_STRING(szUseHintString);
		WEAPON_PARSE_STRING(dropHintString);
		WEAPON_PARSE_STRING(szScript);

		WEAPON_PARSE_ASSET(gunXModel);
		WEAPON_PARSE_ASSET(gunXModelLeftHand);
		WEAPON_PARSE_ASSET(gunXModelRightHand);
		WEAPON_PARSE_ASSET(defaultViewModel);
		WEAPON_PARSE_ASSET(defaultWorldModelLeftHand);
		WEAPON_PARSE_ASSET(defaultWorldModelRightHand);
		WEAPON_PARSE_ASSET(worldModel);
		WEAPON_PARSE_ASSET(worldXModelLeftHand);
		WEAPON_PARSE_ASSET(worldXModelRightHand);
		WEAPON_PARSE_ASSET(defaultWorldModel);
		WEAPON_PARSE_ASSET(playerShadowModel);
		WEAPON_PARSE_ASSET(playerShadowModelLeftHand);
		WEAPON_PARSE_ASSET(playerShadowModelRightHand);
		WEAPON_PARSE_ASSET(handXModel);
		WEAPON_PARSE_ASSET_ALLOC_ARR_MODVAL(complexWorldModel, asset->numComplexModels);
		WEAPON_PARSE_ASSET_ALLOC_ARR_MODVAL(complexWorldModelLeftHand, asset->numComplexModels);
		WEAPON_PARSE_ASSET_ALLOC_ARR_MODVAL(complexWorldModelRightHand, asset->numComplexModels);
		WEAPON_PARSE_ASSET_ALLOC_ARR_MODVAL(complexGunXModel, asset->numComplexModels);
		WEAPON_PARSE_ASSET_ALLOC_ARR_MODVAL(complexGunXModelLeftHand, asset->numComplexModels);
		WEAPON_PARSE_ASSET_ALLOC_ARR_MODVAL(complexGunXModelRightHand, asset->numComplexModels);

		WEAPON_PARSE_ASSET_FIELD_ARR(camoWorldModelMaterialOverride, 16);
		WEAPON_PARSE_ASSET_FIELD_ARR(camoViewModelMaterialOverride, 16);

		WEAPON_PARSE_ASSET(szXAnims);
		WEAPON_PARSE_ASSET(szXAnimsRightHanded);
		WEAPON_PARSE_ASSET(szXAnimsLeftHanded);

		WEAPON_PARSE_SCRIPT_STRING_ALLOC_ARR(notetrackRumbleMapKeys, 16);
		WEAPON_PARSE_SCRIPT_STRING_ALLOC_ARR(notetrackRumbleMapValues, 16);
		WEAPON_PARSE_SCRIPT_STRING_ALLOC_ARR(notetrackFXMapKeys, 16);
		WEAPON_PARSE_FXCOMBINED_ALLOC_ARR(notetrackFXMapValues, 16);
		WEAPON_PARSE_SCRIPT_STRING_ALLOC_ARR(notetrackFXMapTagValues, 16);

		WEAPON_PARSE_FIELD(playerAnimType);
		WEAPON_PARSE_FIELD(scriptedAnimEvent);
		WEAPON_PARSE_FIELD(scriptedAnimType);
		WEAPON_PARSE_FIELD(weapType);
		WEAPON_PARSE_FIELD(whizbyType);
		WEAPON_PARSE_FIELD(weapClass);
		WEAPON_PARSE_FIELD(penetrateType);
		WEAPON_PARSE_FIELD(deathAnimDamageType);
		WEAPON_PARSE_FIELD(inventoryType);
		WEAPON_PARSE_FIELD(fireType);

		WEAPON_PARSE_FIELD(targetAssistType);
		WEAPON_PARSE_FIELD(targetAssistBehavior);
		WEAPON_PARSE_FIELD(targetAssistRange);
		WEAPON_PARSE_FIELD(targetAssistAngle);
		WEAPON_PARSE_FIELD(targetAssistLosOffsetForward);
		WEAPON_PARSE_FIELD(targetAssistLosOffsetRight);
		WEAPON_PARSE_FIELD(targetAssistLosOffsetUp);
		WEAPON_PARSE_FIELD(targetAssistOnlyAvailableInAds);

		WEAPON_PARSE_FIELD(offhandClass);
		WEAPON_PARSE_FIELD(stance);

		WEAPON_PARSE_FIELD(stickiness);
		WEAPON_PARSE_FIELD(dualWieldType);
		WEAPON_PARSE_FIELD(slot);

		WEAPON_PARSE_FIELD(iFireTime);
		WEAPON_PARSE_FIELD(iFireDelay);
		WEAPON_PARSE_FIELD(iFireTimeAkimbo);
		WEAPON_PARSE_FIELD(iFireDelayAkimbo);
		WEAPON_PARSE_FIELD(bSegmentedReload);
		WEAPON_PARSE_FIELD(burstFireCooldown);
		WEAPON_PARSE_FIELD(burstFireAuto);

		WEAPON_PARSE_ASSET(vfxPackage);
		WEAPON_PARSE_ASSET(sfxPackage);

		WEAPON_PARSE_FIELD(reticleType);
		WEAPON_PARSE_ASSET(reticleCenter);
		WEAPON_PARSE_ASSET(reticleSide);
		WEAPON_PARSE_ASSET(reticleOnePiece);
		WEAPON_PARSE_FIELD(iReticleCenterSize);
		WEAPON_PARSE_FIELD(iReticleSideSize);
		WEAPON_PARSE_FIELD(iReticleMinOfs);
		WEAPON_PARSE_FIELD(reticleCenterPulse);
		WEAPON_PARSE_FIELD(activeReticleType);

		WEAPON_PARSE_FIELD_ARR(vStandMove, 3);
		WEAPON_PARSE_FIELD_ARR(vStandRot, 3);
		WEAPON_PARSE_FIELD_ARR(strafeMove, 3);
		WEAPON_PARSE_FIELD_ARR(strafeRot, 3);
		WEAPON_PARSE_FIELD_ARR(vDuckedOfs, 3);
		WEAPON_PARSE_FIELD_ARR(vDuckedMove, 3);
		WEAPON_PARSE_FIELD_ARR(vDuckedRot, 3);
		WEAPON_PARSE_FIELD_ARR(vProneOfs, 3);
		WEAPON_PARSE_FIELD_ARR(vProneMove, 3);
		WEAPON_PARSE_FIELD_ARR(vProneRot, 3);
		WEAPON_PARSE_FIELD(fPosMoveRate);
		WEAPON_PARSE_FIELD(fPosProneMoveRate);
		WEAPON_PARSE_FIELD(fStandMoveMinSpeed);
		WEAPON_PARSE_FIELD(fDuckedMoveMinSpeed);
		WEAPON_PARSE_FIELD(fProneMoveMinSpeed);
		WEAPON_PARSE_FIELD(fPosRotRate);
		WEAPON_PARSE_FIELD(fPosProneRotRate);
		WEAPON_PARSE_FIELD(fStandRotMinSpeed);
		WEAPON_PARSE_FIELD(fDuckedRotMinSpeed);
		WEAPON_PARSE_FIELD(fProneRotMinSpeed);

		WEAPON_PARSE_ASSET(worldClipModel);
		WEAPON_PARSE_ASSET(rocketModel);
		WEAPON_PARSE_ASSET(knifeModel);
		WEAPON_PARSE_ASSET(worldKnifeModel);

		WEAPON_PARSE_ASSET(hudIcon);
		WEAPON_PARSE_ASSET(pickupIcon);
		WEAPON_PARSE_ASSET(dangerIcon);
		WEAPON_PARSE_ASSET(throwBackIcon);

		WEAPON_PARSE_FIELD(hideWarningIcons);
		WEAPON_PARSE_FIELD(warningIconsDelay);
		WEAPON_PARSE_ASSET(ammoCounterIcon);
		WEAPON_PARSE_FIELD(ammoCounterClip);

		WEAPON_PARSE_FIELD(iStartAmmo);
		WEAPON_PARSE_FIELD(iPerkStartAmmo);
		WEAPON_PARSE_FIELD(iPerkMaxAmmo);
		//WEAPON_PARSE_FIELD(iAmmoIndex); // runtime
		WEAPON_PARSE_FIELD(iMaxAmmo);
		WEAPON_PARSE_FIELD(shotCount);
		//WEAPON_PARSE_FIELD(iSharedAmmoCapIndex); // runtime
		WEAPON_PARSE_FIELD(iSharedAmmoCap);
		WEAPON_PARSE_FIELD(iAmmoUsedPerShot);
		WEAPON_PARSE_FIELD(requireAmmoUsedPerShot);
		WEAPON_PARSE_FIELD(lowAmmoWarningThreshold);
		WEAPON_PARSE_FIELD(disableNoAmmoWarning);
		WEAPON_PARSE_FIELD(damage);
		WEAPON_PARSE_FIELD(playerDamage);
		WEAPON_PARSE_FIELD(meleeDamage);
		WEAPON_PARSE_FIELD(iDamageType);

		WEAPON_PARSE_FIELD(autoAimRange);
		WEAPON_PARSE_FIELD(aimAssistRange);
		WEAPON_PARSE_FIELD(aimAssistRangeAds);
		WEAPON_PARSE_FIELD(aimAssistPitchSlowdown);
		WEAPON_PARSE_FIELD(aimAssistPitchSlowdownAds);
		WEAPON_PARSE_FIELD(aimAssistYawSlowdown);
		WEAPON_PARSE_FIELD(aimAssistYawSlowdownAds);
		WEAPON_PARSE_FIELD(aimAssistLockonStrength);
		WEAPON_PARSE_FIELD(aimPadding);
		WEAPON_PARSE_FIELD(enemyCrosshairRange);
		WEAPON_PARSE_FIELD(moveSpeedScale);
		WEAPON_PARSE_FIELD(adsMoveSpeedScale);
		WEAPON_PARSE_FIELD(sprintDurationScale);
		WEAPON_PARSE_FIELD(sprintRestoreDelay);
		WEAPON_PARSE_FIELD(fAdsZoomInFrac);
		WEAPON_PARSE_FIELD(fAdsZoomOutFrac);

		parse(&asset->overlay, data["overlay"], mem);
		WEAPON_PARSE_FIELD(overlayInterface);

		WEAPON_PARSE_FIELD(fAdsBobFactor);
		WEAPON_PARSE_FIELD(fAdsViewBobMult);
		WEAPON_PARSE_FIELD(fHipSpreadStandMin);
		WEAPON_PARSE_FIELD(fHipSpreadDuckedMin);
		WEAPON_PARSE_FIELD(fHipSpreadProneMin);
		WEAPON_PARSE_FIELD(hipSpreadSprintMin);
		WEAPON_PARSE_FIELD(hipSpreadInAirMin);
		WEAPON_PARSE_FIELD(hipSpreadStandMax);
		WEAPON_PARSE_FIELD(hipSpreadDuckedMax);
		WEAPON_PARSE_FIELD(hipSpreadProneMax);
		WEAPON_PARSE_FIELD(hipSpreadSprintMax);
		WEAPON_PARSE_FIELD(hipSpreadInAirMax);
		WEAPON_PARSE_FIELD(fHipSpreadDecayRate);
		WEAPON_PARSE_FIELD(fHipSpreadFireAdd);
		WEAPON_PARSE_FIELD(fHipSpreadTurnAdd);
		WEAPON_PARSE_FIELD(fHipSpreadMoveAdd);
		WEAPON_PARSE_FIELD(fHipSpreadDuckedDecay);
		WEAPON_PARSE_FIELD(fHipSpreadProneDecay);
		WEAPON_PARSE_FIELD(hipSpreadSprintDecay);
		WEAPON_PARSE_FIELD(hipSpreadInAirDecay);
		WEAPON_PARSE_FIELD(fHipReticleSidePos);
		WEAPON_PARSE_FIELD(fAdsIdleAmount);
		WEAPON_PARSE_FIELD(fHipIdleAmount);
		WEAPON_PARSE_FIELD(adsIdleSpeed);
		WEAPON_PARSE_FIELD(hipIdleSpeed);
		WEAPON_PARSE_FIELD(fIdleCrouchFactor);
		WEAPON_PARSE_FIELD(fIdleProneFactor);
		WEAPON_PARSE_FIELD(fGunMaxPitch);
		WEAPON_PARSE_FIELD(fGunMaxYaw);
		WEAPON_PARSE_FIELD(adsIdleLerpStartTime);
		WEAPON_PARSE_FIELD(adsIdleLerpTime);
		WEAPON_PARSE_FIELD(slideSpreadMin);
		WEAPON_PARSE_FIELD(slideSpreadMax);
		WEAPON_PARSE_FIELD(slideSpreadDecayRate);
		WEAPON_PARSE_FIELD(slideSpreadFireAdd);
		WEAPON_PARSE_FIELD(slideSpreadTurnAdd);
		WEAPON_PARSE_FIELD(swayMaxAngleSteadyAim);
		WEAPON_PARSE_FIELD(swayMaxAngle);
		WEAPON_PARSE_FIELD(swayLerpSpeed);
		WEAPON_PARSE_FIELD(swayPitchScale);
		WEAPON_PARSE_FIELD(swayYawScale);
		WEAPON_PARSE_FIELD(swayHorizScale);
		WEAPON_PARSE_FIELD(swayVertScale);
		WEAPON_PARSE_FIELD(swayShellShockScale);
		WEAPON_PARSE_FIELD(adsSwayMaxAngle);
		WEAPON_PARSE_FIELD(adsSwayLerpSpeed);
		WEAPON_PARSE_FIELD(adsSway_UNKNOWN_Scale); // fixme
		WEAPON_PARSE_FIELD(adsSwayPitchScale);
		WEAPON_PARSE_FIELD(adsSwayYawScale);
		WEAPON_PARSE_FIELD(adsSwayHorizScale);
		WEAPON_PARSE_FIELD(adsSwayVertScale);
		WEAPON_PARSE_FIELD(adsFireRateScale);
		WEAPON_PARSE_FIELD(adsDamageRangeScale);
		WEAPON_PARSE_FIELD(adsFireAnimFrac);
		WEAPON_PARSE_FIELD(fireTimerLerpToAdsScale);
		WEAPON_PARSE_FIELD(alwaysFireAtMaxRangeInAds);

		WEAPON_PARSE_FIELD(dualWieldViewModelOffset);

		WEAPON_PARSE_FIELD(fScopeDriftDelay);
		WEAPON_PARSE_FIELD(fScopeDriftLerpInTime);
		WEAPON_PARSE_FIELD(fScopeDriftSteadyTime);
		WEAPON_PARSE_FIELD(fScopeDriftLerpOutTime);
		WEAPON_PARSE_FIELD(fScopeDriftSteadyFactor);
		WEAPON_PARSE_FIELD(fScopeDriftUnsteadyFactor);

		WEAPON_PARSE_FIELD(killIconRatio);

		WEAPON_PARSE_FIELD(iReloadAmmoAdd);
		WEAPON_PARSE_FIELD(iReloadStartAdd);
		WEAPON_PARSE_FIELD(ammoDropStockMin);
		WEAPON_PARSE_FIELD(ammoDropClipPercentMin);
		WEAPON_PARSE_FIELD(ammoDropClipPercentMax);

		WEAPON_PARSE_FIELD(iExplosionRadius);
		WEAPON_PARSE_FIELD(iExplosionRadiusMin);
		WEAPON_PARSE_FIELD(iExplosionForceRadius);
		WEAPON_PARSE_FIELD(iExplosionInnerDamage);
		WEAPON_PARSE_FIELD(iExplosionOuterDamage);
		WEAPON_PARSE_FIELD(damageConeAngle);
		WEAPON_PARSE_FIELD(bulletExplDmgMult);
		WEAPON_PARSE_FIELD(bulletExplRadiusMult);
		WEAPON_PARSE_FIELD(iProjectileSpeed);
		WEAPON_PARSE_FIELD(iProjectileSpeedUp);
		WEAPON_PARSE_FIELD(iProjectileSpeedForward);
		WEAPON_PARSE_FIELD(iProjectileActivateDist);
		WEAPON_PARSE_FIELD(iProjectileDetonationRadius);
		WEAPON_PARSE_FIELD(projLifetime);
		WEAPON_PARSE_FIELD(projLifetimeStdDeviation);
		WEAPON_PARSE_FIELD(timeToAccelerate);
		WEAPON_PARSE_FIELD(projectileCurvature);

		WEAPON_PARSE_ASSET(projectileModel);
		WEAPON_PARSE_FXCOMBINED(projExplosionEffect);
		WEAPON_PARSE_FXCOMBINED(projDudEffect);
		WEAPON_PARSE_FXCOMBINED(projBodyEffect);
		WEAPON_PARSE_FXCOMBINED(projTrailEffect);
		WEAPON_PARSE_FXCOMBINED(projBeaconEffect);
		WEAPON_PARSE_FXCOMBINED(projIgnitionEffect);

		WEAPON_PARSE_FIELD(ricochetChance);
		WEAPON_PARSE_FIELD(riotShieldEnableDamage);
		WEAPON_PARSE_FIELD(riotShieldHealth);
		WEAPON_PARSE_FIELD(riotShieldDamageMult);

		WEAPON_PARSE_FIELD_ARR(vProjectileColor, 3);
		WEAPON_PARSE_FIELD(guidedMissileType);
		WEAPON_PARSE_FIELD(maxSteeringAccel);
		WEAPON_PARSE_FIELD(projIgnitionDelay);

		WEAPON_PARSE_FIELD_ALLOC_ARR(parallelBounce, 64);
		WEAPON_PARSE_FIELD_ALLOC_ARR(perpendicularBounce, 64);

		WEAPON_PARSE_FIELD(fAdsAimPitch);
		WEAPON_PARSE_FIELD(fAdsCrosshairInFrac);
		WEAPON_PARSE_FIELD(fAdsCrosshairOutFrac);
		WEAPON_PARSE_FIELD(adsInCrosshairAlphaStart);
		WEAPON_PARSE_FIELD(adsInCrosshairAlphaEnd);
		WEAPON_PARSE_FIELD(adsOutCrosshairAlphaStart);
		WEAPON_PARSE_FIELD(adsOutCrosshairAlphaEnd);
		WEAPON_PARSE_FIELD(adsShouldShowCrosshair);
		WEAPON_PARSE_FIELD(adsGunKickReducedKickBullets);
		WEAPON_PARSE_FIELD(adsGunKickReducedKickPercent);
		WEAPON_PARSE_FIELD(fAdsGunKickPitchMin);
		WEAPON_PARSE_FIELD(fAdsGunKickPitchMax);
		WEAPON_PARSE_FIELD(fAdsGunKickYawMin);
		WEAPON_PARSE_FIELD(fAdsGunKickYawMax);
		WEAPON_PARSE_FIELD(fAdsGunKickMagMin);
		WEAPON_PARSE_FIELD(fAdsGunKickAccel);
		WEAPON_PARSE_FIELD(fAdsGunKickSpeedMax);
		WEAPON_PARSE_FIELD(fAdsGunKickSpeedDecay);
		WEAPON_PARSE_FIELD(fAdsGunKickStaticDecay);
		WEAPON_PARSE_FIELD(fAdsViewKickPitchMin);
		WEAPON_PARSE_FIELD(fAdsViewKickPitchMax);
		WEAPON_PARSE_FIELD(fAdsViewKickYawMin);
		WEAPON_PARSE_FIELD(fAdsViewKickYawMax);
		WEAPON_PARSE_FIELD(fAdsViewKickMagMin);
		WEAPON_PARSE_FIELD(fAdsViewScatterMin);
		WEAPON_PARSE_FIELD(fAdsViewScatterMax);
		WEAPON_PARSE_FIELD(fAdsSpread);

		WEAPON_PARSE_FIELD(iVisibilityAxis);
		WEAPON_PARSE_FIELD(fVisibilityUpOffset);
		WEAPON_PARSE_FIELD(hipGunKickReducedKickBullets);
		WEAPON_PARSE_FIELD(hipGunKickReducedKickPercent);
		WEAPON_PARSE_FIELD(fHipGunKickPitchMin);
		WEAPON_PARSE_FIELD(fHipGunKickPitchMax);
		WEAPON_PARSE_FIELD(fHipGunKickYawMin);
		WEAPON_PARSE_FIELD(fHipGunKickYawMax);
		WEAPON_PARSE_FIELD(fHipGunKickMagMin);
		WEAPON_PARSE_FIELD(fHipGunKickAccel);
		WEAPON_PARSE_FIELD(fHipGunKickSpeedMax);
		WEAPON_PARSE_FIELD(fHipGunKickSpeedDecay);
		WEAPON_PARSE_FIELD(fHipGunKickStaticDecay);
		WEAPON_PARSE_FIELD(fHipViewKickPitchMin);
		WEAPON_PARSE_FIELD(fHipViewKickPitchMax);
		WEAPON_PARSE_FIELD(fHipViewKickYawMin);
		WEAPON_PARSE_FIELD(fHipViewKickYawMax);
		WEAPON_PARSE_FIELD(fHipViewKickMagMin);
		WEAPON_PARSE_FIELD(fHipViewScatterMin);
		WEAPON_PARSE_FIELD(fHipViewScatterMax);

		WEAPON_PARSE_FIELD(multipleReloadClipPercentage);
		WEAPON_PARSE_FIELD(kickAlignedInputScalar);
		WEAPON_PARSE_FIELD(kickOpposedInputScalar);
		WEAPON_PARSE_FIELD(fightDist);
		WEAPON_PARSE_FIELD(maxDist);

		for (auto i = 0; i < 2; i++)
		{
			asset->accuracyGraphName[i] = data["accuracyGraphName"][i].empty() ? nullptr : mem->duplicate_string(data["accuracyGraphName"][i].get<std::string>());
			asset->originalAccuracyGraphKnots[i] = mem->allocate<vec2_t>(parent->accuracyGraphKnotCount[i]);
			for (auto ii = 0; ii < parent->accuracyGraphKnotCount[i]; ii++)
			{
				asset->originalAccuracyGraphKnots[i][ii][0] = data["originalAccuracyGraphKnots"][i][ii][0].get<float>();
				asset->originalAccuracyGraphKnots[i][ii][1] = data["originalAccuracyGraphKnots"][i][ii][1].get<float>();
			}
			asset->originalAccuracyGraphKnotCount[i] = data["originalAccuracyGraphKnotCount"][i].get<unsigned short>();
		}

		WEAPON_PARSE_FIELD(iPositionReloadTransTime);
		WEAPON_PARSE_FIELD(leftArc);
		WEAPON_PARSE_FIELD(rightArc);
		WEAPON_PARSE_FIELD(topArc);
		WEAPON_PARSE_FIELD(bottomArc);
		WEAPON_PARSE_FIELD(softLeftRightArc);
		WEAPON_PARSE_FIELD(accuracy);
		WEAPON_PARSE_FIELD(aiSpread);
		WEAPON_PARSE_FIELD(playerSpread);
		WEAPON_PARSE_FIELD_ARR(minTurnSpeed, 2);
		WEAPON_PARSE_FIELD_ARR(maxTurnSpeed, 2);
		WEAPON_PARSE_FIELD(pitchConvergenceTime);
		WEAPON_PARSE_FIELD(yawConvergenceTime);
		WEAPON_PARSE_FIELD(suppressTime);
		WEAPON_PARSE_FIELD(maxRange);
		WEAPON_PARSE_FIELD(fAnimHorRotateInc);
		WEAPON_PARSE_FIELD(fPlayerPositionDist);
		//WEAPON_PARSE_FIELD(iUseHintStringIndex); // runtime
		//WEAPON_PARSE_FIELD(dropHintStringIndex); // runtime
		WEAPON_PARSE_FIELD(horizViewJitter);
		WEAPON_PARSE_FIELD(vertViewJitter);
		WEAPON_PARSE_FIELD(scanSpeed);
		WEAPON_PARSE_FIELD(scanAccel);
		WEAPON_PARSE_FIELD(scanPauseTime);
		WEAPON_PARSE_FIELD_ARR(adsSpeedMs, 2);

		WEAPON_PARSE_FIELD(minDamage);
		WEAPON_PARSE_FIELD(minPlayerDamage);
		WEAPON_PARSE_FIELD(mid1Damage);
		WEAPON_PARSE_FIELD(mid2Damage);
		WEAPON_PARSE_FIELD(mid3Damage);
		WEAPON_PARSE_FIELD(mid1PlayerDamage);
		WEAPON_PARSE_FIELD(mid2PlayerDamage);
		WEAPON_PARSE_FIELD(mid3PlayerDamage);
		WEAPON_PARSE_FIELD(maxDamageRange);
		WEAPON_PARSE_FIELD(mid1DamageRange);
		WEAPON_PARSE_FIELD(mid2DamageRange);
		WEAPON_PARSE_FIELD(mid3DamageRange);
		WEAPON_PARSE_FIELD(minDamageRange);

		WEAPON_PARSE_FIELD(destabilizationRateTime);
		WEAPON_PARSE_FIELD(destabilizationCurvatureMax);
		WEAPON_PARSE_FIELD(destabilizeDistance);

		WEAPON_PARSE_FIELD(robotechMaxPitch);
		WEAPON_PARSE_FIELD(robotechMaxYaw);
		WEAPON_PARSE_FIELD(robotechFrequency);
		WEAPON_PARSE_FIELD(robotechVariancePitch);
		WEAPON_PARSE_FIELD(robotechVarianceYaw);
		WEAPON_PARSE_FIELD(robotechVarianceFrequency);

		WEAPON_PARSE_FIELD_ALLOC_ARR(locationDamageMultipliers, 22);
		WEAPON_PARSE_FIELD_ALLOC_ARR(hitLocPriorityMap, 22);
		WEAPON_PARSE_FIELD_ARR(unittypeMultipliers, 9);

		WEAPON_PARSE_ASSET(fireRumble);
		WEAPON_PARSE_ASSET(meleeImpactRumble);

		WEAPON_PARSE_FIELD(tracerStyle);
		WEAPON_PARSE_ASSET(tracerType);
		WEAPON_PARSE_ASSET(overchargeTracerType);

		WEAPON_PARSE_ASSET(laserTypeFriendly);
		WEAPON_PARSE_ASSET(laserTypeEnemy);

		WEAPON_PARSE_FIELD(turretADSEnabled);
		WEAPON_PARSE_FIELD(turretADSTime);
		WEAPON_PARSE_FIELD(turretFov);
		WEAPON_PARSE_FIELD(turretFovADS);
		WEAPON_PARSE_FIELD(turretScopeZoomRate);
		WEAPON_PARSE_FIELD(turretScopeZoomMin);
		WEAPON_PARSE_FIELD(turretScopeZoomMax);
		WEAPON_PARSE_FIELD(turretOverheatUpRate);
		WEAPON_PARSE_FIELD(turretOverheatDownRate);
		WEAPON_PARSE_FIELD(turretOverheatPenalty);
		WEAPON_PARSE_STRING(turretOverheatSound);
		WEAPON_PARSE_FXCOMBINED(turretOverheatEffect);
		WEAPON_PARSE_ASSET(turretBarrelSpinRumble);
		WEAPON_PARSE_FIELD(turretBarrelSpinSpeed);
		WEAPON_PARSE_FIELD(turretBarrelSpinUpTime);
		WEAPON_PARSE_FIELD(turretBarrelSpinDownTime);
		WEAPON_PARSE_STRING(turretBarrelSpinMaxSnd);
		WEAPON_PARSE_STRING_ARR(turretBarrelSpinUpSnd, 4);
		WEAPON_PARSE_STRING_ARR(turretBarrelSpinDownSnd, 4);
		WEAPON_PARSE_STRING(missileConeSoundAlias);
		WEAPON_PARSE_STRING(missileConeSoundAliasAtBase);
		WEAPON_PARSE_FIELD(missileConeSoundRadiusAtTop);
		WEAPON_PARSE_FIELD(missileConeSoundRadiusAtBase);
		WEAPON_PARSE_FIELD(missileConeSoundHeight);
		WEAPON_PARSE_FIELD(missileConeSoundOriginOffset);
		WEAPON_PARSE_FIELD(missileConeSoundVolumescaleAtCore);
		WEAPON_PARSE_FIELD(missileConeSoundVolumescaleAtEdge);
		WEAPON_PARSE_FIELD(missileConeSoundVolumescaleCoreSize);
		WEAPON_PARSE_FIELD(missileConeSoundPitchAtTop);
		WEAPON_PARSE_FIELD(missileConeSoundPitchAtBottom);
		WEAPON_PARSE_FIELD(missileConeSoundPitchTopSize);
		WEAPON_PARSE_FIELD(missileConeSoundPitchBottomSize);
		WEAPON_PARSE_FIELD(missileConeSoundCrossfadeTopSize);
		WEAPON_PARSE_FIELD(missileConeSoundCrossfadeBottomSize);

		WEAPON_PARSE_SCRIPT_STRING(knifeAttachTagOverride);
		WEAPON_PARSE_SCRIPT_STRING(knifeAttachTagOverride2);
		WEAPON_PARSE_FIELD(knifeAlwaysAttached);
		WEAPON_PARSE_FIELD(meleeOverrideValues);
		WEAPON_PARSE_FIELD(aim_automelee_lerp);
		WEAPON_PARSE_FIELD(aim_automelee_region_height);
		WEAPON_PARSE_FIELD(aim_automelee_region_width);
		WEAPON_PARSE_FIELD(aim_automelee_maxPitchMovement);
		WEAPON_PARSE_FIELD(aim_automelee_maxYawMovement);
		WEAPON_PARSE_FIELD(player_meleeHeight);
		WEAPON_PARSE_FIELD(player_meleeWidth);
		WEAPON_PARSE_FIELD(playerMeleeRangeStanding);
		WEAPON_PARSE_FIELD(playerMeleeRangeCrouched);
		WEAPON_PARSE_FIELD(playerMeleeRangeProne);
		WEAPON_PARSE_FIELD(playerMeleeRangeChargeStanding);
		WEAPON_PARSE_FIELD(playerMeleeRangeChargeCrouched);
		WEAPON_PARSE_FIELD(playerMeleeChargeHeightTolerance);

		WEAPON_PARSE_FIELD(shieldAllowFiring);
		WEAPON_PARSE_FIELD(shieldUnkBool);
		WEAPON_PARSE_FIELD(shieldMaxSpeed);
		WEAPON_PARSE_FIELD(shieldAlwaysDisplay);
		WEAPON_PARSE_ASSET(shieldDeployGesture);
		WEAPON_PARSE_ASSET(shieldFireWeapGesture);
		WEAPON_PARSE_ASSET(shieldDeployWhileFiring);
		WEAPON_PARSE_ASSET(shieldRetractWhileFiring);
		WEAPON_PARSE_ASSET(shieldBashGesture);
		WEAPON_PARSE_FXCOMBINED(shieldMeleeFx);
		WEAPON_PARSE_FIELD(shieldMeleeFxDelay);
		WEAPON_PARSE_FIELD(HitEarthquakeScale);
		WEAPON_PARSE_FIELD(HitEarthquakeDuration);
		WEAPON_PARSE_FIELD(HitEarthquakeRadius);
		WEAPON_PARSE_ASSET(shieldHitRumble);
		WEAPON_PARSE_FIELD(MissEarthquakeScale);
		WEAPON_PARSE_FIELD(MissEarthquakeDuration);
		WEAPON_PARSE_FIELD(MissEarthquakeRadius);
		WEAPON_PARSE_ASSET(shieldMissRumble);
		WEAPON_PARSE_FIELD(shieldDeployButton);
		WEAPON_PARSE_FIELD(shieldUsesEnergy);
		WEAPON_PARSE_FIELD(shieldMaxEnergy);
		WEAPON_PARSE_FIELD(shieldConsumptionRate);
		WEAPON_PARSE_FIELD(shieldMeleeEnergyCost);
		WEAPON_PARSE_FIELD(shieldMeleeHitEnergyCost);

		WEAPON_PARSE_FIELD(reactiveMotionRadiusScale);
		WEAPON_PARSE_FIELD(reactiveMotionFrequencyScale);
		WEAPON_PARSE_FIELD(reactiveMotionAmplitudeScale);
		WEAPON_PARSE_FIELD(reactiveMotionFalloff);
		WEAPON_PARSE_FIELD(reactiveMotionLifetime);

		WEAPON_PARSE_FIELD(hasAnyTransientModels);
		WEAPON_PARSE_FIELD(sharedAmmo);
		WEAPON_PARSE_FIELD(lockonSupported);
		WEAPON_PARSE_FIELD(requireLockonToFire);
		WEAPON_PARSE_FIELD(isAirburstWeapon);
		WEAPON_PARSE_FIELD(bigExplosion);
		WEAPON_PARSE_FIELD(noAdsWhenMagEmpty);
		WEAPON_PARSE_FIELD(avoidDropCleanup);
		WEAPON_PARSE_FIELD(allowGrenadeSwitching);
		WEAPON_PARSE_FIELD(inheritsPerks);
		WEAPON_PARSE_FIELD(crosshairColorChange);
		WEAPON_PARSE_FIELD(bRifleBullet);
		WEAPON_PARSE_FIELD(bEnergyBullet);
		WEAPON_PARSE_FIELD(armorPiercing);
		WEAPON_PARSE_FIELD(impaling);
		WEAPON_PARSE_FIELD(bBoltAction);
		WEAPON_PARSE_FIELD(aimDownSight);
		WEAPON_PARSE_FIELD(canHoldBreath);
		WEAPON_PARSE_FIELD(meleeOnly);
		WEAPON_PARSE_FIELD(supportsAlternateMelee);
		WEAPON_PARSE_FIELD(canVariableZoom);
		WEAPON_PARSE_FIELD(bRechamberWhileAds);
		WEAPON_PARSE_FIELD(bBulletExplosiveDamage);
		WEAPON_PARSE_FIELD(bCookOffHold);
		WEAPON_PARSE_FIELD(reticleSpin45);
		WEAPON_PARSE_FIELD(bClipOnly);
		WEAPON_PARSE_FIELD(bDoesNotConsumeAmmo);
		WEAPON_PARSE_FIELD(bRemoveWeaponOnEmpty);
		WEAPON_PARSE_FIELD(noAmmoPickup);
		WEAPON_PARSE_FIELD(adsFireOnly);
		WEAPON_PARSE_FIELD(cancelAutoHolsterWhenEmpty);
		WEAPON_PARSE_FIELD(disableSwitchToWhenEmpty);
		WEAPON_PARSE_FIELD(suppressAmmoPrimaryDisplay);
		WEAPON_PARSE_FIELD(suppressAmmoReserveDisplay);
		WEAPON_PARSE_FIELD(laserSightDuringNightvision);
		WEAPON_PARSE_FIELD(markableViewmodel);
		WEAPON_PARSE_FIELD(flipKillIcon);
		WEAPON_PARSE_FIELD(bNoPartialReload);
		WEAPON_PARSE_FIELD(reloadDisabled);
		WEAPON_PARSE_FIELD(blocksProne);
		WEAPON_PARSE_FIELD(silenced);
		WEAPON_PARSE_FIELD(isRollingGrenade);
		WEAPON_PARSE_FIELD(dropGrenadeHeldOnDeath);
		WEAPON_PARSE_FIELD(projExplosionEffectForceNormalUp);
		WEAPON_PARSE_FIELD(projExplosionEffectInheritParentDirection);
		WEAPON_PARSE_FIELD(bProjImpactExplode);
		WEAPON_PARSE_FIELD(disableProjectileCrumpleCheck);
		WEAPON_PARSE_FIELD(bProjTrajectoryEvents);
		WEAPON_PARSE_FIELD(bProjWhizByEnabled);
		WEAPON_PARSE_FIELD(stickToPlayers);
		WEAPON_PARSE_FIELD(stickToVehicles);
		WEAPON_PARSE_FIELD(stickToTurrets);
		WEAPON_PARSE_FIELD(stickToNonStick);
		WEAPON_PARSE_FIELD(projEnableMissileStickiness);
		WEAPON_PARSE_FIELD(thrownSideways);
		WEAPON_PARSE_FIELD(hasDetonator);
		WEAPON_PARSE_FIELD(disableFiring);
		WEAPON_PARSE_FIELD(firesOnWeaponSwitch);
		WEAPON_PARSE_FIELD(disableHolding);
		WEAPON_PARSE_FIELD(timedDetonation);

		WEAPON_PARSE_FIELD(explosiveDamageDelay);
		WEAPON_PARSE_FIELD(fuseTime);
		WEAPON_PARSE_FIELD(aiFuseTime);
		WEAPON_PARSE_FIELD(maxHoldTime);

		WEAPON_PARSE_FIELD(holdButtonToThrow);
		WEAPON_PARSE_FIELD(infiniteHold);
		WEAPON_PARSE_FIELD(freezeMovementWhenFiring);
		WEAPON_PARSE_FIELD(offhandAllowsSprint);
		WEAPON_PARSE_FIELD(thermalScope);
		WEAPON_PARSE_FIELD(thermalToggle);
		WEAPON_PARSE_FIELD(outlineEnemies);
		WEAPON_PARSE_FIELD(outlineDepthTest);
		WEAPON_PARSE_FIELD(outlineFill);

		WEAPON_PARSE_FIELD(enemyOutlineR);
		WEAPON_PARSE_FIELD(enemyOutlineG);
		WEAPON_PARSE_FIELD(enemyOutlineB);
		WEAPON_PARSE_FIELD(enemyOutlineA);
		WEAPON_PARSE_FIELD(allyOutlineR);
		WEAPON_PARSE_FIELD(allyOutlineG);
		WEAPON_PARSE_FIELD(allyOutlineB);
		WEAPON_PARSE_FIELD(allyOutlineA);
		WEAPON_PARSE_FIELD(depthScan);
		WEAPON_PARSE_FIELD(depthScanThickness);
		WEAPON_PARSE_FIELD(depthScanR);
		WEAPON_PARSE_FIELD(depthScanG);
		WEAPON_PARSE_FIELD(depthScanB);
		WEAPON_PARSE_FIELD(depthScanA);
		WEAPON_PARSE_FIELD(depthScanOutlineThickness);
		WEAPON_PARSE_FIELD(depthScanOutlineR);
		WEAPON_PARSE_FIELD(depthScanOutlineG);
		WEAPON_PARSE_FIELD(depthScanOutlineB);
		WEAPON_PARSE_FIELD(depthScanOutlineA);
		WEAPON_PARSE_FIELD(depthScanOverlay);
		WEAPON_PARSE_FIELD(depthScanOverlayStrength);
		WEAPON_PARSE_FIELD(depthScanOverlayXTiles);
		WEAPON_PARSE_FIELD(depthScanOverlayYTiles);
		WEAPON_PARSE_FIELD(depthScanOverlayXScroll);
		WEAPON_PARSE_FIELD(depthScanOverlayYScroll);

		WEAPON_PARSE_FIELD(blurSceneAdsInFraction);
		WEAPON_PARSE_FIELD(blurSceneAdsOutFraction);

		WEAPON_PARSE_FIELD(altModeSameWeapon);
		WEAPON_PARSE_FIELD(turretBarrelSpinEnabled);
		WEAPON_PARSE_FIELD(missileConeSoundEnabled);
		WEAPON_PARSE_FIELD(missileConeSoundPitchshiftEnabled);
		WEAPON_PARSE_FIELD(missileConeSoundCrossfadeEnabled);
		WEAPON_PARSE_FIELD(offhandHoldIsCancelable);
		WEAPON_PARSE_FIELD(doNotAllowAttachmentsToOverrideSpread);
		WEAPON_PARSE_FIELD(useScopeDrift);
		WEAPON_PARSE_FIELD(alwaysShatterGlassOnImpact);
		WEAPON_PARSE_FIELD(oldWeapon);
		WEAPON_PARSE_FIELD(jumpSpread);
		WEAPON_PARSE_FIELD(noFullViewmodelAnimations);

		WEAPON_PARSE_FIELD(killcamOffset);

		WEAPON_PARSE_FIELD(useDualFOV);
		WEAPON_PARSE_FIELD(disableDrop);
		WEAPON_PARSE_FIELD(preferredDrop);

		WEAPON_PARSE_ASSET(gestureAnimation);
		WEAPON_PARSE_FIELD(gestureFireStateTime);
		WEAPON_PARSE_ASSET(gesturePullback);
		WEAPON_PARSE_FIELD(minPullbackTime);
		WEAPON_PARSE_ASSET(gestureThrow);
		WEAPON_PARSE_FIELD(gestureFireTime);
		WEAPON_PARSE_ASSET(gestureDetonate);
		WEAPON_PARSE_FIELD(gestureDetonationTime);
		WEAPON_PARSE_FIELD(unk_433);
		WEAPON_PARSE_STRING(unknownString1);
		WEAPON_PARSE_FIELD(unk_434);
		WEAPON_PARSE_STRING(unknownString2);
		WEAPON_PARSE_FIELD(gesturesDisablePrimary);
		WEAPON_PARSE_FXCOMBINED(cameraFireEffect);
		WEAPON_PARSE_FIELD(cameraFireEffectDurationSec);
		WEAPON_PARSE_FIELD(changedFireTime);
		WEAPON_PARSE_FIELD(changedFireTimeAkimbo);
		WEAPON_PARSE_FIELD(changedFireTimeNumBullets);

		WEAPON_PARSE_FIELD(chargeType);
		WEAPON_PARSE_FIELD(chargeGain);
		WEAPON_PARSE_FIELD(chargeCostPerShot);
		WEAPON_PARSE_FIELD(chargeLossWhenIdle);
		WEAPON_PARSE_FIELD(chargeEmptyCooldown);
		WEAPON_PARSE_FIELD(chargeFireAtMaxDamageMultiplier);
		WEAPON_PARSE_FIELD(chargeMeterEffectBoneCount);
		WEAPON_PARSE_FXCOMBINED(chargeMeterEffect);
		WEAPON_PARSE_STRING(chargeUpSound);
		WEAPON_PARSE_STRING(chargeDownSound);
		WEAPON_PARSE_STRING(chargeUpSoundPlayer);
		WEAPON_PARSE_STRING(chargeDownSoundPlayer);
		WEAPON_PARSE_STRING(chargeDownToUpSound);
		WEAPON_PARSE_STRING(chargeDownToUpSoundPlayer);
		WEAPON_PARSE_STRING(chargeUpToDownSound);
		WEAPON_PARSE_STRING(chargeUpToDownSoundPlayer);
		WEAPON_PARSE_STRING(chargeMaxSound);
		WEAPON_PARSE_STRING(chargeMaxSoundPlayer);
		WEAPON_PARSE_FIELD(chargeHudReveal);
		WEAPON_PARSE_ASSET(chargeRumble);

		WEAPON_PARSE_SCRIPT_STRING(stowTag);
		WEAPON_PARSE_ASSET(stowOffsetModel);

		WEAPON_PARSE_FIELD(maxTargetAngle);
		WEAPON_PARSE_FIELD(spaceshipSecondaryWeapon);
		WEAPON_PARSE_FIELD(impulseFieldRadius);
		WEAPON_PARSE_FIELD(impulseFieldInitialSpeed);
		WEAPON_PARSE_FIELD(impulseFieldMaxSpeed);
		WEAPON_PARSE_FIELD(impulseFieldAcceleration);
		WEAPON_PARSE_FIELD(impulseFieldInAirImpulseMultiplier);
		WEAPON_PARSE_FIELD(impulseFieldInAirImpulseMultiplierInterpTime);
		WEAPON_PARSE_FIELD(impulseFieldSlideMultiplier);
		WEAPON_PARSE_FIELD(impulseFieldSlideMultiplierInterpTime);
		WEAPON_PARSE_FIELD(impulseFieldIsPush);
		WEAPON_PARSE_FIELD(impulseFieldAffectsFriendlies);
		WEAPON_PARSE_FIELD(impulseFieldAffectsSelf);
		WEAPON_PARSE_FIELD(impulseFieldAffectsProne);

		WEAPON_PARSE_FIELD(regenerationEnabled);
		WEAPON_PARSE_FIELD(regenerationTimeMs);
		WEAPON_PARSE_FIELD(regenerationAddTimeMs);
		WEAPON_PARSE_FIELD(regenerationAmount);
		WEAPON_PARSE_FIELD(regenerationConsumeStock);
		WEAPON_PARSE_FIELD(regenerationDisableWhileFiring);

		WEAPON_PARSE_FIELD(deployRequireOnWalkableSurface);
		WEAPON_PARSE_FIELD(deployRequireOnNavmesh);
		WEAPON_PARSE_FIELD(deployRequireSkyAbove);
		WEAPON_PARSE_FIELD(deployRequireNoOverhang);
		WEAPON_PARSE_FIELD(deployAlwaysUpright);
		WEAPON_PARSE_FIELD(deployEdgeSnap);
		WEAPON_PARSE_FIELD(deployCylinderRadius);
		WEAPON_PARSE_FIELD(deployCylinderHeight);
		WEAPON_PARSE_FIELD(deployMaxDistance);
		WEAPON_PARSE_FXCOMBINED(deployEffect);
		WEAPON_PARSE_FIELD(deployValidGroupId);
		WEAPON_PARSE_FIELD(deployIndoorGroupId);
		WEAPON_PARSE_FIELD(deployInvalidGroupId);
		WEAPON_PARSE_FIELD(deployOutOfRangeGroupId);

		WEAPON_PARSE_FIELD(reactiveEmitterDelay);
		WEAPON_PARSE_FIELD(grenadeDangerIconDistance);
		WEAPON_PARSE_FIELD(unk_466_0);

		// unk stuff
		WEAPON_PARSE_FIELD(unk_0);
		WEAPON_PARSE_FIELD(unk_1);
		WEAPON_PARSE_FIELD(unk_2);
		WEAPON_PARSE_FIELD(unk_3);
		WEAPON_PARSE_FIELD(unk_4_0);
		WEAPON_PARSE_FIELD(unk_4_1);

		parse(&asset->rotationParams, data["rotationParams"], mem);

		for (auto i = 0; i < 21; i++)
		{
			parse(&asset->animOverrides[i], data["animOverrides"][i], mem);
			parse(&asset->sfxOverrides[i], data["sfxOverrides"][i], mem);
			parse(&asset->vfxOverrides[i], data["vfxOverrides"][i], mem);
		}

		{ // unk
			//AttUnknown01; WeaponDef;

#define PARSE_UNK0(__field__) \
			asset->__field__ = data["unk0"][#__field__].get<decltype(asset->__field__)>();

			if (!data["unk0"]["material"].empty())
			{
				asset->unkMaterial = mem->allocate<Material>();
				asset->unkMaterial->name = mem->duplicate_string(data["unk0"]["material"].get<std::string>());
			}
			else
			{
				asset->unkMaterial = nullptr;
			}
			PARSE_UNK0(unk_18);
			PARSE_UNK0(unk_19);
			PARSE_UNK0(unk_20);
			PARSE_UNK0(unk_21);
			PARSE_UNK0(unk_22);
			PARSE_UNK0(unk_23);
			PARSE_UNK0(unk_24);
			PARSE_UNK0(unk_25);
			PARSE_UNK0(unk_26);
			PARSE_UNK0(unk_27);
			PARSE_UNK0(unk_28);
			PARSE_UNK0(unk_29);
			PARSE_UNK0(unk_30);
			PARSE_UNK0(unk_31);
			PARSE_UNK0(unk_32);
			PARSE_UNK0(unk_33);
			PARSE_UNK0(unk_34);
			PARSE_UNK0(unk_35);
			PARSE_UNK0(unk_36);
			PARSE_UNK0(unk_37);
			PARSE_UNK0(unk_38);
			PARSE_UNK0(unk_39);
			PARSE_UNK0(unk_40);
			PARSE_UNK0(unk_41);
			PARSE_UNK0(unk_42);
			PARSE_UNK0(unk_43);
			PARSE_UNK0(unk_44);
			PARSE_UNK0(unk_45);
			PARSE_UNK0(unk_46);
			PARSE_UNK0(unk_47);
			PARSE_UNK0(unk_48);
			PARSE_UNK0(unk_49);
			PARSE_UNK0(unk_50);
			PARSE_UNK0(unk_51);
			PARSE_UNK0(unk_52);
			PARSE_UNK0(unk_53);
			PARSE_UNK0(unk_54);
			PARSE_UNK0(unk_55);
			PARSE_UNK0(unk_56);
			PARSE_UNK0(unk_57);
			PARSE_UNK0(unk_58);
			PARSE_UNK0(unk_59);
			PARSE_UNK0(unk_60);
			PARSE_UNK0(unk_61);
			PARSE_UNK0(unk_62);
			PARSE_UNK0(unk_63);
			PARSE_UNK0(unk_64);
			PARSE_UNK0(unk_65_0);
			PARSE_UNK0(unk_65_1);
			PARSE_UNK0(unk_65_2);
			PARSE_UNK0(unk_65_3);
		}
	}

	void weapon_def::parse_NoteTrackToSoundEntry(NoteTrackToSoundEntry* asset, ordered_json& data, zone_memory* mem)
	{
		WEAPON_PARSE_SCRIPT_STRING(attachment);
		WEAPON_PARSE_SCRIPT_STRING_ALLOC_ARR_MODVAL(notetrackSoundMapKeys, asset->numSoundMappings);
		WEAPON_PARSE_SCRIPT_STRING_ALLOC_ARR_MODVAL(notetrackSoundMapValues, asset->numSoundMappings);
	}

	void weapon_def::parse_NoteTrackToSuitSoundEntry(NoteTrackToSuitSoundEntry* asset, ordered_json& data, zone_memory* mem)
	{
		WEAPON_PARSE_FIELD(suitAnimState);
		WEAPON_PARSE_SCRIPT_STRING_ALLOC_ARR_MODVAL(notetrackSoundMapKeys, asset->numSoundMappings);
		WEAPON_PARSE_SCRIPT_STRING_ALLOC_ARR_MODVAL(notetrackSoundMapValues, asset->numSoundMappings);
	}

	void weapon_def::parse_complete(WeaponCompleteDef* asset, ordered_json& data, zone_memory* mem)
	{
		WEAPON_PARSE_STRING(szInternalName);
		WEAPON_PARSE_STRING(szDisplayName);
		WEAPON_PARSE_STRING(szLootTable);

		WEAPON_PARSE_SCRIPT_STRING_ALLOC_ARR(hideTags, 32);
		WEAPON_PARSE_ASSET_ALLOC_ARR_MODVAL(attachments, asset->numAttachments);
		WEAPON_PARSE_ASSET_ALLOC_ARR_MODVAL(attachments2, asset->numAttachments2);
		WEAPON_PARSE_ASSET_ALLOC_ARR_MODVAL(attachments3, asset->numAttachments3);
		WEAPON_PARSE_ASSET_ALLOC_ARR_MODVAL(attachments4, asset->numAttachments4);
		WEAPON_PARSE_ASSET_ALLOC_ARR_MODVAL(attachments5, asset->numAttachments5);
		WEAPON_PARSE_ASSET_ALLOC_ARR_MODVAL(attachments6, asset->numAttachments6);
		WEAPON_PARSE_SCRIPT_STRING_ALLOC_ARR_MODVAL(notetrackSoundMapKeys, asset->numNotetrackSoundMappings);
		WEAPON_PARSE_SCRIPT_STRING_ALLOC_ARR_MODVAL(notetrackSoundMapValues, asset->numNotetrackSoundMappings);
		if (!data["notetrackOverrides"].is_null())
		{
			asset->numNotetrackOverrides = static_cast<unsigned short>(data["notetrackOverrides"].size());
			asset->notetrackOverrides = mem->allocate<NoteTrackToSoundEntry>(asset->numNotetrackOverrides);
			for (auto i = 0u; i < asset->numNotetrackOverrides; i++)
			{
				parse_NoteTrackToSoundEntry(&asset->notetrackOverrides[i], data["notetrackOverrides"][i], mem);
			}
		}
		else
		{
			asset->notetrackOverrides = nullptr;
		}
		if (!data["notetrackSuitEntries"].is_null())
		{
			asset->numNotetrackSuitEntries = static_cast<unsigned short>(data["notetrackSuitEntries"].size());
			asset->notetrackSuitEntries = mem->allocate<NoteTrackToSuitSoundEntry>(asset->numNotetrackSuitEntries);
			for (auto i = 0u; i < asset->numNotetrackSuitEntries; i++)
			{
				parse_NoteTrackToSuitSoundEntry(&asset->notetrackSuitEntries[i], data["notetrackSuitEntries"][i], mem);
			}
		}
		else
		{
			asset->notetrackSuitEntries = nullptr;
		}
		WEAPON_PARSE_FIELD(fAdsZoomFov);
		WEAPON_PARSE_FIELD(iAdsTransInTime);
		WEAPON_PARSE_FIELD(iAdsTransOutTime);
		WEAPON_PARSE_FIELD(iClipSize);
		WEAPON_PARSE_FIELD(vfxImpactType);
		WEAPON_PARSE_FIELD(sfxImpactType);
		WEAPON_PARSE_FIELD(meleeImpactType);
		WEAPON_PARSE_FIELD(penetrateMultiplier);
		WEAPON_PARSE_FIELD(fAdsViewKickCenterSpeed);
		WEAPON_PARSE_FIELD(fHipViewKickCenterSpeed);
		WEAPON_PARSE_STRING(szAltWeaponName);
		WEAPON_PARSE_ASSET(killIcon);
		WEAPON_PARSE_ASSET(dpadIcon);
		WEAPON_PARSE_FIELD(ammoDropStockMax);
		WEAPON_PARSE_FIELD(adsDofStart);
		WEAPON_PARSE_FIELD(adsDofEnd);
		for (auto i = 0; i < 2; i++)
		{
			if (!data["accuracyGraphKnots"][i].is_null())
			{
				asset->accuracyGraphKnotCount[i] = static_cast<unsigned short>(data["accuracyGraphKnots"][i].size());
				asset->accuracyGraphKnots[i] = mem->allocate<vec2_t>(asset->accuracyGraphKnotCount[i]);
				for (auto ii = 0; ii < asset->accuracyGraphKnotCount[i]; ii++)
				{
					asset->accuracyGraphKnots[i][ii][0] = data["accuracyGraphKnots"][i][ii][0].get<float>();
					asset->accuracyGraphKnots[i][ii][1] = data["accuracyGraphKnots"][i][ii][1].get<float>();
				}
			}
			else
			{
				asset->accuracyGraphKnots[i] = nullptr;
			}
		}
		WEAPON_PARSE_FIELD(motionTracker);
		WEAPON_PARSE_FIELD(enhanced);
		WEAPON_PARSE_FIELD(dpadIconShowsAmmo);
		WEAPON_PARSE_FIELD(luiWeaponInfoWidgetUsesScopeStencil);
		WEAPON_PARSE_STRING(szAdsrBaseSetting);
		WEAPON_PARSE_STRING(szLUIWeaponInfoWidgetName);
		WEAPON_PARSE_STRING(szLUIWeaponInfoWidgetTag);
		WEAPON_PARSE_STRING(szLUICrosshairWidget);

		// parse this last
		asset->weapDef = mem->allocate<WeaponDef>();
		parse_weapondef(asset->weapDef, asset, data["weapDef"], mem);
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

		// parse json file
		file.open("rb");
		ordered_json data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto asset = mem->allocate<WeaponCompleteDef>();

		parse_complete(asset, data, mem);

		return asset;
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

#define WEAPON_PREPARE_SCRIPT_STRING(__field__) \
	asset->__field__ = static_cast<scr_string_t>(buf->write_scriptstring(this->get_script_string(&asset->__field__)));

#define WEAPON_PREPARE_SCRIPT_STRING_ARR(__field__, __size__) \
	for (auto idx = 0u; idx < (unsigned int)__size__; idx++) \
	{ \
		asset->__field__[idx] = static_cast<scr_string_t>(buf->write_scriptstring(this->get_script_string(&asset->__field__[idx]))); \
	} \

	void weapon_def::prepare(AnimOverride* asset, zone_buffer* buf)
	{
		WEAPON_PREPARE_SCRIPT_STRING(attachment1);
		WEAPON_PREPARE_SCRIPT_STRING(attachment2);
	}

	void weapon_def::prepare(SFXOverride* asset, zone_buffer* buf)
	{
		WEAPON_PREPARE_SCRIPT_STRING(attachment1);
		WEAPON_PREPARE_SCRIPT_STRING(attachment2);
	}

	void weapon_def::prepare(VFXOverride* asset, zone_buffer* buf)
	{
		WEAPON_PREPARE_SCRIPT_STRING(attachment1);
		WEAPON_PREPARE_SCRIPT_STRING(attachment2);
	}

	void weapon_def::prepare_weapondef(WeaponDef* asset, zone_buffer* buf)
	{
		WEAPON_PREPARE_SCRIPT_STRING_ARR(notetrackRumbleMapKeys, 16);
		WEAPON_PREPARE_SCRIPT_STRING_ARR(notetrackRumbleMapValues, 16);
		WEAPON_PREPARE_SCRIPT_STRING_ARR(notetrackFXMapKeys, 16);

		WEAPON_PREPARE_SCRIPT_STRING_ARR(notetrackFXMapTagValues, 16);

		WEAPON_PREPARE_SCRIPT_STRING(knifeAttachTagOverride);
		WEAPON_PREPARE_SCRIPT_STRING(knifeAttachTagOverride2);

		WEAPON_PREPARE_SCRIPT_STRING(stowTag);
	}

	void weapon_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* asset = this->asset_;
		WEAPON_PREPARE_SCRIPT_STRING_ARR(hideTags, 32);
		WEAPON_PREPARE_SCRIPT_STRING_ARR(notetrackSoundMapKeys, asset->numNotetrackSoundMappings);
		WEAPON_PREPARE_SCRIPT_STRING_ARR(notetrackSoundMapValues, asset->numNotetrackSoundMappings);
		if (asset->notetrackOverrides)
		{
			for (auto i = 0u; i < asset->numNotetrackOverrides; i++)
			{
				WEAPON_PREPARE_SCRIPT_STRING_ARR(notetrackOverrides[i].notetrackSoundMapKeys, asset->notetrackOverrides[i].numSoundMappings);
				WEAPON_PREPARE_SCRIPT_STRING_ARR(notetrackOverrides[i].notetrackSoundMapValues, asset->notetrackOverrides[i].numSoundMappings);
			}
		}
		if (asset->notetrackSuitEntries)
		{
			for (auto i = 0u; i < asset->numNotetrackSuitEntries; i++)
			{
				WEAPON_PREPARE_SCRIPT_STRING_ARR(notetrackSuitEntries[i].notetrackSoundMapKeys, asset->notetrackSuitEntries[i].numSoundMappings);
				WEAPON_PREPARE_SCRIPT_STRING_ARR(notetrackSuitEntries[i].notetrackSoundMapValues, asset->notetrackSuitEntries[i].numSoundMappings);
			}
		}

		// weapondef
		prepare_weapondef(asset->weapDef, buf);
	}

#define WEAPON_SUBASSET_DEPENDING(__field__,__type__) \
	if (asset->__field__) \
	{ \
		zone->add_asset_of_type(__type__, asset->__field__->name); \
	}

#define WEAPON_SUBASSET_DEPENDING_ARR(__field__,__type__,__size__) \
	if (asset->__field__) \
	{ \
		for (auto idx = 0u; idx < (unsigned int)__size__; idx++) \
		{ \
			if (asset->__field__[idx]) \
			{ \
				zone->add_asset_of_type(__type__, asset->__field__[idx]->name); \
			} \
		} \
	}

#define WEAPON_SUBASSET_DEPENDING_FIELD_ARR(__field__,__type__,__size__) \
	for (auto idx = 0u; idx < (unsigned int)__size__; idx++) \
	{ \
		if(asset->__field__[idx]) \
		{ \
			zone->add_asset_of_type(__type__, asset->__field__[idx]->name); \
		} \
	} \

#define WEAPON_SUBASSET_DEPENDING_FXCOMBINED(__field__) \
	if (asset->__field__.u.data) \
	{ \
		if(asset->__field__.type == FX_COMBINED_PARTICLE_SYSTEM) zone->add_asset_of_type(ASSET_TYPE_VFX, asset->__field__.u.particleSystemDef->name); \
		else zone->add_asset_of_type(ASSET_TYPE_FX, asset->__field__.u.fx->name); \
	}

#define WEAPON_SUBASSET_DEPENDING_FXCOMBINED_ARR(__field__,__size__) \
	if (asset->__field__) \
	{ \
		for (auto idx = 0u; idx < (unsigned int)__size__; idx++) \
		{ \
			if (asset->__field__[idx].u.data) \
			{ \
				if(asset->__field__[idx].type == FX_COMBINED_PARTICLE_SYSTEM) zone->add_asset_of_type(ASSET_TYPE_VFX, asset->__field__[idx].u.particleSystemDef->name); \
				else zone->add_asset_of_type(ASSET_TYPE_FX, asset->__field__[idx].u.fx->name); \
			} \
		} \
	}

	void weapon_def::load_depending_weapondef(WeaponDef* asset, zone_base* zone)
	{
		WEAPON_SUBASSET_DEPENDING(gunXModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(gunXModelLeftHand, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(gunXModelRightHand, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(defaultViewModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(defaultWorldModelLeftHand, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(defaultWorldModelRightHand, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(worldModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(worldXModelLeftHand, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(worldXModelRightHand, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(defaultWorldModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(playerShadowModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(playerShadowModelLeftHand, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(playerShadowModelRightHand, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(handXModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING_ARR(complexWorldModel, ASSET_TYPE_XMODEL, asset->numComplexModels);
		WEAPON_SUBASSET_DEPENDING_ARR(complexWorldModelLeftHand, ASSET_TYPE_XMODEL, asset->numComplexModels);
		WEAPON_SUBASSET_DEPENDING_ARR(complexWorldModelRightHand, ASSET_TYPE_XMODEL, asset->numComplexModels);
		WEAPON_SUBASSET_DEPENDING_ARR(complexGunXModel, ASSET_TYPE_XMODEL, asset->numComplexModels);
		WEAPON_SUBASSET_DEPENDING_ARR(complexGunXModelLeftHand, ASSET_TYPE_XMODEL, asset->numComplexModels);
		WEAPON_SUBASSET_DEPENDING_ARR(complexGunXModelRightHand, ASSET_TYPE_XMODEL, asset->numComplexModels);
		WEAPON_SUBASSET_DEPENDING_FIELD_ARR(camoWorldModelMaterialOverride, ASSET_TYPE_MATERIAL, 16);
		WEAPON_SUBASSET_DEPENDING_FIELD_ARR(camoViewModelMaterialOverride, ASSET_TYPE_MATERIAL, 16);
		WEAPON_SUBASSET_DEPENDING(szXAnims, ASSET_TYPE_ANIM_PACKAGE);
		WEAPON_SUBASSET_DEPENDING(szXAnimsRightHanded, ASSET_TYPE_ANIM_PACKAGE);
		WEAPON_SUBASSET_DEPENDING(szXAnimsLeftHanded, ASSET_TYPE_ANIM_PACKAGE);
		WEAPON_SUBASSET_DEPENDING_FXCOMBINED_ARR(notetrackFXMapValues, 16);

		WEAPON_SUBASSET_DEPENDING(unkMaterial, ASSET_TYPE_MATERIAL);

		WEAPON_SUBASSET_DEPENDING(vfxPackage, ASSET_TYPE_VFX_PACKAGE);
		WEAPON_SUBASSET_DEPENDING(sfxPackage, ASSET_TYPE_SFX_PACKAGE);
		WEAPON_SUBASSET_DEPENDING(reticleCenter, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(reticleSide, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(reticleOnePiece, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(worldClipModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(rocketModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(knifeModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(worldKnifeModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(hudIcon, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(pickupIcon, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(dangerIcon, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(throwBackIcon, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(ammoCounterIcon, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(projectileModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING_FXCOMBINED(projExplosionEffect);
		WEAPON_SUBASSET_DEPENDING_FXCOMBINED(projDudEffect);
		WEAPON_SUBASSET_DEPENDING_FXCOMBINED(projBodyEffect);
		WEAPON_SUBASSET_DEPENDING_FXCOMBINED(projTrailEffect);
		WEAPON_SUBASSET_DEPENDING_FXCOMBINED(projBeaconEffect);
		WEAPON_SUBASSET_DEPENDING_FXCOMBINED(projIgnitionEffect);
		WEAPON_SUBASSET_DEPENDING(fireRumble, ASSET_TYPE_RUMBLE);
		WEAPON_SUBASSET_DEPENDING(meleeImpactRumble, ASSET_TYPE_RUMBLE);
		WEAPON_SUBASSET_DEPENDING(tracerType, ASSET_TYPE_TRACER);
		WEAPON_SUBASSET_DEPENDING(overchargeTracerType, ASSET_TYPE_TRACER);
		WEAPON_SUBASSET_DEPENDING(laserTypeFriendly, ASSET_TYPE_LASER);
		WEAPON_SUBASSET_DEPENDING(laserTypeEnemy, ASSET_TYPE_LASER);
		// turretOverheatSound
		WEAPON_SUBASSET_DEPENDING_FXCOMBINED(turretOverheatEffect);
		WEAPON_SUBASSET_DEPENDING(turretBarrelSpinRumble, ASSET_TYPE_RUMBLE);
		// turretBarrelSpinMaxSnd
		// turretBarrelSpinUpSnd
		// turretBarrelSpinDownSnd
		// missileConeSoundAlias
		// missileConeSoundAliasAtBase
		WEAPON_SUBASSET_DEPENDING(shieldDeployGesture, ASSET_TYPE_GESTURE);
		WEAPON_SUBASSET_DEPENDING(shieldFireWeapGesture, ASSET_TYPE_GESTURE);
		WEAPON_SUBASSET_DEPENDING(shieldDeployWhileFiring, ASSET_TYPE_GESTURE);
		WEAPON_SUBASSET_DEPENDING(shieldRetractWhileFiring, ASSET_TYPE_GESTURE);
		WEAPON_SUBASSET_DEPENDING(shieldBashGesture, ASSET_TYPE_GESTURE);
		WEAPON_SUBASSET_DEPENDING_FXCOMBINED(shieldMeleeFx);
		WEAPON_SUBASSET_DEPENDING(shieldHitRumble, ASSET_TYPE_RUMBLE);
		WEAPON_SUBASSET_DEPENDING(shieldMissRumble, ASSET_TYPE_RUMBLE);
		WEAPON_SUBASSET_DEPENDING(gestureAnimation, ASSET_TYPE_GESTURE);
		WEAPON_SUBASSET_DEPENDING(gesturePullback, ASSET_TYPE_GESTURE);
		WEAPON_SUBASSET_DEPENDING(gestureThrow, ASSET_TYPE_GESTURE);
		WEAPON_SUBASSET_DEPENDING(gestureDetonate, ASSET_TYPE_GESTURE);
		WEAPON_SUBASSET_DEPENDING_FXCOMBINED(cameraFireEffect);
		WEAPON_SUBASSET_DEPENDING_FXCOMBINED(chargeMeterEffect);
		// chargeUpSound
		// chargeDownSound
		// chargeUpSoundPlayer
		// chargeDownSoundPlayer
		// chargeDownToUpSound
		// chargeDownToUpSoundPlayer
		// chargeUpToDownSound
		// chargeUpToDownSoundPlayer
		// chargeMaxSound
		// chargeMaxSoundPlayer
		WEAPON_SUBASSET_DEPENDING(chargeRumble, ASSET_TYPE_RUMBLE);
		WEAPON_SUBASSET_DEPENDING(stowOffsetModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING_FXCOMBINED(deployEffect);
		for (auto i = 0; i < 21; i++)
		{
			WEAPON_SUBASSET_DEPENDING(animOverrides[i].overrides, ASSET_TYPE_ANIM_PACKAGE);
			WEAPON_SUBASSET_DEPENDING(animOverrides[i].overridesAlt, ASSET_TYPE_ANIM_PACKAGE);

			WEAPON_SUBASSET_DEPENDING(sfxOverrides[i].overrides, ASSET_TYPE_SFX_PACKAGE);
			WEAPON_SUBASSET_DEPENDING(sfxOverrides[i].overridesAlt, ASSET_TYPE_SFX_PACKAGE);

			WEAPON_SUBASSET_DEPENDING(vfxOverrides[i].overrides, ASSET_TYPE_VFX_PACKAGE);
			WEAPON_SUBASSET_DEPENDING(vfxOverrides[i].overridesAlt, ASSET_TYPE_VFX_PACKAGE);
		}
	}

	void weapon_def::load_depending(zone_base* zone)
	{
		auto* asset = this->asset_;

		WEAPON_SUBASSET_DEPENDING_ARR(attachments, ASSET_TYPE_ATTACHMENT, asset->numAttachments);
		WEAPON_SUBASSET_DEPENDING_ARR(attachments2, ASSET_TYPE_ATTACHMENT, asset->numAttachments2);
		WEAPON_SUBASSET_DEPENDING_ARR(attachments3, ASSET_TYPE_ATTACHMENT, asset->numAttachments3);
		WEAPON_SUBASSET_DEPENDING_ARR(attachments4, ASSET_TYPE_ATTACHMENT, asset->numAttachments4);
		WEAPON_SUBASSET_DEPENDING_ARR(attachments5, ASSET_TYPE_ATTACHMENT, asset->numAttachments5);
		WEAPON_SUBASSET_DEPENDING_ARR(attachments6, ASSET_TYPE_ATTACHMENT, asset->numAttachments6);

		WEAPON_SUBASSET_DEPENDING(killIcon, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(dpadIcon, ASSET_TYPE_MATERIAL);

		load_depending_weapondef(asset->weapDef, zone);
	}

	std::string weapon_def::name()
	{
		return this->name_;
	}

	std::int32_t weapon_def::type()
	{
		return ASSET_TYPE_WEAPON;
	}

#define WEAPON_WRITE_ARR(__field__, __size__, __align__) \
	if (data->__field__) \
	{ \
		buf->align(__align__); \
		buf->write(data->__field__, __size__); \
		buf->clear_pointer(&dest->__field__); \
	}

#define WEAPON_WRITE_STRING(__field__) \
	if (data->__field__) \
	{ \
		dest->__field__ = buf->write_str(data->__field__); \
	}

#define WEAPON_WRITE_STRING_ARR(__field__, __size__) \
	for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
	{ \
		if(data->__field__[idx##__field__]) \
		{ \
			dest->__field__[idx##__field__] = buf->write_str(data->__field__[idx##__field__]); \
		} \
	} \

#define WEAPON_WRITE_ASSET(__field__, __type__) \
	if (data->__field__) \
	{ \
		dest->__field__ = reinterpret_cast<typename std::remove_reference<decltype(data->__field__)>::type>(zone->get_asset_pointer(__type__, data->__field__->name)); \
	}

#define WEAPON_WRITE_ASSET_ARR(__field__, __type__, __size__, __align__) \
	if (data->__field__) \
	{ \
		buf->align(__align__); \
		dest->__field__ = buf->write(data->__field__, __size__); \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			if (data->__field__[idx##__field__]) \
			{ \
				dest->__field__[idx##__field__] = reinterpret_cast<typename std::remove_reference<decltype(data->__field__[idx##__field__])>::type>( \
				zone->get_asset_pointer(__type__, data->__field__[idx##__field__]->name)); \
			} \
			else \
			{ \
				dest->__field__[idx##__field__] = nullptr; \
			} \
		} \
		buf->clear_pointer(&dest->__field__); \
	} \

#define WEAPON_WRITE_ASSET_FIELD_ARR(__field__, __type__, __size__) \
	for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
	{ \
		if (data->__field__[idx##__field__]) \
		{ \
			dest->__field__[idx##__field__] = reinterpret_cast<typename std::remove_reference<decltype(data->__field__[idx##__field__])>::type>( \
			zone->get_asset_pointer(__type__, data->__field__[idx##__field__]->name)); \
		} \
		else \
		{ \
			dest->__field__[idx##__field__] = nullptr; \
		} \
	} \

#define WEAPON_WRITE_FXCOMBINED(__field__) \
	if (data->__field__.u.data) \
	{ \
		if(data->__field__.type == FX_COMBINED_PARTICLE_SYSTEM) \
			dest->__field__.u.particleSystemDef = reinterpret_cast<ParticleSystemDef*>(zone->get_asset_pointer(ASSET_TYPE_VFX, data->__field__.u.particleSystemDef->name)); \
		else \
			dest->__field__.u.fx = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(ASSET_TYPE_FX, data->__field__.u.fx->name)); \
	}

#define WEAPON_WRITE_FXCOMBINED_ARR(__field__, __size__, __align__) \
	if (data->__field__) \
	{ \
		buf->align(__align__); \
		dest->__field__ = buf->write(data->__field__, __size__); \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			if(data->__field__[idx##__field__].u.data) \
			{ \
				if(data->__field__[idx##__field__].type == FX_COMBINED_PARTICLE_SYSTEM) \
					dest->__field__[idx##__field__].u.particleSystemDef = reinterpret_cast<ParticleSystemDef*>(zone->get_asset_pointer(ASSET_TYPE_VFX, data->__field__[idx##__field__].u.particleSystemDef->name)); \
				else \
					dest->__field__[idx##__field__].u.fx = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(ASSET_TYPE_FX, data->__field__[idx##__field__].u.fx->name)); \
			} \
		} \
		buf->clear_pointer(&dest->__field__); \
	}

	void weapon_def::write_overlay(ADSOverlay* data, ADSOverlay* dest, zone_base* zone, zone_buffer* buf)
	{
		WEAPON_WRITE_ASSET(shader, ASSET_TYPE_MATERIAL);
		WEAPON_WRITE_ASSET(shaderLowRes, ASSET_TYPE_MATERIAL);
		WEAPON_WRITE_ASSET(shaderEMP, ASSET_TYPE_MATERIAL);
		WEAPON_WRITE_ASSET(shaderEMPLowRes, ASSET_TYPE_MATERIAL);
	}

	void weapon_def::write_weapon_def(WeaponCompleteDef* parent_data, zone_base* zone, zone_buffer* buf)
	{
		auto data = parent_data->weapDef;
		auto dest = buf->write(data);

		WEAPON_WRITE_STRING(szOverlayName);
		WEAPON_WRITE_ASSET(gunXModel, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET(gunXModelLeftHand, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET(gunXModelRightHand, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET(defaultViewModel, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET(defaultWorldModelLeftHand, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET(defaultWorldModelRightHand, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET(worldModel, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET(worldXModelLeftHand, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET(worldXModelRightHand, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET(defaultWorldModel, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET(playerShadowModel, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET(playerShadowModelLeftHand, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET(playerShadowModelRightHand, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET(handXModel, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET_ARR(complexWorldModel, ASSET_TYPE_XMODEL, data->numComplexModels, 7);
		WEAPON_WRITE_ASSET_ARR(complexWorldModelLeftHand, ASSET_TYPE_XMODEL, data->numComplexModels, 7);
		WEAPON_WRITE_ASSET_ARR(complexWorldModelRightHand, ASSET_TYPE_XMODEL, data->numComplexModels, 7);
		WEAPON_WRITE_ASSET_ARR(complexGunXModel, ASSET_TYPE_XMODEL, data->numComplexModels, 7);
		WEAPON_WRITE_ASSET_ARR(complexGunXModelLeftHand, ASSET_TYPE_XMODEL, data->numComplexModels, 7);
		WEAPON_WRITE_ASSET_ARR(complexGunXModelRightHand, ASSET_TYPE_XMODEL, data->numComplexModels, 7);
		WEAPON_WRITE_ASSET_FIELD_ARR(camoWorldModelMaterialOverride, ASSET_TYPE_MATERIAL, 16);
		WEAPON_WRITE_ASSET_FIELD_ARR(camoViewModelMaterialOverride, ASSET_TYPE_MATERIAL, 16);
		WEAPON_WRITE_ASSET(szXAnims, ASSET_TYPE_ANIM_PACKAGE);
		WEAPON_WRITE_ASSET(szXAnimsRightHanded, ASSET_TYPE_ANIM_PACKAGE);
		WEAPON_WRITE_ASSET(szXAnimsLeftHanded, ASSET_TYPE_ANIM_PACKAGE);
		WEAPON_WRITE_STRING(szModeName);

		WEAPON_WRITE_ARR(notetrackRumbleMapKeys, 16, 3);
		WEAPON_WRITE_ARR(notetrackRumbleMapValues, 16, 3);
		WEAPON_WRITE_ARR(notetrackFXMapKeys, 16, 3);
		WEAPON_WRITE_FXCOMBINED_ARR(notetrackFXMapValues, 16, 7);
		WEAPON_WRITE_ARR(notetrackFXMapTagValues, 16, 3);

		WEAPON_WRITE_ASSET(unkMaterial, ASSET_TYPE_MATERIAL);

		WEAPON_WRITE_ASSET(vfxPackage, ASSET_TYPE_VFX_PACKAGE);
		WEAPON_WRITE_ASSET(sfxPackage, ASSET_TYPE_SFX_PACKAGE);
		WEAPON_WRITE_ASSET(reticleCenter, ASSET_TYPE_MATERIAL);
		WEAPON_WRITE_ASSET(reticleSide, ASSET_TYPE_MATERIAL);
		WEAPON_WRITE_ASSET(reticleOnePiece, ASSET_TYPE_MATERIAL);
		WEAPON_WRITE_ASSET(worldClipModel, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET(rocketModel, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET(knifeModel, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET(worldKnifeModel, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_ASSET(hudIcon, ASSET_TYPE_MATERIAL);
		WEAPON_WRITE_ASSET(pickupIcon, ASSET_TYPE_MATERIAL);
		WEAPON_WRITE_ASSET(dangerIcon, ASSET_TYPE_MATERIAL);
		WEAPON_WRITE_ASSET(throwBackIcon, ASSET_TYPE_MATERIAL);
		WEAPON_WRITE_ASSET(ammoCounterIcon, ASSET_TYPE_MATERIAL);
		WEAPON_WRITE_STRING(szAmmoName);
		WEAPON_WRITE_STRING(szSharedAmmoCapName);
		write_overlay(&data->overlay, &dest->overlay, zone, buf);
		WEAPON_WRITE_ASSET(projectileModel, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_FXCOMBINED(projExplosionEffect);
		WEAPON_WRITE_FXCOMBINED(projDudEffect);
		WEAPON_WRITE_ARR(parallelBounce, 64, 3);
		WEAPON_WRITE_ARR(perpendicularBounce, 64, 3);
		WEAPON_WRITE_FXCOMBINED(projBodyEffect);
		WEAPON_WRITE_FXCOMBINED(projTrailEffect);
		WEAPON_WRITE_FXCOMBINED(projBeaconEffect);
		WEAPON_WRITE_FXCOMBINED(projIgnitionEffect);
		WEAPON_WRITE_STRING(accuracyGraphName[0]);
		WEAPON_WRITE_ARR(originalAccuracyGraphKnots[0], parent_data->accuracyGraphKnotCount[0], 3);
		WEAPON_WRITE_STRING(accuracyGraphName[1]);
		WEAPON_WRITE_ARR(originalAccuracyGraphKnots[1], parent_data->accuracyGraphKnotCount[1], 3);
		WEAPON_WRITE_STRING(szUseHintString);
		WEAPON_WRITE_STRING(dropHintString);
		WEAPON_WRITE_STRING(szScript);
		WEAPON_WRITE_ARR(locationDamageMultipliers, 22, 3);
		WEAPON_WRITE_ARR(hitLocPriorityMap, 22, 0);
		WEAPON_WRITE_ASSET(fireRumble, ASSET_TYPE_RUMBLE);
		WEAPON_WRITE_ASSET(meleeImpactRumble, ASSET_TYPE_RUMBLE);
		WEAPON_WRITE_ASSET(tracerType, ASSET_TYPE_TRACER);
		WEAPON_WRITE_ASSET(overchargeTracerType, ASSET_TYPE_TRACER);
		WEAPON_WRITE_ASSET(laserTypeFriendly, ASSET_TYPE_LASER);
		WEAPON_WRITE_ASSET(laserTypeEnemy, ASSET_TYPE_LASER);
		WEAPON_WRITE_STRING(turretOverheatSound);
		WEAPON_WRITE_FXCOMBINED(turretOverheatEffect);
		WEAPON_WRITE_ASSET(turretBarrelSpinRumble, ASSET_TYPE_RUMBLE);
		WEAPON_WRITE_STRING(turretBarrelSpinMaxSnd);
		WEAPON_WRITE_STRING_ARR(turretBarrelSpinUpSnd, 4);
		WEAPON_WRITE_STRING_ARR(turretBarrelSpinDownSnd, 4);
		WEAPON_WRITE_STRING(missileConeSoundAlias);
		WEAPON_WRITE_STRING(missileConeSoundAliasAtBase);
		WEAPON_WRITE_ASSET(shieldDeployGesture, ASSET_TYPE_GESTURE);
		WEAPON_WRITE_ASSET(shieldFireWeapGesture, ASSET_TYPE_GESTURE);
		WEAPON_WRITE_ASSET(shieldDeployWhileFiring, ASSET_TYPE_GESTURE);
		WEAPON_WRITE_ASSET(shieldRetractWhileFiring, ASSET_TYPE_GESTURE);
		WEAPON_WRITE_ASSET(shieldBashGesture, ASSET_TYPE_GESTURE);
		WEAPON_WRITE_FXCOMBINED(shieldMeleeFx);
		WEAPON_WRITE_ASSET(shieldHitRumble, ASSET_TYPE_RUMBLE);
		WEAPON_WRITE_ASSET(shieldMissRumble, ASSET_TYPE_RUMBLE);
		WEAPON_WRITE_ASSET(gestureAnimation, ASSET_TYPE_GESTURE);
		WEAPON_WRITE_ASSET(gesturePullback, ASSET_TYPE_GESTURE);
		WEAPON_WRITE_ASSET(gestureThrow, ASSET_TYPE_GESTURE);
		WEAPON_WRITE_ASSET(gestureDetonate, ASSET_TYPE_GESTURE);
		WEAPON_WRITE_STRING(unknownString1);
		WEAPON_WRITE_STRING(unknownString2);
		WEAPON_WRITE_FXCOMBINED(cameraFireEffect);
		WEAPON_WRITE_FXCOMBINED(chargeMeterEffect);
		WEAPON_WRITE_STRING(chargeUpSound);
		WEAPON_WRITE_STRING(chargeDownSound);
		WEAPON_WRITE_STRING(chargeUpSoundPlayer);
		WEAPON_WRITE_STRING(chargeDownSoundPlayer);
		WEAPON_WRITE_STRING(chargeDownToUpSound);
		WEAPON_WRITE_STRING(chargeDownToUpSoundPlayer);
		WEAPON_WRITE_STRING(chargeUpToDownSound);
		WEAPON_WRITE_STRING(chargeUpToDownSoundPlayer);
		WEAPON_WRITE_STRING(chargeMaxSound);
		WEAPON_WRITE_STRING(chargeMaxSoundPlayer);
		WEAPON_WRITE_ASSET(chargeRumble, ASSET_TYPE_RUMBLE);
		WEAPON_WRITE_ASSET(stowOffsetModel, ASSET_TYPE_XMODEL);
		WEAPON_WRITE_FXCOMBINED(deployEffect);
		for (auto i = 0; i < 21; i++)
		{
			WEAPON_WRITE_ASSET(animOverrides[i].overrides, ASSET_TYPE_ANIM_PACKAGE);
			WEAPON_WRITE_ASSET(animOverrides[i].overridesAlt, ASSET_TYPE_ANIM_PACKAGE);
		}
		for (auto i = 0; i < 21; i++)
		{
			WEAPON_WRITE_ASSET(sfxOverrides[i].overrides, ASSET_TYPE_SFX_PACKAGE);
			WEAPON_WRITE_ASSET(sfxOverrides[i].overridesAlt, ASSET_TYPE_SFX_PACKAGE);
		}
		for (auto i = 0; i < 21; i++)
		{
			WEAPON_WRITE_ASSET(vfxOverrides[i].overrides, ASSET_TYPE_VFX_PACKAGE);
			WEAPON_WRITE_ASSET(vfxOverrides[i].overridesAlt, ASSET_TYPE_VFX_PACKAGE);
		}
	}

	void weapon_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->weapDef)
		{
			buf->align(7);
			write_weapon_def(data, zone, buf);
			buf->clear_pointer(&dest->weapDef);
		}

		WEAPON_WRITE_STRING(szDisplayName);
		WEAPON_WRITE_STRING(szLootTable);
		WEAPON_WRITE_ARR(hideTags, 32, 3);
		WEAPON_WRITE_ASSET_ARR(attachments, ASSET_TYPE_ATTACHMENT, data->numAttachments, 7);
		WEAPON_WRITE_ASSET_ARR(attachments2, ASSET_TYPE_ATTACHMENT, data->numAttachments2, 7);
		WEAPON_WRITE_ASSET_ARR(attachments3, ASSET_TYPE_ATTACHMENT, data->numAttachments3, 7);
		WEAPON_WRITE_ASSET_ARR(attachments4, ASSET_TYPE_ATTACHMENT, data->numAttachments4, 7);
		WEAPON_WRITE_ASSET_ARR(attachments5, ASSET_TYPE_ATTACHMENT, data->numAttachments5, 7);
		WEAPON_WRITE_ASSET_ARR(attachments6, ASSET_TYPE_ATTACHMENT, data->numAttachments6, 7);
		WEAPON_WRITE_ARR(notetrackSoundMapKeys, data->numNotetrackSoundMappings, 3);
		WEAPON_WRITE_ARR(notetrackSoundMapValues, data->numNotetrackSoundMappings, 3);

		if (data->notetrackOverrides)
		{
			buf->align(7);
			dest->notetrackOverrides = buf->write(data->notetrackOverrides);
			for (auto i = 0u; i < data->numNotetrackOverrides; i++)
			{
				if (data->notetrackOverrides[i].notetrackSoundMapKeys)
				{
					buf->align(3);
					buf->write(data->notetrackOverrides[i].notetrackSoundMapKeys, data->notetrackOverrides[i].numSoundMappings);
					buf->clear_pointer(&dest->notetrackOverrides[i].notetrackSoundMapKeys);
				}
				if (data->notetrackOverrides[i].notetrackSoundMapValues)
				{
					buf->align(3);
					buf->write(data->notetrackOverrides[i].notetrackSoundMapValues, data->notetrackOverrides[i].numSoundMappings);
					buf->clear_pointer(&dest->notetrackOverrides[i].notetrackSoundMapValues);
				}
			}
			buf->clear_pointer(&dest->notetrackOverrides);
		}
		if (data->notetrackSuitEntries)
		{
			buf->align(7);
			dest->notetrackSuitEntries = buf->write(data->notetrackSuitEntries);
			for (auto i = 0u; i < data->numNotetrackSuitEntries; i++)
			{
				if (data->notetrackSuitEntries[i].notetrackSoundMapKeys)
				{
					buf->align(3);
					buf->write(data->notetrackSuitEntries[i].notetrackSoundMapKeys, data->notetrackSuitEntries[i].numSoundMappings);
					buf->clear_pointer(&dest->notetrackSuitEntries[i].notetrackSoundMapKeys);
				}
				if (data->notetrackSuitEntries[i].notetrackSoundMapValues)
				{
					buf->align(3);
					buf->write(data->notetrackSuitEntries[i].notetrackSoundMapValues, data->notetrackSuitEntries[i].numSoundMappings);
					buf->clear_pointer(&dest->notetrackSuitEntries[i].notetrackSoundMapValues);
				}
			}
			buf->clear_pointer(&dest->notetrackSuitEntries);
		}

		WEAPON_WRITE_STRING(szAltWeaponName);
		WEAPON_WRITE_ASSET(killIcon, ASSET_TYPE_MATERIAL);
		WEAPON_WRITE_ASSET(dpadIcon, ASSET_TYPE_MATERIAL);
		WEAPON_WRITE_ARR(accuracyGraphKnots[0], data->accuracyGraphKnotCount[0], 3);
		WEAPON_WRITE_ARR(accuracyGraphKnots[1], data->accuracyGraphKnotCount[1], 3);
		WEAPON_WRITE_STRING(szAdsrBaseSetting);
		WEAPON_WRITE_STRING(szLUIWeaponInfoWidgetName);
		WEAPON_WRITE_STRING(szLUIWeaponInfoWidgetTag);
		WEAPON_WRITE_STRING(szLUICrosshairWidget);

		buf->pop_stream();
	}

#define WEAPON_DUMP_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	asset->__field__ ? data[#__field__] = asset->__field__ : nullptr;

#define WEAPON_DUMP_STRING_ARR(__field__, __size__) \
	for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
	{ \
		asset->__field__[idx##__field__] ? data[#__field__][idx##__field__] = asset->__field__ : data[#__field__][idx##__field__] = ""; \
	}

#define WEAPON_DUMP_SCRIPT_STRING(__field__) \
	if (asset->__field__) data[#__field__] = SL_ConvertToString(asset->__field__); else data[#__field__] = "";

#define WEAPON_DUMP_SCRIPT_STRING_ALLOC_ARR(__field__, __size__) \
	if(asset->__field__) \
	{ \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			if (asset->__field__[idx##__field__]) data[#__field__][idx##__field__] = SL_ConvertToString(asset->__field__[idx##__field__]); else data[#__field__][idx##__field__] = ""; \
		} \
	} \
	else \
	{ \
		data[#__field__] = nullptr; \
	}

#define WEAPON_DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__;

#define WEAPON_DUMP_FIELD_ARR(__field__, __size__) \
	for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
	{ \
		data[#__field__][idx##__field__] = asset->__field__[idx##__field__]; \
	}

#define WEAPON_DUMP_FIELD_ALLOC_ARR(__field__, __size__) \
	if(asset->__field__ && __size__) \
	{ \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			data[#__field__][idx##__field__] = asset->__field__[idx##__field__]; \
		} \
	} \
	else \
	{ \
		data[#__field__] = nullptr; \
	}

#define WEAPON_DUMP_ASSET_ARR(__field__, __size__) \
	if (asset->__field__ && __size__) \
	{ \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
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

#define WEAPON_DUMP_ASSET(__field__) \
	if (asset->__field__) \
	{ \
		data[#__field__] = asset->__field__->name; \
	} \
	else \
	{ \
		data[#__field__] = ""; \
	}

#define WEAPON_DUMP_FXCOMBINED(__field__) \
	if (asset->__field__.u.data) \
	{ \
		data[#__field__]["type"] = asset->__field__.type; \
		data[#__field__]["fx"] = asset->__field__.u.fx->name; \
	} \
	else \
	{ \
		data[#__field__] = nullptr; \
	}

#define WEAPON_DUMP_FXCOMBINED_ALLOC_ARR(__field__, __size__) \
	if (asset->__field__) \
	{ \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			if (asset->__field__[idx##__field__].u.data) \
			{ \
				data[#__field__][idx##__field__]["type"] = asset->__field__[idx##__field__].type; \
				data[#__field__][idx##__field__]["fx"] = asset->__field__[idx##__field__].u.fx->name; \
			} \
			else \
			{ \
				data[#__field__][idx##__field__] = nullptr; \
			} \
		} \
	} \
	else \
	{ \
		data[#__field__] = nullptr; \
	}

	void weapon_def::dump(AnimOverride* asset, ordered_json& data)
	{
		WEAPON_DUMP_SCRIPT_STRING(attachment1);
		WEAPON_DUMP_SCRIPT_STRING(attachment2);
		WEAPON_DUMP_ASSET(overrides);
		WEAPON_DUMP_ASSET(overridesAlt);
	}

	void weapon_def::dump(SFXOverride* asset, ordered_json& data)
	{
		WEAPON_DUMP_SCRIPT_STRING(attachment1);
		WEAPON_DUMP_SCRIPT_STRING(attachment2);
		WEAPON_DUMP_ASSET(overrides);
		WEAPON_DUMP_ASSET(overridesAlt);
	}

	void weapon_def::dump(VFXOverride* asset, ordered_json& data)
	{
		WEAPON_DUMP_SCRIPT_STRING(attachment1);
		WEAPON_DUMP_SCRIPT_STRING(attachment2);
		WEAPON_DUMP_ASSET(overrides);
		WEAPON_DUMP_ASSET(overridesAlt);
	}

	void weapon_def::dump(GrenadeRotationParams* asset, ordered_json& data)
	{
		WEAPON_DUMP_FIELD(initialPitch);
		WEAPON_DUMP_FIELD(initialYaw);
		WEAPON_DUMP_FIELD(initialRoll);

		WEAPON_DUMP_FIELD(rotationPitchDir);
		WEAPON_DUMP_FIELD(rotationPitchMin);
		WEAPON_DUMP_FIELD(rotationPitchMax);

		WEAPON_DUMP_FIELD(rotationYawDir);
		WEAPON_DUMP_FIELD(rotationYawMin);
		WEAPON_DUMP_FIELD(rotationYawMax);

		WEAPON_DUMP_FIELD(rotationRollDir);
		WEAPON_DUMP_FIELD(rotationRollMin);
		WEAPON_DUMP_FIELD(rotationRollMax);

		WEAPON_DUMP_FIELD(rotate);
	}

	void weapon_def::dump(ADSOverlay* asset, ordered_json& data)
	{
		WEAPON_DUMP_ASSET(shader);
		WEAPON_DUMP_ASSET(shaderLowRes);
		WEAPON_DUMP_ASSET(shaderEMP);
		WEAPON_DUMP_ASSET(shaderEMPLowRes);
		WEAPON_DUMP_FIELD(reticle);
		WEAPON_DUMP_FIELD(width);
		WEAPON_DUMP_FIELD(height);
		WEAPON_DUMP_FIELD(widthSplitscreen);
		WEAPON_DUMP_FIELD(heightSplitscreen);
	}

	void weapon_def::dump_weapondef(WeaponDef* asset, WeaponCompleteDef* parent, ordered_json& data)
	{
		WEAPON_DUMP_STRING(szOverlayName);
		WEAPON_DUMP_STRING(szModeName);
		WEAPON_DUMP_STRING(szAmmoName);
		WEAPON_DUMP_STRING(szSharedAmmoCapName);
		WEAPON_DUMP_STRING(szUseHintString);
		WEAPON_DUMP_STRING(dropHintString);
		WEAPON_DUMP_STRING(szScript);

		WEAPON_DUMP_ASSET(gunXModel);
		WEAPON_DUMP_ASSET(gunXModelLeftHand);
		WEAPON_DUMP_ASSET(gunXModelRightHand);
		WEAPON_DUMP_ASSET(defaultViewModel);
		WEAPON_DUMP_ASSET(defaultWorldModelLeftHand);
		WEAPON_DUMP_ASSET(defaultWorldModelRightHand);
		WEAPON_DUMP_ASSET(worldModel);
		WEAPON_DUMP_ASSET(worldXModelLeftHand);
		WEAPON_DUMP_ASSET(worldXModelRightHand);
		WEAPON_DUMP_ASSET(defaultWorldModel);
		WEAPON_DUMP_ASSET(playerShadowModel);
		WEAPON_DUMP_ASSET(playerShadowModelLeftHand);
		WEAPON_DUMP_ASSET(playerShadowModelRightHand);
		WEAPON_DUMP_ASSET(handXModel);

		WEAPON_DUMP_ASSET_ARR(complexWorldModel, asset->numComplexModels);
		WEAPON_DUMP_ASSET_ARR(complexWorldModelLeftHand, asset->numComplexModels);
		WEAPON_DUMP_ASSET_ARR(complexWorldModelRightHand, asset->numComplexModels);
		WEAPON_DUMP_ASSET_ARR(complexGunXModel, asset->numComplexModels);
		WEAPON_DUMP_ASSET_ARR(complexGunXModelLeftHand, asset->numComplexModels);
		WEAPON_DUMP_ASSET_ARR(complexGunXModelRightHand, asset->numComplexModels);

		WEAPON_DUMP_ASSET_ARR(camoWorldModelMaterialOverride, 16);
		WEAPON_DUMP_ASSET_ARR(camoViewModelMaterialOverride, 16);

		WEAPON_DUMP_ASSET(szXAnims);
		WEAPON_DUMP_ASSET(szXAnimsRightHanded);
		WEAPON_DUMP_ASSET(szXAnimsLeftHanded);

		WEAPON_DUMP_SCRIPT_STRING_ALLOC_ARR(notetrackRumbleMapKeys, 16);
		WEAPON_DUMP_SCRIPT_STRING_ALLOC_ARR(notetrackRumbleMapValues, 16);
		WEAPON_DUMP_SCRIPT_STRING_ALLOC_ARR(notetrackFXMapKeys, 16);
		WEAPON_DUMP_FXCOMBINED_ALLOC_ARR(notetrackFXMapValues, 16);
		WEAPON_DUMP_SCRIPT_STRING_ALLOC_ARR(notetrackFXMapTagValues, 16);

		WEAPON_DUMP_FIELD(playerAnimType);
		WEAPON_DUMP_FIELD(scriptedAnimEvent);
		WEAPON_DUMP_FIELD(scriptedAnimType);
		WEAPON_DUMP_FIELD(weapType);
		WEAPON_DUMP_FIELD(whizbyType);
		WEAPON_DUMP_FIELD(weapClass);
		WEAPON_DUMP_FIELD(penetrateType);
		WEAPON_DUMP_FIELD(deathAnimDamageType);
		WEAPON_DUMP_FIELD(inventoryType);
		WEAPON_DUMP_FIELD(fireType);

		WEAPON_DUMP_FIELD(targetAssistType);
		WEAPON_DUMP_FIELD(targetAssistBehavior);
		WEAPON_DUMP_FIELD(targetAssistRange);
		WEAPON_DUMP_FIELD(targetAssistAngle);
		WEAPON_DUMP_FIELD(targetAssistLosOffsetForward);
		WEAPON_DUMP_FIELD(targetAssistLosOffsetRight);
		WEAPON_DUMP_FIELD(targetAssistLosOffsetUp);
		WEAPON_DUMP_FIELD(targetAssistOnlyAvailableInAds);

		WEAPON_DUMP_FIELD(offhandClass);
		WEAPON_DUMP_FIELD(stance);

		WEAPON_DUMP_FIELD(stickiness);
		WEAPON_DUMP_FIELD(dualWieldType);
		WEAPON_DUMP_FIELD(slot);

		WEAPON_DUMP_FIELD(iFireTime);
		WEAPON_DUMP_FIELD(iFireDelay);
		WEAPON_DUMP_FIELD(iFireTimeAkimbo);
		WEAPON_DUMP_FIELD(iFireDelayAkimbo);
		WEAPON_DUMP_FIELD(bSegmentedReload);
		WEAPON_DUMP_FIELD(burstFireCooldown);
		WEAPON_DUMP_FIELD(burstFireAuto);

		WEAPON_DUMP_ASSET(vfxPackage);
		WEAPON_DUMP_ASSET(sfxPackage);

		WEAPON_DUMP_FIELD(reticleType);
		WEAPON_DUMP_ASSET(reticleCenter);
		WEAPON_DUMP_ASSET(reticleSide);
		WEAPON_DUMP_ASSET(reticleOnePiece);
		WEAPON_DUMP_FIELD(iReticleCenterSize);
		WEAPON_DUMP_FIELD(iReticleSideSize);
		WEAPON_DUMP_FIELD(iReticleMinOfs);
		WEAPON_DUMP_FIELD(reticleCenterPulse);
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
		WEAPON_DUMP_ASSET(pickupIcon);
		WEAPON_DUMP_ASSET(dangerIcon);
		WEAPON_DUMP_ASSET(throwBackIcon);

		WEAPON_DUMP_FIELD(hideWarningIcons);
		WEAPON_DUMP_FIELD(warningIconsDelay);
		WEAPON_DUMP_ASSET(ammoCounterIcon);
		WEAPON_DUMP_FIELD(ammoCounterClip);

		WEAPON_DUMP_FIELD(iStartAmmo);
		WEAPON_DUMP_FIELD(iPerkStartAmmo);
		WEAPON_DUMP_FIELD(iPerkMaxAmmo);
		//WEAPON_DUMP_FIELD(iAmmoIndex); // runtime
		WEAPON_DUMP_FIELD(iMaxAmmo);
		WEAPON_DUMP_FIELD(shotCount);
		//WEAPON_DUMP_FIELD(iSharedAmmoCapIndex); // runtime
		WEAPON_DUMP_FIELD(iSharedAmmoCap);
		WEAPON_DUMP_FIELD(iAmmoUsedPerShot);
		WEAPON_DUMP_FIELD(requireAmmoUsedPerShot);
		WEAPON_DUMP_FIELD(lowAmmoWarningThreshold);
		WEAPON_DUMP_FIELD(disableNoAmmoWarning);
		WEAPON_DUMP_FIELD(damage);
		WEAPON_DUMP_FIELD(playerDamage);
		WEAPON_DUMP_FIELD(meleeDamage);
		WEAPON_DUMP_FIELD(iDamageType);

		WEAPON_DUMP_FIELD(autoAimRange);
		WEAPON_DUMP_FIELD(aimAssistRange);
		WEAPON_DUMP_FIELD(aimAssistRangeAds);
		WEAPON_DUMP_FIELD(aimAssistPitchSlowdown);
		WEAPON_DUMP_FIELD(aimAssistPitchSlowdownAds);
		WEAPON_DUMP_FIELD(aimAssistYawSlowdown);
		WEAPON_DUMP_FIELD(aimAssistYawSlowdownAds);
		WEAPON_DUMP_FIELD(aimAssistLockonStrength);
		WEAPON_DUMP_FIELD(aimPadding);
		WEAPON_DUMP_FIELD(enemyCrosshairRange);
		WEAPON_DUMP_FIELD(moveSpeedScale);
		WEAPON_DUMP_FIELD(adsMoveSpeedScale);
		WEAPON_DUMP_FIELD(sprintDurationScale);
		WEAPON_DUMP_FIELD(sprintRestoreDelay);
		WEAPON_DUMP_FIELD(fAdsZoomInFrac);
		WEAPON_DUMP_FIELD(fAdsZoomOutFrac);

		dump(&asset->overlay, data["overlay"]);
		WEAPON_DUMP_FIELD(overlayInterface);

		WEAPON_DUMP_FIELD(fAdsBobFactor);
		WEAPON_DUMP_FIELD(fAdsViewBobMult);
		WEAPON_DUMP_FIELD(fHipSpreadStandMin);
		WEAPON_DUMP_FIELD(fHipSpreadDuckedMin);
		WEAPON_DUMP_FIELD(fHipSpreadProneMin);
		WEAPON_DUMP_FIELD(hipSpreadSprintMin);
		WEAPON_DUMP_FIELD(hipSpreadInAirMin);
		WEAPON_DUMP_FIELD(hipSpreadStandMax);
		WEAPON_DUMP_FIELD(hipSpreadDuckedMax);
		WEAPON_DUMP_FIELD(hipSpreadProneMax);
		WEAPON_DUMP_FIELD(hipSpreadSprintMax);
		WEAPON_DUMP_FIELD(hipSpreadInAirMax);
		WEAPON_DUMP_FIELD(fHipSpreadDecayRate);
		WEAPON_DUMP_FIELD(fHipSpreadFireAdd);
		WEAPON_DUMP_FIELD(fHipSpreadTurnAdd);
		WEAPON_DUMP_FIELD(fHipSpreadMoveAdd);
		WEAPON_DUMP_FIELD(fHipSpreadDuckedDecay);
		WEAPON_DUMP_FIELD(fHipSpreadProneDecay);
		WEAPON_DUMP_FIELD(hipSpreadSprintDecay);
		WEAPON_DUMP_FIELD(hipSpreadInAirDecay);
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
		WEAPON_DUMP_FIELD(slideSpreadMin);
		WEAPON_DUMP_FIELD(slideSpreadMax);
		WEAPON_DUMP_FIELD(slideSpreadDecayRate);
		WEAPON_DUMP_FIELD(slideSpreadFireAdd);
		WEAPON_DUMP_FIELD(slideSpreadTurnAdd);
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
		WEAPON_DUMP_FIELD(adsSway_UNKNOWN_Scale); // fixme
		WEAPON_DUMP_FIELD(adsSwayPitchScale);
		WEAPON_DUMP_FIELD(adsSwayYawScale);
		WEAPON_DUMP_FIELD(adsSwayHorizScale);
		WEAPON_DUMP_FIELD(adsSwayVertScale);
		WEAPON_DUMP_FIELD(adsFireRateScale);
		WEAPON_DUMP_FIELD(adsDamageRangeScale);
		WEAPON_DUMP_FIELD(adsFireAnimFrac);
		WEAPON_DUMP_FIELD(fireTimerLerpToAdsScale);
		WEAPON_DUMP_FIELD(alwaysFireAtMaxRangeInAds);

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
		WEAPON_DUMP_FIELD(iExplosionForceRadius);
		WEAPON_DUMP_FIELD(iExplosionInnerDamage);
		WEAPON_DUMP_FIELD(iExplosionOuterDamage);
		WEAPON_DUMP_FIELD(damageConeAngle);
		WEAPON_DUMP_FIELD(bulletExplDmgMult);
		WEAPON_DUMP_FIELD(bulletExplRadiusMult);
		WEAPON_DUMP_FIELD(iProjectileSpeed);
		WEAPON_DUMP_FIELD(iProjectileSpeedUp);
		WEAPON_DUMP_FIELD(iProjectileSpeedForward);
		WEAPON_DUMP_FIELD(iProjectileActivateDist);
		WEAPON_DUMP_FIELD(iProjectileDetonationRadius);
		WEAPON_DUMP_FIELD(projLifetime);
		WEAPON_DUMP_FIELD(projLifetimeStdDeviation);
		WEAPON_DUMP_FIELD(timeToAccelerate);
		WEAPON_DUMP_FIELD(projectileCurvature);
		
		WEAPON_DUMP_ASSET(projectileModel);
		WEAPON_DUMP_FXCOMBINED(projExplosionEffect);
		WEAPON_DUMP_FXCOMBINED(projDudEffect);
		WEAPON_DUMP_FXCOMBINED(projBodyEffect);
		WEAPON_DUMP_FXCOMBINED(projTrailEffect);
		WEAPON_DUMP_FXCOMBINED(projBeaconEffect);
		WEAPON_DUMP_FXCOMBINED(projIgnitionEffect);

		WEAPON_DUMP_FIELD(ricochetChance);
		WEAPON_DUMP_FIELD(riotShieldEnableDamage);
		WEAPON_DUMP_FIELD(riotShieldHealth);
		WEAPON_DUMP_FIELD(riotShieldDamageMult);

		WEAPON_DUMP_FIELD_ARR(vProjectileColor, 3);
		WEAPON_DUMP_FIELD(guidedMissileType);
		WEAPON_DUMP_FIELD(maxSteeringAccel);
		WEAPON_DUMP_FIELD(projIgnitionDelay);

		WEAPON_DUMP_FIELD_ALLOC_ARR(parallelBounce, 64);
		WEAPON_DUMP_FIELD_ALLOC_ARR(perpendicularBounce, 64);

		WEAPON_DUMP_FIELD(fAdsAimPitch);
		WEAPON_DUMP_FIELD(fAdsCrosshairInFrac);
		WEAPON_DUMP_FIELD(fAdsCrosshairOutFrac);
		WEAPON_DUMP_FIELD(adsInCrosshairAlphaStart);
		WEAPON_DUMP_FIELD(adsInCrosshairAlphaEnd);
		WEAPON_DUMP_FIELD(adsOutCrosshairAlphaStart);
		WEAPON_DUMP_FIELD(adsOutCrosshairAlphaEnd);
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
		
		WEAPON_DUMP_FIELD(iVisibilityAxis);
		WEAPON_DUMP_FIELD(fVisibilityUpOffset);
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

		WEAPON_DUMP_FIELD(multipleReloadClipPercentage);
		WEAPON_DUMP_FIELD(kickAlignedInputScalar);
		WEAPON_DUMP_FIELD(kickOpposedInputScalar);
		WEAPON_DUMP_FIELD(fightDist);
		WEAPON_DUMP_FIELD(maxDist);

		for (auto i = 0; i < 2; i++)
		{
			data["accuracyGraphName"][i] = asset->accuracyGraphName[i] ? asset->accuracyGraphName[i] : "";
			for (auto ii = 0; ii < parent->accuracyGraphKnotCount[i]; ii++)
			{
				data["originalAccuracyGraphKnots"][i][ii][0] = asset->originalAccuracyGraphKnots[i][ii][0];
				data["originalAccuracyGraphKnots"][i][ii][1] = asset->originalAccuracyGraphKnots[i][ii][1];
			}
			data["originalAccuracyGraphKnotCount"][i] = asset->originalAccuracyGraphKnotCount[i];
		}

		WEAPON_DUMP_FIELD(iPositionReloadTransTime);
		WEAPON_DUMP_FIELD(leftArc);
		WEAPON_DUMP_FIELD(rightArc);
		WEAPON_DUMP_FIELD(topArc);
		WEAPON_DUMP_FIELD(bottomArc);
		WEAPON_DUMP_FIELD(softLeftRightArc);
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
		//WEAPON_DUMP_FIELD(iUseHintStringIndex); // runtime
		//WEAPON_DUMP_FIELD(dropHintStringIndex); // runtime
		WEAPON_DUMP_FIELD(horizViewJitter);
		WEAPON_DUMP_FIELD(vertViewJitter);
		WEAPON_DUMP_FIELD(scanSpeed);
		WEAPON_DUMP_FIELD(scanAccel);
		WEAPON_DUMP_FIELD(scanPauseTime);
		WEAPON_DUMP_FIELD_ARR(adsSpeedMs, 2);
		
		WEAPON_DUMP_FIELD(minDamage);
		WEAPON_DUMP_FIELD(minPlayerDamage);
		WEAPON_DUMP_FIELD(mid1Damage);
		WEAPON_DUMP_FIELD(mid2Damage);
		WEAPON_DUMP_FIELD(mid3Damage);
		WEAPON_DUMP_FIELD(mid1PlayerDamage);
		WEAPON_DUMP_FIELD(mid2PlayerDamage);
		WEAPON_DUMP_FIELD(mid3PlayerDamage);
		WEAPON_DUMP_FIELD(maxDamageRange);
		WEAPON_DUMP_FIELD(mid1DamageRange);
		WEAPON_DUMP_FIELD(mid2DamageRange);
		WEAPON_DUMP_FIELD(mid3DamageRange);
		WEAPON_DUMP_FIELD(minDamageRange);

		WEAPON_DUMP_FIELD(destabilizationRateTime);
		WEAPON_DUMP_FIELD(destabilizationCurvatureMax);
		WEAPON_DUMP_FIELD(destabilizeDistance);

		WEAPON_DUMP_FIELD(robotechMaxPitch);
		WEAPON_DUMP_FIELD(robotechMaxYaw);
		WEAPON_DUMP_FIELD(robotechFrequency);
		WEAPON_DUMP_FIELD(robotechVariancePitch);
		WEAPON_DUMP_FIELD(robotechVarianceYaw);
		WEAPON_DUMP_FIELD(robotechVarianceFrequency);

		WEAPON_DUMP_FIELD_ALLOC_ARR(locationDamageMultipliers, 22);
		WEAPON_DUMP_FIELD_ALLOC_ARR(hitLocPriorityMap, 22);
		WEAPON_DUMP_FIELD_ARR(unittypeMultipliers, 9);

		WEAPON_DUMP_ASSET(fireRumble);
		WEAPON_DUMP_ASSET(meleeImpactRumble);

		WEAPON_DUMP_FIELD(tracerStyle);
		WEAPON_DUMP_ASSET(tracerType);
		WEAPON_DUMP_ASSET(overchargeTracerType);

		WEAPON_DUMP_ASSET(laserTypeFriendly);
		WEAPON_DUMP_ASSET(laserTypeEnemy);

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
		WEAPON_DUMP_STRING(turretOverheatSound);
		WEAPON_DUMP_FXCOMBINED(turretOverheatEffect);
		WEAPON_DUMP_ASSET(turretBarrelSpinRumble);
		WEAPON_DUMP_FIELD(turretBarrelSpinSpeed);
		WEAPON_DUMP_FIELD(turretBarrelSpinUpTime);
		WEAPON_DUMP_FIELD(turretBarrelSpinDownTime);
		WEAPON_DUMP_STRING(turretBarrelSpinMaxSnd);
		WEAPON_DUMP_STRING_ARR(turretBarrelSpinUpSnd, 4);
		WEAPON_DUMP_STRING_ARR(turretBarrelSpinDownSnd, 4);
		WEAPON_DUMP_STRING(missileConeSoundAlias);
		WEAPON_DUMP_STRING(missileConeSoundAliasAtBase);
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

		WEAPON_DUMP_SCRIPT_STRING(knifeAttachTagOverride);
		WEAPON_DUMP_SCRIPT_STRING(knifeAttachTagOverride2);
		WEAPON_DUMP_FIELD(knifeAlwaysAttached);
		WEAPON_DUMP_FIELD(meleeOverrideValues);
		WEAPON_DUMP_FIELD(aim_automelee_lerp);
		WEAPON_DUMP_FIELD(aim_automelee_region_height);
		WEAPON_DUMP_FIELD(aim_automelee_region_width);
		WEAPON_DUMP_FIELD(aim_automelee_maxPitchMovement);
		WEAPON_DUMP_FIELD(aim_automelee_maxYawMovement);
		WEAPON_DUMP_FIELD(player_meleeHeight);
		WEAPON_DUMP_FIELD(player_meleeWidth);
		WEAPON_DUMP_FIELD(playerMeleeRangeStanding);
		WEAPON_DUMP_FIELD(playerMeleeRangeCrouched);
		WEAPON_DUMP_FIELD(playerMeleeRangeProne);
		WEAPON_DUMP_FIELD(playerMeleeRangeChargeStanding);
		WEAPON_DUMP_FIELD(playerMeleeRangeChargeCrouched);
		WEAPON_DUMP_FIELD(playerMeleeChargeHeightTolerance);

		WEAPON_DUMP_FIELD(shieldAllowFiring);
		WEAPON_DUMP_FIELD(shieldUnkBool);
		WEAPON_DUMP_FIELD(shieldMaxSpeed);
		WEAPON_DUMP_FIELD(shieldAlwaysDisplay);
		WEAPON_DUMP_ASSET(shieldDeployGesture);
		WEAPON_DUMP_ASSET(shieldFireWeapGesture);
		WEAPON_DUMP_ASSET(shieldDeployWhileFiring);
		WEAPON_DUMP_ASSET(shieldRetractWhileFiring);
		WEAPON_DUMP_ASSET(shieldBashGesture);
		WEAPON_DUMP_FXCOMBINED(shieldMeleeFx);
		WEAPON_DUMP_FIELD(shieldMeleeFxDelay);
		WEAPON_DUMP_FIELD(HitEarthquakeScale);
		WEAPON_DUMP_FIELD(HitEarthquakeDuration);
		WEAPON_DUMP_FIELD(HitEarthquakeRadius);
		WEAPON_DUMP_ASSET(shieldHitRumble);
		WEAPON_DUMP_FIELD(MissEarthquakeScale);
		WEAPON_DUMP_FIELD(MissEarthquakeDuration);
		WEAPON_DUMP_FIELD(MissEarthquakeRadius);
		WEAPON_DUMP_ASSET(shieldMissRumble);
		WEAPON_DUMP_FIELD(shieldDeployButton);
		WEAPON_DUMP_FIELD(shieldUsesEnergy);
		WEAPON_DUMP_FIELD(shieldMaxEnergy);
		WEAPON_DUMP_FIELD(shieldConsumptionRate);
		WEAPON_DUMP_FIELD(shieldMeleeEnergyCost);
		WEAPON_DUMP_FIELD(shieldMeleeHitEnergyCost);
		
		WEAPON_DUMP_FIELD(reactiveMotionRadiusScale);
		WEAPON_DUMP_FIELD(reactiveMotionFrequencyScale);
		WEAPON_DUMP_FIELD(reactiveMotionAmplitudeScale);
		WEAPON_DUMP_FIELD(reactiveMotionFalloff);
		WEAPON_DUMP_FIELD(reactiveMotionLifetime);

		WEAPON_DUMP_FIELD(hasAnyTransientModels);
		WEAPON_DUMP_FIELD(sharedAmmo);
		WEAPON_DUMP_FIELD(lockonSupported);
		WEAPON_DUMP_FIELD(requireLockonToFire);
		WEAPON_DUMP_FIELD(isAirburstWeapon);
		WEAPON_DUMP_FIELD(bigExplosion);
		WEAPON_DUMP_FIELD(noAdsWhenMagEmpty);
		WEAPON_DUMP_FIELD(avoidDropCleanup);
		WEAPON_DUMP_FIELD(allowGrenadeSwitching);
		WEAPON_DUMP_FIELD(inheritsPerks);
		WEAPON_DUMP_FIELD(crosshairColorChange);
		WEAPON_DUMP_FIELD(bRifleBullet);
		WEAPON_DUMP_FIELD(bEnergyBullet);
		WEAPON_DUMP_FIELD(armorPiercing);
		WEAPON_DUMP_FIELD(impaling);
		WEAPON_DUMP_FIELD(bBoltAction);
		WEAPON_DUMP_FIELD(aimDownSight);
		WEAPON_DUMP_FIELD(canHoldBreath);
		WEAPON_DUMP_FIELD(meleeOnly);
		WEAPON_DUMP_FIELD(supportsAlternateMelee);
		WEAPON_DUMP_FIELD(canVariableZoom);
		WEAPON_DUMP_FIELD(bRechamberWhileAds);
		WEAPON_DUMP_FIELD(bBulletExplosiveDamage);
		WEAPON_DUMP_FIELD(bCookOffHold);
		WEAPON_DUMP_FIELD(reticleSpin45);
		WEAPON_DUMP_FIELD(bClipOnly);
		WEAPON_DUMP_FIELD(bDoesNotConsumeAmmo);
		WEAPON_DUMP_FIELD(bRemoveWeaponOnEmpty);
		WEAPON_DUMP_FIELD(noAmmoPickup);
		WEAPON_DUMP_FIELD(adsFireOnly);
		WEAPON_DUMP_FIELD(cancelAutoHolsterWhenEmpty);
		WEAPON_DUMP_FIELD(disableSwitchToWhenEmpty);
		WEAPON_DUMP_FIELD(suppressAmmoPrimaryDisplay);
		WEAPON_DUMP_FIELD(suppressAmmoReserveDisplay);
		WEAPON_DUMP_FIELD(laserSightDuringNightvision);
		WEAPON_DUMP_FIELD(markableViewmodel);
		WEAPON_DUMP_FIELD(flipKillIcon);
		WEAPON_DUMP_FIELD(bNoPartialReload);
		WEAPON_DUMP_FIELD(reloadDisabled);
		WEAPON_DUMP_FIELD(blocksProne);
		WEAPON_DUMP_FIELD(silenced);
		WEAPON_DUMP_FIELD(isRollingGrenade);
		WEAPON_DUMP_FIELD(dropGrenadeHeldOnDeath);
		WEAPON_DUMP_FIELD(projExplosionEffectForceNormalUp);
		WEAPON_DUMP_FIELD(projExplosionEffectInheritParentDirection);
		WEAPON_DUMP_FIELD(bProjImpactExplode);
		WEAPON_DUMP_FIELD(disableProjectileCrumpleCheck);
		WEAPON_DUMP_FIELD(bProjTrajectoryEvents);
		WEAPON_DUMP_FIELD(bProjWhizByEnabled);
		WEAPON_DUMP_FIELD(stickToPlayers);
		WEAPON_DUMP_FIELD(stickToVehicles);
		WEAPON_DUMP_FIELD(stickToTurrets);
		WEAPON_DUMP_FIELD(stickToNonStick);
		WEAPON_DUMP_FIELD(projEnableMissileStickiness);
		WEAPON_DUMP_FIELD(thrownSideways);
		WEAPON_DUMP_FIELD(hasDetonator);
		WEAPON_DUMP_FIELD(disableFiring);
		WEAPON_DUMP_FIELD(firesOnWeaponSwitch);
		WEAPON_DUMP_FIELD(disableHolding);
		WEAPON_DUMP_FIELD(timedDetonation);

		WEAPON_DUMP_FIELD(explosiveDamageDelay);
		WEAPON_DUMP_FIELD(fuseTime);
		WEAPON_DUMP_FIELD(aiFuseTime);
		WEAPON_DUMP_FIELD(maxHoldTime);

		WEAPON_DUMP_FIELD(holdButtonToThrow);
		WEAPON_DUMP_FIELD(infiniteHold);
		WEAPON_DUMP_FIELD(freezeMovementWhenFiring);
		WEAPON_DUMP_FIELD(offhandAllowsSprint);
		WEAPON_DUMP_FIELD(thermalScope);
		WEAPON_DUMP_FIELD(thermalToggle);
		WEAPON_DUMP_FIELD(outlineEnemies);
		WEAPON_DUMP_FIELD(outlineDepthTest);
		WEAPON_DUMP_FIELD(outlineFill);

		WEAPON_DUMP_FIELD(enemyOutlineR);
		WEAPON_DUMP_FIELD(enemyOutlineG);
		WEAPON_DUMP_FIELD(enemyOutlineB);
		WEAPON_DUMP_FIELD(enemyOutlineA);
		WEAPON_DUMP_FIELD(allyOutlineR);
		WEAPON_DUMP_FIELD(allyOutlineG);
		WEAPON_DUMP_FIELD(allyOutlineB);
		WEAPON_DUMP_FIELD(allyOutlineA);
		WEAPON_DUMP_FIELD(depthScan);
		WEAPON_DUMP_FIELD(depthScanThickness);
		WEAPON_DUMP_FIELD(depthScanR);
		WEAPON_DUMP_FIELD(depthScanG);
		WEAPON_DUMP_FIELD(depthScanB);
		WEAPON_DUMP_FIELD(depthScanA);
		WEAPON_DUMP_FIELD(depthScanOutlineThickness);
		WEAPON_DUMP_FIELD(depthScanOutlineR);
		WEAPON_DUMP_FIELD(depthScanOutlineG);
		WEAPON_DUMP_FIELD(depthScanOutlineB);
		WEAPON_DUMP_FIELD(depthScanOutlineA);
		WEAPON_DUMP_FIELD(depthScanOverlay);
		WEAPON_DUMP_FIELD(depthScanOverlayStrength);
		WEAPON_DUMP_FIELD(depthScanOverlayXTiles);
		WEAPON_DUMP_FIELD(depthScanOverlayYTiles);
		WEAPON_DUMP_FIELD(depthScanOverlayXScroll);
		WEAPON_DUMP_FIELD(depthScanOverlayYScroll);

		WEAPON_DUMP_FIELD(blurSceneAdsInFraction);
		WEAPON_DUMP_FIELD(blurSceneAdsOutFraction);

		WEAPON_DUMP_FIELD(altModeSameWeapon);
		WEAPON_DUMP_FIELD(turretBarrelSpinEnabled);
		WEAPON_DUMP_FIELD(missileConeSoundEnabled);
		WEAPON_DUMP_FIELD(missileConeSoundPitchshiftEnabled);
		WEAPON_DUMP_FIELD(missileConeSoundCrossfadeEnabled);
		WEAPON_DUMP_FIELD(offhandHoldIsCancelable);
		WEAPON_DUMP_FIELD(doNotAllowAttachmentsToOverrideSpread);
		WEAPON_DUMP_FIELD(useScopeDrift);
		WEAPON_DUMP_FIELD(alwaysShatterGlassOnImpact);
		WEAPON_DUMP_FIELD(oldWeapon);
		WEAPON_DUMP_FIELD(jumpSpread);
		WEAPON_DUMP_FIELD(noFullViewmodelAnimations);

		WEAPON_DUMP_FIELD(killcamOffset);

		WEAPON_DUMP_FIELD(useDualFOV);
		WEAPON_DUMP_FIELD(disableDrop);
		WEAPON_DUMP_FIELD(preferredDrop);

		WEAPON_DUMP_ASSET(gestureAnimation);
		WEAPON_DUMP_FIELD(gestureFireStateTime);
		WEAPON_DUMP_ASSET(gesturePullback);
		WEAPON_DUMP_FIELD(minPullbackTime);
		WEAPON_DUMP_ASSET(gestureThrow);
		WEAPON_DUMP_FIELD(gestureFireTime);
		WEAPON_DUMP_ASSET(gestureDetonate);
		WEAPON_DUMP_FIELD(gestureDetonationTime);
		WEAPON_DUMP_FIELD(unk_433);
		WEAPON_DUMP_STRING(unknownString1);
		WEAPON_DUMP_FIELD(unk_434);
		WEAPON_DUMP_STRING(unknownString2);
		WEAPON_DUMP_FIELD(gesturesDisablePrimary);
		WEAPON_DUMP_FXCOMBINED(cameraFireEffect);
		WEAPON_DUMP_FIELD(cameraFireEffectDurationSec);
		WEAPON_DUMP_FIELD(changedFireTime);
		WEAPON_DUMP_FIELD(changedFireTimeAkimbo);
		WEAPON_DUMP_FIELD(changedFireTimeNumBullets);

		WEAPON_DUMP_FIELD(chargeType);
		WEAPON_DUMP_FIELD(chargeGain);
		WEAPON_DUMP_FIELD(chargeCostPerShot);
		WEAPON_DUMP_FIELD(chargeLossWhenIdle);
		WEAPON_DUMP_FIELD(chargeEmptyCooldown);
		WEAPON_DUMP_FIELD(chargeFireAtMaxDamageMultiplier);
		WEAPON_DUMP_FIELD(chargeMeterEffectBoneCount);
		WEAPON_DUMP_FXCOMBINED(chargeMeterEffect);
		WEAPON_DUMP_STRING(chargeUpSound);
		WEAPON_DUMP_STRING(chargeDownSound);
		WEAPON_DUMP_STRING(chargeUpSoundPlayer);
		WEAPON_DUMP_STRING(chargeDownSoundPlayer);
		WEAPON_DUMP_STRING(chargeDownToUpSound);
		WEAPON_DUMP_STRING(chargeDownToUpSoundPlayer);
		WEAPON_DUMP_STRING(chargeUpToDownSound);
		WEAPON_DUMP_STRING(chargeUpToDownSoundPlayer);
		WEAPON_DUMP_STRING(chargeMaxSound);
		WEAPON_DUMP_STRING(chargeMaxSoundPlayer);
		WEAPON_DUMP_FIELD(chargeHudReveal);
		WEAPON_DUMP_ASSET(chargeRumble);

		WEAPON_DUMP_SCRIPT_STRING(stowTag);
		WEAPON_DUMP_ASSET(stowOffsetModel);

		WEAPON_DUMP_FIELD(maxTargetAngle);
		WEAPON_DUMP_FIELD(spaceshipSecondaryWeapon);
		WEAPON_DUMP_FIELD(impulseFieldRadius);
		WEAPON_DUMP_FIELD(impulseFieldInitialSpeed);
		WEAPON_DUMP_FIELD(impulseFieldMaxSpeed);
		WEAPON_DUMP_FIELD(impulseFieldAcceleration);
		WEAPON_DUMP_FIELD(impulseFieldInAirImpulseMultiplier);
		WEAPON_DUMP_FIELD(impulseFieldInAirImpulseMultiplierInterpTime);
		WEAPON_DUMP_FIELD(impulseFieldSlideMultiplier);
		WEAPON_DUMP_FIELD(impulseFieldSlideMultiplierInterpTime);
		WEAPON_DUMP_FIELD(impulseFieldIsPush);
		WEAPON_DUMP_FIELD(impulseFieldAffectsFriendlies);
		WEAPON_DUMP_FIELD(impulseFieldAffectsSelf);
		WEAPON_DUMP_FIELD(impulseFieldAffectsProne);

		WEAPON_DUMP_FIELD(regenerationEnabled);
		WEAPON_DUMP_FIELD(regenerationTimeMs);
		WEAPON_DUMP_FIELD(regenerationAddTimeMs);
		WEAPON_DUMP_FIELD(regenerationAmount);
		WEAPON_DUMP_FIELD(regenerationConsumeStock);
		WEAPON_DUMP_FIELD(regenerationDisableWhileFiring);

		WEAPON_DUMP_FIELD(deployRequireOnWalkableSurface);
		WEAPON_DUMP_FIELD(deployRequireOnNavmesh);
		WEAPON_DUMP_FIELD(deployRequireSkyAbove);
		WEAPON_DUMP_FIELD(deployRequireNoOverhang);
		WEAPON_DUMP_FIELD(deployAlwaysUpright);
		WEAPON_DUMP_FIELD(deployEdgeSnap);
		WEAPON_DUMP_FIELD(deployCylinderRadius);
		WEAPON_DUMP_FIELD(deployCylinderHeight);
		WEAPON_DUMP_FIELD(deployMaxDistance);
		WEAPON_DUMP_FXCOMBINED(deployEffect);
		WEAPON_DUMP_FIELD(deployValidGroupId);
		WEAPON_DUMP_FIELD(deployIndoorGroupId);
		WEAPON_DUMP_FIELD(deployInvalidGroupId);
		WEAPON_DUMP_FIELD(deployOutOfRangeGroupId);

		WEAPON_DUMP_FIELD(reactiveEmitterDelay);
		WEAPON_DUMP_FIELD(grenadeDangerIconDistance);
		WEAPON_DUMP_FIELD(unk_466_0);

		// unk stuff
		WEAPON_DUMP_FIELD(unk_0);
		WEAPON_DUMP_FIELD(unk_1);
		WEAPON_DUMP_FIELD(unk_2);
		WEAPON_DUMP_FIELD(unk_3);
		WEAPON_DUMP_FIELD(unk_4_0);
		WEAPON_DUMP_FIELD(unk_4_1);

		dump(&asset->rotationParams, data["rotationParams"]);

		for (auto i = 0; i < 21; i++)
		{
			dump(&asset->animOverrides[i], data["animOverrides"][i]);
			dump(&asset->sfxOverrides[i], data["sfxOverrides"][i]);
			dump(&asset->vfxOverrides[i], data["vfxOverrides"][i]);
		}

		{ // unk
			//AttUnknown01; WeaponDef;

#define DUMP_UNK0(__field__) \
			data["unk0"][#__field__] = asset->__field__;

			data["unk0"]["material"] = asset->unkMaterial ? asset->unkMaterial->name : "";
			DUMP_UNK0(unk_18);
			DUMP_UNK0(unk_19);
			DUMP_UNK0(unk_20);
			DUMP_UNK0(unk_21);
			DUMP_UNK0(unk_22);
			DUMP_UNK0(unk_23);
			DUMP_UNK0(unk_24);
			DUMP_UNK0(unk_25);
			DUMP_UNK0(unk_26);
			DUMP_UNK0(unk_27);
			DUMP_UNK0(unk_28);
			DUMP_UNK0(unk_29);
			DUMP_UNK0(unk_30);
			DUMP_UNK0(unk_31);
			DUMP_UNK0(unk_32);
			DUMP_UNK0(unk_33);
			DUMP_UNK0(unk_34);
			DUMP_UNK0(unk_35);
			DUMP_UNK0(unk_36);
			DUMP_UNK0(unk_37);
			DUMP_UNK0(unk_38);
			DUMP_UNK0(unk_39);
			DUMP_UNK0(unk_40);
			DUMP_UNK0(unk_41);
			DUMP_UNK0(unk_42);
			DUMP_UNK0(unk_43);
			DUMP_UNK0(unk_44);
			DUMP_UNK0(unk_45);
			DUMP_UNK0(unk_46);
			DUMP_UNK0(unk_47);
			DUMP_UNK0(unk_48);
			DUMP_UNK0(unk_49);
			DUMP_UNK0(unk_50);
			DUMP_UNK0(unk_51);
			DUMP_UNK0(unk_52);
			DUMP_UNK0(unk_53);
			DUMP_UNK0(unk_54);
			DUMP_UNK0(unk_55);
			DUMP_UNK0(unk_56);
			DUMP_UNK0(unk_57);
			DUMP_UNK0(unk_58);
			DUMP_UNK0(unk_59);
			DUMP_UNK0(unk_60);
			DUMP_UNK0(unk_61);
			DUMP_UNK0(unk_62);
			DUMP_UNK0(unk_63);
			DUMP_UNK0(unk_64);
			DUMP_UNK0(unk_65_0);
			DUMP_UNK0(unk_65_1);
			DUMP_UNK0(unk_65_2);
			DUMP_UNK0(unk_65_3);
		}
	}

	void weapon_def::dump_NoteTrackToSoundEntry(NoteTrackToSoundEntry* asset, ordered_json& data)
	{
		WEAPON_DUMP_SCRIPT_STRING(attachment);
		WEAPON_DUMP_SCRIPT_STRING_ALLOC_ARR(notetrackSoundMapKeys, asset->numSoundMappings);
		WEAPON_DUMP_SCRIPT_STRING_ALLOC_ARR(notetrackSoundMapValues, asset->numSoundMappings);
	}

	void weapon_def::dump_NoteTrackToSuitSoundEntry(NoteTrackToSuitSoundEntry* asset, ordered_json& data)
	{
		WEAPON_DUMP_FIELD(suitAnimState);
		WEAPON_DUMP_SCRIPT_STRING_ALLOC_ARR(notetrackSoundMapKeys, asset->numSoundMappings);
		WEAPON_DUMP_SCRIPT_STRING_ALLOC_ARR(notetrackSoundMapValues, asset->numSoundMappings);
	}

	ordered_json weapon_def::dump_complete(WeaponCompleteDef* asset)
	{
		ordered_json data;

		WEAPON_DUMP_STRING(szInternalName);
		WEAPON_DUMP_STRING(szDisplayName);
		WEAPON_DUMP_STRING(szLootTable);

		dump_weapondef(asset->weapDef, asset, data["weapDef"]);

		WEAPON_DUMP_SCRIPT_STRING_ALLOC_ARR(hideTags, 32);
		WEAPON_DUMP_ASSET_ARR(attachments, asset->numAttachments);
		WEAPON_DUMP_ASSET_ARR(attachments2, asset->numAttachments2);
		WEAPON_DUMP_ASSET_ARR(attachments3, asset->numAttachments3);
		WEAPON_DUMP_ASSET_ARR(attachments4, asset->numAttachments4);
		WEAPON_DUMP_ASSET_ARR(attachments5, asset->numAttachments5);
		WEAPON_DUMP_ASSET_ARR(attachments6, asset->numAttachments6);
		WEAPON_DUMP_SCRIPT_STRING_ALLOC_ARR(notetrackSoundMapKeys, asset->numNotetrackSoundMappings);
		WEAPON_DUMP_SCRIPT_STRING_ALLOC_ARR(notetrackSoundMapValues, asset->numNotetrackSoundMappings);
		if (asset->notetrackOverrides)
		{
			for (auto i = 0u; i < asset->numNotetrackOverrides; i++)
			{
				dump_NoteTrackToSoundEntry(&asset->notetrackOverrides[i], data["notetrackOverrides"][i]);
			}
		}
		else
		{
			data["notetrackOverrides"] = nullptr;
		}
		if (asset->notetrackSuitEntries)
		{
			for (auto i = 0u; i < asset->numNotetrackSuitEntries; i++)
			{
				dump_NoteTrackToSuitSoundEntry(&asset->notetrackSuitEntries[i], data["notetrackSuitEntries"][i]);
			}
		}
		else
		{
			data["notetrackSuitEntries"] = nullptr;
		}
		WEAPON_DUMP_FIELD(fAdsZoomFov);
		WEAPON_DUMP_FIELD(iAdsTransInTime);
		WEAPON_DUMP_FIELD(iAdsTransOutTime);
		WEAPON_DUMP_FIELD(iClipSize);
		WEAPON_DUMP_FIELD(vfxImpactType);
		WEAPON_DUMP_FIELD(sfxImpactType);
		WEAPON_DUMP_FIELD(meleeImpactType);
		WEAPON_DUMP_FIELD(penetrateMultiplier);
		WEAPON_DUMP_FIELD(fAdsViewKickCenterSpeed);
		WEAPON_DUMP_FIELD(fHipViewKickCenterSpeed);
		WEAPON_DUMP_STRING(szAltWeaponName);
		WEAPON_DUMP_ASSET(killIcon);
		WEAPON_DUMP_ASSET(dpadIcon);
		WEAPON_DUMP_FIELD(ammoDropStockMax);
		WEAPON_DUMP_FIELD(adsDofStart);
		WEAPON_DUMP_FIELD(adsDofEnd);
		if (asset->accuracyGraphKnots[0])
		{
			for (auto i = 0; i < asset->accuracyGraphKnotCount[0]; i++)
			{
				data["accuracyGraphKnots"][0][i][0] = asset->accuracyGraphKnots[0][i][0];
				data["accuracyGraphKnots"][0][i][1] = asset->accuracyGraphKnots[0][i][1];
			}
		}
		else
		{
			data["accuracyGraphKnots"][0] = nullptr;
		}
		if (asset->accuracyGraphKnots[1])
		{
			for (auto i = 0; i < asset->accuracyGraphKnotCount[1]; i++)
			{
				data["accuracyGraphKnots"][1][i][0] = asset->accuracyGraphKnots[1][i][0];
				data["accuracyGraphKnots"][1][i][1] = asset->accuracyGraphKnots[1][i][1];
			}
		}
		else
		{
			data["accuracyGraphKnots"][1] = nullptr;
		}
		WEAPON_DUMP_FIELD(motionTracker);
		WEAPON_DUMP_FIELD(enhanced);
		WEAPON_DUMP_FIELD(dpadIconShowsAmmo);
		WEAPON_DUMP_FIELD(luiWeaponInfoWidgetUsesScopeStencil);
		WEAPON_DUMP_STRING(szAdsrBaseSetting);
		WEAPON_DUMP_STRING(szLUIWeaponInfoWidgetName);
		WEAPON_DUMP_STRING(szLUIWeaponInfoWidgetTag);
		WEAPON_DUMP_STRING(szLUICrosshairWidget);

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