#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace impactfx
		{
			zonetool::h1::FxImpactTable* convert(FxImpactTable* asset, utils::memory::allocator& allocator);
			void dump(FxImpactTable* asset);
		}
	}
}
