#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class native_script_patch
	{
	public:
		static void read(zone_reader& reader, NativeScriptPatch* asset);
	};
}
