#pragma once

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace aipaths
		{
			zonetool::h1::PathData* convert(PathData* asset, ZoneMemory* mem);
			void dump(PathData* asset, ZoneMemory* mem);
		}
	}
}