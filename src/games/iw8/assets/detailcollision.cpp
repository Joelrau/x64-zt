#include <std_include.hpp>
#include "detailcollision.hpp"

namespace zonetool::iw8
{
	void xmodel_detail_collision::read(zone_reader& reader, XModelDetailCollision* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->physicsLODData, 15, asset->physicsLODDataSize);
		reader.read_script_string_array(asset->physicsLODDataNames, asset->physicsLODDataNameCount);
		reader.pop_stream();
	}
}
