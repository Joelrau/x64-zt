#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class font_icon
	{
	public:
		static void read(auto& reader, FontIcon* asset);
	};

	class objective
	{
	public:
		static void read(auto& reader, Objective* asset);
	};

	class objective_list
	{
	public:
		static void read(auto& reader, ObjectiveList* asset);
	};

	class ttf
	{
	public:
		static void read(auto& reader, TTFDef* asset);
	};

	class compass_tunables
	{
	public:
		static void read(auto& reader, CompassTunables* asset);
	};

	class overhead_camera_tunables
	{
	public:
		static void read(auto& reader, OverheadCameraTunables* asset);
	};

	class player_global_tunables
	{
	public:
		static void read(auto& reader, PlayerGlobalTunables* asset);
	};
}
