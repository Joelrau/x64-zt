#include <std_include.hpp>
#include "weaponattachment.hpp"

namespace zonetool::iw7
{
	const char* weap_type_str[] =
	{
		"none",
		"bullet",
		"grenade",
		"projectile",
		"riotshield",
		"script",
		"shield",
		"charge shield",
		"8",
		"9",
		"10",
		"11",
		"equip deploy",
	};

	std::string get_weap_type_str(weapType_t type)
	{
		assert(type < WEAPTYPE_NUM);
		return weap_type_str[type];
	}

	weapType_t get_weap_type_from_str(const std::string& type_str)
	{
		for (auto i = 0; i < WEAPTYPE_NUM; i++)
		{
			if (!_stricmp(type_str.data(), weap_type_str[i]))
			{
				return static_cast<weapType_t>(i);
			}
		}

		ZONETOOL_FATAL("Invalid weaponType: %s", type_str.data());

		//return WEAPTYPE_NONE;
	}

	const char* weap_class_str[] =
	{
		"rifle",
		"sniper",
		"mg",
		"smg",
		"spread",
		"pistol",
		"grenade",
		"rocket launcher",
		"turret",
		"throwing knife",
		"non player",
		"item",
		"beam",
		"ball",
		"none",
	};

	std::string get_weap_class_str(weapClass_t class_)
	{
		assert(class_ < WEAPCLASS_NUM);
		return weap_class_str[class_];
	}

	weapClass_t get_weap_class_from_str(const std::string& class_str)
	{
		for (auto i = 0; i < WEAPCLASS_NUM; i++)
		{
			if (!_stricmp(class_str.data(), weap_class_str[i]))
			{
				return static_cast<weapClass_t>(i);
			}
		}

		ZONETOOL_FATAL("Invalid weaponClass: %s", class_str.data());

		//return WEAPCLASS_NONE;
	}

	void weapon_attachment::add_script_string(scr_string_t* ptr, const char* str)
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

	const char* weapon_attachment::get_script_string(scr_string_t* ptr)
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

#define ATTACHMENT_PARSE_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	!data[#__field__].is_null() && !data[#__field__].get<std::string>().empty() ? asset->__field__ = mem->duplicate_string(data[#__field__].get<std::string>()) : asset->__field__ = nullptr;

#define ATTACHMENT_PARSE_SCRIPT_STRING(__field__) \
	if (!data[#__field__].is_null() && !data[#__field__].get<std::string>().empty()) this->add_script_string(&asset->__field__, mem->duplicate_string(data[#__field__].get<std::string>())); asset->__field__ = scr_string_t(0);

#define ATTACHMENT_PARSE_FIELD(__field__) \
	if (!data[#__field__].is_null()) asset->__field__ = data[#__field__].get<decltype(asset->__field__)>();

#define ATTACHMENT_PARSE_FIELD_ARR(__field__, __size__) \
	if(!data[#__field__].is_null()) \
	{ \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			asset->__field__[idx##__field__] = data[#__field__][idx##__field__].get<typename std::remove_reference<decltype(asset->__field__[idx##__field__])>::type>(); \
		} \
	}

#define ATTACHMENT_PARSE_FIELD_ALLOC_ARR(__field__, __size__) \
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

#define ATTACHMENT_PARSE_FIELD_RENAME(__field__, __name__) \
	if (!data[__name__].is_null()) asset->__field__ = data[__name__].get<decltype(asset->__field__)>();

#define ATTACHMENT_PARSE_ASSET_ARR(__field__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		if (__size__) assert(__size__ == static_cast<decltype(__size__)>(data[#__field__].size())); \
		__size__ = static_cast<decltype(__size__)>(data[#__field__].size()); \
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

#define ATTACHMENT_PARSE_ASSET(__field__) \
	if (!data[#__field__].is_null() && !data[#__field__].get<std::string>().empty()) \
	{ \
		asset->__field__ = mem->manual_allocate<typename std::remove_reference<decltype(*asset->__field__)>::type>(sizeof(const char*)); \
		asset->__field__->name = mem->duplicate_string(data[#__field__].get<std::string>()); \
	} \
	else \
	{ \
		asset->__field__ = nullptr; \
	}

	void parse_att(AttAmmoGeneral* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(vfxImpactType);
		ATTACHMENT_PARSE_FIELD(sfxImpactType);
		ATTACHMENT_PARSE_FIELD(meleeImpactType);
		ATTACHMENT_PARSE_FIELD(fireType);
		ATTACHMENT_PARSE_FIELD(burstCount);
	}

	void parse_att(AttAmmoTracer* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_ASSET(tracerType);
		ATTACHMENT_PARSE_FIELD(tracerStyle);
	}

	void parse_att(AttPenetration* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(penetrateType);
		ATTACHMENT_PARSE_FIELD(penetrateMultiplier);
		ATTACHMENT_PARSE_FIELD(rifleBullet);
		ATTACHMENT_PARSE_FIELD(armorPiercing);
		ATTACHMENT_PARSE_FIELD(impaling);
	}

	void parse_att(AttSight* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(blurSceneAdsInFraction);
		ATTACHMENT_PARSE_FIELD(blurSceneAdsOutFraction);
		ATTACHMENT_PARSE_FIELD(aimDownSight);
		ATTACHMENT_PARSE_FIELD(adsFire);
		ATTACHMENT_PARSE_FIELD(rechamberWhileAds);
		ATTACHMENT_PARSE_FIELD(noAdsWhenMagEmpty);
		ATTACHMENT_PARSE_FIELD(canVariableZoom);
		ATTACHMENT_PARSE_FIELD(hideRailWithThisScope);
		ATTACHMENT_PARSE_FIELD(forceIronSightDown);
		ATTACHMENT_PARSE_FIELD(useScopeDrift);
		ATTACHMENT_PARSE_FIELD(useDualFOV);
	}

	void parse_att(AttHoldBreath* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(canHoldBreath);
	}

	void parse_att(AttReload* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(noPartialReload);
		ATTACHMENT_PARSE_FIELD(segmentedReload);
		ATTACHMENT_PARSE_FIELD(reloadDisabled);
	}

	void parse_att(AttAddOns* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(motionTracker);
		ATTACHMENT_PARSE_FIELD(silenced);
		ATTACHMENT_PARSE_FIELD(riotShield);
	}

	void parse_att(AttGeneral* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(boltAction);
		ATTACHMENT_PARSE_FIELD(inheritsPerks);
		ATTACHMENT_PARSE_FIELD(reticleSpin45);
		ATTACHMENT_PARSE_FIELD(adsShouldShowCrosshair);
		ATTACHMENT_PARSE_FIELD(enemyCrosshairRange);
		ATTACHMENT_PARSE_FIELD(reticleType);
		ATTACHMENT_PARSE_ASSET(reticleCenter);
		ATTACHMENT_PARSE_ASSET(reticleSide);
		ATTACHMENT_PARSE_ASSET(reticleOnePiece);
		ATTACHMENT_PARSE_FIELD(reticleCenterSize);
		ATTACHMENT_PARSE_FIELD(reticleSideSize);
		ATTACHMENT_PARSE_STRING(szLUICrosshairWidget);
	}

	void parse_att(AttLaser* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_ASSET(laserTypeFriendly);
		ATTACHMENT_PARSE_ASSET(laserTypeEnemy);
		ATTACHMENT_PARSE_FIELD(forceLaserOn);
	}

	void parse_att(AttAimAssist* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(autoAimRange);
		ATTACHMENT_PARSE_FIELD(aimAssistRange);
		ATTACHMENT_PARSE_FIELD(aimAssistRangeAds);
		ATTACHMENT_PARSE_FIELD(aimAssistPitchSlowdown);
		ATTACHMENT_PARSE_FIELD(aimAssistPitchSlowdownAds);
		ATTACHMENT_PARSE_FIELD(aimAssistYawSlowdown);
		ATTACHMENT_PARSE_FIELD(aimAssistYawSlowdownAds);
		ATTACHMENT_PARSE_FIELD(aimAssistLockonStrength);
	}

	void parse_att(AttTargetAssist* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(targetAssistType);
		ATTACHMENT_PARSE_FIELD(targetAssistBehavior);
		ATTACHMENT_PARSE_FIELD(targetAssistRange);
		ATTACHMENT_PARSE_FIELD(targetAssistAngle);
		ATTACHMENT_PARSE_FIELD(targetAssistLosOffsetForward);
		ATTACHMENT_PARSE_FIELD(targetAssistLosOffsetRight);
		ATTACHMENT_PARSE_FIELD(targetAssistLosOffsetUp);
		ATTACHMENT_PARSE_FIELD(targetAssistOnlyAvailableInAds);
	}

	void parse_att(AttUnknown01* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_ASSET(material);
		ATTACHMENT_PARSE_FIELD(unk_0);
		ATTACHMENT_PARSE_FIELD(unk_1);
		ATTACHMENT_PARSE_FIELD(unk_2);
		ATTACHMENT_PARSE_FIELD(unk_3);
		ATTACHMENT_PARSE_FIELD(unk_4);
		ATTACHMENT_PARSE_FIELD(unk_5);
		ATTACHMENT_PARSE_FIELD(unk_6);
		ATTACHMENT_PARSE_FIELD(unk_7);
		ATTACHMENT_PARSE_FIELD(unk_8);
		ATTACHMENT_PARSE_FIELD(unk_9);
		ATTACHMENT_PARSE_FIELD(unk_10);
		ATTACHMENT_PARSE_FIELD(unk_11);
		ATTACHMENT_PARSE_FIELD(unk_12);
		ATTACHMENT_PARSE_FIELD(unk_13);
		ATTACHMENT_PARSE_FIELD(unk_14);
		ATTACHMENT_PARSE_FIELD(unk_15);
		ATTACHMENT_PARSE_FIELD(unk_16);
		ATTACHMENT_PARSE_FIELD(unk_17);
		ATTACHMENT_PARSE_FIELD(unk_18);
		ATTACHMENT_PARSE_FIELD(unk_19);
		ATTACHMENT_PARSE_FIELD(unk_20);
		ATTACHMENT_PARSE_FIELD(unk_21);
		ATTACHMENT_PARSE_FIELD(unk_22);
		ATTACHMENT_PARSE_FIELD(unk_23);
		ATTACHMENT_PARSE_FIELD(unk_24);
		ATTACHMENT_PARSE_FIELD(unk_25);
		ATTACHMENT_PARSE_FIELD(unk_26);
		ATTACHMENT_PARSE_FIELD(unk_27);
		ATTACHMENT_PARSE_FIELD(unk_28);
		ATTACHMENT_PARSE_FIELD(unk_29);
		ATTACHMENT_PARSE_FIELD(unk_30);
		ATTACHMENT_PARSE_FIELD(unk_31);
		ATTACHMENT_PARSE_FIELD(unk_32);
		ATTACHMENT_PARSE_FIELD(unk_33);
		ATTACHMENT_PARSE_FIELD(unk_34);
		ATTACHMENT_PARSE_FIELD(unk_35);
		ATTACHMENT_PARSE_FIELD(unk_36);
		ATTACHMENT_PARSE_FIELD(unk_37);
		ATTACHMENT_PARSE_FIELD(unk_38);
		ATTACHMENT_PARSE_FIELD(unk_39);
		ATTACHMENT_PARSE_FIELD(unk_40);
		ATTACHMENT_PARSE_FIELD(unk_41);
		ATTACHMENT_PARSE_FIELD(unk_42);
		ATTACHMENT_PARSE_FIELD(unk_43);
		ATTACHMENT_PARSE_FIELD(unk_44);
		ATTACHMENT_PARSE_FIELD(unk_45);
		ATTACHMENT_PARSE_FIELD(unk_46);
		ATTACHMENT_PARSE_FIELD(unk_47);
		ATTACHMENT_PARSE_FIELD(unk_48);
	}

	void parse_att(AttAmmunition* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(maxAmmo);
		ATTACHMENT_PARSE_FIELD(startAmmo);
		ATTACHMENT_PARSE_FIELD(clipSize);
		ATTACHMENT_PARSE_FIELD(shotCount);
		ATTACHMENT_PARSE_FIELD(reloadAmmoAdd);
		ATTACHMENT_PARSE_FIELD(reloadStartAdd);
		ATTACHMENT_PARSE_FIELD(requireAmmoUsedPerShot);
	}

	void parse_att(AttDamage* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(damage);
		ATTACHMENT_PARSE_FIELD(mid1Damage);
		ATTACHMENT_PARSE_FIELD(mid2Damage);
		ATTACHMENT_PARSE_FIELD(mid3Damage);
		ATTACHMENT_PARSE_FIELD(minDamage);
		ATTACHMENT_PARSE_FIELD(meleeDamage);
		ATTACHMENT_PARSE_FIELD(maxDamageRange);
		ATTACHMENT_PARSE_FIELD(mid1DamageRange);
		ATTACHMENT_PARSE_FIELD(mid2DamageRange);
		ATTACHMENT_PARSE_FIELD(mid3DamageRange);
		ATTACHMENT_PARSE_FIELD(minDamageRange);
		ATTACHMENT_PARSE_FIELD(playerDamage);
		ATTACHMENT_PARSE_FIELD(mid1PlayerDamage);
		ATTACHMENT_PARSE_FIELD(mid2PlayerDamage);
		ATTACHMENT_PARSE_FIELD(mid3PlayerDamage);
		ATTACHMENT_PARSE_FIELD(minPlayerDamage);
	}

	void parse_att(AttIdleSettings* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(hipIdleAmount);
		ATTACHMENT_PARSE_FIELD(hipIdleSpeed);
		ATTACHMENT_PARSE_FIELD(idleCrouchFactor);
		ATTACHMENT_PARSE_FIELD(idleProneFactor);
		ATTACHMENT_PARSE_FIELD(adsIdleLerpStartTime);
		ATTACHMENT_PARSE_FIELD(adsIdleLerpTime);
	}

	void parse_att(AttADSSettings* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(adsSpread);
		ATTACHMENT_PARSE_FIELD(adsAimPitch);
		ATTACHMENT_PARSE_FIELD(adsTransInTime);
		ATTACHMENT_PARSE_FIELD(adsTransOutTime);
		ATTACHMENT_PARSE_FIELD(adsReloadTransTime);
		ATTACHMENT_PARSE_FIELD(adsCrosshairInFrac);
		ATTACHMENT_PARSE_FIELD(adsCrosshairOutFrac);
		ATTACHMENT_PARSE_FIELD(adsInCrosshairAlphaStart);
		ATTACHMENT_PARSE_FIELD(adsInCrosshairAlphaEnd);
		ATTACHMENT_PARSE_FIELD(adsOutCrosshairAlphaStart);
		ATTACHMENT_PARSE_FIELD(adsOutCrosshairAlphaEnd);
		ATTACHMENT_PARSE_FIELD(adsZoomFov);
		ATTACHMENT_PARSE_FIELD(adsZoomInFrac);
		ATTACHMENT_PARSE_FIELD(adsZoomOutFrac);
		ATTACHMENT_PARSE_FIELD(adsFovLerpTime);
		ATTACHMENT_PARSE_FIELD(adsBobFactor);
		ATTACHMENT_PARSE_FIELD(adsViewBobMult);
		ATTACHMENT_PARSE_FIELD(adsFireRateScale);
		ATTACHMENT_PARSE_FIELD(adsDamageRangeScale);
		ATTACHMENT_PARSE_FIELD(adsFireAnimFrac);
		ATTACHMENT_PARSE_FIELD(fireTimerLerpToAdsScale);
		ATTACHMENT_PARSE_FIELD(alwaysFireAtMaxRangeInAds);
		ATTACHMENT_PARSE_FIELD(adsAlignEnabled);
		ATTACHMENT_PARSE_FIELD(disableTagAlignX);
	}

	void parse_att(AttScopeDriftSettings* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(fScopeDriftDelay);
		ATTACHMENT_PARSE_FIELD(fScopeDriftLerpInTime);
		ATTACHMENT_PARSE_FIELD(fScopeDriftSteadyTime);
		ATTACHMENT_PARSE_FIELD(fScopeDriftLerpOutTime);
		ATTACHMENT_PARSE_FIELD(fScopeDriftSteadyFactor);
		ATTACHMENT_PARSE_FIELD(fScopeDriftUnsteadyFactor);
	}

	void parse_att(AttHipSpread* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(hipSpreadStandMin);
		ATTACHMENT_PARSE_FIELD(hipSpreadDuckedMin);
		ATTACHMENT_PARSE_FIELD(hipSpreadProneMin);
		ATTACHMENT_PARSE_FIELD(hipSpreadSprintMin);
		ATTACHMENT_PARSE_FIELD(hipSpreadInAirMin);
		ATTACHMENT_PARSE_FIELD(hipSpreadMax);
		ATTACHMENT_PARSE_FIELD(hipSpreadDuckedMax);
		ATTACHMENT_PARSE_FIELD(hipSpreadProneMax);
		ATTACHMENT_PARSE_FIELD(hipSpreadSprintMax);
		ATTACHMENT_PARSE_FIELD(hipSpreadInAirMax);
		ATTACHMENT_PARSE_FIELD(hipSpreadFireAdd);
		ATTACHMENT_PARSE_FIELD(hipSpreadTurnAdd);
		ATTACHMENT_PARSE_FIELD(hipSpreadMoveAdd);
		ATTACHMENT_PARSE_FIELD(hipSpreadDecayRate);
		ATTACHMENT_PARSE_FIELD(hipSpreadDuckedDecay);
		ATTACHMENT_PARSE_FIELD(hipSpreadProneDecay);
		ATTACHMENT_PARSE_FIELD(hipSpreadSprintDecay);
		ATTACHMENT_PARSE_FIELD(hipSpreadInAirDecay);
	}

	void parse_att(AttSlideSpread* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(slideSpreadMin);
		ATTACHMENT_PARSE_FIELD(slideSpreadMax);
		ATTACHMENT_PARSE_FIELD(slideSpreadDecayRate);
		ATTACHMENT_PARSE_FIELD(slideSpreadFireAdd);
		ATTACHMENT_PARSE_FIELD(slideSpreadTurnAdd);
	}

	void parse_att(AttGunKick* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(hipGunKickReducedKickBullets);
		ATTACHMENT_PARSE_FIELD(hipGunKickReducedKickPercent);
		ATTACHMENT_PARSE_FIELD(hipGunKickPitchMin);
		ATTACHMENT_PARSE_FIELD(hipGunKickPitchMax);
		ATTACHMENT_PARSE_FIELD(hipGunKickYawMin);
		ATTACHMENT_PARSE_FIELD(hipGunKickYawMax);
		ATTACHMENT_PARSE_FIELD(hipGunKickMagMin);
		ATTACHMENT_PARSE_FIELD(hipGunKickAccel);
		ATTACHMENT_PARSE_FIELD(hipGunKickSpeedMax);
		ATTACHMENT_PARSE_FIELD(hipGunKickSpeedDecay);
		ATTACHMENT_PARSE_FIELD(hipGunKickStaticDecay);
		ATTACHMENT_PARSE_FIELD(adsGunKickReducedKickBullets);
		ATTACHMENT_PARSE_FIELD(adsGunKickReducedKickPercent);
		ATTACHMENT_PARSE_FIELD(adsGunKickPitchMin);
		ATTACHMENT_PARSE_FIELD(adsGunKickPitchMax);
		ATTACHMENT_PARSE_FIELD(adsGunKickYawMin);
		ATTACHMENT_PARSE_FIELD(adsGunKickYawMax);
		ATTACHMENT_PARSE_FIELD(adsGunKickMagMin);
		ATTACHMENT_PARSE_FIELD(adsGunKickAccel);
		ATTACHMENT_PARSE_FIELD(adsGunKickSpeedMax);
		ATTACHMENT_PARSE_FIELD(adsGunKickSpeedDecay);
		ATTACHMENT_PARSE_FIELD(adsGunKickStaticDecay);
	}

	void parse_att(AttViewKick* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(hipViewKickPitchMin);
		ATTACHMENT_PARSE_FIELD(hipViewKickPitchMax);
		ATTACHMENT_PARSE_FIELD(hipViewKickYawMin);
		ATTACHMENT_PARSE_FIELD(hipViewKickYawMax);
		ATTACHMENT_PARSE_FIELD(hipViewKickMagMin);
		ATTACHMENT_PARSE_FIELD(hipViewKickCenterSpeed);
		ATTACHMENT_PARSE_FIELD(adsViewKickPitchMin);
		ATTACHMENT_PARSE_FIELD(adsViewKickPitchMax);
		ATTACHMENT_PARSE_FIELD(adsViewKickYawMin);
		ATTACHMENT_PARSE_FIELD(adsViewKickYawMax);
		ATTACHMENT_PARSE_FIELD(adsViewKickMagMin);
		ATTACHMENT_PARSE_FIELD(adsViewKickCenterSpeed);
		ATTACHMENT_PARSE_FIELD(kickAlignedInputScalar);
		ATTACHMENT_PARSE_FIELD(kickOpposedInputScalar);
	}

	void parse_att(AttADSOverlay* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_ASSET(overlay.shader);
		ATTACHMENT_PARSE_ASSET(overlay.shaderLowRes);
		ATTACHMENT_PARSE_ASSET(overlay.shaderEMP);
		ATTACHMENT_PARSE_ASSET(overlay.shaderEMPLowRes);
		ATTACHMENT_PARSE_FIELD(overlay.reticle);
		ATTACHMENT_PARSE_FIELD(overlay.width);
		ATTACHMENT_PARSE_FIELD(overlay.height);
		ATTACHMENT_PARSE_FIELD(overlay.widthSplitscreen);
		ATTACHMENT_PARSE_FIELD(overlay.heightSplitscreen);
		ATTACHMENT_PARSE_FIELD(hybridToggle);
		ATTACHMENT_PARSE_FIELD(thermalScope);
		ATTACHMENT_PARSE_FIELD(thermalToggle);
		ATTACHMENT_PARSE_FIELD(outlineEnemies);
	}

	void parse_att(AttOutline* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD_ARR(outlineColor, 4);
		ATTACHMENT_PARSE_FIELD_ARR(outlineAltColor, 4);
		ATTACHMENT_PARSE_FIELD_ARR(outlineFillColor0, 4);
		ATTACHMENT_PARSE_FIELD_ARR(outlineFillColor1, 4);
		ATTACHMENT_PARSE_FIELD_ARR(outlineOccludedOutlineColor, 4);
		ATTACHMENT_PARSE_FIELD_ARR(outlineOccludedInlineColor, 4);
		ATTACHMENT_PARSE_FIELD_ARR(outlineOccludedInteriorColor, 4);
		ATTACHMENT_PARSE_FIELD(outlineWidth);
		ATTACHMENT_PARSE_FIELD(outlineEnemies);
		ATTACHMENT_PARSE_FIELD(outlineDepthTest);
		ATTACHMENT_PARSE_FIELD(outlineFill);
		ATTACHMENT_PARSE_FIELD(outlineScriptControlled);
		ATTACHMENT_PARSE_FIELD(outlineVehicles);
		ATTACHMENT_PARSE_FIELD(outlineTurrets);
		ATTACHMENT_PARSE_FIELD(outlineCharacters);
		ATTACHMENT_PARSE_FIELD(outlineAgents);
		ATTACHMENT_PARSE_FIELD(outlineScriptMovers);
		ATTACHMENT_PARSE_FIELD(outlineLockOn);
	}

	void parse_att(AttUI* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_ASSET(hudIcon);
		ATTACHMENT_PARSE_ASSET(pickupIcon);
		ATTACHMENT_PARSE_ASSET(dangerIcon);
		ATTACHMENT_PARSE_ASSET(throwBackIcon);
		ATTACHMENT_PARSE_FIELD(hideWarningIcons);
		ATTACHMENT_PARSE_FIELD(warningIconsDelay);
		ATTACHMENT_PARSE_ASSET(dpadIcon);
		ATTACHMENT_PARSE_ASSET(ammoCounterIcon);
		ATTACHMENT_PARSE_FIELD(ammoCounterClip);
		ATTACHMENT_PARSE_FIELD(grenadeDangerIconDistance);
	}

	void parse_att(AttRumbles* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_ASSET(fireRumble);
		ATTACHMENT_PARSE_ASSET(meleeImpactRumble);
	}

	void parse_att(AttProjectile* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(explosionRadius);
		ATTACHMENT_PARSE_FIELD(explosionForceRadius);
		ATTACHMENT_PARSE_FIELD(explosionInnerDamage);
		ATTACHMENT_PARSE_FIELD(explosionOuterDamage);
		ATTACHMENT_PARSE_FIELD(damageConeAngle);
		ATTACHMENT_PARSE_FIELD(projectileSpeed);
		ATTACHMENT_PARSE_FIELD(projectileSpeedUp);
		ATTACHMENT_PARSE_FIELD(projectileActivateDist);
		ATTACHMENT_PARSE_FIELD(projectileLifetime);
		ATTACHMENT_PARSE_FIELD(projectileLifetimeStdDeviation);
		ATTACHMENT_PARSE_ASSET(projectileModel);
		ATTACHMENT_PARSE_FIELD(projExplosionType);
		ATTACHMENT_PARSE_FIELD(projExplosionEffect.type);
		ATTACHMENT_PARSE_ASSET(projExplosionEffect.u.fx);
		ATTACHMENT_PARSE_FIELD(projExplosionEffectForceNormalUp);
		ATTACHMENT_PARSE_STRING(projExplosionSound);
		ATTACHMENT_PARSE_FIELD(projDudEffect.type);
		ATTACHMENT_PARSE_ASSET(projDudEffect.u.fx);
		ATTACHMENT_PARSE_STRING(projDudSound);
		ATTACHMENT_PARSE_FIELD(projImpactExplode);
		ATTACHMENT_PARSE_FIELD(disableProjectileCrumpleCheck);
		ATTACHMENT_PARSE_FIELD(destabilizationRateTime);
		ATTACHMENT_PARSE_FIELD(destabilizationCurvatureMax);
		ATTACHMENT_PARSE_FIELD(destabilizeDistance);
		ATTACHMENT_PARSE_FIELD(projBodyEffect.type);
		ATTACHMENT_PARSE_ASSET(projBodyEffect.u.fx);
		ATTACHMENT_PARSE_FIELD(projTrailEffect.type);
		ATTACHMENT_PARSE_ASSET(projTrailEffect.u.fx);
		ATTACHMENT_PARSE_FIELD(projBeaconEffect.type);
		ATTACHMENT_PARSE_ASSET(projBeaconEffect.u.fx);
		ATTACHMENT_PARSE_FIELD(projIgnitionDelay);
		ATTACHMENT_PARSE_FIELD(projIgnitionEffect.type);
		ATTACHMENT_PARSE_ASSET(projIgnitionEffect.u.fx);
		ATTACHMENT_PARSE_STRING(projIgnitionSound);
		ATTACHMENT_PARSE_FIELD(stickiness);
		ATTACHMENT_PARSE_FIELD(fuseTime);
		ATTACHMENT_PARSE_FIELD(aiFuseTime);
		ATTACHMENT_PARSE_FIELD(stickToPlayers);
		ATTACHMENT_PARSE_FIELD(stickToVehicles);
		ATTACHMENT_PARSE_FIELD(stickToTurrets);
		ATTACHMENT_PARSE_FIELD(stickToNonStick);
		ATTACHMENT_PARSE_FIELD(timedDetonation);
		ATTACHMENT_PARSE_FIELD(isPredictedProjectile);
		ATTACHMENT_PARSE_FIELD(useSteppedExplosionDamage);
		ATTACHMENT_PARSE_FIELD(reactiveMotionRadiusScale);
		ATTACHMENT_PARSE_FIELD(reactiveMotionFrequencyScale);
		ATTACHMENT_PARSE_FIELD(reactiveMotionAmplitudeScale);
		ATTACHMENT_PARSE_FIELD(reactiveMotionFalloff);
		ATTACHMENT_PARSE_FIELD(reactiveMotionLifetime);
	}

	void parse_att(AttCharged* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(chargeType);
		ATTACHMENT_PARSE_FIELD(chargeGain);
		ATTACHMENT_PARSE_FIELD(chargeCostPerShot);
		ATTACHMENT_PARSE_FIELD(chargeLossWhenIdle);
		ATTACHMENT_PARSE_FIELD(chargeEmptyCooldown);
		ATTACHMENT_PARSE_FIELD(chargeFireAtMaxDamageMultiplier);
		ATTACHMENT_PARSE_FIELD(chargeMeterEffectBoneCount);
		ATTACHMENT_PARSE_FIELD(chargeMeterEffect.type);
		ATTACHMENT_PARSE_ASSET(chargeMeterEffect.u.fx);
		ATTACHMENT_PARSE_STRING(chargeUpSound);
		ATTACHMENT_PARSE_STRING(chargeDownSound);
		ATTACHMENT_PARSE_STRING(chargeDownToUpSound);
		ATTACHMENT_PARSE_STRING(chargeUpToDownSound);
		ATTACHMENT_PARSE_STRING(chargeMaxSound);
		ATTACHMENT_PARSE_STRING(chargeUpSoundPlayer);
		ATTACHMENT_PARSE_STRING(chargeDownSoundPlayer);
		ATTACHMENT_PARSE_STRING(chargeDownToUpSoundPlayer);
		ATTACHMENT_PARSE_STRING(chargeUpToDownSoundPlayer);
		ATTACHMENT_PARSE_STRING(chargeMaxSoundPlayer);
		ATTACHMENT_PARSE_FIELD(chargeHudReveal);
		ATTACHMENT_PARSE_ASSET(chargeRumble);
	}

	void parse_att(AdsAltSwitch* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(fovInterpTimeSecMainToAlt);
		ATTACHMENT_PARSE_FIELD(fovInterpDelaySecMainToAlt);
		ATTACHMENT_PARSE_FIELD(dualFovSwitchRateMainToAlt);
		ATTACHMENT_PARSE_FIELD(fovInterpTypeMainToAlt);
		ATTACHMENT_PARSE_FIELD(fovInterpTimeSecAltToMain);
		ATTACHMENT_PARSE_FIELD(fovInterpDelaySecAltToMain);
		ATTACHMENT_PARSE_FIELD(dualFovSwitchRateAltToMain);
		ATTACHMENT_PARSE_FIELD(fovInterpTypeAltToMain);
	}

	void parse_att(AttRegeneration* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(regenerationEnabled);
		ATTACHMENT_PARSE_FIELD(regenerationTimeMs);
		ATTACHMENT_PARSE_FIELD(regenerationAddTimeMs);
		ATTACHMENT_PARSE_FIELD(regenerationAmount);
		ATTACHMENT_PARSE_FIELD(regenerationConsumeStock);
		ATTACHMENT_PARSE_FIELD(regenerationDisableWhileFiring);
	}

	void parse_att(AttMovement* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(moveSpeedScale);
		ATTACHMENT_PARSE_FIELD(adsMoveSpeedScale);
		ATTACHMENT_PARSE_FIELD(fastADS3PAnims);
	}

	void parse_att(AttBurst* asset, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_PARSE_FIELD(burstFireCooldown);
		ATTACHMENT_PARSE_FIELD(burstFireAuto);
	}

#define PARSE_ATT(__att__) \
	if (!data[#__att__].is_null()) \
	{ \
		asset->__att__ = mem->allocate<typename std::remove_reference<decltype(*asset->__att__)>::type>(); \
		parse_att(asset->__att__, data[#__att__], mem);  \
	} \
	else \
	{ \
		 asset->__att__ = nullptr; \
	}

	WeaponAttachment* weapon_attachment::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "attachments\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing attachment \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();
		ordered_json data = json::parse(bytes);

		auto* asset = mem->allocate<WeaponAttachment>();

		ATTACHMENT_PARSE_STRING(szInternalName);
		ATTACHMENT_PARSE_STRING(szDisplayName);
		this->add_script_string(&asset->internalName, asset->szInternalName);
		ATTACHMENT_PARSE_FIELD(papAttachment);
		ATTACHMENT_PARSE_FIELD(type);
		ATTACHMENT_PARSE_FIELD(weaponType);
		ATTACHMENT_PARSE_FIELD_RENAME(weapClass, "weaponClass");
		ATTACHMENT_PARSE_ASSET(worldModelCamo);
		ATTACHMENT_PARSE_ASSET(viewModelCamo);
		ATTACHMENT_PARSE_ASSET_ARR(worldModelVariations, asset->numModelVariations);
		ATTACHMENT_PARSE_ASSET_ARR(viewModelVariations, asset->numModelVariations);
		ATTACHMENT_PARSE_ASSET_ARR(worldModelCamoMaterials, asset->numWorldModelCamoMaterials);
		ATTACHMENT_PARSE_ASSET_ARR(viewModelCamoMaterials, asset->numViewModelCamoMaterials);
		ATTACHMENT_PARSE_SCRIPT_STRING(attachPoint);
		ATTACHMENT_PARSE_ASSET_ARR(reticles, asset->numReticles);
		ATTACHMENT_PARSE_ASSET_ARR(altReticles, asset->numAltReticles);
		PARSE_ATT(ammogeneral);
		PARSE_ATT(tracer);
		PARSE_ATT(penetration);
		PARSE_ATT(sight);
		PARSE_ATT(holdBreath);
		PARSE_ATT(reload);
		PARSE_ATT(addOns);
		PARSE_ATT(laser);
		PARSE_ATT(general);
		PARSE_ATT(aimAssist);
		PARSE_ATT(targetAssist);
		PARSE_ATT(unkAtt01);
		PARSE_ATT(ammunition);
		PARSE_ATT(damage);
		PARSE_ATT(idleSettings);
		PARSE_ATT(adsSettings);
		PARSE_ATT(adsSettingsMain);
		PARSE_ATT(scopeDriftSettings);
		PARSE_ATT(scopeDriftSettingsMain);
		PARSE_ATT(hipSpread);
		PARSE_ATT(slideSpread);
		PARSE_ATT(gunKick);
		PARSE_ATT(viewKick);
		PARSE_ATT(adsOverlay);
		PARSE_ATT(outline);
		PARSE_ATT(ui);
		PARSE_ATT(rumbles);
		PARSE_ATT(projectile);
		PARSE_ATT(charged);
		PARSE_ATT(adsAltSwitch);
		PARSE_ATT(regeneration);
		PARSE_ATT(movement);
		PARSE_ATT(burst);
		ATTACHMENT_PARSE_FIELD_ALLOC_ARR(locationDamage, 22);
		ATTACHMENT_PARSE_FIELD(ammunitionScale);
		ATTACHMENT_PARSE_FIELD(damageScale);
		ATTACHMENT_PARSE_FIELD(stateTimersScale);
		ATTACHMENT_PARSE_FIELD(reloadTimersScale);
		ATTACHMENT_PARSE_FIELD(fireTimeOverride);
		ATTACHMENT_PARSE_FIELD(fireTimeAkimboOverride);
		ATTACHMENT_PARSE_FIELD(fireTimersScale);
		ATTACHMENT_PARSE_FIELD(burstFireCooldownScale);
		ATTACHMENT_PARSE_FIELD(idleHipMotionScale);
		ATTACHMENT_PARSE_FIELD(idleADSMotionScale);
		ATTACHMENT_PARSE_FIELD(idleADSMotionTimeScale);
		ATTACHMENT_PARSE_FIELD(adsSettingsScale);
		ATTACHMENT_PARSE_FIELD(adsSettingsScaleMain);
		ATTACHMENT_PARSE_FIELD(hipSpreadScale);
		ATTACHMENT_PARSE_FIELD(slideSpreadScale);
		ATTACHMENT_PARSE_FIELD(gunKickScale);
		ATTACHMENT_PARSE_FIELD(viewKickScale);
		ATTACHMENT_PARSE_FIELD(viewCenterScale);
		ATTACHMENT_PARSE_FIELD(adsSwayScale);
		ATTACHMENT_PARSE_FIELD(adsSwayScale2);
		ATTACHMENT_PARSE_FIELD(adsSwayScale3);
		ATTACHMENT_PARSE_FIELD(iAmmoIndex); // probably runtime data
		ATTACHMENT_PARSE_FIELD(loadIndex); // ^
		ATTACHMENT_PARSE_FIELD(useAlternateViewModelLaser);
		ATTACHMENT_PARSE_FIELD(hideIronSightsWithThisAttachment);
		ATTACHMENT_PARSE_FIELD(shareAmmoWithAlt);
		ATTACHMENT_PARSE_FIELD(useLeftTriggerAltFire);
		ATTACHMENT_PARSE_FIELD(containsIKTag);
		ATTACHMENT_PARSE_FIELD(isAkimboAttachment);
		ATTACHMENT_PARSE_FIELD(attachLeftWeaponAkimbo);
		ATTACHMENT_PARSE_FIELD(attachRightWeaponAkimbo);
		ATTACHMENT_PARSE_FIELD(luiWeaponInfoWidgetUsesScopeStencil);
		ATTACHMENT_PARSE_STRING(szAltModeName);
		ATTACHMENT_PARSE_STRING(szLUIWeaponInfoWidgetName);
		ATTACHMENT_PARSE_STRING(szLUIWeaponInfoWidgetTag);

		return asset;
	}

	void weapon_attachment::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_copy<WeaponAttachment>(XAssetType(this->type()), this->name().data(), mem).attachment;
			if(this->asset_->internalName) this->add_script_string(&this->asset_->internalName, SL_ConvertToString(this->asset_->internalName));
			if(this->asset_->attachPoint) this->add_script_string(&this->asset_->attachPoint, SL_ConvertToString(this->asset_->attachPoint));
		}
	}

	void weapon_attachment::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* asset = this->asset_;
		
		asset->internalName = static_cast<scr_string_t>(buf->write_scriptstring(this->get_script_string(&asset->internalName)));
		asset->attachPoint = static_cast<scr_string_t>(buf->write_scriptstring(this->get_script_string(&asset->attachPoint)));
	}

#define ATTACHMENT_SUBASSET_DEPENDING(__field__,__type__) \
	if (asset->__field__) \
	{ \
		zone->add_asset_of_type(__type__, asset->__field__->name); \
	}

#define ATTACHMENT_ATT_SUBASSET_DEPENDING(__att__,__field__,__type__) \
	if (asset->__att__ && asset->__att__->__field__) \
	{ \
		zone->add_asset_of_type(__type__, asset->__att__->__field__->name); \
	}

#define ATTACHMENT_ATT_SUBASSET_DEPENDING_FXCOMBINED(__att__,__field__) \
	if (asset->__att__ && asset->__att__->__field__.u.data) \
	{ \
		if(asset->__att__->__field__.type == FX_COMBINED_VFX) zone->add_asset_of_type(ASSET_TYPE_VFX, asset->__att__->__field__.u.vfx->name); \
		else zone->add_asset_of_type(ASSET_TYPE_FX, asset->__att__->__field__.u.fx->name); \
	}

	void weapon_attachment::load_depending(zone_base* zone)
	{
		auto* asset = this->asset_;

		ATTACHMENT_SUBASSET_DEPENDING(worldModelCamo, ASSET_TYPE_XMODEL);
		ATTACHMENT_SUBASSET_DEPENDING(viewModelCamo, ASSET_TYPE_XMODEL);
		for (auto i = 0u; i < asset->numModelVariations; i++)
		{
			ATTACHMENT_SUBASSET_DEPENDING(worldModelVariations[i], ASSET_TYPE_XMODEL);
			ATTACHMENT_SUBASSET_DEPENDING(viewModelVariations[i], ASSET_TYPE_XMODEL);
		}
		for (auto i = 0u; i < asset->numWorldModelCamoMaterials; i++)
		{
			ATTACHMENT_SUBASSET_DEPENDING(worldModelCamoMaterials[i], ASSET_TYPE_MATERIAL);
		}
		for (auto i = 0u; i < asset->numViewModelCamoMaterials; i++)
		{
			ATTACHMENT_SUBASSET_DEPENDING(viewModelCamoMaterials[i], ASSET_TYPE_MATERIAL);
		}
		for (auto i = 0; i < asset->numReticles; i++)
		{
			ATTACHMENT_SUBASSET_DEPENDING(reticles[i], ASSET_TYPE_RETICLE);
		}
		for (auto i = 0; i < asset->numAltReticles; i++)
		{
			ATTACHMENT_SUBASSET_DEPENDING(altReticles[i], ASSET_TYPE_RETICLE);
		}

		ATTACHMENT_ATT_SUBASSET_DEPENDING(tracer, tracerType, ASSET_TYPE_TRACER);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(general, reticleCenter, ASSET_TYPE_MATERIAL);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(general, reticleSide, ASSET_TYPE_MATERIAL);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(general, reticleOnePiece, ASSET_TYPE_MATERIAL);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(laser, laserTypeFriendly, ASSET_TYPE_LASER);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(laser, laserTypeEnemy, ASSET_TYPE_LASER);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(unkAtt01, material, ASSET_TYPE_MATERIAL);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(adsOverlay, overlay.shader, ASSET_TYPE_MATERIAL);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(adsOverlay, overlay.shaderLowRes, ASSET_TYPE_MATERIAL);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(adsOverlay, overlay.shaderEMP, ASSET_TYPE_MATERIAL);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(adsOverlay, overlay.shaderEMPLowRes, ASSET_TYPE_MATERIAL);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(ui, hudIcon, ASSET_TYPE_MATERIAL);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(ui, pickupIcon, ASSET_TYPE_MATERIAL);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(ui, dangerIcon, ASSET_TYPE_MATERIAL);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(ui, throwBackIcon, ASSET_TYPE_MATERIAL);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(ui, dpadIcon, ASSET_TYPE_MATERIAL);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(ui, ammoCounterIcon, ASSET_TYPE_MATERIAL);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(rumbles, fireRumble, ASSET_TYPE_RUMBLE);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(rumbles, meleeImpactRumble, ASSET_TYPE_RUMBLE);
		ATTACHMENT_ATT_SUBASSET_DEPENDING(projectile, projectileModel, ASSET_TYPE_XMODEL);
		ATTACHMENT_ATT_SUBASSET_DEPENDING_FXCOMBINED(projectile, projExplosionEffect);
		// projExplosionSound
		ATTACHMENT_ATT_SUBASSET_DEPENDING_FXCOMBINED(projectile, projDudEffect);
		// projDudSound
		ATTACHMENT_ATT_SUBASSET_DEPENDING_FXCOMBINED(projectile, projBodyEffect);
		ATTACHMENT_ATT_SUBASSET_DEPENDING_FXCOMBINED(projectile, projTrailEffect);
		ATTACHMENT_ATT_SUBASSET_DEPENDING_FXCOMBINED(projectile, projBeaconEffect);
		ATTACHMENT_ATT_SUBASSET_DEPENDING_FXCOMBINED(projectile, projIgnitionEffect);
		// projIgnitionSound
		ATTACHMENT_ATT_SUBASSET_DEPENDING_FXCOMBINED(charged, chargeMeterEffect);
		// chargeUpSound
		// chargeDownSound
		// chargeDownToUpSound
		// chargeUpToDownSound
		// chargeMaxSound
		// chargeUpSoundPlayer
		// chargeDownSoundPlayer
		// chargeDownToUpSoundPlayer
		// chargeUpToDownSoundPlayer
		// chargeMaxSoundPlayer
		ATTACHMENT_ATT_SUBASSET_DEPENDING(charged, chargeRumble, ASSET_TYPE_RUMBLE);
	}

	std::string weapon_attachment::name()
	{
		return this->name_;
	}

	std::int32_t weapon_attachment::type()
	{
		return ASSET_TYPE_ATTACHMENT;
	}

#define ATTACHMENT_WRITE_STRING(__field__) \
	if (data->__field__) \
	{ \
		dest->__field__ = buf->write_str(data->__field__); \
	}

#define ATTACHMENT_WRITE_ASSET(__field__,__type__) \
	if (data->__field__) \
	{ \
		dest->__field__ = reinterpret_cast<typename std::remove_reference<decltype(data->__field__)>::type>(zone->get_asset_pointer(__type__, data->__field__->name)); \
	}

#define ATTACHMENT_WRITE_ASSET_ARR(__field__, __type__, __size__, __align__) \
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

#define ATTACHMENT_WRITE_FXCOMBINED(__field__) \
	if (data->__field__.u.data) \
	{ \
		if(data->__field__.type == FX_COMBINED_VFX) \
			dest->__field__.u.vfx = reinterpret_cast<ParticleSystemDef*>(zone->get_asset_pointer(ASSET_TYPE_VFX, data->__field__.u.vfx->name)); \
		else \
			dest->__field__.u.fx = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(ASSET_TYPE_FX, data->__field__.u.fx->name)); \
	}

	void write_att(AttAmmoGeneral* data, AttAmmoGeneral* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttAmmoTracer* data, AttAmmoTracer* dest, zone_base* zone, zone_buffer* buf)
	{
		ATTACHMENT_WRITE_ASSET(tracerType, ASSET_TYPE_TRACER);
	}

	void write_att(AttPenetration* data, AttPenetration* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttSight* data, AttSight* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttHoldBreath* data, AttHoldBreath* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttReload* data, AttReload* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttAddOns* data, AttAddOns* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttGeneral* data, AttGeneral* dest, zone_base* zone, zone_buffer* buf)
	{
		ATTACHMENT_WRITE_ASSET(reticleCenter, ASSET_TYPE_MATERIAL);
		ATTACHMENT_WRITE_ASSET(reticleSide, ASSET_TYPE_MATERIAL);
		ATTACHMENT_WRITE_ASSET(reticleOnePiece, ASSET_TYPE_MATERIAL);
		ATTACHMENT_WRITE_STRING(szLUICrosshairWidget);
	}

	void write_att(AttLaser* data, AttLaser* dest, zone_base* zone, zone_buffer* buf)
	{
		ATTACHMENT_WRITE_ASSET(laserTypeFriendly, ASSET_TYPE_LASER);
		ATTACHMENT_WRITE_ASSET(laserTypeEnemy, ASSET_TYPE_LASER);
	}

	void write_att(AttAimAssist* data, AttAimAssist* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttTargetAssist* data, AttTargetAssist* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttUnknown01* data, AttUnknown01* dest, zone_base* zone, zone_buffer* buf)
	{
		ATTACHMENT_WRITE_ASSET(material, ASSET_TYPE_MATERIAL);
	}

	void write_att(AttAmmunition* data, AttAmmunition* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttDamage* data, AttDamage* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttIdleSettings* data, AttIdleSettings* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttADSSettings* data, AttADSSettings* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttScopeDriftSettings* data, AttScopeDriftSettings* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttHipSpread* data, AttHipSpread* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttSlideSpread* data, AttSlideSpread* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttGunKick* data, AttGunKick* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttViewKick* data, AttViewKick* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttADSOverlay* data, AttADSOverlay* dest, zone_base* zone, zone_buffer* buf)
	{
		ATTACHMENT_WRITE_ASSET(overlay.shader, ASSET_TYPE_MATERIAL);
		ATTACHMENT_WRITE_ASSET(overlay.shaderLowRes, ASSET_TYPE_MATERIAL);
		ATTACHMENT_WRITE_ASSET(overlay.shaderEMP, ASSET_TYPE_MATERIAL);
		ATTACHMENT_WRITE_ASSET(overlay.shaderEMPLowRes, ASSET_TYPE_MATERIAL);
	}

	void write_att(AttOutline* data, AttOutline* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttUI* data, AttUI* dest, zone_base* zone, zone_buffer* buf)
	{
		ATTACHMENT_WRITE_ASSET(hudIcon, ASSET_TYPE_MATERIAL);
		ATTACHMENT_WRITE_ASSET(pickupIcon, ASSET_TYPE_MATERIAL);
		ATTACHMENT_WRITE_ASSET(dangerIcon, ASSET_TYPE_MATERIAL);
		ATTACHMENT_WRITE_ASSET(throwBackIcon, ASSET_TYPE_MATERIAL);
		ATTACHMENT_WRITE_ASSET(dpadIcon, ASSET_TYPE_MATERIAL);
		ATTACHMENT_WRITE_ASSET(ammoCounterIcon, ASSET_TYPE_MATERIAL);
	}

	void write_att(AttRumbles* data, AttRumbles* dest, zone_base* zone, zone_buffer* buf)
	{
		ATTACHMENT_WRITE_ASSET(fireRumble, ASSET_TYPE_RUMBLE);
		ATTACHMENT_WRITE_ASSET(meleeImpactRumble, ASSET_TYPE_RUMBLE);
	}

	void write_att(AttProjectile* data, AttProjectile* dest, zone_base* zone, zone_buffer* buf)
	{
		ATTACHMENT_WRITE_ASSET(projectileModel, ASSET_TYPE_XMODEL);
		ATTACHMENT_WRITE_FXCOMBINED(projExplosionEffect);
		ATTACHMENT_WRITE_STRING(projExplosionSound);
		ATTACHMENT_WRITE_FXCOMBINED(projDudEffect);
		ATTACHMENT_WRITE_STRING(projDudSound);
		ATTACHMENT_WRITE_FXCOMBINED(projBodyEffect);
		ATTACHMENT_WRITE_FXCOMBINED(projTrailEffect);
		ATTACHMENT_WRITE_FXCOMBINED(projBeaconEffect);
		ATTACHMENT_WRITE_FXCOMBINED(projIgnitionEffect);
		ATTACHMENT_WRITE_STRING(projIgnitionSound);
	}

	void write_att(AttCharged* data, AttCharged* dest, zone_base* zone, zone_buffer* buf)
	{
		ATTACHMENT_WRITE_FXCOMBINED(chargeMeterEffect);
		ATTACHMENT_WRITE_STRING(chargeUpSound);
		ATTACHMENT_WRITE_STRING(chargeDownSound);
		ATTACHMENT_WRITE_STRING(chargeDownToUpSound);
		ATTACHMENT_WRITE_STRING(chargeUpToDownSound);
		ATTACHMENT_WRITE_STRING(chargeMaxSound);
		ATTACHMENT_WRITE_STRING(chargeUpSoundPlayer);
		ATTACHMENT_WRITE_STRING(chargeDownSoundPlayer);
		ATTACHMENT_WRITE_STRING(chargeDownToUpSoundPlayer);
		ATTACHMENT_WRITE_STRING(chargeUpToDownSoundPlayer);
		ATTACHMENT_WRITE_STRING(chargeMaxSoundPlayer);
		ATTACHMENT_WRITE_ASSET(chargeRumble, ASSET_TYPE_RUMBLE);
	}

	void write_att(AdsAltSwitch* data, AdsAltSwitch* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttRegeneration* data, AttRegeneration* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttMovement* data, AttMovement* dest, zone_base* zone, zone_buffer* buf)
	{
	}

	void write_att(AttBurst* data, AttBurst* dest, zone_base* zone, zone_buffer* buf)
	{
	}

#define WRITE_ATT(__att__, __align__) \
	if(data->__att__) \
	{ \
		buf->align(__align__); \
		dest->__att__ = buf->write(data->__att__); \
		write_att(data->__att__, dest->__att__, zone, buf); \
		buf->clear_pointer(&dest->__att__); \
	}

	void weapon_attachment::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->szInternalName = buf->write_str(this->name());

		ATTACHMENT_WRITE_STRING(szDisplayName);

		ATTACHMENT_WRITE_ASSET(worldModelCamo, ASSET_TYPE_XMODEL);
		ATTACHMENT_WRITE_ASSET(viewModelCamo, ASSET_TYPE_XMODEL);
		ATTACHMENT_WRITE_ASSET_ARR(worldModelVariations, ASSET_TYPE_XMODEL, data->numModelVariations, 7);
		ATTACHMENT_WRITE_ASSET_ARR(viewModelVariations, ASSET_TYPE_XMODEL, data->numModelVariations, 7);
		ATTACHMENT_WRITE_ASSET_ARR(worldModelCamoMaterials, ASSET_TYPE_MATERIAL, data->numWorldModelCamoMaterials, 7);
		ATTACHMENT_WRITE_ASSET_ARR(viewModelCamoMaterials, ASSET_TYPE_MATERIAL, data->numViewModelCamoMaterials, 7);
		ATTACHMENT_WRITE_ASSET_ARR(reticles, ASSET_TYPE_RETICLE, data->numReticles, 7);
		ATTACHMENT_WRITE_ASSET_ARR(altReticles, ASSET_TYPE_RETICLE, data->numAltReticles, 7);

		WRITE_ATT(ammogeneral, 3);
		WRITE_ATT(tracer, 7);
		WRITE_ATT(penetration, 3);
		WRITE_ATT(sight, 3);
		WRITE_ATT(holdBreath, 0);
		WRITE_ATT(reload, 0);
		WRITE_ATT(addOns, 0);
		WRITE_ATT(general, 7);
		WRITE_ATT(laser, 7);
		WRITE_ATT(aimAssist, 3);
		WRITE_ATT(targetAssist, 3);
		WRITE_ATT(unkAtt01, 7);
		WRITE_ATT(ammunition, 3);
		WRITE_ATT(damage, 3);
		WRITE_ATT(idleSettings, 3);
		WRITE_ATT(adsSettings, 3);
		WRITE_ATT(adsSettingsMain, 3);
		WRITE_ATT(scopeDriftSettings, 3);
		WRITE_ATT(scopeDriftSettingsMain, 3);
		WRITE_ATT(hipSpread, 3);
		WRITE_ATT(slideSpread, 3);
		WRITE_ATT(gunKick, 3);
		WRITE_ATT(viewKick, 3);
		WRITE_ATT(adsOverlay, 7);
		WRITE_ATT(outline, 3);
		WRITE_ATT(ui, 7);
		WRITE_ATT(rumbles, 7);
		WRITE_ATT(projectile, 7);
		WRITE_ATT(charged, 7);
		WRITE_ATT(adsAltSwitch, 3);
		WRITE_ATT(regeneration, 3);
		WRITE_ATT(movement, 3);
		WRITE_ATT(burst, 3);

		if (data->locationDamage)
		{
			buf->align(3);
			buf->write(data->locationDamage, 22);
			buf->clear_pointer(&dest->locationDamage);
		}

		ATTACHMENT_WRITE_STRING(szAltModeName);
		ATTACHMENT_WRITE_STRING(szLUIWeaponInfoWidgetName);
		ATTACHMENT_WRITE_STRING(szLUIWeaponInfoWidgetTag);

		buf->pop_stream();
	}

#define ATTACHMENT_DUMP_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	asset->__field__ ? data[#__field__] = asset->__field__ : data[#__field__] = nullptr;

#define ATTACHMENT_DUMP_SCRIPT_STRING(__field__) \
	if (asset->__field__) data[#__field__] = SL_ConvertToString(asset->__field__); else data[#__field__] = "";

#define ATTACHMENT_DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__;

#define ATTACHMENT_DUMP_FIELD_ARR(__field__, __size__) \
	for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
	{ \
		data[#__field__][idx##__field__] = asset->__field__[idx##__field__]; \
	}

#define ATTACHMENT_DUMP_FIELD_ALLOC_ARR(__field__, __size__) \
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

#define ATTACHMENT_DUMP_FIELD_RENAME(__field__, __name__) \
	data[__name__] = asset->__field__;

#define ATTACHMENT_DUMP_ASSET_ARR(__field__, __size__) \
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

#define ATTACHMENT_DUMP_ASSET(__field__) \
	if (asset->__field__) \
	{ \
		data[#__field__] = asset->__field__->name; \
	} \
	else \
	{ \
		data[#__field__] = ""; \
	}

	void dump_att(AttAmmoGeneral* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(vfxImpactType);
		ATTACHMENT_DUMP_FIELD(sfxImpactType);
		ATTACHMENT_DUMP_FIELD(meleeImpactType);
		ATTACHMENT_DUMP_FIELD(fireType);
		ATTACHMENT_DUMP_FIELD(burstCount);
	}

	void dump_att(AttAmmoTracer* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_ASSET(tracerType);
		ATTACHMENT_DUMP_FIELD(tracerStyle);
	}

	void dump_att(AttPenetration* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(penetrateType);
		ATTACHMENT_DUMP_FIELD(penetrateMultiplier);
		ATTACHMENT_DUMP_FIELD(rifleBullet);
		ATTACHMENT_DUMP_FIELD(armorPiercing);
		ATTACHMENT_DUMP_FIELD(impaling);
	}

	void dump_att(AttSight* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(blurSceneAdsInFraction);
		ATTACHMENT_DUMP_FIELD(blurSceneAdsOutFraction);
		ATTACHMENT_DUMP_FIELD(aimDownSight);
		ATTACHMENT_DUMP_FIELD(adsFire);
		ATTACHMENT_DUMP_FIELD(rechamberWhileAds);
		ATTACHMENT_DUMP_FIELD(noAdsWhenMagEmpty);
		ATTACHMENT_DUMP_FIELD(canVariableZoom);
		ATTACHMENT_DUMP_FIELD(hideRailWithThisScope);
		ATTACHMENT_DUMP_FIELD(forceIronSightDown);
		ATTACHMENT_DUMP_FIELD(useScopeDrift);
		ATTACHMENT_DUMP_FIELD(useDualFOV);
	}

	void dump_att(AttHoldBreath* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(canHoldBreath);
	}

	void dump_att(AttReload* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(noPartialReload);
		ATTACHMENT_DUMP_FIELD(segmentedReload);
		ATTACHMENT_DUMP_FIELD(reloadDisabled);
	}

	void dump_att(AttAddOns* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(motionTracker);
		ATTACHMENT_DUMP_FIELD(silenced);
		ATTACHMENT_DUMP_FIELD(riotShield);
	}

	void dump_att(AttGeneral* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(boltAction);
		ATTACHMENT_DUMP_FIELD(inheritsPerks);
		ATTACHMENT_DUMP_FIELD(reticleSpin45);
		ATTACHMENT_DUMP_FIELD(adsShouldShowCrosshair);
		ATTACHMENT_DUMP_FIELD(enemyCrosshairRange);
		ATTACHMENT_DUMP_FIELD(reticleType);
		ATTACHMENT_DUMP_ASSET(reticleCenter);
		ATTACHMENT_DUMP_ASSET(reticleSide);
		ATTACHMENT_DUMP_ASSET(reticleOnePiece);
		ATTACHMENT_DUMP_FIELD(reticleCenterSize);
		ATTACHMENT_DUMP_FIELD(reticleSideSize);
		ATTACHMENT_DUMP_STRING(szLUICrosshairWidget);
	}

	void dump_att(AttLaser* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_ASSET(laserTypeFriendly);
		ATTACHMENT_DUMP_ASSET(laserTypeEnemy);
		ATTACHMENT_DUMP_FIELD(forceLaserOn);
	}

	void dump_att(AttAimAssist* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(autoAimRange);
		ATTACHMENT_DUMP_FIELD(aimAssistRange);
		ATTACHMENT_DUMP_FIELD(aimAssistRangeAds);
		ATTACHMENT_DUMP_FIELD(aimAssistPitchSlowdown);
		ATTACHMENT_DUMP_FIELD(aimAssistPitchSlowdownAds);
		ATTACHMENT_DUMP_FIELD(aimAssistYawSlowdown);
		ATTACHMENT_DUMP_FIELD(aimAssistYawSlowdownAds);
		ATTACHMENT_DUMP_FIELD(aimAssistLockonStrength);
	}

	void dump_att(AttTargetAssist* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(targetAssistType);
		ATTACHMENT_DUMP_FIELD(targetAssistBehavior);
		ATTACHMENT_DUMP_FIELD(targetAssistRange);
		ATTACHMENT_DUMP_FIELD(targetAssistAngle);
		ATTACHMENT_DUMP_FIELD(targetAssistLosOffsetForward);
		ATTACHMENT_DUMP_FIELD(targetAssistLosOffsetRight);
		ATTACHMENT_DUMP_FIELD(targetAssistLosOffsetUp);
		ATTACHMENT_DUMP_FIELD(targetAssistOnlyAvailableInAds);
	}

	void dump_att(AttUnknown01* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_ASSET(material);
		ATTACHMENT_DUMP_FIELD(unk_0);
		ATTACHMENT_DUMP_FIELD(unk_1);
		ATTACHMENT_DUMP_FIELD(unk_2);
		ATTACHMENT_DUMP_FIELD(unk_3);
		ATTACHMENT_DUMP_FIELD(unk_4);
		ATTACHMENT_DUMP_FIELD(unk_5);
		ATTACHMENT_DUMP_FIELD(unk_6);
		ATTACHMENT_DUMP_FIELD(unk_7);
		ATTACHMENT_DUMP_FIELD(unk_8);
		ATTACHMENT_DUMP_FIELD(unk_9);
		ATTACHMENT_DUMP_FIELD(unk_10);
		ATTACHMENT_DUMP_FIELD(unk_11);
		ATTACHMENT_DUMP_FIELD(unk_12);
		ATTACHMENT_DUMP_FIELD(unk_13);
		ATTACHMENT_DUMP_FIELD(unk_14);
		ATTACHMENT_DUMP_FIELD(unk_15);
		ATTACHMENT_DUMP_FIELD(unk_16);
		ATTACHMENT_DUMP_FIELD(unk_17);
		ATTACHMENT_DUMP_FIELD(unk_18);
		ATTACHMENT_DUMP_FIELD(unk_19);
		ATTACHMENT_DUMP_FIELD(unk_20);
		ATTACHMENT_DUMP_FIELD(unk_21);
		ATTACHMENT_DUMP_FIELD(unk_22);
		ATTACHMENT_DUMP_FIELD(unk_23);
		ATTACHMENT_DUMP_FIELD(unk_24);
		ATTACHMENT_DUMP_FIELD(unk_25);
		ATTACHMENT_DUMP_FIELD(unk_26);
		ATTACHMENT_DUMP_FIELD(unk_27);
		ATTACHMENT_DUMP_FIELD(unk_28);
		ATTACHMENT_DUMP_FIELD(unk_29);
		ATTACHMENT_DUMP_FIELD(unk_30);
		ATTACHMENT_DUMP_FIELD(unk_31);
		ATTACHMENT_DUMP_FIELD(unk_32);
		ATTACHMENT_DUMP_FIELD(unk_33);
		ATTACHMENT_DUMP_FIELD(unk_34);
		ATTACHMENT_DUMP_FIELD(unk_35);
		ATTACHMENT_DUMP_FIELD(unk_36);
		ATTACHMENT_DUMP_FIELD(unk_37);
		ATTACHMENT_DUMP_FIELD(unk_38);
		ATTACHMENT_DUMP_FIELD(unk_39);
		ATTACHMENT_DUMP_FIELD(unk_40);
		ATTACHMENT_DUMP_FIELD(unk_41);
		ATTACHMENT_DUMP_FIELD(unk_42);
		ATTACHMENT_DUMP_FIELD(unk_43);
		ATTACHMENT_DUMP_FIELD(unk_44);
		ATTACHMENT_DUMP_FIELD(unk_45);
		ATTACHMENT_DUMP_FIELD(unk_46);
		ATTACHMENT_DUMP_FIELD(unk_47);
		ATTACHMENT_DUMP_FIELD(unk_48);
	}

	void dump_att(AttAmmunition* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(maxAmmo);
		ATTACHMENT_DUMP_FIELD(startAmmo);
		ATTACHMENT_DUMP_FIELD(clipSize);
		ATTACHMENT_DUMP_FIELD(shotCount);
		ATTACHMENT_DUMP_FIELD(reloadAmmoAdd);
		ATTACHMENT_DUMP_FIELD(reloadStartAdd);
		ATTACHMENT_DUMP_FIELD(requireAmmoUsedPerShot);
	}

	void dump_att(AttDamage* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(damage);
		ATTACHMENT_DUMP_FIELD(mid1Damage);
		ATTACHMENT_DUMP_FIELD(mid2Damage);
		ATTACHMENT_DUMP_FIELD(mid3Damage);
		ATTACHMENT_DUMP_FIELD(minDamage);
		ATTACHMENT_DUMP_FIELD(meleeDamage);
		ATTACHMENT_DUMP_FIELD(maxDamageRange);
		ATTACHMENT_DUMP_FIELD(mid1DamageRange);
		ATTACHMENT_DUMP_FIELD(mid2DamageRange);
		ATTACHMENT_DUMP_FIELD(mid3DamageRange);
		ATTACHMENT_DUMP_FIELD(minDamageRange);
		ATTACHMENT_DUMP_FIELD(playerDamage);
		ATTACHMENT_DUMP_FIELD(mid1PlayerDamage);
		ATTACHMENT_DUMP_FIELD(mid2PlayerDamage);
		ATTACHMENT_DUMP_FIELD(mid3PlayerDamage);
		ATTACHMENT_DUMP_FIELD(minPlayerDamage);
	}

	void dump_att(AttIdleSettings* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(hipIdleAmount);
		ATTACHMENT_DUMP_FIELD(hipIdleSpeed);
		ATTACHMENT_DUMP_FIELD(idleCrouchFactor);
		ATTACHMENT_DUMP_FIELD(idleProneFactor);
		ATTACHMENT_DUMP_FIELD(adsIdleLerpStartTime);
		ATTACHMENT_DUMP_FIELD(adsIdleLerpTime);
	}

	void dump_att(AttADSSettings* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(adsSpread);
		ATTACHMENT_DUMP_FIELD(adsAimPitch);
		ATTACHMENT_DUMP_FIELD(adsTransInTime);
		ATTACHMENT_DUMP_FIELD(adsTransOutTime);
		ATTACHMENT_DUMP_FIELD(adsReloadTransTime);
		ATTACHMENT_DUMP_FIELD(adsCrosshairInFrac);
		ATTACHMENT_DUMP_FIELD(adsCrosshairOutFrac);
		ATTACHMENT_DUMP_FIELD(adsInCrosshairAlphaStart);
		ATTACHMENT_DUMP_FIELD(adsInCrosshairAlphaEnd);
		ATTACHMENT_DUMP_FIELD(adsOutCrosshairAlphaStart);
		ATTACHMENT_DUMP_FIELD(adsOutCrosshairAlphaEnd);
		ATTACHMENT_DUMP_FIELD(adsZoomFov);
		ATTACHMENT_DUMP_FIELD(adsZoomInFrac);
		ATTACHMENT_DUMP_FIELD(adsZoomOutFrac);
		ATTACHMENT_DUMP_FIELD(adsFovLerpTime);
		ATTACHMENT_DUMP_FIELD(adsBobFactor);
		ATTACHMENT_DUMP_FIELD(adsViewBobMult);
		ATTACHMENT_DUMP_FIELD(adsFireRateScale);
		ATTACHMENT_DUMP_FIELD(adsDamageRangeScale);
		ATTACHMENT_DUMP_FIELD(adsFireAnimFrac);
		ATTACHMENT_DUMP_FIELD(fireTimerLerpToAdsScale);
		ATTACHMENT_DUMP_FIELD(alwaysFireAtMaxRangeInAds);
		ATTACHMENT_DUMP_FIELD(adsAlignEnabled);
		ATTACHMENT_DUMP_FIELD(disableTagAlignX);
	}

	void dump_att(AttScopeDriftSettings* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(fScopeDriftDelay);
		ATTACHMENT_DUMP_FIELD(fScopeDriftLerpInTime);
		ATTACHMENT_DUMP_FIELD(fScopeDriftSteadyTime);
		ATTACHMENT_DUMP_FIELD(fScopeDriftLerpOutTime);
		ATTACHMENT_DUMP_FIELD(fScopeDriftSteadyFactor);
		ATTACHMENT_DUMP_FIELD(fScopeDriftUnsteadyFactor);
	}

	void dump_att(AttHipSpread* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(hipSpreadStandMin);
		ATTACHMENT_DUMP_FIELD(hipSpreadDuckedMin);
		ATTACHMENT_DUMP_FIELD(hipSpreadProneMin);
		ATTACHMENT_DUMP_FIELD(hipSpreadSprintMin);
		ATTACHMENT_DUMP_FIELD(hipSpreadInAirMin);
		ATTACHMENT_DUMP_FIELD(hipSpreadMax);
		ATTACHMENT_DUMP_FIELD(hipSpreadDuckedMax);
		ATTACHMENT_DUMP_FIELD(hipSpreadProneMax);
		ATTACHMENT_DUMP_FIELD(hipSpreadSprintMax);
		ATTACHMENT_DUMP_FIELD(hipSpreadInAirMax);
		ATTACHMENT_DUMP_FIELD(hipSpreadFireAdd);
		ATTACHMENT_DUMP_FIELD(hipSpreadTurnAdd);
		ATTACHMENT_DUMP_FIELD(hipSpreadMoveAdd);
		ATTACHMENT_DUMP_FIELD(hipSpreadDecayRate);
		ATTACHMENT_DUMP_FIELD(hipSpreadDuckedDecay);
		ATTACHMENT_DUMP_FIELD(hipSpreadProneDecay);
		ATTACHMENT_DUMP_FIELD(hipSpreadSprintDecay);
		ATTACHMENT_DUMP_FIELD(hipSpreadInAirDecay);
	}

	void dump_att(AttSlideSpread* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(slideSpreadMin);
		ATTACHMENT_DUMP_FIELD(slideSpreadMax);
		ATTACHMENT_DUMP_FIELD(slideSpreadDecayRate);
		ATTACHMENT_DUMP_FIELD(slideSpreadFireAdd);
		ATTACHMENT_DUMP_FIELD(slideSpreadTurnAdd);
	}

	void dump_att(AttGunKick* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(hipGunKickReducedKickBullets);
		ATTACHMENT_DUMP_FIELD(hipGunKickReducedKickPercent);
		ATTACHMENT_DUMP_FIELD(hipGunKickPitchMin);
		ATTACHMENT_DUMP_FIELD(hipGunKickPitchMax);
		ATTACHMENT_DUMP_FIELD(hipGunKickYawMin);
		ATTACHMENT_DUMP_FIELD(hipGunKickYawMax);
		ATTACHMENT_DUMP_FIELD(hipGunKickMagMin);
		ATTACHMENT_DUMP_FIELD(hipGunKickAccel);
		ATTACHMENT_DUMP_FIELD(hipGunKickSpeedMax);
		ATTACHMENT_DUMP_FIELD(hipGunKickSpeedDecay);
		ATTACHMENT_DUMP_FIELD(hipGunKickStaticDecay);
		ATTACHMENT_DUMP_FIELD(adsGunKickReducedKickBullets);
		ATTACHMENT_DUMP_FIELD(adsGunKickReducedKickPercent);
		ATTACHMENT_DUMP_FIELD(adsGunKickPitchMin);
		ATTACHMENT_DUMP_FIELD(adsGunKickPitchMax);
		ATTACHMENT_DUMP_FIELD(adsGunKickYawMin);
		ATTACHMENT_DUMP_FIELD(adsGunKickYawMax);
		ATTACHMENT_DUMP_FIELD(adsGunKickMagMin);
		ATTACHMENT_DUMP_FIELD(adsGunKickAccel);
		ATTACHMENT_DUMP_FIELD(adsGunKickSpeedMax);
		ATTACHMENT_DUMP_FIELD(adsGunKickSpeedDecay);
		ATTACHMENT_DUMP_FIELD(adsGunKickStaticDecay);
	}

	void dump_att(AttViewKick* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(hipViewKickPitchMin);
		ATTACHMENT_DUMP_FIELD(hipViewKickPitchMax);
		ATTACHMENT_DUMP_FIELD(hipViewKickYawMin);
		ATTACHMENT_DUMP_FIELD(hipViewKickYawMax);
		ATTACHMENT_DUMP_FIELD(hipViewKickMagMin);
		ATTACHMENT_DUMP_FIELD(hipViewKickCenterSpeed);
		ATTACHMENT_DUMP_FIELD(adsViewKickPitchMin);
		ATTACHMENT_DUMP_FIELD(adsViewKickPitchMax);
		ATTACHMENT_DUMP_FIELD(adsViewKickYawMin);
		ATTACHMENT_DUMP_FIELD(adsViewKickYawMax);
		ATTACHMENT_DUMP_FIELD(adsViewKickMagMin);
		ATTACHMENT_DUMP_FIELD(adsViewKickCenterSpeed);
		ATTACHMENT_DUMP_FIELD(kickAlignedInputScalar);
		ATTACHMENT_DUMP_FIELD(kickOpposedInputScalar);
	}

	void dump_att(AttADSOverlay* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_ASSET(overlay.shader);
		ATTACHMENT_DUMP_ASSET(overlay.shaderLowRes);
		ATTACHMENT_DUMP_ASSET(overlay.shaderEMP);
		ATTACHMENT_DUMP_ASSET(overlay.shaderEMPLowRes);
		ATTACHMENT_DUMP_FIELD(overlay.reticle);
		ATTACHMENT_DUMP_FIELD(overlay.width);
		ATTACHMENT_DUMP_FIELD(overlay.height);
		ATTACHMENT_DUMP_FIELD(overlay.widthSplitscreen);
		ATTACHMENT_DUMP_FIELD(overlay.heightSplitscreen);
		ATTACHMENT_DUMP_FIELD(hybridToggle);
		ATTACHMENT_DUMP_FIELD(thermalScope);
		ATTACHMENT_DUMP_FIELD(thermalToggle);
		ATTACHMENT_DUMP_FIELD(outlineEnemies);
	}

	void dump_att(AttOutline* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD_ARR(outlineColor, 4);
		ATTACHMENT_DUMP_FIELD_ARR(outlineAltColor, 4);
		ATTACHMENT_DUMP_FIELD_ARR(outlineFillColor0, 4);
		ATTACHMENT_DUMP_FIELD_ARR(outlineFillColor1, 4);
		ATTACHMENT_DUMP_FIELD_ARR(outlineOccludedOutlineColor, 4);
		ATTACHMENT_DUMP_FIELD_ARR(outlineOccludedInlineColor, 4);
		ATTACHMENT_DUMP_FIELD_ARR(outlineOccludedInteriorColor, 4);
		ATTACHMENT_DUMP_FIELD(outlineWidth);
		ATTACHMENT_DUMP_FIELD(outlineEnemies);
		ATTACHMENT_DUMP_FIELD(outlineDepthTest);
		ATTACHMENT_DUMP_FIELD(outlineFill);
		ATTACHMENT_DUMP_FIELD(outlineScriptControlled);
		ATTACHMENT_DUMP_FIELD(outlineVehicles);
		ATTACHMENT_DUMP_FIELD(outlineTurrets);
		ATTACHMENT_DUMP_FIELD(outlineCharacters);
		ATTACHMENT_DUMP_FIELD(outlineAgents);
		ATTACHMENT_DUMP_FIELD(outlineScriptMovers);
		ATTACHMENT_DUMP_FIELD(outlineLockOn);
	}

	void dump_att(AttUI* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_ASSET(hudIcon);
		ATTACHMENT_DUMP_ASSET(pickupIcon);
		ATTACHMENT_DUMP_ASSET(dangerIcon);
		ATTACHMENT_DUMP_ASSET(throwBackIcon);
		ATTACHMENT_DUMP_FIELD(hideWarningIcons);
		ATTACHMENT_DUMP_FIELD(warningIconsDelay);
		ATTACHMENT_DUMP_ASSET(dpadIcon);
		ATTACHMENT_DUMP_ASSET(ammoCounterIcon);
		ATTACHMENT_DUMP_FIELD(ammoCounterClip);
		ATTACHMENT_DUMP_FIELD(grenadeDangerIconDistance);
	}

	void dump_att(AttRumbles* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_ASSET(fireRumble);
		ATTACHMENT_DUMP_ASSET(meleeImpactRumble);
	}

	void dump_att(AttProjectile* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(explosionRadius);
		ATTACHMENT_DUMP_FIELD(explosionForceRadius);
		ATTACHMENT_DUMP_FIELD(explosionInnerDamage);
		ATTACHMENT_DUMP_FIELD(explosionOuterDamage);
		ATTACHMENT_DUMP_FIELD(damageConeAngle);
		ATTACHMENT_DUMP_FIELD(projectileSpeed);
		ATTACHMENT_DUMP_FIELD(projectileSpeedUp);
		ATTACHMENT_DUMP_FIELD(projectileActivateDist);
		ATTACHMENT_DUMP_FIELD(projectileLifetime);
		ATTACHMENT_DUMP_FIELD(projectileLifetimeStdDeviation);
		ATTACHMENT_DUMP_ASSET(projectileModel);
		ATTACHMENT_DUMP_FIELD(projExplosionType);
		ATTACHMENT_DUMP_FIELD(projExplosionEffect.type);
		ATTACHMENT_DUMP_ASSET(projExplosionEffect.u.fx);
		ATTACHMENT_DUMP_FIELD(projExplosionEffectForceNormalUp);
		ATTACHMENT_DUMP_STRING(projExplosionSound);
		ATTACHMENT_DUMP_FIELD(projDudEffect.type);
		ATTACHMENT_DUMP_ASSET(projDudEffect.u.fx);
		ATTACHMENT_DUMP_STRING(projDudSound);
		ATTACHMENT_DUMP_FIELD(projImpactExplode);
		ATTACHMENT_DUMP_FIELD(disableProjectileCrumpleCheck);
		ATTACHMENT_DUMP_FIELD(destabilizationRateTime);
		ATTACHMENT_DUMP_FIELD(destabilizationCurvatureMax);
		ATTACHMENT_DUMP_FIELD(destabilizeDistance);
		ATTACHMENT_DUMP_FIELD(projBodyEffect.type);
		ATTACHMENT_DUMP_ASSET(projBodyEffect.u.fx);
		ATTACHMENT_DUMP_FIELD(projTrailEffect.type);
		ATTACHMENT_DUMP_ASSET(projTrailEffect.u.fx);
		ATTACHMENT_DUMP_FIELD(projBeaconEffect.type);
		ATTACHMENT_DUMP_ASSET(projBeaconEffect.u.fx);
		ATTACHMENT_DUMP_FIELD(projIgnitionDelay);
		ATTACHMENT_DUMP_FIELD(projIgnitionEffect.type);
		ATTACHMENT_DUMP_ASSET(projIgnitionEffect.u.fx);
		ATTACHMENT_DUMP_STRING(projIgnitionSound);
		ATTACHMENT_DUMP_FIELD(stickiness);
		ATTACHMENT_DUMP_FIELD(fuseTime);
		ATTACHMENT_DUMP_FIELD(aiFuseTime);
		ATTACHMENT_DUMP_FIELD(stickToPlayers);
		ATTACHMENT_DUMP_FIELD(stickToVehicles);
		ATTACHMENT_DUMP_FIELD(stickToTurrets);
		ATTACHMENT_DUMP_FIELD(stickToNonStick);
		ATTACHMENT_DUMP_FIELD(timedDetonation);
		ATTACHMENT_DUMP_FIELD(isPredictedProjectile);
		ATTACHMENT_DUMP_FIELD(useSteppedExplosionDamage);
		ATTACHMENT_DUMP_FIELD(reactiveMotionRadiusScale);
		ATTACHMENT_DUMP_FIELD(reactiveMotionFrequencyScale);
		ATTACHMENT_DUMP_FIELD(reactiveMotionAmplitudeScale);
		ATTACHMENT_DUMP_FIELD(reactiveMotionFalloff);
		ATTACHMENT_DUMP_FIELD(reactiveMotionLifetime);
	}

	void dump_att(AttCharged* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(chargeType);
		ATTACHMENT_DUMP_FIELD(chargeGain);
		ATTACHMENT_DUMP_FIELD(chargeCostPerShot);
		ATTACHMENT_DUMP_FIELD(chargeLossWhenIdle);
		ATTACHMENT_DUMP_FIELD(chargeEmptyCooldown);
		ATTACHMENT_DUMP_FIELD(chargeFireAtMaxDamageMultiplier);
		ATTACHMENT_DUMP_FIELD(chargeMeterEffectBoneCount);
		ATTACHMENT_DUMP_FIELD(chargeMeterEffect.type);
		ATTACHMENT_DUMP_ASSET(chargeMeterEffect.u.fx);
		ATTACHMENT_DUMP_STRING(chargeUpSound);
		ATTACHMENT_DUMP_STRING(chargeDownSound);
		ATTACHMENT_DUMP_STRING(chargeDownToUpSound);
		ATTACHMENT_DUMP_STRING(chargeUpToDownSound);
		ATTACHMENT_DUMP_STRING(chargeMaxSound);
		ATTACHMENT_DUMP_STRING(chargeUpSoundPlayer);
		ATTACHMENT_DUMP_STRING(chargeDownSoundPlayer);
		ATTACHMENT_DUMP_STRING(chargeDownToUpSoundPlayer);
		ATTACHMENT_DUMP_STRING(chargeUpToDownSoundPlayer);
		ATTACHMENT_DUMP_STRING(chargeMaxSoundPlayer);
		ATTACHMENT_DUMP_FIELD(chargeHudReveal);
		ATTACHMENT_DUMP_ASSET(chargeRumble);
	}

	void dump_att(AdsAltSwitch* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(fovInterpTimeSecMainToAlt);
		ATTACHMENT_DUMP_FIELD(fovInterpDelaySecMainToAlt);
		ATTACHMENT_DUMP_FIELD(dualFovSwitchRateMainToAlt);
		ATTACHMENT_DUMP_FIELD(fovInterpTypeMainToAlt);
		ATTACHMENT_DUMP_FIELD(fovInterpTimeSecAltToMain);
		ATTACHMENT_DUMP_FIELD(fovInterpDelaySecAltToMain);
		ATTACHMENT_DUMP_FIELD(dualFovSwitchRateAltToMain);
		ATTACHMENT_DUMP_FIELD(fovInterpTypeAltToMain);
	}

	void dump_att(AttRegeneration* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(regenerationEnabled);
		ATTACHMENT_DUMP_FIELD(regenerationTimeMs);
		ATTACHMENT_DUMP_FIELD(regenerationAddTimeMs);
		ATTACHMENT_DUMP_FIELD(regenerationAmount);
		ATTACHMENT_DUMP_FIELD(regenerationConsumeStock);
		ATTACHMENT_DUMP_FIELD(regenerationDisableWhileFiring);
	}

	void dump_att(AttMovement* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(moveSpeedScale);
		ATTACHMENT_DUMP_FIELD(adsMoveSpeedScale);
		ATTACHMENT_DUMP_FIELD(fastADS3PAnims);
	}

	void dump_att(AttBurst* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(burstFireCooldown);
		ATTACHMENT_DUMP_FIELD(burstFireAuto);
	}

#define DUMP_ATT(__att__) \
	if (asset->__att__) dump_att(asset->__att__, data[#__att__]); else data[#__att__] = nullptr;

	void weapon_attachment::dump(WeaponAttachment* asset)
	{
		const auto path = "attachments\\"s + asset->name + ".json"s;

		ordered_json data;

		ATTACHMENT_DUMP_STRING(szInternalName);
		ATTACHMENT_DUMP_STRING(szDisplayName);
		//ATTACHMENT_DUMP_SCRIPT_STRING(internalName); // parse this from szInternalName
		ATTACHMENT_DUMP_FIELD(papAttachment);
		ATTACHMENT_DUMP_FIELD(type); // get_attachment_type_str();
		ATTACHMENT_DUMP_FIELD(weaponType); //data["weaponType"] = get_weap_type_str(asset->weaponType);
		ATTACHMENT_DUMP_FIELD_RENAME(weapClass, "weaponClass"); //data["weaponClass"] = get_weap_class_str(asset->weapClass);
		ATTACHMENT_DUMP_ASSET(worldModelCamo);
		ATTACHMENT_DUMP_ASSET(viewModelCamo);
		ATTACHMENT_DUMP_ASSET_ARR(worldModelVariations, asset->numModelVariations);
		ATTACHMENT_DUMP_ASSET_ARR(viewModelVariations, asset->numModelVariations);
		ATTACHMENT_DUMP_ASSET_ARR(worldModelCamoMaterials, asset->numWorldModelCamoMaterials);
		ATTACHMENT_DUMP_ASSET_ARR(viewModelCamoMaterials, asset->numViewModelCamoMaterials);
		ATTACHMENT_DUMP_SCRIPT_STRING(attachPoint);
		ATTACHMENT_DUMP_ASSET_ARR(reticles, asset->numReticles);
		ATTACHMENT_DUMP_ASSET_ARR(altReticles, asset->numAltReticles);
		DUMP_ATT(ammogeneral);
		DUMP_ATT(tracer);
		DUMP_ATT(penetration);
		DUMP_ATT(sight);
		DUMP_ATT(holdBreath);
		DUMP_ATT(reload);
		DUMP_ATT(addOns);
		DUMP_ATT(laser);
		DUMP_ATT(general);
		DUMP_ATT(aimAssist);
		DUMP_ATT(targetAssist);
		DUMP_ATT(unkAtt01);
		DUMP_ATT(ammunition);
		DUMP_ATT(damage);
		DUMP_ATT(idleSettings);
		DUMP_ATT(adsSettings);
		DUMP_ATT(adsSettingsMain);
		DUMP_ATT(scopeDriftSettings);
		DUMP_ATT(scopeDriftSettingsMain);
		DUMP_ATT(hipSpread);
		DUMP_ATT(slideSpread);
		DUMP_ATT(gunKick);
		DUMP_ATT(viewKick);
		DUMP_ATT(adsOverlay);
		DUMP_ATT(outline);
		DUMP_ATT(ui);
		DUMP_ATT(rumbles);
		DUMP_ATT(projectile);
		DUMP_ATT(charged);
		DUMP_ATT(adsAltSwitch);
		DUMP_ATT(regeneration);
		DUMP_ATT(movement);
		DUMP_ATT(burst);
		ATTACHMENT_DUMP_FIELD_ALLOC_ARR(locationDamage, 22);
		ATTACHMENT_DUMP_FIELD(ammunitionScale);
		ATTACHMENT_DUMP_FIELD(damageScale);
		ATTACHMENT_DUMP_FIELD(stateTimersScale);
		ATTACHMENT_DUMP_FIELD(reloadTimersScale);
		ATTACHMENT_DUMP_FIELD(fireTimeOverride);
		ATTACHMENT_DUMP_FIELD(fireTimeAkimboOverride);
		ATTACHMENT_DUMP_FIELD(fireTimersScale);
		ATTACHMENT_DUMP_FIELD(burstFireCooldownScale);
		ATTACHMENT_DUMP_FIELD(idleHipMotionScale);
		ATTACHMENT_DUMP_FIELD(idleADSMotionScale);
		ATTACHMENT_DUMP_FIELD(idleADSMotionTimeScale);
		ATTACHMENT_DUMP_FIELD(adsSettingsScale);
		ATTACHMENT_DUMP_FIELD(adsSettingsScaleMain);
		ATTACHMENT_DUMP_FIELD(hipSpreadScale);
		ATTACHMENT_DUMP_FIELD(slideSpreadScale);
		ATTACHMENT_DUMP_FIELD(gunKickScale);
		ATTACHMENT_DUMP_FIELD(viewKickScale);
		ATTACHMENT_DUMP_FIELD(viewCenterScale);
		ATTACHMENT_DUMP_FIELD(adsSwayScale);
		ATTACHMENT_DUMP_FIELD(adsSwayScale2);
		ATTACHMENT_DUMP_FIELD(adsSwayScale3);
		ATTACHMENT_DUMP_FIELD(iAmmoIndex); // probably runtime data
		ATTACHMENT_DUMP_FIELD(loadIndex); // ^
		ATTACHMENT_DUMP_FIELD(useAlternateViewModelLaser);
		ATTACHMENT_DUMP_FIELD(hideIronSightsWithThisAttachment);
		ATTACHMENT_DUMP_FIELD(shareAmmoWithAlt);
		ATTACHMENT_DUMP_FIELD(useLeftTriggerAltFire);
		ATTACHMENT_DUMP_FIELD(containsIKTag);
		ATTACHMENT_DUMP_FIELD(isAkimboAttachment);
		ATTACHMENT_DUMP_FIELD(attachLeftWeaponAkimbo);
		ATTACHMENT_DUMP_FIELD(attachRightWeaponAkimbo);
		ATTACHMENT_DUMP_FIELD(luiWeaponInfoWidgetUsesScopeStencil);
		ATTACHMENT_DUMP_STRING(szAltModeName);
		ATTACHMENT_DUMP_STRING(szLUIWeaponInfoWidgetName);
		ATTACHMENT_DUMP_STRING(szLUIWeaponInfoWidgetTag);

		std::string json = data.dump(4);

		auto file = filesystem::file(path);
		file.open("wb");
		file.write(json.data(), json.size(), 1);
		file.close();
	}
}
