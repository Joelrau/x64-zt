#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class scriptfile
	{
	public:
		static void read(zone_reader32& reader, ScriptFile* asset);
	};
}
