#pragma once

namespace zonetool::h1
{
	namespace converter::s1
	{
		namespace weapondef
		{
			zonetool::s1::WeaponDef* convert(WeaponDef* asset, utils::memory::allocator& allocator);
			void dump(WeaponDef* asset);
		}
	}
}
