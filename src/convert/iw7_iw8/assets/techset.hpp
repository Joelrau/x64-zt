#pragma once

namespace zonetool::iw7::converter::iw8::techset
{
	enum generated_techset : std::uint8_t
	{
		GENERATED_TECHSET_WORLD,
		GENERATED_TECHSET_STATIC_MODEL,
		GENERATED_TECHSET_COUNT,
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

	const char* get_techset_name(generated_techset techset);
	void dump_techset(generated_techset techset);
}
