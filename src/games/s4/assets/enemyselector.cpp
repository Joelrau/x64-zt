#include <std_include.hpp>
#include "enemyselector.hpp"

namespace zonetool::s4
{
	void enemy_selector::read(zone_reader& reader, EnemySelector* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_script_string(asset->scrName);
		reader.pop_stream();
	}
}
