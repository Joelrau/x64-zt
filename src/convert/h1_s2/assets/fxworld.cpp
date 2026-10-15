#include <std_include.hpp>

#include "../include.hpp"
#include "fxworld.hpp"
#include "techset.hpp"

namespace zonetool::h1::converter::s2::fxworld
{
	namespace target = zonetool::s2;

	// TODO: glass is not converted, the donor world keeps the S2 runtime glass limits
	void dump(FxWorld* asset)
	{
		target::FxWorld new_asset{};
		if (const auto donor = static_cast<target::FxWorld*>(techset::donor_zone().find_asset(target::ASSET_TYPE_FXWORLD)))
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

		target::fx_world::dump(&new_asset);
		add_csv_row("fx_map", new_asset.name);
	}
}
