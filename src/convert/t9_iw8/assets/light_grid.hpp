#pragma once

#include "gfxworld.hpp"

namespace zonetool::t9::converter::iw8::light_grid
{
	using sh_probe = std::array<float, 27>;

	struct probe_sample_counts
	{
		unsigned int interpolated;
		unsigned int nearest;
		unsigned int missing;
	};

	struct light_grid_volume
	{
		float box_min[3];
		float box_size[3];
		unsigned int dimensions[3];
		unsigned int start[3];
	};

	const Lighting* find_lighting(const GfxWorld* world);
	void build_static_model_volumes(const Lighting* lighting, const std::vector<gfxworld::placed_model>& models,
		zonetool::iw8::GfxWorldTransientZone* zone, utils::memory::allocator& allocator);
	std::optional<sh_probe> build_probe_grid(const Lighting* lighting, const zonetool::iw8::Bounds& world_bounds, zonetool::iw8::GfxWorld* world,
		zonetool::iw8::GfxWorldTransientZone* zone, utils::memory::allocator& allocator);
}
