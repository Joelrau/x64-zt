#include <std_include.hpp>

#include "../include.hpp"
#include "xsurface.hpp"

#include <iw7/assets/xsurface.hpp>

namespace zonetool::iw5::converter::iw7
{
	namespace xsurface
	{
		namespace
		{
			void convert_blend_verts(const zonetool::iw5::XSurface* source, zonetool::iw7::XSurface* target,
				utils::memory::allocator& allocator)
			{
				const auto size = 2u * target->blendVertCounts[0] + 4u * target->blendVertCounts[1] +
					6u * target->blendVertCounts[2] + 8u * target->blendVertCounts[3];
				if (!size)
				{
					return;
				}

				target->blendVertSize = size * sizeof(unsigned short);
				target->blendVerts = allocator.manual_allocate<unsigned short>(target->blendVertSize);

				const auto blend = source->vertInfo.vertsBlend.get();
				unsigned int out = 0;
				unsigned int in = 0;

				for (unsigned short i = 0; i < target->blendVertCounts[0]; i++)
				{
					target->blendVerts[out++] = blend[in + 0] / 64;
					target->blendVerts[out++] = 0;
					in += 1;
				}

				for (unsigned short i = 0; i < target->blendVertCounts[1]; i++)
				{
					target->blendVerts[out++] = blend[in + 0] / 64;
					target->blendVerts[out++] = blend[in + 1] / 64;
					target->blendVerts[out++] = blend[in + 2];
					target->blendVerts[out++] = 0;
					in += 3;
				}

				for (unsigned short i = 0; i < target->blendVertCounts[2]; i++)
				{
					target->blendVerts[out++] = blend[in + 0] / 64;
					target->blendVerts[out++] = blend[in + 1] / 64;
					target->blendVerts[out++] = blend[in + 2];
					target->blendVerts[out++] = blend[in + 3] / 64;
					target->blendVerts[out++] = blend[in + 4];
					target->blendVerts[out++] = 0;
					in += 5;
				}

				for (unsigned short i = 0; i < target->blendVertCounts[3]; i++)
				{
					target->blendVerts[out++] = blend[in + 0] / 64;
					target->blendVerts[out++] = blend[in + 1] / 64;
					target->blendVerts[out++] = blend[in + 2];
					target->blendVerts[out++] = blend[in + 3] / 64;
					target->blendVerts[out++] = blend[in + 4];
					target->blendVerts[out++] = blend[in + 5] / 64;
					target->blendVerts[out++] = blend[in + 6];
					target->blendVerts[out++] = 0;
					in += 7;
				}
			}

			void convert_vertices(const zonetool::iw5::XSurface* source, zonetool::iw7::XSurface* target,
				utils::memory::allocator& allocator)
			{
				static_assert(sizeof(zonetool::iw5::GfxPackedVertex) == sizeof(zonetool::iw7::GfxPackedVertex));

				const auto verts = source->verts0.packedVerts0.get();
				const auto new_verts = allocator.allocate_array<zonetool::iw7::GfxPackedVertex>(source->vertCount);
				target->verts0.packedVerts0 = new_verts;

				for (unsigned short i = 0; i < source->vertCount; i++)
				{
					std::memcpy(&new_verts[i], &verts[i], sizeof(zonetool::iw7::GfxPackedVertex));

					float texture_coordinates[2]{};
					PackedVec::Vec2UnpackTexCoords(verts[i].texCoord.packed, texture_coordinates);
					std::swap(texture_coordinates[0], texture_coordinates[1]);
					new_verts[i].texCoord.packed = PackedVec::Vec2PackTexCoords(texture_coordinates);

					float normal[3]{};
					PackedVec::Vec3UnpackUnitVec_T6(reinterpret_cast<const std::uint8_t*>(verts[i].normal.array), normal);
					new_verts[i].normal.packed = PackedVec::Vec3PackUnitVec_H1(normal);

					float tangent[3]{};
					PackedVec::Vec3UnpackUnitVec_T6(reinterpret_cast<const std::uint8_t*>(verts[i].tangent.array), tangent);
					new_verts[i].tangent.packed = PackedVec::Vec3PackUnitVec_H1(tangent);

					float visibility[4] = {0.0f, 0.0f, 1.0f, 0.0f};
					new_verts[i].selfVisibility.packed = self_visibility::XSurfacePackSelfVisibility(visibility);

					new_verts[i].color.array[0] = verts[i].color.array[2];
					new_verts[i].color.array[1] = verts[i].color.array[1];
					new_verts[i].color.array[2] = verts[i].color.array[0];
					new_verts[i].color.array[3] = verts[i].color.array[3];
				}
			}

			void convert_rigid_vert_lists(const zonetool::iw5::XSurface* source, zonetool::iw7::XSurface* target,
				utils::memory::allocator& allocator)
			{
				if (!source->vertListCount)
				{
					return;
				}

				const auto lists = source->vertList.get();
				const auto new_lists = allocator.allocate_array<zonetool::iw7::XRigidVertList>(source->vertListCount);
				target->rigidVertLists = new_lists;

				for (unsigned int i = 0; i < source->vertListCount; i++)
				{
					new_lists[i].boneOffsetIndex = lists[i].boneOffset >> 6;
					new_lists[i].vertCount = lists[i].vertCount;
					new_lists[i].triOffset = lists[i].triOffset;
					new_lists[i].triCount = lists[i].triCount;

					if (!lists[i].collisionTree)
					{
						continue;
					}

					const auto tree = lists[i].collisionTree.get();
					const auto new_tree = allocator.allocate<zonetool::iw7::XSurfaceCollisionTree>();
					new_lists[i].collisionTree = new_tree;

					std::memcpy(new_tree->trans, tree->trans, sizeof(new_tree->trans));
					std::memcpy(new_tree->scale, tree->scale, sizeof(new_tree->scale));

					new_tree->nodeCount = tree->nodeCount;
					new_tree->nodes = allocator.allocate_array<zonetool::iw7::XSurfaceCollisionNode>(tree->nodeCount);
					for (unsigned int node = 0; node < tree->nodeCount; node++)
					{
						const auto& source_node = tree->nodes.get()[node];
						auto& target_node = new_tree->nodes[node];
						std::memcpy(target_node.aabb.mins, source_node.aabb.mins, sizeof(target_node.aabb.mins));
						std::memcpy(target_node.aabb.maxs, source_node.aabb.maxs, sizeof(target_node.aabb.maxs));
						target_node.childBeginIndex = source_node.childBeginIndex;
						target_node.childCount = source_node.childCount;
					}

					new_tree->leafCount = tree->leafCount;
					new_tree->leafs = allocator.allocate_array<zonetool::iw7::XSurfaceCollisionLeaf>(tree->leafCount);
					for (unsigned int leaf = 0; leaf < tree->leafCount; leaf++)
					{
						new_tree->leafs[leaf].triangleBeginIndex = tree->leafs.get()[leaf].triangleBeginIndex;
					}
				}
			}

			void convert_surface(const zonetool::iw5::XSurface* source, zonetool::iw7::XSurface* target,
				utils::memory::allocator& allocator)
			{
				target->flags = zonetool::iw7::SURF_FLAG_SELF_VISIBILITY;
				if (source->flags & zonetool::iw5::SURF_FLAG_VERTCOL_GREY)
				{
					target->flags |= zonetool::iw7::SURF_FLAG_VERTCOL_GREY;
				}

				if (source->flags & zonetool::iw5::SURF_FLAG_VERTCOL_NONE)
				{
					target->flags |= zonetool::iw7::SURF_FLAG_VERTCOL_NONE;
				}

				if (source->flags & zonetool::iw5::SURF_FLAG_SKINNED)
				{
					target->flags |= zonetool::iw7::SURF_FLAG_SKINNED;
				}

				target->vertCount = source->vertCount;
				target->triCount = source->triCount;
				target->rigidVertListCount = static_cast<unsigned char>(source->vertListCount);

				static_assert(sizeof(source->vertInfo.vertCount) <= sizeof(target->blendVertCounts));
				std::memcpy(target->blendVertCounts, source->vertInfo.vertCount, sizeof(source->vertInfo.vertCount));
				convert_blend_verts(source, target, allocator);

				target->triIndices = reinterpret_cast<zonetool::iw7::Face*>(source->triIndices.get());
				convert_vertices(source, target, allocator);
				convert_rigid_vert_lists(source, target, allocator);

				std::memcpy(target->partBits, source->partBits, sizeof(source->partBits));
			}
		}

		zonetool::iw7::XModelSurfs* convert(zonetool::iw5::XModelSurfs* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw7::XModelSurfs>();

			new_asset->name = asset->name.get();
			new_asset->numsurfs = asset->numsurfs;
			std::memcpy(new_asset->partBits, asset->partBits, sizeof(asset->partBits));

			new_asset->surfs = allocator.allocate_array<zonetool::iw7::XSurface>(asset->numsurfs);
			for (unsigned short i = 0; i < asset->numsurfs; i++)
			{
				convert_surface(&asset->surfs.get()[i], &new_asset->surfs[i], allocator);
			}

			return new_asset;
		}

		void dump(zonetool::iw5::XModelSurfs* asset)
		{
			utils::memory::allocator allocator;
			zonetool::iw7::xsurface::dump(convert(asset, allocator));
		}
	}
}
