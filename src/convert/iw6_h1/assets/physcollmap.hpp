#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace physcollmap
		{
			zonetool::h1::PhysCollmap* convert(PhysCollmap* asset, utils::memory::allocator& allocator);
			void dump(PhysCollmap* asset);
		}
	}
}