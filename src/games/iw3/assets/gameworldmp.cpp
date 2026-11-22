#include <std_include.hpp>
#include "gameworldmp.hpp"

namespace zonetool::iw3
{
	void game_world_mp::read(zone_reader32& reader, GameWorldMp* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}
}
