#pragma once

namespace zonetool::h1::converter::s2::shader
{
	constexpr auto s2_model_lighting_index_dest = 10u; // cb0[10] of the fixed per prim layout of the cached model draws (sub_1407E8F90)
	constexpr auto s2_model_lightmap_params_dest = 10u; // the same row in the "ml" passes
	constexpr auto light_grid_colors_slot = 19u;
	constexpr auto light_grid_visibility_slot = 20u;
	constexpr auto ambient_light_grid_sampler = 0u;

	enum class light_grid
	{
		probe_buffer, // "lgv0d"
		array_2d, // "lgv1d2d"
		volume_3d, // "lgv3d"
	};

	struct model_lighting
	{
		bool instanced;
		std::uint16_t coords_dest;
		std::uint16_t eye_offset_dest;
		std::uint16_t texture;
		bool ambient;
	};

	struct sun_shadow_lookup
	{
		std::optional<std::uint32_t> texture;
		std::unordered_map<std::uint32_t, float> cascade_slices;
	};

	struct stable_literal_row
	{
		std::uint32_t buffer;
		std::uint32_t from;
		std::uint32_t to;
	};

	struct program_patch
	{
		sun_shadow_lookup sun_shadow;
		std::optional<model_lighting> lighting;
		light_grid grid{};
		std::uint32_t lighting_semantic{};
		std::optional<std::uint32_t> lightmap_params_dest;
		std::vector<stable_literal_row> stable_literal_rows;
	};

	struct patched_program
	{
		std::string data;
		bool reads_lightmap;
		std::optional<std::uint32_t> lighting_semantic;
		std::map<std::uint32_t, std::uint32_t> texture_slots;
	};

	patched_program patch_program(const unsigned char* program, unsigned int size, bool is_vertex_shader, const program_patch& patch);
}
