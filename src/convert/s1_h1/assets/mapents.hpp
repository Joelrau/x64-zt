#pragma once

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace mapents
		{
			zonetool::h1::MapEnts* convert(MapEnts* asset, utils::memory::allocator& allocator);
			void dump(MapEnts* asset);
		}
	}
}
