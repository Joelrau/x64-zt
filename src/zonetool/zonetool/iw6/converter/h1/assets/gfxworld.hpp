#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace gfxworld
		{
			zonetool::h1::GfxWorld* convert(GfxWorld* asset, ZoneMemory* mem);
			void dump(GfxWorld* asset, ZoneMemory* mem);
		}
	}
}