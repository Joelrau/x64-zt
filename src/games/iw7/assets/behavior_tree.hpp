#pragma once
#include "../iw7.hpp"

namespace zonetool::iw7
{
	class behavior_tree
	{
	public:
		static void read(zone_reader& reader, BehaviorTree* asset);
	};
}
