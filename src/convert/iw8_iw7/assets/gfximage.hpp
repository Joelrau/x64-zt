#pragma once

namespace zonetool::iw8::converter::iw7::gfximage
{
	void dump(GfxImage* asset);
	std::optional<std::string> dump_grading_clut(const GfxGradingClut* clut);
	std::optional<std::string> dump_color(const GfxImage* asset);
	std::optional<std::string> dump_color_specular(const GfxImage* asset, unsigned char scale, unsigned char offset);
	std::optional<std::string> dump_emissive(const GfxImage* asset, const std::array<float, 3>& tint);
	std::optional<std::string> dump_normal_gloss(const GfxImage* asset, unsigned char gloss_scale, unsigned char gloss_offset, unsigned char normal_scale);
	std::string dump_layer_reveal(GfxImage* mask, bool use_coverage, bool use_height);
	void dump_reflection_probe_array(const GfxImage* octahedron, const GfxSH3Vec* luminance, std::size_t luminance_count, const char* name);
	std::optional<std::array<std::string, 3>> dump_lightmap(const GfxLightmapRaw& lightmap, const std::string& name, float scale);
}
