#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace scriptfile
		{
			zonetool::h1::ScriptFile* convert(ScriptFile* asset, ZoneMemory* mem);
			void dump(ScriptFile* asset, ZoneMemory* mem);
		}
	}
}