#pragma once

namespace zonetool::s4::converter::iw8::vfx
{
	zonetool::iw8::ParticleSystemDef* convert(const ParticleSystemDef* asset, utils::memory::allocator& allocator);
	void dump(ParticleSystemDef* asset);
}
