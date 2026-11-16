#include <std_include.hpp>

#include "../include.hpp"
#include "weapon.hpp"
#include "animpackage.hpp"
#include "sound.hpp"
#include "tracer.hpp"
#include "xmodel.hpp"

#include <iw8/assets/weapon.hpp>

#define COPY_VALUE(__name__) \
		static_assert(sizeof(new_asset->__name__) == sizeof(asset->__name__)); \
		new_asset->__name__ = asset->__name__;

#define COPY_ENUM(__name__) \
		static_assert(sizeof(new_asset->__name__) == sizeof(asset->__name__)); \
		new_asset->__name__ = static_cast<decltype(new_asset->__name__)>(asset->__name__);

#define COPY_ARR(__name__) \
		static_assert(sizeof(new_asset->__name__) == sizeof(asset->__name__)); \
		std::memcpy(&new_asset->__name__, &asset->__name__, sizeof(new_asset->__name__));

#define COPY_SCRIPT_STRINGS(__name__) \
		new_asset->__name__ = reinterpret_cast<target::scr_string_t*>(asset->__name__);

namespace zonetool::iw7::converter::iw8::weapon
{
	namespace
	{
		namespace target = zonetool::iw8;

		template <typename T>
		T* find_loaded(const XAssetType type, T* asset)
		{
			if (!asset)
			{
				return nullptr;
			}

			const auto loaded = static_cast<T*>(db_find_asset(type, asset->name));
			return loaded ? loaded : asset;
		}

		target::XModel* convert_model(XModel* asset, utils::memory::allocator& allocator)
		{
			if (!asset)
			{
				return nullptr;
			}

			if (const auto loaded = find_loaded(ASSET_TYPE_XMODEL, asset); loaded->numsurfs)
			{
				xmodel::dump(loaded);
			}

			const auto model = allocator.allocate<target::XModel>();
			model->name = asset->name;
			return model;
		}

		bool is_zero_gravity_bone(const XModel* model, unsigned int bone, const scr_string_t zero_gravity_tag, const scr_string_t zero_gravity_tag2)
		{
			while (bone >= model->numRootBones)
			{
				bone -= model->parentList[bone - model->numRootBones];
				if (model->boneNames[bone] == zero_gravity_tag || model->boneNames[bone] == zero_gravity_tag2)
				{
					return true;
				}
			}

			return false;
		}

		target::scr_string_t* convert_hide_tags(const WeaponCompleteDef* asset, utils::memory::allocator& allocator)
		{
			const auto hide_tags = allocator.allocate_array<target::scr_string_t>(32);
			if (asset->hideTags)
			{
				std::memcpy(hide_tags, asset->hideTags, sizeof(target::scr_string_t) * 32);
			}

			const auto model = asset->weapDef ? find_loaded(ASSET_TYPE_XMODEL, asset->weapDef->gunXModel) : nullptr;
			if (!model || !model->boneNames || !model->parentList)
			{
				return hide_tags;
			}

			const auto zero_gravity_tag = static_cast<scr_string_t>(SL_GetString("tag_zerog_off"));
			const auto zero_gravity_tag2 = static_cast<scr_string_t>(SL_GetString("tag_zerog_off2"));
			auto slot = 0u;
			for (auto bone = 0u; bone < model->numBones; bone++)
			{
				if (!is_zero_gravity_bone(model, bone, zero_gravity_tag, zero_gravity_tag2))
				{
					continue;
				}

				while (slot < 32 && hide_tags[slot])
				{
					slot++;
				}

				if (slot == 32)
				{
					ZONETOOL_WARNING("Weapon \"%s\" has no free hide tag for bone \"%s\"", asset->szInternalName, SL_ConvertToString(model->boneNames[bone]));
					break;
				}

				hide_tags[slot] = model->boneNames[bone];
			}

			return hide_tags;
		}

		target::WeaponAnimPackage* convert_anim_package(WeaponAnimPackage* asset, utils::memory::allocator& allocator)
		{
			if (!asset)
			{
				return nullptr;
			}

			animpackage::dump(find_loaded(ASSET_TYPE_ANIM_PACKAGE, asset));

			const auto package = allocator.allocate<target::WeaponAnimPackage>();
			package->name = asset->name;
			return package;
		}

		target::TracerDef* convert_tracer(TracerDef* asset, utils::memory::allocator& allocator)
		{
			if (!asset)
			{
				return nullptr;
			}

			tracer::dump(find_loaded(ASSET_TYPE_TRACER, asset));

			const auto result = allocator.allocate<target::TracerDef>();
			result->name = asset->name;
			return result;
		}

		target::WeaponSFXPackage* convert_sfx_package(WeaponCompleteDef* asset, utils::memory::allocator& allocator)
		{
			if (!asset->weapDef->sfxPackage)
			{
				return nullptr;
			}

			sound::dump(asset);

			const auto result = allocator.allocate<target::WeaponSFXPackage>();
			result->name = asset->weapDef->sfxPackage->name;
			return result;
		}

		target::weapType_t convert_weap_type(const weapType_t type)
		{
			switch (type)
			{
			case WEAPTYPE_BULLET: return target::WEAPTYPE_BULLET;
			case WEAPTYPE_GRENADE: return target::WEAPTYPE_GRENADE;
			case WEAPTYPE_PROJECTILE: return target::WEAPTYPE_PROJECTILE;
			case WEAPTYPE_RIOTSHIELD: return target::WEAPTYPE_RIOTSHIELD;
			case WEAPTYPE_SCRIPT: return target::WEAPTYPE_SCRIPT;
			case WEAPTYPE_SHIELD: return target::WEAPTYPE_SHIELD;
			case WEAPTYPE_CHARGE_SHIELD: return target::WEAPTYPE_CHARGE_SHIELD;
			case WEAPTYPE_EQUIP_DEPLOY: return target::WEAPTYPE_EQUIP_DEPLOY;
			default: return target::WEAPTYPE_NONE;
			}
		}

		target::ImpactType convert_impact_type(const ImpactType type, const char* weapon)
		{
			switch (type)
			{
			case IMPACT_TYPE_NONE: return target::IMPACT_TYPE_NONE;
			case IMPACT_TYPE_BULLET_SMALL:
			case IMPACT_TYPE_BULLET_SMALL_ENERGY: return target::IMPACT_TYPE_BULLET_SMALL;
			case IMPACT_TYPE_BULLET_LARGE:
			case IMPACT_TYPE_BULLET_LARGE_ENERGY: return target::IMPACT_TYPE_BULLET_MEDIUM;
			case IMPACT_TYPE_SHOTGUN:
			case IMPACT_TYPE_SHOTGUN_ENERGY:
			case IMPACT_TYPE_SHOTGUN_SONIC: return target::IMPACT_TYPE_SHOTGUN;
			default:
				ZONETOOL_WARNING("Weapon \"%s\" has IW7 impact type %d, which has no IW8 equivalent", weapon, type);
				return target::IMPACT_TYPE_NONE;
			}
		}

		target::weapFireType_t convert_fire_type(const weapFireType_t type)
		{
			switch (type)
			{
			case WEAPON_FIRETYPE_SINGLESHOT: return target::WEAPON_FIRETYPE_SINGLESHOT;
			case WEAPON_FIRETYPE_BURST: return target::WEAPON_FIRETYPE_BURST;
			case WEAPON_FIRETYPE_DOUBLEBARREL: return target::WEAPON_FIRETYPE_DOUBLEBARREL;
			case WEAPON_FIRETYPE_BEAM: return target::WEAPON_FIRETYPE_BEAM;
			default: return target::WEAPON_FIRETYPE_FULLAUTO;
			}
		}

		target::OffhandClass convert_offhand_class(const OffhandClass type)
		{
			switch (type)
			{
			case OFFHAND_CLASS_FRAG_GRENADE: return target::OFFHAND_CLASS_FRAG_GRENADE;
			case OFFHAND_CLASS_SMOKE_GRENADE: return target::OFFHAND_CLASS_SMOKE_GRENADE;
			case OFFHAND_CLASS_FLASH_GRENADE: return target::OFFHAND_CLASS_FLASH_GRENADE;
			case OFFHAND_CLASS_THROWINGKNIFE: return target::OFFHAND_CLASS_THROWINGKNIFE;
			case OFFHAND_CLASS_SHIELD: return target::OFFHAND_CLASS_SHIELD;
			case OFFHAND_CLASS_OTHER: return target::OFFHAND_CLASS_OTHER;
			default: return target::OFFHAND_CLASS_NONE;
			}
		}

		target::weapProjExposion_t convert_proj_explosion(const weapProjExposion_t type)
		{
			switch (type)
			{
			case WEAPPROJEXP_GRENADE: return target::WEAPPROJEXP_GRENADE;
			case WEAPPROJEXP_ROCKET: return target::WEAPPROJEXP_ROCKET;
			case WEAPPROJEXP_FLASHBANG: return target::WEAPPROJEXP_FLASHBANG;
			case WEAPPROJEXP_DUD: return target::WEAPPROJEXP_DUD;
			case WEAPPROJEXP_SMOKE: return target::WEAPPROJEXP_SMOKE;
			case WEAPPROJEXP_HEAVY: return target::WEAPPROJEXP_HEAVY;
			default: return target::WEAPPROJEXP_NONE;
			}
		}

		void convert_damage(const WeaponDef* asset, target::WeaponDef* new_asset)
		{
			for (auto& data : new_asset->damageInfo.damageData)
			{
				data.damage = asset->damage;
				data.mid1Damage = asset->mid1Damage;
				data.mid2Damage = asset->mid2Damage;
				data.mid3Damage = asset->mid3Damage;
				data.minDamage = asset->minDamage;
				data.maxDamageRange = asset->maxDamageRange;
				data.mid1DamageRange = asset->mid1DamageRange;
				data.mid2DamageRange = asset->mid2DamageRange;
				data.mid3DamageRange = asset->mid3DamageRange;
				data.minDamageRange = asset->minDamageRange;
			}

			new_asset->damageInfo.deathAnimDamageType = static_cast<target::AnimDamageType>(asset->deathAnimDamageType);
			new_asset->iMeleeDamage = asset->meleeDamage;
		}

		void convert_sway(const WeaponDef* asset, target::WeaponDef* new_asset)
		{
			auto& hip = new_asset->swaySettings.hip;
			hip.common.maxAngle = asset->swayMaxAngle;
			hip.common.lerpSpeed = asset->swayLerpSpeed;
			hip.common.pitchScale = asset->swayPitchScale;
			hip.common.yawScale = asset->swayYawScale;
			hip.common.horizScale = asset->swayHorizScale;
			hip.common.vertScale = asset->swayVertScale;
			hip.maxAngleSteadyAim = asset->swayMaxAngleSteadyAim;

			auto& ads = new_asset->swaySettings.ads;
			ads.common.maxAngle = asset->adsSwayMaxAngle;
			ads.common.lerpSpeed = asset->adsSwayLerpSpeed;
			ads.common.pitchScale = asset->adsSwayPitchScale;
			ads.common.yawScale = asset->adsSwayYawScale;
			ads.common.horizScale = asset->adsSwayHorizScale;
			ads.common.vertScale = asset->adsSwayVertScale;
			ads.swayTransitionLerpSpeed = asset->adsSwayLerpSpeed;
			std::ranges::fill(ads.adsSwayScale, 1.0f);

			new_asset->swaySettings.shellShockScale = asset->swayShellShockScale;
		}

		void set_neutral_scales(const WeaponDef* asset, target::WeaponDef* new_asset)
		{
			new_asset->sprintOutTimerScale = 1.0f;
			new_asset->fReticleRapidFireMultiplier = 1.0f;
			new_asset->adsCrouchViewKickFactor = 1.0f;
			new_asset->adsCrouchGunKickFactor = 1.0f;
			new_asset->adsProneViewKickFactor = 1.0f;
			new_asset->adsProneGunKickFactor = 1.0f;
			new_asset->adsMovementAnimLerpEnd = 1.0f;
			new_asset->adsSpreadEndFrac = 1.0f;
			new_asset->hipStartingGunKickPercent = 1.0f;
			new_asset->hipStartingViewKickPercent = 1.0f;
			new_asset->adsStartingGunKickPercent = 1.0f;
			new_asset->adsStartingViewKickPercent = 1.0f;
			new_asset->hipEndingGunKickPercent = 1.0f;
			new_asset->hipEndingViewKickPercent = 1.0f;
			new_asset->adsEndingGunKickPercent = 1.0f;
			new_asset->adsEndingViewKickPercent = 1.0f;
			new_asset->mountViewmodelOffset = 1.0f;
			new_asset->mountFOVScale = 1.0f;

			new_asset->hipSpreadStandMoveMax = asset->hipSpreadStandMax;
			new_asset->hipSpreadDuckedMoveMax = asset->hipSpreadDuckedMax;
			new_asset->hipSpreadProneMoveMax = asset->hipSpreadProneMax;
			new_asset->hipSpreadSprintMoveMax = asset->hipSpreadSprintMax;
			new_asset->hipSpreadInAirMoveMax = asset->hipSpreadInAirMax;
			new_asset->slideSpreadMoveMax = asset->slideSpreadMax;
		}

		target::WeaponDef* convert_def(WeaponDef* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<target::WeaponDef>();

			COPY_VALUE(szOverlayName);
			COPY_VALUE(iFireTime);
			COPY_VALUE(iFireDelay);
			COPY_VALUE(iFireTimeAkimbo);
			COPY_VALUE(iFireDelayAkimbo);
			COPY_VALUE(szModeName);
			COPY_VALUE(burstCount);
			COPY_VALUE(targetAssistRange);
			COPY_VALUE(targetAssistAngle);
			COPY_VALUE(targetAssistLosOffsetForward);
			COPY_VALUE(targetAssistLosOffsetRight);
			COPY_VALUE(targetAssistLosOffsetUp);
			COPY_VALUE(targetAssistOnlyAvailableInAds);
			COPY_VALUE(burstFireCooldown);
			COPY_VALUE(burstFireAuto);
			COPY_VALUE(iReticleCenterSize);
			COPY_VALUE(iReticleSideSize);
			COPY_VALUE(iReticleMinOfs);
			COPY_VALUE(reticleCenterPulse);
			COPY_VALUE(fPosMoveRate);
			COPY_VALUE(fPosProneMoveRate);
			COPY_VALUE(fStandMoveMinSpeed);
			COPY_VALUE(fDuckedMoveMinSpeed);
			COPY_VALUE(fProneMoveMinSpeed);
			COPY_VALUE(fPosRotRate);
			COPY_VALUE(fPosProneRotRate);
			COPY_VALUE(fStandRotMinSpeed);
			COPY_VALUE(fDuckedRotMinSpeed);
			COPY_VALUE(fProneRotMinSpeed);
			COPY_VALUE(hideWarningIcons);
			COPY_VALUE(warningIconsDelay);
			COPY_VALUE(iStartAmmo);
			COPY_VALUE(iPerkStartAmmo);
			COPY_VALUE(iPerkMaxAmmo);
			COPY_VALUE(szAmmoName);
			COPY_VALUE(iAmmoIndex);
			COPY_VALUE(iMaxAmmo);
			COPY_VALUE(shotCount);
			COPY_VALUE(szSharedAmmoCapName);
			COPY_VALUE(iSharedAmmoCapIndex);
			COPY_VALUE(iSharedAmmoCap);
			COPY_VALUE(iAmmoUsedPerShot);
			COPY_VALUE(requireAmmoUsedPerShot);
			COPY_VALUE(disableNoAmmoWarning);
			COPY_VALUE(lowAmmoWarningThreshold);
			COPY_VALUE(iDamageType);
			COPY_VALUE(autoAimRange);
			COPY_VALUE(aimAssistRange);
			COPY_VALUE(aimAssistRangeAds);
			COPY_VALUE(aimAssistPitchSlowdown);
			COPY_VALUE(aimAssistPitchSlowdownAds);
			COPY_VALUE(aimAssistYawSlowdown);
			COPY_VALUE(aimAssistYawSlowdownAds);
			COPY_VALUE(aimAssistLockonStrength);
			COPY_VALUE(aimPadding);
			COPY_VALUE(enemyCrosshairRange);
			COPY_VALUE(moveSpeedScale);
			COPY_VALUE(adsMoveSpeedScale);
			COPY_VALUE(sprintDurationScale);
			COPY_VALUE(sprintRestoreDelay);
			COPY_VALUE(fAdsBobFactor);
			COPY_VALUE(fAdsViewBobMult);
			COPY_VALUE(fHipSpreadStandMin);
			COPY_VALUE(fHipSpreadDuckedMin);
			COPY_VALUE(fHipSpreadProneMin);
			COPY_VALUE(hipSpreadSprintMin);
			COPY_VALUE(hipSpreadInAirMin);
			COPY_VALUE(hipSpreadStandMax);
			COPY_VALUE(hipSpreadDuckedMax);
			COPY_VALUE(hipSpreadProneMax);
			COPY_VALUE(hipSpreadSprintMax);
			COPY_VALUE(hipSpreadInAirMax);
			COPY_VALUE(fHipSpreadDecayRate);
			COPY_VALUE(fHipSpreadFireAdd);
			COPY_VALUE(fHipSpreadTurnAdd);
			COPY_VALUE(fHipSpreadMoveAdd);
			COPY_VALUE(fHipSpreadDuckedDecay);
			COPY_VALUE(fHipSpreadProneDecay);
			COPY_VALUE(hipSpreadSprintDecay);
			COPY_VALUE(hipSpreadInAirDecay);
			COPY_VALUE(fHipReticleSidePos);
			COPY_VALUE(fAdsIdleAmount);
			COPY_VALUE(fHipIdleAmount);
			COPY_VALUE(adsIdleSpeed);
			COPY_VALUE(hipIdleSpeed);
			COPY_VALUE(fIdleCrouchFactor);
			COPY_VALUE(fIdleProneFactor);
			COPY_VALUE(fGunMaxPitch);
			COPY_VALUE(fGunMaxYaw);
			new_asset->fViewMaxPitch = 10.0f; // BG_KickAngles clamp (0x140749E20)
			new_asset->fViewMaxYaw = 10.0f;
			COPY_VALUE(adsIdleLerpStartTime);
			COPY_VALUE(adsIdleLerpTime);
			COPY_VALUE(slideSpreadMin);
			COPY_VALUE(slideSpreadMax);
			COPY_VALUE(slideSpreadDecayRate);
			COPY_VALUE(slideSpreadFireAdd);
			COPY_VALUE(slideSpreadTurnAdd);
			COPY_VALUE(adsFireRateScale);
			COPY_VALUE(adsDamageRangeScale);
			COPY_VALUE(adsFireAnimFrac);
			COPY_VALUE(fireTimerLerpToAdsScale);
			COPY_VALUE(alwaysFireAtMaxRangeInAds);
			COPY_VALUE(dualWieldViewModelOffset);
			COPY_VALUE(fScopeDriftDelay);
			COPY_VALUE(fScopeDriftLerpInTime);
			COPY_VALUE(fScopeDriftSteadyTime);
			COPY_VALUE(fScopeDriftLerpOutTime);
			COPY_VALUE(fScopeDriftSteadyFactor);
			COPY_VALUE(fScopeDriftUnsteadyFactor);
			COPY_VALUE(iReloadAmmoAdd);
			COPY_VALUE(iReloadStartAdd);
			COPY_VALUE(ammoDropStockMin);
			COPY_VALUE(ammoDropClipPercentMin);
			COPY_VALUE(ammoDropClipPercentMax);
			COPY_VALUE(iExplosionRadius);
			COPY_VALUE(iExplosionRadiusMin);
			COPY_VALUE(iExplosionForceRadius);
			COPY_VALUE(iExplosionInnerDamage);
			COPY_VALUE(iExplosionOuterDamage);
			COPY_VALUE(damageConeAngle);
			COPY_VALUE(bulletExplDmgMult);
			COPY_VALUE(bulletExplRadiusMult);
			COPY_VALUE(iProjectileSpeed);
			COPY_VALUE(iProjectileSpeedUp);
			COPY_VALUE(iProjectileSpeedForward);
			COPY_VALUE(iProjectileActivateDist);
			COPY_VALUE(iProjectileDetonationRadius);
			COPY_VALUE(projLifetime);
			COPY_VALUE(projLifetimeStdDeviation);
			COPY_VALUE(timeToAccelerate);
			COPY_VALUE(projectileCurvature);
			COPY_VALUE(parallelBounce);
			COPY_VALUE(perpendicularBounce);
			COPY_VALUE(maxSteeringAccel);
			COPY_VALUE(projIgnitionDelay);
			COPY_VALUE(fAdsAimPitch);
			COPY_VALUE(adsInCrosshairAlphaStart);
			COPY_VALUE(adsInCrosshairAlphaEnd);
			COPY_VALUE(adsOutCrosshairAlphaStart);
			COPY_VALUE(adsOutCrosshairAlphaEnd);
			COPY_VALUE(adsShouldShowCrosshair);
			COPY_VALUE(fAdsGunKickPitchMin);
			COPY_VALUE(fAdsGunKickPitchMax);
			COPY_VALUE(fAdsGunKickYawMin);
			COPY_VALUE(fAdsGunKickYawMax);
			COPY_VALUE(fAdsGunKickMagMin);
			COPY_VALUE(fAdsGunKickAccel);
			COPY_VALUE(fAdsGunKickSpeedMax);
			COPY_VALUE(fAdsGunKickSpeedDecay);
			COPY_VALUE(fAdsGunKickStaticDecay);
			COPY_VALUE(fAdsViewKickPitchMin);
			COPY_VALUE(fAdsViewKickPitchMax);
			COPY_VALUE(fAdsViewKickYawMin);
			COPY_VALUE(fAdsViewKickYawMax);
			COPY_VALUE(fAdsViewKickMagMin);
			COPY_VALUE(fAdsViewScatterMin);
			COPY_VALUE(fAdsViewScatterMax);
			COPY_VALUE(fAdsSpread);
			COPY_VALUE(iVisibilityAxis);
			COPY_VALUE(fVisibilityUpOffset);
			COPY_VALUE(fHipGunKickPitchMin);
			COPY_VALUE(fHipGunKickPitchMax);
			COPY_VALUE(fHipGunKickYawMin);
			COPY_VALUE(fHipGunKickYawMax);
			COPY_VALUE(fHipGunKickMagMin);
			COPY_VALUE(fHipGunKickAccel);
			COPY_VALUE(fHipGunKickSpeedMax);
			COPY_VALUE(fHipGunKickSpeedDecay);
			COPY_VALUE(fHipGunKickStaticDecay);
			COPY_VALUE(fHipViewKickPitchMin);
			COPY_VALUE(fHipViewKickPitchMax);
			COPY_VALUE(fHipViewKickYawMin);
			COPY_VALUE(fHipViewKickYawMax);
			COPY_VALUE(fHipViewKickMagMin);
			COPY_VALUE(fHipViewScatterMin);
			COPY_VALUE(fHipViewScatterMax);
			COPY_VALUE(multipleReloadClipPercentage);
			COPY_VALUE(kickAlignedInputScalar);
			COPY_VALUE(kickOpposedInputScalar);
			COPY_VALUE(fightDist);
			COPY_VALUE(maxDist);
			COPY_VALUE(iPositionReloadTransTime);
			COPY_VALUE(leftArc);
			COPY_VALUE(rightArc);
			COPY_VALUE(topArc);
			COPY_VALUE(bottomArc);
			COPY_VALUE(softLeftRightArc);
			COPY_VALUE(accuracy);
			COPY_VALUE(aiSpread);
			COPY_VALUE(playerSpread);
			COPY_VALUE(pitchConvergenceTime);
			COPY_VALUE(yawConvergenceTime);
			COPY_VALUE(suppressTime);
			COPY_VALUE(maxRange);
			COPY_VALUE(fAnimHorRotateInc);
			COPY_VALUE(fPlayerPositionDist);
			COPY_VALUE(szUseHintString);
			COPY_VALUE(dropHintString);
			COPY_VALUE(iUseHintStringIndex);
			COPY_VALUE(dropHintStringIndex);
			COPY_VALUE(horizViewJitter);
			COPY_VALUE(vertViewJitter);
			COPY_VALUE(scanSpeed);
			COPY_VALUE(scanAccel);
			COPY_VALUE(scanPauseTime);
			COPY_VALUE(szScript);
			COPY_VALUE(destabilizationRateTime);
			COPY_VALUE(destabilizationCurvatureMax);
			COPY_VALUE(destabilizeDistance);
			COPY_VALUE(robotechMaxPitch);
			COPY_VALUE(robotechMaxYaw);
			COPY_VALUE(robotechFrequency);
			COPY_VALUE(robotechVariancePitch);
			COPY_VALUE(robotechVarianceYaw);
			COPY_VALUE(robotechVarianceFrequency);
			COPY_VALUE(locationDamageMultipliers);
			COPY_VALUE(turretADSEnabled);
			COPY_VALUE(turretADSTime);
			COPY_VALUE(turretFov);
			COPY_VALUE(turretFovADS);
			COPY_VALUE(turretScopeZoomRate);
			COPY_VALUE(turretScopeZoomMin);
			COPY_VALUE(turretScopeZoomMax);
			COPY_VALUE(turretOverheatPenalty);
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
			COPY_VALUE(meleeOverrideValues);
			COPY_VALUE(aim_automelee_lerp);
			COPY_VALUE(aim_automelee_region_height);
			COPY_VALUE(aim_automelee_region_width);
			COPY_VALUE(aim_automelee_maxPitchMovement);
			COPY_VALUE(aim_automelee_maxYawMovement);
			COPY_VALUE(player_meleeHeight);
			COPY_VALUE(player_meleeWidth);
			COPY_VALUE(playerMeleeRangeStanding);
			COPY_VALUE(playerMeleeRangeCrouched);
			COPY_VALUE(playerMeleeRangeProne);
			COPY_VALUE(playerMeleeRangeChargeStanding);
			COPY_VALUE(playerMeleeRangeChargeCrouched);
			COPY_VALUE(playerMeleeChargeHeightTolerance);
			COPY_VALUE(shieldAllowFiring);
			COPY_VALUE(shieldAlwaysDisplay);
			COPY_VALUE(shieldMeleeFxDelay);
			COPY_VALUE(HitEarthquakeScale);
			COPY_VALUE(HitEarthquakeDuration);
			COPY_VALUE(HitEarthquakeRadius);
			COPY_VALUE(MissEarthquakeScale);
			COPY_VALUE(MissEarthquakeDuration);
			COPY_VALUE(MissEarthquakeRadius);
			COPY_VALUE(shieldUsesEnergy);
			COPY_VALUE(shieldMaxEnergy);
			COPY_VALUE(shieldConsumptionRate);
			COPY_VALUE(shieldMeleeEnergyCost);
			COPY_VALUE(shieldMeleeHitEnergyCost);
			COPY_VALUE(reactiveMotionRadiusScale);
			COPY_VALUE(reactiveMotionFrequencyScale);
			COPY_VALUE(reactiveMotionAmplitudeScale);
			COPY_VALUE(reactiveMotionFalloff);
			COPY_VALUE(reactiveMotionLifetime);
			COPY_VALUE(hasAnyTransientModels);
			COPY_VALUE(sharedAmmo);
			COPY_VALUE(lockonSupported);
			COPY_VALUE(requireLockonToFire);
			COPY_VALUE(isAirburstWeapon);
			COPY_VALUE(bigExplosion);
			COPY_VALUE(noAdsWhenMagEmpty);
			COPY_VALUE(avoidDropCleanup);
			COPY_VALUE(allowGrenadeSwitching);
			COPY_VALUE(inheritsPerks);
			COPY_VALUE(crosshairColorChange);
			COPY_VALUE(bRifleBullet);
			COPY_VALUE(bEnergyBullet);
			COPY_VALUE(armorPiercing);
			COPY_VALUE(impaling);
			COPY_VALUE(bBoltAction);
			COPY_VALUE(aimDownSight);
			COPY_VALUE(canHoldBreath);
			COPY_VALUE(meleeOnly);
			COPY_VALUE(supportsAlternateMelee);
			COPY_VALUE(bRechamberWhileAds);
			COPY_VALUE(bBulletExplosiveDamage);
			COPY_VALUE(bCookOffHold);
			COPY_VALUE(reticleSpin45);
			COPY_VALUE(bClipOnly);
			COPY_VALUE(bDoesNotConsumeAmmo);
			COPY_VALUE(bRemoveWeaponOnEmpty);
			COPY_VALUE(noAmmoPickup);
			COPY_VALUE(adsFireOnly);
			COPY_VALUE(cancelAutoHolsterWhenEmpty);
			COPY_VALUE(disableSwitchToWhenEmpty);
			COPY_VALUE(suppressAmmoPrimaryDisplay);
			COPY_VALUE(suppressAmmoReserveDisplay);
			COPY_VALUE(markableViewmodel);
			COPY_VALUE(flipKillIcon);
			COPY_VALUE(bNoPartialReload);
			COPY_VALUE(reloadDisabled);
			COPY_VALUE(blocksProne);
			COPY_VALUE(silenced);
			COPY_VALUE(isRollingGrenade);
			COPY_VALUE(dropGrenadeHeldOnDeath);
			COPY_VALUE(projExplosionEffectForceNormalUp);
			COPY_VALUE(projExplosionEffectInheritParentDirection);
			COPY_VALUE(bProjImpactExplode);
			COPY_VALUE(disableProjectileCrumpleCheck);
			COPY_VALUE(bProjTrajectoryEvents);
			COPY_VALUE(bProjWhizByEnabled);
			COPY_VALUE(stickToPlayers);
			COPY_VALUE(stickToVehicles);
			COPY_VALUE(stickToTurrets);
			COPY_VALUE(stickToNonStick);
			COPY_VALUE(projEnableMissileStickiness);
			COPY_VALUE(thrownSideways);
			COPY_VALUE(disableFiring);
			COPY_VALUE(firesOnWeaponSwitch);
			COPY_VALUE(disableHolding);
			COPY_VALUE(timedDetonation);
			COPY_VALUE(explosiveDamageDelay);
			COPY_VALUE(fuseTime);
			COPY_VALUE(aiFuseTime);
			COPY_VALUE(maxHoldTime);
			COPY_VALUE(holdButtonToThrow);
			COPY_VALUE(infiniteHold);
			COPY_VALUE(freezeMovementWhenFiring);
			COPY_VALUE(offhandAllowsSprint);
			COPY_VALUE(thermalScope);
			COPY_VALUE(thermalToggle);
			COPY_VALUE(outlineEnemies);
			COPY_VALUE(outlineDepthTest);
			COPY_VALUE(outlineFill);
			COPY_VALUE(enemyOutlineR);
			COPY_VALUE(enemyOutlineG);
			COPY_VALUE(enemyOutlineB);
			COPY_VALUE(enemyOutlineA);
			COPY_VALUE(allyOutlineR);
			COPY_VALUE(allyOutlineG);
			COPY_VALUE(allyOutlineB);
			COPY_VALUE(allyOutlineA);
			COPY_VALUE(depthScan);
			COPY_VALUE(depthScanThickness);
			COPY_VALUE(depthScanR);
			COPY_VALUE(depthScanG);
			COPY_VALUE(depthScanB);
			COPY_VALUE(depthScanA);
			COPY_VALUE(depthScanOutlineThickness);
			COPY_VALUE(depthScanOutlineR);
			COPY_VALUE(depthScanOutlineG);
			COPY_VALUE(depthScanOutlineB);
			COPY_VALUE(depthScanOutlineA);
			COPY_VALUE(depthScanOverlay);
			COPY_VALUE(depthScanOverlayStrength);
			COPY_VALUE(depthScanOverlayXTiles);
			COPY_VALUE(depthScanOverlayYTiles);
			COPY_VALUE(depthScanOverlayXScroll);
			COPY_VALUE(depthScanOverlayYScroll);
			COPY_VALUE(blurSceneAdsInFraction);
			COPY_VALUE(blurSceneAdsOutFraction);
			COPY_VALUE(altModeSameWeapon);
			COPY_VALUE(turretBarrelSpinEnabled);
			COPY_VALUE(missileConeSoundEnabled);
			COPY_VALUE(missileConeSoundPitchshiftEnabled);
			COPY_VALUE(missileConeSoundCrossfadeEnabled);
			COPY_VALUE(offhandHoldIsCancelable);
			COPY_VALUE(doNotAllowAttachmentsToOverrideSpread);
			COPY_VALUE(useScopeDrift);
			COPY_VALUE(alwaysShatterGlassOnImpact);
			COPY_VALUE(jumpSpread);
			COPY_VALUE(noFullViewmodelAnimations);
			COPY_VALUE(killcamOffset);
			COPY_VALUE(useDualFOV);
			COPY_VALUE(disableDrop);
			COPY_VALUE(preferredDrop);
			COPY_VALUE(gestureFireStateTime);
			COPY_VALUE(minPullbackTime);
			COPY_VALUE(gestureFireTime);
			COPY_VALUE(gestureDetonationTime);
			COPY_VALUE(gesturesDisablePrimary);
			COPY_VALUE(cameraFireEffectDurationSec);
			COPY_VALUE(changedFireTime);
			COPY_VALUE(changedFireTimeAkimbo);
			COPY_VALUE(changedFireTimeNumBullets);
			COPY_VALUE(chargeGain);
			COPY_VALUE(chargeCostPerShot);
			COPY_VALUE(chargeLossWhenIdle);
			COPY_VALUE(chargeEmptyCooldown);
			COPY_VALUE(chargeFireAtMaxDamageMultiplier);
			COPY_VALUE(chargeMeterEffectBoneCount);
			COPY_VALUE(chargeHudReveal);
			COPY_VALUE(stowTag);
			COPY_VALUE(maxTargetAngle);
			COPY_VALUE(spaceshipSecondaryWeapon);
			COPY_VALUE(impulseFieldRadius);
			COPY_VALUE(impulseFieldInitialSpeed);
			COPY_VALUE(impulseFieldMaxSpeed);
			COPY_VALUE(impulseFieldAcceleration);
			COPY_VALUE(impulseFieldInAirImpulseMultiplier);
			COPY_VALUE(impulseFieldInAirImpulseMultiplierInterpTime);
			COPY_VALUE(impulseFieldSlideMultiplier);
			COPY_VALUE(impulseFieldSlideMultiplierInterpTime);
			COPY_VALUE(impulseFieldIsPush);
			COPY_VALUE(impulseFieldAffectsFriendlies);
			COPY_VALUE(impulseFieldAffectsSelf);
			COPY_VALUE(impulseFieldAffectsProne);
			COPY_VALUE(regenerationEnabled);
			COPY_VALUE(regenerationTimeMs);
			COPY_VALUE(regenerationAddTimeMs);
			COPY_VALUE(regenerationAmount);
			COPY_VALUE(regenerationConsumeStock);
			COPY_VALUE(regenerationDisableWhileFiring);
			COPY_VALUE(deployRequireOnWalkableSurface);
			COPY_VALUE(deployRequireOnNavmesh);
			COPY_VALUE(deployRequireSkyAbove);
			COPY_VALUE(deployRequireNoOverhang);
			COPY_VALUE(deployAlwaysUpright);
			COPY_VALUE(deployEdgeSnap);
			COPY_VALUE(deployCylinderRadius);
			COPY_VALUE(deployCylinderHeight);
			COPY_VALUE(deployMaxDistance);
			COPY_VALUE(reactiveEmitterDelay);
			COPY_VALUE(grenadeDangerIconDistance);

			COPY_ARR(vStandMove);
			COPY_ARR(vStandRot);
			COPY_ARR(strafeMove);
			COPY_ARR(strafeRot);
			COPY_ARR(vDuckedOfs);
			COPY_ARR(vDuckedMove);
			COPY_ARR(vDuckedRot);
			COPY_ARR(vProneOfs);
			COPY_ARR(vProneMove);
			COPY_ARR(vProneRot);
			COPY_ARR(vProjectileColor);
			COPY_ARR(accuracyGraphName);
			COPY_ARR(originalAccuracyGraphKnots);
			COPY_ARR(minTurnSpeed);
			COPY_ARR(maxTurnSpeed);
			COPY_ARR(adsSpeedMs);
			COPY_ARR(rotationParams);

			COPY_ENUM(scriptedAnimEvent);
			COPY_ENUM(weapClass);
			COPY_ENUM(penetrateType);
			COPY_ENUM(inventoryType);
			COPY_ENUM(targetAssistType);
			COPY_ENUM(targetAssistBehavior);
			COPY_ENUM(stance);
			COPY_ENUM(reticleType);
			COPY_ENUM(activeReticleType);
			COPY_ENUM(ammoCounterClip);
			COPY_ENUM(overlayInterface);
			COPY_ENUM(killIconRatio);
			COPY_ENUM(guidedMissileType);
			COPY_ENUM(tracerStyle);
			COPY_ENUM(shieldMaxSpeed);
			COPY_ENUM(dualWieldType);
			COPY_ENUM(slot);

			COPY_SCRIPT_STRINGS(notetrackRumbleMapKeys);
			COPY_SCRIPT_STRINGS(notetrackRumbleMapValues);
			COPY_SCRIPT_STRINGS(notetrackFXMapKeys);
			COPY_SCRIPT_STRINGS(notetrackFXMapTagValues);

			new_asset->weapType = convert_weap_type(asset->weapType);
			new_asset->fireType = convert_fire_type(asset->fireType);
			new_asset->offhandClass = convert_offhand_class(asset->offhandClass);
			new_asset->projExplosion = convert_proj_explosion(asset->projExplosion);
			new_asset->shieldDeployButton = static_cast<target::offhandShieldButton_t>(asset->shieldDeployButton);
			new_asset->chargeType = static_cast<unsigned int>(asset->chargeType);
			new_asset->deployValidGroupId = static_cast<unsigned int>(asset->deployValidGroupId);
			new_asset->deployIndoorGroupId = static_cast<unsigned int>(asset->deployIndoorGroupId);
			new_asset->deployInvalidGroupId = static_cast<unsigned int>(asset->deployInvalidGroupId);
			new_asset->deployOutOfRangeGroupId = static_cast<unsigned int>(asset->deployOutOfRangeGroupId);
			new_asset->hitLocPriorityMap = reinterpret_cast<char*>(asset->hitLocPriorityMap);
			std::memcpy(new_asset->unittypeMultipliers, asset->unittypeMultipliers, sizeof(new_asset->unittypeMultipliers));

			new_asset->gunXModel = convert_model(asset->gunXModel, allocator);
			new_asset->gunXModelLeftHand = convert_model(asset->gunXModelLeftHand, allocator);
			new_asset->gunXModelRightHand = convert_model(asset->gunXModelRightHand, allocator);
			new_asset->defaultViewModel = convert_model(asset->defaultViewModel, allocator);
			new_asset->defaultWorldModelLeftHand = convert_model(asset->defaultWorldModelLeftHand, allocator);
			new_asset->defaultWorldModelRightHand = convert_model(asset->defaultWorldModelRightHand, allocator);
			new_asset->worldModel = convert_model(asset->worldModel, allocator);
			new_asset->worldXModelLeftHand = convert_model(asset->worldXModelLeftHand, allocator);
			new_asset->worldXModelRightHand = convert_model(asset->worldXModelRightHand, allocator);
			new_asset->defaultWorldModel = convert_model(asset->defaultWorldModel, allocator);
			new_asset->playerShadowModel = convert_model(asset->playerShadowModel, allocator);
			new_asset->playerShadowModelLeftHand = convert_model(asset->playerShadowModelLeftHand, allocator);
			new_asset->playerShadowModelRightHand = convert_model(asset->playerShadowModelRightHand, allocator);
			new_asset->handXModel = convert_model(asset->handXModel, allocator);
			new_asset->worldClipModel = convert_model(asset->worldClipModel, allocator);
			new_asset->rocketModel = convert_model(asset->rocketModel, allocator);
			new_asset->projectileModel = convert_model(asset->projectileModel, allocator);
			new_asset->stowOffsetModel = convert_model(asset->stowOffsetModel, allocator);

			new_asset->tracerType = convert_tracer(asset->tracerType, allocator);

			new_asset->szXAnims = convert_anim_package(asset->szXAnims, allocator);
			new_asset->szXAnimsRightHanded = convert_anim_package(asset->szXAnimsRightHanded, allocator);
			new_asset->szXAnimsLeftHanded = convert_anim_package(asset->szXAnimsLeftHanded, allocator);

			convert_damage(asset, new_asset);
			convert_sway(asset, new_asset);
			set_neutral_scales(asset, new_asset);
			return new_asset;
		}

		void convert_zoom(const WeaponCompleteDef* asset, target::WeaponCompleteDef* new_asset)
		{
			auto& scene = new_asset->zoomSettings.scene;
			scene.adsZoomCount = 1;
			scene.adsZoomFov[0] = asset->fAdsZoomFov;
			scene.adsReloadFov[0] = asset->fAdsZoomFov;
			scene.adsZoomInFrac = asset->weapDef->fAdsZoomInFrac;
			scene.adsZoomOutFrac = asset->weapDef->fAdsZoomOutFrac;
			scene.adsFOVNVG = asset->fAdsZoomFov;
			scene.adsReloadFovNVG = asset->fAdsZoomFov;

			auto& weapon = new_asset->zoomSettings.weapon;
			weapon.adsZoomFov = -1.0f;
			weapon.adsReloadZoomFov = -1.0f;
			weapon.adsNVGZoomFov = -1.0f;
			weapon.adsNVGReloadZoomFov = -1.0f;
			weapon.adsZoomInFrac = 1.0f;
			weapon.adsZoomOutFrac = 1.0f;
		}

		void convert_ads_dof(target::WeaponCompleteDef* new_asset)
		{
			new_asset->adsDofPhysicalFocalTag = true;
			new_asset->adsDofPhysicalReloadFstop = 19.5f;
			new_asset->adsDofPhysicalReloadFocusDistance = 60.0f;

			switch (new_asset->weapDef->weapClass)
			{
			case target::WEAPCLASS_PISTOL:
				new_asset->adsDofPhysicalFstop = 10.0f;
				new_asset->adsDofPhysicalFocusDistance = 21.0f;
				break;
			case target::WEAPCLASS_RIFLE:
				new_asset->adsDofPhysicalFstop = 30.0f;
				new_asset->adsDofPhysicalFocusDistance = 15.0f;
				break;
			case target::WEAPCLASS_SNIPER:
			case target::WEAPCLASS_MG:
			case target::WEAPCLASS_SMG:
			case target::WEAPCLASS_SPREAD:
				new_asset->adsDofPhysicalFstop = 30.0f;
				new_asset->adsDofPhysicalFocusDistance = 21.0f;
				break;
			default:
				new_asset->adsDofPhysicalFstop = 19.5f;
				new_asset->adsDofPhysicalFocusDistance = 60.0f;
				break;
			}
		}

		const char* get_crosshair_widget(const target::WeaponDef* def)
		{
			switch (def->weapClass)
			{
			case target::WEAPCLASS_PISTOL:
				return "ReticleCrosshairSingleSmall";
			case target::WEAPCLASS_SPREAD:
				return "ReticleCrosshairSpread";
			case target::WEAPCLASS_SNIPER:
				return "ReticleCrosshairSingle";
			case target::WEAPCLASS_RIFLE:
			case target::WEAPCLASS_MG:
			case target::WEAPCLASS_SMG:
				switch (def->fireType)
				{
				case target::WEAPON_FIRETYPE_BURST:
					return "ReticleCrosshairBurst";
				case target::WEAPON_FIRETYPE_SINGLESHOT:
					return "ReticleCrosshairSingle";
				default:
					return "ReticleCrosshairAuto";
				}
			default:
				return nullptr;
			}
		}

		target::WeaponCompleteDef* convert(WeaponCompleteDef* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<target::WeaponCompleteDef>();
			COPY_VALUE(szInternalName);
			COPY_VALUE(szDisplayName);
			COPY_VALUE(szLootTable);
			new_asset->hideTags = convert_hide_tags(asset, allocator);
			COPY_VALUE(iAdsTransInTime);
			COPY_VALUE(iAdsTransOutTime);
			COPY_VALUE(iClipSize);
			new_asset->vfxImpactType = convert_impact_type(asset->vfxImpactType, asset->szInternalName);
			COPY_VALUE(penetrateMultiplier);
			COPY_VALUE(fAdsViewKickCenterSpeed);
			COPY_VALUE(fHipViewKickCenterSpeed);
			COPY_VALUE(ammoDropStockMax);
			COPY_ARR(accuracyGraphKnotCount);
			COPY_ARR(accuracyGraphKnots);
			COPY_VALUE(enhanced);
			COPY_VALUE(dpadIconShowsAmmo);
			COPY_VALUE(luiWeaponInfoWidgetUsesScopeStencil);
			COPY_VALUE(numNotetrackSoundMappings);
			COPY_SCRIPT_STRINGS(notetrackSoundMapKeys);
			COPY_SCRIPT_STRINGS(notetrackSoundMapValues);
			COPY_VALUE(numNotetrackOverrides);
			COPY_VALUE(numNotetrackSuitEntries);
			static_assert(sizeof(NoteTrackToSoundEntry) == sizeof(target::NoteTrackToSoundEntry));
			static_assert(sizeof(NoteTrackToSuitSoundEntry) == sizeof(target::NoteTrackToSuitSoundEntry));
			new_asset->notetrackOverrides = reinterpret_cast<target::NoteTrackToSoundEntry*>(asset->notetrackOverrides);
			new_asset->notetrackSuitEntries = reinterpret_cast<target::NoteTrackToSuitSoundEntry*>(asset->notetrackSuitEntries);
			new_asset->iAdsTransInToOutTime = asset->iAdsTransOutTime;
			new_asset->iAdsTransOutToInTime = asset->iAdsTransInTime;
			new_asset->weapDef = convert_def(asset->weapDef, allocator);
			new_asset->weapDef->sfxPackage = convert_sfx_package(asset, allocator);
			convert_zoom(asset, new_asset);
			convert_ads_dof(new_asset);
			new_asset->szLUICrosshairWidget = get_crosshair_widget(new_asset->weapDef);
			return new_asset;
		}
	}

	void dump(WeaponCompleteDef* asset)
	{
		if (!asset || !asset->weapDef || !mark_dumped(ASSET_TYPE_WEAPON, asset->name))
		{
			return;
		}

		utils::memory::allocator allocator;
		target::weapon::dump(convert(asset, allocator));
		add_csv_row("weapon", asset->name);
	}
}
