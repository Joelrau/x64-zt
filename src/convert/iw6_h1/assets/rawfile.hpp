#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace rawfile
		{
			std::array<float, 4> get_sky_intensity();
			zonetool::h1::RawFile* convert(RawFile* asset, utils::memory::allocator& allocator);
			void dump(RawFile* asset);
		}
	}
}