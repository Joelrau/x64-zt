#include <std_include.hpp>

#include "../include.hpp"
#include "fxworld.hpp"
#include "techset.hpp"

namespace zonetool::h1::converter::s2::fxworld
{
	void dump(FxWorld* asset)
	{
		// TODO glass is not converted
		zonetool::s2::FxWorld new_asset{};
		if (const auto donor = static_cast<zonetool::s2::FxWorld*>(techset::donor_zone().find_asset(zonetool::s2::ASSET_TYPE_FXWORLD)))
		{
			new_asset = *donor;
		}

		if (asset->glassSys.defCount)
		{
			ZONETOOL_WARNING("FxWorld \"%s\": %u glass defs are not converted", asset->name, asset->glassSys.defCount);
		}

		auto& glass = new_asset.glassSys;
		new_asset.name = asset->name;
		glass.defCount = 0;
		glass.defs = nullptr;
		glass.initGeoDataCount = 0;
		glass.initGeoData = nullptr;
		glass.initPieceCount = 0;
		glass.initPieceStates = nullptr;
		glass.lightingHandles = nullptr;

		zonetool::s2::fx_world::dump(&new_asset);
		add_csv_row("fx_map", new_asset.name);
	}
}
