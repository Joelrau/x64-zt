#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class xmodel
	{
	public:
		static void read(zone_reader& reader, XModel* asset);
	};
}
