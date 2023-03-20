#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace mapents
		{
			zonetool::h1::MapEnts* convert(MapEnts* asset, ZoneMemory* mem);
			void dump(MapEnts* asset, ZoneMemory* mem);
		}
	}
}