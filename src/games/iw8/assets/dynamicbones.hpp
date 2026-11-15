#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class dynamic_bones
	{
	public:
		static void read(zone_reader& reader, XAnimDynamicBones* asset);
	};

	class anim_curve
	{
	public:
		static void read(zone_reader& reader, XAnimCurve* asset);
	};
}
