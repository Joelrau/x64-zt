#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace physpreset
		{
			zonetool::h1::PhysPreset* convert(PhysPreset* asset, utils::memory::allocator& allocator);
			void dump(PhysPreset* asset);
		}
	}
}