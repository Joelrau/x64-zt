#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class skeleton_constraints
	{
	public:
		static void read(zone_reader& reader, SkeletonConstraints* asset);
	};
}
