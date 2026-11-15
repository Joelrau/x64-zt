#include <std_include.hpp>
#include "tracer.hpp"

namespace zonetool::iw8
{
	void tracer_def::read(zone_reader& reader, TracerDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_VFX, asset->effect.particleSystemDef);
		reader.read_asset(ASSET_TYPE_VFX, asset->viewmodelEffect.particleSystemDef);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->material);
		reader.pop_stream();
	}
}
