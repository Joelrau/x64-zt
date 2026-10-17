#include <std_include.hpp>

#include "../include.hpp"
#include "xsurface.hpp"

#include <h1/assets/xsurface.hpp>

namespace zonetool::s2::converter::h1::xsurface
{
	namespace
	{
		constexpr std::uint32_t h1_bone_offset_scale = 64;
		constexpr std::size_t max_blend_bones = 4;

		std::unordered_set<std::string> dumped_surfs;

		struct vertex_streams
		{
			const std::uint16_t* indices;
			const float* positions;
			const std::uint32_t* colors;
			const std::uint32_t* tex_coords;
			const std::uint32_t* normals;
			const std::uint32_t* tangents;
			const XSurfaceSkinVertex* skin;
		};

		std::optional<vertex_streams> map_vertex_streams(const XSurface* surface, const std::string& data)
		{
			const auto vert_count = static_cast<std::size_t>(surface->vertCount);
			std::size_t offset = 0;

			const auto take = [&](const std::size_t size) -> const char*
			{
				offset = (offset + 7) & ~7ull;
				const auto start = offset;
				offset += size;
				return data.data() + start;
			};

			vertex_streams streams{};
			streams.indices = reinterpret_cast<const std::uint16_t*>(take(surface->triCount * 6ull));
			streams.positions = reinterpret_cast<const float*>(take(vert_count * 12));
			take(vert_count * 4);
			streams.colors = reinterpret_cast<const std::uint32_t*>(take(vert_count * 4));
			streams.tex_coords = reinterpret_cast<const std::uint32_t*>(take(vert_count * 4));
			streams.normals = reinterpret_cast<const std::uint32_t*>(take(vert_count * 4));
			streams.tangents = reinterpret_cast<const std::uint32_t*>(take(vert_count * 4));

			if (surface->flags & XSURFACE_FLAG_LIGHTMAP_UV)
			{
				take(vert_count * 4);
			}

			if (surface->flags & XSURFACE_FLAG_EXTRA_STREAM)
			{
				take(vert_count * 4);
			}

			if (surface->flags & XSURFACE_FLAG_SKINNED)
			{
				streams.skin = reinterpret_cast<const XSurfaceSkinVertex*>(take(vert_count * sizeof(XSurfaceSkinVertex)));
			}

			if (offset > data.size())
			{
				return {};
			}

			return streams;
		}

		std::optional<std::string> read_vertex_data(const XSurfaceShared* shared)
		{
			if (!shared)
			{
				return {};
			}

			if (shared->flags & XSURFACE_SHARED_STREAMED)
			{
				return read_shared_stream(shared);
			}

			if (!shared->data)
			{
				return {};
			}

			return std::string(shared->data, shared->dataSize);
		}

		float binormal_sign(const std::uint32_t tangent)
		{
			return (tangent >> 30) == 0 ? -1.0f : 1.0f;
		}

		void convert_vertices(const XSurface* surface, const vertex_streams& streams, zonetool::h1::XSurface* new_surface,
			const bool keep_colors, const std::uint32_t forced_color_bits, utils::memory::allocator& allocator)
		{
			const auto use_colors = keep_colors && (surface->flags & XSURFACE_FLAG_VERTCOL_NONE) == 0;
			new_surface->verts0.packedVerts0 = allocator.allocate_array<zonetool::h1::GfxPackedVertex>(surface->vertCount);
			new_surface->unknown0 = allocator.allocate_array<zonetool::h1::UnknownXSurface0>(surface->vertCount);

			for (auto i = 0; i < surface->vertCount; i++)
			{
				auto& vertex = new_surface->verts0.packedVerts0[i];
				std::memcpy(vertex.xyz, &streams.positions[i * 3], sizeof(vertex.xyz));
				vertex.binormalSign = binormal_sign(streams.tangents[i]);
				vertex.color.packed = use_colors ? streams.colors[i] | forced_color_bits : 0xFFFFFFFF;
				vertex.texCoord.packed = streams.tex_coords[i];
				vertex.normal.packed = streams.normals[i];
				vertex.tangent.packed = streams.tangents[i];

				auto& position = new_surface->unknown0[i];
				std::memcpy(position.xyz, vertex.xyz, sizeof(position.xyz));
				position.normal = vertex.normal;
			}
		}

		void generate_blend_verts_table(zonetool::h1::XSurface* surface)
		{
			auto blend_index = 0u;
			auto table_index = 0u;
			for (auto bone_count = 1u; bone_count <= max_blend_bones; bone_count++)
			{
				for (auto i = 0; i < surface->blendVertCounts[bone_count - 1]; i++)
				{
					auto& entry = surface->blendVertsTable[table_index++];
					const auto* blend = &surface->blendVerts[blend_index];

					entry.b[0] = blend[0] / h1_bone_offset_scale;
					for (auto bone = 1u; bone < bone_count; bone++)
					{
						entry.b[bone] = blend[bone * 2 - 1] / h1_bone_offset_scale;
						entry.b[7 + bone] = blend[bone * 2];
					}

					entry.blendVertCountIndex = static_cast<unsigned short>(bone_count);
					blend_index += bone_count * 2 - 1;
				}
			}
		}

		bool convert_skin(const XSurface* surface, const vertex_streams& streams, zonetool::h1::XSurface* new_surface,
			utils::memory::allocator& allocator)
		{
			std::size_t blend_size = 0;
			for (auto i = 0; i < surface->vertCount; i++)
			{
				const auto count = streams.skin[i].count;
				if (count < 1 || count > max_blend_bones || (i && count < streams.skin[i - 1].count))
				{
					return false;
				}

				new_surface->blendVertCounts[count - 1]++;
				blend_size += count * 2 - 1;
			}

			new_surface->blendVerts = allocator.allocate_array<unsigned short>(blend_size);
			auto* out = new_surface->blendVerts;
			for (auto i = 0; i < surface->vertCount; i++)
			{
				const auto& skin = streams.skin[i];
				*out++ = static_cast<unsigned short>(skin.bones[0] * h1_bone_offset_scale);
				for (auto bone = 1; bone < skin.count; bone++)
				{
					*out++ = static_cast<unsigned short>(skin.bones[bone] * h1_bone_offset_scale);
					*out++ = skin.weights[bone - 1];
				}
			}

			new_surface->blendVertsTable = allocator.allocate_array<zonetool::h1::BlendVertsUnknown>(surface->vertCount);
			generate_blend_verts_table(new_surface);
			return true;
		}

		zonetool::h1::XSurfaceCollisionTree* convert_collision_tree(const XSurfaceCollisionLeaf* tree, utils::memory::allocator& allocator)
		{
			if (!tree)
			{
				return nullptr;
			}

			const auto new_tree = allocator.allocate<zonetool::h1::XSurfaceCollisionTree>();
			std::memcpy(new_tree->trans, tree->trans, sizeof(new_tree->trans));
			std::memcpy(new_tree->scale, tree->scale, sizeof(new_tree->scale));
			new_tree->nodeCount = tree->nodeCount;
			new_tree->nodes = reinterpret_cast<zonetool::h1::XSurfaceCollisionNode*>(tree->nodes);
			new_tree->leafCount = tree->leafCount;
			new_tree->leafs = reinterpret_cast<zonetool::h1::XSurfaceCollisionLeaf*>(tree->leafs);
			return new_tree;
		}

		void convert_rigid_vert_lists(const XSurface* surface, zonetool::h1::XSurface* new_surface, utils::memory::allocator& allocator)
		{
			if (!surface->rigidVertLists || !surface->rigidVertListCount)
			{
				return;
			}

			new_surface->rigidVertListCount = surface->rigidVertListCount;
			new_surface->rigidVertLists = allocator.allocate_array<zonetool::h1::XRigidVertList>(surface->rigidVertListCount);
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

		bool convert_surface(const char* name, const XSurface* surface, const bool keep_colors, const std::uint32_t forced_color_bits,
			zonetool::h1::XSurface* new_surface, utils::memory::allocator& allocator)
		{
			const auto data = read_vertex_data(surface->shared);
			if (!data)
			{
				ZONETOOL_WARNING("XModelSurfs \"%s\" has no vertex data", name);
				return false;
			}

			const auto streams = map_vertex_streams(surface, *data);
			if (!streams)
			{
				ZONETOOL_WARNING("XModelSurfs \"%s\" has vertex data with flags 0x%X that does not fit", name, surface->flags);
				return false;
			}

			const auto vertex_color_flags = zonetool::h1::SURF_FLAG_VERTCOL_GREY | zonetool::h1::SURF_FLAG_VERTCOL_NONE;
			new_surface->flags = (keep_colors ? surface->flags & vertex_color_flags : zonetool::h1::SURF_FLAG_VERTCOL_NONE)
				| (surface->flags & zonetool::h1::SURF_FLAG_SKINNED);
			new_surface->vertCount = surface->vertCount;
			new_surface->triCount = surface->triCount;

			const auto tri_indices = surface->triIndices ? surface->triIndices : streams->indices;
			new_surface->triIndices = allocator.allocate_array<zonetool::h1::Face>(surface->triCount);
			new_surface->triIndices2 = allocator.allocate_array<zonetool::h1::Face>(surface->triCount);
			std::memcpy(new_surface->triIndices, tri_indices, surface->triCount * sizeof(zonetool::h1::Face));
			std::memcpy(new_surface->triIndices2, tri_indices, surface->triCount * sizeof(zonetool::h1::Face));

			convert_vertices(surface, *streams, new_surface, keep_colors, forced_color_bits, allocator);
			convert_rigid_vert_lists(surface, new_surface, allocator);

			if (streams->skin)
			{
				if (!convert_skin(surface, *streams, new_surface, allocator))
				{
					ZONETOOL_WARNING("XModelSurfs \"%s\" has skin data that is not valid", name);
					return false;
				}
			}
			else
			{
				new_surface->blendVertsTable = allocator.allocate_array<zonetool::h1::BlendVertsUnknown>(surface->vertCount);
			}

			std::memcpy(new_surface->partBits, surface->partBits, sizeof(new_surface->partBits));
			return true;
		}
	}

	zonetool::h1::XModelSurfs* convert(const XModelSurfs* asset, const std::unordered_map<int, std::uint32_t>& vertex_color_surfs,
		unsigned int vertex_lighting_index, utils::memory::allocator& allocator)
	{
		const auto new_asset = allocator.allocate<zonetool::h1::XModelSurfs>();
		new_asset->name = asset->name;
		new_asset->numsurfs = asset->numsurfs;
		new_asset->surfs = allocator.allocate_array<zonetool::h1::XSurface>(asset->numsurfs);
		std::memcpy(new_asset->partBits, asset->partBits, sizeof(new_asset->partBits));

		for (auto i = 0; i < asset->numsurfs; i++)
		{
			if (!convert_surface(asset->name, &asset->surfs[i], vertex_color_surfs.contains(i), vertex_color_surfs.contains(i) ? vertex_color_surfs.at(i) : 0u, &new_asset->surfs[i], allocator))
			{
				return nullptr;
			}

			new_asset->surfs[i].vertexLightingIndex = vertex_lighting_index;
			vertex_lighting_index += asset->surfs[i].vertCount;
		}

		return new_asset;
	}

	void dump(XModelSurfs* asset, const std::unordered_map<int, std::uint32_t>& vertex_color_surfs, const unsigned int vertex_lighting_index)
	{
		utils::memory::allocator allocator;
		if (const auto converted = convert(asset, vertex_color_surfs, vertex_lighting_index, allocator))
		{
			zonetool::h1::xsurface::dump(converted);
			dumped_surfs.insert(asset->name);
		}
	}

	bool is_dumped(const std::string& name)
	{
		return dumped_surfs.contains(name);
	}
}
