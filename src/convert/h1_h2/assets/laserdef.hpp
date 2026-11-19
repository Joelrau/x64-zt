#pragma once

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace laserdef
		{
			zonetool::h2::LaserDef* convert(zonetool::h1::LaserDef* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h1::LaserDef* asset);
		}
	}
}
