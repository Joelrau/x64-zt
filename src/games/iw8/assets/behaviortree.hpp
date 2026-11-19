#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class behavior_tree
	{
	public:
		static void read(zone_reader& reader, BehaviorTree* asset);
	};
}
