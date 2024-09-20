#include "std_include.hpp"

#include "zonetool/h1/converter/h2/include.hpp"
#include "xmodel.hpp"
#include "zonetool/h2/assets/xmodel.hpp"

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace xmodel
		{
			zonetool::h2::XModel* convert(zonetool::h1::XModel* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h2::XModel>();

				COPY_VALUE(name);
				COPY_VALUE(numBones);
				COPY_VALUE(numRootBones);
				COPY_VALUE(numsurfs);
				COPY_VALUE(lodRampType);
				COPY_VALUE(numBonePhysics);
				COPY_VALUE(numCompositeModels);
				new_asset->unk_float = -1.0f;
				COPY_VALUE(scale);
				COPY_ARR(noScalePartBits);
				REINTERPRET_CAST_SAFE(boneNames);
				REINTERPRET_CAST_SAFE(parentList);
				REINTERPRET_CAST_SAFE(tagAngles);
				REINTERPRET_CAST_SAFE(tagPositions);
				REINTERPRET_CAST_SAFE(partClassification);
				REINTERPRET_CAST_SAFE(baseMat);
				REINTERPRET_CAST_SAFE(reactiveMotionTweaks);

				new_asset->materialHandles = allocator.allocate_array<zonetool::h2::Material*>(asset->numsurfs);
				for (auto i = 0; i < asset->numsurfs; i++)
				{
					const auto material = allocator.allocate<zonetool::h2::Material>();
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
					REINTERPRET_CAST_SAFE(lodInfo[i].surfs);
				}

				COPY_VALUE(numLods);
				COPY_VALUE(collLod);
				COPY_VALUE(flags);
				COPY_VALUE(numCollSurfs);
				REINTERPRET_CAST_SAFE(collSurfs);
				REINTERPRET_CAST_SAFE(boneInfo);
				COPY_VALUE(contents);
				COPY_VALUE(radius);
				COPY_VALUE_CAST(bounds);
				REINTERPRET_CAST_SAFE(invHighMipRadius);
				REINTERPRET_CAST_SAFE(mdaoVolumes);
				COPY_VALUE(mdaoVolumeCount);
				new_asset->targetCount = asset->blendShapeCount;//COPY_VALUE(targetCount);
				new_asset->numberOfWeights = asset->numberOfBlendShapeWeights;//COPY_VALUE(numberOfWeights);
				new_asset->numberOfWeightMaps = asset->numberOfBlendShapeWeightMaps;//COPY_VALUE(numberOfWeightMaps);
				new_asset->weightNames = reinterpret_cast<zonetool::h2::scr_string_t*>(asset->blendShapeWeightNames);//REINTERPRET_CAST_SAFE(weightNames);
				new_asset->blendShapeWeightMap = reinterpret_cast<zonetool::h2::BlendShapeWeightMap*>(asset->blendShapeWeightMaps);//REINTERPRET_CAST_SAFE(blendShapeWeightMap);
				REINTERPRET_CAST_SAFE(physPreset);
				REINTERPRET_CAST_SAFE(physCollmap);
				COPY_VALUE(quantization);
				COPY_VALUE(memUsage);
				REINTERPRET_CAST_SAFE(skeletonScript);

				new_asset->compositeModels = allocator.allocate_array<zonetool::h2::XModel*>(asset->numCompositeModels);
				for (auto i = 0; i < asset->numCompositeModels; i++)
				{
					const auto model = allocator.allocate<zonetool::h2::XModel>();
					model->name = asset->compositeModels[i]->name;
					new_asset->compositeModels[i] = model;
				}

				REINTERPRET_CAST_SAFE(bonePhysics);

				return new_asset;
			}

			void dump(zonetool::h1::XModel* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h2::xmodel::dump(converted_asset);
			}
		}
	}
}
