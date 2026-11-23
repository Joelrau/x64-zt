#pragma once

#include "scene.hpp"

#include <array>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace utils::umbra
{
	// IW7 user IDs: type in bits 28..30, index in bits 0..23.
	enum user_id_type : std::uint32_t
	{
		USER_ID_SURFACE = 0x00000000,
		USER_ID_SMODEL = 0x10000000,
		USER_ID_VOLUMETRIC = 0x30000000,
		USER_ID_PRIMARY_LIGHT = 0x40000000,
		USER_ID_REFLECTION_PROBE = 0x50000000,
		USER_ID_DECAL = 0x60000000,
		USER_ID_INDEX_MASK = 0x00FFFFFF,
	};

	struct tome_model
	{
		std::vector<float> vertices;
		std::vector<std::uint32_t> indices;
	};

	struct tome_object
	{
		std::uint32_t model;
		std::uint32_t user_id;
		std::uint32_t flags;
	};

	struct tome_input
	{
		std::string name;
		std::vector<tome_model> models;
		std::vector<tome_object> objects;
		std::vector<scene_view_volume> view_volumes;
		std::vector<std::array<float, 3>> seed_points;
		scene_params params = default_params();

		std::string work_directory;
		bool verify = true;
		bool keep_files = true;

		static scene_params default_params();
		std::uint32_t add_box_model(const float* mins, const float* maxs);
	};

	struct tome_stats
	{
		std::uint32_t version = 0;
		std::uint32_t size = 0;
		std::uint32_t crc32 = 0;
		float lod_base_distance = 0.0f;
		float tree_min[3]{};
		float tree_max[3]{};
		std::int32_t object_count = 0;
		std::uint32_t user_id_count = 0;
		std::int32_t tile_count = 0;
		std::int32_t leaf_tile_count = 0;
		std::int32_t cluster_count = 0;
		std::int32_t gate_count = 0;
		std::uint32_t cell_count = 0;
		std::uint32_t portal_count = 0;
		std::uint32_t cells_without_portals = 0;
		std::uint32_t object_list_entries = 0;
	};

	struct tome_result
	{
		std::vector<unsigned char> data;
		tome_stats stats;
		double generation_seconds = 0.0;
		std::string error;
	};

	using log_callback = std::function<void(const std::string&)>;

	// ZT_UMBRA_TOMEGEN, else umbra-tomegen.exe next to this module; empty if neither exists.
	std::string find_generator();

	bool generate_tome(const tome_input& input, tome_result& result, const log_callback& log);
	bool inspect_tome(const unsigned char* data, std::size_t size, tome_stats& stats);

	// The optimizer drops targets that land in no cell, so callers diff this against their input.
	std::vector<std::uint32_t> read_user_ids(const unsigned char* data, std::size_t size);

	bool is_tome_accepted_by_iw7(const unsigned char* data, std::size_t size, std::string& reason);
}
