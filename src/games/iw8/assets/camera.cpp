#include <std_include.hpp>
#include "camera.hpp"

namespace zonetool::iw8
{
	void camera_def::read(zone_reader& reader, CameraDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}
}
