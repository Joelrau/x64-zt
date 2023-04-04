#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace scriptabledef
		{
			zonetool::h1::ScriptableDef* convert(ScriptableDef* asset, zone_memory* mem);
			void dump(ScriptableDef* asset, zone_memory* mem);
		}
	}
}