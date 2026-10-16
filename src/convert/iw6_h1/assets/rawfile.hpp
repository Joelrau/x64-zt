#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace rawfile
		{
			zonetool::h1::RawFile* convert(RawFile* asset, utils::memory::allocator& allocator);
			void dump(RawFile* asset);
		}
	}
}