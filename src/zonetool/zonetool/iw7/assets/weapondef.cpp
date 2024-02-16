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

#define WEAPON_DUMP_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	asset->__field__ ? data[#__field__] = asset->__field__ : data[#__field__] = "";

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

#define WEAPON_DUMP_FIELD_RENAME(__field__, __name__) \
	data[__name__] = asset->__field__;

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

#define WEAPON_DUMP_FXCOMBINED_ARR(__field__, __size__) \
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

	void dump(AnimOverride* asset, ordered_json& data)
	{
		WEAPON_DUMP_SCRIPT_STRING(attachment1);
		WEAPON_DUMP_SCRIPT_STRING(attachment2);
		WEAPON_DUMP_ASSET(overrides);
		WEAPON_DUMP_ASSET(overridesAlt);
	}

	void dump(SFXOverride* asset, ordered_json& data)
	{
		WEAPON_DUMP_SCRIPT_STRING(attachment1);
		WEAPON_DUMP_SCRIPT_STRING(attachment2);
		WEAPON_DUMP_ASSET(overrides);
		WEAPON_DUMP_ASSET(overridesAlt);
	}

	void dump(VFXOverride* asset, ordered_json& data)
	{
		WEAPON_DUMP_SCRIPT_STRING(attachment1);
		WEAPON_DUMP_SCRIPT_STRING(attachment2);
		WEAPON_DUMP_ASSET(overrides);
		WEAPON_DUMP_ASSET(overridesAlt);
	}

	void dump(GrenadeRotationParams* asset, ordered_json& data)
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

	void dump(ADSOverlay* asset, ordered_json& data)
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

	ordered_json dump_weapondef(WeaponDef* asset, WeaponCompleteDef* parent)
	{
		ordered_json data;

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
		WEAPON_DUMP_FXCOMBINED_ARR(notetrackFXMapValues, 16);
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

		WEAPON_DUMP_FIELD_ARR(parallelBounce, 64);
		WEAPON_DUMP_FIELD_ARR(perpendicularBounce, 64);

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

		WEAPON_DUMP_FIELD_ARR(locationDamageMultipliers, 22);
		WEAPON_DUMP_FIELD_ARR(hitLocPriorityMap, 22);
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

		return data;
	}

	void dump_NoteTrackToSoundEntry(NoteTrackToSoundEntry* asset, ordered_json& data)
	{
		WEAPON_DUMP_SCRIPT_STRING(attachment);
		WEAPON_DUMP_SCRIPT_STRING_ALLOC_ARR(notetrackSoundMapKeys, asset->numSoundMappings);
		WEAPON_DUMP_SCRIPT_STRING_ALLOC_ARR(notetrackSoundMapValues, asset->numSoundMappings);
	}

	void dump_NoteTrackToSuitSoundEntry(NoteTrackToSuitSoundEntry* asset, ordered_json& data)
	{
		WEAPON_DUMP_FIELD(suitAnimState);
		WEAPON_DUMP_SCRIPT_STRING_ALLOC_ARR(notetrackSoundMapKeys, asset->numSoundMappings);
		WEAPON_DUMP_SCRIPT_STRING_ALLOC_ARR(notetrackSoundMapValues, asset->numSoundMappings);
	}

	ordered_json dump_complete(WeaponCompleteDef* asset)
	{
		ordered_json data;

		WEAPON_DUMP_STRING(szInternalName);
		WEAPON_DUMP_STRING(szDisplayName);
		WEAPON_DUMP_STRING(szLootTable);

		data["weapDef"] = dump_weapondef(asset->weapDef, asset);

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