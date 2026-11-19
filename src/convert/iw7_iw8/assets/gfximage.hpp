#pragma once

namespace zonetool::iw7::converter::iw8::gfximage
{
	zonetool::iw8::GfxImage* dump_color(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_alpha(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_opacity(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_color_specular(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_normal_gloss(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_normal_gloss(const GfxImage* normal, const GfxImage* specular_gloss, const GfxImage* occlusion);
	zonetool::iw8::GfxImage* dump_sky(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_reflection_probes(const GfxImage* asset, const std::vector<std::array<float, 4>>& luminance_sh);
	std::optional<zonetool::iw8::GfxLightmapRaw> convert_lightmap(const GfxLightMap* lightmap, utils::memory::allocator& allocator);
}
