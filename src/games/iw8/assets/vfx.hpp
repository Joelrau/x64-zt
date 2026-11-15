#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class vfx
	{
	public:
		static void read(zone_reader& reader, ParticleSystemDef* asset);
	};

	class vector_field
	{
	public:
		static void read(zone_reader& reader, VectorField* asset);
	};

	class footstep_vfx
	{
	public:
		static void read(zone_reader& reader, FootstepVFX* asset);
	};
}
