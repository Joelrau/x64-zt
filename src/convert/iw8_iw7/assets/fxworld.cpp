#include <std_include.hpp>

#include "../include.hpp"
#include "fxworld.hpp"

#include <iw7/assets/fxworld.hpp>

namespace zonetool::iw8::converter::iw7::fxworld
{
	void dump(FxWorld* asset)
	{
		zonetool::iw7::FxWorld world{};
		world.name = asset->name;
		zonetool::iw7::fx_world::dump(&world); // TODO: glass defs and pieces are not converted
		add_csv_row("fx_map", asset->name);
	}
}
