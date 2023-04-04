#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace fxworld
		{
			zonetool::h1::FxWorld* convert(FxWorld* asset, zone_memory* mem);
			void dump(FxWorld* asset, zone_memory* mem);
		}
	}
}