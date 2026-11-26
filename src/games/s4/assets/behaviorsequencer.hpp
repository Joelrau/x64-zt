#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class behavior_sequencer
	{
	public:
		static void read(zone_reader& reader, BehaviorSequencer* asset);
	};
}
