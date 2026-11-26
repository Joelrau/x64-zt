#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class behavior_tree
	{
	public:
		static void read(zone_reader& reader, BehaviorTree* asset);
	};
}
