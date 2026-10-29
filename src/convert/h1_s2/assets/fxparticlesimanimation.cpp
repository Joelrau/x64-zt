#include <std_include.hpp>

#include "../include.hpp"
#include "fxparticlesimanimation.hpp"

namespace zonetool::h1::converter::s2::fxparticlesimanimation
{
	namespace
	{
		zonetool::s2::FxParticleSimAnimation* convert(FxParticleSimAnimation* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::s2::FxParticleSimAnimation>();
			new_asset->name = asset->name;
			new_asset->material = reinterpret_cast<zonetool::s2::Material*>(asset->material);
			std::memcpy(&new_asset->header, &asset->header, sizeof(new_asset->header));
			new_asset->particleData = reinterpret_cast<zonetool::s2::FxParticleSimAnimationParticleData*>(asset->particleData);
			new_asset->frames = reinterpret_cast<zonetool::s2::FxParticleSimAnimationFrame*>(asset->frames);
			new_asset->colorTable = reinterpret_cast<zonetool::s2::GfxColorFloat*>(asset->colorTable);
			return new_asset;
		}
	}

	void dump(FxParticleSimAnimation* asset)
	{
		utils::memory::allocator allocator;
		const auto converted = convert(asset, allocator);
		zonetool::s2::particle_sim_animation::dump(converted);
		add_csv_row(zonetool::s2::type_to_string(zonetool::s2::ASSET_TYPE_PARTICLE_SIM_ANIMATION), asset->name);
	}
}
