#pragma once

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace material
		{
			zonetool::h2::Material* convert(zonetool::h1::Material* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h1::Material* asset);
		}
	}
}
