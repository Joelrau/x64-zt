#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class codcaster_data
	{
	public:
		static void read(zone_reader& reader, CodcasterData* asset);
	};
}
