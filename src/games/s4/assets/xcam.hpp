#pragma once
#include "../s4.hpp"

namespace zonetool::s4
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
