#include <std_include.hpp>
#include "zonetool/iw7/converter/h1/include.hpp"
#include "xmodel.hpp"

#include "zonetool/h1/assets/xmodel.hpp"

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace xmodel
		{
			zonetool::h1::XModel* convert(zonetool::iw7::XModel* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::XModel>();

				const auto numBonesTotal = asset->numBones + asset->numClientBones;
				if (numBonesTotal > 256)
				{
					ZONETOOL_WARNING("model %s has too many bones (%d), this is untested and might not work!", asset->name, numBonesTotal);
					assert(numBonesTotal < 256);
				}

				COPY_VALUE(name);
				new_asset->numBones = static_cast<unsigned char>(numBonesTotal);
				COPY_VALUE(numRootBones);
				COPY_VALUE_CAST(numsurfs);
				new_asset->lodRampType = 0;
				COPY_VALUE(scale);
				COPY_ARR(noScalePartBits);
				REINTERPRET_CAST_SAFE(boneNames);
				REINTERPRET_CAST_SAFE(parentList);
				REINTERPRET_CAST_SAFE(tagAngles);
				REINTERPRET_CAST_SAFE(tagPositions);
				REINTERPRET_CAST_SAFE(partClassification);
				REINTERPRET_CAST_SAFE(baseMat);
				REINTERPRET_CAST_SAFE(reactiveMotionTweaks);

				new_asset->materialHandles = allocator.allocate_array<zonetool::h1::Material*>(asset->numsurfs);
				for (auto i = 0; i < asset->numsurfs; i++)
				{
					const auto material = allocator.allocate<zonetool::h1::Material>();
					material->name = asset->materialHandles[i]->name;
					new_asset->materialHandles[i] = material;
				}

				for (auto i = 0; i < asset->numLods; i++)
				{
					COPY_VALUE(lodInfo[i].dist);
					COPY_VALUE(lodInfo[i].numsurfs);
					COPY_VALUE(lodInfo[i].surfIndex);
					COPY_ARR(lodInfo[i].partBits);
					REINTERPRET_CAST_SAFE(lodInfo[i].modelSurfs);
					REINTERPRET_CAST(lodInfo[i].surfs);
				}

				COPY_VALUE(numLods);
				COPY_VALUE(collLod);
				new_asset->flags = asset->flags & 0x7FF;
				COPY_VALUE(numCollSurfs);
				REINTERPRET_CAST_SAFE(collSurfs);
				REINTERPRET_CAST_SAFE(boneInfo);
				COPY_VALUE(contents);
				COPY_VALUE(radius);
				COPY_VALUE_CAST(bounds);
				REINTERPRET_CAST_SAFE(invHighMipRadius);
				new_asset->mdaoVolumes = nullptr;
				new_asset->mdaoVolumeCount = 0;
				new_asset->blendShapeCount = 0;
				new_asset->numberOfBlendShapeWeights = 0;
				new_asset->numberOfBlendShapeWeightMaps = 0;
				new_asset->blendShapeWeightNames = nullptr;
				new_asset->blendShapeWeightMaps = nullptr;

				if (asset->physicsAsset)
				{
					new_asset->physPreset = allocator.allocate<zonetool::h1::PhysPreset>();
					new_asset->physPreset->name = "default";

					new_asset->physCollmap = allocator.allocate<zonetool::h1::PhysCollmap>();
					new_asset->physCollmap->name = allocator.duplicate_string(asset->physicsAsset->name);
				}

				new_asset->quantization = 0.0f;
				COPY_VALUE(memUsage);

				new_asset->skeletonScript = nullptr;

				new_asset->numCompositeModels = asset->unknown03Count;
				new_asset->compositeModels = allocator.allocate_array<zonetool::h1::XModel*>(new_asset->numCompositeModels);
				for (auto i = 0; i < new_asset->numCompositeModels; i++)
				{
					const auto model = allocator.allocate<zonetool::h1::XModel>();
					model->name = reinterpret_cast<const char*>(asset->unknown03[i]);
					new_asset->compositeModels[i] = model;
				}

				new_asset->numBonePhysics = 0;
				new_asset->bonePhysics = nullptr;

				return new_asset;
			}

			void dump(zonetool::iw7::XModel* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::xmodel::dump(converted_asset);
			}
		}
	}
}
