#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace xmodel
		{
			zonetool::h1::XModel* convert(XModel* asset, zone_memory* mem);
			void dump(XModel* asset, zone_memory* mem);
		}
	}
}