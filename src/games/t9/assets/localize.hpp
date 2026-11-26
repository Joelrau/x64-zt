#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class localize
	{
	public:
		static void read(auto& reader, LocalizeEntry* asset);
	};
}
