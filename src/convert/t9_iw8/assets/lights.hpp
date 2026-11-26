#pragma once

namespace zonetool::t9::converter::iw8::lights
{
	std::vector<zonetool::iw8::ComPrimaryLight> convert_primary_lights(const Districts* asset);
	void log_lossy_fields(const Districts* asset);
	void build_frustum_lights(const std::vector<zonetool::iw8::ComPrimaryLight>& lights, zonetool::iw8::GfxFrustumLights& frustum_lights,
		utils::memory::allocator& allocator);
}
