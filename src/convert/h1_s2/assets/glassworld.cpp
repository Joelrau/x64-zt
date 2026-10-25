#include <std_include.hpp>

#include "../include.hpp"
#include "glassworld.hpp"

#include <bit>

namespace zonetool::h1::converter::s2::glassworld
{
	void dump(GlassWorld* asset)
	{
		// same layout, pieceCount = fx_map initPieceCount in both games
		zonetool::s2::G_GlassData data{};
		if (asset->g_glassData)
		{
			data = std::bit_cast<zonetool::s2::G_GlassData>(*asset->g_glassData);
		}

		zonetool::s2::GlassWorld new_asset{};
		new_asset.name = asset->name;
		new_asset.g_glassData = &data;
		zonetool::s2::glass_world::dump(&new_asset);
		add_csv_row("glass_map", new_asset.name);
	}
}
