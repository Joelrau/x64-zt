#include <std_include.hpp>
#include "s2.hpp"
#include "json_fields.hpp"

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
			"UNKNOWN_7",
			"UNKNOWN_8",
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
			"UNKNOWN_26",
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
			"UNKNOWN_51",
			"SPRINT_LOOP",
			"SPRINT_OUT",
			"UNKNOWN_54",
			"UNKNOWN_55",
			"UNKNOWN_56",
			"UNKNOWN_57",
			"DETONATE",
			"UNKNOWN_59",
			"UNKNOWN_60",
			"ADS_FIRE",
			"ADS_LASTSHOT",
			"ADS_RECHAMBER",
			"DIVE",
			"DIVE_END",
			"UNKNOWN_66",
			"UNKNOWN_67",
			"RECOIL_SETTLE",
			"UNKNOWN_69",
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
			"UNKNOWN_123",
			"UNKNOWN_124",
			"UNKNOWN_125",
			"UNKNOWN_126",
			"UNKNOWN_127",
			"UNKNOWN_128",
			"UNKNOWN_129",
			"UNKNOWN_130",
			"UNKNOWN_131",
			"UNKNOWN_132",
			"UNKNOWN_133",
			"UNKNOWN_134",
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
			"UNKNOWN_149",
			"UNKNOWN_150",
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

#define ENTRY_FIELD(field) #field, entry.field
		template <typename J>
		void visit_state_timers(const J& j, StateTimers& entry)
		{
			j.value(ENTRY_FIELD(fireDelay));
			j.value(ENTRY_FIELD(meleeDelay));
			j.value(ENTRY_FIELD(meleeChargeDelay));
			j.value(ENTRY_FIELD(unknown12));
			j.value(ENTRY_FIELD(detonateDelay));
			j.value(ENTRY_FIELD(fireTime));
			j.value(ENTRY_FIELD(unknown24));
			j.value(ENTRY_FIELD(rechamberTime));
			j.value(ENTRY_FIELD(rechamberTimeOneHanded));
			j.value(ENTRY_FIELD(rechamberBoltTime));
			j.value(ENTRY_FIELD(unknown40));
			j.value(ENTRY_FIELD(holdFireTime));
			j.value(ENTRY_FIELD(grenadePrimeReadyToThrowTime));
			j.value(ENTRY_FIELD(detonateTime));
			j.value(ENTRY_FIELD(meleeTime));
			j.value(ENTRY_FIELD(meleeChargeTime));
			j.value(ENTRY_FIELD(unknown64));
			j.value(ENTRY_FIELD(reloadTime));
			j.value(ENTRY_FIELD(unknown72));
			j.value(ENTRY_FIELD(reloadShowRocketTime));
			j.value(ENTRY_FIELD(reloadEmptyTime));
			j.value(ENTRY_FIELD(reloadAddTime));
			j.value(ENTRY_FIELD(reloadEmptyAddTime));
			j.value(ENTRY_FIELD(reloadStartTime));
			j.value(ENTRY_FIELD(reloadStartAddTime));
			j.value(ENTRY_FIELD(reloadEndTime));
			j.value(ENTRY_FIELD(reloadTimeDualWield));
			j.value(ENTRY_FIELD(reloadAddTimeDualWield));
			j.value(ENTRY_FIELD(reloadEmptyDualMag));
			j.value(ENTRY_FIELD(reloadEmptyAddTimeDualMag));
			j.value(ENTRY_FIELD(speedReloadTime));
			j.value(ENTRY_FIELD(speedReloadAddTime));
			j.value(ENTRY_FIELD(dropTime));
			j.value(ENTRY_FIELD(raiseTime));
			j.value(ENTRY_FIELD(unknown152));
			j.value(ENTRY_FIELD(quickDropTime));
			j.value(ENTRY_FIELD(quickRaiseTime));
			j.value(ENTRY_FIELD(firstRaiseTime));
			j.value(ENTRY_FIELD(breachRaiseTime));
			j.value(ENTRY_FIELD(emptyRaiseTime));
			j.value(ENTRY_FIELD(emptyDropTime));
			j.value(ENTRY_FIELD(sprintInTime));
			j.value(ENTRY_FIELD(sprintLoopTime));
			j.value(ENTRY_FIELD(sprintOutTime));
			j.value(ENTRY_FIELD(unknown208));
			j.value(ENTRY_FIELD(stunnedTimeBegin));
			j.value(ENTRY_FIELD(stunnedTimeLoop));
			j.value(ENTRY_FIELD(stunnedTimeEnd));
			j.value(ENTRY_FIELD(nightVisionWearTime));
			j.value(ENTRY_FIELD(nightVisionWearTimeFadeOutEnd));
			j.value(ENTRY_FIELD(nightVisionWearTimePowerUp));
			j.value(ENTRY_FIELD(nightVisionRemoveTime));
			j.value(ENTRY_FIELD(nightVisionRemoveTimePowerDown));
			j.value(ENTRY_FIELD(nightVisionRemoveTimeFadeInStart));
			j.value(ENTRY_FIELD(aiFuseTime));
			j.value(ENTRY_FIELD(fuseTime));
			j.value(ENTRY_FIELD(missileTime));
			j.value(ENTRY_FIELD(primeTime));
			j.value(ENTRY_FIELD(bHoldFullPrime));
			j.value(ENTRY_FIELD(blastFrontTime));
			j.value(ENTRY_FIELD(blastRightTime));
			j.value(ENTRY_FIELD(blastBackTime));
			j.value(ENTRY_FIELD(blastLeftTime));
			j.value(ENTRY_FIELD(slideInTime));
			j.value(ENTRY_FIELD(slideLoopTime));
			j.value(ENTRY_FIELD(slideOutTime));
			j.value(ENTRY_FIELD(unknown300));
		}

#define WEAPON_FIELD(field) #field, w.field
		template <typename J>
		void visit_weapon_json(const J& j, WeaponCompleteDef& w)
		{
			j.string(WEAPON_FIELD(szInternalName));
			j.string(WEAPON_FIELD(szDisplayName));
			j.asset_array(WEAPON_FIELD(gunModel), model_count);
			j.asset_array(WEAPON_FIELD(unknown24), model_count);
			j.asset(WEAPON_FIELD(handModel));
			j.string(WEAPON_FIELD(lobWorldModelName));
			j.named_assets(WEAPON_FIELD(szXAnimsRightHanded), anim_names);
			j.named_assets(WEAPON_FIELD(szXAnimsLeftHanded), anim_names);
			j.script_strings(WEAPON_FIELD(hideTags), hide_tag_count);
			j.asset_list(WEAPON_FIELD(attachments), w.numAttachments);
			j.value(WEAPON_FIELD(unknown80));
			j.named_assets(WEAPON_FIELD(szXAnims), anim_names);
			j.entries(WEAPON_FIELD(animOverrides), w.numAnimOverrides, [](const auto& e, AnimOverrideEntry& entry)
			{
				e.value(ENTRY_FIELD(animHand));
				e.value(ENTRY_FIELD(attachment1));
				e.value(ENTRY_FIELD(attachment2));
				e.value(ENTRY_FIELD(animTreeType));
				e.asset(ENTRY_FIELD(overrideAnim));
				e.asset(ENTRY_FIELD(altmodeAnim));
				e.value(ENTRY_FIELD(animTime));
				e.value(ENTRY_FIELD(altTime));
			});
			j.entries(WEAPON_FIELD(soundOverrides), w.numSoundOverrides, [](const auto& e, SoundOverrideEntry& entry)
			{
				e.value(ENTRY_FIELD(attachment1));
				e.value(ENTRY_FIELD(attachment2));
				e.value(ENTRY_FIELD(soundType));
				e.asset(ENTRY_FIELD(overrideSound));
				e.asset(ENTRY_FIELD(altmodeSound));
			});
			j.entries(WEAPON_FIELD(fxOverrides), w.numFXOverrides, [](const auto& e, FXOverrideEntry& entry)
			{
				e.value(ENTRY_FIELD(attachment1));
				e.value(ENTRY_FIELD(attachment2));
				e.value(ENTRY_FIELD(fxType));
				e.asset(ENTRY_FIELD(overrideFX));
				e.asset(ENTRY_FIELD(altmodeFX));
			});
			j.entries(WEAPON_FIELD(reloadOverrides), w.numReloadStateTimerOverrides, [](const auto& e, ReloadStateTimerEntry& entry)
			{
				e.value(ENTRY_FIELD(attachment));
				e.value(ENTRY_FIELD(reloadAddTime));
				e.value(ENTRY_FIELD(reloadEmptyAddTime));
				e.value(ENTRY_FIELD(reloadStartAddTime));
			});
			j.entries(WEAPON_FIELD(notetrackOverrides), w.numNotetrackOverrides, [](const auto& e, NoteTrackToSoundEntry& entry)
			{
				e.value(ENTRY_FIELD(attachment));
				e.script_strings(ENTRY_FIELD(notetrackSoundMapKeys), notetrack_sound_count);
				e.script_strings(ENTRY_FIELD(notetrackSoundMapValues), notetrack_sound_count);
			});
			j.script_strings(WEAPON_FIELD(notetrackSoundMapKeys), notetrack_sound_count);
			j.script_strings(WEAPON_FIELD(notetrackSoundMapValues), notetrack_sound_count);
			j.script_strings(WEAPON_FIELD(notetrackRumbleMapKeys), notetrack_map_count);
			j.script_strings(WEAPON_FIELD(notetrackRumbleMapValues), notetrack_map_count);
			j.script_strings(WEAPON_FIELD(notetrackFXMapKeys), notetrack_map_count);
			j.asset_array(WEAPON_FIELD(notetrackFXMapValues), notetrack_map_count);
			j.script_strings(WEAPON_FIELD(notetrackFXMapTagValues), notetrack_map_count);
			j.string(WEAPON_FIELD(szAdsrBaseSetting));
			j.asset(WEAPON_FIELD(viewFlashEffect));
			j.asset(WEAPON_FIELD(viewBodyFlashEffect));
			j.asset(WEAPON_FIELD(worldFlashEffect));
			j.asset(WEAPON_FIELD(viewFlashADSEffect));
			j.asset(WEAPON_FIELD(viewBodyFlashADSEffect));
			j.asset(WEAPON_FIELD(signatureViewFlashEffect));
			j.asset(WEAPON_FIELD(signatureViewBodyFlashEffect));
			j.asset(WEAPON_FIELD(signatureWorldFlashEffect));
			j.asset(WEAPON_FIELD(signatureViewFlashADSEffect));
			j.asset(WEAPON_FIELD(signatureViewBodyFlashADSEffect));
			j.asset(WEAPON_FIELD(unknownEffect280));
			j.asset(WEAPON_FIELD(meleeHitEffect));
			j.asset(WEAPON_FIELD(meleeMissEffect));
			j.asset(WEAPON_FIELD(pickupSound));
			j.asset(WEAPON_FIELD(pickupSoundPlayer));
			j.asset(WEAPON_FIELD(ammoPickupSound));
			j.asset(WEAPON_FIELD(ammoPickupSoundPlayer));
			j.asset(WEAPON_FIELD(projectileSound));
			j.asset(WEAPON_FIELD(pullbackSound));
			j.asset(WEAPON_FIELD(pullbackSoundPlayer));
			j.asset(WEAPON_FIELD(throwbackSound));
			j.asset(WEAPON_FIELD(throwbackSoundPlayer));
			j.asset(WEAPON_FIELD(fireSound));
			j.asset(WEAPON_FIELD(fireSoundPlayer));
			j.asset(WEAPON_FIELD(fireSoundPlayerAkimbo));
			j.asset(WEAPON_FIELD(fireLoopSound));
			j.asset(WEAPON_FIELD(fireLoopSoundPlayer));
			j.asset(WEAPON_FIELD(fireLoopEndPointSound));
			j.asset(WEAPON_FIELD(fireLoopEndPointSoundPlayer));
			j.asset(WEAPON_FIELD(fireStopSound));
			j.asset(WEAPON_FIELD(fireStopSoundPlayer));
			j.asset(WEAPON_FIELD(fireLastSound));
			j.asset(WEAPON_FIELD(fireLastSoundPlayer));
			j.asset(WEAPON_FIELD(fireFirstSound));
			j.asset(WEAPON_FIELD(fireFirstSoundPlayer));
			j.asset(WEAPON_FIELD(fireCustomSound));
			j.asset(WEAPON_FIELD(fireCustomSoundPlayer));
			j.asset(WEAPON_FIELD(emptyFireSound));
			j.asset(WEAPON_FIELD(emptyFireSoundPlayer));
			j.asset(WEAPON_FIELD(adsRequiredFireSoundPlayer));
			j.asset(WEAPON_FIELD(meleeSwipeSound));
			j.asset(WEAPON_FIELD(meleeSwipeSoundPlayer));
			j.asset(WEAPON_FIELD(meleeHitSound));
			j.asset(WEAPON_FIELD(meleeMissSound));
			j.asset(WEAPON_FIELD(meleeHitSoundPlayer));
			j.asset(WEAPON_FIELD(meleeMissSoundPlayer));
			j.asset(WEAPON_FIELD(rechamberSound));
			j.asset(WEAPON_FIELD(rechamberSoundPlayer));
			j.asset(WEAPON_FIELD(reloadSound));
			j.asset(WEAPON_FIELD(reloadSoundPlayer));
			j.asset(WEAPON_FIELD(reloadEmptySound));
			j.asset(WEAPON_FIELD(reloadEmptySoundPlayer));
			j.asset(WEAPON_FIELD(reloadStartSound));
			j.asset(WEAPON_FIELD(reloadStartSoundPlayer));
			j.asset(WEAPON_FIELD(reloadEndSound));
			j.asset(WEAPON_FIELD(reloadEndSoundPlayer));
			j.asset(WEAPON_FIELD(detonateSound));
			j.asset(WEAPON_FIELD(detonateSoundPlayer));
			j.asset(WEAPON_FIELD(nightVisionWearSound));
			j.asset(WEAPON_FIELD(nightVisionWearSoundPlayer));
			j.asset(WEAPON_FIELD(nightVisionRemoveSound));
			j.asset(WEAPON_FIELD(nightVisionRemoveSoundPlayer));
			j.asset(WEAPON_FIELD(altSwitchSound));
			j.asset(WEAPON_FIELD(altSwitchSoundPlayer));
			j.asset(WEAPON_FIELD(raiseSound));
			j.asset(WEAPON_FIELD(raiseSoundPlayer));
			j.asset(WEAPON_FIELD(firstRaiseSound));
			j.asset(WEAPON_FIELD(firstRaiseSoundPlayer));
			j.asset(WEAPON_FIELD(putawaySound));
			j.asset(WEAPON_FIELD(putawaySoundPlayer));
			j.asset(WEAPON_FIELD(scanSound));
			j.asset(WEAPON_FIELD(changeVariableZoomSound));
			j.asset(WEAPON_FIELD(adsUpSound));
			j.asset(WEAPON_FIELD(adsDownSound));
			j.asset(WEAPON_FIELD(adsCrosshairEnemySound));
			j.asset(WEAPON_FIELD(unknownSound800));
			j.asset(WEAPON_FIELD(unknownSound808));
			j.asset(WEAPON_FIELD(unknownSound816));
			j.asset_array(WEAPON_FIELD(bounceSound), surface_type_count);
			j.asset_array(WEAPON_FIELD(rollingSound), surface_type_count);
			j.asset(WEAPON_FIELD(viewShellEjectEffect));
			j.asset(WEAPON_FIELD(worldShellEjectEffect));
			j.asset(WEAPON_FIELD(viewLastShotEjectEffect));
			j.asset(WEAPON_FIELD(worldLastShotEjectEffect));
			j.asset(WEAPON_FIELD(viewMagEjectEffect));
			j.asset(WEAPON_FIELD(reticleCenter));
			j.asset(WEAPON_FIELD(unknownMaterial888));
			j.asset(WEAPON_FIELD(reticleSide));
			j.asset_array(WEAPON_FIELD(worldModel), model_count);
			j.asset_array(WEAPON_FIELD(unknown912), model_count);
			j.asset(WEAPON_FIELD(worldClipModel));
			j.asset(WEAPON_FIELD(rocketModel));
			j.asset(WEAPON_FIELD(hudIcon));
			j.asset(WEAPON_FIELD(pickupIcon));
			j.asset(WEAPON_FIELD(unknownMaterial952));
			j.asset(WEAPON_FIELD(ammoCounterIcon));
			j.string(WEAPON_FIELD(szAmmoName));
			j.string(WEAPON_FIELD(szClipName));
			j.string(WEAPON_FIELD(szSharedAmmoCapName));
			j.asset(WEAPON_FIELD(physCollmap));
			j.asset(WEAPON_FIELD(physPreset));
			j.string(WEAPON_FIELD(szUseHintString));
			j.string(WEAPON_FIELD(dropHintString));
			j.array(WEAPON_FIELD(locationDamageMultipliers), location_damage_count);
			j.string(WEAPON_FIELD(fireRumble));
			j.string(WEAPON_FIELD(unknownRumble1040));
			j.string(WEAPON_FIELD(meleeImpactRumble));
			j.asset(WEAPON_FIELD(tracerType));
			j.asset(WEAPON_FIELD(signatureTracerType));
			j.asset(WEAPON_FIELD(unknownTracer1072));
			j.asset(WEAPON_FIELD(laserType));
			j.assets(WEAPON_FIELD(beamType));
			j.asset(WEAPON_FIELD(turretOverheatSound));
			j.asset(WEAPON_FIELD(turretOverheatEffect));
			j.string(WEAPON_FIELD(turretBarrelSpinRumble));
			j.asset(WEAPON_FIELD(turretBarrelSpinMaxSnd));
			j.assets(WEAPON_FIELD(turretBarrelSpinUpSnd));
			j.assets(WEAPON_FIELD(turretBarrelSpinDownSnd));
			j.asset(WEAPON_FIELD(missileConeSoundAlias));
			j.asset(WEAPON_FIELD(missileConeSoundAliasAtBase));
			j.asset(WEAPON_FIELD(stowOffsetModel));
			j.optional_object(WEAPON_FIELD(turretHydraulicSettings), [](const auto& e, TurretHydraulicSettings& entry)
			{
				e.value(ENTRY_FIELD(unknown0));
				e.value(ENTRY_FIELD(unknown4));
				e.value(ENTRY_FIELD(minVelocity));
				e.value(ENTRY_FIELD(unknown12));
				e.asset(ENTRY_FIELD(verticalSound));
				e.asset(ENTRY_FIELD(verticalStopSound));
				e.asset(ENTRY_FIELD(horizontalSound));
				e.asset(ENTRY_FIELD(horizontalStopSound));
				e.assets(ENTRY_FIELD(movementSounds));
				e.value(ENTRY_FIELD(unknown64));
				e.value(ENTRY_FIELD(unknown68));
			});
			j.value(WEAPON_FIELD(playerAnimType));
			j.value(WEAPON_FIELD(unknown1239));
			j.value(WEAPON_FIELD(weapType));
			j.value(WEAPON_FIELD(weapClass));
			j.value(WEAPON_FIELD(penetrateType));
			j.value(WEAPON_FIELD(penetrateDepth));
			j.value(WEAPON_FIELD(impactType));
			j.value(WEAPON_FIELD(unknown1260));
			j.value(WEAPON_FIELD(inventoryType));
			j.value(WEAPON_FIELD(fireType));
			j.value(WEAPON_FIELD(fireBarrels));
			j.value(WEAPON_FIELD(adsFireMode));
			j.value(WEAPON_FIELD(burstFireCooldown));
			j.value(WEAPON_FIELD(greebleType));
			j.value(WEAPON_FIELD(autoReloadType));
			j.value(WEAPON_FIELD(autoHolsterType));
			j.value(WEAPON_FIELD(offhandClass));
			j.value(WEAPON_FIELD(stance));
			j.value(WEAPON_FIELD(reticleCenterSize));
			j.value(WEAPON_FIELD(unknown1308));
			j.value(WEAPON_FIELD(reticleSideSize));
			j.value(WEAPON_FIELD(reticleMinOfs));
			j.value(WEAPON_FIELD(activeReticleType));
			j.value(WEAPON_FIELD(standMove));
			j.value(WEAPON_FIELD(standRot));
			j.value(WEAPON_FIELD(strafeMove));
			j.value(WEAPON_FIELD(strafeRot));
			j.value(WEAPON_FIELD(duckedOfs));
			j.value(WEAPON_FIELD(duckedMove));
			j.value(WEAPON_FIELD(duckedRot));
			j.value(WEAPON_FIELD(proneOfs));
			j.value(WEAPON_FIELD(proneMove));
			j.value(WEAPON_FIELD(proneRot));
			j.value(WEAPON_FIELD(posMoveRate));
			j.value(WEAPON_FIELD(posProneMoveRate));
			j.value(WEAPON_FIELD(standMoveMinSpeed));
			j.value(WEAPON_FIELD(duckedMoveMinSpeed));
			j.value(WEAPON_FIELD(proneMoveMinSpeed));
			j.value(WEAPON_FIELD(posRotRate));
			j.value(WEAPON_FIELD(posProneRotRate));
			j.value(WEAPON_FIELD(hudIconRatio));
			j.value(WEAPON_FIELD(pickupIconRatio));
			j.value(WEAPON_FIELD(ammoCounterIconRatio));
			j.value(WEAPON_FIELD(ammoCounterClip));
			j.value(WEAPON_FIELD(unknown1488));
			j.value(WEAPON_FIELD(maxAmmo));
			j.value(WEAPON_FIELD(unknown1528));
			j.value(WEAPON_FIELD(minAmmoReq));
			j.value(WEAPON_FIELD(startAmmo));
			j.value(WEAPON_FIELD(unknown1540));
			j.value(WEAPON_FIELD(clipSize));
			j.value(WEAPON_FIELD(shotCount));
			j.value(WEAPON_FIELD(sharedAmmoCapIndex));
			j.value(WEAPON_FIELD(sharedAmmoCap));
			j.value(WEAPON_FIELD(unknown1560));
			j.value(WEAPON_FIELD(unknown1564));
			j.value(WEAPON_FIELD(meleeDamage));
			j.value(WEAPON_FIELD(damageType));
			j.object(WEAPON_FIELD(stateTimers), visit_state_timers<J>);
			j.object(WEAPON_FIELD(akimboStateTimers), visit_state_timers<J>);
			j.value(WEAPON_FIELD(unknown2216));
			j.value(WEAPON_FIELD(autoAimRange));
			j.value(WEAPON_FIELD(aimAssistRange));
			j.value(WEAPON_FIELD(aimAssistRangeAds));
			j.value(WEAPON_FIELD(aimPadding));
			j.value(WEAPON_FIELD(unknown2240));
			j.value(WEAPON_FIELD(unknown2244));
			j.value(WEAPON_FIELD(unknown2248));
			j.value(WEAPON_FIELD(unknown2252));
			j.value(WEAPON_FIELD(enemyCrosshairRange));
			j.value(WEAPON_FIELD(moveSpeedScale));
			j.value(WEAPON_FIELD(adsMoveSpeedScale));
			j.value(WEAPON_FIELD(sprintDurationScale));
			j.value(WEAPON_FIELD(unknown2276));
			j.value(WEAPON_FIELD(adsZoomFov));
			j.value(WEAPON_FIELD(adsZoomInFrac));
			j.value(WEAPON_FIELD(adsZoomOutFrac));
			j.value(WEAPON_FIELD(adsSceneBlur));
			j.object(WEAPON_FIELD(overlay), [](const auto& e, ADSOverlay& entry)
			{
				e.asset(ENTRY_FIELD(shader));
				e.asset(ENTRY_FIELD(shaderLowRes));
				e.asset(ENTRY_FIELD(shaderEMP));
				e.asset(ENTRY_FIELD(shaderEMPLowRes));
				e.value(ENTRY_FIELD(reticle));
				e.value(ENTRY_FIELD(width));
				e.value(ENTRY_FIELD(height));
				e.value(ENTRY_FIELD(widthSplitscreen));
				e.value(ENTRY_FIELD(heightSplitscreen));
			});
			j.value(WEAPON_FIELD(overlayInterface));
			j.value(WEAPON_FIELD(adsBobFactor));
			j.value(WEAPON_FIELD(adsViewBobMult));
			j.value(WEAPON_FIELD(hipSpreadDecayRate));
			j.value(WEAPON_FIELD(hipSpreadStandMin));
			j.value(WEAPON_FIELD(hipSpreadStandMax));
			j.value(WEAPON_FIELD(hipSpreadStandDecay));
			j.value(WEAPON_FIELD(hipSpreadDuckedMin));
			j.value(WEAPON_FIELD(hipSpreadDuckedMax));
			j.value(WEAPON_FIELD(hipSpreadDuckedDecay));
			j.value(WEAPON_FIELD(hipSpreadProneMin));
			j.value(WEAPON_FIELD(hipSpreadProneMax));
			j.value(WEAPON_FIELD(hipSpreadProneDecay));
			j.value(WEAPON_FIELD(hipSpreadSprintMin));
			j.value(WEAPON_FIELD(hipSpreadSprintMax));
			j.value(WEAPON_FIELD(hipSpreadSprintDecay));
			j.value(WEAPON_FIELD(hipSpreadSlideMin));
			j.value(WEAPON_FIELD(hipSpreadSlideMax));
			j.value(WEAPON_FIELD(hipSpreadSlideDecay));
			j.value(WEAPON_FIELD(hipSpreadMoveAdd));
			j.value(WEAPON_FIELD(hipSpreadTurnAdd));
			j.value(WEAPON_FIELD(hipSpreadFireAdd));
			j.value(WEAPON_FIELD(hipReticleSidePos));
			j.value(WEAPON_FIELD(adsIdleAmount));
			j.value(WEAPON_FIELD(hipIdleAmount));
			j.value(WEAPON_FIELD(adsIdleSpeed));
			j.value(WEAPON_FIELD(hipIdleSpeed));
			j.value(WEAPON_FIELD(unknown2500));
			j.value(WEAPON_FIELD(idleCrouchFactor));
			j.value(WEAPON_FIELD(idleProneFactor));
			j.value(WEAPON_FIELD(gunMaxPitch));
			j.value(WEAPON_FIELD(gunMaxYaw));
			j.value(WEAPON_FIELD(adsTransInTime));
			j.value(WEAPON_FIELD(adsTransInFromSprintTime));
			j.value(WEAPON_FIELD(adsTransOutTime));
			j.value(WEAPON_FIELD(unknown2532));
			j.value(WEAPON_FIELD(unknown2540));
			j.value(WEAPON_FIELD(swayMaxAngle));
			j.value(WEAPON_FIELD(swayMaxAngleSteadyAim));
			j.value(WEAPON_FIELD(swayLerpSpeed));
			j.value(WEAPON_FIELD(swayPitchScale));
			j.value(WEAPON_FIELD(swayYawScale));
			j.value(WEAPON_FIELD(swayHorizScale));
			j.value(WEAPON_FIELD(swayVertScale));
			j.value(WEAPON_FIELD(swayShellShockScale));
			j.value(WEAPON_FIELD(adsSwayMaxAngle));
			j.value(WEAPON_FIELD(adsSwayLerpSpeed));
			j.value(WEAPON_FIELD(adsSwayPitchScale));
			j.value(WEAPON_FIELD(adsSwayYawScale));
			j.value(WEAPON_FIELD(adsSwayHorizScale));
			j.value(WEAPON_FIELD(adsSwayVertScale));
			j.value(WEAPON_FIELD(adsViewErrorMin));
			j.value(WEAPON_FIELD(adsViewErrorMax));
			j.value(WEAPON_FIELD(adsFireAnimFrac));
			j.value(WEAPON_FIELD(unknown2612));
			j.value(WEAPON_FIELD(bobVerticalFactor));
			j.value(WEAPON_FIELD(bobHorizontalFactor));
			j.value(WEAPON_FIELD(bobViewVerticalFactor));
			j.value(WEAPON_FIELD(bobViewHorizontalFactor));
			j.value(WEAPON_FIELD(stationaryZoomFov));
			j.value(WEAPON_FIELD(stationaryZoomDelay));
			j.value(WEAPON_FIELD(stationaryZoomLerpInTime));
			j.value(WEAPON_FIELD(stationaryZoomLerpOutTime));
			j.value(WEAPON_FIELD(adsDofStart));
			j.value(WEAPON_FIELD(adsDofEnd));
			j.value(WEAPON_FIELD(unknown2676));
			j.asset(WEAPON_FIELD(killIcon));
			j.asset(WEAPON_FIELD(dpadIcon));
			j.asset(WEAPON_FIELD(hudProximityWarningIcon));
			j.value(WEAPON_FIELD(killIconRatio));
			j.value(WEAPON_FIELD(dpadIconRatio));
			j.value(WEAPON_FIELD(fireAnimLength));
			j.value(WEAPON_FIELD(fireAnimLengthAkimbo));
			j.value(WEAPON_FIELD(reloadAmmoAdd));
			j.value(WEAPON_FIELD(reloadStartAdd));
			j.value(WEAPON_FIELD(ammoDropStockMin));
			j.value(WEAPON_FIELD(ammoDropStockMax));
			j.value(WEAPON_FIELD(ammoDropClipPercentMin));
			j.value(WEAPON_FIELD(ammoDropClipPercentMax));
			j.value(WEAPON_FIELD(explosionRadius));
			j.value(WEAPON_FIELD(explosionRadiusMin));
			j.value(WEAPON_FIELD(explosionInnerDamage));
			j.value(WEAPON_FIELD(explosionOuterDamage));
			j.value(WEAPON_FIELD(unknown2760));
			j.value(WEAPON_FIELD(damageConeAngle));
			j.value(WEAPON_FIELD(unknown2772));
			j.value(WEAPON_FIELD(projectileSpeed));
			j.value(WEAPON_FIELD(projectileSpeedUp));
			j.value(WEAPON_FIELD(projectileSpeedForward));
			j.value(WEAPON_FIELD(unknown2796));
			j.value(WEAPON_FIELD(projectileActivateDist));
			j.value(WEAPON_FIELD(projLifetime));
			j.value(WEAPON_FIELD(timeToAccelerate));
			j.value(WEAPON_FIELD(projectileCurvature));
			j.value(WEAPON_FIELD(unknown2816));
			j.string(WEAPON_FIELD(projectileName));
			j.asset(WEAPON_FIELD(projectileModel));
			j.asset(WEAPON_FIELD(projExplosionEffect));
			j.asset(WEAPON_FIELD(projDudEffect));
			j.asset(WEAPON_FIELD(unknownEffect2880));
			j.asset(WEAPON_FIELD(projExplosionSound));
			j.asset(WEAPON_FIELD(projDudSound));
			j.asset(WEAPON_FIELD(unknownSound2904));
			j.value(WEAPON_FIELD(projExplosion));
			j.value(WEAPON_FIELD(stickiness));
			j.value(WEAPON_FIELD(unknown2920));
			j.value(WEAPON_FIELD(lowAmmoWarningThreshold));
			j.value(WEAPON_FIELD(ricochetChance));
			j.value(WEAPON_FIELD(riotShieldHealth));
			j.value(WEAPON_FIELD(riotShieldDamageMult));
			j.value(WEAPON_FIELD(unknown2940));
			j.array(WEAPON_FIELD(parallelBounce), surface_type_count);
			j.array(WEAPON_FIELD(perpendicularBounce), surface_type_count);
			j.asset(WEAPON_FIELD(projTrailEffect));
			j.asset(WEAPON_FIELD(projBeaconEffect));
			j.value(WEAPON_FIELD(projectileColor));
			j.value(WEAPON_FIELD(guidedMissileType));
			j.value(WEAPON_FIELD(maxSteeringAccel));
			j.value(WEAPON_FIELD(unknown2996));
			j.asset(WEAPON_FIELD(projIgnitionEffect));
			j.asset(WEAPON_FIELD(projIgnitionSound));
			j.value(WEAPON_FIELD(adsAimPitch));
			j.value(WEAPON_FIELD(adsCrosshairInFrac));
			j.value(WEAPON_FIELD(adsCrosshairOutFrac));
			j.value(WEAPON_FIELD(adsGunKickPitchMin));
			j.value(WEAPON_FIELD(adsGunKickPitchMax));
			j.value(WEAPON_FIELD(adsGunKickYawMin));
			j.value(WEAPON_FIELD(adsGunKickYawMax));
			j.value(WEAPON_FIELD(adsGunKickMagMin));
			j.value(WEAPON_FIELD(adsGunKickAccel));
			j.value(WEAPON_FIELD(adsGunKickSpeedMax));
			j.value(WEAPON_FIELD(adsGunKickSpeedDecay));
			j.value(WEAPON_FIELD(adsGunKickStaticDecay));
			j.value(WEAPON_FIELD(adsGunKickReducedKickBullets));
			j.value(WEAPON_FIELD(adsGunKickReducedKickPercent));
			j.value(WEAPON_FIELD(adsViewKickPitchMin));
			j.value(WEAPON_FIELD(adsViewKickPitchMax));
			j.value(WEAPON_FIELD(adsViewKickYawMin));
			j.value(WEAPON_FIELD(adsViewKickYawMax));
			j.value(WEAPON_FIELD(adsViewKickMagMin));
			j.value(WEAPON_FIELD(adsViewKickCenterSpeed));
			j.value(WEAPON_FIELD(adsViewScatterMin));
			j.value(WEAPON_FIELD(adsViewScatterMax));
			j.value(WEAPON_FIELD(unknown3136));
			j.value(WEAPON_FIELD(unknown3140));
			j.value(WEAPON_FIELD(unknown3204));
			j.value(WEAPON_FIELD(unknown3208));
			j.value(WEAPON_FIELD(hipGunKickPitchMin));
			j.value(WEAPON_FIELD(hipGunKickPitchMax));
			j.value(WEAPON_FIELD(hipGunKickYawMin));
			j.value(WEAPON_FIELD(hipGunKickYawMax));
			j.value(WEAPON_FIELD(hipGunKickMagMin));
			j.value(WEAPON_FIELD(hipGunKickAccel));
			j.value(WEAPON_FIELD(hipGunKickSpeedMax));
			j.value(WEAPON_FIELD(hipGunKickSpeedDecay));
			j.value(WEAPON_FIELD(hipGunKickStaticDecay));
			j.value(WEAPON_FIELD(hipGunKickReducedKickBullets));
			j.value(WEAPON_FIELD(hipGunKickReducedKickPercent));
			j.value(WEAPON_FIELD(hipViewKickPitchMin));
			j.value(WEAPON_FIELD(hipViewKickPitchMax));
			j.value(WEAPON_FIELD(hipViewKickYawMin));
			j.value(WEAPON_FIELD(hipViewKickYawMax));
			j.value(WEAPON_FIELD(hipViewKickMagMin));
			j.value(WEAPON_FIELD(hipViewKickCenterSpeed));
			j.value(WEAPON_FIELD(hipViewScatterMin));
			j.value(WEAPON_FIELD(hipViewScatterMax));
			j.value(WEAPON_FIELD(unknown3304));
			j.value(WEAPON_FIELD(unknown3308));
			j.value(WEAPON_FIELD(positionReloadTransTime));
			j.value(WEAPON_FIELD(unknown3388));
			j.value(WEAPON_FIELD(unknown3392));
			j.strings(WEAPON_FIELD(accuracyGraphName));
			j.graph_knots(WEAPON_FIELD(accuracyGraphKnots), WEAPON_FIELD(originalAccuracyGraphKnots), w.accuracyGraphKnotCount);
			j.value(WEAPON_FIELD(originalAccuracyGraphKnotCount));
			j.value(WEAPON_FIELD(unknown3456));
			j.string(WEAPON_FIELD(szScript));
			j.value(WEAPON_FIELD(unknown3568));
			j.value(WEAPON_FIELD(damage));
			j.value(WEAPON_FIELD(unknown3576));
			j.value(WEAPON_FIELD(unknown3580));
			j.value(WEAPON_FIELD(unknown3584));
			j.value(WEAPON_FIELD(unknown3588));
			j.value(WEAPON_FIELD(unknown3592));
			j.value(WEAPON_FIELD(unknown3596));
			j.value(WEAPON_FIELD(unknown3600));
			j.value(WEAPON_FIELD(unknown3604));
			j.value(WEAPON_FIELD(playerDamage));
			j.value(WEAPON_FIELD(maxDamageRange));
			j.value(WEAPON_FIELD(minDamage));
			j.value(WEAPON_FIELD(minPlayerDamage));
			j.value(WEAPON_FIELD(unknown3624));
			j.value(WEAPON_FIELD(unknown3628));
			j.value(WEAPON_FIELD(midDamage));
			j.value(WEAPON_FIELD(midPlayerDamage));
			j.value(WEAPON_FIELD(minDamageRange));
			j.value(WEAPON_FIELD(unknown3648));
			j.value(WEAPON_FIELD(unknown3652));
			j.value(WEAPON_FIELD(signatureDamage));
			j.value(WEAPON_FIELD(signatureMinDamage));
			j.value(WEAPON_FIELD(signatureMidDamage));
			j.value(WEAPON_FIELD(signatureMaxDamageRange));
			j.value(WEAPON_FIELD(signatureMinDamageRange));
			j.value(WEAPON_FIELD(destabilizationRateTime));
			j.value(WEAPON_FIELD(destabilizationCurvatureMax));
			j.value(WEAPON_FIELD(destabilizeDistance));
			j.value(WEAPON_FIELD(turretADSTime));
			j.value(WEAPON_FIELD(turretFov));
			j.value(WEAPON_FIELD(turretFovADS));
			j.value(WEAPON_FIELD(turretScopeZoomRate));
			j.value(WEAPON_FIELD(turretScopeZoomMin));
			j.value(WEAPON_FIELD(turretScopeZoomMax));
			j.value(WEAPON_FIELD(overheatUpRate));
			j.value(WEAPON_FIELD(overheatDownRate));
			j.value(WEAPON_FIELD(overheatCooldownRate));
			j.value(WEAPON_FIELD(overheatPenalty));
			j.value(WEAPON_FIELD(turretBarrelSpinSpeed));
			j.value(WEAPON_FIELD(turretBarrelSpinUpTime));
			j.value(WEAPON_FIELD(missileConeSoundRadiusAtTop));
			j.value(WEAPON_FIELD(missileConeSoundRadiusAtBase));
			j.value(WEAPON_FIELD(missileConeSoundHeight));
			j.value(WEAPON_FIELD(missileConeSoundOriginOffset));
			j.value(WEAPON_FIELD(missileConeSoundVolumescaleAtCore));
			j.value(WEAPON_FIELD(missileConeSoundVolumescaleAtEdge));
			j.value(WEAPON_FIELD(missileConeSoundVolumescaleCoreSize));
			j.value(WEAPON_FIELD(missileConeSoundPitchAtTop));
			j.value(WEAPON_FIELD(missileConeSoundPitchAtBottom));
			j.value(WEAPON_FIELD(missileConeSoundPitchTopSize));
			j.value(WEAPON_FIELD(missileConeSoundPitchBottomSize));
			j.value(WEAPON_FIELD(missileConeSoundCrossfadeTopSize));
			j.value(WEAPON_FIELD(missileConeSoundCrossfadeBottomSize));
			j.script_string(WEAPON_FIELD(knifeAttachTagOverride));
			j.value(WEAPON_FIELD(unknown3824));
			j.value(WEAPON_FIELD(unknown3892));
			j.value(WEAPON_FIELD(unknown3928));
			j.script_string(WEAPON_FIELD(stowTag));
			j.script_string(WEAPON_FIELD(unknownTag3948));
			j.value(WEAPON_FIELD(unknown3952));
			j.value(WEAPON_FIELD(unknown3953));
			j.value(WEAPON_FIELD(adsDofPhysicalFstop));
			j.value(WEAPON_FIELD(adsDofPhysicalFocusDistance));
			j.value(WEAPON_FIELD(unknown4064));
			j.value(WEAPON_FIELD(unknown4072));
			j.value(WEAPON_FIELD(unknown4076));
			j.value(WEAPON_FIELD(unknown4092));
			j.value(WEAPON_FIELD(unknown4096));
			j.value(WEAPON_FIELD(unknown4100));
			j.value(WEAPON_FIELD(unknown4104));
			j.value(WEAPON_FIELD(unknown4200));
			j.value(WEAPON_FIELD(unknown4208));
			j.value(WEAPON_FIELD(unknown4224));
			j.value(WEAPON_FIELD(unknown4228));
			j.value(WEAPON_FIELD(unknown4232));
		}
#undef WEAPON_FIELD
#undef ENTRY_FIELD

		std::string json_path(const std::string& name)
		{
			return "weapons\\" + name + ".json";
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

		void share_equal_arrays(WeaponCompleteDef& w)
		{
			share_if_equal(w.szXAnimsLeftHanded, w.szXAnimsRightHanded, xanim_count);
			share_if_equal(w.szXAnims, w.szXAnimsRightHanded, xanim_count);
			share_if_equal(w.worldModel, w.gunModel, model_count);
			share_if_equal(w.perpendicularBounce, w.parallelBounce, surface_type_count);
			for (auto i = 0u; i < std::size(w.accuracyGraphKnots); i++)
			{
				share_if_equal(w.originalAccuracyGraphKnots[i], w.accuracyGraphKnots[i], w.accuracyGraphKnotCount[i]);
			}
		}

		struct weapon_field_names
		{
			std::unordered_map<std::uint16_t, std::string> by_offset;
			std::unordered_map<std::string, std::uint16_t> by_name;
		};

		class json_field_namer
		{
		public:
			json_field_namer(const WeaponCompleteDef& weapon, std::string prefix, weapon_field_names& names)
				: weapon_(weapon)
				, prefix_(std::move(prefix))
				, names_(names)
			{
			}

			template <typename T>
			void value(const char* name, const T& field) const
			{
				this->add(name, field);
			}

			template <typename T>
			void string(const char* name, const T& field) const
			{
				this->add(name, field);
			}

			template <typename T>
			void strings(const char* name, const T& field) const
			{
				this->add(name, field);
			}

			template <typename T>
			void asset(const char* name, const T& field) const
			{
				this->add(name, field);
			}

			template <typename T>
			void assets(const char* name, const T& field) const
			{
				this->add(name, field);
			}

			template <typename T, typename C>
			void asset_array(const char* name, const T& field, const C&) const
			{
				this->add(name, field);
			}

			template <typename T, typename C>
			void asset_list(const char* name, const T& field, const C&) const
			{
				this->add(name, field);
			}

			template <typename T, typename N>
			void named_assets(const char* name, const T& field, const N&) const
			{
				this->add(name, field);
			}

			template <typename T>
			void script_string(const char* name, const T& field) const
			{
				this->add(name, field);
			}

			template <typename T, typename C>
			void script_strings(const char* name, const T& field, const C&) const
			{
				this->add(name, field);
			}

			template <typename T, typename C>
			void array(const char* name, const T& field, const C&) const
			{
				this->add(name, field);
			}

			template <typename T, typename C>
			void graph_knots(const char* knots_name, const T& knots, const char* original_name, const T& original, const C&) const
			{
				this->add(knots_name, knots);
				this->add(original_name, original);
			}

			template <typename T, typename C, typename F>
			void entries(const char* name, const T& field, const C&, F&&) const
			{
				this->add(name, field);
			}

			template <typename T, typename F>
			void object(const char* name, T& field, F&& callback) const
			{
				callback(json_field_namer(this->weapon_, this->prefix_ + name + ".", this->names_), field);
			}

			template <typename T, typename F>
			void optional_object(const char* name, const T& field, F&&) const
			{
				this->add(name, field);
			}

		private:
			template <typename T>
			void add(const std::string& name, const T& field) const
			{
				if constexpr (std::is_array_v<T>)
				{
					for (auto i = 0u; i < std::extent_v<T>; i++)
					{
						this->add(name + "[" + std::to_string(i) + "]", field[i]);
					}
				}
				else
				{
					const auto offset = static_cast<std::uint16_t>(reinterpret_cast<const char*>(&field) - reinterpret_cast<const char*>(&this->weapon_));
					const auto full_name = this->prefix_ + name;
					this->names_.by_offset.emplace(offset, full_name);
					this->names_.by_name.emplace(full_name, offset);
				}
			}

			const WeaponCompleteDef& weapon_;
			std::string prefix_;
			weapon_field_names& names_;
		};

		const weapon_field_names& get_weapon_field_names()
		{
			static const auto names = []
			{
				weapon_field_names result;
				const auto weapon = std::make_unique<WeaponCompleteDef>();
				visit_weapon_json(json_field_namer(*weapon, "", result), *weapon);
				return result;
			}();

			return names;
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
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->unknown24, model_count);
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
		read_sounds(reader, pointer_run(asset->pickupSound, asset->unknownSound816));
		reader.read_name_reference_array(asset->bounceSound, surface_type_count);
		reader.read_name_reference_array(asset->rollingSound, surface_type_count);
		read_assets(reader, ASSET_TYPE_FX, pointer_run(asset->viewShellEjectEffect, asset->viewMagEjectEffect));
		read_assets(reader, ASSET_TYPE_MATERIAL, pointer_run(asset->reticleCenter, asset->reticleSide));
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->worldModel, model_count);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->unknown912, model_count);
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
		add_dependencies(zone, ASSET_TYPE_XMODEL, data->unknown24, model_count);
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
		add_dependencies(zone, ASSET_TYPE_SOUND, pointer_run(data->pickupSound, data->unknownSound816));
		add_dependencies(zone, ASSET_TYPE_SOUND, data->bounceSound, surface_type_count);
		add_dependencies(zone, ASSET_TYPE_SOUND, data->rollingSound, surface_type_count);
		add_dependencies(zone, ASSET_TYPE_FX, pointer_run(data->viewShellEjectEffect, data->viewMagEjectEffect));
		add_dependencies(zone, ASSET_TYPE_MATERIAL, pointer_run(data->reticleCenter, data->reticleSide));
		add_dependencies(zone, ASSET_TYPE_XMODEL, data->worldModel, model_count);
		add_dependencies(zone, ASSET_TYPE_XMODEL, data->unknown912, model_count);
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
		write_asset_array(zone, buf, ASSET_TYPE_XMODEL, data->unknown24, model_count, dest->unknown24);
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
		write_sounds(buf, pointer_run(data->pickupSound, data->unknownSound816), &dest->pickupSound);
		write_sound_array(buf, data->bounceSound, surface_type_count, dest->bounceSound);
		write_sound_array(buf, data->rollingSound, surface_type_count, dest->rollingSound);
		write_assets(zone, ASSET_TYPE_FX, pointer_run(data->viewShellEjectEffect, data->viewMagEjectEffect), &dest->viewShellEjectEffect);
		write_assets(zone, ASSET_TYPE_MATERIAL, pointer_run(data->reticleCenter, data->reticleSide), &dest->reticleCenter);
		write_asset_array(zone, buf, ASSET_TYPE_XMODEL, data->worldModel, model_count, dest->worldModel);
		write_asset_array(zone, buf, ASSET_TYPE_XMODEL, data->unknown912, model_count, dest->unknown912);
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
		const auto data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto* asset = mem->allocate<WeaponCompleteDef>();
		visit_weapon_json(json_field_parser(data, mem), *asset);
		share_equal_arrays(*asset);
		return asset;
	}

	void weapon_def::dump(WeaponCompleteDef* asset)
	{
		ordered_json data;
		visit_weapon_json(json_field_dumper(data), *asset);
		const auto str = data.dump(4);

		auto file = filesystem::file(json_path(asset->szInternalName));
		file.open("wb");
		file.write(str.data(), str.size(), 1);
		file.close();
	}

	std::optional<std::string> weapon_field_name(const std::uint16_t offset)
	{
		const auto& names = get_weapon_field_names().by_offset;
		const auto name = names.find(offset);
		return name != names.end() ? std::optional(name->second) : std::nullopt;
	}

	std::optional<std::uint16_t> weapon_field_offset(const std::string& name)
	{
		const auto& offsets = get_weapon_field_names().by_name;
		const auto offset = offsets.find(name);
		return offset != offsets.end() ? std::optional(offset->second) : std::nullopt;
	}
}
