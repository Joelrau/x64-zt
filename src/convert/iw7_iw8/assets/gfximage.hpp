#pragma once

namespace zonetool::iw7::converter::iw8::gfximage
{
	zonetool::iw8::GfxImage* dump_color(const GfxImage* asset, const std::array<float, 3>& tint = {1.0f, 1.0f, 1.0f});
	zonetool::iw8::GfxImage* dump_color_premultiplied(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_decal_color(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_alpha(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_opacity(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_transmission(const GfxImage* coverage, bool coverage_in_alpha);
	zonetool::iw8::GfxImage* dump_layer_mask(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_layer_mask(const GfxImage* coverage, const GfxImage* reveal);
	zonetool::iw8::GfxImage* dump_2d(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_color_specular(const GfxImage* asset, const std::array<float, 3>& tint = {1.0f, 1.0f, 1.0f});
	zonetool::iw8::GfxImage* dump_color_specular(const GfxImage* color, const GfxImage* specular_gloss, const std::array<float, 3>& tint);
	zonetool::iw8::GfxImage* dump_normal_gloss(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_normal_gloss(const GfxImage* normal, const GfxImage* specular_gloss, const GfxImage* occlusion);
	zonetool::iw8::GfxImage* dump_sky(const GfxImage* asset, const std::array<float, 3>& rotation_angles);
	zonetool::iw8::GfxImage* dump_generated(const GfxImage* asset, zonetool::iw8::GfxPixelFormat format);
	zonetool::iw8::GfxImage* dump_grading_clut(const GfxImage* asset, const std::string& name, std::size_t size,
		const std::function<std::vector<std::uint16_t>(const std::vector<std::uint8_t>& texels, std::size_t source_size)>& convert_texels);
	zonetool::iw8::GfxImage* dump_reflection_probes(const GfxImage* asset, std::size_t probe_count);
	void dump_preview(const GfxImage* asset);
	std::optional<zonetool::iw8::GfxLightmapRaw> convert_lightmap(const GfxLightMap* lightmap, utils::memory::allocator& allocator);
}
