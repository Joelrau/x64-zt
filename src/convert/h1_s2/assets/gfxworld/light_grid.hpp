#pragma once

namespace zonetool::h1::converter::s2::gfxworld
{
	void convert_light_grid(zonetool::s2::GfxWorld& world, utils::memory::allocator& allocator);
	void convert_light_grid_probes(GfxWorld* asset, zonetool::s2::GfxWorld& world, utils::memory::allocator& allocator);
}
