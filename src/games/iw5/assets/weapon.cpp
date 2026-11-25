#include <std_include.hpp>
#include "weapon.hpp"

namespace zonetool::iw5
{
	namespace
	{
		void read_sounds(zone_reader32& reader, x86::pointer<SndAliasCustom>& field, const std::size_t count)
		{
			if (const auto sounds = reader.read_array(field, 3, count))
			{
				for (std::size_t i = 0; i < count; i++)
				{
					sound::read_custom(reader, sounds[i]);
				}
			}
		}

		void read_script_string_array(zone_reader32& reader, x86::pointer<scr_string_t>& field, const std::size_t count)
		{
			if (const auto strings = reader.read_array(field, 1, count))
			{
				reader.read_script_strings(strings, count);
			}
		}

		void read_overlay(zone_reader32& reader, ADSOverlay& overlay)
		{
			reader.read_asset(ASSET_TYPE_MATERIAL, overlay.shader);
			reader.read_asset(ASSET_TYPE_MATERIAL, overlay.shaderLowRes);
			reader.read_asset(ASSET_TYPE_MATERIAL, overlay.shaderEMP);
			reader.read_asset(ASSET_TYPE_MATERIAL, overlay.shaderEMPLowRes);
		}

		void read_weapon_def(zone_reader32& reader, WeaponDef* def, const WeaponCompleteDef* asset)
		{
			reader.read_string(def->szOverlayName);
			reader.read_asset_array(ASSET_TYPE_XMODEL, def->gunXModel, 16);
			reader.read_asset(ASSET_TYPE_XMODEL, def->handXModel);
			reader.read_strings(def->szXAnimsRightHanded, WEAP_ANIM_COUNT);
			reader.read_strings(def->szXAnimsLeftHanded, WEAP_ANIM_COUNT);
			reader.read_string(def->szModeName);
			read_script_string_array(reader, def->notetrackSoundMapKeys, 24);
			read_script_string_array(reader, def->notetrackSoundMapValues, 24);
			read_script_string_array(reader, def->notetrackRumbleMapKeys, 16);
			read_script_string_array(reader, def->notetrackRumbleMapValues, 16);
			reader.read_asset(ASSET_TYPE_FX, def->viewFlashEffect);
			reader.read_asset(ASSET_TYPE_FX, def->worldFlashEffect);

			for (const auto alias : {
				&def->pickupSound, &def->pickupSoundPlayer, &def->ammoPickupSound, &def->ammoPickupSoundPlayer,
				&def->projectileSound, &def->pullbackSound, &def->pullbackSoundPlayer, &def->fireSound,
				&def->fireSoundPlayer, &def->fireSoundPlayerAkimbo, &def->fireLoopSound, &def->fireLoopSoundPlayer,
				&def->fireStopSound, &def->fireStopSoundPlayer, &def->fireLastSound, &def->fireLastSoundPlayer,
				&def->emptyFireSound, &def->emptyFireSoundPlayer, &def->meleeSwipeSound, &def->meleeSwipeSoundPlayer,
				&def->meleeHitSound, &def->meleeMissSound, &def->rechamberSound, &def->rechamberSoundPlayer,
				&def->reloadSound, &def->reloadSoundPlayer, &def->reloadEmptySound, &def->reloadEmptySoundPlayer,
				&def->reloadStartSound, &def->reloadStartSoundPlayer, &def->reloadEndSound, &def->reloadEndSoundPlayer,
				&def->detonateSound, &def->detonateSoundPlayer, &def->nightVisionWearSound, &def->nightVisionWearSoundPlayer,
				&def->nightVisionRemoveSound, &def->nightVisionRemoveSoundPlayer, &def->altSwitchSound,
				&def->altSwitchSoundPlayer, &def->raiseSound, &def->raiseSoundPlayer, &def->firstRaiseSound,
				&def->firstRaiseSoundPlayer, &def->putawaySound, &def->putawaySoundPlayer, &def->scanSound,
				&def->changeVariableZoomSound,
			})
			{
				sound::read_custom(reader, *alias);
			}

			read_sounds(reader, def->bounceSound, SURF_TYPE_COUNT);
			read_sounds(reader, def->rollingSound, SURF_TYPE_COUNT);
			reader.read_asset(ASSET_TYPE_FX, def->viewShellEjectEffect);
			reader.read_asset(ASSET_TYPE_FX, def->worldShellEjectEffect);
			reader.read_asset(ASSET_TYPE_FX, def->viewLastShotEjectEffect);
			reader.read_asset(ASSET_TYPE_FX, def->worldLastShotEjectEffect);
			reader.read_asset(ASSET_TYPE_MATERIAL, def->reticleCenter);
			reader.read_asset(ASSET_TYPE_MATERIAL, def->reticleSide);
			reader.read_asset_array(ASSET_TYPE_XMODEL, def->worldModel, 16);
			reader.read_asset(ASSET_TYPE_XMODEL, def->worldClipModel);
			reader.read_asset(ASSET_TYPE_XMODEL, def->rocketModel);
			reader.read_asset(ASSET_TYPE_XMODEL, def->knifeModel);
			reader.read_asset(ASSET_TYPE_XMODEL, def->worldKnifeModel);
			reader.read_asset(ASSET_TYPE_MATERIAL, def->hudIcon);
			reader.read_asset(ASSET_TYPE_MATERIAL, def->pickupIcon);
			reader.read_asset(ASSET_TYPE_MATERIAL, def->ammoCounterIcon);
			reader.read_string(def->szAmmoName);
			reader.read_string(def->szClipName);
			reader.read_string(def->szSharedAmmoCapName);
			read_overlay(reader, def->overlay);
			reader.read_asset(ASSET_TYPE_PHYSCOLLMAP, def->physCollmap);
			reader.read_asset(ASSET_TYPE_XMODEL, def->projectileModel);
			reader.read_asset(ASSET_TYPE_FX, def->projExplosionEffect);
			reader.read_asset(ASSET_TYPE_FX, def->projDudEffect);
			sound::read_custom(reader, def->projExplosionSound);
			sound::read_custom(reader, def->projDudSound);
			reader.read_array(def->parallelBounce, 3, SURF_TYPE_COUNT);
			reader.read_array(def->perpendicularBounce, 3, SURF_TYPE_COUNT);
			reader.read_asset(ASSET_TYPE_FX, def->projTrailEffect);
			reader.read_asset(ASSET_TYPE_FX, def->projBeaconEffect);
			reader.read_asset(ASSET_TYPE_FX, def->projIgnitionEffect);
			sound::read_custom(reader, def->projIgnitionSound);
			reader.read_string(def->aiVsAiAccuracyGraphName);
			reader.read_array(def->originalAiVsAiAccuracyGraphKnots, 3, asset->aiVsAiAccuracyGraphKnotCount);
			reader.read_string(def->aiVsPlayerAccuracyGraphName);
			reader.read_array(def->originalAiVsPlayerAccuracyGraphKnots, 3, asset->aiVsPlayerAccuracyGraphKnotCount);
			reader.read_string(def->szUseHintString);
			reader.read_string(def->dropHintString);
			reader.read_string(def->szScript);
			reader.read_array(def->locationDamageMultipliers, 3, HITLOC_COUNT);
			reader.read_string(def->fireRumble);
			reader.read_string(def->meleeImpactRumble);
			reader.read_asset(ASSET_TYPE_TRACER, def->tracerType);
			sound::read_custom(reader, def->turretOverheatSound);
			reader.read_asset(ASSET_TYPE_FX, def->turretOverheatEffect);
			reader.read_string(def->turretBarrelSpinRumble);
			sound::read_custom(reader, def->turretBarrelSpinMaxSnd);
			for (auto& alias : def->turretBarrelSpinUpSnd)
			{
				sound::read_custom(reader, alias);
			}
			for (auto& alias : def->turretBarrelSpinDownSnd)
			{
				sound::read_custom(reader, alias);
			}
			sound::read_custom(reader, def->missileConeSoundAlias);
			sound::read_custom(reader, def->missileConeSoundAliasAtBase);
			reader.read_asset(ASSET_TYPE_XMODEL, def->stowOffsetModel);
			reader.read_script_string(def->stowTag);
		}
	}

	void weapon::read(zone_reader32& reader, WeaponCompleteDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->szInternalName);
		if (const auto def = reader.read_single(asset->weapDef, 3))
		{
			read_weapon_def(reader, def, asset);
		}
		reader.read_string(asset->szDisplayName);
		read_script_string_array(reader, asset->hideTags, 32);
		reader.read_asset_array(ASSET_TYPE_ATTACHMENT, asset->scopes, 6);
		reader.read_asset_array(ASSET_TYPE_ATTACHMENT, asset->underBarrels, 3);
		reader.read_asset_array(ASSET_TYPE_ATTACHMENT, asset->others, 4);
		reader.read_strings(asset->szXAnims, WEAP_ANIM_COUNT);

		if (const auto entries = reader.read_array(asset->animOverrides, 3, asset->numAnimOverrides))
		{
			for (auto i = 0u; i < asset->numAnimOverrides; i++)
			{
				reader.read_string(entries[i].overrideAnim);
				reader.read_string(entries[i].altmodeAnim);
			}
		}

		if (const auto entries = reader.read_array(asset->soundOverrides, 3, asset->numSoundOverrides))
		{
			for (auto i = 0u; i < asset->numSoundOverrides; i++)
			{
				sound::read_custom(reader, entries[i].overrideSound);
				sound::read_custom(reader, entries[i].altmodeSound);
			}
		}

		if (const auto entries = reader.read_array(asset->fxOverrides, 3, asset->numFxOverrides))
		{
			for (auto i = 0u; i < asset->numFxOverrides; i++)
			{
				reader.read_asset(ASSET_TYPE_FX, entries[i].overrideFx);
				reader.read_asset(ASSET_TYPE_FX, entries[i].altmodeFx);
			}
		}

		reader.read_array(asset->reloadOverrides, 3, asset->numReloadStateTimerOverrides);

		if (const auto entries = reader.read_array(asset->notetrackOverrides, 3, asset->numNotetrackOverrides))
		{
			for (auto i = 0u; i < asset->numNotetrackOverrides; i++)
			{
				read_script_string_array(reader, entries[i].notetrackSoundMapKeys, 24);
				read_script_string_array(reader, entries[i].notetrackSoundMapValues, 24);
			}
		}

		reader.read_string(asset->szAltWeaponName);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->killIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->dpadIcon);
		reader.read_array(asset->aiVsAiAccuracyGraphKnots, 3, asset->aiVsAiAccuracyGraphKnotCount);
		reader.read_array(asset->aiVsPlayerAccuracyGraphKnots, 3, asset->aiVsPlayerAccuracyGraphKnotCount);
		reader.pop_stream();
	}
}
