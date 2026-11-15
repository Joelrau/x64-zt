#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class procedural_bones
	{
	public:
		static void read(zone_reader& reader, XAnimProceduralBones* asset);
	};
}
