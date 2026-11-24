#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace zonetool::iw7
{
	namespace navpower::builder
	{
		struct triangle
		{
			float verts[3][3];
		};

		struct triangle_buckets
		{
			float origin[2];
			float bucket_size;
			int width;
			int height;
			std::vector<std::vector<int>> triangles;
		};

		struct column_hit
		{
			float z;
			float normal_z;
		};

		struct floor_span
		{
			float z;
			int links[4]; // +x, +y, -x, -y
			int area = -1;
			bool removed = false;
		};

		struct span_grid
		{
			float origin[2];
			float cell_size;
			int width;
			int height;
			std::vector<std::vector<int>> cell_spans;
			std::vector<floor_span> spans;
		};

		struct area_rect
		{
			int x0, y0, x1, y1;
			std::vector<std::vector<int>> rows;
		};

		struct area_edge
		{
			int grid_vertex[2];
			float vertex[3];
			int neighbor;
			std::uint32_t portal;
		};

		struct area_polygon
		{
			std::vector<area_edge> edges;
			float center[3];
			float radius;
			float mins[3];
			float maxs[3];
		};

		struct traversal
		{
			float start[3];
			float end[3];
		};

		struct navmesh_result
		{
			std::vector<std::uint8_t> graph;
			std::vector<traversal> traversals;
		};

		navmesh_result build_graph(const std::vector<triangle>& solid_triangles, const std::vector<std::array<float, 3>>& seeds);
	}
}
