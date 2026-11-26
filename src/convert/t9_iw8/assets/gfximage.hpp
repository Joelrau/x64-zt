#pragma once

namespace zonetool::t9::converter::iw8::gfximage
{
	struct color_tint
	{
		std::array<float, 3> value;
		const GfxImage* mask;
	};

	struct color_layer
	{
		const GfxImage* color;
		color_tint tint;
		std::array<float, 2> scale;
	};

	struct layer_mix
	{
		const GfxImage* map;
		std::array<float, 2> scale;
		std::array<float, 4> levels;
	};

	struct softlight_detail
	{
		const GfxImage* map;
		std::array<float, 2> scale;
	};

	struct layered_color
	{
		std::vector<color_layer> layers;
		std::vector<layer_mix> mixes;
		softlight_detail softlight;
	};

	zonetool::iw8::GfxImage* dump_color(const GfxImage* asset, const color_tint* tint = nullptr);
	zonetool::iw8::GfxImage* dump_layered_color(const layered_color& asset);
	zonetool::iw8::GfxImage* dump_color_specular(const GfxImage* color, const GfxImage* specular, const color_tint* tint = nullptr);
	zonetool::iw8::GfxImage* dump_normal_gloss(const GfxImage* normal, const GfxImage* roughness, const GfxImage* occlusion);
	zonetool::iw8::GfxImage* dump_opacity(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_2d(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_reveal_opacity(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_alpha_transmission(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_identity_grading_clut(const std::string& name, std::size_t size);
	zonetool::iw8::GfxImage* dump_ies_lookup();
	zonetool::iw8::GfxImage* dump_uniform_reflection_probe(const std::string& name);
	zonetool::iw8::GfxImage* dump_sky(const GfxImage* asset);
	bool is_code_image(const GfxImage* asset);
	const char* get_color_reference(const GfxImage* asset);
}
