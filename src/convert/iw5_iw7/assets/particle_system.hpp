#pragma once
#include "../include.hpp"

namespace zonetool::iw5::converter::iw7
{
	namespace particle_system
	{
		enum class size_channel
		{
			none,
			width,
			height,
			scale,
		};

		struct emitter_build
		{
			const zonetool::iw5::FxElemDef* elem;
			utils::memory::allocator* allocator;
			unsigned int system_flags;
			std::uint64_t state_flags;
			unsigned int emitter_flags;
			unsigned short test_module_count;
			std::vector<zonetool::iw7::ParticleModuleDef> modules;
		};

		std::string asset_name(const std::string& effect_name);
		zonetool::iw7::ParticleSystemDef* convert(zonetool::iw5::FxEffectDef* asset, utils::memory::allocator& allocator);
		void dump(zonetool::iw5::FxEffectDef* asset);
	}
}
