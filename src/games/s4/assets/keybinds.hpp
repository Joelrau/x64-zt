#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class key_binds
	{
	public:
		static void read(zone_reader& reader, KeyBinds* asset);
	};
}
