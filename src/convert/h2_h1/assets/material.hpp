#pragma once

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace material
		{
			zonetool::h1::Material* convert(zonetool::h2::Material* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h2::Material* asset);
		}
	}
}
