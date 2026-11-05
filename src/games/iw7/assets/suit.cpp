#include <std_include.hpp>
#include "suit.hpp"

namespace zonetool::iw7
{
	void suit::read(zone_reader& reader, SuitDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->doubleJump_sound);
		reader.read_string(asset->doubleJump_soundPlayer);
		reader.read_string(asset->doubleJump_releaseSound);
		reader.read_string(asset->doubleJump_releaseSoundPlayer);
		reader.read_asset(ASSET_TYPE_SUITANIMPACKAGE, asset->animPackage);
		reader.read_asset(ASSET_TYPE_SUITANIMPACKAGE, asset->animPackageL);
		reader.read_asset(ASSET_TYPE_SUITANIMPACKAGE, asset->animPackageR);
		reader.read_asset(ASSET_TYPE_SUITANIMPACKAGE, asset->animPackageRelaxed);
		reader.read_asset(ASSET_TYPE_SUITANIMPACKAGE, asset->animPackageSafe);
		reader.read_asset(ASSET_TYPE_SUITANIMPACKAGE, asset->animPackageUnknown);
		reader.read_asset(ASSET_TYPE_SCRIPTABLE, asset->scriptableDef);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->groundPound_activationRumble);
		reader.read_string(asset->groundPound_activationSound);
		reader.read_string(asset->groundPound_activationSoundPlayer);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->groundPound_landingRumble);
		reader.read_string(asset->groundPound_landingSound);
		reader.read_string(asset->groundPound_landingSoundPlayer);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->landing_rumbleLowHeight);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->landing_rumbleMediumHeight);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->landing_rumbleHighHeight);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->landing_rumbleExtremeHeight);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->footstep_rumble);
		reader.pop_stream();
	}
}
