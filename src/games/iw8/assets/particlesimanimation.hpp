#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class particle_sim_animation
	{
	public:
		static void read(zone_reader& reader, FxParticleSimAnimation* asset);
	};
}
