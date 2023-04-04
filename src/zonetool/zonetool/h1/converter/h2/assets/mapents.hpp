#pragma once

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace mapents
		{
			zonetool::h2::MapEnts* convert(zonetool::h1::MapEnts* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h1::MapEnts* asset);
		}
	}
}
