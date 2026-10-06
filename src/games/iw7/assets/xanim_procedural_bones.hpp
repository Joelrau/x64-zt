#pragma once
#include "../iw7.hpp"

namespace zonetool::iw7
{
	class xanim_procedural_bones
	{
	public:
		static void read(zone_reader& reader, XAnimProceduralBones* asset);
	};
}
