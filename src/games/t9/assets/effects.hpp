#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	void read_value_array(auto& reader, ValueArray& array);

	class tag_fx
	{
	public:
		static void read(auto& reader, TagFx* asset);
	};

	class beam
	{
	public:
		static void read(auto& reader, Beam* asset);
	};

	class tracer
	{
	public:
		static void read(auto& reader, Tracer* asset);
	};

	class laser
	{
	public:
		static void read(auto& reader, Laser* asset);
	};

	class flame_table
	{
	public:
		static void read(auto& reader, FlameTable* asset);
	};

	class ballistic_desc
	{
	public:
		static void read(auto& reader, BallisticDesc* asset);
	};

	class trigger_effect_desc
	{
	public:
		static void read(auto& reader, TriggerEffectDesc* asset);
	};

	class shell_shock
	{
	public:
		static void read(auto& reader, ShellShock* asset);
	};

	class status_effect
	{
	public:
		static void read(auto& reader, StatusEffect* asset);
	};

	class vehicle_fx_def
	{
	public:
		static void read(auto& reader, VehicleFxDef* asset);
	};
}
