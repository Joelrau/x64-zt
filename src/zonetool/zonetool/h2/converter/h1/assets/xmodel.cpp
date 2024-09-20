#include "std_include.hpp"

#include "zonetool/h2/converter/h1/include.hpp"
#include "xmodel.hpp"
#include "zonetool/h1/assets/xmodel.hpp"

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace xmodel
		{
			zonetool::h1::XModel* convert(zonetool::h2::XModel* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::XModel>();

				COPY_VALUE(name);
				COPY_VALUE(numBones);
				COPY_VALUE(numRootBones);
				COPY_VALUE(numsurfs);
				COPY_VALUE(lodRampType);
				COPY_VALUE(numBonePhysics);
				COPY_VALUE(numCompositeModels);
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
				new_asset->blendShapeCount = asset->targetCount; //COPY_VALUE(targetCount);
				new_asset->numberOfBlendShapeWeights = asset->numberOfWeights; //COPY_VALUE(numberOfWeights);
				new_asset->numberOfBlendShapeWeightMaps = asset->numberOfWeightMaps; //COPY_VALUE(numberOfWeightMaps);
				new_asset->blendShapeWeightNames = reinterpret_cast<zonetool::h1::scr_string_t*>(asset->weightNames); //REINTERPRET_CAST_SAFE(weightNames);
				new_asset->blendShapeWeightMaps = reinterpret_cast<zonetool::h1::BlendShapeWeightMap*>(asset->blendShapeWeightMap); //REINTERPRET_CAST_SAFE(blendShapeWeightMap);
				REINTERPRET_CAST_SAFE(physPreset);
				REINTERPRET_CAST_SAFE(physCollmap);
				COPY_VALUE(quantization);
				COPY_VALUE(memUsage);
				REINTERPRET_CAST_SAFE(skeletonScript);

				new_asset->compositeModels = allocator.allocate_array<zonetool::h1::XModel*>(asset->numCompositeModels);
				for (auto i = 0; i < asset->numCompositeModels; i++)
				{
					const auto model = allocator.allocate<zonetool::h1::XModel>();
					model->name = asset->compositeModels[i]->name;
					new_asset->compositeModels[i] = model;
				}

				REINTERPRET_CAST_SAFE(bonePhysics);

				return new_asset;
			}

			void dump(zonetool::h2::XModel* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::xmodel::dump(converted_asset);
			}
		}
	}
}
