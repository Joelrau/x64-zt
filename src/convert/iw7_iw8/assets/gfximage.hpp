#pragma once

namespace zonetool::iw7::converter::iw8::gfximage
{
	zonetool::iw8::GfxImage* dump_color(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_alpha(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_color_specular(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_normal_gloss(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_sky(const GfxImage* asset);
	std::optional<zonetool::iw8::GfxLightmapRaw> convert_lightmap(const GfxLightMap* lightmap, utils::memory::allocator& allocator);
}
