#pragma once

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace laserdef
		{
			zonetool::h1::LaserDef* convert(zonetool::h2::LaserDef* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h2::LaserDef* asset);
		}
	}
}
