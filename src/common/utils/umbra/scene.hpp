#pragma once

#include <cstdint>

namespace utils::umbra
{
	constexpr char SCENE_MAGIC[8] = {'Z', 'T', 'U', 'M', 'B', 'R', 'A', '1'};
	constexpr std::uint32_t SCENE_VERSION = 1;

	enum scene_object_flags : std::uint32_t
	{
		SCENE_OBJECT_OCCLUDER = 1u << 0,
		SCENE_OBJECT_TARGET = 1u << 1,
		SCENE_OBJECT_GATE = 1u << 2,
		SCENE_OBJECT_VOLUME = 1u << 3,
	};

	struct scene_params
	{
		float smallest_occluder;
		float smallest_hole;
		float backface_limit;
		float cluster_size;
		float object_group_cost;
		float minimum_accurate_distance;
		std::uint32_t output_flags;
		std::uint32_t thread_count;
	};

	struct scene_header
	{
		char magic[8];
		std::uint32_t version;
		std::uint32_t header_size;
		scene_params params;
		std::uint32_t model_count;
		std::uint32_t object_count;
		std::uint32_t view_volume_count;
		std::uint32_t seed_point_count;
	};

	// followed by float[vertex_count][3], then uint32_t[triangle_count][3]
	struct scene_model
	{
		std::uint32_t vertex_count;
		std::uint32_t triangle_count;
	};

	struct scene_object
	{
		std::uint32_t model;
		std::uint32_t user_id;
		std::uint32_t flags;
	};

	struct scene_view_volume
	{
		float mins[3];
		float maxs[3];
		std::uint32_t id;
	};

	struct scene_seed_point
	{
		float position[3];
	};

	static_assert(sizeof(scene_header) == 8 + 4 + 4 + 32 + 16);
	static_assert(sizeof(scene_model) == 8);
	static_assert(sizeof(scene_object) == 12);
	static_assert(sizeof(scene_view_volume) == 28);
	static_assert(sizeof(scene_seed_point) == 12);
}
