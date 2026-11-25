#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class xmodel
	{
	public:
		static void read(zone_reader32& reader, XModel* asset);
	};
}
