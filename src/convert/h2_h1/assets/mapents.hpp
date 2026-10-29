#pragma once

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace mapents
		{
			zonetool::h1::MapEnts* convert(zonetool::h2::MapEnts* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h2::MapEnts* asset);
		}
	}
}
