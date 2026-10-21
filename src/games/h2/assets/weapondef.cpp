#include "std_include.hpp"
#include "weapondef.hpp"

#include "io/json_reflect.hpp"

namespace zonetool::h2
{
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

	namespace
	{
		constexpr auto overlay_fields = glz::tuple
		{
			&ADSOverlay::shader,
			&ADSOverlay::shaderLowRes,
			&ADSOverlay::shaderEMP,
			&ADSOverlay::shaderEMPLowRes,
			&ADSOverlay::reticle,
			&ADSOverlay::xU_01,
			&ADSOverlay::xU_02,
			&ADSOverlay::width,
			&ADSOverlay::height,
			&ADSOverlay::widthSplitscreen,
			&ADSOverlay::heightSplitscreen,
		};

		constexpr auto turret_hydraulic_fields = glz::tuple
		{
			&TurretHydraulicSettings::horizontalSound,
			&TurretHydraulicSettings::horizontalStopSound,
			&TurretHydraulicSettings::maxVelocity,
			&TurretHydraulicSettings::minVelocity,
			&TurretHydraulicSettings::verticalSound,
			&TurretHydraulicSettings::verticalStopSound,
		};

		constexpr auto anim_override_fields = glz::tuple
		{
			&AnimOverrideEntry::altmodeAnim,
			&AnimOverrideEntry::overrideAnim,
			&AnimOverrideEntry::attachment1,
			&AnimOverrideEntry::attachment2,
			&AnimOverrideEntry::altTime,
			&AnimOverrideEntry::animTime,
			&AnimOverrideEntry::animTreeType,
			&AnimOverrideEntry::animHand,
		};

		constexpr auto sound_override_fields = glz::tuple
		{
			&SoundOverrideEntry::altmodeSound,
			&SoundOverrideEntry::attachment1,
			&SoundOverrideEntry::attachment2,
			&SoundOverrideEntry::overrideSound,
			&SoundOverrideEntry::soundType,
		};

		constexpr auto fx_override_fields = glz::tuple
		{
			&FXOverrideEntry::altmodeFX,
			&FXOverrideEntry::attachment1,
			&FXOverrideEntry::attachment2,
			&FXOverrideEntry::fxType,
			&FXOverrideEntry::overrideFX,
		};

		constexpr auto reload_override_fields = glz::tuple
		{
			&ReloadStateTimerEntry::attachment,
			&ReloadStateTimerEntry::reloadAddTime,
			&ReloadStateTimerEntry::reloadEmptyAddTime,
			&ReloadStateTimerEntry::reloadStartAddTime,
		};

		constexpr auto weapon_sound_fields = glz::tuple
		{
			&WeaponDef::pickupSound,
			&WeaponDef::pickupSoundPlayer,
			&WeaponDef::ammoPickupSound,
			&WeaponDef::ammoPickupSoundPlayer,
			&WeaponDef::projectileSound,
			&WeaponDef::pullbackSound,
			&WeaponDef::pullbackSoundPlayer,
			&WeaponDef::pullbackSoundQuick,
			&WeaponDef::pullbackSoundQuickPlayer,
			&WeaponDef::fireSound,
			&WeaponDef::fireSoundPlayer,
			&WeaponDef::fireSoundPlayerLeft,
			&WeaponDef::fireSoundPlayerRight,
			&WeaponDef::sound14,
			&WeaponDef::sound15,
			&WeaponDef::sound16,
			&WeaponDef::sound17,
			&WeaponDef::sound18,
			&WeaponDef::fireLoopSound,
			&WeaponDef::fireLoopSoundPlayer,
			&WeaponDef::sound21,
			&WeaponDef::sound22,
			&WeaponDef::sound23,
			&WeaponDef::sound24,
			&WeaponDef::sound25,
			&WeaponDef::sound26,
			&WeaponDef::fireStopSound,
			&WeaponDef::fireStopSoundPlayer,
			&WeaponDef::sound29,
			&WeaponDef::sound30,
			&WeaponDef::sound31,
			&WeaponDef::sound32,
			&WeaponDef::sound33,
			&WeaponDef::fireFirstSound,
			&WeaponDef::fireFirstSoundPlayer,
			&WeaponDef::fireSound2,
			&WeaponDef::fireSoundPlayer2,
			&WeaponDef::fireSpecialSound,
			&WeaponDef::fireSpecialSoundPlayer,
			&WeaponDef::emptyFireSound,
			&WeaponDef::emptyFireSoundPlayer,
			&WeaponDef::emptyFireSoundPlayerLeft,
			&WeaponDef::emptyFireSoundPlayerRight,
			&WeaponDef::emptyFireSoundReleasePlayer,
			&WeaponDef::emptyFireSoundReleasePlayerLeft,
			&WeaponDef::emptyFireSoundReleasePlayerRight,
			&WeaponDef::sound47,
			&WeaponDef::meleeSwipeSound,
			&WeaponDef::meleeSwipeSoundPlayer,
			&WeaponDef::meleeHitSound,
			&WeaponDef::meleeHitSoundPlayer,
			&WeaponDef::meleeMissSound,
			&WeaponDef::meleeMissSoundPlayer,
			&WeaponDef::sound54,
			&WeaponDef::sound55,
			&WeaponDef::sound56,
			&WeaponDef::sound57,
			&WeaponDef::sound58,
			&WeaponDef::sound59,
			&WeaponDef::sound60,
			&WeaponDef::sound61,
			&WeaponDef::sound62,
			&WeaponDef::sound63,
			&WeaponDef::sound64,
			&WeaponDef::sound65,
			&WeaponDef::nightVisionWearSound,
			&WeaponDef::nightVisionWearSoundPlayer,
			&WeaponDef::nightVisionRemoveSound,
			&WeaponDef::nightVisionRemoveSoundPlayer,
			&WeaponDef::raiseSound,
			&WeaponDef::raiseSoundPlayer,
			&WeaponDef::raiseSoundPlayerLeft,
			&WeaponDef::raiseSoundPlayerRight,
			&WeaponDef::sound74,
			&WeaponDef::quickRaiseSoundPlayer,
			&WeaponDef::quickRaiseSoundPlayerLeft,
			&WeaponDef::quickRaiseSoundPlayerRight,
			&WeaponDef::raiseSound2,
			&WeaponDef::sound79,
			&WeaponDef::sound80,
			&WeaponDef::sound81,
			&WeaponDef::putawaySound,
			&WeaponDef::putawaySoundPlayer,
			&WeaponDef::putawaySoundPlayerLeft,
			&WeaponDef::putawaySoundPlayerRight,
			&WeaponDef::sound86,
			&WeaponDef::sound87,
			&WeaponDef::adsEnterSoundPlayer,
			&WeaponDef::adsLeaveSoundPlayer,
			&WeaponDef::adsCrosshairEnemySound,
		};

		constexpr auto weapon_fields = glz::tuple
		{
			&WeaponDef::szInternalName,
			&WeaponDef::szDisplayName,
			&WeaponDef::szOverlayName,
			&WeaponDef::szAttachmentName,
			&WeaponDef::szAltWeaponName,
			json_reflect::counted(&WeaponDef::gunModel, 2),
			json_reflect::counted(&WeaponDef::worldModel, 2),
			json_reflect::counted(&WeaponDef::reticleViewModels, 64),
			&WeaponDef::handModel,
			&WeaponDef::persistentArmXModel,
			&WeaponDef::worldClipModel,
			&WeaponDef::rocketModel,
			&WeaponDef::knifeModel,
			&WeaponDef::worldKnifeModel,
			json_reflect::manual("szXAnimsRightHanded"),
			json_reflect::manual("szXAnimsLeftHanded"),
			json_reflect::manual("szXAnims"),
			json_reflect::manual("hideTags"),
			json_reflect::counted(&WeaponDef::attachments, &WeaponDef::numAttachments),
			json_reflect::counted(&WeaponDef::animOverrides, &WeaponDef::numAnimOverrides, anim_override_fields),
			json_reflect::counted(&WeaponDef::soundOverrides, &WeaponDef::numSoundOverrides, sound_override_fields),
			json_reflect::counted(&WeaponDef::fxOverrides, &WeaponDef::numFXOverrides, fx_override_fields),
			json_reflect::counted(&WeaponDef::reloadOverrides, &WeaponDef::numReloadStateTimerOverrides, reload_override_fields),
			json_reflect::manual("notetrackOverrides"),
			json_reflect::manual("notetrackSoundMap"),
			json_reflect::manual("notetrackRumbleMap"),
			json_reflect::manual("notetrackFXMap"),
			json_reflect::manual("notetrackHideTag"),
			&WeaponDef::lobWorldModelName,
			&WeaponDef::szAdsrBaseSetting,
			&WeaponDef::szAmmoName,
			&WeaponDef::szClipName,
			&WeaponDef::szSharedAmmoCapName,
			&WeaponDef::viewFlashEffect,
			&WeaponDef::viewBodyFlashEffect,
			&WeaponDef::worldFlashEffect,
			&WeaponDef::viewFlashADSEffect,
			&WeaponDef::viewBodyFlashADSEffect,
			&WeaponDef::signatureViewFlashEffect,
			&WeaponDef::signatureViewBodyFlashEffect,
			&WeaponDef::signatureWorldFlashEffect,
			&WeaponDef::signatureViewFlashADSEffect,
			&WeaponDef::signatureViewBodyFlashADSEffect,
			&WeaponDef::meleeHitEffect,
			&WeaponDef::meleeMissEffect,
			json_reflect::group("sounds", weapon_sound_fields),
			json_reflect::counted(&WeaponDef::bounceSound, 53),
			json_reflect::counted(&WeaponDef::rollingSound, 53),
			&WeaponDef::viewShellEjectEffect,
			&WeaponDef::worldShellEjectEffect,
			&WeaponDef::viewLastShotEjectEffect,
			&WeaponDef::worldLastShotEjectEffect,
			&WeaponDef::viewMagEjectEffect,
			&WeaponDef::reticleCenter,
			&WeaponDef::reticleSide,
			&WeaponDef::hudIcon,
			&WeaponDef::pickupIcon,
			&WeaponDef::minimapIconFriendly,
			&WeaponDef::minimapIconEnemy,
			&WeaponDef::minimapIconNeutral,
			&WeaponDef::ammoCounterIcon,
			&WeaponDef::physCollmap,
			&WeaponDef::physPreset,
			&WeaponDef::szUseHintString,
			&WeaponDef::dropHintString,
			json_reflect::counted(&WeaponDef::locationDamageMultipliers, 22),
			&WeaponDef::fireRumble,
			&WeaponDef::fireMedRumble,
			&WeaponDef::fireHighRumble,
			&WeaponDef::meleeImpactRumble,
			&WeaponDef::tracerType,
			&WeaponDef::signatureTracerType,
			&WeaponDef::laserType,
			&WeaponDef::turretOverheatSound,
			&WeaponDef::turretOverheatEffect,
			&WeaponDef::turretBarrelSpinRumble,
			&WeaponDef::turretBarrelSpinMaxSnd,
			&WeaponDef::turretBarrelSpinUpSnd,
			&WeaponDef::turretBarrelSpinDownSnd,
			&WeaponDef::missileConeSoundAlias,
			&WeaponDef::missileConeSoundAliasAtBase,
			&WeaponDef::stowOffsetModel,
			&WeaponDef::killIcon,
			&WeaponDef::dpadIcon,
			&WeaponDef::hudProximityWarningIcon,
			&WeaponDef::projectileName,
			&WeaponDef::projectileModel,
			&WeaponDef::projExplosionEffect,
			&WeaponDef::projDudEffect,
			&WeaponDef::projExplosionSound,
			&WeaponDef::projDudSound,
			&WeaponDef::projTrailEffect,
			&WeaponDef::projBeaconEffect,
			&WeaponDef::projIgnitionEffect,
			&WeaponDef::projIgnitionSound,
			&WeaponDef::szScript,
			json_reflect::nested(&WeaponDef::turretHydraulicSettings, turret_hydraulic_fields),
			json_reflect::manual("stateTimers"),
			json_reflect::manual("stateTimersAkimbo"),
			json_reflect::nested(&WeaponDef::overlay, overlay_fields),
			json_reflect::manual("accuracy_graph"),
			json_reflect::manual("stowTag"),
			&WeaponDef::altWeapon,
			&WeaponDef::playerAnimType,
			&WeaponDef::weapType,
			&WeaponDef::weapClass,
			&WeaponDef::penetrateType,
			&WeaponDef::penetrateDepth,
			&WeaponDef::impactType,
			&WeaponDef::inventoryType,
			&WeaponDef::fireType,
			&WeaponDef::fireBarrels,
			&WeaponDef::adsFireMode,
			&WeaponDef::burstFireCooldown,
			&WeaponDef::greebleType,
			&WeaponDef::autoReloadType,
			&WeaponDef::autoHolsterType,
			&WeaponDef::offhandClass,
			&WeaponDef::stance,
			&WeaponDef::reticleCenterSize,
			&WeaponDef::reticleSideSize,
			&WeaponDef::reticleMinOfs,
			&WeaponDef::activeReticleType,
			&WeaponDef::standMove,
			&WeaponDef::standRot,
			&WeaponDef::strafeMove,
			&WeaponDef::strafeRot,
			&WeaponDef::duckedOfs,
			&WeaponDef::duckedMove,
			&WeaponDef::duckedRot,
			&WeaponDef::proneOfs,
			&WeaponDef::proneMove,
			&WeaponDef::proneRot,
			&WeaponDef::unkVec1,
			&WeaponDef::unkVec2,
			&WeaponDef::posMoveRate,
			&WeaponDef::posProneMoveRate,
			&WeaponDef::standMoveMinSpeed,
			&WeaponDef::duckedMoveMinSpeed,
			&WeaponDef::proneMoveMinSpeed,
			&WeaponDef::posRotRate,
			&WeaponDef::posProneRotRate,
			&WeaponDef::hudIconRatio,
			&WeaponDef::pickupIconRatio,
			&WeaponDef::ammoCounterIconRatio,
			&WeaponDef::ammoCounterClip,
			&WeaponDef::startAmmo,
			&WeaponDef::maxAmmo,
			&WeaponDef::minAmmoReq,
			&WeaponDef::clipSize,
			&WeaponDef::shotCount,
			&WeaponDef::sharedAmmoCap,
			&WeaponDef::damage,
			&WeaponDef::playerDamage,
			&WeaponDef::meleeDamage,
			&WeaponDef::damageType,
			&WeaponDef::autoAimRange,
			&WeaponDef::aimAssistRange,
			&WeaponDef::aimAssistRangeAds,
			&WeaponDef::aimPadding,
			&WeaponDef::enemyCrosshairRange,
			&WeaponDef::moveSpeedScale,
			&WeaponDef::adsMoveSpeedScale,
			&WeaponDef::sprintDurationScale,
			&WeaponDef::adsZoomFov,
			&WeaponDef::adsZoomInFrac,
			&WeaponDef::adsZoomOutFrac,
			&WeaponDef::adsSceneBlurStrength,
			&WeaponDef::adsSceneBlurPhysicalScale,
			&WeaponDef::adsBobFactor,
			&WeaponDef::adsViewBobMult,
			&WeaponDef::hipSpreadStandMin,
			&WeaponDef::hipSpreadDuckedMin,
			&WeaponDef::hipSpreadProneMin,
			&WeaponDef::hipSpreadStandMax,
			&WeaponDef::hipSpreadSprintMax,
			&WeaponDef::hipSpreadSlideMax,
			&WeaponDef::hipSpreadDuckedMax,
			&WeaponDef::hipSpreadProneMax,
			&WeaponDef::hipSpreadDecayRate,
			&WeaponDef::hipSpreadFireAdd,
			&WeaponDef::hipSpreadTurnAdd,
			&WeaponDef::hipSpreadMoveAdd,
			&WeaponDef::hipSpreadDuckedDecay,
			&WeaponDef::hipSpreadProneDecay,
			&WeaponDef::hipReticleSidePos,
			&WeaponDef::adsIdleAmount,
			&WeaponDef::hipIdleAmount,
			&WeaponDef::adsIdleSpeed,
			&WeaponDef::hipIdleSpeed,
			&WeaponDef::idleCrouchFactor,
			&WeaponDef::idleProneFactor,
			&WeaponDef::gunMaxPitch,
			&WeaponDef::gunMaxYaw,
			&WeaponDef::adsIdleLerpStartTime,
			&WeaponDef::adsIdleLerpTime,
			&WeaponDef::adsTransInTime,
			&WeaponDef::adsTransInFromSprintTime,
			&WeaponDef::adsTransOutTime,
			&WeaponDef::swayMaxAngleSteadyAim,
			&WeaponDef::swayMaxAngle,
			&WeaponDef::swayLerpSpeed,
			&WeaponDef::swayPitchScale,
			&WeaponDef::swayYawScale,
			&WeaponDef::swayVertScale,
			&WeaponDef::swayHorizScale,
			&WeaponDef::swayShellShockScale,
			&WeaponDef::adsSwayMaxAngle,
			&WeaponDef::adsSwayLerpSpeed,
			&WeaponDef::adsSwayPitchScale,
			&WeaponDef::adsSwayYawScale,
			&WeaponDef::adsSwayHorizScale,
			&WeaponDef::adsSwayVertScale,
			&WeaponDef::adsViewErrorMin,
			&WeaponDef::adsViewErrorMax,
			&WeaponDef::adsFireAnimFrac,
			&WeaponDef::dualWieldViewModelOffset,
			&WeaponDef::scopeDriftDelay,
			&WeaponDef::scopeDriftLerpInTime,
			&WeaponDef::scopeDriftSteadyTime,
			&WeaponDef::scopeDriftLerpOutTime,
			&WeaponDef::scopeDriftSteadyFactor,
			&WeaponDef::scopeDriftUnsteadyFactor,
			&WeaponDef::bobVerticalFactor,
			&WeaponDef::bobHorizontalFactor,
			&WeaponDef::bobViewVerticalFactor,
			&WeaponDef::bobViewHorizontalFactor,
			&WeaponDef::stationaryZoomFov,
			&WeaponDef::stationaryZoomDelay,
			&WeaponDef::stationaryZoomLerpInTime,
			&WeaponDef::stationaryZoomLerpOutTime,
			&WeaponDef::adsDofStart,
			&WeaponDef::adsDofEnd,
			&WeaponDef::killIconRatio,
			&WeaponDef::dpadIconRatio,
			&WeaponDef::fireAnimLength,
			&WeaponDef::fireAnimLengthAkimbo,
			&WeaponDef::inspectAnimTime,
			&WeaponDef::reloadAmmoAdd,
			&WeaponDef::reloadStartAdd,
			&WeaponDef::ammoDropStockMin,
			&WeaponDef::ammoDropStockMax,
			&WeaponDef::ammoDropClipPercentMin,
			&WeaponDef::ammoDropClipPercentMax,
			&WeaponDef::explosionRadius,
			&WeaponDef::explosionRadiusMin,
			&WeaponDef::explosionInnerDamage,
			&WeaponDef::explosionOuterDamage,
			&WeaponDef::damageConeAngle,
			&WeaponDef::bulletExplDmgMult,
			&WeaponDef::bulletExplRadiusMult,
			&WeaponDef::projectileSpeed,
			&WeaponDef::projectileSpeedUp,
			&WeaponDef::projectileSpeedForward,
			&WeaponDef::projectileActivateDist,
			&WeaponDef::projLifetime,
			&WeaponDef::timeToAccelerate,
			&WeaponDef::projectileCurvature,
			&WeaponDef::projExplosion,
			&WeaponDef::stickiness,
			&WeaponDef::lowAmmoWarningThreshold,
			&WeaponDef::ricochetChance,
			&WeaponDef::riotShieldHealth,
			&WeaponDef::riotShieldDamageMult,
			json_reflect::counted(&WeaponDef::parallelBounce, 53),
			json_reflect::counted(&WeaponDef::perpendicularBounce, 53),
			&WeaponDef::projectileColor,
			&WeaponDef::guidedMissileType,
			&WeaponDef::maxSteeringAccel,
			&WeaponDef::projIgnitionDelay,
			&WeaponDef::adsAimPitch,
			&WeaponDef::adsCrosshairInFrac,
			&WeaponDef::adsCrosshairOutFrac,
			&WeaponDef::adsGunKickReducedKickBullets,
			&WeaponDef::adsGunKickReducedKickPercent,
			&WeaponDef::adsGunKickPitchMin,
			&WeaponDef::adsGunKickPitchMax,
			&WeaponDef::adsGunKickYawMin,
			&WeaponDef::adsGunKickYawMax,
			&WeaponDef::adsGunKickMagMin,
			&WeaponDef::adsGunKickAccel,
			&WeaponDef::adsGunKickSpeedMax,
			&WeaponDef::adsGunKickSpeedDecay,
			&WeaponDef::adsGunKickStaticDecay,
			&WeaponDef::adsViewKickPitchMin,
			&WeaponDef::adsViewKickPitchMax,
			&WeaponDef::adsViewKickYawMin,
			&WeaponDef::adsViewKickYawMax,
			&WeaponDef::adsViewKickMagMin,
			&WeaponDef::adsViewKickCenterSpeed,
			&WeaponDef::adsViewScatterMin,
			&WeaponDef::adsViewScatterMax,
			&WeaponDef::adsSpread,
			&WeaponDef::hipGunKickReducedKickBullets,
			&WeaponDef::hipGunKickReducedKickPercent,
			&WeaponDef::hipGunKickPitchMin,
			&WeaponDef::hipGunKickPitchMax,
			&WeaponDef::hipGunKickYawMin,
			&WeaponDef::hipGunKickYawMax,
			&WeaponDef::hipGunKickMagMin,
			&WeaponDef::hipGunKickAccel,
			&WeaponDef::hipGunKickSpeedMax,
			&WeaponDef::hipGunKickSpeedDecay,
			&WeaponDef::hipGunKickStaticDecay,
			&WeaponDef::hipViewKickPitchMin,
			&WeaponDef::hipViewKickPitchMax,
			&WeaponDef::hipViewKickYawMin,
			&WeaponDef::hipViewKickYawMax,
			&WeaponDef::hipViewKickMagMin,
			&WeaponDef::hipViewKickCenterSpeed,
			&WeaponDef::hipViewScatterMin,
			&WeaponDef::hipViewScatterMax,
			&WeaponDef::viewKickScale,
			&WeaponDef::positionReloadTransTime,
			&WeaponDef::fightDist,
			&WeaponDef::maxDist,
			&WeaponDef::leftArc,
			&WeaponDef::rightArc,
			&WeaponDef::topArc,
			&WeaponDef::bottomArc,
			&WeaponDef::accuracy,
			&WeaponDef::aiSpread,
			&WeaponDef::playerSpread,
			&WeaponDef::minTurnSpeed,
			&WeaponDef::maxTurnSpeed,
			&WeaponDef::pitchConvergenceTime,
			&WeaponDef::yawConvergenceTime,
			&WeaponDef::suppressTime,
			&WeaponDef::maxRange,
			&WeaponDef::animHorRotateInc,
			&WeaponDef::playerPositionDist,
			&WeaponDef::horizViewJitter,
			&WeaponDef::vertViewJitter,
			&WeaponDef::scanSpeed,
			&WeaponDef::scanAccel,
			&WeaponDef::scanPauseTime,
			&WeaponDef::minDamage,
			&WeaponDef::midDamage,
			&WeaponDef::minPlayerDamage,
			&WeaponDef::midPlayerDamage,
			&WeaponDef::maxDamageRange,
			&WeaponDef::minDamageRange,
			&WeaponDef::signatureAmmoInClip,
			&WeaponDef::signatureDamage,
			&WeaponDef::signatureMidDamage,
			&WeaponDef::signatureMinDamage,
			&WeaponDef::signatureMaxDamageRange,
			&WeaponDef::signatureMinDamageRange,
			&WeaponDef::destabilizationRateTime,
			&WeaponDef::destabilizationCurvatureMax,
			&WeaponDef::destabilizeDistance,
			&WeaponDef::turretADSTime,
			&WeaponDef::turretFov,
			&WeaponDef::turretFovADS,
			&WeaponDef::turretScopeZoomRate,
			&WeaponDef::turretScopeZoomMin,
			&WeaponDef::turretScopeZoomMax,
			&WeaponDef::overheatUpRate,
			&WeaponDef::overheatDownRate,
			&WeaponDef::overheatCooldownRate,
			&WeaponDef::overheatPenalty,
			&WeaponDef::turretBarrelSpinSpeed,
			&WeaponDef::turretBarrelSpinUpTime,
			&WeaponDef::turretBarrelSpinDownTime,
			&WeaponDef::missileConeSoundRadiusAtTop,
			&WeaponDef::missileConeSoundRadiusAtBase,
			&WeaponDef::missileConeSoundHeight,
			&WeaponDef::missileConeSoundOriginOffset,
			&WeaponDef::missileConeSoundVolumescaleAtCore,
			&WeaponDef::missileConeSoundVolumescaleAtEdge,
			&WeaponDef::missileConeSoundVolumescaleCoreSize,
			&WeaponDef::missileConeSoundPitchAtTop,
			&WeaponDef::missileConeSoundPitchAtBottom,
			&WeaponDef::missileConeSoundPitchTopSize,
			&WeaponDef::missileConeSoundPitchBottomSize,
			&WeaponDef::missileConeSoundCrossfadeTopSize,
			&WeaponDef::missileConeSoundCrossfadeBottomSize,
			&WeaponDef::aim_automelee_lerp,
			&WeaponDef::aim_automelee_range,
			&WeaponDef::aim_automelee_region_height,
			&WeaponDef::aim_automelee_region_width,
			&WeaponDef::player_meleeHeight,
			&WeaponDef::player_meleeRange,
			&WeaponDef::player_meleeWidth,
			&WeaponDef::changedFireTime,
			&WeaponDef::changedFireTimeNumBullets,
			&WeaponDef::fireTimeInterpolationType,
			&WeaponDef::generateAmmo,
			&WeaponDef::ammoPerShot,
			&WeaponDef::explodeCount,
			&WeaponDef::batteryDischargeRate,
			&WeaponDef::extendedBattery,
			&WeaponDef::bulletsPerTag,
			&WeaponDef::maxTags,
			&WeaponDef::rattleSoundType,
			&WeaponDef::adsShouldShowCrosshair,
			&WeaponDef::adsCrosshairShouldScale,
			&WeaponDef::turretADSEnabled,
			&WeaponDef::knifeAttachTagLeft,
			&WeaponDef::knifeAlwaysAttached,
			&WeaponDef::meleeOverrideValues,
			&WeaponDef::riotShieldEnableDamage,
			&WeaponDef::allowPrimaryWeaponPickup,
			&WeaponDef::sharedAmmo,
			&WeaponDef::lockonSupported,
			&WeaponDef::requireLockonToFire,
			&WeaponDef::isAirburstWeapon,
			&WeaponDef::bigExplosion,
			&WeaponDef::noAdsWhenMagEmpty,
			&WeaponDef::avoidDropCleanup,
			&WeaponDef::inheritsPerks,
			&WeaponDef::crosshairColorChange,
			&WeaponDef::rifleBullet,
			&WeaponDef::armorPiercing,
			&WeaponDef::boltAction,
			&WeaponDef::aimDownSight,
			&WeaponDef::canHoldBreath,
			&WeaponDef::meleeOnly,
			&WeaponDef::quickMelee,
			&WeaponDef::bU_086,
			&WeaponDef::canVariableZoom,
			&WeaponDef::rechamberWhileAds,
			&WeaponDef::bulletExplosiveDamage,
			&WeaponDef::cookOffHold,
			&WeaponDef::useBattery,
			&WeaponDef::reticleSpin45,
			&WeaponDef::clipOnly,
			&WeaponDef::noAmmoPickup,
			&WeaponDef::disableSwitchToWhenEmpty,
			&WeaponDef::suppressAmmoReserveDisplay,
			&WeaponDef::motionTracker,
			&WeaponDef::markableViewmodel,
			&WeaponDef::noDualWield,
			&WeaponDef::flipKillIcon,
			&WeaponDef::actionSlotShowAmmo,
			&WeaponDef::noPartialReload,
			&WeaponDef::segmentedReload,
			&WeaponDef::multipleReload,
			&WeaponDef::blocksProne,
			&WeaponDef::silenced,
			&WeaponDef::isRollingGrenade,
			&WeaponDef::projExplosionEffectForceNormalUp,
			&WeaponDef::projExplosionEffectInheritParentDirection,
			&WeaponDef::projImpactExplode,
			&WeaponDef::projTrajectoryEvents,
			&WeaponDef::projWhizByEnabled,
			&WeaponDef::stickToPlayers,
			&WeaponDef::stickToVehicles,
			&WeaponDef::stickToTurrets,
			&WeaponDef::thrownSideways,
			&WeaponDef::hasDetonatorEmptyThrow,
			&WeaponDef::hasDetonatorDoubleTap,
			&WeaponDef::disableFiring,
			&WeaponDef::timedDetonation,
			&WeaponDef::noCrumpleMissile,
			&WeaponDef::fuseLitAfterImpact,
			&WeaponDef::rotate,
			&WeaponDef::holdButtonToThrow,
			&WeaponDef::freezeMovementWhenFiring,
			&WeaponDef::thermalScope,
			&WeaponDef::thermalToggle,
			&WeaponDef::outlineEnemies,
			&WeaponDef::altModeSameWeapon,
			&WeaponDef::turretBarrelSpinEnabled,
			&WeaponDef::missileConeSoundEnabled,
			&WeaponDef::missileConeSoundPitchshiftEnabled,
			&WeaponDef::missileConeSoundCrossfadeEnabled,
			&WeaponDef::offhandHoldIsCancelable,
			&WeaponDef::doNotAllowAttachmentsToOverrideSpread,
			&WeaponDef::useFastReloadAnims,
			&WeaponDef::dualMagReloadSupported,
			&WeaponDef::reloadStopsAlt,
			&WeaponDef::useScopeDrift,
			&WeaponDef::alwaysShatterGlassOnImpact,
			&WeaponDef::oldWeapon,
			&WeaponDef::raiseToHold,
			&WeaponDef::notifyOnPlayerImpact,
			&WeaponDef::decreasingKick,
			&WeaponDef::counterSilencer,
			&WeaponDef::projSuppressedByEMP,
			&WeaponDef::projDisabledByEMP,
			&WeaponDef::autosimDisableVariableRate,
			&WeaponDef::projPlayTrailEffectForOwnerOnly,
			&WeaponDef::projPlayBeaconEffectForOwnerOnly,
			&WeaponDef::projKillTrailEffectOnDeath,
			&WeaponDef::projKillBeaconEffectOnDeath,
			&WeaponDef::reticleDetonateHide,
			&WeaponDef::cloaked,
			&WeaponDef::adsHideWeapon,
			&WeaponDef::adsHideHands,
			&WeaponDef::bU_108,
			&WeaponDef::adsSceneBlur,
			&WeaponDef::usesSniperScope,
			&WeaponDef::hasTransientModels,
			&WeaponDef::signatureAmmoAlternate,
			&WeaponDef::useScriptCallbackForHit,
			&WeaponDef::useBulletTagSystem,
			&WeaponDef::hideBulletTags,
			&WeaponDef::adsDofPhysicalFstop,
			&WeaponDef::adsDofPhysicalFocusDistance,
			&WeaponDef::autosimSpeedScale,
			&WeaponDef::reactiveMotionRadiusScale,
			&WeaponDef::reactiveMotionFrequencyScale,
			&WeaponDef::reactiveMotionAmplitudeScale,
			&WeaponDef::reactiveMotionFalloff,
			&WeaponDef::reactiveMotionLifetime,
			&WeaponDef::fU_3604,
		};

		constexpr json_reflect::options json_options{.strings = json_reflect::null_string::null};

		void read_anim_array(json& data, XAnimParts**& anims, zone_memory* mem)
		{
			if (data.is_null())
			{
				anims = nullptr;
				return;
			}

			anims = mem->allocate<XAnimParts*>(NUM_WEAP_ANIMS);
			for (auto i = 0; i < NUM_WEAP_ANIMS; i++)
			{
				json_reflect::read(data[get_anim_name_from_index(i)], anims[i], mem);
			}
		}

		void write_anim_array(ordered_json& data, XAnimParts** anims)
		{
			if (!anims)
			{
				data = nullptr;
				return;
			}

			for (auto i = 0; i < NUM_WEAP_ANIMS; i++)
			{
				json_reflect::write(data[get_anim_name_from_index(i)], anims[i]);
			}
		}

		json write_state_timers(const StateTimers& timers)
		{
			json data; // v1 writes the state timers with sorted keys
			json_reflect::write(data, timers);
			return data;
		}
	}

	void parse_accuracy_graph(WeaponDef* def, json& data, zone_memory* mem)
	{
		for (auto i = 0; i < 2; i++)
		{
			def->accuracyGraphName[i] = data["accuracyGraphName"][i].is_string()
				? mem->duplicate_string(data["accuracyGraphName"][i].get<std::string>())
				: nullptr;

			if (data["accuracyGraphKnots"][i].is_array())
			{
				const auto count = data["accuracyGraphKnots"][i].size();
				def->accuracyGraphKnots[i] = mem->allocate<vec2_t>(count);
				def->accuracyGraphKnotCount[i] = static_cast<short>(count);

				for (auto o = 0; o < count; o++)
				{
					def->accuracyGraphKnots[i][o][0] = data["accuracyGraphKnots"][i][o][0].get<float>();
					def->accuracyGraphKnots[i][o][1] = data["accuracyGraphKnots"][i][o][1].get<float>();
				}
			}

			if (data["originalAccuracyGraphKnots"][i].is_array())
			{
				const auto count = data["originalAccuracyGraphKnots"][i].size();
				def->originalAccuracyGraphKnots[i] = mem->allocate<vec2_t>(count);

				if (def->accuracyGraphKnotCount[i] != count)
				{
					ZONETOOL_FATAL("accuracyGraphKnots and originalAccuracyGraphKnots arrays must be the same size");
				}

				for (auto o = 0; o < count; o++)
				{
					def->originalAccuracyGraphKnots[i][o][0] = data["originalAccuracyGraphKnots"][i][o][0].get<float>();
					def->originalAccuracyGraphKnots[i][o][1] = data["originalAccuracyGraphKnots"][i][o][1].get<float>();
				}
			}
		}
	}


	WeaponDef* weapon_def::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "weapons\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing weapon \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();
		json data = json::parse(bytes);

		auto* weapon = mem->allocate<WeaponDef>();
		json_reflect::read<weapon_fields>(data, *weapon, mem, json_options);

		read_anim_array(data["szXAnimsRightHanded"], weapon->szXAnimsRightHanded, mem);
		read_anim_array(data["szXAnimsLeftHanded"], weapon->szXAnimsLeftHanded, mem);
		read_anim_array(data["szXAnims"], weapon->szXAnims, mem);

		json_reflect::read(data["stateTimers"], weapon->stateTimers, mem);
		json_reflect::read(data["stateTimersAkimbo"], weapon->akimboStateTimers, mem);

		weapon->hideTags = mem->allocate<scr_string_t>(32);
		for (auto i = 0; i < 32; i++)
		{
			this->add_script_string(&weapon->hideTags[i], mem->duplicate_string(data["hideTags"][i].get<std::string>()));
		}

		weapon->numNotetrackOverrides = static_cast<unsigned char>(data["notetrackOverrides"].size());
		if (weapon->numNotetrackOverrides)
		{
			weapon->notetrackOverrides = mem->allocate<NoteTrackToSoundEntry>(weapon->numNotetrackOverrides);
			for (auto i = 0u; i < weapon->numNotetrackOverrides; i++)
			{
				weapon->notetrackOverrides[i].attachment = data["notetrackOverrides"][i]["attachment"].get<int>();
				weapon->notetrackOverrides[i].notetrackSoundMapKeys = mem->allocate<scr_string_t>(36);
				weapon->notetrackOverrides[i].notetrackSoundMapValues = mem->allocate<scr_string_t>(36);
				for (auto j = 0u; j < 36; j++)
				{
					this->add_script_string(&weapon->notetrackOverrides[i].notetrackSoundMapKeys[j],
						mem->duplicate_string(data["notetrackOverrides"][i]["notetrackSoundMap"][j]["Key"].get<std::string>()));

					this->add_script_string(&weapon->notetrackOverrides[i].notetrackSoundMapValues[j],
						mem->duplicate_string(data["notetrackOverrides"][i]["notetrackSoundMap"][j]["Value"].get<std::string>()));
				}
			}
		}

		weapon->notetrackSoundMapKeys = mem->allocate<scr_string_t>(36);
		weapon->notetrackSoundMapValues = mem->allocate<scr_string_t>(36);
		for (auto i = 0; i < 36; i++)
		{
			this->add_script_string(&weapon->notetrackSoundMapKeys[i],
				mem->duplicate_string(data["notetrackSoundMap"][i]["Key"].get<std::string>()));

			this->add_script_string(&weapon->notetrackSoundMapValues[i],
				mem->duplicate_string(data["notetrackSoundMap"][i]["Value"].get<std::string>()));
		}

		weapon->notetrackRumbleMapKeys = mem->allocate<scr_string_t>(16);
		weapon->notetrackRumbleMapValues = mem->allocate<scr_string_t>(16);
		for (auto i = 0; i < 16; i++)
		{
			this->add_script_string(&weapon->notetrackRumbleMapKeys[i],
				mem->duplicate_string(data["notetrackRumbleMap"][i]["Key"].get<std::string>()));

			this->add_script_string(&weapon->notetrackRumbleMapValues[i],
				mem->duplicate_string(data["notetrackRumbleMap"][i]["Value"].get<std::string>()));
		}

		weapon->notetrackFXMapKeys = mem->allocate<scr_string_t>(16);
		weapon->notetrackFXMapTagValues = mem->allocate<scr_string_t>(16);
		weapon->notetrackFXMapValues = mem->allocate<FxEffectDef*>(16);
		for (auto i = 0; i < 16; i++)
		{
			this->add_script_string(&weapon->notetrackFXMapKeys[i],
				mem->duplicate_string(data["notetrackFXMap"][i]["Key"].get<std::string>()));

			this->add_script_string(&weapon->notetrackFXMapTagValues[i],
				mem->duplicate_string(data["notetrackFXMap"][i]["Tag"].get<std::string>()));

			json_reflect::read(data["notetrackFXMap"][i]["Value"], weapon->notetrackFXMapValues[i], mem);
		}

		weapon->notetrackHideTagKeys = mem->allocate<scr_string_t>(16);
		weapon->notetrackHideTagTagValues = mem->allocate<scr_string_t>(16);
		weapon->notetrackHideTagValues = mem->allocate<bool>(16);
		for (auto i = 0; i < 16; i++)
		{
			this->add_script_string(&weapon->notetrackHideTagKeys[i],
				mem->duplicate_string(data["notetrackHideTag"][i]["Key"].get<std::string>()));

			this->add_script_string(&weapon->notetrackHideTagTagValues[i],
				mem->duplicate_string(data["notetrackHideTag"][i]["Tag"].get<std::string>()));

			weapon->notetrackHideTagValues[i] = data["notetrackHideTag"][i]["Value"].get<bool>();
		}

		parse_accuracy_graph(weapon, data["accuracy_graph"], mem);

		if (!data["stowTag"].is_null())
		{
			auto stowTag = data["stowTag"].get<std::string>();
			this->add_script_string(&weapon->stowTag, mem->duplicate_string(stowTag));
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

	void weapon_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* weapon = this->asset_;

#define WEAPON_SCRIPTSTRING_ARRAY_PREPARE(__field__,__count__) \
		if (weapon->__field__) \
		{ \
			for (int nt = 0; nt < __count__; nt++) \
			{ \
				weapon->__field__[nt] = static_cast<scr_string_t>(buf->write_scriptstring(this->get_script_string(&weapon->__field__[nt]))); \
			} \
		}

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
				WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackOverrides[i].notetrackSoundMapKeys, 36);
				WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackOverrides[i].notetrackSoundMapValues, 36);
			}
		}

		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackSoundMapKeys, 36);
		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackSoundMapValues, 36);
		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackRumbleMapKeys, 16);
		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackRumbleMapValues, 16);
		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackFXMapKeys, 16);
		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackFXMapTagValues, 16);
		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackHideTagKeys, 16);
		WEAPON_SCRIPTSTRING_ARRAY_PREPARE(notetrackHideTagTagValues, 16);

		weapon->stowTag = static_cast<scr_string_t>(buf->write_scriptstring(
			this->get_script_string(&weapon->stowTag)));
	}

	void weapon_def::load_depending(zone_base* zone)
	{
		auto weapon = this->asset_;
#define WEAPON_SUBASSET_DEPENDING(__field__,__type__/*,__struct__*/) \
		if (weapon->__field__) \
		{ \
			zone->add_asset_of_type(__type__, weapon->__field__->name); \
		}

		if (weapon->szDisplayName)
		{
			zone->add_asset_of_type(ASSET_TYPE_LOCALIZE_ENTRY, weapon->szDisplayName);
		}

		for (int i = 0; i < 2; i++)
		{
			if (weapon->gunModel)
			{
				WEAPON_SUBASSET_DEPENDING(gunModel[i], ASSET_TYPE_XMODEL);
			}
			if (weapon->worldModel)
			{
				WEAPON_SUBASSET_DEPENDING(worldModel[i], ASSET_TYPE_XMODEL);
			}
		}

		for (auto i = 0; i < 64; i++)
		{
			if (weapon->reticleViewModels)
			{
				WEAPON_SUBASSET_DEPENDING(reticleViewModels[i], ASSET_TYPE_XMODEL);
			}
		}

		WEAPON_SUBASSET_DEPENDING(handModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(persistentArmXModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(worldClipModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(rocketModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(knifeModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(worldKnifeModel, ASSET_TYPE_XMODEL);

		for (auto i = 0; i < NUM_WEAP_ANIMS; i++)
		{
			if (weapon->szXAnimsRightHanded)
			{
				WEAPON_SUBASSET_DEPENDING(szXAnimsRightHanded[i], ASSET_TYPE_XANIM);
			}
			if (weapon->szXAnimsLeftHanded)
			{
				WEAPON_SUBASSET_DEPENDING(szXAnimsLeftHanded[i], ASSET_TYPE_XANIM);
			}
			if (weapon->szXAnims)
			{
				WEAPON_SUBASSET_DEPENDING(szXAnims[i], ASSET_TYPE_XANIM);
			}
		}

		for (auto i = 0; i < weapon->numAttachments; i++)
		{
			if (weapon->attachments)
			{
				WEAPON_SUBASSET_DEPENDING(attachments[i], ASSET_TYPE_ATTACHMENT);
			}
		}

		if (weapon->animOverrides)
		{
			for (auto i = 0u; i < weapon->numAnimOverrides; i++)
			{
				WEAPON_SUBASSET_DEPENDING(animOverrides[i].overrideAnim, ASSET_TYPE_XANIM);
				WEAPON_SUBASSET_DEPENDING(animOverrides[i].altmodeAnim, ASSET_TYPE_XANIM);
			}
		}

		if (weapon->soundOverrides)
		{
			for (auto i = 0u; i < weapon->numSoundOverrides; i++)
			{
				WEAPON_SUBASSET_DEPENDING(soundOverrides[i].overrideSound, ASSET_TYPE_SOUND);
				WEAPON_SUBASSET_DEPENDING(soundOverrides[i].altmodeSound, ASSET_TYPE_SOUND);
			}
		}

		if (weapon->fxOverrides)
		{
			for (auto i = 0u; i < weapon->numFXOverrides; i++)
			{
				WEAPON_SUBASSET_DEPENDING(fxOverrides[i].overrideFX, ASSET_TYPE_FX);
				WEAPON_SUBASSET_DEPENDING(fxOverrides[i].altmodeFX, ASSET_TYPE_FX);
			}
		}

		if (weapon->notetrackOverrides)
		{
			for (auto i = 0u; i < weapon->numNotetrackOverrides; i++)
			{
				for (auto j = 0; j < 36; j++)
				{
					auto str = this->get_script_string(&weapon->notetrackOverrides[i].notetrackSoundMapValues[j]);
					if (str != nullptr && str[0] != '\0')
					{
						zone->add_asset_of_type(ASSET_TYPE_SOUND, str);
					}
				}
			}
		}

		for (auto i = 0; i < 36; i++)
		{
			auto str = this->get_script_string(&weapon->notetrackSoundMapValues[i]);
			if (str != nullptr && str[0] != '\0')
			{
				zone->add_asset_of_type(ASSET_TYPE_SOUND, str);
			}
		}

		for (auto i = 0; i < 16; i++)
		{
			if (weapon->notetrackFXMapValues)
			{
				WEAPON_SUBASSET_DEPENDING(notetrackFXMapValues[i], ASSET_TYPE_FX);
			}
		}
		
		WEAPON_SUBASSET_DEPENDING(viewFlashEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(viewBodyFlashEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(worldFlashEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(viewFlashADSEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(viewBodyFlashADSEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(signatureViewFlashEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(signatureViewBodyFlashEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(signatureWorldFlashEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(signatureViewFlashADSEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(signatureViewBodyFlashADSEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(meleeHitEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(meleeMissEffect, ASSET_TYPE_FX);

		WEAPON_SUBASSET_DEPENDING(pickupSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(pickupSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(ammoPickupSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(ammoPickupSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(projectileSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(pullbackSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(pullbackSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(pullbackSoundQuick, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(pullbackSoundQuickPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireSoundPlayerLeft, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireSoundPlayerRight, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound14, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound15, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound16, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound17, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound18, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireLoopSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireLoopSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound21, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound22, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound23, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound24, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound25, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound26, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireStopSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireStopSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound29, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound30, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound31, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound32, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound33, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireFirstSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireFirstSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireSound2, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireSoundPlayer2, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireSpecialSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(fireSpecialSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(emptyFireSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(emptyFireSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(emptyFireSoundPlayerLeft, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(emptyFireSoundPlayerRight, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(emptyFireSoundReleasePlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(emptyFireSoundReleasePlayerLeft, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(emptyFireSoundReleasePlayerRight, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound47, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(meleeSwipeSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(meleeSwipeSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(meleeHitSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(meleeHitSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(meleeMissSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(meleeMissSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound54, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound55, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound56, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound57, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound58, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound59, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound60, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound61, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound62, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound63, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound64, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound65, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(nightVisionWearSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(nightVisionWearSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(nightVisionRemoveSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(nightVisionRemoveSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(raiseSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(raiseSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(raiseSoundPlayerLeft, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(raiseSoundPlayerRight, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound74, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(quickRaiseSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(quickRaiseSoundPlayerLeft, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(quickRaiseSoundPlayerRight, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(raiseSound2, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound79, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound80, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound81, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(putawaySound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(putawaySoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(putawaySoundPlayerLeft, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(putawaySoundPlayerRight, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound86, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(sound87, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(adsEnterSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(adsLeaveSoundPlayer, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(adsCrosshairEnemySound, ASSET_TYPE_SOUND);
		
		for (auto i = 0; i < 53; i++)
		{
			if (weapon->bounceSound)
			{
				WEAPON_SUBASSET_DEPENDING(bounceSound[i], ASSET_TYPE_SOUND);
			}

			if (weapon->rollingSound)
			{
				WEAPON_SUBASSET_DEPENDING(rollingSound[i], ASSET_TYPE_SOUND);
			}
		}

		WEAPON_SUBASSET_DEPENDING(viewShellEjectEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(worldShellEjectEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(viewLastShotEjectEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(worldLastShotEjectEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(viewMagEjectEffect, ASSET_TYPE_FX);

		WEAPON_SUBASSET_DEPENDING(reticleCenter, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(reticleSide, ASSET_TYPE_MATERIAL);

		WEAPON_SUBASSET_DEPENDING(hudIcon, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(pickupIcon, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(minimapIconFriendly, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(minimapIconEnemy, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(minimapIconNeutral, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(ammoCounterIcon, ASSET_TYPE_MATERIAL);

		WEAPON_SUBASSET_DEPENDING(physCollmap, ASSET_TYPE_PHYSCOLLMAP);
		WEAPON_SUBASSET_DEPENDING(physPreset, ASSET_TYPE_PHYSPRESET);

		WEAPON_SUBASSET_DEPENDING(tracerType, ASSET_TYPE_TRACER);
		WEAPON_SUBASSET_DEPENDING(signatureTracerType, ASSET_TYPE_TRACER);

		WEAPON_SUBASSET_DEPENDING(laserType, ASSET_TYPE_LASER);

		WEAPON_SUBASSET_DEPENDING(turretOverheatSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(turretOverheatEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(turretBarrelSpinMaxSnd, ASSET_TYPE_SOUND);
		for (int i = 0; i < 4; i++)
		{
			WEAPON_SUBASSET_DEPENDING(turretBarrelSpinUpSnd[i], ASSET_TYPE_SOUND);
			WEAPON_SUBASSET_DEPENDING(turretBarrelSpinDownSnd[i], ASSET_TYPE_SOUND);
		}
		WEAPON_SUBASSET_DEPENDING(missileConeSoundAlias, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(missileConeSoundAliasAtBase, ASSET_TYPE_SOUND);

		WEAPON_SUBASSET_DEPENDING(stowOffsetModel, ASSET_TYPE_XMODEL);

		WEAPON_SUBASSET_DEPENDING(overlay.shader, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(overlay.shaderLowRes, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(overlay.shaderEMP, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(overlay.shaderEMPLowRes, ASSET_TYPE_MATERIAL);

		WEAPON_SUBASSET_DEPENDING(killIcon, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(dpadIcon, ASSET_TYPE_MATERIAL);
		WEAPON_SUBASSET_DEPENDING(hudProximityWarningIcon, ASSET_TYPE_MATERIAL);
		
		WEAPON_SUBASSET_DEPENDING(projectileModel, ASSET_TYPE_XMODEL);
		WEAPON_SUBASSET_DEPENDING(projExplosionEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(projDudEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(projExplosionSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(projDudSound, ASSET_TYPE_SOUND);
		WEAPON_SUBASSET_DEPENDING(projTrailEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(projBeaconEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(projIgnitionEffect, ASSET_TYPE_FX);
		WEAPON_SUBASSET_DEPENDING(projIgnitionSound, ASSET_TYPE_SOUND);
	}

	std::string weapon_def::name()
	{
		return this->name_;
	}

	std::int32_t weapon_def::type()
	{
		return ASSET_TYPE_WEAPON;
	}

	namespace
	{
		constexpr auto flash_effect_count = 12;
		constexpr auto weapon_sound_count = 90;
		constexpr auto anim_count = NUM_WEAP_ANIMS;
		constexpr auto surface_count = 53;

		static_assert(offsetof(WeaponDef, meleeMissEffect) == offsetof(WeaponDef, viewFlashEffect) + (flash_effect_count - 1) * sizeof(FxEffectDef*));
		static_assert(offsetof(WeaponDef, adsCrosshairEnemySound) == offsetof(WeaponDef, pickupSound) + (weapon_sound_count - 1) * sizeof(snd_alias_list_t*));

		void read_overrides(zone_reader& reader, WeaponDef* asset)
		{
			if (const auto anims = reader.read_array(asset->animOverrides, 3, asset->numAnimOverrides))
			{
				for (auto i = 0; i < asset->numAnimOverrides; i++)
				{
					reader.read_asset(ASSET_TYPE_XANIM, anims[i].overrideAnim);
					reader.read_asset(ASSET_TYPE_XANIM, anims[i].altmodeAnim);
				}
			}

			if (const auto sounds = reader.read_array(asset->soundOverrides, 3, asset->numSoundOverrides))
			{
				for (auto i = 0; i < asset->numSoundOverrides; i++)
				{
					reader.read_name_reference(sounds[i].overrideSound);
					reader.read_name_reference(sounds[i].altmodeSound);
				}
			}

			if (const auto effects = reader.read_array(asset->fxOverrides, 3, asset->numFXOverrides))
			{
				for (auto i = 0; i < asset->numFXOverrides; i++)
				{
					reader.read_asset(ASSET_TYPE_FX, effects[i].overrideFX);
					reader.read_asset(ASSET_TYPE_FX, effects[i].altmodeFX);
				}
			}

			reader.read_array(asset->reloadOverrides, 3, asset->numReloadStateTimerOverrides);

			if (const auto notetracks = reader.read_array(asset->notetrackOverrides, 3, asset->numNotetrackOverrides))
			{
				for (auto i = 0; i < asset->numNotetrackOverrides; i++)
				{
					reader.read_script_string_array(notetracks[i].notetrackSoundMapKeys, 36);
					reader.read_script_string_array(notetracks[i].notetrackSoundMapValues, 36);
				}
			}
		}

		void read_notetrack_maps(zone_reader& reader, WeaponDef* asset)
		{
			reader.read_script_string_array(asset->notetrackSoundMapKeys, 36);
			reader.read_script_string_array(asset->notetrackSoundMapValues, 36);
			reader.read_script_string_array(asset->notetrackRumbleMapKeys, 16);
			reader.read_script_string_array(asset->notetrackRumbleMapValues, 16);
			reader.read_script_string_array(asset->notetrackFXMapKeys, 16);
			reader.read_asset_array(ASSET_TYPE_FX, asset->notetrackFXMapValues, 16);
			reader.read_script_string_array(asset->notetrackFXMapTagValues, 16);
			reader.read_script_string_array(asset->notetrackHideTagKeys, 16);
			reader.read_array(asset->notetrackHideTagValues, 0, 16);
			reader.read_script_string_array(asset->notetrackHideTagTagValues, 16);
		}

		void read_turret(zone_reader& reader, WeaponDef* asset)
		{
			reader.read_name_reference(asset->turretOverheatSound);
			reader.read_asset(ASSET_TYPE_FX, asset->turretOverheatEffect);
			reader.read_string(asset->turretBarrelSpinRumble);
			reader.read_name_reference(asset->turretBarrelSpinMaxSnd);

			for (auto i = 0; i < 4; i++)
			{
				reader.read_name_reference(asset->turretBarrelSpinUpSnd[i]);
				reader.read_name_reference(asset->turretBarrelSpinDownSnd[i]);
			}

			reader.read_name_reference(asset->missileConeSoundAlias);
			reader.read_name_reference(asset->missileConeSoundAliasAtBase);
			reader.read_asset(ASSET_TYPE_XMODEL, asset->stowOffsetModel);

			if (const auto hydraulics = reader.read_single(asset->turretHydraulicSettings, 3))
			{
				reader.read_name_reference(hydraulics->verticalSound);
				reader.read_name_reference(hydraulics->verticalStopSound);
				reader.read_name_reference(hydraulics->horizontalSound);
				reader.read_name_reference(hydraulics->horizontalStopSound);
			}
		}

		void read_projectile(zone_reader& reader, WeaponDef* asset)
		{
			reader.read_string(asset->projectileName);
			reader.read_asset(ASSET_TYPE_XMODEL, asset->projectileModel);
			reader.read_asset(ASSET_TYPE_FX, asset->projExplosionEffect);
			reader.read_asset(ASSET_TYPE_FX, asset->projDudEffect);
			reader.read_name_reference(asset->projExplosionSound);
			reader.read_name_reference(asset->projDudSound);
			reader.read_array(asset->parallelBounce, 3, surface_count);
			reader.read_array(asset->perpendicularBounce, 3, surface_count);
			reader.read_asset(ASSET_TYPE_FX, asset->projTrailEffect);
			reader.read_asset(ASSET_TYPE_FX, asset->projBeaconEffect);
			reader.read_asset(ASSET_TYPE_FX, asset->projIgnitionEffect);
			reader.read_name_reference(asset->projIgnitionSound);
		}
	}

	void weapon_def::read(zone_reader& reader, WeaponDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->szInternalName);
		reader.read_string(asset->szDisplayName);
		reader.read_string(asset->szOverlayName);
		reader.read_string(asset->szAttachmentName);
		reader.read_string(asset->szAltWeaponName);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->gunModel, 2);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->handModel);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->persistentArmXModel);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->reticleViewModels, 64);
		reader.read_string(asset->lobWorldModelName);
		reader.read_asset_array(ASSET_TYPE_XANIM, asset->szXAnimsRightHanded, anim_count);
		reader.read_asset_array(ASSET_TYPE_XANIM, asset->szXAnimsLeftHanded, anim_count);
		reader.read_script_string_array(asset->hideTags, 32);
		reader.read_asset_array(ASSET_TYPE_ATTACHMENT, asset->attachments, asset->numAttachments);
		reader.read_asset_array(ASSET_TYPE_XANIM, asset->szXAnims, anim_count);
		read_overrides(reader, asset);
		read_notetrack_maps(reader, asset);
		reader.read_string(asset->szAdsrBaseSetting);

		const auto flash_effects = &asset->viewFlashEffect;
		for (auto i = 0; i < flash_effect_count; i++)
		{
			reader.read_asset(ASSET_TYPE_FX, flash_effects[i]);
		}

		const auto sounds = &asset->pickupSound;
		for (auto i = 0; i < weapon_sound_count; i++)
		{
			reader.read_name_reference(sounds[i]);
		}

		reader.read_name_reference_array(asset->bounceSound, surface_count);
		reader.read_name_reference_array(asset->rollingSound, surface_count);
		reader.read_asset(ASSET_TYPE_FX, asset->viewShellEjectEffect);
		reader.read_asset(ASSET_TYPE_FX, asset->worldShellEjectEffect);
		reader.read_asset(ASSET_TYPE_FX, asset->viewLastShotEjectEffect);
		reader.read_asset(ASSET_TYPE_FX, asset->worldLastShotEjectEffect);
		reader.read_asset(ASSET_TYPE_FX, asset->viewMagEjectEffect);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->reticleCenter);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->reticleSide);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->worldModel, 2);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->worldClipModel);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->rocketModel);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->knifeModel);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->worldKnifeModel);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->hudIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->pickupIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->minimapIconFriendly);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->minimapIconEnemy);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->minimapIconNeutral);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->ammoCounterIcon);
		reader.read_string(asset->szAmmoName);
		reader.read_string(asset->szClipName);
		reader.read_string(asset->szSharedAmmoCapName);
		reader.read_asset(ASSET_TYPE_PHYSCOLLMAP, asset->physCollmap);
		reader.read_asset(ASSET_TYPE_PHYSPRESET, asset->physPreset);
		reader.read_string(asset->szUseHintString);
		reader.read_string(asset->dropHintString);
		reader.read_array(asset->locationDamageMultipliers, 3, 22);
		reader.read_string(asset->fireRumble);
		reader.read_string(asset->fireMedRumble);
		reader.read_string(asset->fireHighRumble);
		reader.read_string(asset->meleeImpactRumble);
		reader.read_asset(ASSET_TYPE_TRACER, asset->tracerType);
		reader.read_asset(ASSET_TYPE_TRACER, asset->signatureTracerType);
		reader.read_asset(ASSET_TYPE_LASER, asset->laserType);
		read_turret(reader, asset);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->overlay.shader);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->overlay.shaderLowRes);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->overlay.shaderEMP);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->overlay.shaderEMPLowRes);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->killIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->dpadIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->hudProximityWarningIcon);
		read_projectile(reader, asset);

		for (auto i = 0; i < 2; i++)
		{
			reader.read_string(asset->accuracyGraphName[i]);
			reader.read_array(asset->accuracyGraphKnots[i], 3, asset->accuracyGraphKnotCount[i]);
			reader.read_array(asset->originalAccuracyGraphKnots[i], 3, asset->accuracyGraphKnotCount[i]);
		}

		reader.read_string(asset->szScript);
		reader.read_script_string(asset->stowTag);
		reader.pop_stream();
	}

	void weapon_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

#define WEAPON_STRING(__field__) \
		if(data->__field__) \
		{ \
			dest->__field__ = buf->write_str(data->__field__); \
		}

#define WEAPON_SUBASSET(__field__,__type__,__struct__) \
		if (data->__field__) \
		{ \
			dest->__field__ = reinterpret_cast<__struct__*>(zone->get_asset_pointer(__type__, data->__field__->name)); \
		}

#define WEAPON_SOUND_CUSTOM(__field__) \
		if (data->__field__) \
		{ \
			buf->align(7); \
			auto* ptr = reinterpret_cast<const char**>(buf->write(&buf->data_following)); \
			*ptr = buf->write_str(data->__field__->name); \
			buf->clear_pointer(&dest->__field__); \
		}

#define WEAPON_SCRIPTSTRING_ARRAY(__field__,__count__) \
		if (data->__field__) \
		{ \
			buf->align(3); \
			buf->write(data->__field__,__count__); \
			buf->clear_pointer(&dest->__field__); \
		}

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->szInternalName = buf->write_str(this->name());

		WEAPON_STRING(szDisplayName);
		WEAPON_STRING(szOverlayName);
		WEAPON_STRING(szAttachmentName);
		WEAPON_STRING(szAltWeaponName);

		if (data->gunModel)
		{
			buf->align(7);
			auto destModels = buf->write(data->gunModel, 2);

			for (auto i = 0; i < 2; i++)
			{
				if (destModels[i])
				{
					destModels[i] = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->gunModel[i]->name)
						);
				}
			}

			buf->clear_pointer(&dest->gunModel);
		}

		if (data->handModel)
		{
			dest->handModel = reinterpret_cast<XModel*>(
				zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->handModel->name)
				);
		}

		if (data->persistentArmXModel)
		{
			dest->persistentArmXModel = reinterpret_cast<XModel*>(
				zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->persistentArmXModel->name)
				);
		}

		if (data->reticleViewModels)
		{
			buf->align(7);
			auto destModels = buf->write(data->reticleViewModels, 64);

			for (auto i = 0; i < 64; i++)
			{
				if (destModels[i])
				{
					destModels[i] = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->reticleViewModels[i]->name)
						);
				}
			}

			buf->clear_pointer(&dest->reticleViewModels);
		}

		WEAPON_STRING(lobWorldModelName);

		if (data->szXAnimsRightHanded)
		{
			buf->align(7);
			auto destAnims = buf->write(data->szXAnimsRightHanded, NUM_WEAP_ANIMS);

			for (auto i = 0; i < NUM_WEAP_ANIMS; i++)
			{
				if (destAnims[i])
				{
					destAnims[i] = reinterpret_cast<XAnimParts*>(
						zone->get_asset_pointer(ASSET_TYPE_XANIM, data->szXAnimsRightHanded[i]->name)
						);
				}
			}

			buf->clear_pointer(&dest->szXAnimsRightHanded);
		}

		if (data->szXAnimsLeftHanded)
		{
			buf->align(7);
			auto destAnims = buf->write(data->szXAnimsLeftHanded, NUM_WEAP_ANIMS);

			for (auto i = 0; i < NUM_WEAP_ANIMS; i++)
			{
				if (destAnims[i])
				{
					destAnims[i] = reinterpret_cast<XAnimParts*>(
						zone->get_asset_pointer(ASSET_TYPE_XANIM, data->szXAnimsLeftHanded[i]->name)
						);
				}
			}

			buf->clear_pointer(&dest->szXAnimsLeftHanded);
		}

		if (data->hideTags)
		{
			buf->align(3);
			buf->write(data->hideTags, 32);
			buf->clear_pointer(&dest->hideTags);
		}

		if (data->attachments)
		{
			buf->align(7);
			auto destAttachments = buf->write(data->attachments, data->numAttachments);

			for (auto i = 0; i < data->numAttachments; i++)
			{
				if (destAttachments[i])
				{
					destAttachments[i] = reinterpret_cast<WeaponAttachment*>(
						zone->get_asset_pointer(ASSET_TYPE_ATTACHMENT, data->attachments[i]->name)
						);
				}
			}

			buf->clear_pointer(&dest->attachments);
		}

		if (data->szXAnims)
		{
			buf->align(7);
			auto destAnims = buf->write(data->szXAnims, NUM_WEAP_ANIMS);

			for (auto i = 0; i < NUM_WEAP_ANIMS; i++)
			{
				if (destAnims[i])
				{
					destAnims[i] = reinterpret_cast<XAnimParts*>(
						zone->get_asset_pointer(ASSET_TYPE_XANIM, data->szXAnims[i]->name)
						);
				}
			}

			buf->clear_pointer(&dest->szXAnims);
		}

		if (data->animOverrides)
		{
			buf->align(3);
			auto destAnimOverrides = buf->write(data->animOverrides, data->numAnimOverrides);

			for (auto i = 0; i < data->numAnimOverrides; i++)
			{
				if (destAnimOverrides[i].overrideAnim)
				{
					destAnimOverrides[i].overrideAnim = reinterpret_cast<XAnimParts*>(
						zone->get_asset_pointer(ASSET_TYPE_XANIM, data->animOverrides[i].overrideAnim->name)
						);
				}

				if (destAnimOverrides[i].altmodeAnim)
				{
					destAnimOverrides[i].altmodeAnim = reinterpret_cast<XAnimParts*>(
						zone->get_asset_pointer(ASSET_TYPE_XANIM, data->animOverrides[i].altmodeAnim->name)
						);
				}
			}

			buf->clear_pointer(&dest->animOverrides);
		}

		if (data->soundOverrides)
		{
			buf->align(3);
			dest->soundOverrides = buf->write(data->soundOverrides, data->numSoundOverrides);

			for (auto i = 0; i < data->numSoundOverrides; i++)
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

			buf->clear_pointer(&dest->soundOverrides);
		}

		if (data->fxOverrides)
		{
			buf->align(3);
			auto destFxOverrides = buf->write(data->fxOverrides, data->numFXOverrides);

			for (auto i = 0; i < data->numFXOverrides; i++)
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

			buf->clear_pointer(&dest->fxOverrides);
		}

		if (data->reloadOverrides)
		{
			buf->align(3);
			buf->write(data->reloadOverrides, data->numReloadStateTimerOverrides);
			buf->clear_pointer(&dest->reloadOverrides);
		}

		if (data->notetrackOverrides)
		{
			buf->align(3);
			auto destNoteTrackOverrides = buf->write(data->notetrackOverrides, data->numNotetrackOverrides);

			for (auto i = 0; i < data->numNotetrackOverrides; i++)
			{
				if (destNoteTrackOverrides[i].notetrackSoundMapKeys)
				{
					buf->align(3);
					buf->write(data->notetrackOverrides[i].notetrackSoundMapKeys, 36);
					buf->clear_pointer(&destNoteTrackOverrides[i].notetrackSoundMapKeys);
				}

				if (destNoteTrackOverrides[i].notetrackSoundMapValues)
				{
					buf->align(3);
					buf->write(data->notetrackOverrides[i].notetrackSoundMapValues, 36);
					buf->clear_pointer(&destNoteTrackOverrides[i].notetrackSoundMapValues);
				}
			}

			buf->clear_pointer(&dest->notetrackOverrides);
		}

		WEAPON_SCRIPTSTRING_ARRAY(notetrackSoundMapKeys, 36);
		WEAPON_SCRIPTSTRING_ARRAY(notetrackSoundMapValues, 36);
		WEAPON_SCRIPTSTRING_ARRAY(notetrackRumbleMapKeys, 16);
		WEAPON_SCRIPTSTRING_ARRAY(notetrackRumbleMapValues, 16);

		WEAPON_SCRIPTSTRING_ARRAY(notetrackFXMapKeys, 16);
		if (data->notetrackFXMapValues)
		{
			buf->align(7);
			auto destNotetrackFXMapValues = buf->write(data->notetrackFXMapValues, 16);

			for (auto i = 0; i < 16; i++)
			{
				if (destNotetrackFXMapValues[i])
				{
					destNotetrackFXMapValues[i] = reinterpret_cast<FxEffectDef*>(
						zone->get_asset_pointer(ASSET_TYPE_FX, data->notetrackFXMapValues[i]->name)
						);
				}
			}

			buf->clear_pointer(&dest->notetrackFXMapValues);
		}
		WEAPON_SCRIPTSTRING_ARRAY(notetrackFXMapTagValues, 16);

		WEAPON_SCRIPTSTRING_ARRAY(notetrackHideTagKeys, 16);
		if (data->notetrackHideTagValues)
		{
			buf->align(0);
			buf->write(data->notetrackHideTagValues, 16);
			buf->clear_pointer(&dest->notetrackHideTagValues);
		}
		WEAPON_SCRIPTSTRING_ARRAY(notetrackHideTagTagValues, 16);

		WEAPON_STRING(szAdsrBaseSetting);

		WEAPON_SUBASSET(viewFlashEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(viewBodyFlashEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(worldFlashEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(viewFlashADSEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(viewBodyFlashADSEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(signatureViewFlashEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(signatureViewBodyFlashEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(signatureWorldFlashEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(signatureViewFlashADSEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(signatureViewBodyFlashADSEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(meleeHitEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(meleeMissEffect, ASSET_TYPE_FX, FxEffectDef);

		WEAPON_SOUND_CUSTOM(pickupSound);
		WEAPON_SOUND_CUSTOM(pickupSoundPlayer);
		WEAPON_SOUND_CUSTOM(ammoPickupSound);
		WEAPON_SOUND_CUSTOM(ammoPickupSoundPlayer);
		WEAPON_SOUND_CUSTOM(projectileSound);
		WEAPON_SOUND_CUSTOM(pullbackSound);
		WEAPON_SOUND_CUSTOM(pullbackSoundPlayer);
		WEAPON_SOUND_CUSTOM(pullbackSoundQuick);
		WEAPON_SOUND_CUSTOM(pullbackSoundQuickPlayer);
		WEAPON_SOUND_CUSTOM(fireSound);
		WEAPON_SOUND_CUSTOM(fireSoundPlayer);
		WEAPON_SOUND_CUSTOM(fireSoundPlayerLeft);
		WEAPON_SOUND_CUSTOM(fireSoundPlayerRight);
		WEAPON_SOUND_CUSTOM(sound14);
		WEAPON_SOUND_CUSTOM(sound15);
		WEAPON_SOUND_CUSTOM(sound16);
		WEAPON_SOUND_CUSTOM(sound17);
		WEAPON_SOUND_CUSTOM(sound18);
		WEAPON_SOUND_CUSTOM(fireLoopSound);
		WEAPON_SOUND_CUSTOM(fireLoopSoundPlayer);
		WEAPON_SOUND_CUSTOM(sound21);
		WEAPON_SOUND_CUSTOM(sound22);
		WEAPON_SOUND_CUSTOM(sound23);
		WEAPON_SOUND_CUSTOM(sound24);
		WEAPON_SOUND_CUSTOM(sound25);
		WEAPON_SOUND_CUSTOM(sound26);
		WEAPON_SOUND_CUSTOM(fireStopSound);
		WEAPON_SOUND_CUSTOM(fireStopSoundPlayer);
		WEAPON_SOUND_CUSTOM(sound29);
		WEAPON_SOUND_CUSTOM(sound30);
		WEAPON_SOUND_CUSTOM(sound31);
		WEAPON_SOUND_CUSTOM(sound32);
		WEAPON_SOUND_CUSTOM(sound33);
		WEAPON_SOUND_CUSTOM(fireFirstSound);
		WEAPON_SOUND_CUSTOM(fireFirstSoundPlayer);
		WEAPON_SOUND_CUSTOM(fireSound2);
		WEAPON_SOUND_CUSTOM(fireSoundPlayer2);
		WEAPON_SOUND_CUSTOM(fireSpecialSound);
		WEAPON_SOUND_CUSTOM(fireSpecialSoundPlayer);
		WEAPON_SOUND_CUSTOM(emptyFireSound);
		WEAPON_SOUND_CUSTOM(emptyFireSoundPlayer);
		WEAPON_SOUND_CUSTOM(emptyFireSoundPlayerLeft);
		WEAPON_SOUND_CUSTOM(emptyFireSoundPlayerRight);
		WEAPON_SOUND_CUSTOM(emptyFireSoundReleasePlayer);
		WEAPON_SOUND_CUSTOM(emptyFireSoundReleasePlayerLeft);
		WEAPON_SOUND_CUSTOM(emptyFireSoundReleasePlayerRight);
		WEAPON_SOUND_CUSTOM(sound47);
		WEAPON_SOUND_CUSTOM(meleeSwipeSound);
		WEAPON_SOUND_CUSTOM(meleeSwipeSoundPlayer);
		WEAPON_SOUND_CUSTOM(meleeHitSound);
		WEAPON_SOUND_CUSTOM(meleeHitSoundPlayer);
		WEAPON_SOUND_CUSTOM(meleeMissSound);
		WEAPON_SOUND_CUSTOM(meleeMissSoundPlayer);
		WEAPON_SOUND_CUSTOM(sound54);
		WEAPON_SOUND_CUSTOM(sound55);
		WEAPON_SOUND_CUSTOM(sound56);
		WEAPON_SOUND_CUSTOM(sound57);
		WEAPON_SOUND_CUSTOM(sound58);
		WEAPON_SOUND_CUSTOM(sound59);
		WEAPON_SOUND_CUSTOM(sound60);
		WEAPON_SOUND_CUSTOM(sound61);
		WEAPON_SOUND_CUSTOM(sound62);
		WEAPON_SOUND_CUSTOM(sound63);
		WEAPON_SOUND_CUSTOM(sound64);
		WEAPON_SOUND_CUSTOM(sound65);
		WEAPON_SOUND_CUSTOM(nightVisionWearSound);
		WEAPON_SOUND_CUSTOM(nightVisionWearSoundPlayer);
		WEAPON_SOUND_CUSTOM(nightVisionRemoveSound);
		WEAPON_SOUND_CUSTOM(nightVisionRemoveSoundPlayer);
		WEAPON_SOUND_CUSTOM(raiseSound);
		WEAPON_SOUND_CUSTOM(raiseSoundPlayer);
		WEAPON_SOUND_CUSTOM(raiseSoundPlayerLeft);
		WEAPON_SOUND_CUSTOM(raiseSoundPlayerRight);
		WEAPON_SOUND_CUSTOM(sound74);
		WEAPON_SOUND_CUSTOM(quickRaiseSoundPlayer);
		WEAPON_SOUND_CUSTOM(quickRaiseSoundPlayerLeft);
		WEAPON_SOUND_CUSTOM(quickRaiseSoundPlayerRight);
		WEAPON_SOUND_CUSTOM(raiseSound2);
		WEAPON_SOUND_CUSTOM(sound79);
		WEAPON_SOUND_CUSTOM(sound80);
		WEAPON_SOUND_CUSTOM(sound81);
		WEAPON_SOUND_CUSTOM(putawaySound);
		WEAPON_SOUND_CUSTOM(putawaySoundPlayer);
		WEAPON_SOUND_CUSTOM(putawaySoundPlayerLeft);
		WEAPON_SOUND_CUSTOM(putawaySoundPlayerRight);
		WEAPON_SOUND_CUSTOM(sound86);
		WEAPON_SOUND_CUSTOM(sound87);
		WEAPON_SOUND_CUSTOM(adsEnterSoundPlayer);
		WEAPON_SOUND_CUSTOM(adsLeaveSoundPlayer);
		WEAPON_SOUND_CUSTOM(adsCrosshairEnemySound);
		
		if (data->bounceSound)
		{
			buf->align(7);
			dest->bounceSound = buf->write(data->bounceSound, 53);

			for (auto i = 0; i < 53; i++)
			{
				WEAPON_SOUND_CUSTOM(bounceSound[i]);
			}

			buf->clear_pointer(&dest->bounceSound);
		}

		if (data->rollingSound)
		{
			buf->align(7);
			dest->rollingSound = buf->write(data->rollingSound, 53);

			for (auto i = 0; i < 53; i++)
			{
				WEAPON_SOUND_CUSTOM(rollingSound[i]);
			}

			buf->clear_pointer(&dest->rollingSound);
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
			auto destModels = buf->write(data->worldModel, 2);

			for (auto i = 0; i < 2; i++)
			{
				if (destModels[i])
				{
					destModels[i] = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->worldModel[i]->name)
						);
				}
			}

			buf->clear_pointer(&dest->worldModel);
		}

		WEAPON_SUBASSET(worldClipModel, ASSET_TYPE_XMODEL, XModel);
		WEAPON_SUBASSET(rocketModel, ASSET_TYPE_XMODEL, XModel);
		WEAPON_SUBASSET(knifeModel, ASSET_TYPE_XMODEL, XModel);
		WEAPON_SUBASSET(worldKnifeModel, ASSET_TYPE_XMODEL, XModel);
		
		WEAPON_SUBASSET(hudIcon, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(pickupIcon, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(minimapIconFriendly, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(minimapIconEnemy, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(minimapIconNeutral, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(ammoCounterIcon, ASSET_TYPE_MATERIAL, Material);

		WEAPON_STRING(szAmmoName);
		WEAPON_STRING(szClipName);
		WEAPON_STRING(szSharedAmmoCapName);

		WEAPON_SUBASSET(physCollmap, ASSET_TYPE_PHYSCOLLMAP, PhysCollmap);
		WEAPON_SUBASSET(physPreset, ASSET_TYPE_PHYSPRESET, PhysPreset);

		WEAPON_STRING(szUseHintString);
		WEAPON_STRING(dropHintString);

		if (data->locationDamageMultipliers)
		{
			buf->align(3);
			buf->write(data->locationDamageMultipliers, 22);
			buf->clear_pointer(&dest->locationDamageMultipliers);
		}

		WEAPON_STRING(fireRumble);
		WEAPON_STRING(fireMedRumble);
		WEAPON_STRING(fireHighRumble);
		WEAPON_STRING(meleeImpactRumble);
		
		WEAPON_SUBASSET(tracerType, ASSET_TYPE_TRACER, TracerDef);
		WEAPON_SUBASSET(signatureTracerType, ASSET_TYPE_TRACER, TracerDef);

		WEAPON_SUBASSET(laserType, ASSET_TYPE_LASER, LaserDef);

		WEAPON_SOUND_CUSTOM(turretOverheatSound);
		WEAPON_SUBASSET(turretOverheatEffect, ASSET_TYPE_FX, FxEffectDef);

		WEAPON_STRING(turretBarrelSpinRumble);

		WEAPON_SOUND_CUSTOM(turretBarrelSpinMaxSnd);

		for (int i = 0; i < 4; i++)
		{
			WEAPON_SOUND_CUSTOM(turretBarrelSpinUpSnd[i]);
			WEAPON_SOUND_CUSTOM(turretBarrelSpinDownSnd[i]);
		}

		WEAPON_SOUND_CUSTOM(missileConeSoundAlias);
		WEAPON_SOUND_CUSTOM(missileConeSoundAliasAtBase);

		WEAPON_SUBASSET(stowOffsetModel, ASSET_TYPE_XMODEL, XModel);

		if (data->turretHydraulicSettings)
		{
			buf->align(3);
			auto destTurretHydraulicSettings = buf->write(data->turretHydraulicSettings);

#define HYDRAULIC_SOUND_CUSTOM(__data__, __dest__) \
		if (__data__) \
		{ \
			buf->align(7); \
			auto* ptr = reinterpret_cast<const char**>(buf->write(&buf->data_following)); \
			*ptr = buf->write_str(__data__->name); \
			buf->clear_pointer(&__dest__); \
		}

			HYDRAULIC_SOUND_CUSTOM(data->turretHydraulicSettings->verticalSound, destTurretHydraulicSettings->verticalSound);
			HYDRAULIC_SOUND_CUSTOM(data->turretHydraulicSettings->verticalStopSound, destTurretHydraulicSettings->verticalStopSound);
			HYDRAULIC_SOUND_CUSTOM(data->turretHydraulicSettings->horizontalSound, destTurretHydraulicSettings->horizontalSound);
			HYDRAULIC_SOUND_CUSTOM(data->turretHydraulicSettings->horizontalStopSound, destTurretHydraulicSettings->horizontalStopSound);
#undef HYDRAULIC_SOUND_CUSTOM

			buf->clear_pointer(&dest->turretHydraulicSettings);
		}

		WEAPON_SUBASSET(overlay.shader, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(overlay.shaderLowRes, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(overlay.shaderEMP, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(overlay.shaderEMPLowRes, ASSET_TYPE_MATERIAL, Material);
		
		WEAPON_SUBASSET(killIcon, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(dpadIcon, ASSET_TYPE_MATERIAL, Material);
		WEAPON_SUBASSET(hudProximityWarningIcon, ASSET_TYPE_MATERIAL, Material);

		WEAPON_STRING(projectileName);

		WEAPON_SUBASSET(projectileModel, ASSET_TYPE_XMODEL, XModel);

		WEAPON_SUBASSET(projExplosionEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(projDudEffect, ASSET_TYPE_FX, FxEffectDef);

		WEAPON_SOUND_CUSTOM(projExplosionSound);
		WEAPON_SOUND_CUSTOM(projDudSound);

		if (data->parallelBounce)
		{
			buf->align(3);
			buf->write(data->parallelBounce, 53);
			buf->clear_pointer(&dest->parallelBounce);
		}

		if (data->perpendicularBounce)
		{
			buf->align(3);
			buf->write(data->perpendicularBounce, 53);
			buf->clear_pointer(&dest->perpendicularBounce);
		}

		WEAPON_SUBASSET(projTrailEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(projBeaconEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(projIgnitionEffect, ASSET_TYPE_FX, FxEffectDef);

		WEAPON_SOUND_CUSTOM(projIgnitionSound);

		for (auto i = 0; i < 2; i++)
		{
			WEAPON_STRING(accuracyGraphName[i]);

			if (data->accuracyGraphKnots[i])
			{
				buf->align(3);
				buf->write(data->accuracyGraphKnots[i], data->accuracyGraphKnotCount[i]);
				buf->clear_pointer(&dest->accuracyGraphKnots[i]);
			}

			if (data->originalAccuracyGraphKnots[i])
			{
				buf->align(3);
				buf->write(data->originalAccuracyGraphKnots[i], data->accuracyGraphKnotCount[i]);
				buf->clear_pointer(&dest->originalAccuracyGraphKnots[i]);
			}
		}

		WEAPON_STRING(szScript);

		buf->pop_stream();
	}

	json dump_accuracy_graph(WeaponDef* asset)
	{
		json data;

		for (auto i = 0; i < 2; i++)
		{
			if (asset->accuracyGraphName[i])
			{
				data["accuracyGraphName"][i] = asset->accuracyGraphName[i];
			}
			else
			{
				data["accuracyGraphName"][i] = "";
			}

			for (auto o = 0; o < asset->accuracyGraphKnotCount[i]; o++)
			{
				if (asset->accuracyGraphKnots[i])
				{
					data["accuracyGraphKnots"][i][o][0] = asset->accuracyGraphKnots[i][o][0];
					data["accuracyGraphKnots"][i][o][1] = asset->accuracyGraphKnots[i][o][1];
				}
				else
				{
					data["accuracyGraphKnots"] = nullptr;
				}
				if (asset->originalAccuracyGraphKnots[i])
				{
					data["originalAccuracyGraphKnots"][i][o][0] = asset->originalAccuracyGraphKnots[i][o][0];
					data["originalAccuracyGraphKnots"][i][o][1] = asset->originalAccuracyGraphKnots[i][o][1];
				}
				else
				{
					data["originalAccuracyGraphKnots"] = nullptr;
				}
			}
		}

		return data;
	}

	void weapon_def::dump(WeaponDef* asset)
	{
		const auto path = "weapons\\"s + asset->name + ".json"s;

		ordered_json data;
		json_reflect::write<weapon_fields>(data, *asset, json_options);

		write_anim_array(data["szXAnimsRightHanded"], asset->szXAnimsRightHanded);
		write_anim_array(data["szXAnimsLeftHanded"], asset->szXAnimsLeftHanded);
		write_anim_array(data["szXAnims"], asset->szXAnims);

		data["stateTimers"] = write_state_timers(asset->stateTimers);
		data["stateTimersAkimbo"] = write_state_timers(asset->akimboStateTimers);

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

		for (auto i = 0u; i < asset->numNotetrackOverrides; i++)
		{
			data["notetrackOverrides"][i]["attachment"] = asset->notetrackOverrides[i].attachment;

			for (auto j = 0u; j < 36; j++)
			{
				data["notetrackOverrides"][i]["notetrackSoundMap"][j]["Key"] = (asset->notetrackOverrides[i].notetrackSoundMapKeys[j])
					? SL_ConvertToString(asset->notetrackOverrides[i].notetrackSoundMapKeys[j])
					: "";

				data["notetrackOverrides"][i]["notetrackSoundMap"][j]["Value"] = (asset->notetrackOverrides[i].notetrackSoundMapValues[j])
					? SL_ConvertToString(asset->notetrackOverrides[i].notetrackSoundMapValues[j])
					: "";
			}
		}

		for (auto i = 0; i < 36; i++)
		{
			data["notetrackSoundMap"][i]["Key"] = asset->notetrackSoundMapKeys && asset->notetrackSoundMapKeys[i]
				? SL_ConvertToString(asset->notetrackSoundMapKeys[i])
				: "";

			data["notetrackSoundMap"][i]["Value"] = asset->notetrackSoundMapValues && asset->notetrackSoundMapValues[i]
				? SL_ConvertToString(asset->notetrackSoundMapValues[i])
				: "";
		}

		for (auto i = 0; i < 16; i++)
		{
			data["notetrackRumbleMap"][i]["Key"] = asset->notetrackRumbleMapKeys && asset->notetrackRumbleMapKeys[i]
				? SL_ConvertToString(asset->notetrackRumbleMapKeys[i])
				: "";

			data["notetrackRumbleMap"][i]["Value"] = asset->notetrackRumbleMapValues && asset->notetrackRumbleMapValues[i]
				? SL_ConvertToString(asset->notetrackRumbleMapValues[i])
				: "";
		}

		for (auto i = 0; i < 16; i++)
		{
			data["notetrackFXMap"][i]["Key"] = asset->notetrackFXMapKeys && asset->notetrackFXMapKeys[i]
				? SL_ConvertToString(asset->notetrackFXMapKeys[i])
				: "";

			data["notetrackFXMap"][i]["Value"] = asset->notetrackFXMapValues && asset->notetrackFXMapValues[i]
				? asset->notetrackFXMapValues[i]->name
				: "";

			data["notetrackFXMap"][i]["Tag"] = asset->notetrackFXMapTagValues && asset->notetrackFXMapTagValues[i]
				? SL_ConvertToString(asset->notetrackFXMapTagValues[i])
				: "";
		}

		for (auto i = 0; i < 16; i++)
		{
			data["notetrackHideTag"][i]["Key"] = asset->notetrackHideTagKeys && asset->notetrackHideTagKeys[i]
				? SL_ConvertToString(asset->notetrackHideTagKeys[i])
				: "";

			data["notetrackHideTag"][i]["Value"] = asset->notetrackHideTagValues && asset->notetrackHideTagValues[i]
				? asset->notetrackHideTagValues[i]
				: false;

			data["notetrackHideTag"][i]["Tag"] = asset->notetrackHideTagTagValues && asset->notetrackHideTagTagValues[i]
				? SL_ConvertToString(asset->notetrackHideTagTagValues[i])
				: "";
		}

		data["accuracy_graph"] = dump_accuracy_graph(asset);
		data["stowTag"] = SL_ConvertToString(asset->stowTag) ? SL_ConvertToString(asset->stowTag) : "";

		std::string json = data.dump(4);

		auto file = filesystem::file(path);
		file.open("wb");
		file.write(json.data(), json.size(), 1);
		file.close();
	}
}
