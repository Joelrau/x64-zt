#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class animset
	{
	public:
		static void read(zone_reader& reader, Animset* asset);
	};

	class ai_asm
	{
	public:
		static void read(zone_reader& reader, ASM* asset);
	};
}
