#pragma once

namespace zonetool::t9::converter::iw8::havok
{
	using point = std::array<float, 3>;

	struct bounds
	{
		float min[3];
		float max[3];
	};

	struct aabb
	{
		float min[4];
		float max[4];
	};

	struct world_triangle
	{
		std::array<point, 3> points;
		std::uint64_t order;
	};

	using grid_point = std::array<std::int32_t, 3>;
	using grid_triangle = std::array<grid_point, 3>;

	struct output_section
	{
		std::array<float, 6> codec{};
		bounds domain{};
		std::uint16_t leaf_index{};
		std::uint32_t first_packed{};
		std::uint8_t packed_count{};
		std::vector<std::array<std::uint8_t, 4>> nodes;
		std::vector<std::array<std::uint8_t, 4>> primitives;
		std::vector<bounds> primitive_bounds;
		std::vector<zonetool::iw8::hkcdPrimitiveDataRun> runs;
		std::uint32_t first_primitive{};
		std::uint32_t first_run{};
	};

	struct top_node
	{
		bool leaf;
		std::uint32_t value;
	};

	struct mesh_shape
	{
		aabb domain{};
		std::vector<output_section> outputs;
		std::vector<std::uint32_t> packed_vertices;
		std::vector<std::byte> simd_nodes;
		std::vector<bounds> boxes;
		std::uint16_t box_shape_tag{};
		std::vector<std::uint16_t> shared_vertex_indices;
	};

	struct simd_item
	{
		bounds box;
		std::uint32_t key;
	};

	struct mesh_keys
	{
		std::uint32_t bits_per_key;
		std::uint32_t max_key;
		std::uint32_t triangle_count;
	};

	struct world_collision
	{
		std::string world_shape;
		std::string ents_shape;
	};

	using dvec3 = std::array<double, 3>;

	struct hull_plane
	{
		dvec3 normal;
		double distance;
	};

	world_collision build_world_collision(const zonetool::t9::ClipMap* clip_map, const std::vector<gfxworld::placed_model>& models,
		const std::vector<gfxworld::collision_triangle>& world_surface_triangles, const std::vector<mapents::trigger>& triggers,
		const std::string& map_name);
	std::string write_trigger_physics_asset();
}
