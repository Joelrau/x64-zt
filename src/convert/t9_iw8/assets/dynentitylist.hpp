#pragma once

namespace zonetool::t9::converter::iw8::dynentitylist
{
	unsigned int dump(const ClipMap* asset);
	void fill_map_ents(zonetool::iw8::MapEnts* map_ents, unsigned int count, const zonetool::iw8::Bounds& world_bounds,
		utils::memory::allocator& allocator);
	void fill_world(zonetool::iw8::GfxWorld* world, utils::memory::allocator& allocator);
}
