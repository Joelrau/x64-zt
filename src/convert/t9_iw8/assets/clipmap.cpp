#include <std_include.hpp>

#include "../include.hpp"
#include "clipmap.hpp"

#include <iw8/assets/clipmap.hpp>

namespace zonetool::t9::converter::iw8::clipmap
{
	namespace
	{
		namespace target = zonetool::iw8;

		target::Stage* make_stage(utils::memory::allocator& allocator)
		{
			auto* stage = allocator.allocate<target::Stage>();
			stage->name = allocator.duplicate_string("stage 0");
			stage->triggerIndex = 0x400;
			stage->sunPrimaryLightIndex = 1;
			return stage;
		}
	}

	void dump(const std::string& map_name, const zonetool::iw8::Bounds& world_bounds, std::string& world_shape)
	{
		utils::memory::allocator allocator;

		const auto world_name = std::format("maps/mp/{}.d3dbsp", map_name);
		auto* clip = allocator.allocate<target::clipMap_t>();
		clip->name = allocator.duplicate_string(world_name);
		clip->mapEnts = allocator.allocate<target::MapEnts>();
		clip->mapEnts->name = clip->name;

		clip->stageCount = 1;
		clip->stages = make_stage(allocator);

		for (auto axis = 0; axis < 3; axis++)
		{
			clip->broadphaseMin[axis] = world_bounds.midPoint[axis] - world_bounds.halfSize[axis];
			clip->broadphaseMax[axis] = world_bounds.midPoint[axis] + world_bounds.halfSize[axis];
		}

		clip->havokWorldShapeDataSize = static_cast<unsigned int>(world_shape.size());
		clip->havokWorldShapeData = world_shape.data();

		target::clip_map::dump(clip);
		add_csv_row("col_map", world_name);
	}
}
