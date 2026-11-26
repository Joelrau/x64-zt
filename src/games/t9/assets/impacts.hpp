#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class surface_sound_def
	{
	public:
		static void read(auto&, SurfaceSoundDef*)
		{
		}
	};

	class entity_sound_impacts
	{
	public:
		static void read(auto&, EntitySoundImpacts*)
		{
		}
	};

	class impact_sounds_table
	{
	public:
		static void read(auto& reader, ImpactSoundsTable* asset);
	};

	class surface_fx_table
	{
	public:
		static void read(auto& reader, SurfaceFxTable* asset);
	};

	class entity_fx_impacts
	{
	public:
		static void read(auto& reader, EntityFxImpacts* asset);
	};

	class fx_impact_table
	{
	public:
		static void read(auto& reader, FxImpactTable* asset);
	};
}
