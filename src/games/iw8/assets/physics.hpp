#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class physics_sfx_event
	{
	public:
		static void read(zone_reader& reader, PhysicsSFXEventAsset* asset);
	};

	class physics_vfx_event
	{
	public:
		static void read(zone_reader& reader, PhysicsVFXEventAsset* asset);
	};

	class physics_asset
	{
	public:
		static void read(zone_reader& reader, PhysicsAsset* asset);
	};

	class physics_fx_shape
	{
	public:
		static void read(zone_reader& reader, PhysicsFXShape* asset);
	};

	class physics_fx_pipeline
	{
	public:
		static void read(zone_reader& reader, PhysicsFXPipeline* asset);
	};
}
