#pragma once

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace material
		{
			zonetool::h1::Material* convert(Material* asset, utils::memory::allocator& allocator);
			void dump(Material* asset);
		}
	}
}
