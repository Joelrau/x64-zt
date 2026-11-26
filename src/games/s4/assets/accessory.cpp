#include <std_include.hpp>
#include "accessory.hpp"

namespace zonetool::s4
{
	void accessory_def::read(zone_reader& reader, AccessoryDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_script_string(asset->VMAttachBone);
		reader.read_asset(ASSET_TYPE_GESTURE, asset->interactGesture);
		reader.read_string(asset->widgetVM);
		reader.pop_stream();
	}
}
