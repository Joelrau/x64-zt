#include <std_include.hpp>
#include "s2.hpp"

#include "io/json_reflect.hpp"

namespace zonetool::s2
{
	namespace
	{
		constexpr std::size_t model_count = 2;
		constexpr std::size_t xanim_count = 188;
		constexpr std::size_t hide_tag_count = 32;
		constexpr std::size_t notetrack_sound_count = 36;
		constexpr std::size_t notetrack_map_count = 16;
		constexpr std::size_t location_damage_count = 22;
		constexpr std::size_t surface_type_count = 62;

		constexpr const char* anim_names[xanim_count] =
		{
			"ROOT",
			"IDLE",
			"EMPTY_IDLE",
			"FIRE",
			"HOLD_FIRE",
			"LASTSHOT",
			"RECHAMBER",
			"RECHAMBER_ALT",
			"RELOAD_START_ALT",
			"GRENADE_PRIME",
			"GRENADE_PRIME_READY_TO_THROW",
			"MELEE_SWIPE",
			"MELEE_HIT",
			"MELEE_FATAL",
			"MELEE_MISS",
			"MELEE_VICTIM_CROUCHING_HIT",
			"MELEE_VICTIM_CROUCHING_FATAL",
			"MELEE_VICTIM_CROUCHING_MISS",
			"MELEE_ALT_STANDING",
			"MELEE_ALT_CROUCHING",
			"MELEE_ALT_PRONE",
			"MELEE_ALT_JUMPING",
			"RELOAD",
			"RELOAD_EMPTY",
			"RELOAD_START",
			"RELOAD_END",
			"RELOAD_JAM",
			"UNKNOWN_27",
			"FAST_RELOAD",
			"FAST_RELOAD_EMPTY",
			"FAST_RELOAD_START",
			"FAST_RELOAD_END",
			"DUALMAG_RELOAD",
			"DUALMAG_RELOAD_EMPTY",
			"SPEED_RELOAD",
			"RELOAD_FROM_ALT",
			"RAISE",
			"FIRST_RAISE",
			"BREACH_RAISE",
			"DROP",
			"INSPECTION",
			"ALT_RAISE",
			"ALT_DROP",
			"ALT_OVERRIDE",
			"QUICK_RAISE",
			"QUICK_DROP",
			"EMPTY_RAISE",
			"EMPTY_DROP",
			"HYBRID_SIGHT_ON",
			"HYBRID_SIGHT_OFF",
			"SPRINT_IN",
			"SPRINT_IN_CANCEL",
			"SPRINT_LOOP",
			"SPRINT_OUT",
			"STUNNED_START",
			"STUNNED_LOOP",
			"STUNNED_END",
			"UNKNOWN_57",
			"DETONATE",
			"NIGHTVISION_WEAR",
			"NIGHTVISION_REMOVE",
			"ADS_FIRE",
			"ADS_LASTSHOT",
			"ADS_RECHAMBER",
			"DIVE",
			"DIVE_END",
			"CRAWL_IN",
			"CRAWL_LOOP",
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
			"GOLIATH_ENTRY",
			"OFFHAND_SWITCH",
			"HEAT_COOLDOWN_IN",
			"HEAT_COOLDOWN_OUT",
			"OVERHEAT_OUT",
			"SCRIPTED",
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
			"SLIDE_IN",
			"SLIDE_LOOP",
			"SLIDE_OUT",
			"BLAST_FRONT",
			"BLAST_RIGHT",
			"BLAST_BACK",
			"BLAST_LEFT",
			"SPRINTANDFIRE_IN",
			"SPRINTANDFIRE_LOOP",
			"SPRINTANDFIRE_OUT",
			"SPRINT_BAYONET_IN",
			"SPRINT_BAYONET_LOOP",
			"SPRINT_BAYONET_OUT",
			"UNKNOWN_122",
			"SPRINT_CRAWL_IN",
			"SPRINT_CRAWL_LOOP",
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
			"UNKNOWN_135",
			"UNKNOWN_136",
			"UNKNOWN_137",
			"UNKNOWN_138",
			"UNKNOWN_139",
			"UNKNOWN_140",
			"ADDITIVE_JUMP_ROOT",
			"ADDITIVE_JUMP",
			"ADDITIVE_JUMP_BOOST",
			"ADDITIVE_JUMP_LAND_ROOT",
			"ADDITIVE_JUMP_LAND",
			"ADDITIVE_JUMP_LAND_HEAVY",
			"ADDITIVE_WALK_ROOT",
			"ADDITIVE_WALK",
			"ADDITIVE_LOWREADY_ROOT",
			"ADDITIVE_LOWREADY",
			"ADDITIVE_HEAT_COOLDOWN_LOOP_ROOT",
			"ADDITIVE_HEAT_COOLDOWN_LOOP",
			"ADDITIVE_OVERHEAT_IN_ROOT",
			"ADDITIVE_OVERHEAT_IN",
			"ADDITIVE_OVERHEAT_LOOP_ROOT",
			"ADDITIVE_OVERHEAT_LOOP",
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
			"ADDITIVE_STAND_TO_PRONE_ROOT",
			"ADDITIVE_STAND_TO_PRONE",
			"ADDITIVE_PRONE_TO_STAND_ROOT",
			"ADDITIVE_PRONE_TO_STAND",
			"UNKNOWN_171",
			"FLINCH_SMALL_2",
			"FLINCH_SMALL_4",
			"FLINCH_SMALL_6",
			"FLINCH_MEDIUM_2",
			"FLINCH_MEDIUM_4",
			"FLINCH_MEDIUM_6",
			"FLINCH_LARGE_2",
			"FLINCH_LARGE_4",
			"FLINCH_LARGE_6",
			"FLINCH_ADS_2",
			"FLINCH_ADS_4",
			"FLINCH_ADS_6",
			"ADDITIVE_EMPTY_ROOT",
			"ADDITIVE_EMPTY",
			"ADDITIVE_SCRIPTED_ROOT",
			"ADDITIVE_SCRIPTED",
		};

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

		constexpr auto turret_hydraulic_fields = glz::tuple
		{
			&TurretHydraulicSettings::enabled,
			&TurretHydraulicSettings::useAxisSounds,
			&TurretHydraulicSettings::minVelocity,
			&TurretHydraulicSettings::maxVelocity,
			&TurretHydraulicSettings::verticalSound,
			&TurretHydraulicSettings::verticalStopSound,
			&TurretHydraulicSettings::horizontalSound,
			&TurretHydraulicSettings::horizontalStopSound,
			&TurretHydraulicSettings::movementSounds,
			&TurretHydraulicSettings::soundFadeInTime,
			&TurretHydraulicSettings::soundFadeOutTime,
		};

		constexpr auto state_timer_fields = glz::tuple // glaze miscounts aggregate members with C arrays, so StateTimers needs an explicit list
		{
			&StateTimers::fireDelay,
			&StateTimers::meleeDelay,
			&StateTimers::meleeChargeDelay,
			&StateTimers::unknown12,
			&StateTimers::detonateDelay,
			&StateTimers::fireTime,
			&StateTimers::rechamberTimeAlt,
			&StateTimers::rechamberTime,
			&StateTimers::rechamberTimeOneHanded,
			&StateTimers::rechamberBoltTime,
			&StateTimers::unknown40,
			&StateTimers::holdFireTime,
			&StateTimers::grenadePrimeReadyToThrowTime,
			&StateTimers::detonateTime,
			&StateTimers::meleeTime,
			&StateTimers::meleeChargeTime,
			&StateTimers::unknown64,
			&StateTimers::reloadTime,
			&StateTimers::reloadJamTime,
			&StateTimers::unknown76,
			&StateTimers::activeReloadSuccessWindowEnd,
			&StateTimers::activeReloadSuccessWindowStart,
			&StateTimers::activeReloadJamWindowEnd,
			&StateTimers::reloadShowRocketTime,
			&StateTimers::reloadEmptyTime,
			&StateTimers::reloadAddTime,
			&StateTimers::reloadEmptyAddTime,
			&StateTimers::reloadStartTime,
			&StateTimers::reloadStartAddTime,
			&StateTimers::reloadEndTime,
			&StateTimers::reloadTimeDualWield,
			&StateTimers::reloadAddTimeDualWield,
			&StateTimers::reloadEmptyDualMag,
			&StateTimers::reloadEmptyAddTimeDualMag,
			&StateTimers::speedReloadTime,
			&StateTimers::speedReloadAddTime,
			&StateTimers::dropTime,
			&StateTimers::raiseTime,
			&StateTimers::inspectTime,
			&StateTimers::unknown156,
			&StateTimers::altDropTime,
			&StateTimers::unknown164,
			&StateTimers::altRaiseTime,
			&StateTimers::quickDropTime,
			&StateTimers::quickRaiseTime,
			&StateTimers::firstRaiseTime,
			&StateTimers::breachRaiseTime,
			&StateTimers::emptyRaiseTime,
			&StateTimers::emptyDropTime,
			&StateTimers::sprintInTime,
			&StateTimers::sprintLoopTime,
			&StateTimers::sprintOutTime,
			&StateTimers::unknown208,
			&StateTimers::stunnedTimeBegin,
			&StateTimers::stunnedTimeLoop,
			&StateTimers::stunnedTimeEnd,
			&StateTimers::nightVisionWearTime,
			&StateTimers::nightVisionWearTimeFadeOutEnd,
			&StateTimers::nightVisionWearTimePowerUp,
			&StateTimers::nightVisionRemoveTime,
			&StateTimers::nightVisionRemoveTimePowerDown,
			&StateTimers::nightVisionRemoveTimeFadeInStart,
			&StateTimers::aiFuseTime,
			&StateTimers::fuseTime,
			&StateTimers::missileTime,
			&StateTimers::primeTime,
			&StateTimers::bHoldFullPrime,
			&StateTimers::blastFrontTime,
			&StateTimers::blastRightTime,
			&StateTimers::blastBackTime,
			&StateTimers::blastLeftTime,
			&StateTimers::slideInTime,
			&StateTimers::slideLoopTime,
			&StateTimers::slideOutTime,
			&StateTimers::crawlInTime,
			&StateTimers::crawlLoopTime,
			&StateTimers::unknown308,
			&StateTimers::unknown312,
			&StateTimers::offhandSwitchTime,
		};

		constexpr auto anim_override_fields = glz::tuple
		{
			&AnimOverrideEntry::animHand,
			&AnimOverrideEntry::attachment1,
			&AnimOverrideEntry::attachment2,
			&AnimOverrideEntry::animTreeType,
			&AnimOverrideEntry::overrideAnim,
			&AnimOverrideEntry::altmodeAnim,
			&AnimOverrideEntry::animTime,
			&AnimOverrideEntry::altTime,
		};

		constexpr auto sound_override_fields = glz::tuple
		{
			&SoundOverrideEntry::attachment1,
			&SoundOverrideEntry::attachment2,
			&SoundOverrideEntry::soundType,
			&SoundOverrideEntry::overrideSound,
			&SoundOverrideEntry::altmodeSound,
		};

		constexpr auto fx_override_fields = glz::tuple
		{
			&FXOverrideEntry::attachment1,
			&FXOverrideEntry::attachment2,
			&FXOverrideEntry::fxType,
			&FXOverrideEntry::overrideFX,
			&FXOverrideEntry::altmodeFX,
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
			&WeaponCompleteDef::pickupSound,
			&WeaponCompleteDef::pickupSoundPlayer,
			&WeaponCompleteDef::ammoPickupSound,
			&WeaponCompleteDef::ammoPickupSoundPlayer,
			&WeaponCompleteDef::projectileSound,
			&WeaponCompleteDef::pullbackSound,
			&WeaponCompleteDef::pullbackSoundPlayer,
			&WeaponCompleteDef::throwbackSound,
			&WeaponCompleteDef::throwbackSoundPlayer,
			&WeaponCompleteDef::fireSound,
			&WeaponCompleteDef::fireSoundPlayer,
			&WeaponCompleteDef::fireSoundPlayerAkimbo,
			&WeaponCompleteDef::fireLoopSound,
			&WeaponCompleteDef::fireLoopSoundPlayer,
			&WeaponCompleteDef::fireLoopEndPointSound,
			&WeaponCompleteDef::fireLoopEndPointSoundPlayer,
			&WeaponCompleteDef::fireStopSound,
			&WeaponCompleteDef::fireStopSoundPlayer,
			&WeaponCompleteDef::fireLastSound,
			&WeaponCompleteDef::fireLastSoundPlayer,
			&WeaponCompleteDef::fireFirstSound,
			&WeaponCompleteDef::fireFirstSoundPlayer,
			&WeaponCompleteDef::fireCustomSound,
			&WeaponCompleteDef::fireCustomSoundPlayer,
			&WeaponCompleteDef::emptyFireSound,
			&WeaponCompleteDef::emptyFireSoundPlayer,
			&WeaponCompleteDef::adsRequiredFireSoundPlayer,
			&WeaponCompleteDef::meleeSwipeSound,
			&WeaponCompleteDef::meleeSwipeSoundPlayer,
			&WeaponCompleteDef::meleeHitSound,
			&WeaponCompleteDef::meleeMissSound,
			&WeaponCompleteDef::meleeHitSoundPlayer,
			&WeaponCompleteDef::meleeMissSoundPlayer,
			&WeaponCompleteDef::rechamberSound,
			&WeaponCompleteDef::rechamberSoundPlayer,
			&WeaponCompleteDef::reloadSound,
			&WeaponCompleteDef::reloadSoundPlayer,
			&WeaponCompleteDef::reloadEmptySound,
			&WeaponCompleteDef::reloadEmptySoundPlayer,
			&WeaponCompleteDef::reloadStartSound,
			&WeaponCompleteDef::reloadStartSoundPlayer,
			&WeaponCompleteDef::reloadEndSound,
			&WeaponCompleteDef::reloadEndSoundPlayer,
			&WeaponCompleteDef::detonateSound,
			&WeaponCompleteDef::detonateSoundPlayer,
			&WeaponCompleteDef::nightVisionWearSound,
			&WeaponCompleteDef::nightVisionWearSoundPlayer,
			&WeaponCompleteDef::nightVisionRemoveSound,
			&WeaponCompleteDef::nightVisionRemoveSoundPlayer,
			&WeaponCompleteDef::altSwitchSound,
			&WeaponCompleteDef::altSwitchSoundPlayer,
			&WeaponCompleteDef::raiseSound,
			&WeaponCompleteDef::raiseSoundPlayer,
			&WeaponCompleteDef::firstRaiseSound,
			&WeaponCompleteDef::firstRaiseSoundPlayer,
			&WeaponCompleteDef::putawaySound,
			&WeaponCompleteDef::putawaySoundPlayer,
			&WeaponCompleteDef::scanSound,
			&WeaponCompleteDef::changeVariableZoomSound,
			&WeaponCompleteDef::adsUpSound,
			&WeaponCompleteDef::adsDownSound,
			&WeaponCompleteDef::adsCrosshairEnemySound,
			&WeaponCompleteDef::turretMountSound,
			&WeaponCompleteDef::turretDismountSound,
			&WeaponCompleteDef::grenadeStickSound,
		};

		constexpr auto weapon_fields = glz::tuple
		{
			&WeaponCompleteDef::szInternalName,
			&WeaponCompleteDef::szDisplayName,
			json_reflect::counted(&WeaponCompleteDef::gunModel, model_count),
			json_reflect::counted(&WeaponCompleteDef::gunModelConditions, model_count),
			&WeaponCompleteDef::handModel,
			&WeaponCompleteDef::lobWorldModelName,
			json_reflect::manual("szXAnimsRightHanded"),
			json_reflect::manual("szXAnimsLeftHanded"),
			json_reflect::manual("hideTags"),
			json_reflect::manual("attachments"),
			&WeaponCompleteDef::unknown80,
			json_reflect::manual("szXAnims"),
			json_reflect::counted(&WeaponCompleteDef::animOverrides, &WeaponCompleteDef::numAnimOverrides, anim_override_fields),
			json_reflect::counted(&WeaponCompleteDef::soundOverrides, &WeaponCompleteDef::numSoundOverrides, sound_override_fields),
			json_reflect::counted(&WeaponCompleteDef::fxOverrides, &WeaponCompleteDef::numFXOverrides, fx_override_fields),
			json_reflect::counted(&WeaponCompleteDef::reloadOverrides, &WeaponCompleteDef::numReloadStateTimerOverrides, reload_override_fields),
			json_reflect::manual("notetrackOverrides"),
			json_reflect::manual("notetrackSoundMap"),
			json_reflect::manual("notetrackRumbleMap"),
			json_reflect::manual("notetrackFXMap"),
			&WeaponCompleteDef::szAdsrBaseSetting,
			&WeaponCompleteDef::viewFlashEffect,
			&WeaponCompleteDef::viewBodyFlashEffect,
			&WeaponCompleteDef::worldFlashEffect,
			&WeaponCompleteDef::viewFlashADSEffect,
			&WeaponCompleteDef::viewBodyFlashADSEffect,
			&WeaponCompleteDef::signatureViewFlashEffect,
			&WeaponCompleteDef::signatureViewBodyFlashEffect,
			&WeaponCompleteDef::signatureWorldFlashEffect,
			&WeaponCompleteDef::signatureViewFlashADSEffect,
			&WeaponCompleteDef::signatureViewBodyFlashADSEffect,
			&WeaponCompleteDef::unknownEffect280,
			&WeaponCompleteDef::meleeHitEffect,
			&WeaponCompleteDef::meleeMissEffect,
			json_reflect::group("sounds", weapon_sound_fields),
			json_reflect::counted(&WeaponCompleteDef::bounceSound, surface_type_count),
			json_reflect::counted(&WeaponCompleteDef::rollingSound, surface_type_count),
			&WeaponCompleteDef::viewShellEjectEffect,
			&WeaponCompleteDef::worldShellEjectEffect,
			&WeaponCompleteDef::viewLastShotEjectEffect,
			&WeaponCompleteDef::worldLastShotEjectEffect,
			&WeaponCompleteDef::viewMagEjectEffect,
			&WeaponCompleteDef::reticleCenter,
			&WeaponCompleteDef::reticleCenterEnemy,
			&WeaponCompleteDef::reticleSide,
			json_reflect::counted(&WeaponCompleteDef::worldModel, model_count),
			json_reflect::counted(&WeaponCompleteDef::worldModelConditions, model_count),
			&WeaponCompleteDef::worldClipModel,
			&WeaponCompleteDef::rocketModel,
			&WeaponCompleteDef::hudIcon,
			&WeaponCompleteDef::pickupIcon,
			&WeaponCompleteDef::grenadeIndicatorIcon,
			&WeaponCompleteDef::ammoCounterIcon,
			&WeaponCompleteDef::szAmmoName,
			&WeaponCompleteDef::szClipName,
			&WeaponCompleteDef::szSharedAmmoCapName,
			&WeaponCompleteDef::physCollmap,
			&WeaponCompleteDef::physPreset,
			&WeaponCompleteDef::szUseHintString,
			&WeaponCompleteDef::dropHintString,
			json_reflect::counted(&WeaponCompleteDef::locationDamageMultipliers, location_damage_count),
			&WeaponCompleteDef::fireRumble,
			&WeaponCompleteDef::unknownRumble1040,
			&WeaponCompleteDef::meleeImpactRumble,
			&WeaponCompleteDef::tracerType,
			&WeaponCompleteDef::signatureTracerType,
			&WeaponCompleteDef::unknownTracer1072,
			&WeaponCompleteDef::laserType,
			&WeaponCompleteDef::beamType,
			&WeaponCompleteDef::turretOverheatSound,
			&WeaponCompleteDef::turretOverheatEffect,
			&WeaponCompleteDef::turretBarrelSpinRumble,
			&WeaponCompleteDef::turretBarrelSpinMaxSnd,
			&WeaponCompleteDef::turretBarrelSpinUpSnd,
			&WeaponCompleteDef::turretBarrelSpinDownSnd,
			&WeaponCompleteDef::missileConeSoundAlias,
			&WeaponCompleteDef::missileConeSoundAliasAtBase,
			&WeaponCompleteDef::stowOffsetModel,
			json_reflect::nested(&WeaponCompleteDef::turretHydraulicSettings, turret_hydraulic_fields),
			&WeaponCompleteDef::playerAnimType,
			&WeaponCompleteDef::unknown1239,
			&WeaponCompleteDef::weapType,
			&WeaponCompleteDef::weapClass,
			&WeaponCompleteDef::penetrateType,
			&WeaponCompleteDef::penetrateDepth,
			&WeaponCompleteDef::impactType,
			&WeaponCompleteDef::unknown1260,
			&WeaponCompleteDef::inventoryType,
			&WeaponCompleteDef::fireType,
			&WeaponCompleteDef::fireBarrels,
			&WeaponCompleteDef::adsFireMode,
			&WeaponCompleteDef::burstFireCooldown,
			&WeaponCompleteDef::greebleType,
			&WeaponCompleteDef::autoReloadType,
			&WeaponCompleteDef::autoHolsterType,
			&WeaponCompleteDef::offhandClass,
			&WeaponCompleteDef::stance,
			&WeaponCompleteDef::reticleCenterSize,
			&WeaponCompleteDef::reticleCenterEnemySize,
			&WeaponCompleteDef::reticleSideSize,
			&WeaponCompleteDef::reticleMinOfs,
			&WeaponCompleteDef::activeReticleType,
			&WeaponCompleteDef::standMove,
			&WeaponCompleteDef::standRot,
			&WeaponCompleteDef::strafeMove,
			&WeaponCompleteDef::strafeRot,
			&WeaponCompleteDef::duckedOfs,
			&WeaponCompleteDef::duckedMove,
			&WeaponCompleteDef::duckedRot,
			&WeaponCompleteDef::proneOfs,
			&WeaponCompleteDef::proneMove,
			&WeaponCompleteDef::proneRot,
			&WeaponCompleteDef::posMoveRate,
			&WeaponCompleteDef::posProneMoveRate,
			&WeaponCompleteDef::standMoveMinSpeed,
			&WeaponCompleteDef::duckedMoveMinSpeed,
			&WeaponCompleteDef::proneMoveMinSpeed,
			&WeaponCompleteDef::posRotRate,
			&WeaponCompleteDef::posProneRotRate,
			&WeaponCompleteDef::hudIconRatio,
			&WeaponCompleteDef::pickupIconRatio,
			&WeaponCompleteDef::grenadeIndicatorIconRatio,
			&WeaponCompleteDef::ammoCounterIconRatio,
			&WeaponCompleteDef::ammoCounterClip,
			&WeaponCompleteDef::unknown1492,
			&WeaponCompleteDef::unknown1496,
			&WeaponCompleteDef::maxAmmo,
			&WeaponCompleteDef::maxAmmoClips,
			&WeaponCompleteDef::minAmmoReq,
			&WeaponCompleteDef::startAmmo,
			&WeaponCompleteDef::startAmmoClips,
			&WeaponCompleteDef::clipSize,
			&WeaponCompleteDef::shotCount,
			&WeaponCompleteDef::sharedAmmoCapIndex,
			&WeaponCompleteDef::sharedAmmoCap,
			&WeaponCompleteDef::unknown1560,
			&WeaponCompleteDef::unknown1564,
			&WeaponCompleteDef::meleeDamage,
			&WeaponCompleteDef::damageType,
			json_reflect::nested(&WeaponCompleteDef::stateTimers, state_timer_fields),
			json_reflect::nested(&WeaponCompleteDef::akimboStateTimers, state_timer_fields),
			&WeaponCompleteDef::unknown2216,
			&WeaponCompleteDef::autoAimRange,
			&WeaponCompleteDef::aimAssistRange,
			&WeaponCompleteDef::aimAssistRangeAds,
			&WeaponCompleteDef::aimPadding,
			&WeaponCompleteDef::unknown2240,
			&WeaponCompleteDef::unknown2244,
			&WeaponCompleteDef::unknown2248,
			&WeaponCompleteDef::maxBreathHoldTime,
			&WeaponCompleteDef::unknown2256,
			&WeaponCompleteDef::enemyCrosshairRange,
			&WeaponCompleteDef::moveSpeedScale,
			&WeaponCompleteDef::adsMoveSpeedScale,
			&WeaponCompleteDef::sprintDurationScale,
			&WeaponCompleteDef::unknown2276,
			&WeaponCompleteDef::adsZoomFov,
			&WeaponCompleteDef::adsZoomInFrac,
			&WeaponCompleteDef::adsZoomOutFrac,
			&WeaponCompleteDef::adsSceneBlur,
			json_reflect::nested(&WeaponCompleteDef::overlay, overlay_fields),
			&WeaponCompleteDef::overlayInterface,
			&WeaponCompleteDef::adsBobFactor,
			&WeaponCompleteDef::adsViewBobMult,
			&WeaponCompleteDef::hipSpreadDecayRate,
			&WeaponCompleteDef::hipSpreadStandMin,
			&WeaponCompleteDef::hipSpreadStandMax,
			&WeaponCompleteDef::hipSpreadStandDecay,
			&WeaponCompleteDef::hipSpreadDuckedMin,
			&WeaponCompleteDef::hipSpreadDuckedMax,
			&WeaponCompleteDef::hipSpreadDuckedDecay,
			&WeaponCompleteDef::hipSpreadProneMin,
			&WeaponCompleteDef::hipSpreadProneMax,
			&WeaponCompleteDef::hipSpreadProneDecay,
			&WeaponCompleteDef::hipSpreadSprintMin,
			&WeaponCompleteDef::hipSpreadSprintMax,
			&WeaponCompleteDef::hipSpreadSprintDecay,
			&WeaponCompleteDef::hipSpreadSlideMin,
			&WeaponCompleteDef::hipSpreadSlideMax,
			&WeaponCompleteDef::hipSpreadSlideDecay,
			&WeaponCompleteDef::hipSpreadMoveAdd,
			&WeaponCompleteDef::hipSpreadTurnAdd,
			&WeaponCompleteDef::hipSpreadFireAdd,
			&WeaponCompleteDef::hipReticleSidePos,
			&WeaponCompleteDef::adsIdleAmount,
			&WeaponCompleteDef::hipIdleAmount,
			&WeaponCompleteDef::adsIdleSpeed,
			&WeaponCompleteDef::hipIdleSpeed,
			&WeaponCompleteDef::unknown2500,
			&WeaponCompleteDef::idleCrouchFactor,
			&WeaponCompleteDef::idleProneFactor,
			&WeaponCompleteDef::gunMaxPitch,
			&WeaponCompleteDef::gunMaxYaw,
			&WeaponCompleteDef::adsTransInTime,
			&WeaponCompleteDef::adsTransInFromSprintTime,
			&WeaponCompleteDef::adsTransOutTime,
			&WeaponCompleteDef::unknown2532,
			&WeaponCompleteDef::unknown2540,
			&WeaponCompleteDef::swayMaxAngle,
			&WeaponCompleteDef::swayMaxAngleSteadyAim,
			&WeaponCompleteDef::swayLerpSpeed,
			&WeaponCompleteDef::swayPitchScale,
			&WeaponCompleteDef::swayYawScale,
			&WeaponCompleteDef::swayHorizScale,
			&WeaponCompleteDef::swayVertScale,
			&WeaponCompleteDef::swayShellShockScale,
			&WeaponCompleteDef::adsSwayMaxAngle,
			&WeaponCompleteDef::adsSwayLerpSpeed,
			&WeaponCompleteDef::adsSwayPitchScale,
			&WeaponCompleteDef::adsSwayYawScale,
			&WeaponCompleteDef::adsSwayHorizScale,
			&WeaponCompleteDef::adsSwayVertScale,
			&WeaponCompleteDef::adsViewErrorMin,
			&WeaponCompleteDef::adsViewErrorMax,
			&WeaponCompleteDef::adsFireAnimFrac,
			&WeaponCompleteDef::scopeDriftDelay,
			&WeaponCompleteDef::scopeDriftLerpInTime,
			&WeaponCompleteDef::scopeDriftSteadyTime,
			&WeaponCompleteDef::scopeDriftLerpOutTime,
			&WeaponCompleteDef::scopeDriftSteadyFactor,
			&WeaponCompleteDef::scopeDriftUnsteadyFactor,
			&WeaponCompleteDef::bobVerticalFactor,
			&WeaponCompleteDef::bobHorizontalFactor,
			&WeaponCompleteDef::bobViewVerticalFactor,
			&WeaponCompleteDef::bobViewHorizontalFactor,
			&WeaponCompleteDef::stationaryZoomFov,
			&WeaponCompleteDef::stationaryZoomDelay,
			&WeaponCompleteDef::stationaryZoomLerpInTime,
			&WeaponCompleteDef::stationaryZoomLerpOutTime,
			&WeaponCompleteDef::adsDofStart,
			&WeaponCompleteDef::adsDofEnd,
			&WeaponCompleteDef::unknown2676,
			&WeaponCompleteDef::killIcon,
			&WeaponCompleteDef::dpadIcon,
			&WeaponCompleteDef::hudProximityWarningIcon,
			&WeaponCompleteDef::killIconRatio,
			&WeaponCompleteDef::dpadIconRatio,
			&WeaponCompleteDef::fireAnimLength,
			&WeaponCompleteDef::fireAnimLengthAkimbo,
			&WeaponCompleteDef::reloadAmmoAdd,
			&WeaponCompleteDef::reloadStartAdd,
			&WeaponCompleteDef::ammoDropStockMin,
			&WeaponCompleteDef::ammoDropStockMax,
			&WeaponCompleteDef::ammoDropClipPercentMin,
			&WeaponCompleteDef::ammoDropClipPercentMax,
			&WeaponCompleteDef::explosionRadius,
			&WeaponCompleteDef::explosionRadiusMin,
			&WeaponCompleteDef::explosionInnerDamage,
			&WeaponCompleteDef::explosionOuterDamage,
			&WeaponCompleteDef::tankMetalSolidDamageMult,
			&WeaponCompleteDef::tankMetalThinDamageMult,
			&WeaponCompleteDef::damageConeAngle,
			&WeaponCompleteDef::bulletExplosionRadius,
			&WeaponCompleteDef::bulletExplosionInnerDamage,
			&WeaponCompleteDef::bulletExplosionOuterDamage,
			&WeaponCompleteDef::projectileSpeed,
			&WeaponCompleteDef::projectileSpeedUp,
			&WeaponCompleteDef::projectileSpeedForward,
			&WeaponCompleteDef::projectileSpeedHorizontal,
			&WeaponCompleteDef::projectileActivateDist,
			&WeaponCompleteDef::projLifetime,
			&WeaponCompleteDef::timeToAccelerate,
			&WeaponCompleteDef::projectileCurvature,
			&WeaponCompleteDef::explosionShakeRadius,
			&WeaponCompleteDef::explosionShakeScale,
			&WeaponCompleteDef::explosionShakeDuration,
			&WeaponCompleteDef::projectileSpinRate,
			&WeaponCompleteDef::unknown2832,
			&WeaponCompleteDef::unknown2844,
			&WeaponCompleteDef::projectileName,
			&WeaponCompleteDef::projectileModel,
			&WeaponCompleteDef::projExplosionEffect,
			&WeaponCompleteDef::projDudEffect,
			&WeaponCompleteDef::unknownEffect2880,
			&WeaponCompleteDef::projExplosionSound,
			&WeaponCompleteDef::projDudSound,
			&WeaponCompleteDef::unknownSound2904,
			&WeaponCompleteDef::projExplosion,
			&WeaponCompleteDef::stickiness,
			&WeaponCompleteDef::unknown2920,
			&WeaponCompleteDef::lowAmmoWarningThreshold,
			&WeaponCompleteDef::ricochetChance,
			&WeaponCompleteDef::riotShieldHealth,
			&WeaponCompleteDef::riotShieldDamageMult,
			&WeaponCompleteDef::unknown2940,
			json_reflect::counted(&WeaponCompleteDef::parallelBounce, surface_type_count),
			json_reflect::counted(&WeaponCompleteDef::perpendicularBounce, surface_type_count),
			&WeaponCompleteDef::projTrailEffect,
			&WeaponCompleteDef::projBeaconEffect,
			&WeaponCompleteDef::projectileColor,
			&WeaponCompleteDef::guidedMissileType,
			&WeaponCompleteDef::maxSteeringAccel,
			&WeaponCompleteDef::unknown2996,
			&WeaponCompleteDef::guidedPitchMin,
			&WeaponCompleteDef::guidedPitchMax,
			&WeaponCompleteDef::guidedYawLimit,
			&WeaponCompleteDef::guidedRollLimit,
			&WeaponCompleteDef::guidedPitchRate,
			&WeaponCompleteDef::guidedYawRate,
			&WeaponCompleteDef::projIgnitionDelay,
			&WeaponCompleteDef::unknown3028,
			&WeaponCompleteDef::projIgnitionEffect,
			&WeaponCompleteDef::projIgnitionSound,
			&WeaponCompleteDef::adsAimPitch,
			&WeaponCompleteDef::adsCrosshairInFrac,
			&WeaponCompleteDef::adsCrosshairOutFrac,
			&WeaponCompleteDef::adsGunKickPitchMin,
			&WeaponCompleteDef::adsGunKickPitchMax,
			&WeaponCompleteDef::adsGunKickYawMin,
			&WeaponCompleteDef::adsGunKickYawMax,
			&WeaponCompleteDef::adsGunKickMagMin,
			&WeaponCompleteDef::adsGunKickAccel,
			&WeaponCompleteDef::adsGunKickSpeedMax,
			&WeaponCompleteDef::adsGunKickSpeedDecay,
			&WeaponCompleteDef::adsGunKickStaticDecay,
			&WeaponCompleteDef::adsGunKickReducedKickBullets,
			&WeaponCompleteDef::adsGunKickReducedKickPercent,
			&WeaponCompleteDef::adsViewKickPitchMin,
			&WeaponCompleteDef::adsViewKickPitchMax,
			&WeaponCompleteDef::adsViewKickYawMin,
			&WeaponCompleteDef::adsViewKickYawMax,
			&WeaponCompleteDef::adsViewKickMagMin,
			&WeaponCompleteDef::adsViewKickCenterSpeed,
			&WeaponCompleteDef::adsViewScatterMin,
			&WeaponCompleteDef::adsViewScatterMax,
			&WeaponCompleteDef::adsViewKickAltBullets,
			&WeaponCompleteDef::adsViewKickAltPitchMin,
			&WeaponCompleteDef::adsViewKickAltPitchMax,
			&WeaponCompleteDef::adsViewKickAltYawMin,
			&WeaponCompleteDef::adsViewKickAltYawMax,
			&WeaponCompleteDef::adsViewKickAltMagMin,
			&WeaponCompleteDef::adsViewKickAltCenterSpeed,
			&WeaponCompleteDef::adsViewScatterAltMin,
			&WeaponCompleteDef::adsViewScatterAltMax,
			&WeaponCompleteDef::unknown3172,
			&WeaponCompleteDef::adsSpread,
			&WeaponCompleteDef::adsKickDuckedScale,
			&WeaponCompleteDef::adsKickProneScale,
			&WeaponCompleteDef::adsKickCenterSpeedDuckedScale,
			&WeaponCompleteDef::adsKickCenterSpeedProneScale,
			&WeaponCompleteDef::unknown3204,
			&WeaponCompleteDef::unknown3208,
			&WeaponCompleteDef::hipGunKickPitchMin,
			&WeaponCompleteDef::hipGunKickPitchMax,
			&WeaponCompleteDef::hipGunKickYawMin,
			&WeaponCompleteDef::hipGunKickYawMax,
			&WeaponCompleteDef::hipGunKickMagMin,
			&WeaponCompleteDef::hipGunKickAccel,
			&WeaponCompleteDef::hipGunKickSpeedMax,
			&WeaponCompleteDef::hipGunKickSpeedDecay,
			&WeaponCompleteDef::hipGunKickStaticDecay,
			&WeaponCompleteDef::hipGunKickReducedKickBullets,
			&WeaponCompleteDef::hipGunKickReducedKickPercent,
			&WeaponCompleteDef::hipViewKickPitchMin,
			&WeaponCompleteDef::hipViewKickPitchMax,
			&WeaponCompleteDef::hipViewKickYawMin,
			&WeaponCompleteDef::hipViewKickYawMax,
			&WeaponCompleteDef::hipViewKickMagMin,
			&WeaponCompleteDef::hipViewKickCenterSpeed,
			&WeaponCompleteDef::hipViewScatterMin,
			&WeaponCompleteDef::hipViewScatterMax,
			&WeaponCompleteDef::hipViewKickAltBullets,
			&WeaponCompleteDef::hipViewKickAltPitchMin,
			&WeaponCompleteDef::hipViewKickAltPitchMax,
			&WeaponCompleteDef::hipViewKickAltYawMin,
			&WeaponCompleteDef::hipViewKickAltYawMax,
			&WeaponCompleteDef::hipViewKickAltMagMin,
			&WeaponCompleteDef::hipViewKickAltCenterSpeed,
			&WeaponCompleteDef::hipViewScatterAltMin,
			&WeaponCompleteDef::hipViewScatterAltMax,
			&WeaponCompleteDef::unknown3340,
			&WeaponCompleteDef::hipKickDuckedScale,
			&WeaponCompleteDef::hipKickProneScale,
			&WeaponCompleteDef::hipKickCenterSpeedDuckedScale,
			&WeaponCompleteDef::hipKickCenterSpeedProneScale,
			&WeaponCompleteDef::unknown3364,
			&WeaponCompleteDef::positionReloadTransTime,
			&WeaponCompleteDef::unknown3388,
			&WeaponCompleteDef::unknown3392,
			json_reflect::manual("accuracyGraphs"),
			&WeaponCompleteDef::originalAccuracyGraphKnotCount,
			&WeaponCompleteDef::maxRange,
			&WeaponCompleteDef::turretFireRange,
			&WeaponCompleteDef::leftArc,
			&WeaponCompleteDef::rightArc,
			&WeaponCompleteDef::topArc,
			&WeaponCompleteDef::bottomArc,
			&WeaponCompleteDef::accuracy,
			&WeaponCompleteDef::aiSpread,
			&WeaponCompleteDef::playerSpread,
			&WeaponCompleteDef::minTurnSpeed,
			&WeaponCompleteDef::maxTurnSpeed,
			&WeaponCompleteDef::pitchConvergenceTime,
			&WeaponCompleteDef::yawConvergenceTime,
			&WeaponCompleteDef::suppressTime,
			&WeaponCompleteDef::animHorRotateInc,
			&WeaponCompleteDef::playerPositionDist,
			&WeaponCompleteDef::unknown3528,
			&WeaponCompleteDef::useHintStringIndex,
			&WeaponCompleteDef::dropHintStringIndex,
			&WeaponCompleteDef::horizViewJitter,
			&WeaponCompleteDef::vertViewJitter,
			&WeaponCompleteDef::scanSpeed,
			&WeaponCompleteDef::scanAccel,
			&WeaponCompleteDef::scanPauseTime,
			&WeaponCompleteDef::szScript,
			&WeaponCompleteDef::damageFalloffLerpNear,
			&WeaponCompleteDef::damageFalloffLerpFar,
			&WeaponCompleteDef::unknown3570,
			&WeaponCompleteDef::damage,
			&WeaponCompleteDef::unknown3576,
			&WeaponCompleteDef::burnDamage,
			&WeaponCompleteDef::burnDuration,
			&WeaponCompleteDef::burnType,
			&WeaponCompleteDef::unknown3592,
			&WeaponCompleteDef::unknown3596,
			&WeaponCompleteDef::unknown3600,
			&WeaponCompleteDef::unknown3604,
			&WeaponCompleteDef::playerDamage,
			&WeaponCompleteDef::maxDamageRange,
			&WeaponCompleteDef::mid1Damage,
			&WeaponCompleteDef::mid1PlayerDamage,
			&WeaponCompleteDef::mid1DamageRange,
			&WeaponCompleteDef::mid2Damage,
			&WeaponCompleteDef::mid2PlayerDamage,
			&WeaponCompleteDef::minDamage,
			&WeaponCompleteDef::minPlayerDamage,
			&WeaponCompleteDef::minDamageRange,
			&WeaponCompleteDef::unknown3648,
			&WeaponCompleteDef::unknown3652,
			&WeaponCompleteDef::signatureDamage,
			&WeaponCompleteDef::signatureMid1Damage,
			&WeaponCompleteDef::signatureMinDamage,
			&WeaponCompleteDef::signatureMaxDamageRange,
			&WeaponCompleteDef::signatureMinDamageRange,
			&WeaponCompleteDef::destabilizationRateTime,
			&WeaponCompleteDef::destabilizationCurvatureMax,
			&WeaponCompleteDef::destabilizeDistance,
			&WeaponCompleteDef::turretADSTime,
			&WeaponCompleteDef::turretFov,
			&WeaponCompleteDef::turretFovADS,
			&WeaponCompleteDef::turretScopeZoomRate,
			&WeaponCompleteDef::turretScopeZoomMin,
			&WeaponCompleteDef::turretScopeZoomMax,
			&WeaponCompleteDef::overheatUpRate,
			&WeaponCompleteDef::overheatDownRate,
			&WeaponCompleteDef::overheatCooldownRate,
			&WeaponCompleteDef::overheatPenalty,
			&WeaponCompleteDef::turretBarrelSpinSpeed,
			&WeaponCompleteDef::turretBarrelSpinUpTime,
			&WeaponCompleteDef::missileConeSoundRadiusAtTop,
			&WeaponCompleteDef::missileConeSoundRadiusAtBase,
			&WeaponCompleteDef::missileConeSoundHeight,
			&WeaponCompleteDef::missileConeSoundOriginOffset,
			&WeaponCompleteDef::missileConeSoundVolumescaleAtCore,
			&WeaponCompleteDef::missileConeSoundVolumescaleAtEdge,
			&WeaponCompleteDef::missileConeSoundVolumescaleCoreSize,
			&WeaponCompleteDef::missileConeSoundPitchAtTop,
			&WeaponCompleteDef::missileConeSoundPitchAtBottom,
			&WeaponCompleteDef::missileConeSoundPitchTopSize,
			&WeaponCompleteDef::missileConeSoundPitchBottomSize,
			&WeaponCompleteDef::missileConeSoundCrossfadeTopSize,
			&WeaponCompleteDef::missileConeSoundCrossfadeBottomSize,
			json_reflect::manual("knifeAttachTagOverride"),
			&WeaponCompleteDef::aim_automelee_lerp,
			&WeaponCompleteDef::aim_automelee_range,
			&WeaponCompleteDef::unknown3832,
			&WeaponCompleteDef::aim_automelee_region_width,
			&WeaponCompleteDef::aim_automelee_region_height,
			&WeaponCompleteDef::player_meleeHeight,
			&WeaponCompleteDef::player_meleeRange,
			&WeaponCompleteDef::player_meleeWidth,
			&WeaponCompleteDef::unknown3856,
			&WeaponCompleteDef::changedFireTime,
			&WeaponCompleteDef::changedFireTimeNumBullets,
			&WeaponCompleteDef::fireTimeInterpolationType,
			&WeaponCompleteDef::generateAmmo,
			&WeaponCompleteDef::ammoPerShot,
			&WeaponCompleteDef::explodeCount,
			&WeaponCompleteDef::unknown3900,
			&WeaponCompleteDef::unknown3928,
			json_reflect::manual("stowTag"),
			json_reflect::manual("unknownTag3948"),
			&WeaponCompleteDef::rattleSoundType,
			&WeaponCompleteDef::adsShouldShowCrosshair,
			&WeaponCompleteDef::unknown3954,
			&WeaponCompleteDef::adsCrosshairShouldScale,
			&WeaponCompleteDef::turretADSEnabled,
			&WeaponCompleteDef::unknown3957,
			&WeaponCompleteDef::meleeOverrideValues,
			&WeaponCompleteDef::unknown3959,
			&WeaponCompleteDef::unknown3960,
			&WeaponCompleteDef::unknown3961,
			&WeaponCompleteDef::lockonSupported,
			&WeaponCompleteDef::requireLockonToFire,
			&WeaponCompleteDef::isAirburstWeapon,
			&WeaponCompleteDef::bigExplosion,
			&WeaponCompleteDef::noAdsWhenMagEmpty,
			&WeaponCompleteDef::unknown3967,
			&WeaponCompleteDef::inheritsPerks,
			&WeaponCompleteDef::crosshairColorChange,
			&WeaponCompleteDef::rifleBullet,
			&WeaponCompleteDef::armorPiercing,
			&WeaponCompleteDef::boltAction,
			&WeaponCompleteDef::aimDownSight,
			&WeaponCompleteDef::unknown3974,
			&WeaponCompleteDef::canHoldBreath,
			&WeaponCompleteDef::unknown3976,
			&WeaponCompleteDef::unknown3977,
			&WeaponCompleteDef::bulletExplosiveDamage,
			&WeaponCompleteDef::cookOffHold,
			&WeaponCompleteDef::unknown3980,
			&WeaponCompleteDef::reticleSpin45,
			&WeaponCompleteDef::unknown3982,
			&WeaponCompleteDef::clipOnly,
			&WeaponCompleteDef::noAmmoPickup,
			&WeaponCompleteDef::unknown3985,
			&WeaponCompleteDef::unknown3986,
			&WeaponCompleteDef::unknown3987,
			&WeaponCompleteDef::unknown3988,
			&WeaponCompleteDef::motionTracker,
			&WeaponCompleteDef::markableViewmodel,
			&WeaponCompleteDef::unknown3991,
			&WeaponCompleteDef::flipKillIcon,
			&WeaponCompleteDef::dpadIconShowsAmmo,
			&WeaponCompleteDef::noPartialReload,
			&WeaponCompleteDef::segmentedReload,
			&WeaponCompleteDef::multipleReload,
			&WeaponCompleteDef::blocksProne,
			&WeaponCompleteDef::silenced,
			&WeaponCompleteDef::isRollingGrenade,
			&WeaponCompleteDef::projExplosionEffectForceNormalUp,
			&WeaponCompleteDef::projExplosionEffectInheritParentDirection,
			&WeaponCompleteDef::unknown4002,
			&WeaponCompleteDef::projImpactExplode,
			&WeaponCompleteDef::unknown4004,
			&WeaponCompleteDef::unknown4005,
			&WeaponCompleteDef::stickToPlayers,
			&WeaponCompleteDef::stickToVehicles,
			&WeaponCompleteDef::stickToTurrets,
			&WeaponCompleteDef::thrownSideways,
			&WeaponCompleteDef::detonatesOnEmptyThrow,
			&WeaponCompleteDef::detonatesOnDoubleTap,
			&WeaponCompleteDef::disableFiring,
			&WeaponCompleteDef::timedDetonation,
			&WeaponCompleteDef::unknown4014,
			&WeaponCompleteDef::fuseLitAfterImpact,
			&WeaponCompleteDef::rotate,
			&WeaponCompleteDef::holdButtonToThrow,
			&WeaponCompleteDef::freezeMovementWhenFiring,
			&WeaponCompleteDef::unknown4019,
			&WeaponCompleteDef::unknown4020,
			&WeaponCompleteDef::turretBarrelSpinEnabled,
			&WeaponCompleteDef::missileConeSoundEnabled,
			&WeaponCompleteDef::missileConeSoundPitchshiftEnabled,
			&WeaponCompleteDef::missileConeSoundCrossfadeEnabled,
			&WeaponCompleteDef::offhandHoldIsCancelable,
			&WeaponCompleteDef::doNotAllowAttachmentsToOverrideSpread,
			&WeaponCompleteDef::unknown4027,
			&WeaponCompleteDef::useDualMagReloadAnims,
			&WeaponCompleteDef::unknown4029,
			&WeaponCompleteDef::unknown4030,
			&WeaponCompleteDef::alwaysShatterGlassOnImpact,
			&WeaponCompleteDef::unknown4032,
			&WeaponCompleteDef::unknown4033,
			&WeaponCompleteDef::unknown4034,
			&WeaponCompleteDef::unknown4035,
			&WeaponCompleteDef::counterSilencer,
			&WeaponCompleteDef::projSuppressedByEMP,
			&WeaponCompleteDef::projDisabledByEMP,
			&WeaponCompleteDef::unknown4039,
			&WeaponCompleteDef::unknown4040,
			&WeaponCompleteDef::unknown4041,
			&WeaponCompleteDef::projPlayTrailEffectForOwnerOnly,
			&WeaponCompleteDef::projPlayBeaconEffectForOwnerOnly,
			&WeaponCompleteDef::projKillTrailEffectOnDeath,
			&WeaponCompleteDef::projKillBeaconEffectOnDeath,
			&WeaponCompleteDef::unknown4046,
			&WeaponCompleteDef::cloaked,
			&WeaponCompleteDef::unknown4048,
			&WeaponCompleteDef::hasTransientModels,
			&WeaponCompleteDef::unknown4050,
			&WeaponCompleteDef::useScriptCallbackForHit,
			&WeaponCompleteDef::unknown4052,
			&WeaponCompleteDef::unknown4053,
			&WeaponCompleteDef::unknown4054,
			&WeaponCompleteDef::unknown4055,
			&WeaponCompleteDef::adsDofPhysicalFstop,
			&WeaponCompleteDef::adsDofPhysicalFocusDistance,
			&WeaponCompleteDef::unknown4064,
			&WeaponCompleteDef::unknown4072,
			&WeaponCompleteDef::unknown4076,
			&WeaponCompleteDef::unknown4092,
			&WeaponCompleteDef::unknown4096,
			&WeaponCompleteDef::unknown4100,
			&WeaponCompleteDef::unknown4104,
			&WeaponCompleteDef::unknown4200,
			&WeaponCompleteDef::unknown4208,
			&WeaponCompleteDef::unknown4224,
			&WeaponCompleteDef::unknown4228,
			&WeaponCompleteDef::unknown4232,
		};

		constexpr json_reflect::options json_options{.strings = json_reflect::null_string::null};

		std::string json_path(const std::string& name)
		{
			return "weapons\\" + name + ".json";
		}

		std::size_t anim_index(const std::string& name)
		{
			for (auto i = 0u; i < xanim_count; i++)
			{
				if (!_stricmp(anim_names[i], name.data()))
				{
					return i;
				}
			}

			ZONETOOL_FATAL("Unknown weapon anim \"%s\"", name.data());
		}

		void read_anim_array(json& data, XAnimParts**& anims, zone_memory* mem)
		{
			if (!data.is_object())
			{
				return;
			}

			anims = mem->allocate<XAnimParts*>(xanim_count);
			for (const auto& [name, anim] : data.items())
			{
				json_reflect::read(anim, anims[anim_index(name)], mem);
			}
		}

		void write_anim_array(ordered_json& data, XAnimParts* const* anims)
		{
			if (!anims)
			{
				data = nullptr;
				return;
			}

			data = ordered_json::object();
			for (auto i = 0u; i < xanim_count; i++)
			{
				if (anims[i])
				{
					json_reflect::write(data[anim_names[i]], anims[i]);
				}
			}
		}

		void read_attachments(json& data, WeaponCompleteDef& weapon, zone_memory* mem)
		{
			if (!data.is_array() || data.empty())
			{
				return;
			}

			weapon.numAttachments = static_cast<unsigned char>(data.size());
			weapon.attachments = mem->allocate<WeaponAttachment*>(data.size());
			for (auto i = 0u; i < data.size(); i++)
			{
				const auto name = data[i].get<std::string>();
				if (!name.empty())
				{
					weapon.attachments[i] = mem->allocate<WeaponAttachment>();
					weapon.attachments[i]->szInternalName = mem->duplicate_string(name);
				}
			}
		}

		void write_attachments(ordered_json& data, const WeaponCompleteDef& weapon)
		{
			for (auto i = 0u; weapon.attachments && i < weapon.numAttachments; i++)
			{
				json_reflect::write(data[i], weapon.attachments[i] ? asset_name(weapon.attachments[i]) : nullptr);
			}
		}

		void read_notetrack_map(json& data, scr_string_t*& keys, scr_string_t*& values, const std::size_t count, zone_memory* mem)
		{
			if (!data.is_array())
			{
				return;
			}

			keys = mem->allocate<scr_string_t>(count);
			values = mem->allocate<scr_string_t>(count);
			for (auto i = 0u; i < count && i < data.size(); i++)
			{
				keys[i] = parse_script_string(data[i]["Key"]);
				values[i] = parse_script_string(data[i]["Value"]);
			}
		}

		void write_notetrack_map(ordered_json& data, const scr_string_t* keys, const scr_string_t* values, const std::size_t count)
		{
			if (!keys || !values)
			{
				data = nullptr;
				return;
			}

			for (auto i = 0u; i < count; i++)
			{
				dump_script_string(data[i]["Key"], keys[i]);
				dump_script_string(data[i]["Value"], values[i]);
			}
		}

		void read_notetrack_fx_map(json& data, WeaponCompleteDef& weapon, zone_memory* mem)
		{
			if (!data.is_array())
			{
				return;
			}

			weapon.notetrackFXMapKeys = mem->allocate<scr_string_t>(notetrack_map_count);
			weapon.notetrackFXMapValues = mem->allocate<FxEffectDef*>(notetrack_map_count);
			weapon.notetrackFXMapTagValues = mem->allocate<scr_string_t>(notetrack_map_count);
			for (auto i = 0u; i < notetrack_map_count && i < data.size(); i++)
			{
				weapon.notetrackFXMapKeys[i] = parse_script_string(data[i]["Key"]);
				json_reflect::read(data[i]["Value"], weapon.notetrackFXMapValues[i], mem);
				weapon.notetrackFXMapTagValues[i] = parse_script_string(data[i]["Tag"]);
			}
		}

		void write_notetrack_fx_map(ordered_json& data, const WeaponCompleteDef& weapon)
		{
			if (!weapon.notetrackFXMapKeys || !weapon.notetrackFXMapValues || !weapon.notetrackFXMapTagValues)
			{
				data = nullptr;
				return;
			}

			for (auto i = 0u; i < notetrack_map_count; i++)
			{
				dump_script_string(data[i]["Key"], weapon.notetrackFXMapKeys[i]);
				json_reflect::write(data[i]["Value"], weapon.notetrackFXMapValues[i]);
				dump_script_string(data[i]["Tag"], weapon.notetrackFXMapTagValues[i]);
			}
		}

		void read_notetrack_overrides(json& data, WeaponCompleteDef& weapon, zone_memory* mem)
		{
			if (!data.is_array() || data.empty())
			{
				return;
			}

			weapon.numNotetrackOverrides = static_cast<unsigned char>(data.size());
			weapon.notetrackOverrides = mem->allocate<NoteTrackToSoundEntry>(data.size());
			for (auto i = 0u; i < data.size(); i++)
			{
				auto& entry = weapon.notetrackOverrides[i];
				entry.attachment = data[i]["attachment"].get<int>();
				read_notetrack_map(data[i]["notetrackSoundMap"], entry.notetrackSoundMapKeys, entry.notetrackSoundMapValues, notetrack_sound_count, mem);
			}
		}

		void write_notetrack_overrides(ordered_json& data, const WeaponCompleteDef& weapon)
		{
			for (auto i = 0u; weapon.notetrackOverrides && i < weapon.numNotetrackOverrides; i++)
			{
				const auto& entry = weapon.notetrackOverrides[i];
				data[i]["attachment"] = entry.attachment;
				write_notetrack_map(data[i]["notetrackSoundMap"], entry.notetrackSoundMapKeys, entry.notetrackSoundMapValues, notetrack_sound_count);
			}
		}

		vec2_t* read_knots(json& data, const std::size_t count, zone_memory* mem)
		{
			if (!data.is_array() || !count)
			{
				return nullptr;
			}

			auto* knots = mem->allocate<vec2_t>(count);
			for (auto i = 0u; i < count; i++)
			{
				json_reflect::read(data[i], knots[i], mem);
			}

			return knots;
		}

		void write_knots(ordered_json& data, const vec2_t* knots, const std::size_t count)
		{
			for (auto i = 0u; knots && i < count; i++)
			{
				json_reflect::write(data[i], knots[i]);
			}
		}

		void read_accuracy_graphs(json& data, WeaponCompleteDef& weapon, zone_memory* mem)
		{
			for (auto i = 0u; i < std::size(weapon.accuracyGraphName) && i < data.size(); i++)
			{
				auto& graph = data[i];
				const auto count = graph["knots"].size();
				if (graph["originalKnots"].is_array() && graph["originalKnots"].size() != count)
				{
					ZONETOOL_FATAL("Accuracy graph %u of weapon \"%s\" has a different original knot count", i, weapon.szInternalName);
				}

				if (graph["name"].is_string())
				{
					json_reflect::read(graph["name"], weapon.accuracyGraphName[i], mem, json_options);
				}

				weapon.accuracyGraphKnotCount[i] = static_cast<unsigned short>(count);
				weapon.accuracyGraphKnots[i] = read_knots(graph["knots"], count, mem);
				weapon.originalAccuracyGraphKnots[i] = read_knots(graph["originalKnots"], count, mem);
			}
		}

		void write_accuracy_graphs(ordered_json& data, const WeaponCompleteDef& weapon)
		{
			for (auto i = 0u; i < std::size(weapon.accuracyGraphName); i++)
			{
				auto& graph = data[i];
				json_reflect::write(graph["name"], weapon.accuracyGraphName[i], json_options);
				write_knots(graph["knots"], weapon.accuracyGraphKnots[i], weapon.accuracyGraphKnotCount[i]);
				write_knots(graph["originalKnots"], weapon.originalAccuracyGraphKnots[i], weapon.accuracyGraphKnotCount[i]);
			}
		}

		template <typename T>
		bool equal_entries(const T* entries, const T* other, const std::size_t count)
		{
			if constexpr (std::is_pointer_v<T>)
			{
				for (auto i = 0u; i < count; i++)
				{
					if (entries[i] != other[i] && (!entries[i] || !other[i] || std::strcmp(asset_name(entries[i]), asset_name(other[i]))))
					{
						return false;
					}
				}

				return true;
			}
			else
			{
				return !std::memcmp(entries, other, sizeof(T) * count);
			}
		}

		template <typename T>
		void share_if_equal(T*& field, T* source, const std::size_t count)
		{
			if (field && source && count && equal_entries(field, source, count))
			{
				field = source;
			}
		}

		void share_equal_arrays(WeaponCompleteDef& weapon)
		{
			share_if_equal(weapon.szXAnimsLeftHanded, weapon.szXAnimsRightHanded, xanim_count);
			share_if_equal(weapon.szXAnims, weapon.szXAnimsRightHanded, xanim_count);
			share_if_equal(weapon.worldModel, weapon.gunModel, model_count);
			share_if_equal(weapon.perpendicularBounce, weapon.parallelBounce, surface_type_count);
			for (auto i = 0u; i < std::size(weapon.accuracyGraphKnots); i++)
			{
				share_if_equal(weapon.originalAccuracyGraphKnots[i], weapon.accuracyGraphKnots[i], weapon.accuracyGraphKnotCount[i]);
			}
		}

		template <typename T>
		void add_field_offset(std::unordered_map<std::string, std::uint16_t>& offsets, const std::string& name, const T& field, const WeaponCompleteDef& weapon)
		{
			if constexpr (std::is_array_v<T>)
			{
				for (auto i = 0u; i < std::extent_v<T>; i++)
				{
					add_field_offset(offsets, name + "[" + std::to_string(i) + "]", field[i], weapon);
				}
			}
			else
			{
				const auto offset = reinterpret_cast<const char*>(&field) - reinterpret_cast<const char*>(&weapon);
				offsets.emplace(name, static_cast<std::uint16_t>(offset));
			}
		}

		template <typename T>
		void add_reflected_offsets(std::unordered_map<std::string, std::uint16_t>& offsets, const T& object, const std::string& prefix, const WeaponCompleteDef& weapon)
		{
			constexpr auto keys = glz::reflect<T>::keys;
			[&]<std::size_t... index>(std::index_sequence<index...>)
			{
				(add_field_offset(offsets, prefix + std::string(keys[index]), glz::get_member(object, glz::get<index>(glz::to_tie(object))), weapon), ...);
			}(std::make_index_sequence<keys.size()>{});
		}

		template <const auto& fields>
		void add_member_offsets(std::unordered_map<std::string, std::uint16_t>& offsets, const auto& object, const std::string& prefix, const WeaponCompleteDef& weapon);

		template <const auto& fields, std::size_t index>
		void add_member_offset(std::unordered_map<std::string, std::uint16_t>& offsets, const auto& object, const std::string& prefix, const WeaponCompleteDef& weapon)
		{
			constexpr auto& field = glz::get<index>(fields);
			using field_type = std::remove_cvref_t<decltype(field)>;
			if constexpr (json_reflect::group_entry<field_type>)
			{
				add_member_offsets<*field.fields>(offsets, object, prefix, weapon);
			}
			else if constexpr (!std::is_same_v<field_type, json_reflect::manual_field>)
			{
				const auto name = prefix + std::string(json_reflect::detail::field_key<fields, index>());
				const auto& member = object.*json_reflect::detail::field_member<fields, index>();
				if constexpr (!json_reflect::nested_entry<field_type> || std::is_pointer_v<std::remove_cvref_t<decltype(member)>>)
				{
					add_field_offset(offsets, name, member, weapon);
				}
				else if constexpr (field_type::has_fields)
				{
					add_member_offsets<*field.fields>(offsets, member, name + ".", weapon);
				}
				else
				{
					add_reflected_offsets(offsets, member, name + ".", weapon);
				}
			}
		}

		template <const auto& fields>
		void add_member_offsets(std::unordered_map<std::string, std::uint16_t>& offsets, const auto& object, const std::string& prefix, const WeaponCompleteDef& weapon)
		{
			[&]<std::size_t... index>(std::index_sequence<index...>)
			{
				(add_member_offset<fields, index>(offsets, object, prefix, weapon), ...);
			}(std::make_index_sequence<glz::tuple_size_v<std::remove_cvref_t<decltype(fields)>>>{});
		}

		constexpr auto manual_weapon_fields = glz::tuple
		{
			&WeaponCompleteDef::szXAnimsRightHanded,
			&WeaponCompleteDef::szXAnimsLeftHanded,
			&WeaponCompleteDef::hideTags,
			&WeaponCompleteDef::attachments,
			&WeaponCompleteDef::szXAnims,
			&WeaponCompleteDef::notetrackOverrides,
			&WeaponCompleteDef::notetrackSoundMapKeys,
			&WeaponCompleteDef::notetrackSoundMapValues,
			&WeaponCompleteDef::notetrackRumbleMapKeys,
			&WeaponCompleteDef::notetrackRumbleMapValues,
			&WeaponCompleteDef::notetrackFXMapKeys,
			&WeaponCompleteDef::notetrackFXMapValues,
			&WeaponCompleteDef::notetrackFXMapTagValues,
			&WeaponCompleteDef::numAttachments,
			&WeaponCompleteDef::numAnimOverrides,
			&WeaponCompleteDef::numSoundOverrides,
			&WeaponCompleteDef::numFXOverrides,
			&WeaponCompleteDef::numReloadStateTimerOverrides,
			&WeaponCompleteDef::numNotetrackOverrides,
			&WeaponCompleteDef::accuracyGraphName,
			&WeaponCompleteDef::accuracyGraphKnots,
			&WeaponCompleteDef::originalAccuracyGraphKnots,
			&WeaponCompleteDef::accuracyGraphKnotCount,
			&WeaponCompleteDef::knifeAttachTagOverride,
			&WeaponCompleteDef::stowTag,
			&WeaponCompleteDef::unknownTag3948,
		};

		const std::unordered_map<std::string, std::uint16_t>& weapon_field_offsets()
		{
			static const auto offsets = []
			{
				std::unordered_map<std::string, std::uint16_t> result;
				const auto weapon = std::make_unique<WeaponCompleteDef>();
				add_member_offsets<weapon_fields>(result, *weapon, "", *weapon);
				add_member_offsets<manual_weapon_fields>(result, *weapon, "", *weapon);
				return result;
			}();

			return offsets;
		}

		template <typename T>
		std::span<T*> pointer_run(T*& first, T*& last)
		{
			return {&first, &last + 1};
		}

		template <typename R>
		void read_assets(zone_reader& reader, const XAssetType type, R&& assets)
		{
			for (auto& asset : assets)
			{
				reader.read_asset(type, asset);
			}
		}

		template <typename R>
		void read_sounds(zone_reader& reader, R&& sounds)
		{
			for (auto& sound : sounds)
			{
				reader.read_name_reference(sound);
			}
		}

		template <typename R, typename T>
		void write_assets(zone_base* zone, const XAssetType type, R&& assets, T** dest)
		{
			for (std::size_t i = 0; i < std::size(assets); i++)
			{
				dest[i] = asset_reference(zone, type, assets[i]);
			}
		}

		template <typename R>
		void write_sounds(zone_buffer* buf, R&& sounds, snd_alias_list_t** dest)
		{
			for (std::size_t i = 0; i < std::size(sounds); i++)
			{
				write_sound(buf, sounds[i], dest[i]);
			}
		}

		template <typename R>
		void add_dependencies(zone_base* zone, const XAssetType type, R&& assets)
		{
			for (auto* asset : assets)
			{
				add_dependency(zone, type, asset);
			}
		}

		void read_overrides(zone_reader& reader, WeaponCompleteDef* asset)
		{
			if (const auto anims = reader.read_array(asset->animOverrides, 7, asset->numAnimOverrides))
			{
				for (auto i = 0; i < asset->numAnimOverrides; i++)
				{
					reader.read_asset(ASSET_TYPE_XANIM, anims[i].overrideAnim);
					reader.read_asset(ASSET_TYPE_XANIM, anims[i].altmodeAnim);
				}
			}

			if (const auto sounds = reader.read_array(asset->soundOverrides, 7, asset->numSoundOverrides))
			{
				for (auto i = 0; i < asset->numSoundOverrides; i++)
				{
					reader.read_name_reference(sounds[i].overrideSound);
					reader.read_name_reference(sounds[i].altmodeSound);
				}
			}

			if (const auto effects = reader.read_array(asset->fxOverrides, 7, asset->numFXOverrides))
			{
				for (auto i = 0; i < asset->numFXOverrides; i++)
				{
					reader.read_asset(ASSET_TYPE_FX, effects[i].overrideFX);
					reader.read_asset(ASSET_TYPE_FX, effects[i].altmodeFX);
				}
			}

			reader.read_array(asset->reloadOverrides, 3, asset->numReloadStateTimerOverrides);

			if (const auto notetracks = reader.read_array(asset->notetrackOverrides, 7, asset->numNotetrackOverrides))
			{
				for (auto i = 0; i < asset->numNotetrackOverrides; i++)
				{
					reader.read_script_string_array(notetracks[i].notetrackSoundMapKeys, notetrack_sound_count);
					reader.read_script_string_array(notetracks[i].notetrackSoundMapValues, notetrack_sound_count);
				}
			}
		}

		void read_notetrack_maps(zone_reader& reader, WeaponCompleteDef* asset)
		{
			reader.read_script_string_array(asset->notetrackSoundMapKeys, notetrack_sound_count);
			reader.read_script_string_array(asset->notetrackSoundMapValues, notetrack_sound_count);
			reader.read_script_string_array(asset->notetrackRumbleMapKeys, notetrack_map_count);
			reader.read_script_string_array(asset->notetrackRumbleMapValues, notetrack_map_count);
			reader.read_script_string_array(asset->notetrackFXMapKeys, notetrack_map_count);
			reader.read_asset_array(ASSET_TYPE_FX, asset->notetrackFXMapValues, notetrack_map_count);
			reader.read_script_string_array(asset->notetrackFXMapTagValues, notetrack_map_count);
		}

		void read_turret(zone_reader& reader, WeaponCompleteDef* asset)
		{
			reader.read_name_reference(asset->turretOverheatSound);
			reader.read_asset(ASSET_TYPE_FX, asset->turretOverheatEffect);
			reader.read_string(asset->turretBarrelSpinRumble);
			reader.read_name_reference(asset->turretBarrelSpinMaxSnd);
			read_sounds(reader, asset->turretBarrelSpinUpSnd);
			read_sounds(reader, asset->turretBarrelSpinDownSnd);
			reader.read_name_reference(asset->missileConeSoundAlias);
			reader.read_name_reference(asset->missileConeSoundAliasAtBase);
			reader.read_asset(ASSET_TYPE_XMODEL, asset->stowOffsetModel);

			if (const auto hydraulics = reader.read_array(asset->turretHydraulicSettings, 7))
			{
				read_sounds(reader, pointer_run(hydraulics->verticalSound, hydraulics->horizontalStopSound));
				read_assets(reader, ASSET_TYPE_SOUND, hydraulics->movementSounds);
			}
		}

		void read_projectile(zone_reader& reader, WeaponCompleteDef* asset)
		{
			reader.read_string(asset->projectileName);
			reader.read_asset(ASSET_TYPE_XMODEL, asset->projectileModel);
			read_assets(reader, ASSET_TYPE_FX, pointer_run(asset->projExplosionEffect, asset->unknownEffect2880));
			read_sounds(reader, pointer_run(asset->projExplosionSound, asset->unknownSound2904));
			reader.read_array(asset->parallelBounce, 3, surface_type_count);
			reader.read_array(asset->perpendicularBounce, 3, surface_type_count);
			read_assets(reader, ASSET_TYPE_FX, pointer_run(asset->projTrailEffect, asset->projBeaconEffect));
			reader.read_asset(ASSET_TYPE_FX, asset->projIgnitionEffect);
			reader.read_name_reference(asset->projIgnitionSound);
		}

		void write_overrides(zone_base* zone, zone_buffer* buf, WeaponCompleteDef* data, WeaponCompleteDef* dest)
		{
			if (auto* anims = write_array(buf, 7, data->animOverrides, data->numAnimOverrides, dest->animOverrides))
			{
				for (auto i = 0; i < data->numAnimOverrides; i++)
				{
					anims[i].overrideAnim = asset_reference(zone, ASSET_TYPE_XANIM, data->animOverrides[i].overrideAnim);
					anims[i].altmodeAnim = asset_reference(zone, ASSET_TYPE_XANIM, data->animOverrides[i].altmodeAnim);
				}
			}

			if (auto* sounds = write_array(buf, 7, data->soundOverrides, data->numSoundOverrides, dest->soundOverrides))
			{
				for (auto i = 0; i < data->numSoundOverrides; i++)
				{
					write_sound(buf, data->soundOverrides[i].overrideSound, sounds[i].overrideSound);
					write_sound(buf, data->soundOverrides[i].altmodeSound, sounds[i].altmodeSound);
				}
			}

			if (auto* effects = write_array(buf, 7, data->fxOverrides, data->numFXOverrides, dest->fxOverrides))
			{
				for (auto i = 0; i < data->numFXOverrides; i++)
				{
					effects[i].overrideFX = asset_reference(zone, ASSET_TYPE_FX, data->fxOverrides[i].overrideFX);
					effects[i].altmodeFX = asset_reference(zone, ASSET_TYPE_FX, data->fxOverrides[i].altmodeFX);
				}
			}

			write_array(buf, 3, data->reloadOverrides, data->numReloadStateTimerOverrides, dest->reloadOverrides);

			if (auto* notetracks = write_array(buf, 7, data->notetrackOverrides, data->numNotetrackOverrides, dest->notetrackOverrides))
			{
				for (auto i = 0; i < data->numNotetrackOverrides; i++)
				{
					const auto& notetrack = data->notetrackOverrides[i];
					write_array(buf, 3, notetrack.notetrackSoundMapKeys, notetrack_sound_count, notetracks[i].notetrackSoundMapKeys);
					write_array(buf, 3, notetrack.notetrackSoundMapValues, notetrack_sound_count, notetracks[i].notetrackSoundMapValues);
				}
			}
		}

		void write_notetrack_maps(zone_base* zone, zone_buffer* buf, WeaponCompleteDef* data, WeaponCompleteDef* dest)
		{
			write_array(buf, 3, data->notetrackSoundMapKeys, notetrack_sound_count, dest->notetrackSoundMapKeys);
			write_array(buf, 3, data->notetrackSoundMapValues, notetrack_sound_count, dest->notetrackSoundMapValues);
			write_array(buf, 3, data->notetrackRumbleMapKeys, notetrack_map_count, dest->notetrackRumbleMapKeys);
			write_array(buf, 3, data->notetrackRumbleMapValues, notetrack_map_count, dest->notetrackRumbleMapValues);
			write_array(buf, 3, data->notetrackFXMapKeys, notetrack_map_count, dest->notetrackFXMapKeys);
			write_asset_array(zone, buf, ASSET_TYPE_FX, data->notetrackFXMapValues, notetrack_map_count, dest->notetrackFXMapValues);
			write_array(buf, 3, data->notetrackFXMapTagValues, notetrack_map_count, dest->notetrackFXMapTagValues);
		}

		void write_turret(zone_base* zone, zone_buffer* buf, WeaponCompleteDef* data, WeaponCompleteDef* dest)
		{
			write_sound(buf, data->turretOverheatSound, dest->turretOverheatSound);
			dest->turretOverheatEffect = asset_reference(zone, ASSET_TYPE_FX, data->turretOverheatEffect);
			write_string(buf, data->turretBarrelSpinRumble, dest->turretBarrelSpinRumble);
			write_sound(buf, data->turretBarrelSpinMaxSnd, dest->turretBarrelSpinMaxSnd);
			write_sounds(buf, data->turretBarrelSpinUpSnd, dest->turretBarrelSpinUpSnd);
			write_sounds(buf, data->turretBarrelSpinDownSnd, dest->turretBarrelSpinDownSnd);
			write_sound(buf, data->missileConeSoundAlias, dest->missileConeSoundAlias);
			write_sound(buf, data->missileConeSoundAliasAtBase, dest->missileConeSoundAliasAtBase);
			dest->stowOffsetModel = asset_reference(zone, ASSET_TYPE_XMODEL, data->stowOffsetModel);

			if (auto* hydraulics = write_array(buf, 7, data->turretHydraulicSettings, 1, dest->turretHydraulicSettings))
			{
				auto* settings = data->turretHydraulicSettings;
				write_sounds(buf, pointer_run(settings->verticalSound, settings->horizontalStopSound), &hydraulics->verticalSound);
				write_assets(zone, ASSET_TYPE_SOUND, settings->movementSounds, hydraulics->movementSounds);
			}
		}

		void write_projectile(zone_base* zone, zone_buffer* buf, WeaponCompleteDef* data, WeaponCompleteDef* dest)
		{
			write_string(buf, data->projectileName, dest->projectileName);
			dest->projectileModel = asset_reference(zone, ASSET_TYPE_XMODEL, data->projectileModel);
			write_assets(zone, ASSET_TYPE_FX, pointer_run(data->projExplosionEffect, data->unknownEffect2880), &dest->projExplosionEffect);
			write_sounds(buf, pointer_run(data->projExplosionSound, data->unknownSound2904), &dest->projExplosionSound);
			write_array(buf, 3, data->parallelBounce, surface_type_count, dest->parallelBounce);
			write_array(buf, 3, data->perpendicularBounce, surface_type_count, dest->perpendicularBounce);
			write_assets(zone, ASSET_TYPE_FX, pointer_run(data->projTrailEffect, data->projBeaconEffect), &dest->projTrailEffect);
			dest->projIgnitionEffect = asset_reference(zone, ASSET_TYPE_FX, data->projIgnitionEffect);
			write_sound(buf, data->projIgnitionSound, dest->projIgnitionSound);
		}
	}

	void weapon_def::read(zone_reader& reader, WeaponCompleteDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->szInternalName);
		reader.read_string(asset->szDisplayName);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->gunModel, model_count);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->gunModelConditions, model_count);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->handModel);
		reader.read_string(asset->lobWorldModelName);
		reader.read_asset_array(ASSET_TYPE_XANIM, asset->szXAnimsRightHanded, xanim_count);
		reader.read_asset_array(ASSET_TYPE_XANIM, asset->szXAnimsLeftHanded, xanim_count);
		reader.read_script_string_array(asset->hideTags, hide_tag_count);
		reader.read_asset_array(ASSET_TYPE_ATTACHMENT, asset->attachments, asset->numAttachments);
		reader.read_asset_array(ASSET_TYPE_XANIM, asset->szXAnims, xanim_count);
		read_overrides(reader, asset);
		read_notetrack_maps(reader, asset);
		reader.read_string(asset->szAdsrBaseSetting);
		read_assets(reader, ASSET_TYPE_FX, pointer_run(asset->viewFlashEffect, asset->meleeMissEffect));
		read_sounds(reader, pointer_run(asset->pickupSound, asset->grenadeStickSound));
		reader.read_name_reference_array(asset->bounceSound, surface_type_count);
		reader.read_name_reference_array(asset->rollingSound, surface_type_count);
		read_assets(reader, ASSET_TYPE_FX, pointer_run(asset->viewShellEjectEffect, asset->viewMagEjectEffect));
		read_assets(reader, ASSET_TYPE_MATERIAL, pointer_run(asset->reticleCenter, asset->reticleSide));
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->worldModel, model_count);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->worldModelConditions, model_count);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->worldClipModel);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->rocketModel);
		read_assets(reader, ASSET_TYPE_MATERIAL, pointer_run(asset->hudIcon, asset->ammoCounterIcon));
		reader.read_string(asset->szAmmoName);
		reader.read_string(asset->szClipName);
		reader.read_string(asset->szSharedAmmoCapName);
		reader.read_asset(ASSET_TYPE_PHYSCOLLMAP, asset->physCollmap);
		reader.read_asset(ASSET_TYPE_PHYSPRESET, asset->physPreset);
		reader.read_string(asset->szUseHintString);
		reader.read_string(asset->dropHintString);
		reader.read_array(asset->locationDamageMultipliers, 3, location_damage_count);
		reader.read_string(asset->fireRumble);
		reader.read_string(asset->unknownRumble1040);
		reader.read_string(asset->meleeImpactRumble);
		read_assets(reader, ASSET_TYPE_TRACER, pointer_run(asset->tracerType, asset->unknownTracer1072));
		reader.read_asset(ASSET_TYPE_LASER, asset->laserType);
		read_assets(reader, ASSET_TYPE_BEAM, asset->beamType);
		read_turret(reader, asset);
		read_assets(reader, ASSET_TYPE_MATERIAL, pointer_run(asset->overlay.shader, asset->overlay.shaderEMPLowRes));
		read_assets(reader, ASSET_TYPE_MATERIAL, pointer_run(asset->killIcon, asset->hudProximityWarningIcon));
		read_projectile(reader, asset);

		for (auto i = 0u; i < std::size(asset->accuracyGraphName); i++)
		{
			reader.read_string(asset->accuracyGraphName[i]);
			reader.read_array(asset->accuracyGraphKnots[i], 3, asset->accuracyGraphKnotCount[i]);
			reader.read_array(asset->originalAccuracyGraphKnots[i], 3, asset->accuracyGraphKnotCount[i]);
		}

		reader.read_string(asset->szScript);
		reader.read_script_string(asset->knifeAttachTagOverride);
		reader.read_script_string(asset->stowTag);
		reader.read_script_string(asset->unknownTag3948);
		reader.pop_stream();
	}

	void weapon_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;
		script_string_preparer prepare(buf);
		prepare(data->hideTags, hide_tag_count);
		for (auto i = 0; data->notetrackOverrides && i < data->numNotetrackOverrides; i++)
		{
			prepare(data->notetrackOverrides[i].notetrackSoundMapKeys, notetrack_sound_count);
			prepare(data->notetrackOverrides[i].notetrackSoundMapValues, notetrack_sound_count);
		}
		prepare(data->notetrackSoundMapKeys, notetrack_sound_count);
		prepare(data->notetrackSoundMapValues, notetrack_sound_count);
		prepare(data->notetrackRumbleMapKeys, notetrack_map_count);
		prepare(data->notetrackRumbleMapValues, notetrack_map_count);
		prepare(data->notetrackFXMapKeys, notetrack_map_count);
		prepare(data->notetrackFXMapTagValues, notetrack_map_count);
		prepare(data->knifeAttachTagOverride);
		prepare(data->stowTag);
		prepare(data->unknownTag3948);
	}

	void weapon_def::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;
		add_dependencies(zone, ASSET_TYPE_XMODEL, data->gunModel, model_count);
		add_dependencies(zone, ASSET_TYPE_XMODEL, data->gunModelConditions, model_count);
		add_dependency(zone, ASSET_TYPE_XMODEL, data->handModel);
		add_dependencies(zone, ASSET_TYPE_XANIM, data->szXAnimsRightHanded, xanim_count);
		add_dependencies(zone, ASSET_TYPE_XANIM, data->szXAnimsLeftHanded, xanim_count);
		add_dependencies(zone, ASSET_TYPE_ATTACHMENT, data->attachments, data->numAttachments);
		add_dependencies(zone, ASSET_TYPE_XANIM, data->szXAnims, xanim_count);

		for (auto i = 0; data->animOverrides && i < data->numAnimOverrides; i++)
		{
			add_dependency(zone, ASSET_TYPE_XANIM, data->animOverrides[i].overrideAnim);
			add_dependency(zone, ASSET_TYPE_XANIM, data->animOverrides[i].altmodeAnim);
		}
		for (auto i = 0; data->soundOverrides && i < data->numSoundOverrides; i++)
		{
			add_dependency(zone, ASSET_TYPE_SOUND, data->soundOverrides[i].overrideSound);
			add_dependency(zone, ASSET_TYPE_SOUND, data->soundOverrides[i].altmodeSound);
		}
		for (auto i = 0; data->fxOverrides && i < data->numFXOverrides; i++)
		{
			add_dependency(zone, ASSET_TYPE_FX, data->fxOverrides[i].overrideFX);
			add_dependency(zone, ASSET_TYPE_FX, data->fxOverrides[i].altmodeFX);
		}

		add_dependencies(zone, ASSET_TYPE_FX, data->notetrackFXMapValues, notetrack_map_count);
		add_dependencies(zone, ASSET_TYPE_FX, pointer_run(data->viewFlashEffect, data->meleeMissEffect));
		add_dependencies(zone, ASSET_TYPE_SOUND, pointer_run(data->pickupSound, data->grenadeStickSound));
		add_dependencies(zone, ASSET_TYPE_SOUND, data->bounceSound, surface_type_count);
		add_dependencies(zone, ASSET_TYPE_SOUND, data->rollingSound, surface_type_count);
		add_dependencies(zone, ASSET_TYPE_FX, pointer_run(data->viewShellEjectEffect, data->viewMagEjectEffect));
		add_dependencies(zone, ASSET_TYPE_MATERIAL, pointer_run(data->reticleCenter, data->reticleSide));
		add_dependencies(zone, ASSET_TYPE_XMODEL, data->worldModel, model_count);
		add_dependencies(zone, ASSET_TYPE_XMODEL, data->worldModelConditions, model_count);
		add_dependency(zone, ASSET_TYPE_XMODEL, data->worldClipModel);
		add_dependency(zone, ASSET_TYPE_XMODEL, data->rocketModel);
		add_dependencies(zone, ASSET_TYPE_MATERIAL, pointer_run(data->hudIcon, data->ammoCounterIcon));
		add_dependency(zone, ASSET_TYPE_PHYSCOLLMAP, data->physCollmap);
		add_dependency(zone, ASSET_TYPE_PHYSPRESET, data->physPreset);
		add_dependencies(zone, ASSET_TYPE_TRACER, pointer_run(data->tracerType, data->unknownTracer1072));
		add_dependency(zone, ASSET_TYPE_LASER, data->laserType);
		add_dependencies(zone, ASSET_TYPE_BEAM, data->beamType);

		add_dependency(zone, ASSET_TYPE_SOUND, data->turretOverheatSound);
		add_dependency(zone, ASSET_TYPE_FX, data->turretOverheatEffect);
		add_dependency(zone, ASSET_TYPE_SOUND, data->turretBarrelSpinMaxSnd);
		add_dependencies(zone, ASSET_TYPE_SOUND, data->turretBarrelSpinUpSnd);
		add_dependencies(zone, ASSET_TYPE_SOUND, data->turretBarrelSpinDownSnd);
		add_dependency(zone, ASSET_TYPE_SOUND, data->missileConeSoundAlias);
		add_dependency(zone, ASSET_TYPE_SOUND, data->missileConeSoundAliasAtBase);
		add_dependency(zone, ASSET_TYPE_XMODEL, data->stowOffsetModel);
		if (auto* settings = data->turretHydraulicSettings)
		{
			add_dependencies(zone, ASSET_TYPE_SOUND, pointer_run(settings->verticalSound, settings->horizontalStopSound));
			add_dependencies(zone, ASSET_TYPE_SOUND, settings->movementSounds);
		}

		add_dependencies(zone, ASSET_TYPE_MATERIAL, pointer_run(data->overlay.shader, data->overlay.shaderEMPLowRes));
		add_dependencies(zone, ASSET_TYPE_MATERIAL, pointer_run(data->killIcon, data->hudProximityWarningIcon));
		add_dependency(zone, ASSET_TYPE_XMODEL, data->projectileModel);
		add_dependencies(zone, ASSET_TYPE_FX, pointer_run(data->projExplosionEffect, data->unknownEffect2880));
		add_dependencies(zone, ASSET_TYPE_SOUND, pointer_run(data->projExplosionSound, data->unknownSound2904));
		add_dependencies(zone, ASSET_TYPE_FX, pointer_run(data->projTrailEffect, data->projBeaconEffect));
		add_dependency(zone, ASSET_TYPE_FX, data->projIgnitionEffect);
		add_dependency(zone, ASSET_TYPE_SOUND, data->projIgnitionSound);
	}

	void weapon_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->szInternalName = this->write_name(buf);
		write_string(buf, data->szDisplayName, dest->szDisplayName);
		write_asset_array(zone, buf, ASSET_TYPE_XMODEL, data->gunModel, model_count, dest->gunModel);
		write_asset_array(zone, buf, ASSET_TYPE_XMODEL, data->gunModelConditions, model_count, dest->gunModelConditions);
		dest->handModel = asset_reference(zone, ASSET_TYPE_XMODEL, data->handModel);
		write_string(buf, data->lobWorldModelName, dest->lobWorldModelName);
		write_asset_array(zone, buf, ASSET_TYPE_XANIM, data->szXAnimsRightHanded, xanim_count, dest->szXAnimsRightHanded);
		write_asset_array(zone, buf, ASSET_TYPE_XANIM, data->szXAnimsLeftHanded, xanim_count, dest->szXAnimsLeftHanded);
		write_array(buf, 3, data->hideTags, hide_tag_count, dest->hideTags);
		write_asset_array(zone, buf, ASSET_TYPE_ATTACHMENT, data->attachments, data->numAttachments, dest->attachments);
		write_asset_array(zone, buf, ASSET_TYPE_XANIM, data->szXAnims, xanim_count, dest->szXAnims);
		write_overrides(zone, buf, data, dest);
		write_notetrack_maps(zone, buf, data, dest);
		write_string(buf, data->szAdsrBaseSetting, dest->szAdsrBaseSetting);
		write_assets(zone, ASSET_TYPE_FX, pointer_run(data->viewFlashEffect, data->meleeMissEffect), &dest->viewFlashEffect);
		write_sounds(buf, pointer_run(data->pickupSound, data->grenadeStickSound), &dest->pickupSound);
		write_sound_array(buf, data->bounceSound, surface_type_count, dest->bounceSound);
		write_sound_array(buf, data->rollingSound, surface_type_count, dest->rollingSound);
		write_assets(zone, ASSET_TYPE_FX, pointer_run(data->viewShellEjectEffect, data->viewMagEjectEffect), &dest->viewShellEjectEffect);
		write_assets(zone, ASSET_TYPE_MATERIAL, pointer_run(data->reticleCenter, data->reticleSide), &dest->reticleCenter);
		write_asset_array(zone, buf, ASSET_TYPE_XMODEL, data->worldModel, model_count, dest->worldModel);
		write_asset_array(zone, buf, ASSET_TYPE_XMODEL, data->worldModelConditions, model_count, dest->worldModelConditions);
		dest->worldClipModel = asset_reference(zone, ASSET_TYPE_XMODEL, data->worldClipModel);
		dest->rocketModel = asset_reference(zone, ASSET_TYPE_XMODEL, data->rocketModel);
		write_assets(zone, ASSET_TYPE_MATERIAL, pointer_run(data->hudIcon, data->ammoCounterIcon), &dest->hudIcon);
		write_string(buf, data->szAmmoName, dest->szAmmoName);
		write_string(buf, data->szClipName, dest->szClipName);
		write_string(buf, data->szSharedAmmoCapName, dest->szSharedAmmoCapName);
		dest->physCollmap = asset_reference(zone, ASSET_TYPE_PHYSCOLLMAP, data->physCollmap);
		dest->physPreset = asset_reference(zone, ASSET_TYPE_PHYSPRESET, data->physPreset);
		write_string(buf, data->szUseHintString, dest->szUseHintString);
		write_string(buf, data->dropHintString, dest->dropHintString);
		write_array(buf, 3, data->locationDamageMultipliers, location_damage_count, dest->locationDamageMultipliers);
		write_string(buf, data->fireRumble, dest->fireRumble);
		write_string(buf, data->unknownRumble1040, dest->unknownRumble1040);
		write_string(buf, data->meleeImpactRumble, dest->meleeImpactRumble);
		write_assets(zone, ASSET_TYPE_TRACER, pointer_run(data->tracerType, data->unknownTracer1072), &dest->tracerType);
		dest->laserType = asset_reference(zone, ASSET_TYPE_LASER, data->laserType);
		write_assets(zone, ASSET_TYPE_BEAM, data->beamType, dest->beamType);
		write_turret(zone, buf, data, dest);
		write_assets(zone, ASSET_TYPE_MATERIAL, pointer_run(data->overlay.shader, data->overlay.shaderEMPLowRes), &dest->overlay.shader);
		write_assets(zone, ASSET_TYPE_MATERIAL, pointer_run(data->killIcon, data->hudProximityWarningIcon), &dest->killIcon);
		write_projectile(zone, buf, data, dest);

		for (auto i = 0u; i < std::size(data->accuracyGraphName); i++)
		{
			write_string(buf, data->accuracyGraphName[i], dest->accuracyGraphName[i]);
			write_array(buf, 3, data->accuracyGraphKnots[i], data->accuracyGraphKnotCount[i], dest->accuracyGraphKnots[i]);
			write_array(buf, 3, data->originalAccuracyGraphKnots[i], data->accuracyGraphKnotCount[i], dest->originalAccuracyGraphKnots[i]);
		}

		write_string(buf, data->szScript, dest->szScript);

		buf->pop_stream();
	}

	WeaponCompleteDef* weapon_def::parse(const std::string& name, zone_memory* mem)
	{
		auto file = filesystem::file(json_path(name));
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing weapon \"%s\"...", name.data());

		file.open("rb");
		auto data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto* asset = mem->allocate<WeaponCompleteDef>();
		json_reflect::read<weapon_fields>(data, *asset, mem, json_options);

		read_anim_array(data["szXAnimsRightHanded"], asset->szXAnimsRightHanded, mem);
		read_anim_array(data["szXAnimsLeftHanded"], asset->szXAnimsLeftHanded, mem);
		asset->hideTags = parse_script_string_array(data["hideTags"], mem, hide_tag_count);
		read_attachments(data["attachments"], *asset, mem);
		read_anim_array(data["szXAnims"], asset->szXAnims, mem);
		read_notetrack_overrides(data["notetrackOverrides"], *asset, mem);
		read_notetrack_map(data["notetrackSoundMap"], asset->notetrackSoundMapKeys, asset->notetrackSoundMapValues, notetrack_sound_count, mem);
		read_notetrack_map(data["notetrackRumbleMap"], asset->notetrackRumbleMapKeys, asset->notetrackRumbleMapValues, notetrack_map_count, mem);
		read_notetrack_fx_map(data["notetrackFXMap"], *asset, mem);
		read_accuracy_graphs(data["accuracyGraphs"], *asset, mem);
		asset->knifeAttachTagOverride = parse_script_string(data["knifeAttachTagOverride"]);
		asset->stowTag = parse_script_string(data["stowTag"]);
		asset->unknownTag3948 = parse_script_string(data["unknownTag3948"]);

		share_equal_arrays(*asset);
		return asset;
	}

	void weapon_def::dump(WeaponCompleteDef* asset)
	{
		ordered_json data;
		json_reflect::write<weapon_fields>(data, *asset, json_options);

		write_anim_array(data["szXAnimsRightHanded"], asset->szXAnimsRightHanded);
		write_anim_array(data["szXAnimsLeftHanded"], asset->szXAnimsLeftHanded);
		dump_script_string_array(data["hideTags"], asset->hideTags, hide_tag_count);
		write_attachments(data["attachments"], *asset);
		write_anim_array(data["szXAnims"], asset->szXAnims);
		write_notetrack_overrides(data["notetrackOverrides"], *asset);
		write_notetrack_map(data["notetrackSoundMap"], asset->notetrackSoundMapKeys, asset->notetrackSoundMapValues, notetrack_sound_count);
		write_notetrack_map(data["notetrackRumbleMap"], asset->notetrackRumbleMapKeys, asset->notetrackRumbleMapValues, notetrack_map_count);
		write_notetrack_fx_map(data["notetrackFXMap"], *asset);
		write_accuracy_graphs(data["accuracyGraphs"], *asset);
		dump_script_string(data["knifeAttachTagOverride"], asset->knifeAttachTagOverride);
		dump_script_string(data["stowTag"], asset->stowTag);
		dump_script_string(data["unknownTag3948"], asset->unknownTag3948);

		const auto str = data.dump(4);
		auto file = filesystem::file(json_path(asset->szInternalName));
		file.open("wb");
		file.write(str.data(), str.size(), 1);
		file.close();
	}

	std::optional<std::string> weapon_field_name(const std::uint16_t offset)
	{
		for (const auto& [name, field_offset] : weapon_field_offsets())
		{
			if (field_offset == offset)
			{
				return name;
			}
		}

		return std::nullopt;
	}

	std::optional<std::uint16_t> weapon_field_offset(const std::string& name)
	{
		const auto& offsets = weapon_field_offsets();
		const auto offset = offsets.find(name);
		return offset != offsets.end() ? std::optional(offset->second) : std::nullopt;
	}
}
