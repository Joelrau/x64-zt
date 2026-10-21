#include <std_include.hpp>

#include "../include.hpp"
#include "xsurface.hpp"

#include <utils/cryptography.hpp>

namespace zonetool::h1::converter::s2::xsurface
{
	namespace
	{
		constexpr std::uint16_t base_streams = 0x7E0;
		constexpr std::uint16_t skin_stream = 0x1000;
		constexpr std::uint16_t lightmap_uv_stream = 0x2000;
		constexpr std::uint16_t wind_stream = 0x800;
		constexpr std::uint32_t h1_bone_offset_scale = 64;

		std::vector<XModelSurfs*> pending_surfs;
		std::unordered_map<const XSurface*, std::uint32_t> base_vert_indices;

		struct vertex_data
		{
			char* data;
			std::uint32_t size;
		};

		bool has_vertex_color(const XSurface* surface)
		{
			return (surface->flags & (SURF_FLAG_VERTCOL_NONE | SURF_FLAG_REACTIVE_MOTION)) == 0;
		}

		bool has_lightmap_uv(const XSurface* surface)
		{
			return (surface->flags & SURF_FLAG_LIGHTMAP_COORDS) && surface->lmapUnwrap && (surface->flags & SURF_FLAG_SKINNED) == 0;
		}

		bool has_wind_stream(const XSurface* surface)
		{
			return (surface->flags & SURF_FLAG_REACTIVE_MOTION) != 0;
		}

		bool is_skinned(const XSurface* surface)
		{
			return (surface->flags & SURF_FLAG_SKINNED) != 0;
		}

		std::uint32_t pack_wind(const GfxPackedVertex& vertex, const float max_weight)
		{
			// stock weeds: x flutter 0 or 255, y phase 55..255, z vertical flutter 0 or 204, w 255; h1 keeps the weight in |binormalSign|
			const auto weight = max_weight > 0.0f ? std::clamp(std::abs(vertex.binormalSign) / max_weight, 0.0f, 1.0f) : 0.0f;
			constexpr auto phase_cell = 8.0f;
			auto hash = 2166136261u;
			for (const auto coord : {vertex.xyz[0], vertex.xyz[1]})
			{
				hash = (hash ^ static_cast<std::uint32_t>(static_cast<std::int32_t>(std::floor(coord / phase_cell)))) * 16777619u;
			}

			const auto flutter = static_cast<std::uint32_t>(std::lround(weight * 255.0f));
			const auto phase = 55u + hash % 201u;
			const auto bounce = static_cast<std::uint32_t>(std::lround(weight * 204.0f));
			return flutter | (phase << 8) | (bounce << 16) | (255u << 24);
		}

		std::uint32_t pack_tangent(const GfxPackedVertex& vertex)
		{
			// the top two bits are set when the binormal is -cross(normal, tangent), like every stock s2 surface
			return (vertex.tangent.packed & ~0xC0000000u) | (vertex.binormalSign < 0.0f ? 0xC0000000u : 0u);
		}

		std::uint32_t pack_sky_visibility(const std::uint32_t packed_normal)
		{
			// octahedral bent normal in bits 0-17, visibility in 18-24, cone aperture in 25-31
			float n[3]{};
			for (auto i = 0; i < 3; i++)
			{
				n[i] = static_cast<float>((packed_normal >> (10 * i)) & 1023) / 1023.0f * 2.0f - 1.0f;
			}

			const auto sum = std::abs(n[0]) + std::abs(n[1]) + std::abs(n[2]);
			auto x = sum > 0.0f ? n[0] / sum : 0.0f;
			auto y = sum > 0.0f ? n[1] / sum : 0.0f;
			if (n[2] < 0.0f)
			{
				const auto folded_x = (1.0f - std::abs(y)) * (x >= 0.0f ? 1.0f : -1.0f);
				const auto folded_y = (1.0f - std::abs(x)) * (y >= 0.0f ? 1.0f : -1.0f);
				x = folded_x;
				y = folded_y;
			}

			const auto quantize = [](const float value)
			{
				return static_cast<std::uint32_t>(std::lround(std::clamp(value * 0.5f + 0.5f, 0.0f, 1.0f) * 511.0f));
			};

			return quantize(x) | (quantize(y) << 9) | (127u << 18);
		}

		void convert_skin(const XSurface* surface, zonetool::s2::XSurfaceSkinVertex* skin)
		{
			const auto* blend = surface->blendVerts;
			auto vertex = 0u;
			for (auto bone_count = 1u; bone_count <= 8; bone_count++)
			{
				for (auto i = 0; i < surface->blendVertCounts[bone_count - 1] && vertex < surface->vertCount; i++)
				{
					auto& vertex_skin = skin[vertex++];
					vertex_skin.count = static_cast<std::uint16_t>(std::min(bone_count, 4u));
					vertex_skin.bones[0] = static_cast<std::uint16_t>(blend[0] / h1_bone_offset_scale);
					for (auto bone = 1u; bone < vertex_skin.count; bone++)
					{
						vertex_skin.bones[bone] = static_cast<std::uint16_t>(blend[bone * 2 - 1] / h1_bone_offset_scale);
						vertex_skin.weights[bone - 1] = blend[bone * 2];
					}

					blend += bone_count * 2 - 1;
				}
			}
		}

		vertex_data build_vertex_data(const XSurface* surface, utils::memory::allocator& allocator)
		{
			const auto vert_count = static_cast<std::size_t>(surface->vertCount);
			const auto* verts = surface->verts0.packedVerts0;

			std::size_t size = 0;
			const auto take = [&](const std::size_t bytes)
			{
				size = (size + 7) & ~7ull;
				const auto offset = size;
				size += bytes;
				return offset;
			};

			const auto faces_offset = take(surface->triCount * sizeof(Face));
			const auto positions_offset = take(vert_count * sizeof(float[3]));
			const auto sky_visibility_offset = take(vert_count * sizeof(std::uint32_t));
			const auto colors_offset = take(vert_count * sizeof(std::uint32_t));
			const auto tex_coords_offset = take(vert_count * sizeof(std::uint32_t));
			const auto normals_offset = take(vert_count * sizeof(std::uint32_t));
			const auto tangents_offset = take(vert_count * sizeof(std::uint32_t));
			const auto lightmap_offset = has_lightmap_uv(surface) ? take(vert_count * sizeof(std::uint32_t)) : 0;
			const auto wind_offset = has_wind_stream(surface) ? take(vert_count * sizeof(std::uint32_t)) : 0;
			const auto skin_offset = is_skinned(surface) ? take(vert_count * sizeof(zonetool::s2::XSurfaceSkinVertex)) : 0;

			const auto data = allocator.allocate_array<char>(size);
			std::memcpy(data + faces_offset, surface->triIndices, surface->triCount * sizeof(Face));

			const auto positions = reinterpret_cast<float*>(data + positions_offset);
			const auto sky_visibility = reinterpret_cast<std::uint32_t*>(data + sky_visibility_offset);
			const auto colors = reinterpret_cast<std::uint32_t*>(data + colors_offset);
			const auto tex_coords = reinterpret_cast<std::uint32_t*>(data + tex_coords_offset);
			const auto normals = reinterpret_cast<std::uint32_t*>(data + normals_offset);
			const auto tangents = reinterpret_cast<std::uint32_t*>(data + tangents_offset);
			const auto use_color = has_vertex_color(surface);
			for (auto i = 0u; i < vert_count; i++)
			{
				std::memcpy(&positions[i * 3], verts[i].xyz, sizeof(verts[i].xyz));
				sky_visibility[i] = pack_sky_visibility(verts[i].normal.packed);
				colors[i] = use_color ? verts[i].color.packed : 0xFFFFFFFF;
				tex_coords[i] = verts[i].texCoord.packed;
				normals[i] = verts[i].normal.packed;
				tangents[i] = pack_tangent(verts[i]);
			}

			if (has_lightmap_uv(surface))
			{
				std::memcpy(data + lightmap_offset, surface->lmapUnwrap, vert_count * sizeof(std::uint32_t));
			}

			if (has_wind_stream(surface))
			{
				auto max_weight = 0.0f;
				for (auto i = 0u; i < vert_count; i++)
				{
					max_weight = std::max(max_weight, std::abs(verts[i].binormalSign));
				}

				const auto wind = reinterpret_cast<std::uint32_t*>(data + wind_offset);
				for (auto i = 0u; i < vert_count; i++)
				{
					wind[i] = pack_wind(verts[i], max_weight);
				}
			}

			if (is_skinned(surface))
			{
				convert_skin(surface, reinterpret_cast<zonetool::s2::XSurfaceSkinVertex*>(data + skin_offset));
			}

			return {data, static_cast<std::uint32_t>(size)};
		}

		zonetool::s2::XSurfaceShared* convert_shared(const XSurface* surface, utils::memory::allocator& allocator)
		{
			const auto data = build_vertex_data(surface, allocator);
			const auto streams = static_cast<std::uint16_t>(base_streams
				| (is_skinned(surface) ? skin_stream : 0)
				| (has_lightmap_uv(surface) ? lightmap_uv_stream : 0)
				| (has_wind_stream(surface) ? wind_stream : 0));
			const auto hash = utils::cryptography::sha256::compute(std::string(data.data, data.data ? data.size : 0), true);

			const auto shared = allocator.allocate<zonetool::s2::XSurfaceShared>();
			shared->flags = static_cast<std::uint8_t>(streams);
			shared->__pad0 = static_cast<std::uint8_t>(streams >> 8);
			std::memcpy(shared->hash, hash.data(), sizeof(shared->hash));
			shared->indexCount = surface->triCount * 3u;
			shared->vertCount = surface->vertCount;
			shared->dataSize = data.size;
			shared->data = data.data;
			return shared;
		}

		zonetool::s2::XSurfaceCollisionLeaf* convert_collision_tree(const XSurfaceCollisionTree* tree, utils::memory::allocator& allocator)
		{
			if (!tree)
			{
				return nullptr;
			}

			const auto new_tree = allocator.allocate<zonetool::s2::XSurfaceCollisionLeaf>();
			std::memcpy(new_tree->trans, tree->trans, sizeof(new_tree->trans));
			std::memcpy(new_tree->scale, tree->scale, sizeof(new_tree->scale));
			new_tree->nodeCount = tree->nodeCount;
			new_tree->leafCount = tree->leafCount;
			new_tree->nodes = reinterpret_cast<zonetool::s2::XSurfaceCollisionNode*>(tree->nodes);
			new_tree->leafs = reinterpret_cast<unsigned short*>(tree->leafs);
			return new_tree;
		}

		void convert_rigid_vert_lists(const XSurface* surface, zonetool::s2::XSurface* new_surface, utils::memory::allocator& allocator)
		{
			if (!surface->rigidVertLists || !surface->rigidVertListCount)
			{
				return;
			}

			new_surface->rigidVertListCount = surface->rigidVertListCount;
			new_surface->rigidVertLists = allocator.allocate_array<zonetool::s2::XRigidVertList>(surface->rigidVertListCount);
			for (auto i = 0; i < surface->rigidVertListCount; i++)
			{
				const auto& list = surface->rigidVertLists[i];
				auto& new_list = new_surface->rigidVertLists[i];
				new_list.boneOffset = list.boneOffset;
				new_list.vertCount = list.vertCount;
				new_list.triOffset = list.triOffset;
				new_list.triCount = list.triCount;
				new_list.collisionTree = convert_collision_tree(list.collisionTree, allocator);
			}
		}

		zonetool::s2::vec4_t* convert_verts(const XSurface* surface, utils::memory::allocator& allocator)
		{
			const auto verts = allocator.allocate_array<zonetool::s2::vec4_t>(surface->vertCount);
			for (auto i = 0; i < surface->vertCount; i++)
			{
				const auto& vertex = surface->verts0.packedVerts0[i];
				std::memcpy(verts[i], vertex.xyz, sizeof(vertex.xyz));
				std::memcpy(&verts[i][3], &vertex.normal.packed, sizeof(std::uint32_t));
			}

			return verts;
		}

		void convert_surface(const XSurface* surface, const std::uint32_t base_vert_index, zonetool::s2::XSurface* new_surface,
			utils::memory::allocator& allocator)
		{
			new_surface->flags = zonetool::s2::XSURFACE_FLAG_UNK_0x20
				| (has_vertex_color(surface) ? 0 : zonetool::s2::XSURFACE_FLAG_VERTCOL_NONE)
				| (is_skinned(surface) ? zonetool::s2::XSURFACE_FLAG_SKINNED : 0)
				| (has_lightmap_uv(surface) ? zonetool::s2::XSURFACE_FLAG_LIGHTMAP_UV : 0)
				| (has_wind_stream(surface) ? zonetool::s2::XSURFACE_FLAG_EXTRA_STREAM : 0);
			new_surface->vertCount = surface->vertCount;
			new_surface->triCount = surface->triCount;
			new_surface->uvSetCount = 1;
			new_surface->shared = convert_shared(surface, allocator);
			new_surface->triIndices = reinterpret_cast<unsigned short*>(surface->triIndices);
			convert_rigid_vert_lists(surface, new_surface, allocator);
			new_surface->verts = convert_verts(surface, allocator);
			std::memcpy(new_surface->partBits, surface->partBits, sizeof(surface->partBits));
			new_surface->baseVertIndex = base_vert_index;
		}

		std::uint32_t next_base_vert_index(const std::uint32_t base_vert_index, const XSurface& surface)
		{
			// the s2 linker numbers the surfaces of all lods in one run with each count rounded up to even
			return base_vert_index + ((surface.vertCount + 1u) & ~1u);
		}

		void write(XModelSurfs* asset)
		{
			utils::memory::allocator allocator;
			const auto new_asset = allocator.allocate<zonetool::s2::XModelSurfs>();
			new_asset->name = asset->name;
			new_asset->numsurfs = asset->numsurfs;
			new_asset->surfs = allocator.allocate_array<zonetool::s2::XSurface>(asset->numsurfs);
			std::memcpy(new_asset->partBits, asset->partBits, sizeof(asset->partBits));
			new_asset->surfaceArea = surface_area(asset);

			auto own_base_vert_index = 0u;
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				const auto& surface = asset->surfs[i];
				const auto base = base_vert_indices.find(&surface);
				convert_surface(&surface, base != base_vert_indices.end() ? base->second : own_base_vert_index, &new_asset->surfs[i], allocator);
				own_base_vert_index = next_base_vert_index(own_base_vert_index, surface);
			}

			zonetool::s2::xmodel_surfs::dump(new_asset);
		}

		bool has_vertex_data(const XSurface& surface)
		{
			return surface.verts0.packedVerts0 && surface.triIndices && (!is_skinned(&surface) || surface.blendVerts);
		}

		struct area_sum
		{
			double log_area;
			std::uint32_t tris;
		};

		void add_triangle_areas(const XSurface* surface, area_sum& sum)
		{
			const auto* verts = surface->verts0.packedVerts0;
			for (auto i = 0; verts && surface->triIndices && i < surface->triCount; i++)
			{
				const auto& face = surface->triIndices[i];
				const auto* a = verts[face.v1].xyz;
				const auto* b = verts[face.v2].xyz;
				const auto* c = verts[face.v3].xyz;
				const double u[3]{b[0] - a[0], b[1] - a[1], b[2] - a[2]};
				const double v[3]{c[0] - a[0], c[1] - a[1], c[2] - a[2]};
				const double n[3]{u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2], u[0] * v[1] - u[1] * v[0]};
				const auto area = 0.5 * std::sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
				if (area > 0.0)
				{
					sum.log_area += std::log(area);
					sum.tris++;
				}
			}
		}
	}

	void dump(XModelSurfs* asset)
	{
		if (!asset->surfs || !asset->numsurfs)
		{
			return;
		}

		for (auto i = 0; i < asset->numsurfs; i++)
		{
			if (!has_vertex_data(asset->surfs[i]))
			{
				ZONETOOL_WARNING("XModelSurfs \"%s\" has no vertex data", asset->name);
				return;
			}
		}

		pending_surfs.emplace_back(asset);
		add_csv_row("xmodelsurfs", asset->name);
	}

	void assign_base_vert_indices(const XModel* model)
	{
		auto base_vert_index = 0u;
		for (auto i = 0; i < model->numLods && i < static_cast<int>(std::size(model->lodInfo)); i++)
		{
			const auto* surfs = model->lodInfo[i].modelSurfs;
			for (auto j = 0; surfs && surfs->surfs && j < surfs->numsurfs; j++)
			{
				const auto& surface = surfs->surfs[j];
				base_vert_indices.try_emplace(&surface, base_vert_index);
				base_vert_index = next_base_vert_index(base_vert_index, surface);
			}
		}
	}

	void finish()
	{
		for (auto* asset : pending_surfs)
		{
			write(asset);
		}

		pending_surfs.clear();
		base_vert_indices.clear();
	}

	float surface_area(const XModelSurfs* asset)
	{
		// the s2 linker stores the geometric mean of the triangle areas
		area_sum sum{};
		for (auto i = 0; asset && asset->surfs && i < asset->numsurfs; i++)
		{
			add_triangle_areas(&asset->surfs[i], sum);
		}

		return sum.tris ? static_cast<float>(std::exp(sum.log_area / sum.tris)) : 0.0f;
	}

	std::uint32_t triangle_count(const XModelSurfs* asset)
	{
		std::uint32_t count{};
		for (auto i = 0; asset && asset->surfs && i < asset->numsurfs; i++)
		{
			count += asset->surfs[i].triCount;
		}

		return count;
	}
}
