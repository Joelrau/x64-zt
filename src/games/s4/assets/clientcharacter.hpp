#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class client_character
	{
	public:
		static void read(zone_reader& reader, ClientCharacter* asset);
	};
}
