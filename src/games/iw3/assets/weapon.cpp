#include <std_include.hpp>
#include "weapon.hpp"

namespace zonetool::iw3
{
	namespace
	{
		void read_sound(zone_reader32& reader, SndAliasCustom& sound)
		{
			if (const auto name = reader.read_single(sound.name, 3))
			{
				reader.read_string(name->soundName);
			}
		}
	}

	void weapon::read(zone_reader32& reader, WeaponDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->szInternalName);
		reader.read_string(asset->szDisplayName);
		reader.read_string(asset->szOverlayName);
		for (auto& model : asset->gunXModel)
		{
			reader.read_asset(ASSET_TYPE_XMODEL, model);
		}
		reader.read_asset(ASSET_TYPE_XMODEL, asset->handXModel);
		for (auto& name : asset->szXAnims)
		{
			reader.read_string(name);
		}
		reader.read_string(asset->szModeName);
		reader.read_asset(ASSET_TYPE_FX, asset->viewFlashEffect);
		reader.read_asset(ASSET_TYPE_FX, asset->worldFlashEffect);
		read_sound(reader, asset->pickupSound);
		read_sound(reader, asset->pickupSoundPlayer);
		read_sound(reader, asset->ammoPickupSound);
		read_sound(reader, asset->ammoPickupSoundPlayer);
		read_sound(reader, asset->projectileSound);
		read_sound(reader, asset->pullbackSound);
		read_sound(reader, asset->pullbackSoundPlayer);
		read_sound(reader, asset->fireSound);
		read_sound(reader, asset->fireSoundPlayer);
		read_sound(reader, asset->fireLoopSound);
		read_sound(reader, asset->fireLoopSoundPlayer);
		read_sound(reader, asset->fireStopSound);
		read_sound(reader, asset->fireStopSoundPlayer);
		read_sound(reader, asset->fireLastSound);
		read_sound(reader, asset->fireLastSoundPlayer);
		read_sound(reader, asset->emptyFireSound);
		read_sound(reader, asset->emptyFireSoundPlayer);
		read_sound(reader, asset->meleeSwipeSound);
		read_sound(reader, asset->meleeSwipeSoundPlayer);
		read_sound(reader, asset->meleeHitSound);
		read_sound(reader, asset->meleeMissSound);
		read_sound(reader, asset->rechamberSound);
		read_sound(reader, asset->rechamberSoundPlayer);
		read_sound(reader, asset->reloadSound);
		read_sound(reader, asset->reloadSoundPlayer);
		read_sound(reader, asset->reloadEmptySound);
		read_sound(reader, asset->reloadEmptySoundPlayer);
		read_sound(reader, asset->reloadStartSound);
		read_sound(reader, asset->reloadStartSoundPlayer);
		read_sound(reader, asset->reloadEndSound);
		read_sound(reader, asset->reloadEndSoundPlayer);
		read_sound(reader, asset->detonateSound);
		read_sound(reader, asset->detonateSoundPlayer);
		read_sound(reader, asset->nightVisionWearSound);
		read_sound(reader, asset->nightVisionWearSoundPlayer);
		read_sound(reader, asset->nightVisionRemoveSound);
		read_sound(reader, asset->nightVisionRemoveSoundPlayer);
		read_sound(reader, asset->altSwitchSound);
		read_sound(reader, asset->altSwitchSoundPlayer);
		read_sound(reader, asset->raiseSound);
		read_sound(reader, asset->raiseSoundPlayer);
		read_sound(reader, asset->firstRaiseSound);
		read_sound(reader, asset->firstRaiseSoundPlayer);
		read_sound(reader, asset->putawaySound);
		read_sound(reader, asset->putawaySoundPlayer);
		if (const auto sounds = reader.read_array(asset->bounceSound, 3, 29))
		{
			for (auto i = 0; i < 29; i++)
			{
				read_sound(reader, sounds[i]);
			}
		}
		reader.read_asset(ASSET_TYPE_FX, asset->viewShellEjectEffect);
		reader.read_asset(ASSET_TYPE_FX, asset->worldShellEjectEffect);
		reader.read_asset(ASSET_TYPE_FX, asset->viewLastShotEjectEffect);
		reader.read_asset(ASSET_TYPE_FX, asset->worldLastShotEjectEffect);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->reticleCenter);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->reticleSide);
		for (auto& model : asset->worldModel)
		{
			reader.read_asset(ASSET_TYPE_XMODEL, model);
		}
		reader.read_asset(ASSET_TYPE_XMODEL, asset->worldClipModel);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->rocketModel);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->knifeModel);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->worldKnifeModel);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->hudIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->ammoCounterIcon);
		reader.read_string(asset->szAmmoName);
		reader.read_string(asset->szClipName);
		reader.read_string(asset->szSharedAmmoCapName);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->overlayMaterial);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->overlayMaterialLowRes);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->killIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->dpadIcon);
		reader.read_string(asset->szAltWeaponName);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->projectileModel);
		reader.read_asset(ASSET_TYPE_FX, asset->projExplosionEffect);
		reader.read_asset(ASSET_TYPE_FX, asset->projDudEffect);
		read_sound(reader, asset->projExplosionSound);
		read_sound(reader, asset->projDudSound);
		reader.read_asset(ASSET_TYPE_FX, asset->projTrailEffect);
		reader.read_asset(ASSET_TYPE_FX, asset->projIgnitionEffect);
		read_sound(reader, asset->projIgnitionSound);
		reader.read_string(asset->aiVsAiAccuracyGraphName);
		reader.read_array(asset->aiVsAiAccuracyGraphKnots, 3, asset->aiVsAiAccuracyGraphKnotCount);
		reader.read_array(asset->originalAiVsAiAccuracyGraphKnots, 3, asset->aiVsAiAccuracyGraphKnotCount);
		reader.read_string(asset->aiVsPlayerAccuracyGraphName);
		reader.read_array(asset->aiVsPlayerAccuracyGraphKnots, 3, asset->aiVsPlayerAccuracyGraphKnotCount);
		reader.read_array(asset->originalAiVsPlayerAccuracyGraphKnots, 3, asset->aiVsPlayerAccuracyGraphKnotCount);
		reader.read_string(asset->szUseHintString);
		reader.read_string(asset->dropHintString);
		reader.read_string(asset->szScript);
		reader.read_string(asset->fireRumble);
		reader.read_string(asset->meleeImpactRumble);
		reader.pop_stream();
	}
}
