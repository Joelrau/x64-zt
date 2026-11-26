#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class nav_mesh
	{
	public:
		static void read(auto& reader, NavMesh* asset);
	};

	class nav_volume
	{
	public:
		static void read(auto& reader, NavVolume* asset);
	};
}
