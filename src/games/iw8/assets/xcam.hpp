#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class xcam
	{
	public:
		static void read(zone_reader& reader, XCam* asset);
	};

	class execution_def
	{
	public:
		static void read(zone_reader& reader, ExecutionDef* asset);
	};
}
