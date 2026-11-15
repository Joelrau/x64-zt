#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class nav_mesh
	{
	public:
		static void read(zone_reader& reader, NavMeshData* asset);
	};
}
