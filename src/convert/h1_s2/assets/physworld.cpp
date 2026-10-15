#include <std_include.hpp>

#include "../include.hpp"
#include "physworld.hpp"

namespace zonetool::h1::converter::s2::physworld
{
	namespace target = zonetool::s2;

	// stock S2 models are {int polytope_start, u16 polytope_count, u16 mesh}. A model without a shape has
	// polytope_start -1, polytope_count 0 and mesh 0xffff. The engine reads all polytopes of model 0 (0x631F89).
	constexpr std::uint64_t empty_model = 0xFFFF0000FFFFFFFF;

	namespace
	{
		// stock S2 mesh node: float bounds[6], then a and b.
		// leaf: a = first triangle, b = count << 24 | 3 << 16 | flags.
		// internal: a = offset to the right child (the left child is the next node), b = axis << 16 | flags.
		// flags is the union of the low 16 bits of the collision flags of the triangles under the node.
		struct s2_mesh_node
		{
			float mins[3];
			float maxs[3];
			std::uint32_t a;
			std::uint32_t b;
		};
		static_assert(sizeof(s2_mesh_node) == 32);

		// stock S2 meshes store the tree height, the surface area and a zero after the counts
		struct s2_mesh_tail
		{
			int height;
			float area;
			int unused;
		};
		static_assert(sizeof(s2_mesh_tail) == sizeof(target::PhysMeshData::__pad0));

		// stock leaf bounds are padded by 1/2.54 units
		constexpr auto node_padding = 0.3937008f;
		constexpr auto leaf_axis = 3u;
		constexpr auto max_leaf_triangles = 4u;

		// H1 world triangles all have collision flags 1, most stock S2 world triangles have 0xffff (collide with everything)
		constexpr auto world_collision_flags = 0xFFFF;

		struct mesh_builder
		{
			const dmMeshData& source;
			std::vector<dmMeshTriangle> triangles;
			std::vector<s2_mesh_node> nodes;
			std::vector<std::array<float, 3>> centers;
			std::vector<std::size_t> order;
			int height = 0;

			explicit mesh_builder(const dmMeshData& mesh)
				: source(mesh)
			{
				const auto count = static_cast<std::size_t>(mesh.m_triangleCount);
				centers.resize(count);
				order.resize(count);
				for (auto i = 0u; i < count; i++)
				{
					order[i] = i;
					const auto& tri = mesh.m_aTriangles[i];
					for (auto axis = 0; axis < 3; axis++)
					{
						centers[i][axis] = (vertex(tri.i1, axis) + vertex(tri.i2, axis) + vertex(tri.i3, axis)) / 3.0f;
					}
				}

				if (count)
				{
					build(0, count, 1);
				}
			}

			float vertex(const int index, const int axis) const
			{
				const auto& v = source.m_aVertices[index];
				return axis == 0 ? v.x : axis == 1 ? v.y : v.z;
			}

			std::size_t build(const std::size_t begin, const std::size_t end, const int depth)
			{
				height = std::max(height, depth);

				const auto index = nodes.size();
				nodes.emplace_back();

				s2_mesh_node node{};
				std::fill_n(node.mins, 3, std::numeric_limits<float>::max());
				std::fill_n(node.maxs, 3, -std::numeric_limits<float>::max());

				std::uint32_t flags = 0;
				float center_mins[3];
				float center_maxs[3];
				std::fill_n(center_mins, 3, std::numeric_limits<float>::max());
				std::fill_n(center_maxs, 3, -std::numeric_limits<float>::max());

				for (auto i = begin; i < end; i++)
				{
					const auto& tri = source.m_aTriangles[order[i]];
					flags |= world_collision_flags;
					for (auto axis = 0; axis < 3; axis++)
					{
						for (const auto vert : {tri.i1, tri.i2, tri.i3})
						{
							node.mins[axis] = std::min(node.mins[axis], vertex(vert, axis) - node_padding);
							node.maxs[axis] = std::max(node.maxs[axis], vertex(vert, axis) + node_padding);
						}

						center_mins[axis] = std::min(center_mins[axis], centers[order[i]][axis]);
						center_maxs[axis] = std::max(center_maxs[axis], centers[order[i]][axis]);
					}
				}

				const auto count = end - begin;
				if (count <= max_leaf_triangles)
				{
					node.a = static_cast<std::uint32_t>(triangles.size());
					node.b = static_cast<std::uint32_t>(count) << 24 | leaf_axis << 16 | flags;
					for (auto i = begin; i < end; i++)
					{
						auto& tri = triangles.emplace_back(source.m_aTriangles[order[i]]);
						tri.collisionFlags = world_collision_flags;
					}

					nodes[index] = node;
					return index;
				}

				auto axis = 0u;
				for (auto i = 1u; i < 3; i++)
				{
					if (center_maxs[i] - center_mins[i] > center_maxs[axis] - center_mins[axis])
					{
						axis = i;
					}
				}

				const auto middle = begin + count / 2;
				std::nth_element(order.begin() + begin, order.begin() + middle, order.begin() + end, [&](const auto lhs, const auto rhs)
				{
					return centers[lhs][axis] < centers[rhs][axis];
				});

				build(begin, middle, depth + 1);
				const auto right = build(middle, end, depth + 1);

				node.a = static_cast<std::uint32_t>(right - index);
				node.b = axis << 16 | flags;
				nodes[index] = node;
				return index;
			}

			float area() const
			{
				auto total = 0.0f;
				for (const auto& tri : triangles)
				{
					float edge[2][3]{};
					for (auto axis = 0; axis < 3; axis++)
					{
						edge[0][axis] = vertex(tri.i2, axis) - vertex(tri.i1, axis);
						edge[1][axis] = vertex(tri.i3, axis) - vertex(tri.i1, axis);
					}

					const float cross[3] =
					{
						edge[0][1] * edge[1][2] - edge[0][2] * edge[1][1],
						edge[0][2] * edge[1][0] - edge[0][0] * edge[1][2],
						edge[0][0] * edge[1][1] - edge[0][1] * edge[1][0],
					};
					total += std::sqrt(cross[0] * cross[0] + cross[1] * cross[1] + cross[2] * cross[2]) * 0.5f;
				}

				return total;
			}
		};

		// H1 stores the tree as quantized nodes and S2 as float nodes, so the tree is rebuilt from the triangles
		target::PhysMeshData convert_mesh(const dmMeshData& mesh, utils::memory::allocator& allocator)
		{
			const mesh_builder builder(mesh);

			target::PhysMeshData result{};
			result.vertexCount = static_cast<int>(builder.nodes.size());
			result.normalCount = mesh.m_vertexCount;
			result.triangleCount = static_cast<int>(builder.triangles.size());

			// THIS CAN BE CONFUSING, but vertices holds the nodes and normals holds the vertices for s2
			const auto nodes = allocator.allocate_array<char[32]>(builder.nodes.size());
			std::memcpy(nodes, builder.nodes.data(), builder.nodes.size() * sizeof(s2_mesh_node));
			result.vertices = nodes;

			const auto vertices = allocator.allocate_array<char[16]>(mesh.m_vertexCount);
			std::memcpy(vertices, mesh.m_aVertices, mesh.m_vertexCount * sizeof(dmFloat4));
			result.normals = vertices;

			const auto triangles = allocator.allocate_array<char[32]>(builder.triangles.size());
			std::memcpy(triangles, builder.triangles.data(), builder.triangles.size() * sizeof(dmMeshTriangle));
			result.triangles = triangles;

			const s2_mesh_tail tail{builder.height, builder.area(), 0};
			std::memcpy(result.__pad0, &tail, sizeof(tail));

			ZONETOOL_INFO("PhysWorld mesh: %d vertices, %d triangles, %d nodes, height %d", mesh.m_vertexCount,
				result.triangleCount, result.vertexCount, builder.height);
			return result;
		}

		std::uint64_t convert_model(const PhysBrushModel& model, const std::size_t mesh_count)
		{
			const auto mesh = model.fields.meshIndex;
			if (mesh < 0 || static_cast<std::size_t>(mesh) >= mesh_count)
			{
				return empty_model;
			}

			return static_cast<std::uint64_t>(static_cast<std::uint16_t>(mesh)) << 48 | 0x0000FFFFFFFFull;
		}
	}

	// TODO: polytopes and water volumes are not converted
	void dump(PhysWorld* asset)
	{
		if (asset->polytopeCount || asset->waterVolumeDefCount)
		{
			ZONETOOL_WARNING("PhysWorld \"%s\": %u polytopes and %u water volumes are not converted",
				asset->name, asset->polytopeCount, asset->waterVolumeDefCount);
		}

		utils::memory::allocator allocator;

		std::vector<target::PhysMeshData> meshes;
		for (auto i = 0u; asset->meshDatas && i < asset->meshDataCount; i++)
		{
			meshes.push_back(convert_mesh(asset->meshDatas[i], allocator));
		}

		std::vector<std::uint64_t> models(asset->brushModelCount, empty_model);
		for (auto i = 0u; asset->brushModels && i < asset->brushModelCount; i++)
		{
			models[i] = convert_model(asset->brushModels[i], meshes.size());
		}

		target::PhysWorld new_asset{};
		new_asset.name = asset->name;
		new_asset.modelCount = asset->brushModelCount;
		new_asset.models = models.empty() ? nullptr : models.data();
		new_asset.meshCount = static_cast<unsigned int>(meshes.size());
		new_asset.meshes = meshes.empty() ? nullptr : meshes.data();
		target::phys_worldmap::dump(&new_asset);
		add_csv_row("phys_worldmap", new_asset.name);
	}
}
