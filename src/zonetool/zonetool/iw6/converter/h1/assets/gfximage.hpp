#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace gfximage
		{
			zonetool::h1::GfxImage* convert(GfxImage* asset, ZoneMemory* mem);
			void dump(GfxImage* asset, ZoneMemory* mem);
		}
	}
}