#pragma once

namespace zonetool::iw8::converter::iw7::havok
{
	struct converted_mesh
	{
		std::vector<zonetool::iw7::hkcdStaticMeshTreeBasePrimitive> primitives;
		std::vector<unsigned short> shared_vertices_index;
		std::vector<unsigned long long> shared_vertices;
		std::vector<unsigned int> quad_is_flat;
		int num_triangles;
		int num_convex_shapes;
	};

	std::string convert_world_shape(std::string_view tagfile);
	std::string convert_ents_shapes(std::string_view tagfile);
}
