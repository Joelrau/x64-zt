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
			DEFINE_FIELD(szAltWeaponName),
			DEFINE_FIELD(gunModel),
			DEFINE_FIELD(handModel),
			DEFINE_FIELD(persistentArmXModel),
			DEFINE_FIELD(reticleViewModels),
			DEFINE_FIELD(lobWorldModelName),
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
			DEFINE_FIELD(notetrackHideTagKeys),
			DEFINE_FIELD(notetrackHideTagValues),
			DEFINE_FIELD(notetrackHideTagTagValues),
			DEFINE_FIELD(szAdsrBaseSetting),
			DEFINE_FIELD(viewFlashEffect),
			DEFINE_FIELD(viewBodyFlashEffect),
			DEFINE_FIELD(worldFlashEffect),
			DEFINE_FIELD(viewFlashADSEffect),
			DEFINE_FIELD(viewBodyFlashADSEffect),
			DEFINE_FIELD(signatureViewFlashEffect),
			DEFINE_FIELD(signatureViewBodyFlashEffect),
			DEFINE_FIELD(signatureWorldFlashEffect),
			DEFINE_FIELD(signatureViewFlashADSEffect),
			DEFINE_FIELD(signatureViewBodyFlashADSEffect),
			DEFINE_FIELD(meleeHitEffect),
			DEFINE_FIELD(meleeMissEffect),
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
			DEFINE_FIELD(fireMedSound),
			DEFINE_FIELD(fireMedSoundPlayer),
			DEFINE_FIELD(fireHighSound),
			DEFINE_FIELD(fireHighSoundPlayer),
			DEFINE_FIELD(fireLoopSound),
			DEFINE_FIELD(fireLoopSoundPlayer),
			DEFINE_FIELD(fireMedLoopSound),
			DEFINE_FIELD(fireMedLoopSoundPlayer),
			DEFINE_FIELD(fireHighLoopSound),
			DEFINE_FIELD(fireHighLoopSoundPlayer),
			DEFINE_FIELD(fireLoopEndPointSound),
			DEFINE_FIELD(fireLoopEndPointSoundPlayer),
			DEFINE_FIELD(fireStopSound),
			DEFINE_FIELD(fireStopSoundPlayer),
			DEFINE_FIELD(fireMedStopSound),
			DEFINE_FIELD(fireMedStopSoundPlayer),
			DEFINE_FIELD(fireHighStopSound),
			DEFINE_FIELD(fireHighStopSoundPlayer),
			DEFINE_FIELD(fireLastSound),
			DEFINE_FIELD(fireLastSoundPlayer),
			DEFINE_FIELD(fireFirstSound),
			DEFINE_FIELD(fireFirstSoundPlayer),
			DEFINE_FIELD(fireCustomSound),
			DEFINE_FIELD(fireCustomSoundPlayer),
			DEFINE_FIELD(emptyFireSound),
			DEFINE_FIELD(emptyFireSoundPlayer),
			DEFINE_FIELD(adsRequiredFireSoundPlayer),
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
			DEFINE_FIELD(altSwitchSound),
			DEFINE_FIELD(altSwitchSoundPlayer),
			DEFINE_FIELD(raiseSound),
			DEFINE_FIELD(raiseSoundPlayer),
			DEFINE_FIELD(firstRaiseSound),
			DEFINE_FIELD(firstRaiseSoundPlayer),
			DEFINE_FIELD(putawaySound),
			DEFINE_FIELD(putawaySoundPlayer),
			DEFINE_FIELD(scanSound),
			DEFINE_FIELD(changeVariableZoomSound),
			DEFINE_FIELD(adsUpSound),
			DEFINE_FIELD(adsDownSound),
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
			DEFINE_FIELD(minimapIconFriendly),
			DEFINE_FIELD(minimapIconEnemy),
			DEFINE_FIELD(minimapIconNeutral),
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
			DEFINE_FIELD(tracerType),
			DEFINE_FIELD(signatureTracerType),
			DEFINE_FIELD(laserType),
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
			DEFINE_FIELD(numAttachments),
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
			DEFINE_FIELD(autoHolsterType),
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
			//DEFINE_FIELD(iAmmoIndex),
			//DEFINE_FIELD(iClipIndex),
			DEFINE_FIELD(maxAmmo),
			DEFINE_FIELD(minAmmoReq),
			DEFINE_FIELD(clipSize),
			DEFINE_FIELD(shotCount),
			//DEFINE_FIELD(sharedAmmoCapIndex),
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
			DEFINE_FIELD(stateTimers.speedReloadTime),
			DEFINE_FIELD(stateTimers.speedReloadAddTime),
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
			DEFINE_FIELD(stateTimers.slideInTime),
			DEFINE_FIELD(stateTimers.slideLoopTime),
			DEFINE_FIELD(stateTimers.slideOutTime),
			DEFINE_FIELD(stateTimers.highJumpInTime),
			DEFINE_FIELD(stateTimers.highJumpDropInTime),
			DEFINE_FIELD(stateTimers.highJumpDropLoopTime),
			DEFINE_FIELD(stateTimers.highJumpDropLandTime),
			DEFINE_FIELD(stateTimers.dodgeTime),
			DEFINE_FIELD(stateTimers.landDipTime),
			DEFINE_FIELD(stateTimers.hybridSightInTime),
			DEFINE_FIELD(stateTimers.hybridSightOutTime),
			DEFINE_FIELD(stateTimers.offhandSwitchTime),
			DEFINE_FIELD(stateTimers.heatCooldownInTime),
			DEFINE_FIELD(stateTimers.heatCooldownOutTime),
			DEFINE_FIELD(stateTimers.heatCooldownOutReadyTime),
			DEFINE_FIELD(stateTimers.overheatOutTime),
			DEFINE_FIELD(stateTimers.overheatOutReadyTime),
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
			DEFINE_FIELD(akimboStateTimers.speedReloadTime),
			DEFINE_FIELD(akimboStateTimers.speedReloadAddTime),
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
			DEFINE_FIELD(akimboStateTimers.slideInTime),
			DEFINE_FIELD(akimboStateTimers.slideLoopTime),
			DEFINE_FIELD(akimboStateTimers.slideOutTime),
			DEFINE_FIELD(akimboStateTimers.highJumpInTime),
			DEFINE_FIELD(akimboStateTimers.highJumpDropInTime),
			DEFINE_FIELD(akimboStateTimers.highJumpDropLoopTime),
			DEFINE_FIELD(akimboStateTimers.highJumpDropLandTime),
			DEFINE_FIELD(akimboStateTimers.dodgeTime),
			DEFINE_FIELD(akimboStateTimers.landDipTime),
			DEFINE_FIELD(akimboStateTimers.hybridSightInTime),
			DEFINE_FIELD(akimboStateTimers.hybridSightOutTime),
			DEFINE_FIELD(akimboStateTimers.offhandSwitchTime),
			DEFINE_FIELD(akimboStateTimers.heatCooldownInTime),
			DEFINE_FIELD(akimboStateTimers.heatCooldownOutTime),
			DEFINE_FIELD(akimboStateTimers.heatCooldownOutReadyTime),
			DEFINE_FIELD(akimboStateTimers.overheatOutTime),
			DEFINE_FIELD(akimboStateTimers.overheatOutReadyTime),
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
			DEFINE_FIELD(adsSceneBlurStrength),
			DEFINE_FIELD(adsSceneBlurPhysicalScale),
			//DEFINE_FIELD(pad3),
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
			DEFINE_FIELD(hipSpreadSprintMax),
			DEFINE_FIELD(hipSpreadSlideMax),
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
			DEFINE_FIELD(adsTransInFromSprintTime),
			DEFINE_FIELD(adsTransOutTime),
			DEFINE_FIELD(swayMaxAngleSteadyAim),
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
			//DEFINE_FIELD(pad1),
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
			//DEFINE_FIELD(pad2),
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
			DEFINE_FIELD(viewKickScale),
			DEFINE_FIELD(positionReloadTransTime),
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
			DEFINE_FIELD(signatureAmmoInClip),
			DEFINE_FIELD(signatureDamage),
			DEFINE_FIELD(signatureMidDamage),
			DEFINE_FIELD(signatureMinDamage),
			DEFINE_FIELD(signatureMaxDamageRange),
			DEFINE_FIELD(signatureMinDamageRange),
			DEFINE_FIELD(destabilizationRateTime),
			DEFINE_FIELD(destabilizationCurvatureMax),
			DEFINE_FIELD(destabilizeDistance),
			DEFINE_FIELD(turretADSTime),
			DEFINE_FIELD(turretFov),
			DEFINE_FIELD(turretFovADS),
			DEFINE_FIELD(turretScopeZoomRate),
			DEFINE_FIELD(turretScopeZoomMin),
			DEFINE_FIELD(turretScopeZoomMax),
			DEFINE_FIELD(overheatUpRate),
			DEFINE_FIELD(overheatDownRate),
			DEFINE_FIELD(overheatCooldownRate),
			DEFINE_FIELD(overheatPenalty),
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
			DEFINE_FIELD(changedFireTime),
			DEFINE_FIELD(changedFireTimeNumBullets),
			DEFINE_FIELD(fireTimeInterpolationType),
			DEFINE_FIELD(generateAmmo),
			DEFINE_FIELD(ammoPerShot),
			DEFINE_FIELD(explodeCount),
			DEFINE_FIELD(batteryDischargeRate),
			DEFINE_FIELD(extendedBattery),
			DEFINE_FIELD(bulletsPerTag),
			DEFINE_FIELD(maxTags),
			DEFINE_FIELD(stowTag),
			DEFINE_FIELD(rattleSoundType),
			DEFINE_FIELD(adsShouldShowCrosshair),
			DEFINE_FIELD(adsCrosshairShouldScale),
			DEFINE_FIELD(turretADSEnabled),
			DEFINE_FIELD(knifeAttachTagLeft),
			DEFINE_FIELD(knifeAlwaysAttached),
			DEFINE_FIELD(meleeOverrideValues),
			DEFINE_FIELD(riotShieldEnableDamage),
			DEFINE_FIELD(allowPrimaryWeaponPickup),
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
			DEFINE_FIELD(quickMelee),
			DEFINE_FIELD(bU_086),
			DEFINE_FIELD(canVariableZoom),
			DEFINE_FIELD(rechamberWhileAds),
			DEFINE_FIELD(bulletExplosiveDamage),
			DEFINE_FIELD(cookOffHold),
			DEFINE_FIELD(useBattery),
			DEFINE_FIELD(reticleSpin45),
			DEFINE_FIELD(clipOnly),
			DEFINE_FIELD(noAmmoPickup),
			DEFINE_FIELD(disableSwitchToWhenEmpty),
			DEFINE_FIELD(suppressAmmoReserveDisplay),
			DEFINE_FIELD(motionTracker),
			DEFINE_FIELD(markableViewmodel),
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
			DEFINE_FIELD(noCrumpleMissile),
			DEFINE_FIELD(fuseLitAfterImpact),
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
			DEFINE_FIELD(useScopeDrift),
			DEFINE_FIELD(alwaysShatterGlassOnImpact),
			DEFINE_FIELD(oldWeapon),
			DEFINE_FIELD(raiseToHold),
			DEFINE_FIELD(notifyOnPlayerImpact),
			DEFINE_FIELD(decreasingKick),
			DEFINE_FIELD(counterSilencer),
			DEFINE_FIELD(projSuppressedByEMP),
			DEFINE_FIELD(projDisabledByEMP),
			DEFINE_FIELD(autosimDisableVariableRate),
			DEFINE_FIELD(projPlayTrailEffectForOwnerOnly),
			DEFINE_FIELD(projPlayBeaconEffectForOwnerOnly),
			DEFINE_FIELD(projKillTrailEffectOnDeath),
			DEFINE_FIELD(projKillBeaconEffectOnDeath),
			DEFINE_FIELD(reticleDetonateHide),
			DEFINE_FIELD(cloaked),
			DEFINE_FIELD(adsHideWeapon),
			DEFINE_FIELD(adsHideHands),
			DEFINE_FIELD(bU_108),
			DEFINE_FIELD(adsSceneBlur),
			DEFINE_FIELD(usesSniperScope),
			DEFINE_FIELD(hasTransientModels),
			DEFINE_FIELD(signatureAmmoAlternate),
			DEFINE_FIELD(useScriptCallbackForHit),
			DEFINE_FIELD(useBulletTagSystem),
			DEFINE_FIELD(hideBulletTags),
			DEFINE_FIELD(adsDofPhysicalFstop),
			DEFINE_FIELD(adsDofPhysicalFocusDistance),
			DEFINE_FIELD(autosimSpeedScale),
			DEFINE_FIELD(reactiveMotionRadiusScale),
			DEFINE_FIELD(reactiveMotionFrequencyScale),
			DEFINE_FIELD(reactiveMotionAmplitudeScale),
			DEFINE_FIELD(reactiveMotionFalloff),
			DEFINE_FIELD(reactiveMotionLifetime),
			DEFINE_FIELD(fU_3604[0]),
			DEFINE_FIELD(fU_3604[1]),
			DEFINE_FIELD(fU_3604[2]),
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

#define ATTACHMENT_READ_ASSET_ARR(__type__, __field__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		attachment->__field__ = mem->allocate<typename std::remove_reference<decltype(*attachment->__field__)>::type>(__size__); \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			auto asset##__field__ = data[#__field__][idx##__field__].get<std::string>(); \
			if (asset##__field__.empty()) \
			{ \
				attachment->__field__[idx##__field__] = nullptr; \
			} \
			else \
			{ \
				attachment->__field__[idx##__field__] = mem->manual_allocate<typename std::remove_reference<decltype(*attachment->__field__[idx##__field__])>::type>(sizeof(const char*)); \
				attachment->__field__[idx##__field__]->name = mem->duplicate_string(asset##__field__); \
			} \
		} \
	} \
	else \
	{ \
		attachment->__field__ = nullptr; \
	}

	void parse_charge_info(AttChargeInfo* attachment, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_READ_FIELD(float, minChargeTime);
		ATTACHMENT_READ_FIELD(float, overChargeTime);
		ATTACHMENT_READ_FIELD(float, timePerChargeShot);
		ATTACHMENT_READ_FIELD(int, maxChargeShots);
		ATTACHMENT_READ_FIELD(float, minChargeAngle);
		ATTACHMENT_READ_FIELD(float, maxChargeAngle);
		ATTACHMENT_READ_FIELD(bool, autoFireOnMaxCharge);
	}

	void parse_hybrid_settings(AttHybridSettings* attachment, ordered_json& data, zone_memory* mem)
	{
		ATTACHMENT_READ_FIELD(float, adsSpread);
		ATTACHMENT_READ_FIELD(float, adsAimPitch);
		ATTACHMENT_READ_FIELD(float, adsTransInTime);
		ATTACHMENT_READ_FIELD(float, adsTransInFromSprintTime);
		ATTACHMENT_READ_FIELD(float, adsTransOutTime);
		ATTACHMENT_READ_FIELD(int, adsReloadTransTime);
		ATTACHMENT_READ_FIELD(float, adsCrosshairInFrac);
		ATTACHMENT_READ_FIELD(float, adsCrosshairOutFrac);
		ATTACHMENT_READ_FIELD(float, adsZoomFov);
		ATTACHMENT_READ_FIELD(float, adsZoomInFrac);
		ATTACHMENT_READ_FIELD(float, adsZoomOutFrac);
		ATTACHMENT_READ_FIELD(float, adsFovLerpInTime);
		ATTACHMENT_READ_FIELD(float, adsFovLerpOutTime);
		ATTACHMENT_READ_FIELD(float, adsBobFactor);
		ATTACHMENT_READ_FIELD(float, adsViewBobMult);
		ATTACHMENT_READ_FIELD(float, adsViewErrorMin);
		ATTACHMENT_READ_FIELD(float, adsViewErrorMax);
		ATTACHMENT_READ_FIELD(float, adsFireAnimFrac);
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

		auto* attachment = mem->allocate<WeaponAttachment>();

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

		ATTACHMENT_READ_ASSET_ARR(ASSET_TYPE_XMODEL, worldModels, 2);
		ATTACHMENT_READ_ASSET_ARR(ASSET_TYPE_XMODEL, viewModels, 2);
		ATTACHMENT_READ_ASSET_ARR(ASSET_TYPE_XMODEL, reticleViewModels, 64);

		ATTACHMENT_READ_ASSET_ARR(ASSET_TYPE_SOUND, bounceSounds, 53);
		ATTACHMENT_READ_ASSET_ARR(ASSET_TYPE_SOUND, rollingSounds, 53);

		if (!data["chargeInfo"].is_null())
		{
			attachment->chargeInfo = mem->allocate<AttChargeInfo>();
			parse_charge_info(attachment->chargeInfo, data["chargeInfo"], mem);
		}
		else
		{
			attachment->chargeInfo = nullptr;
		}

		if (!data["hybridSettings"].is_null())
		{
			attachment->hybridSettings = mem->allocate<AttHybridSettings>();
			parse_hybrid_settings(attachment->hybridSettings, data["hybridSettings"], mem);
		}
		else
		{
			attachment->hybridSettings = nullptr;
		}

		attachment->hideTags = mem->allocate<scr_string_t>(4);
		for (auto i = 0; i < 4; i++)
		{
			this->add_script_string(&attachment->hideTags[i], mem->duplicate_string(data["hideTags"][i].get<std::string>()));
		}

		attachment->showTags = mem->allocate<scr_string_t>(4);
		for (auto i = 0; i < 4; i++)
		{
			this->add_script_string(&attachment->showTags[i], mem->duplicate_string(data["showTags"][i].get<std::string>()));
		}

		if (!data["fields"].is_null())
		{
			attachment->numFields = static_cast<unsigned int>(data["fields"].size());
			attachment->fieldOffsets = mem->allocate<unsigned short>(attachment->numFields);
			attachment->fields = mem->allocate<WAField>(attachment->numFields);

			std::vector<field_info> sorted_fields;
			for (auto i = 0u; i < attachment->numFields; i++)
			{
				field_info info{};

				info.json_index = i;
				info.type = data["fields"][i]["type"].get<unsigned char>();
				info.code = data["fields"][i]["code"].get<unsigned char>();

				if (data["fields"][i]["offset"].is_number())
				{
					info.offset = data["fields"][i]["offset"].get<unsigned short>();

				}
				else if (data["fields"][i]["name"].is_string())
				{
					info.offset = weapon_field_to_offset(data["fields"][i]["name"].get<std::string>());
				}
				else
				{
					ZONETOOL_FATAL("Invalid WAField offset/name value");
				}

				if (data["fields"][i]["index"].is_number())
				{
					info.index = data["fields"][i]["index"].get<unsigned char>();
				}
				else if (info.type == WAFIELD_TYPE_ANIM && data["fields"][i]["anim"].is_string())
				{
					info.index = get_anim_index(data["fields"][i]["anim"].get<std::string>());
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

				attachment->fieldOffsets[f] = field.offset;
				attachment->fields[f].code = field.code;
				attachment->fields[f].index = field.index;
				attachment->fields[f].type = field.type;

				auto type = attachment->fields[f].type;
				if (type == WAFIELD_TYPE_STRING ||
					type == WAFIELD_TYPE_FX ||
					type == WAFIELD_TYPE_MODEL ||
					type == WAFIELD_TYPE_ANIM ||
					type == WAFIELD_TYPE_MATERIAL ||
					//type == 14 ||
					type == WAFIELD_TYPE_SOUND ||
					type == WAFIELD_TYPE_TRACER)
				{
					attachment->fields[f].parm.string = mem->duplicate_string(data["fields"][i]["value"].get<std::string>());
				}
				else if (type == WAFIELD_TYPE_INT)
				{
					attachment->fields[f].parm.p_float = static_cast<float>(data["fields"][i]["value"].get<int>());
				}
				else if (type == WAFIELD_TYPE_BOOL)
				{
					attachment->fields[f].parm.p_bool = data["fields"][i]["value"].get<bool>();
				}
				else if (type == WAFIELD_TYPE_FLOAT)
				{
					attachment->fields[f].parm.p_float = data["fields"][i]["value"].get<float>();
				}
				else if (type == WAFIELD_TYPE_FLOAT32)
				{
					attachment->fields[f].parm.p_float = static_cast<float>(data["fields"][i]["value"].get<int>() / 1000.0f);
				}
				else
				{
					ZONETOOL_FATAL("Unknown WAField type: %d, for attachment \"%s\"", type, name.data());
				}
			}
		}
		else
		{
			attachment->fieldOffsets = nullptr;
			attachment->fields = nullptr;
			attachment->numFields = 0;
		}

		//ATTACHMENT_READ_FIELD(int, loadIndex); // runtime data, most likely

		//ATTACHMENT_READ_FIELD(int, unused1);
		ATTACHMENT_READ_FIELD(bool, isAlternateAmmo);
		ATTACHMENT_READ_FIELD(bool, hideIronSightsWithThisAttachment);
		ATTACHMENT_READ_FIELD(bool, showMasterRail);
		ATTACHMENT_READ_FIELD(bool, showSideRail);
		ATTACHMENT_READ_FIELD(bool, shareAmmoWithAlt);
		ATTACHMENT_READ_FIELD(bool, knifeAlwaysAttached);
		ATTACHMENT_READ_FIELD(bool, riotShield);
		ATTACHMENT_READ_FIELD(bool, automaticAttachment);
		//ATTACHMENT_READ_FIELD(int, unused3);

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
			if (attachment->hideTags)
			{
				auto* original_strings1 = attachment->hideTags;
				attachment->hideTags = mem->allocate<scr_string_t>(4);
				for (auto i = 0; i < 4; i++)
				{
					this->add_script_string(&attachment->hideTags[i], SL_ConvertToString(original_strings1[i]));
				}
			}
			if (attachment->showTags)
			{
				auto* original_strings1 = attachment->showTags;
				attachment->showTags = mem->allocate<scr_string_t>(4);
				for (auto i = 0; i < 4; i++)
				{
					this->add_script_string(&attachment->showTags[i], SL_ConvertToString(original_strings1[i]));
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
		ATTACHMENT_SCRIPTSTRING_ARRAY_PREPARE(hideTags, 4);
		ATTACHMENT_SCRIPTSTRING_ARRAY_PREPARE(showTags, 4);
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

		if (data->fields)
		{
			for (auto i = 0u; i < data->numFields; i++)
			{
#define SUBASSET_CASE(__type__, __asset_type__) \
				case __type__: \
					if (field->parm.string) \
					{ \
						zone->add_asset_of_type(__asset_type__, field->parm.string); \
					} \
					break; \

				const auto field = &data->fields[i];
				switch (field->type)
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
			for (auto i = 0; i < __count__; i++) \
			{ \
				buf->align(7); \
				buf->write(&buf->data_following); \
				buf->write_str_raw(data->__field__[i]->name); \
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

		ATTACHMENT_SCRIPTSTRING_ARRAY(hideTags, 4);
		ATTACHMENT_SCRIPTSTRING_ARRAY(showTags, 4);

		if (data->fieldOffsets)
		{
			buf->align(1);
			buf->write(data->fieldOffsets, data->numFields);
			buf->clear_pointer(&dest->fieldOffsets);
		}

		if (data->fields)
		{
			buf->align(3);
			auto* dest_fields = buf->write(data->fields, data->numFields);
			for (unsigned int i = 0; i < data->numFields; i++)
			{
				auto type = data->fields[i].type;
				if (type == WAFIELD_TYPE_STRING ||
					type == WAFIELD_TYPE_FX ||
					type == WAFIELD_TYPE_MODEL ||
					type == WAFIELD_TYPE_ANIM ||
					type == WAFIELD_TYPE_MATERIAL ||
					//type == 14 ||
					type == WAFIELD_TYPE_SOUND ||
					type == WAFIELD_TYPE_TRACER)
				{
					if (data->fields[i].parm.string)
					{
						dest_fields[i].parm.string = buf->write_str(data->fields[i].parm.string);
					}
				}
			}
			buf->clear_pointer(&dest->fields);
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

	void dump_charge_info(AttChargeInfo* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(minChargeTime);
		ATTACHMENT_DUMP_FIELD(overChargeTime);
		ATTACHMENT_DUMP_FIELD(timePerChargeShot);
		ATTACHMENT_DUMP_FIELD(maxChargeShots);
		ATTACHMENT_DUMP_FIELD(minChargeAngle);
		ATTACHMENT_DUMP_FIELD(maxChargeAngle);
		ATTACHMENT_DUMP_FIELD(autoFireOnMaxCharge);
	}

	void dump_hybrid_settings(AttHybridSettings* asset, ordered_json& data)
	{
		ATTACHMENT_DUMP_FIELD(adsSpread);
		ATTACHMENT_DUMP_FIELD(adsAimPitch);
		ATTACHMENT_DUMP_FIELD(adsTransInTime);
		ATTACHMENT_DUMP_FIELD(adsTransInFromSprintTime);
		ATTACHMENT_DUMP_FIELD(adsTransOutTime);
		ATTACHMENT_DUMP_FIELD(adsReloadTransTime);
		ATTACHMENT_DUMP_FIELD(adsCrosshairInFrac);
		ATTACHMENT_DUMP_FIELD(adsCrosshairOutFrac);
		ATTACHMENT_DUMP_FIELD(adsZoomFov);
		ATTACHMENT_DUMP_FIELD(adsZoomInFrac);
		ATTACHMENT_DUMP_FIELD(adsZoomOutFrac);
		ATTACHMENT_DUMP_FIELD(adsFovLerpInTime);
		ATTACHMENT_DUMP_FIELD(adsFovLerpOutTime);
		ATTACHMENT_DUMP_FIELD(adsBobFactor);
		ATTACHMENT_DUMP_FIELD(adsViewBobMult);
		ATTACHMENT_DUMP_FIELD(adsViewErrorMin);
		ATTACHMENT_DUMP_FIELD(adsViewErrorMax);
		ATTACHMENT_DUMP_FIELD(adsFireAnimFrac);
	}

	void weapon_attachment::dump(WeaponAttachment* asset)
	{
		const auto path = "attachments\\"s + asset->name + ".json"s;

		ordered_json data;

		data["internalName"] = asset->szInternalName;
		data["displayName"] = asset->szDisplayName ? asset->szDisplayName : "";

		ATTACHMENT_DUMP_FIELD(type);
		ATTACHMENT_DUMP_FIELD_RENAME(weaponType, "weaponType");
		ATTACHMENT_DUMP_FIELD_RENAME(weapClass, "weaponClass");
		ATTACHMENT_DUMP_FIELD_RENAME(greebleType, "greebleType");

		ATTACHMENT_DUMP_ASSET_ARR(worldModels, 2);
		ATTACHMENT_DUMP_ASSET_ARR(viewModels, 2);
		ATTACHMENT_DUMP_ASSET_ARR(reticleViewModels, 64);

		ATTACHMENT_DUMP_ASSET_ARR(bounceSounds, 53);
		ATTACHMENT_DUMP_ASSET_ARR(rollingSounds, 53);

		if (asset->chargeInfo)
		{
			dump_charge_info(asset->chargeInfo, data["chargeInfo"]);
		}
		else
		{
			data["chargeInfo"] = nullptr;
		}

		if (asset->hybridSettings)
		{
			dump_hybrid_settings(asset->hybridSettings, data["hybridSettings"]);
		}
		else
		{
			data["hybridSettings"] = nullptr;
		}

		for (auto i = 0; i < 4; i++)
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

		for (auto i = 0; i < 4; i++)
		{
			if (asset->showTags && asset->showTags[i])
			{
				data["showTags"][i] = SL_ConvertToString(asset->showTags[i]);
			}
			else
			{
				data["showTags"][i] = "";
			}
		}

		if (asset->fieldOffsets && asset->fields)
		{
			data["fields"] = {};
			for (unsigned int i = 0; i < asset->numFields; i++)
			{
				const auto field_name = weapon_offset_to_name(asset->fieldOffsets[i]);
				if (field_name.has_value())
				{
					data["fields"][i]["name"] = field_name.value();
				}
				else
				{
					data["fields"][i]["offset"] = asset->fieldOffsets[i];
				}

				data["fields"][i]["type"] = asset->fields[i].type;

				if (asset->fields[i].type == WAFIELD_TYPE_ANIM)
				{
					data["fields"][i]["anim"] = anim_names[asset->fields[i].index];
				}
				else
				{
					data["fields"][i]["index"] = asset->fields[i].index;
				}

				data["fields"][i]["code"] = asset->fields[i].code;

				auto type = asset->fields[i].type;
				if (type == WAFIELD_TYPE_STRING ||
					type == WAFIELD_TYPE_FX ||
					type == WAFIELD_TYPE_MODEL ||
					type == WAFIELD_TYPE_ANIM ||
					type == WAFIELD_TYPE_MATERIAL ||
					//type == 14 ||
					type == WAFIELD_TYPE_SOUND ||
					type == WAFIELD_TYPE_TRACER)
				{
					data["fields"][i]["value"] = asset->fields[i].parm.string;
				}
				else if (type == WAFIELD_TYPE_INT)
				{
					data["fields"][i]["value"] = static_cast<int>(asset->fields[i].parm.p_float);
				}
				else if (type == WAFIELD_TYPE_BOOL)
				{
					data["fields"][i]["value"] = asset->fields[i].parm.p_bool;
				}
				else if (type == WAFIELD_TYPE_FLOAT)
				{
					data["fields"][i]["value"] = asset->fields[i].parm.p_float;
				}
				else if (type == WAFIELD_TYPE_FLOAT32)
				{
					data["fields"][i]["value"] = static_cast<int>(asset->fields[i].parm.p_float * 1000.0f);
				}
				else
				{
					ZONETOOL_FATAL("Unknown WAField type: %d, for attachment \"%s\"", type, asset->name);
				}
			}
		}
		else
		{
			data["fields"] = nullptr;
		}

		//ATTACHMENT_DUMP_FIELD(loadIndex); // runtime data, most likely

		//ATTACHMENT_DUMP_FIELD(unused1);
		ATTACHMENT_DUMP_FIELD(isAlternateAmmo);
		ATTACHMENT_DUMP_FIELD(hideIronSightsWithThisAttachment);
		ATTACHMENT_DUMP_FIELD(showMasterRail);
		ATTACHMENT_DUMP_FIELD(showSideRail);
		ATTACHMENT_DUMP_FIELD(shareAmmoWithAlt);
		ATTACHMENT_DUMP_FIELD(knifeAlwaysAttached);
		ATTACHMENT_DUMP_FIELD(riotShield);
		ATTACHMENT_DUMP_FIELD(automaticAttachment);
		//ATTACHMENT_DUMP_FIELD(unused3);

		std::string json = data.dump(4);

		auto file = filesystem::file(path);
		file.open("wb");
		file.write(json.data(), json.size(), 1);
		file.close();
	}
}
