#pragma once

namespace zonetool::h1::converter::s2::shader
{
	constexpr auto h1_stable_constant_buffer = 2u;
	constexpr auto h1_material_constant_buffer = 3u;
	constexpr auto s2_stable_constant_buffer = h1_stable_constant_buffer + 1; // s2 adds a buffer before the h1 stable one, so it and the ones after it move up one slot
	constexpr auto s2_material_constant_buffer = h1_material_constant_buffer + 1;
	constexpr auto s2_model_lighting_index_dest = 10u; // cb0[10] of the fixed per prim layout of the cached model draws (sub_1407E8F90)
	constexpr auto s2_model_lightmap_params_dest = 10u; // the same row in the "ml" passes
	constexpr auto light_grid_colors_slot = 19u;
	constexpr auto light_grid_visibility_slot = 20u;
	constexpr auto ambient_light_grid_sampler = 0u;
	constexpr auto sprite_buffer_slot = 12u; // raw t12 with a 64 byte record per sprite, bound for customBufferFlags bit 31
	constexpr auto forward_lights_slot = 16u; // structured t16 with a 104 byte record per light, bound for CUSTOM_BUFFER_FORWARD_LIGHTS (0xC0)
	constexpr auto light_tile_bits_slot = 23u; // uint t23 with the light bitmask words of each 8x8 pixel tile, bound for CUSTOM_BUFFER_LIGHT_TILE_BITS (0x400000)
	constexpr auto spot_shadow_slot = 21u; // texture2darray of the static spot shadow maps (code texture 8), the stock lw_fplus t5
	constexpr auto spot_shadow_dynamic_slot = 22u; // texture2darray of the dynamic spot shadow maps (code texture 13), the stock lw_fplus t7
	constexpr auto spot_shadow_sampler = 15u; // comparison sampler (code sampler 6), the stock lw_fplus s4
	constexpr auto spot_shadow_record_slot = 17u; // structured t17 with a 96 byte shadow record per shadow index: 4 matrix rows, filter scale and slices, bound for CUSTOM_BUFFER_FORWARD_LIGHTS
	constexpr auto light_cluster_bits_slot = 24u; // uint t24 with the light bitmask words of each froxel (screen cell x 32 view depth slices), bound for CUSTOM_BUFFER_LIGHT_CLUSTER_BITS (0x200000)
	constexpr auto light_tile_zbins_slot = 25u; // t25 with the light z bins (min / max light of each view depth bin) of the stock bmtile loop, bound for CUSTOM_BUFFER_LIGHT_TILE_ZBINS (0x100000)
	constexpr std::array<std::uint32_t, 7> forward_light_slots{forward_lights_slot, spot_shadow_record_slot, spot_shadow_slot, spot_shadow_dynamic_slot,
		light_tile_bits_slot, light_cluster_bits_slot, light_tile_zbins_slot}; // the fixed texture slots of the forward light buffers
	constexpr auto light_const_count = 5u;

	// the h1 light pass constants LIGHT_POSITION, DIFFUSE, PHYSICALSIZE, FADEOFFSET and SPECULAR, filled from the s2 forward light records
	struct primary_lights
	{
		std::uint32_t buffer;
		std::array<std::optional<std::uint32_t>, light_const_count> rows;
		std::uint32_t render_target_size_row;
		std::uint32_t tile_parms_row;
		bool shadows; // samples the s2 spot shadow maps of lights with a shadow index, like the stock lw_fplus loop
		std::optional<std::uint32_t> cluster_row; // first of the stable rows with code consts 93 (cells), 91 (slice starts) and 92 (slice scales), the stock bmclust loop of surfaces without a depth prepass
	};

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

	using constant_rows = std::map<std::uint32_t, std::uint32_t>;

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
		bool sprite{};
		bool lit_instance_rows{}; // the lit pass of the same draw fills cb7 with 9 rows per instance (appended light passes)
		std::optional<primary_lights> lights;
		constant_rows material_rows; // h1 material constant buffer rows read at another row
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
		constant_rows material_rows; // h1 light pass material rows read at the lit pass rows
		constant_rows stable_rows; // h1 light pass stable rows -> s2 stable rows of the merged pass
		std::map<std::uint32_t, std::uint32_t> texture_slots;
		std::map<std::uint32_t, std::uint32_t> sampler_slots;
	};

	patched_program patch_program(const unsigned char* program, unsigned int size, bool is_vertex_shader, const program_patch& patch);

	// appends the h1 light pass body, looped over the forward+ lights of the pixel's tile, to a converted lit pixel shader, texture_slots = moved lit textures
	patched_program merge_light_program(const unsigned char* lit, unsigned int lit_size, const unsigned char* light, unsigned int light_size, const light_merge& merge);
	std::pair<std::set<std::uint32_t>, std::set<std::uint32_t>> declared_slots(const unsigned char* program, unsigned int size); // textures, samplers
}
