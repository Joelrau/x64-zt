#include <std_include.hpp>
#include "physicslibrary.hpp"

namespace zonetool::iw8
{
	void physics_library::read(zone_reader& reader, PhysicsLibrary* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->havokData, 15, asset->havokDataSize);
		reader.pop_stream();
	}
}
