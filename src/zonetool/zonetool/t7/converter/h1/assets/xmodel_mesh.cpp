#include <std_include.hpp>
#include "zonetool/t7/converter/h1/include.hpp"
#include "xmodel_mesh.hpp"

#include "zonetool/h1/assets/xsurface.hpp"

#include "zonetool/t7/common/xpak.hpp"

namespace zonetool::t7
{
	namespace converter::h1
	{
		namespace xmodel_mesh
		{
			void GenerateH1BlendVertsTable(zonetool::h1::XSurface* surf)
			{
				size_t a = 0, b = 0;

				auto zero_row = [](zonetool::h1::BlendVertsUnknown& r) {
					std::fill(std::begin(r.b), std::end(r.b), 0);
				};

				for (size_t s = 0; s < surf->blendVertCounts[0]; ++s, ++a, b += 1) {
					auto& row = surf->blendVertsTable[a]; zero_row(row);
					row.b[0] = static_cast<uint8_t>(surf->blendVerts[b + 0] / 64);
					row.blendVertCountIndex = 1;
				}
				for (size_t s = 0; s < surf->blendVertCounts[1]; ++s, ++a, b += 3) {
					auto& row = surf->blendVertsTable[a]; zero_row(row);
					row.b[0] = static_cast<uint8_t>(surf->blendVerts[b + 0] / 64);
					row.b[1] = static_cast<uint8_t>(surf->blendVerts[b + 1] / 64);
					row.b[8] = static_cast<uint8_t>(surf->blendVerts[b + 2]);
					row.blendVertCountIndex = 2;
				}
				for (size_t s = 0; s < surf->blendVertCounts[2]; ++s, ++a, b += 5) {
					auto& row = surf->blendVertsTable[a]; zero_row(row);
					row.b[0] = static_cast<uint8_t>(surf->blendVerts[b + 0] / 64);
					row.b[1] = static_cast<uint8_t>(surf->blendVerts[b + 1] / 64);
					row.b[2] = static_cast<uint8_t>(surf->blendVerts[b + 3] / 64);
					row.b[8] = static_cast<uint8_t>(surf->blendVerts[b + 2]);
					row.b[9] = static_cast<uint8_t>(surf->blendVerts[b + 4]);
					row.blendVertCountIndex = 3;
				}
				for (size_t s = 0; s < surf->blendVertCounts[3]; ++s, ++a, b += 7) {
					auto& row = surf->blendVertsTable[a]; zero_row(row);
					row.b[0] = static_cast<uint8_t>(surf->blendVerts[b + 0] / 64);
					row.b[1] = static_cast<uint8_t>(surf->blendVerts[b + 1] / 64);
					row.b[2] = static_cast<uint8_t>(surf->blendVerts[b + 3] / 64);
					row.b[3] = static_cast<uint8_t>(surf->blendVerts[b + 5] / 64);
					row.b[8] = static_cast<uint8_t>(surf->blendVerts[b + 2]);
					row.b[9] = static_cast<uint8_t>(surf->blendVerts[b + 4]);
					row.b[10] = static_cast<uint8_t>(surf->blendVerts[b + 6]);
					row.blendVertCountIndex = 4;
				}
			}

			zonetool::h1::XModelSurfs* convert(XModelMesh* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::XModelSurfs>();

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
				new_asset->surfs = allocator.allocate_array<zonetool::h1::XSurface>(new_asset->numsurfs);

				GfxStreamVertex* verts = nullptr;
				GfxStreamFace* indices = nullptr;
				vec3_t* positions = nullptr;
				GfxStreamWeight* weights = nullptr;

				if (asset->shared)
				{
					verts = reinterpret_cast<GfxStreamVertex*>(data + asset->shared->vertsOffset);
					indices = reinterpret_cast<GfxStreamFace*>(data + asset->shared->indicesOffset);
					positions = reinterpret_cast<vec3_t*>(data + asset->shared->posOffset);
					weights = reinterpret_cast<GfxStreamWeight*>(data + asset->shared->skinWeightsOffset);
				}
				else
				{
					__debugbreak();
				}

				unsigned int baseVertOffset = 0;
				unsigned int baseIndexOffset = 0;
				unsigned int skinVertCount = 0;

				auto parse_surf = [&](const auto i)
				{
					auto* surf = &asset->surfs[i];
					auto* new_surf = &new_asset->surfs[i];

					new_surf->flags = 0;
					new_surf->flags |= ((surf->flags & zonetool::t7::XSURFACE_FLAG_SKINNED) != 0) ? zonetool::h1::SURF_FLAG_SKINNED : 0;
					//new_surf->flags |= zonetool::h1::SURF_FLAG_VERTCOL_NONE;

					new_surf->vertCount = surf->vertCount;
					new_surf->triCount = surf->triCount;
					new_surf->rigidVertListCount = surf->vertListCount;

					new_surf->subdivLevelCount = 0;

					if (surf->shared && surf->shared->dataSize && data)
					{
#ifdef DEBUG
						assert(surf->baseVertOffset == baseVertOffset);
						assert(surf->baseIndexOffset == baseIndexOffset);

						auto* verts_ = reinterpret_cast<GfxStreamVertex*>(data + surf->shared->vertsOffset + surf->baseVertOffset * sizeof(GfxStreamVertex));
						auto* indices_ = reinterpret_cast<GfxStreamFace*>(data + surf->shared->indicesOffset + (surf->baseIndexOffset / 3) * sizeof(GfxStreamFace));
						auto* positions_ = reinterpret_cast<vec3_t*>(data + surf->shared->posOffset + surf->baseVertOffset * sizeof(vec3_t));
						auto* weights_ = reinterpret_cast<GfxStreamWeight*>(data + surf->shared->skinWeightsOffset + surf->baseVertOffset * sizeof(GfxStreamWeight));

						assert(verts == verts_);
						assert(indices == indices_);
						assert(positions == positions_);
						assert(weights == weights_);
#endif

						new_surf->verts0.packedVerts0 = allocator.allocate_array<zonetool::h1::GfxPackedVertex>(new_surf->vertCount);

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

							new_surf->verts0.packedVerts0[j].normal.packed = verts[j].VertexNormal;
							new_surf->verts0.packedVerts0[j].tangent.packed = verts[j].VertexTangent;

							float unpacked_normal[3]{};
							PackedVec::Vec3UnpackUnitVec_IW8(verts[j].VertexNormal, unpacked_normal);

							float unpacked_tangent[3]{};
							PackedVec::Vec3UnpackUnitVec_IW8(verts[j].VertexTangent, unpacked_tangent);

							new_surf->verts0.packedVerts0[j].binormalSign = unpacked_normal[0] > 0.0f ? 1.0f : -1.0f; // check
						}

						new_surf->triIndices = reinterpret_cast<zonetool::h1::Face*>(indices);
						new_surf->triIndices2 = allocator.allocate_array<zonetool::h1::Face>(surf->triCount);
						for (unsigned short triIndex = 0; triIndex < surf->triCount; triIndex++)
						{
							memcpy(&new_surf->triIndices2[triIndex], &new_surf->triIndices[triIndex], sizeof(zonetool::h1::Face));
						}

						std::vector<GfxStreamWeight*> weights_arr[4];
						if ((surf->flags & zonetool::t7::XSURFACE_FLAG_SKINNED) != 0)
						{
							std::memset(new_surf->blendVertCounts, 0, sizeof(new_surf->blendVertCounts));

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
								new_surf->blendVerts = allocator.allocate_array<zonetool::h1::XBlendInfo>(total_blend_verts);
								unsigned short b = 0;
								for (short s = 0; s < (new_surf->blendVertCounts[0]); s++)
								{
									auto weight = weights_arr[0][s];

									new_surf->blendVerts[b] = weight->WeightID1 * 64;

									b += 1;
								}

								for (short s = 0; s < (new_surf->blendVertCounts[1]); s++)
								{
									auto weight = weights_arr[1][s];

									new_surf->blendVerts[b] = weight->WeightID1 * 64;
									new_surf->blendVerts[b + 1] = weight->WeightID2 * 64;

									new_surf->blendVerts[b + 2] = weight->WeightVal1;

									b += 3;
								}

								for (short s = 0; s < (new_surf->blendVertCounts[2]); s++)
								{
									auto weight = weights_arr[2][s];

									new_surf->blendVerts[b] = weight->WeightID1 * 64;
									new_surf->blendVerts[b + 1] = weight->WeightID2 * 64;
									new_surf->blendVerts[b + 3] = weight->WeightID3 * 64;

									new_surf->blendVerts[b + 2] = weight->WeightVal1;
									new_surf->blendVerts[b + 4] = weight->WeightVal2;

									b += 5;
								}

								for (short s = 0; s < (new_surf->blendVertCounts[3]); s++)
								{
									auto weight = weights_arr[3][s];

									new_surf->blendVerts[b] = weight->WeightID1 * 64;
									new_surf->blendVerts[b + 1] = weight->WeightID2 * 64;
									new_surf->blendVerts[b + 3] = weight->WeightID3 * 64;
									new_surf->blendVerts[b + 5] = weight->WeightID4 * 64;

									new_surf->blendVerts[b + 2] = weight->WeightVal1;
									new_surf->blendVerts[b + 4] = weight->WeightVal2;
									new_surf->blendVerts[b + 6] = weight->WeightVal3;

									b += 7;
								}

								new_surf->blendVertsTable = allocator.allocate_array<zonetool::h1::BlendVertsUnknown>(new_surf->vertCount);
								GenerateH1BlendVertsTable(new_surf);
							}
						}
					}
					else
					{
						__debugbreak();
					}

					// unknown
					new_surf->unknown0 = allocator.allocate_array<zonetool::h1::UnknownXSurface0>(new_surf->vertCount); // related to indices2?
					for (unsigned short j = 0; j < new_surf->vertCount; j++)
					{
						new_surf->unknown0[j].xyz[0] = new_surf->verts0.packedVerts0[j].xyz[0];
						new_surf->unknown0[j].xyz[1] = new_surf->verts0.packedVerts0[j].xyz[1];
						new_surf->unknown0[j].xyz[2] = new_surf->verts0.packedVerts0[j].xyz[2];
						new_surf->unknown0[j].normal = new_surf->verts0.packedVerts0[j].normal;
					}

					static_assert(sizeof(zonetool::h1::XRigidVertList) == sizeof(zonetool::t7::XRigidVertList));
					static_assert(sizeof(zonetool::h1::XSurfaceCollisionTree) == sizeof(zonetool::t7::XSurfaceCollisionTree));
					static_assert(sizeof(zonetool::h1::XSurfaceCollisionNode) == sizeof(zonetool::t7::XSurfaceCollisionNode));
					static_assert(sizeof(zonetool::h1::XSurfaceCollisionLeaf) == sizeof(zonetool::t7::XSurfaceCollisionLeaf));

					//new_surf->rigidVertLists = reinterpret_cast<zonetool::h1::XRigidVertList*>(surf->vertList);
					new_surf->rigidVertLists = allocator.allocate_array<zonetool::h1::XRigidVertList>(surf->vertListCount);
					for (int vert_index = 0; vert_index < surf->vertListCount; vert_index++)
					{
						new_surf->rigidVertLists[vert_index].boneOffset = surf->vertList[vert_index].boneOffset << 6; // R_MarkFragments_AnimatedXModel
						new_surf->rigidVertLists[vert_index].vertCount = surf->vertList[vert_index].vertCount;
						new_surf->rigidVertLists[vert_index].triOffset = surf->vertList[vert_index].triOffset;
						new_surf->rigidVertLists[vert_index].triCount = surf->vertList[vert_index].triCount;

						new_surf->rigidVertLists[vert_index].collisionTree = reinterpret_cast<zonetool::h1::XSurfaceCollisionTree*>(surf->vertList[vert_index].collisionTree);
					}

					

					verts += surf->vertCount;
					indices += surf->triCount;
					positions += surf->vertCount;
					if ((surf->flags & zonetool::t7::XSURFACE_FLAG_SKINNED) != 0)
					{
						weights += surf->vertCount;
						skinVertCount += surf->vertCount;
					}

					baseVertOffset += surf->vertCount;
					baseIndexOffset += surf->triCount * 3;

					std::memcpy(new_surf->partBits, surf->partBits, sizeof(uint32_t) * 8);
				};

				for (auto i = 0; i < asset->numSurfs; i++)
				{
					if((asset->surfs[i].flags & zonetool::t7::XSURFACE_FLAG_SKINNED) != 0)
						parse_surf(i);
				}

				for (auto i = 0; i < asset->numSurfs; i++)
				{
					if ((asset->surfs[i].flags & zonetool::t7::XSURFACE_FLAG_SKINNED) == 0)
						parse_surf(i);
				}

				assert(asset->shared->skinVertCount == skinVertCount);

				std::memcpy(new_asset->partBits, asset->partBits, sizeof(uint32_t) * 8);
				return new_asset;
			}

			void dump(XModelMesh* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				if (!converted_asset) return;
				zonetool::h1::xsurface::dump(converted_asset);
			}
		}
	}
}
