#include <std_include.hpp>

#include "../include.hpp"
#include "physworld.hpp"

namespace zonetool::h1::converter::s2::physworld
{
	namespace
	{
		constexpr auto node_padding = 1.0f / 2.54f;
		constexpr auto solid_collision_flags = 0xFFFFu;

		constexpr std::uint32_t contents_solid = 0x1;
		constexpr std::uint32_t contents_glass = 0x10;
		constexpr std::uint32_t contents_water = 0x20;
		constexpr std::uint32_t contents_canshootclip = 0x40;
		constexpr std::uint32_t contents_missileclip = 0x80;
		constexpr std::uint32_t contents_vehicleclip = 0x200;
		constexpr std::uint32_t contents_itemclip = 0x400;
		constexpr std::uint32_t contents_ai_nosight = 0x1000;
		constexpr std::uint32_t contents_clipshot = 0x2000;
		constexpr std::uint32_t contents_playerclip = 0x10000;
		constexpr std::uint32_t contents_monsterclip = 0x20000;

		constexpr std::array<std::pair<std::uint32_t, std::uint32_t>, 11> contents_collision_flags
		{{
			{contents_solid, solid_collision_flags},
			{contents_glass, solid_collision_flags},
			{contents_water, 0x100},
			{contents_canshootclip, 0x2000},
			{contents_ai_nosight, 0x2000},
			{contents_missileclip, 0x180},
			{contents_clipshot, 0x180},
			{contents_vehicleclip, 0x10},
			{contents_itemclip, 0x80E},
			{contents_playerclip, 0x600},
			{contents_monsterclip, 0x600},
		}};

		int convert_collision_flags(const int contents)
		{
			auto flags = 0u;
			for (const auto& [bit, collision_flags] : contents_collision_flags)
			{
				if (contents & bit)
				{
					flags |= collision_flags;
				}
			}

			return static_cast<int>(flags ? flags : solid_collision_flags);
		}

		struct mesh_builder
		{
			const dmMeshData& source;
			std::vector<zonetool::s2::dmFloat4> vertices;
			std::vector<zonetool::s2::dmMeshTriangle> triangles;
			std::vector<zonetool::s2::PhysMeshNode> nodes;
			std::vector<std::array<float, 3>> centers;
			std::vector<std::size_t> order;
			int height = 0;

			explicit mesh_builder(const dmMeshData& mesh)
				: source(mesh)
			{
				// h1 stores vertices relative to the mesh center, s2 stores them in model space
				this->vertices.resize(mesh.m_vertexCount);
				for (auto i = 0; i < mesh.m_vertexCount; i++)
				{
					const auto& v = mesh.m_aVertices[i];
					this->vertices[i] = {v.x + mesh.m_center.x, v.y + mesh.m_center.y, v.z + mesh.m_center.z, v.w};
				}

				const auto count = static_cast<std::size_t>(mesh.m_triangleCount);
				this->centers.resize(count);
				this->order.resize(count);
				for (auto i = 0u; i < count; i++)
				{
					this->order[i] = i;
					const auto& tri = mesh.m_aTriangles[i];
					for (auto axis = 0; axis < 3; axis++)
					{
						this->centers[i][axis] = (this->vertex(tri.i1, axis) + this->vertex(tri.i2, axis) + this->vertex(tri.i3, axis)) / 3.0f;
					}
				}

				if (count)
				{
					this->build(0, count, 1);
				}
			}

			float vertex(const int index, const int axis) const
			{
				const auto& v = this->vertices[index];
				return axis == 0 ? v.x : axis == 1 ? v.y : v.z;
			}

			std::size_t build(const std::size_t begin, const std::size_t end, const int depth)
			{
				this->height = std::max(this->height, depth);

				const auto index = this->nodes.size();
				this->nodes.emplace_back();

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
					const auto& tri = this->source.m_aTriangles[this->order[i]];
					flags |= convert_collision_flags(tri.collisionFlags);
					for (auto axis = 0; axis < 3; axis++)
					{
						for (const auto vert : {tri.i1, tri.i2, tri.i3})
						{
							node.mins[axis] = std::min(node.mins[axis], this->vertex(vert, axis) - node_padding);
							node.maxs[axis] = std::max(node.maxs[axis], this->vertex(vert, axis) + node_padding);
						}

						center_mins[axis] = std::min(center_mins[axis], this->centers[this->order[i]][axis]);
						center_maxs[axis] = std::max(center_maxs[axis], this->centers[this->order[i]][axis]);
					}
				}

				const auto count = end - begin;
				if (count <= 4)
				{
					node.u.firstTriangle = static_cast<unsigned int>(this->triangles.size());
					node.flags = flags;
					node.axis = 3;
					node.triangleCount = static_cast<unsigned int>(count);
					for (auto i = begin; i < end; i++)
					{
						auto& tri = this->triangles.emplace_back(std::bit_cast<zonetool::s2::dmMeshTriangle>(this->source.m_aTriangles[this->order[i]]));
						tri.materialIndex = static_cast<int>(convert_surface_type(tri.materialIndex));
						tri.collisionFlags = convert_collision_flags(tri.collisionFlags);
					}

					this->nodes[index] = node;
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
				std::nth_element(this->order.begin() + begin, this->order.begin() + middle, this->order.begin() + end, [&](const auto lhs, const auto rhs)
				{
					return this->centers[lhs][axis] < this->centers[rhs][axis];
				});

				this->build(begin, middle, depth + 1);
				const auto right = this->build(middle, end, depth + 1);

				node.u.rightChildOffset = static_cast<unsigned int>(right - index);
				node.flags = flags;
				node.axis = axis;
				this->nodes[index] = node;
				return index;
			}

			float area() const
			{
				auto total = 0.0f;
				for (const auto& tri : this->triangles)
				{
					float edge[2][3]{};
					for (auto axis = 0; axis < 3; axis++)
					{
						edge[0][axis] = this->vertex(tri.i2, axis) - this->vertex(tri.i1, axis);
						edge[1][axis] = this->vertex(tri.i3, axis) - this->vertex(tri.i1, axis);
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
			result.vertices = allocator.allocate_array<zonetool::s2::dmFloat4>(builder.vertices.size());
			std::memcpy(result.vertices, builder.vertices.data(), builder.vertices.size() * sizeof(zonetool::s2::dmFloat4));
			result.triangles = allocator.allocate_array<zonetool::s2::dmMeshTriangle>(builder.triangles.size());
			std::memcpy(result.triangles, builder.triangles.data(), builder.triangles.size() * sizeof(zonetool::s2::dmMeshTriangle));

			result.treeHeight = builder.height;
			result.surfaceArea = builder.area();

			ZONETOOL_INFO("PhysWorld mesh: %d vertices, %d triangles, %d nodes, height %d", mesh.m_vertexCount,
				result.triangleCount, result.nodeCount, builder.height);
			return result;
		}

		zonetool::s2::PhysWaterVolumeDef convert_water_volume(const PhysWaterVolumeDef& water)
		{
			zonetool::s2::PhysWaterVolumeDef result{};
			result.preset = reinterpret_cast<zonetool::s2::PhysWaterPreset*>(water.physWaterPreset);
			std::memcpy(result.origin, water.origin, sizeof(result.origin));
			result.name = static_cast<zonetool::s2::scr_string_t>(water.string);
			result.brushModelIndex = water.brushModelIndex;
			return result;
		}

		zonetool::s2::PhysBrushModel convert_model(const PhysBrushModel& model, const std::size_t mesh_count, const std::size_t polytope_count)
		{
			zonetool::s2::PhysBrushModel new_model{-1, 0, 0xFFFF};
			const auto mesh = model.fields.meshIndex;
			if (mesh >= 0 && static_cast<std::size_t>(mesh) < mesh_count)
			{
				new_model.meshIndex = static_cast<unsigned short>(mesh);
			}

			const auto polytope = model.fields.polytopeIndex;
			if (polytope >= 0 && model.fields.polytopeCount > 0 && static_cast<std::size_t>(polytope + model.fields.polytopeCount) <= polytope_count)
			{
				new_model.polytopeStart = polytope;
				new_model.polytopeCount = static_cast<unsigned short>(model.fields.polytopeCount);
			}

			return new_model;
		}

		zonetool::s2::PhysWorld* convert(PhysWorld* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::s2::PhysWorld>();
			new_asset->name = asset->name;

			new_asset->waterCount = asset->waterVolumeDefs ? asset->waterVolumeDefCount : 0;
			new_asset->waters = new_asset->waterCount ? allocator.allocate_array<zonetool::s2::PhysWaterVolumeDef>(new_asset->waterCount) : nullptr;
			for (auto i = 0u; i < new_asset->waterCount; i++)
			{
				new_asset->waters[i] = convert_water_volume(asset->waterVolumeDefs[i]);
			}

			new_asset->polytopeCount = asset->polytopeDatas ? asset->polytopeCount : 0;
			new_asset->polytopes = new_asset->polytopeCount ? allocator.allocate_array<zonetool::s2::dmPolytopeData>(new_asset->polytopeCount) : nullptr;
			for (auto i = 0u; i < new_asset->polytopeCount; i++)
			{
				new_asset->polytopes[i] = convert_polytope(asset->polytopeDatas[i]);
			}

			new_asset->meshCount = asset->meshDatas ? asset->meshDataCount : 0;
			new_asset->meshes = new_asset->meshCount ? allocator.allocate_array<zonetool::s2::PhysMeshData>(new_asset->meshCount) : nullptr;
			for (auto i = 0u; i < new_asset->meshCount; i++)
			{
				new_asset->meshes[i] = convert_mesh(asset->meshDatas[i], allocator);
			}

			new_asset->modelCount = asset->brushModelCount;
			new_asset->models = new_asset->modelCount ? allocator.allocate_array<zonetool::s2::PhysBrushModel>(new_asset->modelCount) : nullptr;
			for (auto i = 0u; asset->brushModels && i < new_asset->modelCount; i++)
			{
				new_asset->models[i] = convert_model(asset->brushModels[i], new_asset->meshCount, new_asset->polytopeCount);
			}

			return new_asset;
		}
	}

	zonetool::s2::dmPolytopeData convert_polytope(const dmPolytopeData& polytope)
	{
		zonetool::s2::dmPolytopeData result{};
		result.vertices = retype<zonetool::s2::dmFloat4>(polytope.m_aVertices);
		result.planes = retype<zonetool::s2::dmPlane>(polytope.m_aPlanes);
		result.subEdges = retype<zonetool::s2::dmSubEdge>(polytope.m_aSubEdges);
		result.faceSubEdges = polytope.m_aFaceSubEdges;
		result.centroid[0] = polytope.m_centroid.x;
		result.centroid[1] = polytope.m_centroid.y;
		result.centroid[2] = polytope.m_centroid.z;
		result.vertexCount = polytope.m_vertexCount;
		result.faceCount = polytope.m_faceCount;
		result.subEdgeCount = polytope.m_subEdgeCount;
		result.volume = polytope.m_volume;
		result.area = polytope.m_area;
		result.inertiaMoments[0] = polytope.m_inertiaMoments.x;
		result.inertiaMoments[1] = polytope.m_inertiaMoments.y;
		result.inertiaMoments[2] = polytope.m_inertiaMoments.z;
		result.inertiaProducts[0] = polytope.m_inertiaProducts.x;
		result.inertiaProducts[1] = polytope.m_inertiaProducts.y;
		result.inertiaProducts[2] = polytope.m_inertiaProducts.z;
		result.collisionFlags = static_cast<unsigned int>(convert_collision_flags(polytope.contents));

		const auto surface_type = polytope.m_surfaceTypes && polytope.m_faceCount ? polytope.m_surfaceTypes[0] : 0;
		result.surfaceType = convert_surface_type(surface_type);
		return result;
	}

	void dump(PhysWorld* asset)
	{
		utils::memory::allocator allocator;
		const auto new_asset = convert(asset, allocator);
		zonetool::s2::phys_worldmap::dump(new_asset);
		add_csv_row("phys_worldmap", new_asset->name);
	}

	void dump_water_preset(PhysWaterPreset* asset)
	{
		// TODO: map H1 unknownParams, until then this uses stock S2 water_default
		constexpr std::array<float, 8> stock_unknown_params{0.0f, 0.0f, 0.0f, 0.036127292f, 0.25f, 0.0f, 0.02f, 0.0f};

		utils::memory::allocator allocator;
		zonetool::s2::PhysWaterPreset new_asset{};
		new_asset.name = asset->name;
		std::copy(stock_unknown_params.begin(), stock_unknown_params.end(), new_asset.unknownParams);
		std::memcpy(new_asset.sharedParams, asset->sharedParams, sizeof(new_asset.sharedParams));
		new_asset.sharedFlags = asset->sharedFlags;

		const FxEffectDef* fx[]{asset->fx0, asset->fx1, asset->fx2, asset->fx3, asset->fx4, asset->fx5, asset->fx6};
		for (auto i = 0u; i < std::size(fx); i++)
		{
			new_asset.fx[i] = fx[i] ? make_stub<zonetool::s2::FxEffectDef>(fx[i]->name, allocator) : nullptr;
		}

		zonetool::s2::phys_water_preset::dump(&new_asset);
		add_csv_row(zonetool::s2::type_to_string(zonetool::s2::ASSET_TYPE_PHYSWATERPRESET), asset->name);
	}
}
