#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class rumble
	{
	public:
		static void read(auto& reader, RumbleInfo* asset);
	};
}
