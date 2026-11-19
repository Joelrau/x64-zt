#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace weaponattachment
		{
			zonetool::h1::WeaponAttachment* convert(WeaponAttachment* asset, utils::memory::allocator& allocator);
			void dump(WeaponAttachment* asset);
		}
	}
}