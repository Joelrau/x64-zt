#pragma once

namespace zonetool::iw7::converter::iw8::techset
{
	enum generated_techset : std::uint8_t
	{
		GENERATED_TECHSET_WORLD,
		GENERATED_TECHSET_STATIC_MODEL,
		GENERATED_TECHSET_SKY,
		GENERATED_TECHSET_COUNT,
	};

	enum generated_blend : std::uint8_t
	{
		GENERATED_BLEND_OPAQUE,
		GENERATED_BLEND_ALPHA,
		GENERATED_BLEND_ADD,
		GENERATED_BLEND_MULTIPLY,
	};

	struct generated_variant
	{
		generated_techset techset;
		generated_blend blend = GENERATED_BLEND_OPAQUE;
		bool lit = true;
		bool uv_anim = false;
		bool alpha_test = false;
		bool two_sided = false;
		bool visible = true;
		bool casts_shadow = true;
	};

	struct technique_definition
	{
		zonetool::iw8::MaterialTechniqueType type;
		const char* name;
		const char* vertex_entry;
		const char* pixel_entry;
		std::vector<zonetool::iw8::MaterialRenderTargetFormat> render_target_formats;
		unsigned char color_rt_count;
		std::uint64_t custom_buffer_flags;
		unsigned char precompiled_index;
		zonetool::iw8::GfxStateBits state_bits;
		std::vector<zonetool::iw8::MaterialShaderArgument> args;
		std::vector<unsigned char> vertex_decls;
	};

	std::string get_techset_name(const generated_variant& variant);
	zonetool::iw8::MaterialGeometryType get_material_type(generated_techset techset);
	void dump_techset(const generated_variant& variant);
}
