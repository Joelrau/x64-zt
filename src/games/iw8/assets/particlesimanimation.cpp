#include <std_include.hpp>
#include "particlesimanimation.hpp"

namespace zonetool::iw8
{
	void particle_sim_animation::read(zone_reader& reader, FxParticleSimAnimation* asset)
	{
		const auto& header = asset->header;

		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->material);
		reader.read_array(asset->particleData, 31, header.particleDataCount);
		reader.read_array(asset->particleDataOrientation3D, 7, header.hasOrientation3D ? header.particleDataCount : 0);
		reader.read_array(asset->particleDataIndex, 0, header.hasIndex ? header.particleDataCount : 0);
		reader.read_array(asset->frames, 3, header.frameCount);
		reader.read_array(asset->colorTable, 3, header.colorTableSize);
		reader.pop_stream();
	}
}
