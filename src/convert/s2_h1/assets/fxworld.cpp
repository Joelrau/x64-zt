#include <std_include.hpp>

#include "../include.hpp"
#include "fxworld.hpp"

#include <h1/assets/fxworld.hpp>

namespace zonetool::s2::converter::h1::fxworld
{
	void dump(FxWorld* asset)
	{
		if (asset->glassSys.defCount)
		{
			ZONETOOL_WARNING("fx_map \"%s\": %u glass defs are not converted", asset->name, asset->glassSys.defCount);
		}

		zonetool::h1::FxWorld new_asset{};
		new_asset.name = asset->name;
		new_asset.glassSys.firstFreePiece = 0xFFFF;
		zonetool::h1::fx_world::dump(&new_asset);
		add_csv_row("fx_map", new_asset.name);
	}
}
