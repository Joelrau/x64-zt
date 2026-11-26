#include <std_include.hpp>
#include "clientcharacter.hpp"

namespace zonetool::s4
{
	void client_character::read(zone_reader& reader, ClientCharacter* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_SCRIPTABLE, asset->scriptableDef);
		reader.read_string(asset->animFileName);
		reader.read_string(asset->animTreeName);
		reader.pop_stream();
	}
}
