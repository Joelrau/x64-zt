#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace comworld
		{
			zonetool::h1::ComWorld* convert(ComWorld* asset, zone_memory* mem);
			void dump(ComWorld* asset, zone_memory* mem);
		}
	}
}