#include <std_include.hpp>
#include "menulist.hpp"

namespace zonetool::iw5
{
	void menu_list::read(zone_reader32& reader, MenuList* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset_array(ASSET_TYPE_MENU, asset->menus, asset->menuCount);
		reader.pop_stream();
	}
}
