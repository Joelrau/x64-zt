#include <std_include.hpp>

#include "../include.hpp"
#include "glassworld.hpp"

#include <iw7/assets/glassworld.hpp>

namespace zonetool::iw8::converter::iw7::glassworld
{
	void dump(GlassWorld* asset)
	{
		zonetool::iw7::G_GlassData data{};
		if (asset->g_glassData)
		{
			data.damageToWeaken = asset->g_glassData->damageToWeaken;
			data.damageToDestroy = asset->g_glassData->damageToDestroy;
		}

		zonetool::iw7::GlassWorld world{};
		world.name = asset->name;
		world.g_glassData = &data; // TODO: glass pieces are not converted, fx_map has no glass
		zonetool::iw7::glass_world::dump(&world);
		add_csv_row("glass_map", asset->name);
	}
}
