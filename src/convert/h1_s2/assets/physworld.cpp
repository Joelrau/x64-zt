#include <std_include.hpp>

#include "../include.hpp"
#include "physworld.hpp"

namespace zonetool::h1::converter::s2::physworld
{
	namespace
	{
		constexpr auto node_padding = 1.0f / 2.54f;
		constexpr auto world_collision_flags = 0xFFFF;

		struct mesh_builder
		{
			const dmMeshData& source;
			std::vector<zonetool::s2::dmMeshTriangle> triangles;
			std::vector<zonetool::s2::PhysMeshNode> nodes;
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

				zonetool::s2::PhysMeshNode node{};
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
				if (count <= 4)
				{
					node.u.firstTriangle = static_cast<unsigned int>(triangles.size());
					node.flags = flags;
					node.axis = 3;
					node.triangleCount = static_cast<unsigned int>(count);
					for (auto i = begin; i < end; i++)
					{
						auto& tri = triangles.emplace_back(std::bit_cast<zonetool::s2::dmMeshTriangle>(source.m_aTriangles[order[i]]));
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

				node.u.rightChildOffset = static_cast<unsigned int>(right - index);
				node.flags = flags;
				node.axis = axis;
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

		zonetool::s2::PhysMeshData convert_mesh(const dmMeshData& mesh, utils::memory::allocator& allocator)
		{
			const mesh_builder builder(mesh);

			zonetool::s2::PhysMeshData result{};
			result.nodeCount = static_cast<int>(builder.nodes.size());
			result.vertexCount = mesh.m_vertexCount;
			result.triangleCount = static_cast<int>(builder.triangles.size());

			result.nodes = allocator.allocate_array<zonetool::s2::PhysMeshNode>(builder.nodes.size());
			std::memcpy(result.nodes, builder.nodes.data(), builder.nodes.size() * sizeof(zonetool::s2::PhysMeshNode));
			result.vertices = allocator.allocate_array<zonetool::s2::dmFloat4>(mesh.m_vertexCount);
			std::memcpy(result.vertices, mesh.m_aVertices, mesh.m_vertexCount * sizeof(zonetool::s2::dmFloat4));
			result.triangles = allocator.allocate_array<zonetool::s2::dmMeshTriangle>(builder.triangles.size());
			std::memcpy(result.triangles, builder.triangles.data(), builder.triangles.size() * sizeof(zonetool::s2::dmMeshTriangle));

			result.treeHeight = builder.height;
			result.surfaceArea = builder.area();

			ZONETOOL_INFO("PhysWorld mesh: %d vertices, %d triangles, %d nodes, height %d", mesh.m_vertexCount,
				result.triangleCount, result.nodeCount, builder.height);
			return result;
		}

		zonetool::s2::PhysBrushModel convert_model(const PhysBrushModel& model, const std::size_t mesh_count)
		{
			zonetool::s2::PhysBrushModel new_model{-1, 0, 0xFFFF}; // 0x631F89
			const auto mesh = model.fields.meshIndex;
			if (mesh >= 0 && static_cast<std::size_t>(mesh) < mesh_count)
			{
				new_model.meshIndex = static_cast<unsigned short>(mesh);
			}

			return new_model;
		}
	}

	// TODO polytopes and water volumes are not converted
	void dump(PhysWorld* asset)
	{
		if (asset->polytopeCount || asset->waterVolumeDefCount)
		{
			ZONETOOL_WARNING("PhysWorld \"%s\": %u polytopes and %u water volumes are not converted",
				asset->name, asset->polytopeCount, asset->waterVolumeDefCount);
		}

		utils::memory::allocator allocator;

		std::vector<zonetool::s2::PhysMeshData> meshes;
		for (auto i = 0u; asset->meshDatas && i < asset->meshDataCount; i++)
		{
			meshes.push_back(convert_mesh(asset->meshDatas[i], allocator));
		}

		std::vector<zonetool::s2::PhysBrushModel> models(asset->brushModelCount);
		for (auto i = 0u; asset->brushModels && i < asset->brushModelCount; i++)
		{
			models[i] = convert_model(asset->brushModels[i], meshes.size());
		}

		zonetool::s2::PhysWorld new_asset{};
		new_asset.name = asset->name;
		new_asset.modelCount = asset->brushModelCount;
		new_asset.models = models.empty() ? nullptr : models.data();
		new_asset.meshCount = static_cast<unsigned int>(meshes.size());
		new_asset.meshes = meshes.empty() ? nullptr : meshes.data();
		zonetool::s2::phys_worldmap::dump(&new_asset);
		add_csv_row("phys_worldmap", new_asset.name);
	}
}
