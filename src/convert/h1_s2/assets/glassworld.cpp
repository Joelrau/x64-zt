#include <std_include.hpp>

#include "../include.hpp"
#include "glassworld.hpp"
#include "techset.hpp"

namespace zonetool::h1::converter::s2::glassworld
{
	namespace target = zonetool::s2;

	void dump(GlassWorld* asset)
	{
		target::G_GlassData data{};
		const auto donor = static_cast<target::GlassWorld*>(techset::donor_zone().find_asset(target::ASSET_TYPE_GLASSWORLD));
		if (donor && donor->g_glassData)
		{
			data = *donor->g_glassData;
		}

		if (asset->g_glassData && asset->g_glassData->glassNameCount)
		{
			ZONETOOL_WARNING("GlassWorld \"%s\": %u glass names are not converted", asset->name, asset->g_glassData->glassNameCount);
		}

		data.glassPieces = nullptr;
		data.pieceCount = 0;
		data.glassNames = nullptr;
		data.glassNameCount = 0;

		target::GlassWorld new_asset{};
		new_asset.name = asset->name;
		new_asset.g_glassData = &data;
		target::glass_world::dump(&new_asset);
		add_csv_row("glass_map", new_asset.name);
	}
}
