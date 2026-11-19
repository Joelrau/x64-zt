#include <std_include.hpp>
#include "carryobject.hpp"

namespace zonetool::iw8
{
	void carry_object::read(zone_reader& reader, CarryObjectDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->carryViewModel);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->carryWorldModel);
		reader.read_asset(ASSET_TYPE_GESTURE, asset->gesture);
		reader.read_script_string(asset->attachTag);
		reader.pop_stream();
	}
}
