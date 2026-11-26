#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class dyn_model
	{
	public:
		static void read(auto& reader, DynModel* asset);
	};
}
