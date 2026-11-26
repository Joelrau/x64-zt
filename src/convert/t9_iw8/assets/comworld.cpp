#include <std_include.hpp>

#include "../include.hpp"
#include "comworld.hpp"
#include "lights.hpp"

#include <iw8/assets/comworld.hpp>

namespace zonetool::t9::converter::iw8::comworld
{
	namespace
	{
		namespace target = zonetool::iw8;
	}

	void dump(const Districts* asset, const std::string& map_name)
	{
		utils::memory::allocator allocator;

		const auto world_name = std::format("maps/mp/{}.d3dbsp", map_name);
		auto* world = allocator.allocate<target::ComWorld>();
		world->name = allocator.duplicate_string(world_name);
		world->isInUse = 1;
		world->useForwardPlus = 1;
		world->bakeQuality = 3;
		world->trVisRadii[0] = 4500.0f;
		world->trVisRadii[1] = 25000.0f;
		world->trVisRadii[2] = 30000.0f;
		world->trVisRadii[3] = 40000.0f;
		world->trVisRadii[4] = 50000.0f;
		world->trVisFacingDistAdd = 1500.0f;

		const auto primary_lights = lights::convert_primary_lights(asset);
		lights::log_lossy_fields(asset);
		world->primaryLightCount = static_cast<unsigned int>(primary_lights.size());
		world->primaryLights = allocator.allocate_array<target::ComPrimaryLight>(primary_lights.size());
		std::memcpy(world->primaryLights, primary_lights.data(), primary_lights.size() * sizeof(target::ComPrimaryLight));
		world->firstScriptablePrimaryLight = world->primaryLightCount;

		world->transientTableSize = 1;
		world->transientTable = allocator.allocate<unsigned short>();
		*world->transientTable = 0x8000;

		std::memset(world->umbraGateInitialStates, 0xFF, sizeof(world->umbraGateInitialStates));

		target::com_world::dump(world);
		add_csv_row("com_map", world_name);
	}
}
