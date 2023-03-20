#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace fxworld
		{
			zonetool::h1::FxWorld* convert(FxWorld* asset, ZoneMemory* mem);
			void dump(FxWorld* asset, ZoneMemory* mem);
		}
	}
}