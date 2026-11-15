#pragma once

namespace zonetool::iw8::converter::iw7::material
{
	struct surface_constants
	{
		std::array<float, 4> uv_transform{1.0f, 1.0f, 0.0f, 0.0f};
		std::array<float, 4> uv_rotation{1.0f, 0.0f, 0.0f, 0.0f};
		std::array<float, 4> color_tint{1.0f, 1.0f, 1.0f, 0.0f};
		std::array<float, 4> surface_ranges{1.0f, 0.0f, 1.0f, 1.0f};
		std::array<float, 4> decal_params{};
	};

	struct state_definition
	{
		std::array<unsigned int, 6> load_bits;
		std::array<std::uint64_t, 14> depth_stencil_bits;
		std::array<std::uint32_t, 4> blend_bits;
		unsigned char rasterizer_state;
	};

	struct material_flags
	{
		unsigned char game_flags;
		unsigned char sort_key;
		unsigned char state_flags;
		unsigned char material_type;
		unsigned char render_flags;
	};

	struct stock_techset
	{
		const char* name;
		bool in_common_zone;
		material_flags flags;
		unsigned char camera_region;
		unsigned char sampler_state;
		std::vector<state_definition> states;
		std::vector<std::pair<unsigned char, unsigned char>> technique_states;
		std::vector<unsigned char> constant_buffer_techniques;
	};

	bool is_supported(const Material* asset);
	bool uses_lightmap(const Material* asset);
	unsigned int get_vertex_layer_count(const Material* asset);
	bool dump(Material* asset);
}
