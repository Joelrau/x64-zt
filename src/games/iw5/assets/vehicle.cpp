#include <std_include.hpp>
#include "vehicle.hpp"

namespace zonetool::iw5
{
	void vehicle::read(zone_reader32& reader, VehicleDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->useHintString);
		reader.read_string(asset->vehPhysDef.physPresetName);
		reader.read_asset(ASSET_TYPE_PHYSPRESET, asset->vehPhysDef.physPreset);
		reader.read_string(asset->vehPhysDef.accelGraphName);
		reader.read_string(asset->turretWeaponName);
		reader.read_asset(ASSET_TYPE_WEAPON, asset->turretWeapon);
		sound::read_custom(reader, asset->turretSpinSnd);
		sound::read_custom(reader, asset->turretStopSnd);
		reader.read_asset(ASSET_TYPE_FX, asset->trophyExplodeFx);
		reader.read_asset(ASSET_TYPE_FX, asset->trophyFlashFx);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassFriendlyIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassEnemyIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassFriendlyAltIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassEnemyAltIcon);

		for (const auto alias : {
			&asset->idleLowSnd, &asset->idleHighSnd, &asset->engineLowSnd, &asset->engineHighSnd,
			&asset->idleLowSndAlt, &asset->idleHighSndAlt, &asset->engineLowSndAlt, &asset->engineHighSndAlt,
			&asset->turretSpinSndAlt, &asset->turretStopSndAlt, &asset->engineStartUpSnd, &asset->engineShutdownSnd,
			&asset->engineIdleSnd, &asset->engineSustainSnd, &asset->engineRampUpSnd, &asset->engineRampDownSnd,
			&asset->suspensionSoftSnd, &asset->suspensionHardSnd, &asset->collisionSnd, &asset->speedSnd,
		})
		{
			sound::read_custom(reader, *alias);
		}

		reader.read_string(asset->surfaceSndPrefix);
		for (auto& alias : asset->surfaceSnds)
		{
			sound::read_custom(reader, alias);
		}

		reader.read_script_strings(asset->trophyTags, 4);
		reader.read_script_string(asset->audioOriginTag);
		reader.read_script_string(asset->audioOriginTagAlt);
		reader.pop_stream();
	}
}
