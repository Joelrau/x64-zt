#include <std_include.hpp>
#include "animpackage.hpp"

namespace zonetool::iw8
{
	void weapon_anim_package::read(zone_reader& reader, WeaponAnimPackage* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset_array(ASSET_TYPE_XANIMPARTS, asset->anims, 614);
		reader.read_single(asset->timers, 1);
		reader.read_asset_array(ASSET_TYPE_BLEND_SPACE_2D, asset->blendSpaces, 8);
		reader.read_array(asset->footstep, 3, 13);
		reader.pop_stream();
	}
}
