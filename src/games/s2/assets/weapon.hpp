#pragma once

namespace zonetool::s2
{
	class weapon_def
	{
	public:
		static void read(zone_reader& reader, WeaponCompleteDef* asset);
	};

	class laser_def
	{
	public:
		static void read(zone_reader& reader, LaserDef* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->laserMaterial);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->laserLightMaterial);
			reader.read_asset(ASSET_TYPE_FX, asset->effect);
			reader.read_asset(ASSET_TYPE_LASER, asset->altLaser);
			reader.read_script_string(asset->ownerTag);
			reader.pop_stream();
		}
	};

	class beam_def
	{
	public:
		static void read(zone_reader& reader, BeamDef* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->beamMaterial);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->beamMaterial2);
			for (auto& effect : asset->effects)
			{
				reader.read_asset(ASSET_TYPE_FX, effect);
			}
			for (auto& model : asset->models)
			{
				reader.read_asset(ASSET_TYPE_XMODEL, model);
			}
			reader.read_string(asset->string664);
			reader.read_string(asset->string672);
			for (auto& sound : asset->sounds)
			{
				reader.read_name_reference(sound);
			}
			for (auto& sound : asset->sounds2)
			{
				reader.read_name_reference(sound);
			}
			reader.pop_stream();
		}
	};
}
