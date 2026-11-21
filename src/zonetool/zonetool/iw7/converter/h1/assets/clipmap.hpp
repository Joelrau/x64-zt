#pragma once

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace clipmap
		{
			// IW7 clipmaps are Havok based; H1 collision is synthesized from the render geometry.
			zonetool::h1::clipMap_t* generate_from_gfxworld(GfxWorld* asset, utils::memory::allocator& allocator);
			void dump_from_gfxworld(GfxWorld* asset);
		}
	}
}
