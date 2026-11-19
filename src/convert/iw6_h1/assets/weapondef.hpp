#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace weapondef
		{
			zonetool::h1::WeaponDef* convert(WeaponCompleteDef* asset, utils::memory::allocator& allocator);
			void dump(WeaponCompleteDef* asset);
		}
	}
}