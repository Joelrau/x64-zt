#include <std_include.hpp>

#include "../include.hpp"
#include "physworld.hpp"

#include <h1/assets/physworld.hpp>

namespace zonetool::s2::converter::h1::physworld
{
	namespace
	{
		using point_t = std::array<float, 3>;

		struct tree_node
		{
			point_t lower;
			point_t upper;
			unsigned int axis;
			unsigned int triangle_count;
			unsigned int index;
		};

		constexpr std::array<std::pair<std::uint32_t, std::uint32_t>, 6> collision_flags_contents
		{{
			{0x100, zonetool::h1::CONTENTS_WATER},
			{0x2000, zonetool::h1::CONTENTS_CANSHOOTCLIP | zonetool::h1::CONTENTS_AI_NOSIGHT},
			{0x180, zonetool::h1::CONTENTS_MISSILECLIP | zonetool::h1::CONTENTS_CLIPSHOT},
			{0x10, zonetool::h1::CONTENTS_VEHICLECLIP},
			{0x80E, zonetool::h1::CONTENTS_ITEMCLIP},
			{0x600, zonetool::h1::CONTENTS_PLAYERCLIP | zonetool::h1::CONTENTS_MONSTERCLIP},
		}};

		int convert_contents(const std::uint32_t collision_flags)
		{
			const auto mask = collision_flags & 0xFFFF;
			if (mask == 0xFFFF || !mask)
			{
				return zonetool::h1::CONTENTS_SOLID;
			}

			auto contents = 0u;
			for (const auto& [bits, h1_contents] : collision_flags_contents)
			{
				if ((mask & bits) == bits)
				{
					contents |= h1_contents;
				}
			}

			return static_cast<int>(contents ? contents : zonetool::h1::CONTENTS_SOLID);
		}

		point_t vertex(const PhysMeshData& mesh, const int index)
		{
			const auto& v = mesh.vertices[index];
			return {v.x, v.y, v.z};
		}

		unsigned int build_tree(const PhysMeshData& mesh, const std::vector<point_t>& centroids, std::vector<int>& order, const int begin, const int end,
			std::vector<tree_node>& nodes, const int depth, int& max_depth)
		{
			const auto node_index = static_cast<unsigned int>(nodes.size());
			nodes.emplace_back();
			max_depth = std::max(max_depth, depth);

			point_t lower{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
			point_t upper{-lower[0], -lower[1], -lower[2]};
			auto centroid_lower = lower;
			auto centroid_upper = upper;
			for (auto i = begin; i < end; i++)
			{
				const auto& tri = mesh.triangles[order[i]];
				for (const auto vert : {tri.i1, tri.i2, tri.i3})
				{
					const auto position = vertex(mesh, vert);
					for (auto axis = 0; axis < 3; axis++)
					{
						lower[axis] = std::min(lower[axis], position[axis]);
						upper[axis] = std::max(upper[axis], position[axis]);
					}
				}

				for (auto axis = 0; axis < 3; axis++)
				{
					centroid_lower[axis] = std::min(centroid_lower[axis], centroids[order[i]][axis]);
					centroid_upper[axis] = std::max(centroid_upper[axis], centroids[order[i]][axis]);
				}
			}

			nodes[node_index].lower = lower;
			nodes[node_index].upper = upper;
			if (end - begin <= 15)
			{
				nodes[node_index].axis = 3u;
				nodes[node_index].triangle_count = static_cast<unsigned int>(end - begin);
				nodes[node_index].index = static_cast<unsigned int>(begin);
				return node_index;
			}

			auto split_axis = 0u;
			for (auto axis = 1u; axis < 3; axis++)
			{
				if (centroid_upper[axis] - centroid_lower[axis] > centroid_upper[split_axis] - centroid_lower[split_axis])
				{
					split_axis = axis;
				}
			}

			const auto middle = begin + (end - begin) / 2;
			std::nth_element(order.begin() + begin, order.begin() + middle, order.begin() + end, [&](const int a, const int b)
			{
				return centroids[a][split_axis] < centroids[b][split_axis];
			});

			nodes[node_index].axis = split_axis;
			build_tree(mesh, centroids, order, begin, middle, nodes, depth + 1, max_depth);
			nodes[node_index].index = build_tree(mesh, centroids, order, middle, end, nodes, depth + 1, max_depth) - node_index;
			return node_index;
		}

		zonetool::h1::dmMeshData convert_mesh(const PhysMeshData& mesh, utils::memory::allocator& allocator)
		{
			zonetool::h1::dmMeshData data{};
			if (!mesh.triangleCount || !mesh.triangles || !mesh.vertices)
			{
				return data;
			}

			std::vector<int> order(mesh.triangleCount);
			std::vector<point_t> centroids(mesh.triangleCount);
			for (auto i = 0; i < mesh.triangleCount; i++)
			{
				order[i] = i;
				const auto& tri = mesh.triangles[i];
				for (const auto vert : {tri.i1, tri.i2, tri.i3})
				{
					const auto position = vertex(mesh, vert);
					for (auto axis = 0; axis < 3; axis++)
					{
						centroids[i][axis] += position[axis] / 3.0f;
					}
				}
			}

			std::vector<tree_node> nodes;
			auto max_depth = 0;
			build_tree(mesh, centroids, order, 0, mesh.triangleCount, nodes, 1, max_depth);

			point_t center{};
			point_t extents{};
			point_t unquantize{};
			for (auto axis = 0; axis < 3; axis++)
			{
				center[axis] = (nodes[0].lower[axis] + nodes[0].upper[axis]) * 0.5f;
				extents[axis] = (nodes[0].upper[axis] - nodes[0].lower[axis]) * 0.5f + 0.5f;
				unquantize[axis] = extents[axis] / 32766.0f;
			}

			data.m_center = {center[0], center[1], center[2]};
			data.m_extents = {extents[0], extents[1], extents[2]};
			data.m_unquantize = {unquantize[0], unquantize[1], unquantize[2]};
			data.m_height = max_depth;

			data.m_vertexCount = mesh.vertexCount;
			data.m_aVertices = allocator.allocate_array<zonetool::h1::dmFloat4>(mesh.vertexCount);
			for (auto i = 0; i < mesh.vertexCount; i++)
			{
				const auto& v = mesh.vertices[i];
				data.m_aVertices[i] = {v.x - center[0], v.y - center[1], v.z - center[2], 0.0f};
			}

			data.m_triangleCount = mesh.triangleCount;
			data.m_aTriangles = allocator.allocate_array<zonetool::h1::dmMeshTriangle>(mesh.triangleCount);
			for (auto i = 0; i < mesh.triangleCount; i++)
			{
				const auto& tri = mesh.triangles[order[i]];
				auto& out = data.m_aTriangles[i];
				out = std::bit_cast<zonetool::h1::dmMeshTriangle>(tri);
				out.materialIndex = static_cast<int>(convert_surface_type(tri.materialIndex));
				out.collisionFlags = convert_contents(tri.collisionFlags);
				data.contents |= out.collisionFlags;
			}

			const auto quantize = [&](const float value, const int axis, const bool upper)
			{
				const auto relative = value - center[axis];
				auto steps = upper ? std::ceil(relative / unquantize[axis]) : std::floor(relative / unquantize[axis]);
				if (upper ? steps * unquantize[axis] < relative : steps * unquantize[axis] > relative)
				{
					steps += upper ? 1.0f : -1.0f;
				}

				return static_cast<zonetool::h1::dm_int16>(std::clamp(steps, -32767.0f, 32767.0f));
			};

			data.m_nodeCount = static_cast<int>(nodes.size());
			data.m_pRoot = allocator.allocate_array<zonetool::h1::dmMeshNode>(nodes.size());
			for (auto i = 0u; i < nodes.size(); i++)
			{
				auto& out = data.m_pRoot[i];
				out.lowerX = quantize(nodes[i].lower[0], 0, false);
				out.lowerY = quantize(nodes[i].lower[1], 1, false);
				out.lowerZ = quantize(nodes[i].lower[2], 2, false);
				out.upperX = quantize(nodes[i].upper[0], 0, true);
				out.upperY = quantize(nodes[i].upper[1], 1, true);
				out.upperZ = quantize(nodes[i].upper[2], 2, true);
				out.anon.fields.axis = nodes[i].axis;
				out.anon.fields.triangleCount = nodes[i].triangle_count;
				out.anon.fields.index = nodes[i].index;
			}

			return data;
		}

		zonetool::h1::PhysWorld* convert(PhysWorld* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::h1::PhysWorld>();
			new_asset->name = asset->name;

			new_asset->polytopeCount = asset->polytopes ? asset->polytopeCount : 0;
			new_asset->polytopeDatas = allocator.allocate_array<zonetool::h1::dmPolytopeData>(std::max(new_asset->polytopeCount, 1u));
			for (auto i = 0u; i < new_asset->polytopeCount; i++)
			{
				new_asset->polytopeDatas[i] = convert_polytope(asset->polytopes[i], allocator);
			}

			new_asset->meshDataCount = asset->meshes ? asset->meshCount : 0;
			new_asset->meshDatas = allocator.allocate_array<zonetool::h1::dmMeshData>(std::max(new_asset->meshDataCount, 1u));
			auto triangles = 0;
			for (auto i = 0u; i < new_asset->meshDataCount; i++)
			{
				new_asset->meshDatas[i] = convert_mesh(asset->meshes[i], allocator);
				triangles += new_asset->meshDatas[i].m_triangleCount;
			}

			new_asset->brushModelCount = asset->models ? asset->modelCount : 0;
			new_asset->brushModels = allocator.allocate_array<zonetool::h1::PhysBrushModel>(std::max(new_asset->brushModelCount, 1u));
			for (auto i = 0u; i < new_asset->brushModelCount; i++)
			{
				const auto& model = asset->models[i];
				auto& fields = new_asset->brushModels[i].fields;
				const auto has_polytopes = model.polytopeCount && model.polytopeStart >= 0 && model.polytopeStart + model.polytopeCount <= static_cast<int>(new_asset->polytopeCount);
				fields.polytopeIndex = has_polytopes ? static_cast<short>(model.polytopeStart) : -1;
				fields.unk = -1;
				fields.polytopeCount = has_polytopes ? static_cast<short>(model.polytopeCount) : 0;
				fields.meshIndex = model.meshIndex < new_asset->meshDataCount ? static_cast<short>(model.meshIndex) : -1;
			}

			if (asset->waterCount) // TODO: water volumes need a PhysWaterPreset
			{
				ZONETOOL_WARNING("PhysWorld \"%s\": %u water volumes are not converted", asset->name, asset->waterCount);
			}

			ZONETOOL_INFO("PhysWorld \"%s\": %u meshes (%d triangles), %u polytopes, %u brush models", asset->name, new_asset->meshDataCount, triangles,
				new_asset->polytopeCount, new_asset->brushModelCount);
			return new_asset;
		}
	}

	zonetool::h1::dmPolytopeData convert_polytope(const dmPolytopeData& polytope, utils::memory::allocator& allocator)
	{
		zonetool::h1::dmPolytopeData result{};
		result.m_aVertices = reinterpret_cast<zonetool::h1::dmFloat4*>(polytope.vertices);
		result.m_aPlanes = reinterpret_cast<zonetool::h1::dmPlane*>(polytope.planes);
		result.m_aSubEdges = reinterpret_cast<zonetool::h1::dmSubEdge*>(polytope.subEdges);
		result.m_aFaceSubEdges = polytope.faceSubEdges;
		result.m_centroid = {polytope.centroid[0], polytope.centroid[1], polytope.centroid[2]};
		result.m_vertexCount = polytope.vertexCount;
		result.m_faceCount = polytope.faceCount;
		result.m_subEdgeCount = polytope.subEdgeCount;
		result.m_volume = polytope.volume;
		result.m_area = polytope.area;
		result.m_inertiaMoments = {polytope.inertiaMoments[0], polytope.inertiaMoments[1], polytope.inertiaMoments[2]};
		result.m_inertiaProducts = {polytope.inertiaProducts[0], polytope.inertiaProducts[1], polytope.inertiaProducts[2]};
		result.contents = convert_contents(polytope.collisionFlags);

		result.m_surfaceTypes = allocator.allocate_array<zonetool::h1::dm_uint16>(std::max(polytope.faceCount, 1));
		std::fill_n(result.m_surfaceTypes, polytope.faceCount, static_cast<zonetool::h1::dm_uint16>(convert_surface_type(polytope.surfaceType)));
		result.m_vertexMaterials = allocator.allocate_array<zonetool::h1::dm_uint16>(std::max(polytope.vertexCount, 1));
		return result;
	}

	void dump(PhysWorld* asset)
	{
		utils::memory::allocator allocator;
		const auto new_asset = convert(asset, allocator);
		zonetool::h1::phys_world::dump(new_asset);
		add_csv_row("phys_worldmap", new_asset->name);
	}
}
