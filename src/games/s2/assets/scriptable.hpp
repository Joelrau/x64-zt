#pragma once

namespace zonetool::s2
{
	class scriptable_def
	{
	public:
		static void read(zone_reader& reader, ScriptableDef* asset);
	};
}
