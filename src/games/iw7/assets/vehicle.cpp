#include <std_include.hpp>
#include "vehicle.hpp"

namespace zonetool::iw7
{
	void vehicle::read(zone_reader& reader, VehicleDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->useHintString);
		read_fx_combined(reader, asset->vehHelicopterGroundFx);
		read_fx_combined(reader, asset->vehHelicopterGroundWaterFx);

		reader.read_script_string(asset->ssAnimTree);
		for (auto& parts : asset->ssAnimParts)
		{
			for (auto& anim : parts.anim)
			{
				reader.read_asset(ASSET_TYPE_XANIMPARTS, anim);
			}
		}

		read_fx_combined(reader, asset->ssThrustFxLoop);
		read_fx_combined(reader, asset->ssJukeFx);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->ssIdleRumble);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->ssSmallRumble);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->ssMedRumble);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->ssLargeRumble);

		reader.read_string(asset->rattleLoop);
		reader.read_string(asset->airLoop);
		reader.read_string(asset->engineLoop);
		reader.read_string(asset->hoverLoop);
		reader.read_string(asset->boostLoop);
		reader.read_string(asset->ssThrustLoop);
		reader.read_string(asset->boostStart);
		reader.read_string(asset->boostStop);
		reader.read_string(asset->boostDepleted);
		reader.read_string(asset->boostUnavailable);
		reader.read_string(asset->jukeLeft);
		reader.read_string(asset->jukeRight);
		reader.read_string(asset->jukeUpDown);
		reader.read_string(asset->jukeBack);
		reader.read_string(asset->jukeFront);
		reader.read_string(asset->flightOn);
		reader.read_string(asset->flightOff);
		reader.read_string(asset->hardCollision);
		reader.read_string(asset->softCollision);

		reader.read_string(asset->turretWeaponName);
		reader.read_asset(ASSET_TYPE_WEAPON, asset->turretWeapon);
		reader.read_string(asset->turretSpinSnd);
		reader.read_string(asset->turretStopSnd);
		reader.read_script_strings(asset->trophyTags, std::size(asset->trophyTags));
		read_fx_combined(reader, asset->trophyExplodeFx);
		read_fx_combined(reader, asset->trophyFlashFx);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassFriendlyIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassEnemyIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassFriendlyAltIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassEnemyAltIcon);

		reader.read_string(asset->idleLowSnd);
		reader.read_string(asset->idleHighSnd);
		reader.read_string(asset->engineLowSnd);
		reader.read_string(asset->engineHighSnd);
		reader.read_script_string(asset->audioOriginTag);
		reader.read_string(asset->engineStartUpSnd);
		reader.read_string(asset->engineShutdownSnd);
		reader.read_string(asset->engineIdleSnd);
		reader.read_string(asset->engineSustainSnd);
		reader.read_string(asset->engineRampUpSnd);
		reader.read_string(asset->engineRampDownSnd);
		reader.read_string(asset->suspensionSoftSnd);
		reader.read_string(asset->suspensionHardSnd);
		reader.read_string(asset->collisionSnd);
		reader.read_string(asset->speedSnd);
		reader.read_string(asset->surfaceSndName);
		reader.read_string(asset->soundTriggerOverrideZone);
		reader.read_string(asset->globalVisionSettings);
		reader.read_string(asset->mapVisionSettings);
		reader.pop_stream();
	}
}
