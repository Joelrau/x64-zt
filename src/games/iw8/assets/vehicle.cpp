#include <std_include.hpp>
#include "vehicle.hpp"

namespace zonetool::iw8
{
	namespace
	{
		void read_physics(zone_reader& reader, VehiclePhysicsDef& physics)
		{
			reader.read_script_strings(physics.physics_wheelBones, std::size(physics.physics_wheelBones));
			reader.read_string(physics.physics_rcpBoostSound.name);
			reader.read_script_strings(physics.treads_firstBones, std::size(physics.treads_firstBones));

			for (auto group : {&physics.sndImpactHard, &physics.sndImpactLight, &physics.sndDrivingFast, &physics.sndDrivingSlow,
				&physics.sndSuspension, &physics.sndBrakingHard, &physics.sndBrakingLight, &physics.sndDrifting, &physics.sndSkidding,
				&physics.sndDlc1, &physics.sndDlc2, &physics.sndDlc3, &physics.sndDlc4})
			{
				reader.read_string(group->alias.name);
			}

			for (auto group : {&physics.vfxImpactHard, &physics.vfxImpactLight, &physics.vfxDrivingFast, &physics.vfxDrivingSlow,
				&physics.vfxBrakingHard, &physics.vfxBrakingLight, &physics.vfxDrifting, &physics.vfxSkidding,
				&physics.vfxDrivingFlatFast, &physics.vfxDrivingFlatSlow, &physics.vfxDlc3, &physics.vfxDlc4})
			{
				reader.read_asset(ASSET_TYPE_FOOTSTEP_VFX, group->surfaceEffects);
			}
		}
	}

	void vehicle::read(zone_reader& reader, VehicleDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->useHintString);
		reader.read_script_strings(asset->doorBones, std::size(asset->doorBones));
		reader.read_asset(ASSET_TYPE_CAMERA, asset->cameraDef);
		reader.read_asset(ASSET_TYPE_CAMERA, asset->killCamDef);
		read_physics(reader, asset->vehiclePhysicsDef);
		reader.read_asset(ASSET_TYPE_VFX, asset->vehHelicopterGroundFx.particleSystemDef);
		reader.read_asset(ASSET_TYPE_VFX, asset->vehHelicopterGroundWaterFx.particleSystemDef);
		reader.read_script_string(asset->ssAnimTree);

		for (auto& parts : asset->ssAnimParts)
		{
			for (auto& anim : parts.anim)
			{
				reader.read_asset(ASSET_TYPE_XANIMPARTS, anim);
			}
		}

		reader.read_asset(ASSET_TYPE_VFX, asset->ssThrustFxLoop.particleSystemDef);
		reader.read_asset(ASSET_TYPE_VFX, asset->ssJukeFx.particleSystemDef);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->ssIdleRumble);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->ssSmallRumble);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->ssMedRumble);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->ssLargeRumble);
		reader.read_string(asset->rattleLoop.name);
		reader.read_string(asset->airLoop.name);
		reader.read_string(asset->engineLoop.name);
		reader.read_string(asset->hoverLoop.name);
		reader.read_string(asset->boostLoop.name);
		reader.read_string(asset->ssThrustLoop.name);
		reader.read_string(asset->boostStart.name);
		reader.read_string(asset->boostStop.name);
		reader.read_string(asset->boostDepleted.name);
		reader.read_string(asset->boostUnavailable.name);
		reader.read_string(asset->jukeLeft.name);
		reader.read_string(asset->jukeRight.name);
		reader.read_string(asset->jukeUpDown.name);
		reader.read_string(asset->jukeBack.name);
		reader.read_string(asset->jukeFront.name);
		reader.read_string(asset->flightOn.name);
		reader.read_string(asset->flightOff.name);
		reader.read_string(asset->hardCollision.name);
		reader.read_string(asset->softCollision.name);
		reader.read_string(asset->turretWeaponName);
		reader.read_asset(ASSET_TYPE_WEAPON, asset->turretWeapon);
		reader.read_string(asset->turretSpinSnd.name);
		reader.read_string(asset->turretStopSnd.name);
		reader.read_script_strings(asset->trophyTags, std::size(asset->trophyTags));
		reader.read_asset(ASSET_TYPE_VFX, asset->trophyExplodeFx.particleSystemDef);
		reader.read_asset(ASSET_TYPE_VFX, asset->trophyFlashFx.particleSystemDef);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassFriendlyIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassEnemyIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassFriendlyAltIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassEnemyAltIcon);
		reader.read_asset(ASSET_TYPE_SOUND_BANK_TRANSIENT, asset->detailSoundBankNPC);
		reader.read_asset(ASSET_TYPE_SOUND_BANK_TRANSIENT, asset->detailSoundBankPlayer);
		reader.read_string(asset->idleLowSnd.name);
		reader.read_string(asset->idleHighSnd.name);
		reader.read_string(asset->engineLowSnd.name);
		reader.read_string(asset->engineHighSnd.name);
		reader.read_string(asset->fallbackIdleLowSnd.name);
		reader.read_string(asset->fallbackIdleHighSnd.name);
		reader.read_string(asset->fallbackEngineLowSnd.name);
		reader.read_string(asset->fallbackEngineHighSnd.name);
		reader.read_script_string(asset->audioOriginTag);
		reader.read_string(asset->mainRotorLowRpmSnd.name);
		reader.read_string(asset->mainRotorOperatingRpmSnd.name);
		reader.read_string(asset->mainRotorOperatingEffortSnd.name);
		reader.read_string(asset->tailRotorLowRpmSnd.name);
		reader.read_string(asset->tailRotorOperatingRpmSnd.name);
		reader.read_string(asset->tailRotorOperatingEffortSndLeft.name);
		reader.read_string(asset->tailRotorOperatingEffortSndRight.name);
		reader.read_string(asset->physicsHeliStartup.name);
		reader.read_string(asset->physicsHeliShutdown.name);
		reader.read_string(asset->turbineLowRpmSnd.name);
		reader.read_string(asset->turbineOperatingRpmSnd.name);
		reader.read_string(asset->revLowFidelityMod.name);
		reader.read_string(asset->revHighFidelityMod.name);
		reader.read_string(asset->engineStartUpSnd.name);
		reader.read_string(asset->engineShutdownSnd.name);
		reader.read_string(asset->engineIdleSnd.name);
		reader.read_string(asset->engineSustainSnd.name);
		reader.read_string(asset->engineRampUpSnd.name);
		reader.read_string(asset->engineRampDownSnd.name);
		reader.read_string(asset->suspensionSoftSnd.name);
		reader.read_string(asset->suspensionHardSnd.name);
		reader.read_string(asset->collisionSnd.name);
		reader.read_string(asset->speedSnd.name);
		reader.read_string(asset->surfaceSndName);
		reader.read_string(asset->soundTriggerOverrideZone);
		reader.read_string(asset->globalVisionSettings);
		reader.read_string(asset->mapVisionSettings);
		reader.read_string(asset->luiCrosshairWidget);
		reader.pop_stream();
	}
}
