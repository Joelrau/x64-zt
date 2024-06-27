#pragma once

namespace zonetool::h1
{
	namespace converter::s1
	{
		namespace material
		{
			zonetool::s1::Material* convert(Material* asset, utils::memory::allocator& allocator);
			void dump(Material* asset);
		}
	}
}
