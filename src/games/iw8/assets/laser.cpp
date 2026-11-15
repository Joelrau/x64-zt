#include <std_include.hpp>
#include "laser.hpp"

namespace zonetool::iw8
{
	void laser_def::read(zone_reader& reader, LaserDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->laserMaterial);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->laserLightMaterial);
		reader.read_asset(ASSET_TYPE_VFX, asset->beamEffect.particleSystemDef);
		reader.read_asset(ASSET_TYPE_VFX, asset->laserEndEffect.particleSystemDef);
		reader.pop_stream();
	}
}
