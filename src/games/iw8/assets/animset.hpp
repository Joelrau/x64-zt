#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
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
