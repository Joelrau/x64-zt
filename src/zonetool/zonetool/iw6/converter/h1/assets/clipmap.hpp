#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace clipmap
		{
			zonetool::h1::clipMap_t* convert(clipMap_t* asset, ZoneMemory* mem);
			void dump(clipMap_t* asset, ZoneMemory* mem);
		}
	}
}