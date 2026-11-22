#include <std_include.hpp>

#include "../include.hpp"
#include "xmodel.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace xmodel
	{
		namespace
		{
			void convert_surface(const zonetool::iw3::XSurface* asset, zonetool::iw4::XSurface* new_asset)
			{
				static_assert(sizeof(zonetool::iw3::GfxPackedVertex) == sizeof(zonetool::iw4::GfxPackedVertex));
				static_assert(sizeof(zonetool::iw3::XRigidVertList) == sizeof(zonetool::iw4::XRigidVertList));
				static_assert(sizeof(zonetool::iw3::XSurfaceTri16) == sizeof(zonetool::iw4::Face));
				static_assert(sizeof(zonetool::iw3::XSurfaceVertexInfo) == sizeof(zonetool::iw4::XSurfaceVertexInfo));

				new_asset->tileMode = asset->tileMode;
				new_asset->deformed = asset->deformed;
				new_asset->vertCount = asset->vertCount;
				new_asset->triCount = asset->triCount;
				new_asset->zoneHandle = 0;
				new_asset->baseTriIndex = asset->baseTriIndex;
				new_asset->baseVertIndex = asset->baseVertIndex;
				new_asset->triIndices.value = asset->triIndices.value;
				std::memcpy(&new_asset->vertexInfo, &asset->vertInfo, sizeof(new_asset->vertexInfo));
				new_asset->verticies.value = asset->verts0.value;
				new_asset->vertListCount = static_cast<int>(asset->vertListCount);
				new_asset->rigidVertLists.value = asset->vertList.value;
				std::memcpy(new_asset->partBits, asset->partBits, sizeof(asset->partBits));
			}

			void compute_bounds(zonetool::iw4::Bounds& bounds, const float* mins, const float* maxs)
			{
				for (auto i = 0; i < 3; i++)
				{
					bounds.halfSize[i] = (maxs[i] - mins[i]) / 2.0f;
					bounds.midPoint[i] = bounds.halfSize[i] + mins[i];
				}
			}
		}

		zonetool::iw4::XModel* convert(zonetool::iw3::XModel* asset, x86::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw4::XModel>();

			COPY_POINTER_RAW(name);
			COPY_VALUE(numBones);
			COPY_VALUE(numRootBones);
			new_asset->numSurfaces = asset->numsurfs;
			COPY_VALUE(lodRampType);
			new_asset->scale = 1.0f;

			COPY_POINTER_RAW(boneNames);
			COPY_POINTER_RAW(parentList);
			COPY_POINTER_RAW(partClassification);
			static_assert(sizeof(zonetool::iw3::XModelQuat) == sizeof(zonetool::iw4::XModelAngle));
			new_asset->tagAngles.value = asset->quats.value;
			new_asset->tagPositions.value = asset->trans.value;
			static_assert(sizeof(zonetool::iw3::DObjAnimMat) == sizeof(zonetool::iw4::DObjAnimMat));
			new_asset->animMatrix.value = asset->baseMat.value;
			new_asset->materials.value = asset->materialHandles.value;

			for (auto i = 0; i < asset->numLods && i < 4; i++)
			{
				const auto& lod = asset->lodInfo[i];
				auto& new_lod = new_asset->lods[i];

				new_lod.dist = lod.dist;
				new_lod.numSurfacesInLod = static_cast<short>(lod.numsurfs);
				new_lod.surfIndex = static_cast<short>(lod.surfIndex);
				std::memcpy(new_lod.partBits, lod.partBits, sizeof(lod.partBits));
				new_lod.lod = lod.lod;

				const auto surfaces = allocator.allocate<zonetool::iw4::XModelSurfs>();
				const auto name = std::string(asset->name.get()) + "_lod" + std::to_string(i);
				surfaces->name = allocator.duplicate_string(name);
				surfaces->numsurfs = lod.numsurfs;
				std::memcpy(surfaces->partBits, lod.partBits, sizeof(lod.partBits));

				const auto surfs = allocator.allocate_array<zonetool::iw4::XSurface>(lod.numsurfs);
				surfaces->surfs = surfs;
				for (auto surf = 0; surf < lod.numsurfs; surf++)
				{
					convert_surface(&asset->surfs.get()[lod.surfIndex + surf], &surfs[surf]);
				}

				new_lod.surfaces = surfaces;
			}

			new_asset->numLods = static_cast<char>(asset->numLods);
			new_asset->collLod = static_cast<char>(asset->collLod);
			new_asset->flags = static_cast<char>(asset->flags);

			new_asset->numColSurfs = asset->numCollSurfs;
			new_asset->contents = asset->contents;
			if (asset->collSurfs && asset->numCollSurfs > 0)
			{
				const auto surfaces = allocator.allocate_array<zonetool::iw4::XModelCollSurf_s>(asset->numCollSurfs);
				for (auto i = 0; i < asset->numCollSurfs; i++)
				{
					const auto& source = asset->collSurfs.get()[i];
					surfaces[i].collTris.value = source.collTris.value;
					surfaces[i].numCollTris = source.numCollTris;
					compute_bounds(surfaces[i].bounds, source.mins, source.maxs);
					surfaces[i].boneIdx = source.boneIdx;
					surfaces[i].contents = source.contents;
					surfaces[i].surfFlags = source.surfFlags;
				}

				new_asset->collSurfs = surfaces;
			}

			if (asset->boneInfo && asset->numBones > 0)
			{
				const auto infos = allocator.allocate_array<zonetool::iw4::XBoneInfo>(asset->numBones);
				for (auto i = 0; i < asset->numBones; i++)
				{
					const auto& source = asset->boneInfo.get()[i];
					infos[i].radiusSquared = source.radiusSquared;
					compute_bounds(infos[i].packedBounds, source.bounds[0], source.bounds[1]);
					for (auto axis = 0; axis < 3; axis++)
					{
						infos[i].packedBounds.midPoint[axis] += source.offset[axis];
					}
				}

				new_asset->boneInfo = infos;
			}

			COPY_VALUE(radius);
			new_asset->memUsage = asset->memUsage;
			new_asset->bad = asset->bad;
			new_asset->physPreset.value = asset->physPreset.value;
			compute_bounds(new_asset->bounds, asset->mins, asset->maxs);

			return new_asset;
		}
	}
}
