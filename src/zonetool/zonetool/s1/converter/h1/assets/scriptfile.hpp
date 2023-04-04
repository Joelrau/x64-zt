#pragma once

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace scriptfile
		{
			zonetool::h1::ScriptFile* convert(ScriptFile* asset, zone_memory* mem);
			void dump(ScriptFile* asset, zone_memory* mem);
		}
	}
}