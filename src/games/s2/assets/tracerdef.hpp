#pragma once

namespace zonetool::s2
{
	class tracer_def
	{
	public:
		static void read(zone_reader& reader, TracerDef* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->material);
			reader.read_asset(ASSET_TYPE_FX, asset->effect);
			reader.pop_stream();
		}
	};
}
