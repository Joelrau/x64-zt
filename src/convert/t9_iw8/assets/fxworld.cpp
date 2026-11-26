#include <std_include.hpp>

#include "../include.hpp"
#include "fxworld.hpp"

#include <iw8/assets/fxworld.hpp>

namespace zonetool::t9::converter::iw8::fxworld
{
	void dump(const std::string& map_name)
	{
		utils::memory::allocator allocator;

		const auto world_name = std::format("maps/mp/{}.d3dbsp", map_name);
		auto* world = allocator.allocate<zonetool::iw8::FxWorld>();
		world->name = allocator.duplicate_string(world_name); // TODO: no glass, T9 glasses are not converted

		zonetool::iw8::fx_world::dump(world);
		add_csv_row("fx_map", world_name);
	}
}
