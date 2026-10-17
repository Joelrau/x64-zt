#include <std_include.hpp>

#include "../include.hpp"
#include "xsurface.hpp"

#include <utils/cryptography.hpp>

namespace zonetool::h1::converter::s2::xsurface
{
	namespace
	{
		std::unordered_set<std::string> dumped_surfs;

		struct converted_surface
		{
			zonetool::s2::XSurfaceShared shared;
			std::string data;
			std::vector<zonetool::s2::XRigidVertList> rigid_lists;
			std::vector<zonetool::s2::XSurfaceCollisionLeaf> trees;
			std::vector<std::array<float, 4>> verts;
		};

		class stream_writer
		{
		public:
			template <typename T>
			std::size_t take(const std::size_t count)
			{
				this->data_.resize((this->data_.size() + 7) & ~7ull);
				const auto start = this->data_.size();
				this->data_.resize(start + count * sizeof(T));
				return start;
			}

			template <typename T>
			T* at(const std::size_t offset)
			{
				return reinterpret_cast<T*>(this->data_.data() + offset);
			}

			std::string& data()
			{
				return this->data_;
			}

		private:
			std::string data_;
		};

		bool has_vertex_color(const XSurface* surface)
		{
			return (surface->flags & (SURF_FLAG_VERTCOL_NONE | SURF_FLAG_REACTIVE_MOTION)) == 0;
		}

		std::uint32_t pack_tangent(const GfxPackedVertex& vertex)
		{
			return (vertex.tangent.packed & ~0xC0000000u) | (vertex.binormalSign < 0.0f ? 0u : 0xC0000000u);
		}

		std::uint32_t pack_sky_visibility(const std::uint32_t packed_normal)
		{
			// bits 0-17 hold the octahedral bent normal, 18-24 the visibility and 25-31 the cone aperture
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

		std::vector<zonetool::s2::XSurfaceSkinVertex> convert_skin(const XSurface* surface)
		{
			std::vector<zonetool::s2::XSurfaceSkinVertex> result(surface->vertCount);

			const auto* blend = surface->blendVerts;
			auto vertex = 0u;
			static const std::uint32_t h1_bone_offset_scale = 64;

			for (auto bone_count = 1u; bone_count <= 8; bone_count++)
			{
				for (auto i = 0; i < surface->blendVertCounts[bone_count - 1] && vertex < result.size(); i++)
				{
					auto& skin = result[vertex++];
					skin.count = static_cast<std::uint16_t>(std::min(bone_count, 4u));
					skin.bones[0] = static_cast<std::uint16_t>(blend[0] / h1_bone_offset_scale);
					for (auto bone = 1u; bone < skin.count; bone++)
					{
						skin.bones[bone] = static_cast<std::uint16_t>(blend[bone * 2 - 1] / h1_bone_offset_scale);
						skin.weights[bone - 1] = blend[bone * 2];
					}

					blend += bone_count * 2 - 1;
				}
			}

			return result;
		}

		std::string build_vertex_data(const XSurface* surface)
		{
			const auto vert_count = static_cast<std::size_t>(surface->vertCount);
			const auto* verts = surface->verts0.packedVerts0;
			const auto use_color = has_vertex_color(surface);

			stream_writer writer;
			const auto faces_offset = writer.take<Face>(surface->triCount);
			const auto positions_offset = writer.take<float>(vert_count * 3);
			const auto sky_visibility_offset = writer.take<std::uint32_t>(vert_count);
			const auto colors_offset = writer.take<std::uint32_t>(vert_count);
			const auto tex_coords_offset = writer.take<std::uint32_t>(vert_count);
			const auto normals_offset = writer.take<std::uint32_t>(vert_count);
			const auto tangents_offset = writer.take<std::uint32_t>(vert_count);
			const auto skin_offset = (surface->flags & SURF_FLAG_SKINNED) ? std::optional{writer.take<zonetool::s2::XSurfaceSkinVertex>(vert_count)} : std::nullopt;

			std::memcpy(writer.at<Face>(faces_offset), surface->triIndices, surface->triCount * sizeof(Face));

			auto* positions = writer.at<float>(positions_offset);
			auto* sky_visibility = writer.at<std::uint32_t>(sky_visibility_offset);
			auto* colors = writer.at<std::uint32_t>(colors_offset);
			auto* tex_coords = writer.at<std::uint32_t>(tex_coords_offset);
			auto* normals = writer.at<std::uint32_t>(normals_offset);
			auto* tangents = writer.at<std::uint32_t>(tangents_offset);
			for (auto i = 0u; i < vert_count; i++)
			{
				std::memcpy(&positions[i * 3], verts[i].xyz, sizeof(verts[i].xyz));
				colors[i] = use_color ? verts[i].color.packed : 0xFFFFFFFF;
				tex_coords[i] = verts[i].texCoord.packed;
				normals[i] = verts[i].normal.packed;
				tangents[i] = pack_tangent(verts[i]);
				sky_visibility[i] = pack_sky_visibility(verts[i].normal.packed);
			}

			if (skin_offset)
			{
				const auto skin = convert_skin(surface);
				std::memcpy(writer.at<zonetool::s2::XSurfaceSkinVertex>(*skin_offset), skin.data(), vert_count * sizeof(zonetool::s2::XSurfaceSkinVertex));
			}

			return std::move(writer.data());
		}

		float surface_area(const XSurface* surface)
		{
			const auto* verts = surface->verts0.packedVerts0;
			auto area = 0.0f;
			for (auto i = 0; i < surface->triCount; i++)
			{
				const auto& face = surface->triIndices[i];
				const auto* a = verts[face.v1].xyz;
				const auto* b = verts[face.v2].xyz;
				const auto* c = verts[face.v3].xyz;
				const float u[3]{b[0] - a[0], b[1] - a[1], b[2] - a[2]};
				const float v[3]{c[0] - a[0], c[1] - a[1], c[2] - a[2]};
				const float n[3]{u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2], u[0] * v[1] - u[1] * v[0]};
				area += 0.5f * std::sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
			}

			return area;
		}

		void convert_rigid_vert_lists(const XSurface* surface, converted_surface& result)
		{
			if (!surface->rigidVertLists || !surface->rigidVertListCount)
			{
				return;
			}

			result.rigid_lists.resize(surface->rigidVertListCount);
			result.trees.resize(surface->rigidVertListCount);
			for (auto i = 0; i < surface->rigidVertListCount; i++)
			{
				const auto& list = surface->rigidVertLists[i];
				auto& new_list = result.rigid_lists[i];
				new_list.boneOffset = list.boneOffset;
				new_list.vertCount = list.vertCount;
				new_list.triOffset = list.triOffset;
				new_list.triCount = list.triCount;
				new_list.collisionTree = nullptr;

				if (const auto* tree = list.collisionTree)
				{
					auto& new_tree = result.trees[i];
					std::memcpy(new_tree.trans, tree->trans, sizeof(new_tree.trans));
					std::memcpy(new_tree.scale, tree->scale, sizeof(new_tree.scale));
					new_tree.nodeCount = tree->nodeCount;
					new_tree.leafCount = tree->leafCount;
					new_tree.nodes = reinterpret_cast<zonetool::s2::vec4_t*>(tree->nodes);
					new_tree.leafs = reinterpret_cast<unsigned short*>(tree->leafs);
					new_list.collisionTree = &new_tree;
				}
			}
		}

		void convert_surface(const XSurface* surface, zonetool::s2::XSurface* new_surface, converted_surface& result)
		{
			const auto skinned = (surface->flags & SURF_FLAG_SKINNED) != 0;

			std::memset(new_surface, 0, sizeof(zonetool::s2::XSurface));
			new_surface->flags = zonetool::s2::XSURFACE_FLAG_UNK_0x20
				| (has_vertex_color(surface) ? 0 : zonetool::s2::XSURFACE_FLAG_VERTCOL_NONE)
				| (skinned ? zonetool::s2::XSURFACE_FLAG_SKINNED : 0);
			new_surface->vertCount = surface->vertCount;
			new_surface->triCount = surface->triCount;
			new_surface->uvSetCount = 1;

			result.data = build_vertex_data(surface);
			auto& shared = result.shared;
			std::memset(&shared, 0, sizeof(shared));
			const auto streams = static_cast<std::uint16_t>(0x7E0 | (skinned ? 0x1000 : 0));
			shared.flags = static_cast<std::uint8_t>(streams);
			shared.__pad0 = static_cast<std::uint8_t>(streams >> 8);
			const auto hash = utils::cryptography::sha256::compute(result.data, true);
			std::memcpy(shared.hash, hash.data(), sizeof(shared.hash));
			shared.indexCount = surface->triCount * 3u;
			shared.vertCount = surface->vertCount;
			shared.dataSize = static_cast<std::uint32_t>(result.data.size());
			shared.data = result.data.data();
			new_surface->shared = &shared;
			new_surface->triIndices = reinterpret_cast<unsigned short*>(surface->triIndices);

			convert_rigid_vert_lists(surface, result);
			new_surface->rigidVertListCount = static_cast<unsigned char>(result.rigid_lists.size());
			new_surface->rigidVertLists = result.rigid_lists.empty() ? nullptr : result.rigid_lists.data();

			result.verts.resize(surface->vertCount);
			for (auto i = 0; i < surface->vertCount; i++)
			{
				const auto& vertex = surface->verts0.packedVerts0[i];
				auto* out = result.verts[i].data();
				std::memcpy(out, vertex.xyz, sizeof(vertex.xyz));
				std::memcpy(&out[3], &vertex.normal.packed, sizeof(std::uint32_t));
			}
			new_surface->verts = reinterpret_cast<zonetool::s2::vec4_t*>(result.verts.data());

			std::memcpy(new_surface->partBits, surface->partBits, sizeof(new_surface->partBits));
		}

		bool has_vertex_data(const XSurface& surface)
		{
			return surface.verts0.packedVerts0 && surface.triIndices
				&& ((surface.flags & SURF_FLAG_SKINNED) == 0 || surface.blendVerts);
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

		std::vector<zonetool::s2::XSurface> surfaces(asset->numsurfs);
		std::deque<converted_surface> converted(asset->numsurfs);
		auto area = 0.0f;
		for (auto i = 0; i < asset->numsurfs; i++)
		{
			convert_surface(&asset->surfs[i], &surfaces[i], converted[i]);
			area += surface_area(&asset->surfs[i]);
		}

		zonetool::s2::XModelSurfs new_asset{};
		new_asset.name = asset->name;
		new_asset.surfs = surfaces.data();
		new_asset.numsurfs = asset->numsurfs;
		std::memcpy(new_asset.partBits, asset->partBits, sizeof(new_asset.partBits));
		new_asset.surfaceArea = area;

		zonetool::s2::xmodel_surfs::dump(&new_asset);
		dumped_surfs.insert(asset->name);
		add_csv_row("xmodelsurfs", asset->name);
	}

	bool is_dumped(const std::string& name)
	{
		return dumped_surfs.contains(name);
	}
}
