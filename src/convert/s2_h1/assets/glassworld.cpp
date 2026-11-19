#include <std_include.hpp>

#include "../include.hpp"
#include "glassworld.hpp"

#include <h1/assets/glassworld.hpp>

namespace zonetool::s2::converter::h1::glassworld
{
	void dump(GlassWorld* asset)
	{
		if (asset->g_glassData && asset->g_glassData->pieceCount)
		{
			ZONETOOL_WARNING("glass_map \"%s\": %u glass pieces are not converted", asset->name, asset->g_glassData->pieceCount);
		}

		zonetool::h1::G_GlassData glass_data{};
		zonetool::h1::GlassWorld new_asset{};
		new_asset.name = asset->name;
		new_asset.g_glassData = &glass_data;
		zonetool::h1::glass_world::dump(&new_asset);
		add_csv_row("glass_map", new_asset.name);
	}
}
