#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace scriptabledef
		{
			zonetool::h1::ScriptableDef* convert(ScriptableDef* asset, ZoneMemory* mem);
			void dump(ScriptableDef* asset, ZoneMemory* mem);
		}
	}
}