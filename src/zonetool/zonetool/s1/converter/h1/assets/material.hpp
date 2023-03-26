#pragma once

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace material
		{
			zonetool::h1::Material* convert(Material* asset, ZoneMemory* mem);
			void dump(Material* asset, ZoneMemory* mem);
		}
	}
}