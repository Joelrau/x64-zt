#include <std_include.hpp>
#include "zonetool/iw7/converter/h1/include.hpp"
#include "xsurface.hpp"

#include "zonetool/h1/assets/xsurface.hpp"

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace xsurface
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

			void ReconstructH1BlendVerts(const zonetool::iw7::XSurface* surf, zonetool::h1::XSurface* surf_, utils::memory::allocator& mem)
			{
				unsigned int count =
					surf->blendVertCounts[0]
					+ 3 * surf->blendVertCounts[1]
					+ 5 * surf->blendVertCounts[2]
					+ 7 * surf->blendVertCounts[3]
					+ 9 * surf->blendVertCounts[4]
					+ 11 * surf->blendVertCounts[5]
					+ 13 * surf->blendVertCounts[6]
					+ 15 * surf->blendVertCounts[7];

				if (!count)
					return;

				surf_->blendVerts = mem.manual_allocate<unsigned short>(count * 2);
				unsigned short* out = surf_->blendVerts;

				const unsigned short* in = surf->blendVerts;
				unsigned int a = 0;
				unsigned int b = 0;

				// Weights1
				for (unsigned short i = 0; i < surf->blendVertCounts[0]; ++i)
				{
					out[b++] = in[a++] * 64;
					++a; // skip 0
				}

				// Weights2
				for (unsigned short i = 0; i < surf->blendVertCounts[1]; ++i)
				{
					out[b++] = in[a++] * 64;
					out[b++] = in[a++] * 64;
					out[b++] = in[a++];
					++a; // skip 0
				}

				// Weights3
				for (unsigned short i = 0; i < surf->blendVertCounts[2]; ++i)
				{
					out[b++] = in[a++] * 64;
					out[b++] = in[a++] * 64;
					out[b++] = in[a++];
					out[b++] = in[a++] * 64;
					out[b++] = in[a++];
					++a; // skip 0
				}

				// Weights4
				for (unsigned short i = 0; i < surf->blendVertCounts[3]; ++i)
				{
					out[b++] = in[a++] * 64;
					out[b++] = in[a++] * 64;
					out[b++] = in[a++];
					out[b++] = in[a++] * 64;
					out[b++] = in[a++];
					out[b++] = in[a++] * 64;
					out[b++] = in[a++];
					++a; // skip 0
				}

				assert(b == count);
			}

			zonetool::h1::XModelSurfs* convert(XModelSurfs* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::XModelSurfs>();

				REINTERPRET_CAST_SAFE(name);

				COPY_VALUE(numsurfs);

				new_asset->surfs = allocator.allocate_array<zonetool::h1::XSurface>(asset->numsurfs);
				for (unsigned short i = 0; i < asset->numsurfs; i++)
				{
					auto* new_surf = &new_asset->surfs[i];
					auto* surf = &asset->surfs[i];

					COPY_VALUE(surfs[i].flags); // convert?
					COPY_VALUE(surfs[i].vertCount);
					COPY_VALUE(surfs[i].triCount);
					COPY_VALUE(surfs[i].rigidVertListCount);
					COPY_VALUE(surfs[i].subdivLevelCount);
					COPY_ARR(surfs[i].blendVertCounts);
					COPY_VALUE(surfs[i].vertCount);
					if ((surf->flags & 8) != 0)
					{
						REINTERPRET_CAST_SAFE(surfs[i].verts0.packedMotionVerts0);
					}
					else
					{
						REINTERPRET_CAST_SAFE(surfs[i].verts0.packedVerts0);
					}

					REINTERPRET_CAST_SAFE(surfs[i].triIndices);
					new_surf->triIndices2 = allocator.allocate_array<zonetool::h1::Face>(surf->triCount); // ?
					for (unsigned short j = 0; j < surf->triCount; j++)
					{
						memcpy(&new_surf->triIndices2[j], &surf->triIndices[j], sizeof(zonetool::h1::Face));
					}

					// unknown
					new_surf->unknown0 = allocator.allocate_array<zonetool::h1::UnknownXSurface0>(surf->vertCount); // related to indices2?
					for (unsigned short j = 0; j < surf->vertCount; j++)
					{
						if ((surf->flags & 8) != 0)
						{
							new_surf->unknown0[j].xyz[0] = new_surf->verts0.packedMotionVerts0[j].xyz[0];
							new_surf->unknown0[j].xyz[1] = new_surf->verts0.packedMotionVerts0[j].xyz[1];
							new_surf->unknown0[j].xyz[2] = new_surf->verts0.packedMotionVerts0[j].xyz[2];
							new_surf->unknown0[j].normal = new_surf->verts0.packedMotionVerts0[j].normal;
						}
						else
						{
							new_surf->unknown0[j].xyz[0] = new_surf->verts0.packedVerts0[j].xyz[0];
							new_surf->unknown0[j].xyz[1] = new_surf->verts0.packedVerts0[j].xyz[1];
							new_surf->unknown0[j].xyz[2] = new_surf->verts0.packedVerts0[j].xyz[2];
							new_surf->unknown0[j].normal = new_surf->verts0.packedVerts0[j].normal;
						}
					}

					REINTERPRET_CAST_SAFE(surfs[i].rigidVertLists);

					// check
					ReconstructH1BlendVerts(surf, new_surf, allocator);

					new_surf->blendVertsTable = allocator.allocate_array<zonetool::h1::BlendVertsUnknown>(surf->vertCount);
					GenerateH1BlendVertsTable(new_surf);

					if (surf->lmapUnwrap)
					{
						new_surf->lmapUnwrap = allocator.allocate_array<zonetool::h1::alignVertBufFloat16Vec2_t>(surf->vertCount);
						for (int j = 0; j < surf->vertCount; j++)
						{
							// check (float -> short)
							new_surf->lmapUnwrap[j][0] = static_cast<unsigned short>(surf->lmapUnwrap[j][0]);
							new_surf->lmapUnwrap[j][1] = static_cast<unsigned short>(surf->lmapUnwrap[j][1]);
						}
					}

					REINTERPRET_CAST_SAFE(surfs[i].subdiv);
					REINTERPRET_CAST_SAFE(surfs[i].tensionData);
					REINTERPRET_CAST_SAFE(surfs[i].tensionAccumTable);

					new_surf->blendShapes = nullptr;
					new_surf->blendShapesCount = 0;

					new_surf->vertexLightingIndex = 0;

					COPY_ARR(surfs[i].partBits);
				}

				COPY_ARR(partBits);

				return new_asset;
			}

			void dump(XModelSurfs* asset)
			{
				utils::memory::allocator allocator;
				auto* converted_asset = convert(asset, allocator);
				zonetool::h1::xsurface::dump(converted_asset);
			}
		}
	}
}