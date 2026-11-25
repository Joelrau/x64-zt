#include <std_include.hpp>
#include "tracer.hpp"

namespace zonetool::iw5
{
	void tracer::read(zone_reader32& reader, TracerDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->material);
		reader.pop_stream();
	}
}
