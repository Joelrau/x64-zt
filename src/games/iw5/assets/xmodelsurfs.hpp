#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class xmodel_surfs
	{
	public:
		static void read(zone_reader32& reader, XModelSurfs* asset);
	};
}
