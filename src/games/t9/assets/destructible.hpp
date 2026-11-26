#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class destructible_def
	{
	public:
		static void read(auto& reader, DestructibleDef* asset);
	};

	class zbarrier
	{
	public:
		static void read(auto& reader, ZBarrier* asset);
	};
}
