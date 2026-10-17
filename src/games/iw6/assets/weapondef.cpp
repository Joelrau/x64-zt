#include "std_include.hpp"
#include "weapondef.hpp"

#include "io/json_reflect.hpp"

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

	namespace
	{
		constexpr auto overlay_fields = glz::tuple
		{
			&ADSOverlay::shader,
			&ADSOverlay::shaderLowRes,
			&ADSOverlay::shaderEMP,
			&ADSOverlay::shaderEMPLowRes,
			&ADSOverlay::reticle,
			&ADSOverlay::width,
			&ADSOverlay::height,
			&ADSOverlay::widthSplitscreen,
			&ADSOverlay::heightSplitscreen,
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
			&WeaponDef::fireSound,
			&WeaponDef::fireSoundPlayer,
			&WeaponDef::fireSoundPlayerAkimbo,
			&WeaponDef::fireLoopSound,
			&WeaponDef::fireLoopSoundPlayer,
			&WeaponDef::fireStopSound,
			&WeaponDef::fireStopSoundPlayer,
			&WeaponDef::fireLastSound,
			&WeaponDef::fireLastSoundPlayer,
			&WeaponDef::emptyFireSound,
			&WeaponDef::emptyFireSoundPlayer,
			&WeaponDef::meleeSwipeSound,
			&WeaponDef::meleeSwipeSoundPlayer,
			&WeaponDef::meleeHitSound,
			&WeaponDef::meleeMissSound,
			&WeaponDef::rechamberSound,
			&WeaponDef::rechamberSoundPlayer,
			&WeaponDef::reloadSound,
			&WeaponDef::reloadSoundPlayer,
			&WeaponDef::reloadEmptySound,
			&WeaponDef::reloadEmptySoundPlayer,
			&WeaponDef::reloadStartSound,
			&WeaponDef::reloadStartSoundPlayer,
			&WeaponDef::reloadEndSound,
			&WeaponDef::reloadEndSoundPlayer,
			&WeaponDef::detonateSound,
			&WeaponDef::detonateSoundPlayer,
			&WeaponDef::nightVisionWearSound,
			&WeaponDef::nightVisionWearSoundPlayer,
			&WeaponDef::nightVisionRemoveSound,
			&WeaponDef::nightVisionRemoveSoundPlayer,
			&WeaponDef::altSwitchSound,
			&WeaponDef::altSwitchSoundPlayer,
			&WeaponDef::raiseSound,
			&WeaponDef::raiseSoundPlayer,
			&WeaponDef::firstRaiseSound,
			&WeaponDef::firstRaiseSoundPlayer,
			&WeaponDef::putawaySound,
			&WeaponDef::putawaySoundPlayer,
			&WeaponDef::scanSound,
			&WeaponDef::changeVariableZoomSound,
		};

		constexpr auto weapon_fields = glz::tuple
		{
			json_reflect::counted(&WeaponDef::gunXModel, 64),
			json_reflect::counted(&WeaponDef::worldModel, 64),
			json_reflect::counted(&WeaponDef::camoMaterialTarget, 63),
			json_reflect::manual("szXAnimsRightHanded"),
			json_reflect::manual("szXAnimsLeftHanded"),
			json_reflect::manual("notetrackSoundMapKeys"),
			json_reflect::manual("notetrackSoundMapValues"),
			json_reflect::manual("notetrackRumbleMapKeys"),
			json_reflect::manual("notetrackRumbleMapValues"),
			json_reflect::manual("notetrackFXMapKeys"),
			json_reflect::manual("notetrackFXMapValues"),
			json_reflect::manual("notetrackFXMapTagValues"),
			json_reflect::group("sounds", weapon_sound_fields),
			json_reflect::counted(&WeaponDef::bounceSound, 31),
			json_reflect::counted(&WeaponDef::rollingSound, 31),
			&WeaponDef::turretBarrelSpinUpSnd,
			&WeaponDef::turretBarrelSpinDownSnd,
			&WeaponDef::szOverlayName,
			&WeaponDef::handXModel,
			&WeaponDef::camoWorldModel,
			&WeaponDef::camoViewModel,
			&WeaponDef::camoWorldModelMaterialOverride,
			&WeaponDef::camoViewModelMaterialOverride,
			&WeaponDef::szModeName,
			&WeaponDef::playerAnimType,
			&WeaponDef::weapType,
			&WeaponDef::weapClass,
			&WeaponDef::penetrateType,
			&WeaponDef::inventoryType,
			&WeaponDef::fireType,
			&WeaponDef::burstFireCooldown,
			&WeaponDef::offhandClass,
			&WeaponDef::stance,
			&WeaponDef::rattleSoundType,
			&WeaponDef::viewFlashEffect,
			&WeaponDef::worldFlashEffect,
			&WeaponDef::viewFlashADSEffect,
			&WeaponDef::viewShellEjectEffect,
			&WeaponDef::worldShellEjectEffect,
			&WeaponDef::viewLastShotEjectEffect,
			&WeaponDef::worldLastShotEjectEffect,
			&WeaponDef::viewMagEjectEffect,
			&WeaponDef::reticleCenter,
			&WeaponDef::reticleSide,
			&WeaponDef::iReticleCenterSize,
			&WeaponDef::iReticleSideSize,
			&WeaponDef::iReticleMinOfs,
			&WeaponDef::activeReticleType,
			&WeaponDef::vStandMove,
			&WeaponDef::vStandRot,
			&WeaponDef::strafeMove,
			&WeaponDef::strafeRot,
			&WeaponDef::vDuckedOfs,
			&WeaponDef::vDuckedMove,
			&WeaponDef::vDuckedRot,
			&WeaponDef::vProneOfs,
			&WeaponDef::vProneMove,
			&WeaponDef::vProneRot,
			&WeaponDef::fPosMoveRate,
			&WeaponDef::fPosProneMoveRate,
			&WeaponDef::fStandMoveMinSpeed,
			&WeaponDef::fDuckedMoveMinSpeed,
			&WeaponDef::fProneMoveMinSpeed,
			&WeaponDef::fPosRotRate,
			&WeaponDef::fPosProneRotRate,
			&WeaponDef::fStandRotMinSpeed,
			&WeaponDef::fDuckedRotMinSpeed,
			&WeaponDef::fProneRotMinSpeed,
			&WeaponDef::worldClipModel,
			&WeaponDef::rocketModel,
			&WeaponDef::knifeModel,
			&WeaponDef::worldKnifeModel,
			&WeaponDef::hudIcon,
			&WeaponDef::hudIconRatio,
			&WeaponDef::pickupIcon,
			&WeaponDef::pickupIconRatio,
			&WeaponDef::ammoCounterIcon,
			&WeaponDef::ammoCounterIconRatio,
			&WeaponDef::ammoCounterClip,
			&WeaponDef::iStartAmmo,
			&WeaponDef::szAmmoName,
			&WeaponDef::iAmmoIndex,
			&WeaponDef::szClipName,
			&WeaponDef::iClipIndex,
			&WeaponDef::iMaxAmmo,
			&WeaponDef::shotCount,
			&WeaponDef::szSharedAmmoCapName,
			&WeaponDef::iSharedAmmoCapIndex,
			&WeaponDef::iSharedAmmoCap,
			&WeaponDef::damage,
			&WeaponDef::playerDamage,
			&WeaponDef::iMeleeDamage,
			&WeaponDef::iDamageType,
			json_reflect::manual("stateTimers"),
			json_reflect::manual("akimboStateTimers"),
			&WeaponDef::autoAimRange,
			&WeaponDef::aimAssistRange,
			&WeaponDef::aimAssistRangeAds,
			&WeaponDef::aimPadding,
			&WeaponDef::enemyCrosshairRange,
			&WeaponDef::moveSpeedScale,
			&WeaponDef::adsMoveSpeedScale,
			&WeaponDef::sprintDurationScale,
			&WeaponDef::fAdsZoomInFrac,
			&WeaponDef::fAdsZoomOutFrac,
			json_reflect::nested(&WeaponDef::overlay, overlay_fields),
			&WeaponDef::overlayInterface,
			&WeaponDef::fAdsBobFactor,
			&WeaponDef::fAdsViewBobMult,
			&WeaponDef::fHipSpreadStandMin,
			&WeaponDef::fHipSpreadDuckedMin,
			&WeaponDef::fHipSpreadProneMin,
			&WeaponDef::hipSpreadStandMax,
			&WeaponDef::hipSpreadDuckedMax,
			&WeaponDef::hipSpreadProneMax,
			&WeaponDef::fHipSpreadDecayRate,
			&WeaponDef::fHipSpreadFireAdd,
			&WeaponDef::fHipSpreadTurnAdd,
			&WeaponDef::fHipSpreadMoveAdd,
			&WeaponDef::fHipSpreadDuckedDecay,
			&WeaponDef::fHipSpreadProneDecay,
			&WeaponDef::fHipReticleSidePos,
			&WeaponDef::fAdsIdleAmount,
			&WeaponDef::fHipIdleAmount,
			&WeaponDef::adsIdleSpeed,
			&WeaponDef::hipIdleSpeed,
			&WeaponDef::fIdleCrouchFactor,
			&WeaponDef::fIdleProneFactor,
			&WeaponDef::fGunMaxPitch,
			&WeaponDef::fGunMaxYaw,
			&WeaponDef::adsIdleLerpStartTime,
			&WeaponDef::adsIdleLerpTime,
			&WeaponDef::swayMaxAngleSteadyAim,
			&WeaponDef::swayMaxAngle,
			&WeaponDef::swayLerpSpeed,
			&WeaponDef::swayPitchScale,
			&WeaponDef::swayYawScale,
			&WeaponDef::swayHorizScale,
			&WeaponDef::swayVertScale,
			&WeaponDef::swayShellShockScale,
			&WeaponDef::adsSwayMaxAngle,
			&WeaponDef::adsSwayLerpSpeed,
			&WeaponDef::adsSwayPitchScale,
			&WeaponDef::adsSwayYawScale,
			&WeaponDef::adsSwayHorizScale,
			&WeaponDef::adsSwayVertScale,
			&WeaponDef::adsFireRateScale,
			&WeaponDef::adsDamageRangeScale,
			&WeaponDef::adsFireAnimFrac,
			&WeaponDef::physCollmap,
			&WeaponDef::dualWieldViewModelOffset,
			&WeaponDef::fScopeDriftDelay,
			&WeaponDef::fScopeDriftLerpInTime,
			&WeaponDef::fScopeDriftSteadyTime,
			&WeaponDef::fScopeDriftLerpOutTime,
			&WeaponDef::fScopeDriftSteadyFactor,
			&WeaponDef::fScopeDriftUnsteadyFactor,
			&WeaponDef::killIconRatio,
			&WeaponDef::iReloadAmmoAdd,
			&WeaponDef::iReloadStartAdd,
			&WeaponDef::ammoDropStockMin,
			&WeaponDef::ammoDropClipPercentMin,
			&WeaponDef::ammoDropClipPercentMax,
			&WeaponDef::iExplosionRadius,
			&WeaponDef::iExplosionRadiusMin,
			&WeaponDef::iExplosionInnerDamage,
			&WeaponDef::iExplosionOuterDamage,
			&WeaponDef::damageConeAngle,
			&WeaponDef::bulletExplDmgMult,
			&WeaponDef::bulletExplRadiusMult,
			&WeaponDef::iProjectileSpeed,
			&WeaponDef::iProjectileSpeedUp,
			&WeaponDef::iProjectileSpeedForward,
			&WeaponDef::iProjectileActivateDist,
			&WeaponDef::projLifetime,
			&WeaponDef::timeToAccelerate,
			&WeaponDef::projectileCurvature,
			&WeaponDef::projectileName,
			&WeaponDef::projectileModel,
			&WeaponDef::projExplosion,
			&WeaponDef::projExplosionEffect,
			&WeaponDef::projDudEffect,
			&WeaponDef::projExplosionSound,
			&WeaponDef::projDudSound,
			&WeaponDef::stickiness,
			&WeaponDef::lowAmmoWarningThreshold,
			&WeaponDef::ricochetChance,
			&WeaponDef::riotShieldEnableDamage,
			&WeaponDef::riotShieldHealth,
			&WeaponDef::riotShieldDamageMult,
			&WeaponDef::projTrailEffect,
			&WeaponDef::projBeaconEffect,
			&WeaponDef::vProjectileColor,
			&WeaponDef::guidedMissileType,
			&WeaponDef::maxSteeringAccel,
			&WeaponDef::projIgnitionDelay,
			&WeaponDef::projIgnitionEffect,
			&WeaponDef::projIgnitionSound,
			&WeaponDef::fAdsAimPitch,
			&WeaponDef::fAdsCrosshairInFrac,
			&WeaponDef::fAdsCrosshairOutFrac,
			&WeaponDef::adsShouldShowCrosshair,
			&WeaponDef::adsGunKickReducedKickBullets,
			&WeaponDef::adsGunKickReducedKickPercent,
			&WeaponDef::fAdsGunKickPitchMin,
			&WeaponDef::fAdsGunKickPitchMax,
			&WeaponDef::fAdsGunKickYawMin,
			&WeaponDef::fAdsGunKickYawMax,
			&WeaponDef::fAdsGunKickMagMin,
			&WeaponDef::fAdsGunKickAccel,
			&WeaponDef::fAdsGunKickSpeedMax,
			&WeaponDef::fAdsGunKickSpeedDecay,
			&WeaponDef::fAdsGunKickStaticDecay,
			&WeaponDef::fAdsViewKickPitchMin,
			&WeaponDef::fAdsViewKickPitchMax,
			&WeaponDef::fAdsViewKickYawMin,
			&WeaponDef::fAdsViewKickYawMax,
			&WeaponDef::fAdsViewKickMagMin,
			&WeaponDef::fAdsViewScatterMin,
			&WeaponDef::fAdsViewScatterMax,
			&WeaponDef::fAdsSpread,
			&WeaponDef::hipGunKickReducedKickBullets,
			&WeaponDef::hipGunKickReducedKickPercent,
			&WeaponDef::fHipGunKickPitchMin,
			&WeaponDef::fHipGunKickPitchMax,
			&WeaponDef::fHipGunKickYawMin,
			&WeaponDef::fHipGunKickYawMax,
			&WeaponDef::fHipGunKickMagMin,
			&WeaponDef::fHipGunKickAccel,
			&WeaponDef::fHipGunKickSpeedMax,
			&WeaponDef::fHipGunKickSpeedDecay,
			&WeaponDef::fHipGunKickStaticDecay,
			&WeaponDef::fHipViewKickPitchMin,
			&WeaponDef::fHipViewKickPitchMax,
			&WeaponDef::fHipViewKickYawMin,
			&WeaponDef::fHipViewKickYawMax,
			&WeaponDef::fHipViewKickMagMin,
			&WeaponDef::fHipViewScatterMin,
			&WeaponDef::fHipViewScatterMax,
			&WeaponDef::fightDist,
			&WeaponDef::maxDist,
			json_reflect::manual("accuracyGraphName"),
			&WeaponDef::iPositionReloadTransTime,
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
			&WeaponDef::fAnimHorRotateInc,
			&WeaponDef::fPlayerPositionDist,
			&WeaponDef::szUseHintString,
			&WeaponDef::dropHintString,
			&WeaponDef::iUseHintStringIndex,
			&WeaponDef::dropHintStringIndex,
			&WeaponDef::horizViewJitter,
			&WeaponDef::vertViewJitter,
			&WeaponDef::scanSpeed,
			&WeaponDef::scanAccel,
			&WeaponDef::scanPauseTime,
			&WeaponDef::szScript,
			&WeaponDef::fOOPosAnimLength,
			&WeaponDef::minDamage,
			&WeaponDef::minPlayerDamage,
			&WeaponDef::fMaxDamageRange,
			&WeaponDef::fMinDamageRange,
			&WeaponDef::destabilizationRateTime,
			&WeaponDef::destabilizationCurvatureMax,
			&WeaponDef::destabilizeDistance,
			json_reflect::counted(&WeaponDef::locationDamageMultipliers, 22),
			&WeaponDef::fireRumble,
			&WeaponDef::meleeImpactRumble,
			&WeaponDef::tracerType,
			&WeaponDef::turretADSEnabled,
			&WeaponDef::turretADSTime,
			&WeaponDef::turretFov,
			&WeaponDef::turretFovADS,
			&WeaponDef::turretScopeZoomRate,
			&WeaponDef::turretScopeZoomMin,
			&WeaponDef::turretScopeZoomMax,
			&WeaponDef::turretOverheatUpRate,
			&WeaponDef::turretOverheatDownRate,
			&WeaponDef::turretOverheatPenalty,
			&WeaponDef::turretOverheatSound,
			&WeaponDef::turretOverheatEffect,
			&WeaponDef::turretBarrelSpinRumble,
			&WeaponDef::turretBarrelSpinSpeed,
			&WeaponDef::turretBarrelSpinUpTime,
			&WeaponDef::turretBarrelSpinDownTime,
			&WeaponDef::turretBarrelSpinMaxSnd,
			&WeaponDef::missileConeSoundAlias,
			&WeaponDef::missileConeSoundAliasAtBase,
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
			json_reflect::manual("knifeAttachTagOverride"),
			&WeaponDef::knifeAlwaysAttached,
			&WeaponDef::meleeOverrideValues,
			&WeaponDef::aim_automelee_lerp,
			&WeaponDef::aim_automelee_range,
			&WeaponDef::aim_automelee_region_height,
			&WeaponDef::aim_automelee_region_width,
			&WeaponDef::player_meleeHeight,
			&WeaponDef::player_meleeRange,
			&WeaponDef::player_meleeWidth,
			&WeaponDef::sharedAmmo,
			&WeaponDef::lockonSupported,
			&WeaponDef::requireLockonToFire,
			&WeaponDef::isAirburstWeapon,
			&WeaponDef::bigExplosion,
			&WeaponDef::noAdsWhenMagEmpty,
			&WeaponDef::avoidDropCleanup,
			&WeaponDef::inheritsPerks,
			&WeaponDef::crosshairColorChange,
			&WeaponDef::bRifleBullet,
			&WeaponDef::armorPiercing,
			&WeaponDef::bBoltAction,
			&WeaponDef::aimDownSight,
			&WeaponDef::canHoldBreath,
			&WeaponDef::meleeOnly,
			&WeaponDef::canVariableZoom,
			&WeaponDef::bRechamberWhileAds,
			&WeaponDef::bBulletExplosiveDamage,
			&WeaponDef::bCookOffHold,
			&WeaponDef::reticleSpin45,
			&WeaponDef::bClipOnly,
			&WeaponDef::noAmmoPickup,
			&WeaponDef::adsFireOnly,
			&WeaponDef::cancelAutoHolsterWhenEmpty,
			&WeaponDef::disableSwitchToWhenEmpty,
			&WeaponDef::suppressAmmoReserveDisplay,
			&WeaponDef::laserSightDuringNightvision,
			&WeaponDef::markableViewmodel,
			&WeaponDef::noDualWield,
			&WeaponDef::flipKillIcon,
			&WeaponDef::bNoPartialReload,
			&WeaponDef::bSegmentedReload,
			&WeaponDef::bMultipleReload,
			&WeaponDef::blocksProne,
			&WeaponDef::silenced,
			&WeaponDef::isRollingGrenade,
			&WeaponDef::projExplosionEffectForceNormalUp,
			&WeaponDef::projExplosionEffectInheritParentDirection,
			&WeaponDef::bProjImpactExplode,
			&WeaponDef::bProjTrajectoryEvents,
			&WeaponDef::bProjWhizByEnabled,
			&WeaponDef::stickToPlayers,
			&WeaponDef::stickToVehicles,
			&WeaponDef::stickToTurrets,
			&WeaponDef::thrownSideways,
			&WeaponDef::hasDetonator,
			&WeaponDef::disableFiring,
			&WeaponDef::timedDetonation,
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
			&WeaponDef::useScopeDrift,
			&WeaponDef::alwaysShatterGlassOnImpact,
			&WeaponDef::oldWeapon,
			json_reflect::manual("stowTag"),
			&WeaponDef::stowOffsetModel,
		};

		constexpr auto complete_fields = glz::tuple
		{
			json_reflect::manual("hideTags"),
			json_reflect::counted(&WeaponCompleteDef::scopes, 6),
			json_reflect::counted(&WeaponCompleteDef::underBarrels, 3),
			json_reflect::counted(&WeaponCompleteDef::others, 8),
			json_reflect::manual("szXAnims"),
			json_reflect::counted(&WeaponCompleteDef::animOverrides, &WeaponCompleteDef::numAnimOverrides, anim_override_fields),
			json_reflect::counted(&WeaponCompleteDef::soundOverrides, &WeaponCompleteDef::numSoundOverrides, sound_override_fields),
			json_reflect::counted(&WeaponCompleteDef::fxOverrides, &WeaponCompleteDef::numFXOverrides, fx_override_fields),
			json_reflect::counted(&WeaponCompleteDef::reloadOverrides, &WeaponCompleteDef::numReloadStateTimerOverrides, reload_override_fields),
			json_reflect::manual("notetrackOverrides"),
			&WeaponCompleteDef::szInternalName,
			&WeaponCompleteDef::szDisplayName,
			&WeaponCompleteDef::numAnimOverrides,
			&WeaponCompleteDef::numSoundOverrides,
			&WeaponCompleteDef::numFXOverrides,
			&WeaponCompleteDef::numReloadStateTimerOverrides,
			&WeaponCompleteDef::numNotetrackOverrides,
			&WeaponCompleteDef::fAdsZoomFov,
			&WeaponCompleteDef::iAdsTransInTime,
			&WeaponCompleteDef::iAdsTransOutTime,
			&WeaponCompleteDef::iClipSize,
			&WeaponCompleteDef::impactType,
			&WeaponCompleteDef::iFireTime,
			&WeaponCompleteDef::iFireTimeAkimbo,
			&WeaponCompleteDef::dpadIconRatio,
			&WeaponCompleteDef::penetrateMultiplier,
			&WeaponCompleteDef::fAdsViewKickCenterSpeed,
			&WeaponCompleteDef::fHipViewKickCenterSpeed,
			&WeaponCompleteDef::szAltWeaponName,
			&WeaponCompleteDef::altWeapon,
			&WeaponCompleteDef::iAltRaiseTime,
			&WeaponCompleteDef::iAltRaiseTimeAkimbo,
			&WeaponCompleteDef::killIcon,
			&WeaponCompleteDef::dpadIcon,
			&WeaponCompleteDef::fireAnimLength,
			&WeaponCompleteDef::fireAnimLengthAkimbo,
			&WeaponCompleteDef::iFirstRaiseTime,
			&WeaponCompleteDef::iFirstRaiseTimeAkimbo,
			&WeaponCompleteDef::ammoDropStockMax,
			&WeaponCompleteDef::adsDofStart,
			&WeaponCompleteDef::adsDofEnd,
			&WeaponCompleteDef::accuracyGraphKnotCount,
			&WeaponCompleteDef::motionTracker,
			&WeaponCompleteDef::enhanced,
			&WeaponCompleteDef::dpadIconShowsAmmo,
			&WeaponCompleteDef::szAdsrBaseSetting,
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

		void write_anim_array(json& data, XAnimParts** anims)
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
	}

	WeaponDef* weapon_def::parse_weapondef(json& data, WeaponCompleteDef* baseAsset, zone_memory* mem)
	{
		auto weapon = mem->allocate<WeaponDef>();

		if (baseAsset)
		{
			memcpy(weapon, baseAsset->weapDef, sizeof WeaponDef);
		}

		json_reflect::read<weapon_fields>(data, *weapon, mem, json_options);

		read_anim_array(data["szXAnimsRightHanded"], weapon->szXAnimsRightHanded, mem);
		read_anim_array(data["szXAnimsLeftHanded"], weapon->szXAnimsLeftHanded, mem);

		json_reflect::read(data["stateTimers"], weapon->stateTimers, mem);
		json_reflect::read(data["akimboStateTimers"], weapon->akimboStateTimers, mem);

		if (!data["knifeAttachTagOverride"].is_null())
		{
			auto knifeAttachTagOverride = data["knifeAttachTagOverride"].get<std::string>();
			this->add_script_string(&weapon->knifeAttachTagOverride, mem->duplicate_string(knifeAttachTagOverride));
		}

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
			json_reflect::read(data["notetrackFXMapValues"][i], weapon->notetrackFXMapValues[i], mem);
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
			if (baseAsset)
			{
				memcpy(weapon, baseAsset, sizeof WeaponCompleteDef);
			}
		}
		else
		{
			ZONETOOL_WARNING("No base asset is defined for weapon %s, stuff might go wrong!", name.data());
		}

		json_reflect::read<complete_fields>(data, *weapon, mem, json_options);

		weapon->weapDef = parse_weapondef(data["weapDef"], baseAsset, mem);
		read_anim_array(data["szXAnims"], weapon->szXAnims, mem);

		weapon->hideTags = mem->allocate<scr_string_t>(32);
		for (auto i = 0u; i < 32; i++)
		{
			auto script_str = data["hideTags"][i].get<std::string>();
			this->add_script_string(&weapon->hideTags[i], mem->duplicate_string(script_str));
		}

		if (weapon->numNotetrackOverrides)
		{
			weapon->notetrackOverrides = mem->allocate<NoteTrackToSoundEntry>(weapon->numNotetrackOverrides);
			for (auto i = 0u; i < weapon->numNotetrackOverrides; i++)
			{
				weapon->notetrackOverrides[i].attachment = data["notetrackOverrides"][i]["attachment"].get<int>();
				weapon->notetrackOverrides[i].notetrackSoundMapKeys = mem->allocate<scr_string_t>(24);
				weapon->notetrackOverrides[i].notetrackSoundMapValues = mem->allocate<scr_string_t>(24);
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

		if (data->szDisplayName)
		{
			zone->add_asset_of_type(ASSET_TYPE_LOCALIZE_ENTRY, data->szDisplayName);
		}

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

	namespace
	{
		constexpr auto weapon_sound_count = 48;
		static_assert(offsetof(WeaponDef, changeVariableZoomSound) == offsetof(WeaponDef, pickupSound) + (weapon_sound_count - 1) * sizeof(snd_alias_list_t*));

		void read_weapon_def(zone_reader& reader, const WeaponCompleteDef* complete, WeaponDef* asset)
		{
			reader.read_string(asset->szOverlayName);
			reader.read_asset_array(ASSET_TYPE_XMODEL, asset->gunXModel, 64);
			reader.read_asset(ASSET_TYPE_XMODEL, asset->handXModel);
			reader.read_asset(ASSET_TYPE_XMODEL, asset->camoWorldModel);
			reader.read_asset(ASSET_TYPE_XMODEL, asset->camoViewModel);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->camoWorldModelMaterialOverride);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->camoViewModelMaterialOverride);
			reader.read_asset_array(ASSET_TYPE_MATERIAL, asset->camoMaterialTarget, 63);
			reader.read_asset_array(ASSET_TYPE_XANIMPARTS, asset->szXAnimsRightHanded, NUM_WEAP_ANIMS);
			reader.read_asset_array(ASSET_TYPE_XANIMPARTS, asset->szXAnimsLeftHanded, NUM_WEAP_ANIMS);
			reader.read_string(asset->szModeName);
			reader.read_script_string_array(asset->notetrackSoundMapKeys, 24);
			reader.read_script_string_array(asset->notetrackSoundMapValues, 24);
			reader.read_script_string_array(asset->notetrackRumbleMapKeys, 16);
			reader.read_script_string_array(asset->notetrackRumbleMapValues, 16);
			reader.read_script_string_array(asset->notetrackFXMapKeys, 16);
			reader.read_asset_array(ASSET_TYPE_FX, asset->notetrackFXMapValues, 16);
			reader.read_script_string_array(asset->notetrackFXMapTagValues, 16);
			reader.read_asset(ASSET_TYPE_FX, asset->viewFlashEffect);
			reader.read_asset(ASSET_TYPE_FX, asset->worldFlashEffect);
			reader.read_asset(ASSET_TYPE_FX, asset->viewFlashADSEffect);

			const auto sounds = &asset->pickupSound;
			for (auto i = 0; i < weapon_sound_count; i++)
			{
				reader.read_name_reference(sounds[i]);
			}

			reader.read_name_reference_array(asset->bounceSound, 31);
			reader.read_name_reference_array(asset->rollingSound, 31);
			reader.read_asset(ASSET_TYPE_FX, asset->viewShellEjectEffect);
			reader.read_asset(ASSET_TYPE_FX, asset->worldShellEjectEffect);
			reader.read_asset(ASSET_TYPE_FX, asset->viewLastShotEjectEffect);
			reader.read_asset(ASSET_TYPE_FX, asset->worldLastShotEjectEffect);
			reader.read_asset(ASSET_TYPE_FX, asset->viewMagEjectEffect);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->reticleCenter);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->reticleSide);
			reader.read_asset_array(ASSET_TYPE_XMODEL, asset->worldModel, 64);
			reader.read_asset(ASSET_TYPE_XMODEL, asset->worldClipModel);
			reader.read_asset(ASSET_TYPE_XMODEL, asset->rocketModel);
			reader.read_asset(ASSET_TYPE_XMODEL, asset->knifeModel);
			reader.read_asset(ASSET_TYPE_XMODEL, asset->worldKnifeModel);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->hudIcon);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->pickupIcon);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->ammoCounterIcon);
			reader.read_string(asset->szAmmoName);
			reader.read_string(asset->szClipName);
			reader.read_string(asset->szSharedAmmoCapName);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->overlay.shader);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->overlay.shaderLowRes);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->overlay.shaderEMP);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->overlay.shaderEMPLowRes);
			reader.read_asset(ASSET_TYPE_PHYSCOLLMAP, asset->physCollmap);
			reader.read_string(asset->projectileName);
			reader.read_asset(ASSET_TYPE_XMODEL, asset->projectileModel);
			reader.read_asset(ASSET_TYPE_FX, asset->projExplosionEffect);
			reader.read_asset(ASSET_TYPE_FX, asset->projDudEffect);
			reader.read_name_reference(asset->projExplosionSound);
			reader.read_name_reference(asset->projDudSound);
			reader.read_array(asset->parallelBounce, 3, 31);
			reader.read_array(asset->perpendicularBounce, 3, 31);
			reader.read_asset(ASSET_TYPE_FX, asset->projTrailEffect);
			reader.read_asset(ASSET_TYPE_FX, asset->projBeaconEffect);
			reader.read_asset(ASSET_TYPE_FX, asset->projIgnitionEffect);
			reader.read_name_reference(asset->projIgnitionSound);

			for (auto i = 0; i < 2; i++)
			{
				reader.read_string(asset->accuracyGraphName[i]);
				reader.read_array(asset->originalAccuracyGraphKnots[i], 3, complete->accuracyGraphKnotCount[i]);
			}

			reader.read_string(asset->szUseHintString);
			reader.read_string(asset->dropHintString);
			reader.read_string(asset->szScript);
			reader.read_array(asset->locationDamageMultipliers, 3, 22);
			reader.read_string(asset->fireRumble);
			reader.read_string(asset->meleeImpactRumble);
			reader.read_asset(ASSET_TYPE_TRACER, asset->tracerType);
			reader.read_name_reference(asset->turretOverheatSound);
			reader.read_asset(ASSET_TYPE_FX, asset->turretOverheatEffect);
			reader.read_string(asset->turretBarrelSpinRumble);
			reader.read_name_reference(asset->turretBarrelSpinMaxSnd);

			for (auto& sound : asset->turretBarrelSpinUpSnd)
			{
				reader.read_name_reference(sound);
			}

			for (auto& sound : asset->turretBarrelSpinDownSnd)
			{
				reader.read_name_reference(sound);
			}

			reader.read_name_reference(asset->missileConeSoundAlias);
			reader.read_name_reference(asset->missileConeSoundAliasAtBase);
			reader.read_script_string(asset->knifeAttachTagOverride);
			reader.read_script_string(asset->stowTag);
			reader.read_asset(ASSET_TYPE_XMODEL, asset->stowOffsetModel);
		}

		void read_overrides(zone_reader& reader, WeaponCompleteDef* asset)
		{
			if (const auto anims = reader.read_array(asset->animOverrides, 3, asset->numAnimOverrides))
			{
				for (auto i = 0u; i < asset->numAnimOverrides; i++)
				{
					reader.read_asset(ASSET_TYPE_XANIMPARTS, anims[i].overrideAnim);
					reader.read_asset(ASSET_TYPE_XANIMPARTS, anims[i].altmodeAnim);
				}
			}

			if (const auto sounds = reader.read_array(asset->soundOverrides, 3, asset->numSoundOverrides))
			{
				for (auto i = 0u; i < asset->numSoundOverrides; i++)
				{
					reader.read_name_reference(sounds[i].overrideSound);
					reader.read_name_reference(sounds[i].altmodeSound);
				}
			}

			if (const auto effects = reader.read_array(asset->fxOverrides, 3, asset->numFXOverrides))
			{
				for (auto i = 0u; i < asset->numFXOverrides; i++)
				{
					reader.read_asset(ASSET_TYPE_FX, effects[i].overrideFX);
					reader.read_asset(ASSET_TYPE_FX, effects[i].altmodeFX);
				}
			}

			reader.read_array(asset->reloadOverrides, 3, asset->numReloadStateTimerOverrides);

			if (const auto notetracks = reader.read_array(asset->notetrackOverrides, 3, asset->numNotetrackOverrides))
			{
				for (auto i = 0u; i < asset->numNotetrackOverrides; i++)
				{
					reader.read_script_string_array(notetracks[i].notetrackSoundMapKeys, 24);
					reader.read_script_string_array(notetracks[i].notetrackSoundMapValues, 24);
				}
			}
		}
	}

	void weapon_def::read(zone_reader& reader, WeaponCompleteDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->szInternalName);

		if (const auto weapon = reader.read_single(asset->weapDef, 3))
		{
			read_weapon_def(reader, asset, weapon);
		}

		reader.read_string(asset->szDisplayName);
		reader.read_script_string_array(asset->hideTags, 32);
		reader.read_asset_array(ASSET_TYPE_ATTACHMENT, asset->scopes, 6);
		reader.read_asset_array(ASSET_TYPE_ATTACHMENT, asset->underBarrels, 3);
		reader.read_asset_array(ASSET_TYPE_ATTACHMENT, asset->others, 8);
		reader.read_asset_array(ASSET_TYPE_XANIMPARTS, asset->szXAnims, NUM_WEAP_ANIMS);
		read_overrides(reader, asset);
		reader.read_string(asset->szAltWeaponName);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->killIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->dpadIcon);
		reader.read_array(asset->accuracyGraphKnots[0], 3, asset->accuracyGraphKnotCount[0]);
		reader.read_array(asset->accuracyGraphKnots[1], 3, asset->accuracyGraphKnotCount[1]);
		reader.read_string(asset->szAdsrBaseSetting);
		reader.pop_stream();
	}

#define WEAPON_SOUND_CUSTOM(__field__) \
		if (data->__field__) \
		{ \
			buf->align(7); \
			buf->write(&buf->data_following); \
			buf->write_str_raw(data->__field__->name); \
			buf->clear_pointer(&dest->__field__); \
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
			buf->clear_pointer(&dest->__field__); \
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
			buf->clear_pointer(&dest->__field__); \
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

			buf->clear_pointer(&dest->gunXModel);
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

			buf->clear_pointer(&dest->camoMaterialTarget);
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

			buf->clear_pointer(&dest->szXAnimsRightHanded);
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

			buf->clear_pointer(&dest->szXAnimsLeftHanded);
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

			buf->clear_pointer(&dest->notetrackFXMapValues);
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

			buf->clear_pointer(&dest->bounceSound);
		}

		if (data->rollingSound)
		{
			buf->align(7);
			dest->rollingSound = buf->write(data->rollingSound, 31);

			for (auto i = 0u; i < 31; i++)
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

			buf->clear_pointer(&dest->worldModel);
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
			buf->align(3);
			buf->write(data->parallelBounce, 31);
			buf->clear_pointer(&dest->parallelBounce);
		}

		if (data->perpendicularBounce)
		{
			buf->align(3);
			buf->write(data->perpendicularBounce, 31);
			buf->clear_pointer(&dest->perpendicularBounce);
		}

		WEAPON_SUBASSET(projTrailEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(projBeaconEffect, ASSET_TYPE_FX, FxEffectDef);
		WEAPON_SUBASSET(projIgnitionEffect, ASSET_TYPE_FX, FxEffectDef);

		WEAPON_SOUND_CUSTOM(projIgnitionSound);

		for (auto i = 0; i < 2; i++)
		{
			if (data->accuracyGraphName[i])
			{
				dest->accuracyGraphName[i] = buf->write_str(data->accuracyGraphName[i]);
			}

			if (data->originalAccuracyGraphKnots[i])
			{
				buf->align(3);
				buf->write(data->originalAccuracyGraphKnots[i], complete->accuracyGraphKnotCount[i]);
				buf->clear_pointer(&dest->originalAccuracyGraphKnots[i]);
			}
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
			buf->clear_pointer(&dest->locationDamageMultipliers);
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
			buf->clear_pointer(&dest->weapDef);
		}

		if (data->szDisplayName)
		{
			dest->szDisplayName = buf->write_str(data->szDisplayName);
		}

		if (data->hideTags)
		{
			buf->align(3);
			buf->write(data->hideTags, 32);
			buf->clear_pointer(&dest->hideTags);
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

			buf->clear_pointer(&dest->szXAnims);
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

			buf->clear_pointer(&dest->animOverrides);
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

			buf->clear_pointer(&dest->soundOverrides);
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

			for (auto i = 0u; i < data->numNotetrackOverrides; i++)
			{
				if (destNoteTrackOverrides[i].notetrackSoundMapKeys)
				{
					buf->align(3);
					buf->write(data->notetrackOverrides[i].notetrackSoundMapKeys, 24);
					buf->clear_pointer(&destNoteTrackOverrides[i].notetrackSoundMapKeys);
				}

				if (destNoteTrackOverrides[i].notetrackSoundMapValues)
				{
					buf->align(3);
					buf->write(data->notetrackOverrides[i].notetrackSoundMapValues, 24);
					buf->clear_pointer(&destNoteTrackOverrides[i].notetrackSoundMapValues);
				}
			}

			buf->clear_pointer(&dest->notetrackOverrides);
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
			buf->clear_pointer(&dest->accuracyGraphKnots[0]);
		}

		if (data->accuracyGraphKnots[1])
		{
			buf->align(3);
			buf->write(data->accuracyGraphKnots[1], data->accuracyGraphKnotCount[1]);
			buf->clear_pointer(&dest->accuracyGraphKnots[1]);
		}

		if (data->szAdsrBaseSetting)
		{
			dest->szAdsrBaseSetting = buf->write_str(data->szAdsrBaseSetting);
		}

		buf->pop_stream();
	}

	json weapon_def::dump_weapondef(WeaponDef* asset)
	{
		assert(sizeof(WeaponDef) == 2712);

		json data;
		json_reflect::write<weapon_fields>(data, *asset, json_options);

		write_anim_array(data["szXAnimsRightHanded"], asset->szXAnimsRightHanded);
		write_anim_array(data["szXAnimsLeftHanded"], asset->szXAnimsLeftHanded);

		json_reflect::write(data["stateTimers"], asset->stateTimers);
		json_reflect::write(data["akimboStateTimers"], asset->akimboStateTimers);

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

		for (auto i = 0; i < 2; i++)
		{
			data["accuracyGraphName"][i] = asset->accuracyGraphName[i] ? asset->accuracyGraphName[i] : "";
		}

		data["knifeAttachTagOverride"] = (asset->knifeAttachTagOverride) ?
			SL_ConvertToString(asset->knifeAttachTagOverride) :
			"";

		data["stowTag"] = (asset->stowTag) ?
			SL_ConvertToString(asset->stowTag)
			: "";

		return data;
	}

	json weapon_def::dump_complete(WeaponCompleteDef* asset)
	{
		assert(sizeof(WeaponCompleteDef) == 296);

		json data;
		data["baseAsset"] = asset->szInternalName;
		data["weapDef"] = dump_weapondef(asset->weapDef);
		json_reflect::write<complete_fields>(data, *asset, json_options);

		write_anim_array(data["szXAnims"], asset->szXAnims);

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
