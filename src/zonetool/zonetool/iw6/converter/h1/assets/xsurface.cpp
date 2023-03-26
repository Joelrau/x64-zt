#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "xsurface.hpp"

#include "zonetool/h1/assets/xsurface.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace xsurface
		{
			zonetool::h1::XModelSurfs* convert(XModelSurfs* asset, ZoneMemory* mem)
			{
				auto* new_asset = mem->Alloc<zonetool::h1::XModelSurfs>();

				REINTERPRET_CAST_SAFE(name);

				COPY_VALUE(numsurfs);

				new_asset->surfs = mem->Alloc<zonetool::h1::XSurface>(asset->numsurfs);
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
					new_surf->triIndices2 = mem->Alloc<zonetool::h1::Face>(surf->triCount); // ?
					for (unsigned short j = 0; j < surf->triCount; j++)
					{
						memcpy(&new_surf->triIndices2[j], &surf->triIndices[j], sizeof(zonetool::h1::Face));
					}

					// unknown
					new_surf->unknown0 = mem->Alloc<zonetool::h1::UnknownXSurface0>(surf->vertCount); // related to indices2?
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
					REINTERPRET_CAST_SAFE(surfs[i].blendVerts);
					REINTERPRET_CAST_SAFE(surfs[i].blendVertsTable);

					if (surf->lmapUnwrap)
					{
						new_surf->lmapUnwrap = mem->Alloc<zonetool::h1::alignVertBufFloat16Vec2_t>(surf->vertCount);
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

					COPY_VALUE(surfs[i].vertexLightingIndex);

					COPY_ARR(surfs[i].partBits);
				}

				COPY_ARR(partBits);

				return new_asset;
			}

			void dump(XModelSurfs* asset, ZoneMemory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::IXSurface::dump(converted_asset);
			}
		}
	}
}