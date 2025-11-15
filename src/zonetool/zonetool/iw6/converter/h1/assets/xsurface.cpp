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
			void R_FixSubdivRegularPatchFlags(zonetool::h1::XSurface* surf)
			{
				if (!surf || !surf->subdiv || !surf->subdivLevelCount)
					return;

				zonetool::h1::XSurfaceSubdivInfo* subdiv = surf->subdiv;

				if (subdiv->flags & 4)
					return;

				for (unsigned int levelIndex = 0; levelIndex < static_cast<unsigned int>(surf->subdivLevelCount); ++levelIndex)
				{
					zonetool::h1::XSurfaceSubdivLevel& level = subdiv->levels[levelIndex];

					for (unsigned int i = 0; i < level.regularPatchCount; ++i)
					{
						unsigned int flags = level.regularPatchFlags[i] & 0x3FF;

						if (flags & (1 << 8))
							flags |= 0x110000;

						if (flags & (1 << 9))
							flags |= 0x440000;

						level.regularPatchFlags[i] = flags;
					}
				}

				subdiv->flags |= 4;
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
					REINTERPRET_CAST_SAFE(surfs[i].blendVerts);
					REINTERPRET_CAST_SAFE(surfs[i].blendVertsTable);

					if (surf->lmapUnwrap)
					{
						new_surf->lmapUnwrap = allocator.allocate_array<zonetool::h1::alignVertBufFloat16Vec2_t>(surf->vertCount);
						for (int j = 0; j < surf->vertCount; j++)
						{
							new_surf->lmapUnwrap[j][0] = half_float::float_to_half(surf->lmapUnwrap[j][0]);
							new_surf->lmapUnwrap[j][1] = half_float::float_to_half(surf->lmapUnwrap[j][1]);
						}
					}
					
					REINTERPRET_CAST_SAFE(surfs[i].subdiv);
					REINTERPRET_CAST_SAFE(surfs[i].tensionData);
					REINTERPRET_CAST_SAFE(surfs[i].tensionAccumTable);
					
					new_surf->blendShapes = nullptr;
					new_surf->blendShapesCount = 0;

					COPY_VALUE(surfs[i].vertexLightingIndex);

					COPY_ARR(surfs[i].partBits);

					R_FixSubdivRegularPatchFlags(new_surf);
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