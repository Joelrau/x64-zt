#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class client_character
	{
	public:
		static void read(zone_reader& reader, ClientCharacter* asset);
	};
}
