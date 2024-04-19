#pragma once

namespace zonetool::h1
{
	namespace converter::s1
	{
		namespace mapents
		{
			zonetool::s1::MapEnts* convert(MapEnts* asset, utils::memory::allocator& allocator);
			void dump(MapEnts* asset);
		}
	}
}
