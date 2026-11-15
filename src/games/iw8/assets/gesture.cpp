#include <std_include.hpp>
#include "gesture.hpp"

namespace zonetool::iw8
{
	void gesture::read(zone_reader& reader, Gesture* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset_array(ASSET_TYPE_XANIMPARTS, asset->anims, 22);
		reader.read_script_string(asset->ikTargetEntityBoneName);
		reader.pop_stream();
	}
}
