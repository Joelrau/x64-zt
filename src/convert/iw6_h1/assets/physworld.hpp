#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace physworld
		{
			zonetool::h1::PhysWorld* generate(zonetool::h1::clipMap_t* asset, utils::memory::allocator& allocator);
		}
	}
}