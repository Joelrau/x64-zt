#pragma once

namespace zonetool::h1::converter::s2::shader
{
	constexpr auto h1_stable_constant_buffer = 2u;
	constexpr auto h1_material_constant_buffer = 3u;
	constexpr auto s2_stable_constant_buffer = h1_stable_constant_buffer + 1;
	constexpr auto s2_material_constant_buffer = h1_material_constant_buffer + 1;
	constexpr auto s2_model_lighting_index_dest = 10u; // fixed per prim row of the cached model draws
	constexpr auto s2_model_lightmap_params_dest = 10u;
	constexpr auto light_grid_colors_slot = 19u;
	constexpr auto light_grid_visibility_slot = 20u;
	constexpr auto ambient_light_grid_sampler = 0u;
	constexpr auto sprite_buffer_slot = 12u;
	constexpr auto forward_lights_slot = 16u;
	constexpr auto light_tile_bits_slot = 23u;
	constexpr auto spot_shadow_slot = 21u;
	constexpr auto spot_shadow_dynamic_slot = 22u;
	constexpr auto spot_shadow_sampler = 15u;
	constexpr auto spot_shadow_record_slot = 17u;
	constexpr auto light_cluster_bits_slot = 24u;
	constexpr auto light_tile_zbins_slot = 25u;
	constexpr std::array<std::uint32_t, 7> forward_light_slots{forward_lights_slot, spot_shadow_record_slot, spot_shadow_slot, spot_shadow_dynamic_slot,
		light_tile_bits_slot, light_cluster_bits_slot, light_tile_zbins_slot};
	constexpr auto light_const_count = 5u;

	struct primary_lights
	{
		std::uint32_t buffer;
		std::array<std::optional<std::uint32_t>, light_const_count> rows;
		std::uint32_t render_target_size_row;
		std::uint32_t tile_parms_row;
		bool spot_shadows;
		std::optional<std::uint32_t> cluster_row;
	};

	enum class light_grid
	{
		probe_buffer,
		array_2d,
		volume_3d,
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

	using constant_rows = std::map<std::uint32_t, std::uint32_t>;

	struct stable_literal_row
	{
		std::uint32_t buffer;
		std::uint32_t from;
		std::uint32_t to;
	};

	struct foliage_wind
	{
		std::uint32_t time_row;
		std::uint32_t wind_row;
		std::uint32_t params_row;
	};

	struct atmos_fog_rows
	{
		std::optional<std::uint32_t> extinction;
		std::optional<std::uint32_t> in_scatter;
		std::optional<std::uint32_t> sun_dir;
	};

	struct program_patch
	{
		sun_shadow_lookup sun_shadow;
		std::optional<model_lighting> lighting;
		light_grid grid{};
		std::uint32_t lighting_semantic{};
		std::optional<std::uint32_t> lightmap_params_dest;
		std::vector<stable_literal_row> stable_literal_rows;
		bool sprite{};
		bool lit_instance_rows{};
		std::optional<primary_lights> lights;
		std::optional<foliage_wind> wind;
		constant_rows material_rows;
		std::optional<std::uint32_t> ssr_texture;
		atmos_fog_rows fog;
	};

	struct patched_program
	{
		std::string data;
		bool reads_lightmap;
		std::optional<std::uint32_t> lighting_semantic;
		std::map<std::uint32_t, std::uint32_t> texture_slots;
	};

	struct light_merge
	{
		primary_lights lights;
		constant_rows material_rows;
		constant_rows stable_rows;
		std::map<std::uint32_t, std::uint32_t> texture_slots;
		std::map<std::uint32_t, std::uint32_t> sampler_slots;
	};

	std::string instance_program(const unsigned char* program, unsigned int size, std::optional<std::uint32_t> coords_row);
	patched_program patch_program(const unsigned char* program, unsigned int size, bool is_vertex_shader, const program_patch& patch);
	patched_program merge_light_program(const unsigned char* lit, unsigned int lit_size, const unsigned char* light, unsigned int light_size, const light_merge& merge);
	std::pair<std::set<std::uint32_t>, std::set<std::uint32_t>> declared_slots(const unsigned char* program, unsigned int size);
}
