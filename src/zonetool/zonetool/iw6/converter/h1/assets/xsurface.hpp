#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace xsurface
		{
			zonetool::h1::XModelSurfs* convert(XModelSurfs* asset, zone_memory* mem);
			void dump(XModelSurfs* asset, zone_memory* mem);
		}
	}
}