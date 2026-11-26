#include <std_include.hpp>

#include "../include.hpp"
#include "glassworld.hpp"

#include <iw8/assets/glassworld.hpp>

namespace zonetool::t9::converter::iw8::glassworld
{
	void dump(const std::string& map_name)
	{
		utils::memory::allocator allocator;

		const auto world_name = std::format("maps/mp/{}.d3dbsp", map_name);
		auto* data = allocator.allocate<zonetool::iw8::G_GlassData>();
		data->spatialTree = allocator.allocate<zonetool::iw8::SpatialPartition_Tree>();

		zonetool::iw8::GlassWorld world{};
		world.name = allocator.duplicate_string(world_name);
		world.g_glassData = data;

		zonetool::iw8::glass_world::dump(&world);
		add_csv_row("glass_map", world_name);
	}
}
