#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace tracer_def
		{
			zonetool::h1::TracerDef* convert(TracerDef* asset, utils::memory::allocator& allocator);
			void dump(TracerDef* asset);
		}
	}
}