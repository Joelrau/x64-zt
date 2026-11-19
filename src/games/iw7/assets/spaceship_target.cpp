#include <std_include.hpp>
#include "spaceship_target.hpp"

namespace zonetool::iw7
{
	void spaceship_target::read(zone_reader& reader, SpaceshipTargetDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_script_string(asset->targetTag);
		reader.pop_stream();
	}
}
