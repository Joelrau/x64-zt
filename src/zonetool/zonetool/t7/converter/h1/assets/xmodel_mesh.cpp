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
				unsigned short a = 0;
				unsigned short b = 0;
				unsigned short index = 1;
				for (short s = 0; s < (surf->blendVertCounts[0]); s++)
				{
					auto& table = surf->blendVertsTable[a];

					table.b[0] = surf->blendVerts[b + 0] / 64;
					table.b[1] = 0;
					table.b[2] = 0;
					table.b[3] = 0;
					table.b[4] = 0;
					table.b[5] = 0;
					table.b[6] = 0;
					table.b[7] = 0;

					table.b[8] = 0;
					table.b[9] = 0;
					table.b[10] = 0;
					table.b[11] = 0;
					table.b[12] = 0;
					table.b[13] = 0;
					table.b[14] = 0;

					table.blendVertCountIndex = index;

					a++;
					b += 1;
				}
				index++;

				for (short s = 0; s < (surf->blendVertCounts[1]); s++)
				{
					auto& table = surf->blendVertsTable[a];

					table.b[0] = surf->blendVerts[b + 0] / 64;
					table.b[1] = surf->blendVerts[b + 1] / 64;
					table.b[2] = 0;
					table.b[3] = 0;
					table.b[4] = 0;
					table.b[5] = 0;
					table.b[6] = 0;
					table.b[7] = 0;

					table.b[8] = surf->blendVerts[b + 2];
					table.b[9] = 0;
					table.b[10] = 0;
					table.b[11] = 0;
					table.b[12] = 0;
					table.b[13] = 0;
					table.b[14] = 0;

					table.blendVertCountIndex = index;

					a++;
					b += 3;
				}
				index++;

				for (short s = 0; s < (surf->blendVertCounts[2]); s++)
				{
					auto& table = surf->blendVertsTable[a];

					table.b[0] = surf->blendVerts[b + 0] / 64;
					table.b[1] = surf->blendVerts[b + 1] / 64;
					table.b[2] = surf->blendVerts[b + 3] / 64;
					table.b[3] = 0;
					table.b[4] = 0;
					table.b[5] = 0;
					table.b[6] = 0;
					table.b[7] = 0;

					table.b[8] = surf->blendVerts[b + 2];
					table.b[9] = surf->blendVerts[b + 4];
					table.b[10] = 0;
					table.b[11] = 0;
					table.b[12] = 0;
					table.b[13] = 0;
					table.b[14] = 0;

					table.blendVertCountIndex = index;

					a++;
					b += 5;
				}
				index++;

				for (short s = 0; s < (surf->blendVertCounts[3]); s++)
				{
					auto& table = surf->blendVertsTable[a];

					table.b[0] = surf->blendVerts[b + 0] / 64;
					table.b[1] = surf->blendVerts[b + 1] / 64;
					table.b[2] = surf->blendVerts[b + 3] / 64;
					table.b[3] = surf->blendVerts[b + 5] / 64;
					table.b[4] = 0;
					table.b[5] = 0;
					table.b[6] = 0;
					table.b[7] = 0;

					table.b[8] = surf->blendVerts[b + 2];
					table.b[9] = surf->blendVerts[b + 4];
					table.b[10] = surf->blendVerts[b + 6];
					table.b[11] = 0;
					table.b[12] = 0;
					table.b[13] = 0;
					table.b[14] = 0;

					table.blendVertCountIndex = index;

					a++;
					b += 7;
				}
				index++;
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
				for (auto i = 0; i < new_asset->numsurfs; i++)
				{
					auto* surf = &asset->surfs[i];
					auto* new_surf = &new_asset->surfs[i];

					new_surf->flags = 0;
					new_surf->flags |= ((surf->flags & zonetool::t7::XSURFACE_FLAG_SKINNED) != 0) ? zonetool::h1::SURF_FLAG_SKINNED : 0;
					new_surf->flags |= zonetool::h1::SURF_FLAG_VERTCOL_NONE;

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
						new_surf->triIndices2 = reinterpret_cast<zonetool::h1::Face*>(indices);

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
								new_surf->blendVerts = allocator.manual_allocate<zonetool::h1::XBlendInfo>(2 * total_blend_verts);
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

					new_surf->rigidVertLists = reinterpret_cast<zonetool::h1::XRigidVertList*>(surf->vertList);

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
				zonetool::h1::xsurface::dump(converted_asset);
			}
		}
	}
}
