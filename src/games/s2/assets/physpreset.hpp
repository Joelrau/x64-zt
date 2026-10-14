#pragma once

namespace zonetool::s2
{
	class phys_preset
	{
	public:
		static void read(zone_reader& reader, PhysPreset* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_string(asset->sndAliasPrefix);
			for (auto& sound : asset->sounds)
			{
				reader.read_string(sound.name);
			}
			reader.read_asset(ASSET_TYPE_SNDPHYSPRESET, asset->sndPhysPreset);
			reader.pop_stream();
		}
	};

	class phys_water_preset
	{
	public:
		static void read(zone_reader& reader, PhysWaterPreset* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			for (auto& fx : asset->fx)
			{
				reader.read_asset(ASSET_TYPE_FX, fx);
			}
			reader.pop_stream();
		}
	};

	class phys_cloth_tuning
	{
	public:
		static void read(zone_reader& reader, PhysClothTuning* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_single(asset->data, 15);
			reader.pop_stream();
		}
	};
}