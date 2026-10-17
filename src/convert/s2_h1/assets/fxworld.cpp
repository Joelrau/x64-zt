#include <std_include.hpp>

#include "../include.hpp"
#include "fxworld.hpp"

#include <h1/assets/fxworld.hpp>

namespace zonetool::s2::converter::h1::fxworld
{
	namespace
	{
		namespace target = zonetool::h1;

		constexpr auto no_free_piece = 0xFFFFu;
	}

	void dump(FxWorld* asset)
	{
		if (asset->glassSys.defCount)
		{
			ZONETOOL_WARNING("fx_map \"%s\": %u glass defs are not converted", asset->name, asset->glassSys.defCount);
		}

		target::FxWorld new_asset{};
		new_asset.name = asset->name;
		new_asset.glassSys.firstFreePiece = no_free_piece;
		target::fx_world::dump(&new_asset);
		add_csv_row("fx_map", new_asset.name);
	}
}
