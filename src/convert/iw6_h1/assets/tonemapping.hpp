#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace tonemapping
		{
			zonetool::h1::StringTable* convert(ToneMapping* asset, utils::memory::allocator& allocator);
			void dump(ToneMapping* asset);
		}
	}
}