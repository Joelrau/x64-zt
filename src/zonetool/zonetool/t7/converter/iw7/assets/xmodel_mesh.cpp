#include <std_include.hpp>
#include "zonetool/t7/converter/iw7/include.hpp"
#include "xmodel_mesh.hpp"

#include "zonetool/iw7/assets/xsurface.hpp"

#include "zonetool/t7/common/xpak.hpp"

namespace zonetool::t7
{
	namespace converter::iw7
	{
		namespace xmodel_mesh
		{
			void GenerateIW7BlendVerts(unsigned short* vertsBlend, zonetool::iw7::XSurface* surf, utils::memory::allocator& mem)
			{
				unsigned int size = (2 * surf->blendVertCounts[0]
					+ 4 * surf->blendVertCounts[1]
					+ 6 * surf->blendVertCounts[2]
					+ 8 * surf->blendVertCounts[3]);

				if (!size) return;

				surf->blendVertSize = size * sizeof(unsigned short);
				surf->blendVerts = mem.manual_allocate<unsigned short>(surf->blendVertSize);

				assert(surf->blendVertSize % 2 == 0);

				unsigned int a = 0;
				unsigned int b = 0;
				for (unsigned short s = 0; s < (surf->blendVertCounts[0]); s++)
				{
					surf->blendVerts[a++] = vertsBlend[b + 0] / 64;
					surf->blendVerts[a++] = 0;

					b += 1;
				}

				for (unsigned short s = 0; s < (surf->blendVertCounts[1]); s++)
				{
					surf->blendVerts[a++] = vertsBlend[b + 0] / 64;
					surf->blendVerts[a++] = vertsBlend[b + 1] / 64;
					surf->blendVerts[a++] = vertsBlend[b + 2];
					surf->blendVerts[a++] = 0;
					b += 3;
				}

				for (unsigned short s = 0; s < (surf->blendVertCounts[2]); s++)
				{
					surf->blendVerts[a++] = vertsBlend[b + 0] / 64;
					surf->blendVerts[a++] = vertsBlend[b + 1] / 64;
					surf->blendVerts[a++] = vertsBlend[b + 2];
					surf->blendVerts[a++] = vertsBlend[b + 3] / 64;
					surf->blendVerts[a++] = vertsBlend[b + 4];
					surf->blendVerts[a++] = 0;
					b += 5;
				}

				for (unsigned short s = 0; s < (surf->blendVertCounts[3]); s++)
				{
					surf->blendVerts[a++] = vertsBlend[b + 0] / 64;
					surf->blendVerts[a++] = vertsBlend[b + 1] / 64;
					surf->blendVerts[a++] = vertsBlend[b + 2];
					surf->blendVerts[a++] = vertsBlend[b + 3] / 64;
					surf->blendVerts[a++] = vertsBlend[b + 4];
					surf->blendVerts[a++] = vertsBlend[b + 5] / 64;
					surf->blendVerts[a++] = vertsBlend[b + 6];
					surf->blendVerts[a++] = 0;
					b += 7;
				}

				assert(a == size);
			}

			zonetool::iw7::XModelSurfs* convert(XModelMesh* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::iw7::XModelSurfs>();

				REINTERPRET_CAST_SAFE(name);

				byte* data = nullptr;
				if (asset->shared && asset->shared->dataSize)
				{
					if ((asset->shared->flags & 0x1) == 0) // loaded
					{
						data = asset->shared->data;
					}
					else // streamed
					{
						auto xpak_data = xpak::get_data_for_xpak_key(asset->xpakEntry.key, asset->shared->dataSize);
						if (xpak_data.empty())
						{
							ZONETOOL_ERROR("error getting xmodelmesh data from xpak for \"%s\"\n", asset->name);
							return nullptr;
						}

						assert(static_cast<uint32_t>(xpak_data.size()) == asset->shared->dataSize);
						data = allocator.allocate_array<byte>(asset->shared->dataSize);
						memcpy(data, xpak_data.data(), asset->shared->dataSize);
					}
				}
				
				new_asset->numsurfs = asset->numSurfs;
				new_asset->surfs = allocator.allocate_array<zonetool::iw7::XSurface>(new_asset->numsurfs);
				for (auto i = 0; i < new_asset->numsurfs; i++)
				{
					auto* surf = &asset->surfs[i];
					auto* new_surf = &new_asset->surfs[i];

					new_surf->flags = 0;
					new_surf->flags |= ((surf->flags & zonetool::t7::XSURFACE_FLAG_SKINNED) != 0) ? zonetool::iw7::SURF_FLAG_SKINNED : 0;
					//new_surf->flags |= zonetool::iw7::SURF_FLAG_VERTCOL_NONE;
					new_surf->flags |= zonetool::iw7::SURF_FLAG_SELF_VISIBILITY;

					new_surf->vertCount = surf->vertCount;
					new_surf->triCount = surf->triCount;
					new_surf->rigidVertListCount = surf->vertListCount;

					new_surf->subdivLevelCount = 0;

					if (surf->shared && surf->shared->dataSize && data)
					{
						auto* verts = reinterpret_cast<GfxStreamVertex*>(data + surf->shared->vertsOffset + surf->baseVertOffset * sizeof(GfxStreamVertex));
						auto* indices = reinterpret_cast<GfxStreamFace*>(data + surf->shared->indicesOffset + surf->baseIndexOffset * 2);
						auto* positions = reinterpret_cast<vec3_t*>(data + surf->shared->posOffset + surf->baseVertOffset * sizeof(vec3_t));
						auto* weights = reinterpret_cast<GfxStreamWeight*>(data + surf->shared->skinWeightsOffset + surf->baseVertOffset * sizeof(GfxStreamWeight));

						new_surf->verts0.packedVerts0 = allocator.allocate_array<zonetool::iw7::GfxPackedVertex>(new_surf->vertCount);

						for (auto j = 0; j < new_surf->vertCount; j++)
						{
							new_surf->verts0.packedVerts0[j].xyz[0] = positions[j][0];
							new_surf->verts0.packedVerts0[j].xyz[1] = positions[j][1];
							new_surf->verts0.packedVerts0[j].xyz[2] = positions[j][2];

							new_surf->verts0.packedVerts0[j].color.array[0] = verts[j].Color[0];
							new_surf->verts0.packedVerts0[j].color.array[1] = verts[j].Color[1];
							new_surf->verts0.packedVerts0[j].color.array[2] = verts[j].Color[2];
							new_surf->verts0.packedVerts0[j].color.array[3] = verts[j].Color[3];

							memcpy(&new_surf->verts0.packedVerts0[j].texCoord.packed, &verts[j].UVUPosition, sizeof(int)); // check

							new_surf->verts0.packedVerts0[j].normal.packed = verts[j].VertexNormal; // check
							new_surf->verts0.packedVerts0[j].tangent.packed = verts[j].VertexTangent; // check

							float default_visibility[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
							new_surf->verts0.packedVerts0[i].selfVisibility.packed = self_visibility::XSurfacePackSelfVisibility(default_visibility);
						}

						new_surf->triIndices = reinterpret_cast<zonetool::iw7::Face*>(indices);

						std::vector<GfxStreamWeight*> weights_arr[4];
						if ((surf->flags & XSURFACE_FLAG_SKINNED) != 0)
						{
							// calc counts
							for (auto j = 0u; j < surf->vertCount; j++)
							{
								auto weight = &weights[j];

								auto idx = 0;

								if (weight->WeightVal2)
								{
									idx++;
								}

								if (weight->WeightVal3)
								{
									idx++;
								}

								if (weight->WeightVal4)
								{
									idx++;
								}

								new_surf->blendVertCounts[idx]++;
								weights_arr[idx].push_back(weight);
							}

							const auto total_blend_verts = (new_surf->blendVertCounts[0]
								+ 3 * new_surf->blendVertCounts[1]
								+ 5 * new_surf->blendVertCounts[2]
								+ 7 * new_surf->blendVertCounts[3]
								+ 9 * new_surf->blendVertCounts[4]
								+ 11 * new_surf->blendVertCounts[5]
								+ 13 * new_surf->blendVertCounts[6]
								+ 15 * new_surf->blendVertCounts[7]);

							// generate
							{
								auto vertsBlend = allocator.manual_allocate<unsigned short>(2 * total_blend_verts);
								unsigned short b = 0;
								for (short s = 0; s < (new_surf->blendVertCounts[0]); s++)
								{
									auto weight = weights_arr[0][s];

									vertsBlend[b] = weight->WeightID1 * 64;

									b += 1;
								}

								for (short s = 0; s < (new_surf->blendVertCounts[1]); s++)
								{
									auto weight = weights_arr[1][s];

									vertsBlend[b] = weight->WeightID1 * 64;
									vertsBlend[b + 1] = weight->WeightID2 * 64;

									vertsBlend[b + 2] = weight->WeightVal1;

									b += 3;
								}

								for (short s = 0; s < (new_surf->blendVertCounts[2]); s++)
								{
									auto weight = weights_arr[2][s];

									vertsBlend[b] = weight->WeightID1 * 64;
									vertsBlend[b + 1] = weight->WeightID2 * 64;
									vertsBlend[b + 3] = weight->WeightID3 * 64;

									vertsBlend[b + 2] = weight->WeightVal1;
									vertsBlend[b + 4] = weight->WeightVal2;

									b += 5;
								}

								for (short s = 0; s < (new_surf->blendVertCounts[3]); s++)
								{
									auto weight = weights_arr[3][s];

									vertsBlend[b] = weight->WeightID1 * 64;
									vertsBlend[b + 1] = weight->WeightID2 * 64;
									vertsBlend[b + 3] = weight->WeightID3 * 64;
									vertsBlend[b + 5] = weight->WeightID4 * 64;

									vertsBlend[b + 2] = weight->WeightVal1;
									vertsBlend[b + 4] = weight->WeightVal2;
									vertsBlend[b + 6] = weight->WeightVal3;

									b += 7;
								}

								GenerateIW7BlendVerts(vertsBlend, new_surf, allocator);
								allocator.free(vertsBlend);
							}
						}
					}

					static_assert(sizeof(zonetool::iw7::XSurfaceCollisionTree) == sizeof(zonetool::t7::XSurfaceCollisionTree));
					static_assert(sizeof(zonetool::iw7::XSurfaceCollisionNode) == sizeof(zonetool::t7::XSurfaceCollisionNode));
					static_assert(sizeof(zonetool::iw7::XSurfaceCollisionLeaf) == sizeof(zonetool::t7::XSurfaceCollisionLeaf));

					new_surf->rigidVertLists = allocator.allocate_array<zonetool::iw7::XRigidVertList>(surf->vertListCount);
					for (int j = 0; j < surf->vertListCount; j++)
					{
						new_surf->rigidVertLists[j].boneOffsetIndex = surf->vertList[j].boneOffset >> 6; // R_MarkFragments_AnimatedXModel
						new_surf->rigidVertLists[j].vertCount = surf->vertList[j].vertCount;
						new_surf->rigidVertLists[j].triOffset = surf->vertList[j].triOffset;
						new_surf->rigidVertLists[j].triCount = surf->vertList[j].triCount;

						new_surf->rigidVertLists[j].collisionTree = reinterpret_cast<zonetool::iw7::XSurfaceCollisionTree*>(surf->vertList[j].collisionTree);
					}

					memcpy(new_surf->partBits, surf->partBits, sizeof(float[8]));
				}

				memcpy(new_asset->partBits, asset->partBits, sizeof(float[8]));

				return new_asset;
			}

			void dump(XModelMesh* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				if (!converted_asset) return;
				zonetool::iw7::xsurface::dump(converted_asset);
			}
		}
	}
}
