#include <std_include.hpp>

#include "../include.hpp"
#include "stringtable.hpp"
#include "common/stock_zone.hpp"
#include "stringtable/lightsets.hpp"
#include "stringtable/soundtables.hpp"

namespace zonetool::h1::converter::s2::stringtable
{
	void dump(StringTable* asset)
	{
		const std::string_view name = asset->name;
		if (name.ends_with("_lightsets_hdr.csv"))
		{
			lightsets::dump(asset);
		}
		else if (name.starts_with("soundtables/"))
		{
			soundtables::dump(asset);
		}
		else if (!stock_zone::is_stock_asset(zonetool::s2::ASSET_TYPE_STRINGTABLE, asset->name))
		{
			zonetool::s2::string_table::dump(asset);
			add_csv_row("stringtable", asset->name);
		}
	}
}
