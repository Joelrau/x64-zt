#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class camera_def
	{
	public:
		static void read(zone_reader& reader, CameraDef* asset);
	};
}
