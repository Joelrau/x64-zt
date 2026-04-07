#pragma once

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace weapondef
		{
			zonetool::h2::WeaponDef* convert(zonetool::h1::WeaponDef* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h1::WeaponDef* asset);
		}
	}
}