#include <std_include.hpp>

#include "../include.hpp"
#include "xmodel.hpp"

namespace zonetool::iw4::converter::iw5
{
	namespace xmodel
	{
		namespace
		{
			zonetool::iw5::XModelSurfs* convert_surfaces(const zonetool::iw4::XModelSurfs* asset, x86::allocator& allocator)
			{
				static_assert(sizeof(zonetool::iw4::GfxPackedVertex) == sizeof(zonetool::iw5::GfxPackedVertex));
				static_assert(sizeof(zonetool::iw4::XRigidVertList) == sizeof(zonetool::iw5::XRigidVertList));
				static_assert(sizeof(zonetool::iw4::XSurfaceVertexInfo) == sizeof(zonetool::iw5::XSurfaceVertexInfo));

				const auto new_asset = allocator.allocate<zonetool::iw5::XModelSurfs>();
				new_asset->name.value = asset->name.value;
				new_asset->numsurfs = asset->numsurfs;
				std::memcpy(new_asset->partBits, asset->partBits, sizeof(asset->partBits));

				const auto surfs = allocator.allocate_array<zonetool::iw5::XSurface>(asset->numsurfs);
				new_asset->surfs = surfs;
				for (unsigned short i = 0; i < asset->numsurfs; i++)
				{
					const auto& surface = asset->surfs.get()[i];
					auto& new_surface = surfs[i];

					new_surface.flags = zonetool::iw5::SURF_FLAG_VERTCOL_NONE;
					if (surface.deformed)
					{
						new_surface.flags |= zonetool::iw5::SURF_FLAG_SKINNED;
					}

					new_surface.tileMode = static_cast<unsigned char>(surface.tileMode);
					new_surface.vertCount = surface.vertCount;
					new_surface.triCount = surface.triCount;
					new_surface.zoneHandle = 0;
					new_surface.baseTriIndex = surface.baseTriIndex;
					new_surface.baseVertIndex = surface.baseVertIndex;
					new_surface.quantizeScale = 0.0f;
					new_surface.triIndices.value = surface.triIndices.value;
					std::memcpy(&new_surface.vertInfo, &surface.vertexInfo, sizeof(new_surface.vertInfo));
					new_surface.verts0.verts0.value = surface.verticies.value;
					new_surface.vertListCount = static_cast<unsigned int>(surface.vertListCount);
					new_surface.vertList.value = surface.rigidVertLists.value;
					std::memcpy(new_surface.partBits, surface.partBits, sizeof(surface.partBits));
				}

				return new_asset;
			}
		}

		zonetool::iw5::XModel* convert(zonetool::iw4::XModel* asset, x86::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw5::XModel>();

			COPY_POINTER_RAW(name);
			COPY_VALUE(numBones);
			COPY_VALUE(numRootBones);
			new_asset->numsurfs = asset->numSurfaces;
			COPY_VALUE(scale);
			COPY_ARR(noScalePartBits);
			new_asset->boneNames.value = asset->boneNames.value;
			COPY_POINTER_RAW(parentList);
			new_asset->quats.value = asset->tagAngles.value;
			new_asset->trans.value = asset->tagPositions.value;
			new_asset->partClassification.value = asset->partClassification.value;
			new_asset->baseMat.value = asset->animMatrix.value;
			new_asset->materialHandles.value = asset->materials.value;

			for (auto i = 0; i < 4; i++)
			{
				const auto& lod = asset->lods[i];
				auto& new_lod = new_asset->lodInfo[i];

				new_lod.dist = lod.dist;
				new_lod.numsurfs = static_cast<unsigned short>(lod.numSurfacesInLod);
				new_lod.surfIndex = static_cast<unsigned short>(lod.surfIndex);
				std::memcpy(new_lod.partBits, lod.partBits, sizeof(lod.partBits));
				new_lod.lod = lod.lod;

				if (i < asset->numLods && lod.surfaces)
				{
					new_lod.modelSurfs = convert_surfaces(lod.surfaces.get(), allocator);
				}
			}

			new_asset->maxLoadedLod = asset->maxLoadedLod;
			new_asset->numLods = static_cast<unsigned char>(asset->numLods);
			new_asset->collLod = static_cast<unsigned char>(asset->collLod);
			new_asset->flags = static_cast<unsigned char>(asset->flags);

			static_assert(sizeof(zonetool::iw4::XModelCollSurf_s) == sizeof(zonetool::iw5::XModelCollSurf_s));
			new_asset->collSurfs.value = asset->collSurfs.value;
			new_asset->numCollSurfs = asset->numColSurfs;
			COPY_VALUE(contents);
			static_assert(sizeof(zonetool::iw4::XBoneInfo) == sizeof(zonetool::iw5::XBoneInfo));
			new_asset->boneInfo.value = asset->boneInfo.value;
			COPY_VALUE(radius);
			COPY_ARR(bounds);
			COPY_VALUE(memUsage);
			new_asset->physPreset.value = asset->physPreset.value;
			new_asset->physCollmap.value = asset->physCollmap.value;
			new_asset->quantization = 0.0f;

			return new_asset;
		}
	}
}
