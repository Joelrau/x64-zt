#include <std_include.hpp>
#include "weaponattachment.hpp"
#include "weapondef.hpp"

namespace zonetool::h1
{
	namespace
	{
		struct field_info
		{
			int json_index;
			unsigned short offset;
			unsigned char index;
			unsigned char type;
			unsigned char code;
		};

#define DEFINE_FIELD(__name__) {#__name__, static_cast<unsigned short>(offsetof(WeaponDef, __name__))}

		std::unordered_map<unsigned short, std::string> weapon_def_fields_rev;
		std::unordered_map<std::string, unsigned short> weapon_def_fields =
		{
			DEFINE_FIELD(name),
			DEFINE_FIELD(szDisplayName),
			DEFINE_FIELD(szOverlayName),
			DEFINE_FIELD(gunModel),
			DEFINE_FIELD(handModel),
			DEFINE_FIELD(unknownModel),
			DEFINE_FIELD(reticleViewModels),
			DEFINE_FIELD(szModeName),
			DEFINE_FIELD(szXAnimsRightHanded),
			DEFINE_FIELD(szXAnimsLeftHanded),
			DEFINE_FIELD(hideTags),
			DEFINE_FIELD(attachments),
			DEFINE_FIELD(szXAnims),
			DEFINE_FIELD(animOverrides),
			DEFINE_FIELD(soundOverrides),
			DEFINE_FIELD(fxOverrides),
			DEFINE_FIELD(reloadOverrides),
			DEFINE_FIELD(notetrackOverrides),
			DEFINE_FIELD(notetrackSoundMapKeys),
			DEFINE_FIELD(notetrackSoundMapValues),
			DEFINE_FIELD(notetrackRumbleMapKeys),
			DEFINE_FIELD(notetrackRumbleMapValues),
			DEFINE_FIELD(notetrackFXMapKeys),
			DEFINE_FIELD(notetrackFXMapValues),
			DEFINE_FIELD(notetrackFXMapTagValues),
			DEFINE_FIELD(notetrackUnknownKeys),
			DEFINE_FIELD(notetrackUnknown),
			DEFINE_FIELD(notetrackUnknownValues),
			DEFINE_FIELD(szAltWeaponName),
			DEFINE_FIELD(viewFlashEffect),
			DEFINE_FIELD(viewBodyFlashEffect),
			DEFINE_FIELD(worldFlashEffect),
			DEFINE_FIELD(viewFlashADSEffect),
			DEFINE_FIELD(viewBodyFlashADSEffect),
			DEFINE_FIELD(effect06),
			DEFINE_FIELD(effect07),
			DEFINE_FIELD(effect08),
			DEFINE_FIELD(effect09),
			DEFINE_FIELD(effect10),
			DEFINE_FIELD(effect11),
			DEFINE_FIELD(effect12),
			DEFINE_FIELD(pickupSound),
			DEFINE_FIELD(pickupSoundPlayer),
			DEFINE_FIELD(ammoPickupSound),
			DEFINE_FIELD(ammoPickupSoundPlayer),
			DEFINE_FIELD(projectileSound),
			DEFINE_FIELD(pullbackSound),
			DEFINE_FIELD(pullbackSoundPlayer),
			DEFINE_FIELD(pullbackSoundQuick),
			DEFINE_FIELD(pullbackSoundQuickPlayer),
			DEFINE_FIELD(fireSound),
			DEFINE_FIELD(fireSoundPlayer),
			DEFINE_FIELD(fireSoundPlayerAkimbo),
			DEFINE_FIELD(sound13),
			DEFINE_FIELD(sound14),
			DEFINE_FIELD(sound15),
			DEFINE_FIELD(sound16),
			DEFINE_FIELD(fireLoopSound),
			DEFINE_FIELD(fireLoopSoundPlayer),
			DEFINE_FIELD(sound19),
			DEFINE_FIELD(sound20),
			DEFINE_FIELD(sound21),
			DEFINE_FIELD(sound22),
			DEFINE_FIELD(fireLoopEndPointSound),
			DEFINE_FIELD(fireLoopEndPointSoundPlayer),
			DEFINE_FIELD(fireStopSound),
			DEFINE_FIELD(fireStopSoundPlayer),
			DEFINE_FIELD(sound27),
			DEFINE_FIELD(sound28),
			DEFINE_FIELD(sound29),
			DEFINE_FIELD(sound30),
			DEFINE_FIELD(fireLastShotSound),
			DEFINE_FIELD(fireLastShotSoundPlayer),
			DEFINE_FIELD(fireFirstSound),
			DEFINE_FIELD(fireFirstSoundPlayer),
			DEFINE_FIELD(fireLastSound),
			DEFINE_FIELD(fireLastSoundPlayer),
			DEFINE_FIELD(emptyFireSound),
			DEFINE_FIELD(emptyFireSoundPlayer),
			DEFINE_FIELD(sound39),
			DEFINE_FIELD(meleeSwipeSound),
			DEFINE_FIELD(meleeSwipeSoundPlayer),
			DEFINE_FIELD(meleeHitSound),
			DEFINE_FIELD(meleeHitSoundPlayer),
			DEFINE_FIELD(meleeMissSound),
			DEFINE_FIELD(meleeMissSoundPlayer),
			DEFINE_FIELD(rechamberSound),
			DEFINE_FIELD(rechamberSoundPlayer),
			DEFINE_FIELD(reloadSound),
			DEFINE_FIELD(reloadSoundPlayer),
			DEFINE_FIELD(reloadEmptySound),
			DEFINE_FIELD(reloadEmptySoundPlayer),
			DEFINE_FIELD(reloadStartSound),
			DEFINE_FIELD(reloadStartSoundPlayer),
			DEFINE_FIELD(reloadEndSound),
			DEFINE_FIELD(reloadEndSoundPlayer),
			DEFINE_FIELD(detonateSound),
			DEFINE_FIELD(detonateSoundPlayer),
			DEFINE_FIELD(nightVisionWearSound),
			DEFINE_FIELD(nightVisionWearSoundPlayer),
			DEFINE_FIELD(nightVisionRemoveSound),
			DEFINE_FIELD(nightVisionRemoveSoundPlayer),
			DEFINE_FIELD(raiseSound),
			DEFINE_FIELD(raiseSoundPlayer),
			DEFINE_FIELD(sound64),
			DEFINE_FIELD(sound65),
			DEFINE_FIELD(sound66),
			DEFINE_FIELD(sound67),
			DEFINE_FIELD(putawaySound),
			DEFINE_FIELD(putawaySoundPlayer),
			DEFINE_FIELD(sound70),
			DEFINE_FIELD(sound71),
			DEFINE_FIELD(adsEnterSoundPlayer),
			DEFINE_FIELD(adsLeaveSoundPlayer),
			DEFINE_FIELD(adsCrosshairEnemySound),
			DEFINE_FIELD(bounceSound),
			DEFINE_FIELD(rollingSound),
			DEFINE_FIELD(viewShellEjectEffect),
			DEFINE_FIELD(worldShellEjectEffect),
			DEFINE_FIELD(viewLastShotEjectEffect),
			DEFINE_FIELD(worldLastShotEjectEffect),
			DEFINE_FIELD(viewMagEjectEffect),
			DEFINE_FIELD(reticleCenter),
			DEFINE_FIELD(reticleSide),
			DEFINE_FIELD(worldModel),
			DEFINE_FIELD(worldClipModel),
			DEFINE_FIELD(rocketModel),
			DEFINE_FIELD(knifeModel),
			DEFINE_FIELD(worldKnifeModel),
			DEFINE_FIELD(hudIcon),
			DEFINE_FIELD(pickupIcon),
			DEFINE_FIELD(unknownIcon2),
			DEFINE_FIELD(unknownIcon3),
			DEFINE_FIELD(unknownIcon4),
			DEFINE_FIELD(ammoCounterIcon),
			DEFINE_FIELD(szAmmoName),
			DEFINE_FIELD(szClipName),
			DEFINE_FIELD(szSharedAmmoCapName),
			DEFINE_FIELD(physCollmap),
			DEFINE_FIELD(physPreset),
			DEFINE_FIELD(szUseHintString),
			DEFINE_FIELD(dropHintString),
			DEFINE_FIELD(locationDamageMultipliers),
			DEFINE_FIELD(fireRumble),
			DEFINE_FIELD(fireMedRumble),
			DEFINE_FIELD(fireHighRumble),
			DEFINE_FIELD(meleeImpactRumble),
			DEFINE_FIELD(tracer1),
			DEFINE_FIELD(tracer2),
			DEFINE_FIELD(laser),
			DEFINE_FIELD(turretOverheatSound),
			DEFINE_FIELD(turretOverheatEffect),
			DEFINE_FIELD(turretBarrelSpinRumble),
			DEFINE_FIELD(turretBarrelSpinMaxSnd),
			DEFINE_FIELD(turretBarrelSpinUpSnd[0]),
			DEFINE_FIELD(turretBarrelSpinUpSnd[1]),
			DEFINE_FIELD(turretBarrelSpinUpSnd[2]),
			DEFINE_FIELD(turretBarrelSpinUpSnd[3]),
			DEFINE_FIELD(turretBarrelSpinDownSnd[0]),
			DEFINE_FIELD(turretBarrelSpinDownSnd[1]),
			DEFINE_FIELD(turretBarrelSpinDownSnd[2]),
			DEFINE_FIELD(turretBarrelSpinDownSnd[3]),
			DEFINE_FIELD(missileConeSoundAlias),
			DEFINE_FIELD(missileConeSoundAliasAtBase),
			DEFINE_FIELD(stowOffsetModel),
			DEFINE_FIELD(turretHydraulicSettings),
			DEFINE_FIELD(altWeapon),
			DEFINE_FIELD(numWeaponAttachments),
			DEFINE_FIELD(numAnimOverrides),
			DEFINE_FIELD(numSoundOverrides),
			DEFINE_FIELD(numFXOverrides),
			DEFINE_FIELD(numReloadStateTimerOverrides),
			DEFINE_FIELD(numNotetrackOverrides),
			DEFINE_FIELD(playerAnimType),
			DEFINE_FIELD(__pad000[0]),
			DEFINE_FIELD(weapType),
			DEFINE_FIELD(weapClass),
			DEFINE_FIELD(penetrateType),
			DEFINE_FIELD(penetrateDepth),
			DEFINE_FIELD(impactType),
			DEFINE_FIELD(inventoryType),
			DEFINE_FIELD(fireType),
			DEFINE_FIELD(fireBarrels),
			DEFINE_FIELD(adsFireMode),
			DEFINE_FIELD(burstFireCooldown),
			DEFINE_FIELD(greebleType),
			DEFINE_FIELD(autoReloadType),
			DEFINE_FIELD(slotRestriction),
			DEFINE_FIELD(offhandClass),
			DEFINE_FIELD(stance),
			DEFINE_FIELD(reticleCenterSize),
			DEFINE_FIELD(reticleSideSize),
			DEFINE_FIELD(reticleMinOfs),
			DEFINE_FIELD(activeReticleType),
			DEFINE_FIELD(standMove[0]),
			DEFINE_FIELD(standMove[1]),
			DEFINE_FIELD(standMove[2]),
			DEFINE_FIELD(standRot[0]),
			DEFINE_FIELD(standRot[1]),
			DEFINE_FIELD(standRot[2]),
			DEFINE_FIELD(strafeMove[0]),
			DEFINE_FIELD(strafeMove[1]),
			DEFINE_FIELD(strafeMove[2]),
			DEFINE_FIELD(strafeRot[0]),
			DEFINE_FIELD(strafeRot[1]),
			DEFINE_FIELD(strafeRot[2]),
			DEFINE_FIELD(duckedOfs[0]),
			DEFINE_FIELD(duckedOfs[1]),
			DEFINE_FIELD(duckedOfs[2]),
			DEFINE_FIELD(duckedMove[0]),
			DEFINE_FIELD(duckedMove[1]),
			DEFINE_FIELD(duckedMove[2]),
			DEFINE_FIELD(duckedRot[0]),
			DEFINE_FIELD(duckedRot[1]),
			DEFINE_FIELD(duckedRot[2]),
			DEFINE_FIELD(proneOfs[0]),
			DEFINE_FIELD(proneOfs[1]),
			DEFINE_FIELD(proneOfs[2]),
			DEFINE_FIELD(proneMove[0]),
			DEFINE_FIELD(proneMove[1]),
			DEFINE_FIELD(proneMove[2]),
			DEFINE_FIELD(proneRot[0]),
			DEFINE_FIELD(proneRot[1]),
			DEFINE_FIELD(proneRot[2]),
			DEFINE_FIELD(posMoveRate),
			DEFINE_FIELD(posProneMoveRate),
			DEFINE_FIELD(standMoveMinSpeed),
			DEFINE_FIELD(duckedMoveMinSpeed),
			DEFINE_FIELD(proneMoveMinSpeed),
			DEFINE_FIELD(posRotRate),
			DEFINE_FIELD(posProneRotRate),
			DEFINE_FIELD(hudIconRatio),
			DEFINE_FIELD(pickupIconRatio),
			DEFINE_FIELD(ammoCounterIconRatio),
			DEFINE_FIELD(ammoCounterClip),
			DEFINE_FIELD(startAmmo),
			DEFINE_FIELD(ammoIndex),
			DEFINE_FIELD(ammoIndexUnknown),
			DEFINE_FIELD(__pad002[0]),
			DEFINE_FIELD(__pad002[1]),
			DEFINE_FIELD(__pad002[2]),
			DEFINE_FIELD(clipIndex),
			DEFINE_FIELD(clipIndexUnknown),
			DEFINE_FIELD(__pad003[0]),
			DEFINE_FIELD(__pad003[1]),
			DEFINE_FIELD(__pad003[2]),
			DEFINE_FIELD(maxAmmo),
			DEFINE_FIELD(minAmmoReq),
			DEFINE_FIELD(clipSize),
			DEFINE_FIELD(shotCount),
			DEFINE_FIELD(sharedAmmoCapIndex),
			DEFINE_FIELD(sharedAmmoCap),
			DEFINE_FIELD(damage),
			DEFINE_FIELD(playerDamage),
			DEFINE_FIELD(meleeDamage),
			DEFINE_FIELD(damageType),
			DEFINE_FIELD(stateTimers.fireDelay),
			DEFINE_FIELD(stateTimers.meleeDelay),
			DEFINE_FIELD(stateTimers.meleeChargeDelay),
			DEFINE_FIELD(stateTimers.detonateDelay),
			DEFINE_FIELD(stateTimers.fireTime),
			DEFINE_FIELD(stateTimers.rechamberTime),
			DEFINE_FIELD(stateTimers.rechamberTimeOneHanded),
			DEFINE_FIELD(stateTimers.rechamberBoltTime),
			DEFINE_FIELD(stateTimers.holdFireTime),
			DEFINE_FIELD(stateTimers.grenadePrimeReadyToThrowTime),
			DEFINE_FIELD(stateTimers.detonateTime),
			DEFINE_FIELD(stateTimers.meleeTime),
			DEFINE_FIELD(stateTimers.meleeChargeTime),
			DEFINE_FIELD(stateTimers.reloadTime),
			DEFINE_FIELD(stateTimers.reloadShowRocketTime),
			DEFINE_FIELD(stateTimers.reloadEmptyTime),
			DEFINE_FIELD(stateTimers.reloadAddTime),
			DEFINE_FIELD(stateTimers.reloadEmptyAddTime),
			DEFINE_FIELD(stateTimers.reloadStartTime),
			DEFINE_FIELD(stateTimers.reloadStartAddTime),
			DEFINE_FIELD(stateTimers.reloadEndTime),
			DEFINE_FIELD(stateTimers.reloadTimeDualWield),
			DEFINE_FIELD(stateTimers.reloadAddTimeDualWield),
			DEFINE_FIELD(stateTimers.reloadEmptyDualMag),
			DEFINE_FIELD(stateTimers.reloadEmptyAddTimeDualMag),
			DEFINE_FIELD(stateTimers.u25),
			DEFINE_FIELD(stateTimers.u26),
			DEFINE_FIELD(stateTimers.dropTime),
			DEFINE_FIELD(stateTimers.raiseTime),
			DEFINE_FIELD(stateTimers.altDropTime),
			DEFINE_FIELD(stateTimers.altRaiseTime),
			DEFINE_FIELD(stateTimers.quickDropTime),
			DEFINE_FIELD(stateTimers.quickRaiseTime),
			DEFINE_FIELD(stateTimers.firstRaiseTime),
			DEFINE_FIELD(stateTimers.breachRaiseTime),
			DEFINE_FIELD(stateTimers.emptyRaiseTime),
			DEFINE_FIELD(stateTimers.emptyDropTime),
			DEFINE_FIELD(stateTimers.sprintInTime),
			DEFINE_FIELD(stateTimers.sprintLoopTime),
			DEFINE_FIELD(stateTimers.sprintOutTime),
			DEFINE_FIELD(stateTimers.stunnedTimeBegin),
			DEFINE_FIELD(stateTimers.stunnedTimeLoop),
			DEFINE_FIELD(stateTimers.stunnedTimeEnd),
			DEFINE_FIELD(stateTimers.nightVisionWearTime),
			DEFINE_FIELD(stateTimers.nightVisionWearTimeFadeOutEnd),
			DEFINE_FIELD(stateTimers.nightVisionWearTimePowerUp),
			DEFINE_FIELD(stateTimers.nightVisionRemoveTime),
			DEFINE_FIELD(stateTimers.nightVisionRemoveTimePowerDown),
			DEFINE_FIELD(stateTimers.nightVisionRemoveTimeFadeInStart),
			DEFINE_FIELD(stateTimers.aiFuseTime),
			DEFINE_FIELD(stateTimers.fuseTime),
			DEFINE_FIELD(stateTimers.missileTime),
			DEFINE_FIELD(stateTimers.primeTime),
			DEFINE_FIELD(stateTimers.bHoldFullPrime),
			DEFINE_FIELD(stateTimers.blastFrontTime),
			DEFINE_FIELD(stateTimers.blastRightTime),
			DEFINE_FIELD(stateTimers.blastBackTime),
			DEFINE_FIELD(stateTimers.blastLeftTime),
			DEFINE_FIELD(stateTimers.u58),
			DEFINE_FIELD(stateTimers.u59),
			DEFINE_FIELD(stateTimers.u60),
			DEFINE_FIELD(stateTimers.u61),
			DEFINE_FIELD(stateTimers.u62),
			DEFINE_FIELD(stateTimers.u63),
			DEFINE_FIELD(stateTimers.u64),
			DEFINE_FIELD(stateTimers.u65),
			DEFINE_FIELD(stateTimers.u66),
			DEFINE_FIELD(stateTimers.u67),
			DEFINE_FIELD(stateTimers.u68),
			DEFINE_FIELD(stateTimers.offhandSwitchTime),
			DEFINE_FIELD(stateTimers.u70),
			DEFINE_FIELD(stateTimers.u71),
			DEFINE_FIELD(stateTimers.u72),
			DEFINE_FIELD(stateTimers.u73),
			DEFINE_FIELD(stateTimers.u74),
			DEFINE_FIELD(stateTimers),
			DEFINE_FIELD(akimboStateTimers.fireDelay),
			DEFINE_FIELD(akimboStateTimers.meleeDelay),
			DEFINE_FIELD(akimboStateTimers.meleeChargeDelay),
			DEFINE_FIELD(akimboStateTimers.detonateDelay),
			DEFINE_FIELD(akimboStateTimers.fireTime),
			DEFINE_FIELD(akimboStateTimers.rechamberTime),
			DEFINE_FIELD(akimboStateTimers.rechamberTimeOneHanded),
			DEFINE_FIELD(akimboStateTimers.rechamberBoltTime),
			DEFINE_FIELD(akimboStateTimers.holdFireTime),
			DEFINE_FIELD(akimboStateTimers.grenadePrimeReadyToThrowTime),
			DEFINE_FIELD(akimboStateTimers.detonateTime),
			DEFINE_FIELD(akimboStateTimers.meleeTime),
			DEFINE_FIELD(akimboStateTimers.meleeChargeTime),
			DEFINE_FIELD(akimboStateTimers.reloadTime),
			DEFINE_FIELD(akimboStateTimers.reloadShowRocketTime),
			DEFINE_FIELD(akimboStateTimers.reloadEmptyTime),
			DEFINE_FIELD(akimboStateTimers.reloadAddTime),
			DEFINE_FIELD(akimboStateTimers.reloadEmptyAddTime),
			DEFINE_FIELD(akimboStateTimers.reloadStartTime),
			DEFINE_FIELD(akimboStateTimers.reloadStartAddTime),
			DEFINE_FIELD(akimboStateTimers.reloadEndTime),
			DEFINE_FIELD(akimboStateTimers.reloadTimeDualWield),
			DEFINE_FIELD(akimboStateTimers.reloadAddTimeDualWield),
			DEFINE_FIELD(akimboStateTimers.reloadEmptyDualMag),
			DEFINE_FIELD(akimboStateTimers.reloadEmptyAddTimeDualMag),
			DEFINE_FIELD(akimboStateTimers.u25),
			DEFINE_FIELD(akimboStateTimers.u26),
			DEFINE_FIELD(akimboStateTimers.dropTime),
			DEFINE_FIELD(akimboStateTimers.raiseTime),
			DEFINE_FIELD(akimboStateTimers.altDropTime),
			DEFINE_FIELD(akimboStateTimers.altRaiseTime),
			DEFINE_FIELD(akimboStateTimers.quickDropTime),
			DEFINE_FIELD(akimboStateTimers.quickRaiseTime),
			DEFINE_FIELD(akimboStateTimers.firstRaiseTime),
			DEFINE_FIELD(akimboStateTimers.breachRaiseTime),
			DEFINE_FIELD(akimboStateTimers.emptyRaiseTime),
			DEFINE_FIELD(akimboStateTimers.emptyDropTime),
			DEFINE_FIELD(akimboStateTimers.sprintInTime),
			DEFINE_FIELD(akimboStateTimers.sprintLoopTime),
			DEFINE_FIELD(akimboStateTimers.sprintOutTime),
			DEFINE_FIELD(akimboStateTimers.stunnedTimeBegin),
			DEFINE_FIELD(akimboStateTimers.stunnedTimeLoop),
			DEFINE_FIELD(akimboStateTimers.stunnedTimeEnd),
			DEFINE_FIELD(akimboStateTimers.nightVisionWearTime),
			DEFINE_FIELD(akimboStateTimers.nightVisionWearTimeFadeOutEnd),
			DEFINE_FIELD(akimboStateTimers.nightVisionWearTimePowerUp),
			DEFINE_FIELD(akimboStateTimers.nightVisionRemoveTime),
			DEFINE_FIELD(akimboStateTimers.nightVisionRemoveTimePowerDown),
			DEFINE_FIELD(akimboStateTimers.nightVisionRemoveTimeFadeInStart),
			DEFINE_FIELD(akimboStateTimers.aiFuseTime),
			DEFINE_FIELD(akimboStateTimers.fuseTime),
			DEFINE_FIELD(akimboStateTimers.missileTime),
			DEFINE_FIELD(akimboStateTimers.primeTime),
			DEFINE_FIELD(akimboStateTimers.bHoldFullPrime),
			DEFINE_FIELD(akimboStateTimers.blastFrontTime),
			DEFINE_FIELD(akimboStateTimers.blastRightTime),
			DEFINE_FIELD(akimboStateTimers.blastBackTime),
			DEFINE_FIELD(akimboStateTimers.blastLeftTime),
			DEFINE_FIELD(akimboStateTimers.u58),
			DEFINE_FIELD(akimboStateTimers.u59),
			DEFINE_FIELD(akimboStateTimers.u60),
			DEFINE_FIELD(akimboStateTimers.u61),
			DEFINE_FIELD(akimboStateTimers.u62),
			DEFINE_FIELD(akimboStateTimers.u63),
			DEFINE_FIELD(akimboStateTimers.u64),
			DEFINE_FIELD(akimboStateTimers.u65),
			DEFINE_FIELD(akimboStateTimers.u66),
			DEFINE_FIELD(akimboStateTimers.u67),
			DEFINE_FIELD(akimboStateTimers.u68),
			DEFINE_FIELD(akimboStateTimers.offhandSwitchTime),
			DEFINE_FIELD(akimboStateTimers.u70),
			DEFINE_FIELD(akimboStateTimers.u71),
			DEFINE_FIELD(akimboStateTimers.u72),
			DEFINE_FIELD(akimboStateTimers.u73),
			DEFINE_FIELD(akimboStateTimers.u74),
			DEFINE_FIELD(akimboStateTimers),
			DEFINE_FIELD(autoAimRange),
			DEFINE_FIELD(aimAssistRange),
			DEFINE_FIELD(aimAssistRangeAds),
			DEFINE_FIELD(aimPadding),
			DEFINE_FIELD(enemyCrosshairRange),
			DEFINE_FIELD(moveSpeedScale),
			DEFINE_FIELD(adsMoveSpeedScale),
			DEFINE_FIELD(sprintDurationScale),
			DEFINE_FIELD(adsZoomFov),
			DEFINE_FIELD(adsZoomInFrac),
			DEFINE_FIELD(adsZoomOutFrac),
			DEFINE_FIELD(adsSceneBlur),
			DEFINE_FIELD(fU_007),
			DEFINE_FIELD(xU_008),
			DEFINE_FIELD(overlay.shader),
			DEFINE_FIELD(overlay.shaderLowRes),
			DEFINE_FIELD(overlay.shaderEMP),
			DEFINE_FIELD(overlay.shaderEMPLowRes),
			DEFINE_FIELD(overlay.reticle),
			DEFINE_FIELD(overlay.width),
			DEFINE_FIELD(overlay.height),
			DEFINE_FIELD(overlay.widthSplitscreen),
			DEFINE_FIELD(overlay.heightSplitscreen),
			DEFINE_FIELD(overlay),
			DEFINE_FIELD(overlayInterface),
			DEFINE_FIELD(adsBobFactor),
			DEFINE_FIELD(adsViewBobMult),
			DEFINE_FIELD(hipSpreadStandMin),
			DEFINE_FIELD(hipSpreadDuckedMin),
			DEFINE_FIELD(hipSpreadProneMin),
			DEFINE_FIELD(hipSpreadStandMax),
			DEFINE_FIELD(xU_009),
			DEFINE_FIELD(xU_010),
			DEFINE_FIELD(hipSpreadDuckedMax),
			DEFINE_FIELD(hipSpreadProneMax),
			DEFINE_FIELD(hipSpreadDecayRate),
			DEFINE_FIELD(hipSpreadFireAdd),
			DEFINE_FIELD(hipSpreadTurnAdd),
			DEFINE_FIELD(hipSpreadMoveAdd),
			DEFINE_FIELD(hipSpreadDuckedDecay),
			DEFINE_FIELD(hipSpreadProneDecay),
			DEFINE_FIELD(hipReticleSidePos),
			DEFINE_FIELD(adsIdleAmount),
			DEFINE_FIELD(hipIdleAmount),
			DEFINE_FIELD(adsIdleSpeed),
			DEFINE_FIELD(hipIdleSpeed),
			DEFINE_FIELD(idleCrouchFactor),
			DEFINE_FIELD(idleProneFactor),
			DEFINE_FIELD(gunMaxPitch),
			DEFINE_FIELD(gunMaxYaw),
			DEFINE_FIELD(adsIdleLerpStartTime),
			DEFINE_FIELD(adsIdleLerpTime),
			DEFINE_FIELD(adsTransInTime),
			DEFINE_FIELD(xU_011),
			DEFINE_FIELD(adsTransOutTime),
			DEFINE_FIELD(xU_012),
			DEFINE_FIELD(swayMaxAngle),
			DEFINE_FIELD(swayLerpSpeed),
			DEFINE_FIELD(swayPitchScale),
			DEFINE_FIELD(swayYawScale),
			DEFINE_FIELD(swayVertScale),
			DEFINE_FIELD(swayHorizScale),
			DEFINE_FIELD(swayShellShockScale),
			DEFINE_FIELD(adsSwayMaxAngle),
			DEFINE_FIELD(adsSwayLerpSpeed),
			DEFINE_FIELD(adsSwayPitchScale),
			DEFINE_FIELD(adsSwayYawScale),
			DEFINE_FIELD(adsSwayHorizScale),
			DEFINE_FIELD(adsSwayVertScale),
			DEFINE_FIELD(adsViewErrorMin),
			DEFINE_FIELD(adsViewErrorMax),
			DEFINE_FIELD(adsFireAnimFrac),
			DEFINE_FIELD(dualWieldViewModelOffset),
			DEFINE_FIELD(scopeDriftDelay),
			DEFINE_FIELD(scopeDriftLerpInTime),
			DEFINE_FIELD(scopeDriftSteadyTime),
			DEFINE_FIELD(scopeDriftLerpOutTime),
			DEFINE_FIELD(scopeDriftSteadyFactor),
			DEFINE_FIELD(scopeDriftUnsteadyFactor),
			DEFINE_FIELD(bobVerticalFactor),
			DEFINE_FIELD(bobHorizontalFactor),
			DEFINE_FIELD(bobViewVerticalFactor),
			DEFINE_FIELD(bobViewHorizontalFactor),
			DEFINE_FIELD(stationaryZoomFov),
			DEFINE_FIELD(stationaryZoomDelay),
			DEFINE_FIELD(stationaryZoomLerpInTime),
			DEFINE_FIELD(stationaryZoomLerpOutTime),
			DEFINE_FIELD(adsDofStart),
			DEFINE_FIELD(adsDofEnd),
			DEFINE_FIELD(xU_020),
			DEFINE_FIELD(killIcon),
			DEFINE_FIELD(dpadIcon),
			DEFINE_FIELD(hudProximityWarningIcon),
			DEFINE_FIELD(killIconRatio),
			DEFINE_FIELD(dpadIconRatio),
			DEFINE_FIELD(fireAnimLength),
			DEFINE_FIELD(fireAnimLengthAkimbo),
			DEFINE_FIELD(inspectAnimTime),
			DEFINE_FIELD(reloadAmmoAdd),
			DEFINE_FIELD(reloadStartAdd),
			DEFINE_FIELD(ammoDropStockMin),
			DEFINE_FIELD(ammoDropStockMax),
			DEFINE_FIELD(ammoDropClipPercentMin),
			DEFINE_FIELD(ammoDropClipPercentMax),
			DEFINE_FIELD(explosionRadius),
			DEFINE_FIELD(explosionRadiusMin),
			DEFINE_FIELD(explosionInnerDamage),
			DEFINE_FIELD(explosionOuterDamage),
			DEFINE_FIELD(damageConeAngle),
			DEFINE_FIELD(bulletExplDmgMult),
			DEFINE_FIELD(bulletExplRadiusMult),
			DEFINE_FIELD(projectileSpeed),
			DEFINE_FIELD(projectileSpeedUp),
			DEFINE_FIELD(projectileSpeedForward),
			DEFINE_FIELD(projectileActivateDist),
			DEFINE_FIELD(projLifetime),
			DEFINE_FIELD(timeToAccelerate),
			DEFINE_FIELD(projectileCurvature),
			DEFINE_FIELD(xU_021),
			DEFINE_FIELD(projectileName),
			DEFINE_FIELD(projectileModel),
			DEFINE_FIELD(projExplosionEffect),
			DEFINE_FIELD(projDudEffect),
			DEFINE_FIELD(projExplosionSound),
			DEFINE_FIELD(projDudSound),
			DEFINE_FIELD(projExplosion),
			DEFINE_FIELD(stickiness),
			DEFINE_FIELD(lowAmmoWarningThreshold),
			DEFINE_FIELD(ricochetChance),
			DEFINE_FIELD(riotShieldHealth),
			DEFINE_FIELD(riotShieldDamageMult),
			DEFINE_FIELD(parallelBounce),
			DEFINE_FIELD(perpendicularBounce),
			DEFINE_FIELD(projTrailEffect),
			DEFINE_FIELD(projBeaconEffect),
			DEFINE_FIELD(projectileColor[0]),
			DEFINE_FIELD(projectileColor[1]),
			DEFINE_FIELD(projectileColor[2]),
			DEFINE_FIELD(guidedMissileType),
			DEFINE_FIELD(maxSteeringAccel),
			DEFINE_FIELD(projIgnitionDelay),
			DEFINE_FIELD(projIgnitionEffect),
			DEFINE_FIELD(projIgnitionSound),
			DEFINE_FIELD(adsAimPitch),
			DEFINE_FIELD(adsCrosshairInFrac),
			DEFINE_FIELD(adsCrosshairOutFrac),
			DEFINE_FIELD(adsGunKickReducedKickBullets),
			DEFINE_FIELD(adsGunKickReducedKickPercent),
			DEFINE_FIELD(adsGunKickPitchMin),
			DEFINE_FIELD(adsGunKickPitchMax),
			DEFINE_FIELD(adsGunKickYawMin),
			DEFINE_FIELD(adsGunKickYawMax),
			DEFINE_FIELD(adsGunKickMagMin),
			DEFINE_FIELD(adsGunKickAccel),
			DEFINE_FIELD(adsGunKickSpeedMax),
			DEFINE_FIELD(adsGunKickSpeedDecay),
			DEFINE_FIELD(adsGunKickStaticDecay),
			DEFINE_FIELD(adsViewKickPitchMin),
			DEFINE_FIELD(adsViewKickPitchMax),
			DEFINE_FIELD(adsViewKickYawMin),
			DEFINE_FIELD(adsViewKickYawMax),
			DEFINE_FIELD(adsViewKickMagMin),
			DEFINE_FIELD(adsViewKickCenterSpeed),
			DEFINE_FIELD(adsViewScatterMin),
			DEFINE_FIELD(adsViewScatterMax),
			DEFINE_FIELD(adsSpread),
			DEFINE_FIELD(hipGunKickReducedKickBullets),
			DEFINE_FIELD(hipGunKickReducedKickPercent),
			DEFINE_FIELD(hipGunKickPitchMin),
			DEFINE_FIELD(hipGunKickPitchMax),
			DEFINE_FIELD(hipGunKickYawMin),
			DEFINE_FIELD(hipGunKickYawMax),
			DEFINE_FIELD(hipGunKickMagMin),
			DEFINE_FIELD(hipGunKickAccel),
			DEFINE_FIELD(hipGunKickSpeedMax),
			DEFINE_FIELD(hipGunKickSpeedDecay),
			DEFINE_FIELD(hipGunKickStaticDecay),
			DEFINE_FIELD(hipViewKickPitchMin),
			DEFINE_FIELD(hipViewKickPitchMax),
			DEFINE_FIELD(hipViewKickYawMin),
			DEFINE_FIELD(hipViewKickYawMax),
			DEFINE_FIELD(hipViewKickMagMin),
			DEFINE_FIELD(hipViewKickCenterSpeed),
			DEFINE_FIELD(hipViewScatterMin),
			DEFINE_FIELD(hipViewScatterMax),
			DEFINE_FIELD(xU_043),
			DEFINE_FIELD(adsReloadTransTime),
			DEFINE_FIELD(fightDist),
			DEFINE_FIELD(maxDist),
			DEFINE_FIELD(accuracyGraphName[0]),
			DEFINE_FIELD(accuracyGraphName[1]),
			DEFINE_FIELD(accuracyGraphKnots[0]),
			DEFINE_FIELD(accuracyGraphKnots[1]),
			DEFINE_FIELD(originalAccuracyGraphKnots[0]),
			DEFINE_FIELD(originalAccuracyGraphKnots[1]),
			DEFINE_FIELD(accuracyGraphKnotCount[0]),
			DEFINE_FIELD(accuracyGraphKnotCount[1]),
			DEFINE_FIELD(positionReloadTransTime),
			DEFINE_FIELD(leftArc),
			DEFINE_FIELD(rightArc),
			DEFINE_FIELD(topArc),
			DEFINE_FIELD(bottomArc),
			DEFINE_FIELD(accuracy),
			DEFINE_FIELD(aiSpread),
			DEFINE_FIELD(playerSpread),
			DEFINE_FIELD(minTurnSpeed[0]),
			DEFINE_FIELD(minTurnSpeed[1]),
			DEFINE_FIELD(maxTurnSpeed[0]),
			DEFINE_FIELD(maxTurnSpeed[1]),
			DEFINE_FIELD(pitchConvergenceTime),
			DEFINE_FIELD(yawConvergenceTime),
			DEFINE_FIELD(suppressTime),
			DEFINE_FIELD(maxRange),
			DEFINE_FIELD(animHorRotateInc),
			DEFINE_FIELD(playerPositionDist),
			DEFINE_FIELD(useHintStringIndex),
			DEFINE_FIELD(dropHintStringIndex),
			DEFINE_FIELD(horizViewJitter),
			DEFINE_FIELD(vertViewJitter),
			DEFINE_FIELD(scanSpeed),
			DEFINE_FIELD(scanAccel),
			DEFINE_FIELD(scanPauseTime),
			DEFINE_FIELD(szScript),
			DEFINE_FIELD(minDamage),
			DEFINE_FIELD(midDamage),
			DEFINE_FIELD(minPlayerDamage),
			DEFINE_FIELD(midPlayerDamage),
			DEFINE_FIELD(maxDamageRange),
			DEFINE_FIELD(minDamageRange),
			DEFINE_FIELD(iU_045),
			DEFINE_FIELD(iU_046),
			DEFINE_FIELD(iU_047),
			DEFINE_FIELD(iU_048),
			DEFINE_FIELD(fU_049),
			DEFINE_FIELD(fU_050),
			DEFINE_FIELD(destabilizationRateTime),
			DEFINE_FIELD(destabilizationCurvatureMax),
			DEFINE_FIELD(destabilizeDistance),
			DEFINE_FIELD(turretADSTime),
			DEFINE_FIELD(turretFov),
			DEFINE_FIELD(turretFovADS),
			DEFINE_FIELD(turretScopeZoomRate),
			DEFINE_FIELD(turretScopeZoomMin),
			DEFINE_FIELD(turretScopeZoomMax),
			DEFINE_FIELD(xU_056),
			DEFINE_FIELD(xU_057),
			DEFINE_FIELD(xU_058),
			DEFINE_FIELD(xU_059),
			DEFINE_FIELD(turretBarrelSpinSpeed),
			DEFINE_FIELD(turretBarrelSpinUpTime),
			DEFINE_FIELD(turretBarrelSpinDownTime),
			DEFINE_FIELD(missileConeSoundRadiusAtTop),
			DEFINE_FIELD(missileConeSoundRadiusAtBase),
			DEFINE_FIELD(missileConeSoundHeight),
			DEFINE_FIELD(missileConeSoundOriginOffset),
			DEFINE_FIELD(missileConeSoundVolumescaleAtCore),
			DEFINE_FIELD(missileConeSoundVolumescaleAtEdge),
			DEFINE_FIELD(missileConeSoundVolumescaleCoreSize),
			DEFINE_FIELD(missileConeSoundPitchAtTop),
			DEFINE_FIELD(missileConeSoundPitchAtBottom),
			DEFINE_FIELD(missileConeSoundPitchTopSize),
			DEFINE_FIELD(missileConeSoundPitchBottomSize),
			DEFINE_FIELD(missileConeSoundCrossfadeTopSize),
			DEFINE_FIELD(missileConeSoundCrossfadeBottomSize),
			DEFINE_FIELD(aim_automelee_lerp),
			DEFINE_FIELD(aim_automelee_range),
			DEFINE_FIELD(aim_automelee_region_height),
			DEFINE_FIELD(aim_automelee_region_width),
			DEFINE_FIELD(player_meleeHeight),
			DEFINE_FIELD(player_meleeRange),
			DEFINE_FIELD(player_meleeWidth),
			DEFINE_FIELD(signatureFireTime),
			DEFINE_FIELD(signatureNumBullets),
			DEFINE_FIELD(fireTimeInterpolationType),
			DEFINE_FIELD(xU_075),
			DEFINE_FIELD(ammoUsedPerShot),
			DEFINE_FIELD(xU_076),
			DEFINE_FIELD(xU_077),
			DEFINE_FIELD(xU_078),
			DEFINE_FIELD(iU_079),
			DEFINE_FIELD(iU_080),
			DEFINE_FIELD(stowTag),
			DEFINE_FIELD(bU_081),
			DEFINE_FIELD(unknownReticleBooleanValue1),
			DEFINE_FIELD(unknownReticleBooleanValue2),
			DEFINE_FIELD(turretADSEnabled),
			DEFINE_FIELD(knifeAttachTagLeft),
			DEFINE_FIELD(knifeAlwaysAttached),
			DEFINE_FIELD(meleeOverrideValues),
			DEFINE_FIELD(bU_083),
			DEFINE_FIELD(bU_084),
			DEFINE_FIELD(sharedAmmo),
			DEFINE_FIELD(lockonSupported),
			DEFINE_FIELD(requireLockonToFire),
			DEFINE_FIELD(isAirburstWeapon),
			DEFINE_FIELD(bigExplosion),
			DEFINE_FIELD(noAdsWhenMagEmpty),
			DEFINE_FIELD(avoidDropCleanup),
			DEFINE_FIELD(inheritsPerks),
			DEFINE_FIELD(crosshairColorChange),
			DEFINE_FIELD(rifleBullet),
			DEFINE_FIELD(armorPiercing),
			DEFINE_FIELD(boltAction),
			DEFINE_FIELD(aimDownSight),
			DEFINE_FIELD(canHoldBreath),
			DEFINE_FIELD(meleeOnly),
			DEFINE_FIELD(bU_085),
			DEFINE_FIELD(bU_086),
			DEFINE_FIELD(canVariableZoom),
			DEFINE_FIELD(rechamberWhileAds),
			DEFINE_FIELD(bulletExplosiveDamage),
			DEFINE_FIELD(cookOffHold),
			DEFINE_FIELD(reticleSpin45),
			DEFINE_FIELD(reticleSideEnabled),
			DEFINE_FIELD(clipOnly),
			DEFINE_FIELD(noAmmoPickup),
			DEFINE_FIELD(disableSwitchToWhenEmpty),
			DEFINE_FIELD(bU_088),
			DEFINE_FIELD(hasMotionTracker),
			DEFINE_FIELD(bU_089),
			DEFINE_FIELD(noDualWield),
			DEFINE_FIELD(flipKillIcon),
			DEFINE_FIELD(actionSlotShowAmmo),
			DEFINE_FIELD(noPartialReload),
			DEFINE_FIELD(segmentedReload),
			DEFINE_FIELD(multipleReload),
			DEFINE_FIELD(blocksProne),
			DEFINE_FIELD(silenced),
			DEFINE_FIELD(isRollingGrenade),
			DEFINE_FIELD(projExplosionEffectForceNormalUp),
			DEFINE_FIELD(projExplosionEffectInheritParentDirection),
			DEFINE_FIELD(projImpactExplode),
			DEFINE_FIELD(projTrajectoryEvents),
			DEFINE_FIELD(projWhizByEnabled),
			DEFINE_FIELD(stickToPlayers),
			DEFINE_FIELD(stickToVehicles),
			DEFINE_FIELD(stickToTurrets),
			DEFINE_FIELD(thrownSideways),
			DEFINE_FIELD(hasDetonatorEmptyThrow),
			DEFINE_FIELD(hasDetonatorDoubleTap),
			DEFINE_FIELD(disableFiring),
			DEFINE_FIELD(timedDetonation),
			DEFINE_FIELD(bU_090),
			DEFINE_FIELD(bU_091),
			DEFINE_FIELD(rotate),
			DEFINE_FIELD(holdButtonToThrow),
			DEFINE_FIELD(freezeMovementWhenFiring),
			DEFINE_FIELD(thermalScope),
			DEFINE_FIELD(thermalToggle),
			DEFINE_FIELD(outlineEnemies),
			DEFINE_FIELD(altModeSameWeapon),
			DEFINE_FIELD(turretBarrelSpinEnabled),
			DEFINE_FIELD(missileConeSoundEnabled),
			DEFINE_FIELD(missileConeSoundPitchshiftEnabled),
			DEFINE_FIELD(missileConeSoundCrossfadeEnabled),
			DEFINE_FIELD(offhandHoldIsCancelable),
			DEFINE_FIELD(doNotAllowAttachmentsToOverrideSpread),
			DEFINE_FIELD(useFastReloadAnims),
			DEFINE_FIELD(dualMagReloadSupported),
			DEFINE_FIELD(reloadStopsAlt),
			DEFINE_FIELD(bU_092),
			DEFINE_FIELD(alwaysShatterGlassOnImpact),
			DEFINE_FIELD(oldWeapon),
			DEFINE_FIELD(bU_093),
			DEFINE_FIELD(bU_094),
			DEFINE_FIELD(xU_095),
			DEFINE_FIELD(hasCounterSilencer),
			DEFINE_FIELD(xU_097),
			DEFINE_FIELD(xU_098),
			DEFINE_FIELD(disableVariableAutosimRate),
			DEFINE_FIELD(bU_100),
			DEFINE_FIELD(bU_101),
			DEFINE_FIELD(bU_102),
			DEFINE_FIELD(bU_103),
			DEFINE_FIELD(bU_104),
			DEFINE_FIELD(cloakedWeapon),
			DEFINE_FIELD(adsHideWeapon),
			DEFINE_FIELD(adsHideHands),
			DEFINE_FIELD(bU_108),
			DEFINE_FIELD(adsBlurSceneEnabled),
			DEFINE_FIELD(usesSniperScope),
			DEFINE_FIELD(bU_111),
			DEFINE_FIELD(bU_112),
			DEFINE_FIELD(bU_113),
			DEFINE_FIELD(bU_114),
			DEFINE_FIELD(bU_115),
			DEFINE_FIELD(adsDofPhysicalFStop),
			DEFINE_FIELD(adsDofPhysicalFocusDistance),
			DEFINE_FIELD(autosimSpeedScalar),
			DEFINE_FIELD(explosionReactiveMotionParts[0]),
			DEFINE_FIELD(explosionReactiveMotionParts[1]),
			DEFINE_FIELD(explosionReactiveMotionParts[2]),
			DEFINE_FIELD(explosionReactiveMotionParts[3]),
			DEFINE_FIELD(explosionReactiveMotionParts[4]),
			DEFINE_FIELD(__pad_unknown[0]),
			DEFINE_FIELD(__pad_unknown[1]),
			DEFINE_FIELD(__pad_unknown[2]),
			DEFINE_FIELD(__pad_unknown[3]),
			DEFINE_FIELD(__pad_unknown[4]),
			DEFINE_FIELD(__pad_unknown[5]),
			DEFINE_FIELD(__pad_unknown[6]),
			DEFINE_FIELD(__pad_unknown[7]),
			DEFINE_FIELD(__pad_unknown[8]),
			DEFINE_FIELD(__pad_unknown[9]),
			DEFINE_FIELD(__pad_unknown[10]),
			DEFINE_FIELD(__pad_unknown[11]),
		};

		unsigned char get_anim_index(const std::string& anim)
		{
			for (auto i = 0; i < NUM_WEAP_ANIMS; i++)
			{
				if (anim_names[i] == anim)
				{
					return static_cast<unsigned char>(i);
				}
			}

			ZONETOOL_FATAL("Invalid waField anim %s", anim.data());
		}

		std::optional<std::string> weapon_offset_to_name(const unsigned short offset)
		{
			const auto name = weapon_def_fields_rev.find(offset);
			if (name == weapon_def_fields_rev.end())
			{
				return {};
			}

			return {name->second};
		}

		unsigned short weapon_field_to_offset(const std::string& name)
		{
			const auto offset = weapon_def_fields.find(name);
			if (offset == weapon_def_fields.end())
			{
				ZONETOOL_FATAL("Invalid weapon field name %s\n", name.data());
			}

			return offset->second;
		}

		struct _
		{
			_()
			{
				for (const auto& [field, offset] : weapon_def_fields)
				{
					weapon_def_fields_rev.insert(std::make_pair(offset, field));
				}
			}
		} _;
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

#define ATTACHMENT_READ_FIELD(__type__, __field__) \
	if(!data[#__field__].is_null()) \
	{ \
		attachment->__field__ = data[#__field__].get<__type__>(); \
	}

#define ATTACHMENT_READ_FIELD_RENAME(__type__, __field__, __name__) \
	if(!data[#__field__].is_null()) \
	{ \
		attachment->__field__ = data[__name__].get<__type__>(); \
	}

#define ATTACHMENT_READ_ASSET_ARR(__type__, __datafield__, __field__, __struct__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		attachment->__field__ = mem->allocate<__struct__*>(__size__); \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			auto asset##__field__ = data[#__field__][idx##__field__].get<std::string>(); \
			if (asset##__field__.empty()) \
			{ \
				attachment->__field__[idx##__field__] = nullptr; \
			} \
			else \
			{ \
				attachment->__field__[idx##__field__] = db_find_x_asset_header(XAssetType::__type__, asset##__field__.data(), 1).__datafield__; \
			} \
		} \
	} \
	else \
	{ \
		attachment->__field__ = nullptr; \
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
		json data = json::parse(bytes);

		auto* attachment = mem->allocate<WeaponAttachment>();

		// base asset
		auto base = data["baseAsset"].get<std::string>();
		if (!base.empty())
		{
			const auto base_asset = db_find_x_asset_header(ASSET_TYPE_ATTACHMENT, base.data(), 0).attachment;
			if (base_asset == nullptr)
			{
				ZONETOOL_WARNING("Could not load base asset \"%s\" into memory...", base.data());
			}
			else
			{
				std::memcpy(attachment, base_asset, sizeof(WeaponAttachment));
			}
		}
		else
		{
			ZONETOOL_WARNING("No base asset is defined for attachment \"%s\", stuff might go wrong!", name.data());
		}

		if (!data["internalName"].is_null())
		{
			attachment->szInternalName = mem->duplicate_string(data["internalName"].get<std::string>());
		}

		if (!data["displayName"].is_null())
		{
			attachment->szDisplayName = mem->duplicate_string(data["displayName"].get<std::string>());
		}

		ATTACHMENT_READ_FIELD(AttachmentType, type);
		ATTACHMENT_READ_FIELD_RENAME(weapType_t, weaponType, "weaponType");
		ATTACHMENT_READ_FIELD_RENAME(weapClass_t, weapClass, "weaponClass");
		ATTACHMENT_READ_FIELD_RENAME(weapGreebleType_t, greebleType, "greebleType");

		ATTACHMENT_READ_ASSET_ARR(ASSET_TYPE_XMODEL, model, worldModels, XModel, 2);
		ATTACHMENT_READ_ASSET_ARR(ASSET_TYPE_XMODEL, model, viewModels, XModel, 2);
		ATTACHMENT_READ_ASSET_ARR(ASSET_TYPE_XMODEL, model, reticleViewModels, XModel, 64);

		ATTACHMENT_READ_ASSET_ARR(ASSET_TYPE_SOUND, sound, bounceSounds, snd_alias_list_t, 53);
		ATTACHMENT_READ_ASSET_ARR(ASSET_TYPE_SOUND, sound, rollingSounds, snd_alias_list_t, 53);

		attachment->stringArray1 = mem->allocate<scr_string_t>(4);
		for (auto i = 0; i < 4; i++)
		{
			this->add_script_string(&attachment->stringArray1[i], mem->duplicate_string(data["stringArray1"][i].get<std::string>()));
		}

		attachment->stringArray2 = mem->allocate<scr_string_t>(4);
		for (auto i = 0; i < 4; i++)
		{
			this->add_script_string(&attachment->stringArray2[i], mem->duplicate_string(data["stringArray2"][i].get<std::string>()));
		}

		if (!data["waFields"].is_null())
		{
			attachment->waFieldsCount = static_cast<unsigned int>(data["waFields"].size());
			attachment->waFieldOffsets = mem->allocate<unsigned short>(attachment->waFieldsCount);
			attachment->waFields = mem->allocate<WAField>(attachment->waFieldsCount);

			std::vector<field_info> sorted_fields;
			for (auto i = 0u; i < attachment->waFieldsCount; i++)
			{
				field_info info{};

				info.json_index = i;
				info.type = data["waFields"][i]["type"].get<unsigned char>();
				info.code = data["waFields"][i]["code"].get<unsigned char>();

				if (data["waFields"][i]["offset"].is_number())
				{
					info.offset = data["waFields"][i]["offset"].get<unsigned short>();

				}
				else if (data["waFields"][i]["name"].is_string())
				{
					info.offset = weapon_field_to_offset(data["waFields"][i]["name"].get<std::string>());
				}
				else
				{
					ZONETOOL_FATAL("Invalid WAField offset/name value");
				}

				if (data["waFields"][i]["index"].is_number())
				{
					info.index = data["waFields"][i]["index"].get<unsigned char>();
				}
				else if (info.type == WAFIELD_TYPE_ANIM && data["waFields"][i]["anim"].is_string())
				{
					info.index = get_anim_index(data["waFields"][i]["anim"].get<std::string>());
				}
				else
				{
					ZONETOOL_FATAL("Invalid WAField index");
				}

				sorted_fields.emplace_back(info);
			}

			std::sort(sorted_fields.begin(), sorted_fields.end(), [](const auto a, const auto b)
			{
				if (a.offset == b.offset)
				{
					return a.index < b.index;
				}

			return a.offset < b.offset;
			});

			for (auto f = 0; f < sorted_fields.size(); f++)
			{
				const auto& field = sorted_fields[f];
				const auto i = field.json_index;

				attachment->waFieldOffsets[f] = field.offset;
				attachment->waFields[f].code = field.code;
				attachment->waFields[f].index = field.index;
				attachment->waFields[f].fieldType = field.type;

				auto type = attachment->waFields[f].fieldType;
				if (type == WAFIELD_TYPE_STRING ||
					type == WAFIELD_TYPE_FX ||
					type == WAFIELD_TYPE_MODEL ||
					type == WAFIELD_TYPE_ANIM ||
					type == WAFIELD_TYPE_MATERIAL ||
					//type == 14 ||
					type == WAFIELD_TYPE_SOUND ||
					type == WAFIELD_TYPE_TRACER)
				{
					attachment->waFields[f].parm.string = mem->duplicate_string(data["waFields"][i]["value"].get<std::string>());
				}
				else if (type == WAFIELD_TYPE_INT)
				{
					attachment->waFields[f].parm.p_float = static_cast<float>(data["waFields"][i]["value"].get<int>());
				}
				else if (type == WAFIELD_TYPE_BOOL)
				{
					attachment->waFields[f].parm.p_bool = data["waFields"][i]["value"].get<bool>();
				}
				else if (type == WAFIELD_TYPE_FLOAT)
				{
					attachment->waFields[f].parm.p_float = data["waFields"][i]["value"].get<float>();
				}
				else if (type == WAFIELD_TYPE_FLOAT32)
				{
					attachment->waFields[f].parm.p_float = static_cast<float>(data["waFields"][i]["value"].get<int>() / 1000.0f);
				}
				else
				{
					ZONETOOL_FATAL("Unknown WAField type: %d, for attachment \"%s\"", type, name.data());
				}
			}
		}
		else
		{
			attachment->waFieldOffsets = nullptr;
			attachment->waFields = nullptr;
			attachment->waFieldsCount = 0;
		}

		return attachment;
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

			auto* attachment = this->asset_;
			if (attachment->stringArray1)
			{
				auto* original_strings1 = attachment->stringArray1;
				attachment->stringArray1 = mem->allocate<scr_string_t>(4);
				for (auto i = 0; i < 4; i++)
				{
					this->add_script_string(&attachment->stringArray1[i], SL_ConvertToString(original_strings1[i]));
				}
			}
			if (attachment->stringArray2)
			{
				auto* original_strings1 = attachment->stringArray2;
				attachment->stringArray2 = mem->allocate<scr_string_t>(4);
				for (auto i = 0; i < 4; i++)
				{
					this->add_script_string(&attachment->stringArray2[i], SL_ConvertToString(original_strings1[i]));
				}
			}
		}
	}

#define ATTACHMENT_SCRIPTSTRING_ARRAY_PREPARE(__field__,__count__) \
		if (data->__field__) \
		{ \
			for (int i = 0; i < __count__; i++) \
			{ \
				data->__field__[i] = static_cast<scr_string_t>(buf->write_scriptstring(this->get_script_string(&data->__field__[i]))); \
			} \
		}

	void weapon_attachment::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;
		ATTACHMENT_SCRIPTSTRING_ARRAY_PREPARE(stringArray1, 4);
		ATTACHMENT_SCRIPTSTRING_ARRAY_PREPARE(stringArray2, 4);
	}

#define ATTACHMENT_SUBASSET_DEPENDING(__field__,__type__/*,__struct__*/) \
		if (data->__field__) \
		{ \
			zone->add_asset_of_type(__type__, data->__field__->name); \
		}

	void weapon_attachment::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;

		if (data->worldModels)
		{
			for (auto i = 0; i < 2; i++)
			{
				ATTACHMENT_SUBASSET_DEPENDING(worldModels[i], ASSET_TYPE_XMODEL);
			}
		}

		if (data->viewModels)
		{
			for (auto i = 0; i < 2; i++)
			{
				ATTACHMENT_SUBASSET_DEPENDING(viewModels[i], ASSET_TYPE_XMODEL);
			}
		}

		if (data->reticleViewModels)
		{
			for (auto i = 0; i < 64; i++)
			{
				ATTACHMENT_SUBASSET_DEPENDING(reticleViewModels[i], ASSET_TYPE_XMODEL);
			}
		}

		if (data->bounceSounds)
		{
			for (auto i = 0; i < 53; i++)
			{
				ATTACHMENT_SUBASSET_DEPENDING(bounceSounds[i], ASSET_TYPE_SOUND);
			}
		}

		if (data->rollingSounds)
		{
			for (auto i = 0; i < 53; i++)
			{
				ATTACHMENT_SUBASSET_DEPENDING(rollingSounds[i], ASSET_TYPE_SOUND);
			}
		}

		if (data->waFields)
		{
			for (auto i = 0u; i < data->waFieldsCount; i++)
			{
#define SUBASSET_CASE(__type__, __asset_type__) \
				case __type__: \
					if (field->parm.string) \
					{ \
						zone->add_asset_of_type(__asset_type__, field->parm.string); \
					} \
					break; \

				const auto field = &data->waFields[i];
				switch (field->fieldType)
				{
					SUBASSET_CASE(WAFIELD_TYPE_FX, ASSET_TYPE_FX);
					SUBASSET_CASE(WAFIELD_TYPE_MATERIAL, ASSET_TYPE_MATERIAL);
					SUBASSET_CASE(WAFIELD_TYPE_MODEL, ASSET_TYPE_XMODEL);
					SUBASSET_CASE(WAFIELD_TYPE_SOUND, ASSET_TYPE_SOUND);
					SUBASSET_CASE(WAFIELD_TYPE_TRACER, ASSET_TYPE_TRACER);
				}
#undef SUBASSET_CASE
			}
		}
	}

	std::string weapon_attachment::name()
	{
		return this->name_;
	}

	std::int32_t weapon_attachment::type()
	{
		return ASSET_TYPE_ATTACHMENT;
	}

#define ATTACHMENT_SOUND_CUSTOM_ARRAY(__field__, __count__) \
		if (data->__field__) \
		{ \
			buf->align(7); \
			auto* dest_sounds = buf->write(data->__field__, __count__); \
			auto ptr = 0xFDFDFDFFFFFFFFFF; \
			for (auto i = 0; i < __count__; i++) \
			{ \
				buf->align(7); \
				buf->write(&ptr); \
				buf->write_str(data->__field__[i]->name); \
				buf->clear_pointer(&dest_sounds[i]); \
			} \
			buf->clear_pointer(&dest->__field__); \
		}

#define ATTACHMENT_SCRIPTSTRING_ARRAY(__field__,__count__) \
		if (data->__field__) \
		{ \
			buf->align(3); \
			buf->write(data->__field__,__count__); \
			buf->clear_pointer(&dest->__field__); \
		}

	void weapon_attachment::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->szDisplayName)
		{
			dest->szDisplayName = buf->write_str(data->szDisplayName);
		}

		if (data->worldModels)
		{
			buf->align(7);
			auto destModels = buf->write(data->worldModels, 2);

			for (auto i = 0; i < 2; i++)
			{
				if (destModels[i])
				{
					destModels[i] = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->worldModels[i]->name)
						);
				}
			}

			buf->clear_pointer(&dest->worldModels);
		}

		if (data->viewModels)
		{
			buf->align(7);
			auto destModels = buf->write(data->viewModels, 2);

			for (auto i = 0; i < 2; i++)
			{
				if (destModels[i])
				{
					destModels[i] = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->viewModels[i]->name)
						);
				}
			}

			buf->clear_pointer(&dest->viewModels);
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

		ATTACHMENT_SOUND_CUSTOM_ARRAY(bounceSounds, 53);
		ATTACHMENT_SOUND_CUSTOM_ARRAY(rollingSounds, 53);

		if (data->chargeInfo)
		{
			buf->align(3);
			buf->write(data->chargeInfo);
			buf->clear_pointer(&dest->chargeInfo);
		}

		if (data->hybridSettings)
		{
			buf->align(3);
			buf->write(data->hybridSettings);
			buf->clear_pointer(&dest->hybridSettings);
		}

		ATTACHMENT_SCRIPTSTRING_ARRAY(stringArray1, 4);
		ATTACHMENT_SCRIPTSTRING_ARRAY(stringArray2, 4);

		if (data->waFieldOffsets)
		{
			buf->align(1);
			buf->write(data->waFieldOffsets, data->waFieldsCount);
			buf->clear_pointer(&dest->waFieldOffsets);
		}

		if (data->waFields)
		{
			buf->align(3);
			auto* dest_waFields = buf->write(data->waFields, data->waFieldsCount);
			for (unsigned int i = 0; i < data->waFieldsCount; i++)
			{
				auto type = data->waFields[i].fieldType;
				if (type == WAFIELD_TYPE_STRING ||
					type == WAFIELD_TYPE_FX ||
					type == WAFIELD_TYPE_MODEL ||
					type == WAFIELD_TYPE_ANIM ||
					type == WAFIELD_TYPE_MATERIAL ||
					//type == 14 ||
					type == WAFIELD_TYPE_SOUND ||
					type == WAFIELD_TYPE_TRACER)
				{
					if (data->waFields[i].parm.string)
					{
						dest_waFields[i].parm.string = buf->write_str(data->waFields[i].parm.string);
					}
				}
			}
			buf->clear_pointer(&dest->waFields);
		}

		buf->pop_stream();
	}

#define ATTACHMENT_DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__;

#define ATTACHMENT_DUMP_FIELD_RENAME(__field__, __name__) \
	data[__name__] = asset->__field__;

#define ATTACHMENT_DUMP_ASSET_ARR(__field__, __size__) \
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

	void weapon_attachment::dump(WeaponAttachment* asset)
	{
		const auto path = "attachments\\"s + asset->name + ".json"s;

		ordered_json data;

		data["baseAsset"] = asset->szInternalName;

		data["internalName"] = asset->szInternalName;
		data["displayName"] = asset->szDisplayName ? asset->szDisplayName : "";

		ATTACHMENT_DUMP_FIELD(type);
		ATTACHMENT_DUMP_FIELD_RENAME(weaponType, "weaponType");
		ATTACHMENT_DUMP_FIELD_RENAME(weapClass, "weaponClass");
		ATTACHMENT_DUMP_FIELD_RENAME(greebleType, "greebleType");

		ATTACHMENT_DUMP_FIELD(shareAmmoWithAlt);
		ATTACHMENT_DUMP_FIELD(riotShield);

		ATTACHMENT_DUMP_ASSET_ARR(worldModels, 2);
		ATTACHMENT_DUMP_ASSET_ARR(viewModels, 2);
		ATTACHMENT_DUMP_ASSET_ARR(reticleViewModels, 64);

		ATTACHMENT_DUMP_ASSET_ARR(bounceSounds, 53);
		ATTACHMENT_DUMP_ASSET_ARR(rollingSounds, 53);

		for (auto i = 0; i < 4; i++)
		{
			if (asset->stringArray1 && asset->stringArray1[i])
			{
				data["stringArray1"][i] = SL_ConvertToString(asset->stringArray1[i]);
			}
			else
			{
				data["stringArray1"][i] = "";
			}
		}

		for (auto i = 0; i < 4; i++)
		{
			if (asset->stringArray2 && asset->stringArray2[i])
			{
				data["stringArray2"][i] = SL_ConvertToString(asset->stringArray2[i]);
			}
			else
			{
				data["stringArray2"][i] = "";
			}
		}

		if (asset->waFieldOffsets && asset->waFields)
		{
			data["waFields"] = {};
			for (unsigned int i = 0; i < asset->waFieldsCount; i++)
			{
				const auto field_name = weapon_offset_to_name(asset->waFieldOffsets[i]);
				if (field_name.has_value())
				{
					data["waFields"][i]["name"] = field_name.value();
				}
				else
				{
					data["waFields"][i]["offset"] = asset->waFieldOffsets[i];
				}

				data["waFields"][i]["type"] = asset->waFields[i].fieldType;

				if (asset->waFields[i].fieldType == WAFIELD_TYPE_ANIM)
				{
					data["waFields"][i]["anim"] = anim_names[asset->waFields[i].index];
				}
				else
				{
					data["waFields"][i]["index"] = asset->waFields[i].index;
				}

				data["waFields"][i]["code"] = asset->waFields[i].code;

				auto type = asset->waFields[i].fieldType;
				if (type == WAFIELD_TYPE_STRING ||
					type == WAFIELD_TYPE_FX ||
					type == WAFIELD_TYPE_MODEL ||
					type == WAFIELD_TYPE_ANIM ||
					type == WAFIELD_TYPE_MATERIAL ||
					//type == 14 ||
					type == WAFIELD_TYPE_SOUND ||
					type == WAFIELD_TYPE_TRACER)
				{
					data["waFields"][i]["value"] = asset->waFields[i].parm.string;
				}
				else if (type == WAFIELD_TYPE_INT)
				{
					data["waFields"][i]["value"] = static_cast<int>(asset->waFields[i].parm.p_float);
				}
				else if (type == WAFIELD_TYPE_BOOL)
				{
					data["waFields"][i]["value"] = asset->waFields[i].parm.p_bool;
				}
				else if (type == WAFIELD_TYPE_FLOAT)
				{
					data["waFields"][i]["value"] = asset->waFields[i].parm.p_float;
				}
				else if (type == WAFIELD_TYPE_FLOAT32)
				{
					data["waFields"][i]["value"] = static_cast<int>(asset->waFields[i].parm.p_float * 1000.0f);
				}
				else
				{
					ZONETOOL_FATAL("Unknown WAField type: %d, for attachment \"%s\"", type, asset->name);
				}
			}
		}
		else
		{
			data["waFields"] = nullptr;
		}

		std::string json = data.dump(4);

		auto file = filesystem::file(path);
		file.open("wb");
		file.write(json.data(), json.size(), 1);
		file.close();
	}
}