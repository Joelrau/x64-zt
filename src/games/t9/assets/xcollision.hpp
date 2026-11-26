#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	void write_hull_raw(filesystem::file& file, const XCollisionHull* hull);

	class xcollision
	{
	public:
		static void read(auto& reader, XCollision* asset);
		static void dump(XCollision* asset);
	};

	class trigger_actions
	{
	public:
		static void read(auto&, TriggerActions*)
		{
		}
	};
}
