#include <std_include.hpp>
#include "gameprops.hpp"

namespace zonetool::s4
{
	void game_props::read(zone_reader& reader, GameProps* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		reader.read_asset(ASSET_TYPE_VFX, asset->data.vfx0);
		reader.read_asset(ASSET_TYPE_VFX, asset->data.vfx1);
		reader.read_string(asset->data.string);
		reader.pop_stream();
	}
}
