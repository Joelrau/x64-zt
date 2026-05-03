#include <std_include.hpp>
#include "zonetool/h1/converter/iw7/include.hpp"
#include "xmodel.hpp"

#include "zonetool/iw7/assets/xmodel.hpp"

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace xmodel
		{
			zonetool::iw7::XModel* convert(zonetool::h1::XModel* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::iw7::XModel>();

				COPY_VALUE(name);

				new_asset->hasLods = asset->numLods ? 1 : 0;
				new_asset->maxLoadedLod = 0;
				COPY_VALUE(numLods);
				COPY_VALUE(collLod);
				new_asset->shadowCutoffLod = 6; // iw5->iw7 sentinel; 0xFF caused models to fade out
				new_asset->characterCollBoundsType = 1; // CharCollBoundsType_Human

				new_asset->flags = asset->flags;

				COPY_VALUE(numBones);
				COPY_VALUE(numRootBones);
				new_asset->numReactiveMotionParts = asset->numReactiveMotionParts;
				new_asset->numsurfs = asset->numsurfs;

				COPY_VALUE(scale);

				COPY_VALUE(numCollSurfs);
				new_asset->collSurfs = allocator.allocate_array<zonetool::iw7::XModelCollSurf_s>(asset->numCollSurfs);
				for (auto i = 0; i < asset->numCollSurfs; i++)
				{
					memcpy(&new_asset->collSurfs[i].bounds, &asset->collSurfs[i].bounds, sizeof(float[2][3]));

					new_asset->collSurfs[i].boneIdx = asset->collSurfs[i].boneIdx;
					new_asset->collSurfs[i].contents = asset->collSurfs[i].contents;
					new_asset->collSurfs[i].surfFlags = asset->collSurfs[i].surfFlags; // convert
				}

				COPY_VALUE(contents);
				COPY_VALUE(radius);
				COPY_VALUE_CAST(bounds);
				COPY_VALUE(memUsage);

				COPY_ARR(noScalePartBits);

				REINTERPRET_CAST_SAFE(boneNames);
				REINTERPRET_CAST_SAFE(parentList);
				REINTERPRET_CAST_SAFE(tagAngles);
				REINTERPRET_CAST_SAFE(tagPositions);
				REINTERPRET_CAST_SAFE(partClassification);
				REINTERPRET_CAST_SAFE(baseMat);
				REINTERPRET_CAST_SAFE(reactiveMotionParts);
				REINTERPRET_CAST_SAFE(reactiveMotionTweaks);

				new_asset->materialHandles = allocator.allocate_array<zonetool::iw7::Material*>(asset->numsurfs);
				for (auto i = 0; i < asset->numsurfs; i++)
				{
					if (asset->materialHandles[i])
					{
						const auto material = allocator.allocate<zonetool::iw7::Material>();
						material->name = asset->materialHandles[i]->name;
						new_asset->materialHandles[i] = material;
					}
				}

				//for (auto i = 0; i < 6; i++)
				//{
				//	std::memset(&new_asset->lodInfo[i], 0, sizeof(new_asset->lodInfo[i]));
				//	new_asset->lodInfo[i].dist = 1000000.0f;
				//}

				for (auto i = 0; i < asset->numLods; i++)
				{
					new_asset->lodInfo[i].dist = asset->lodInfo[i].dist;
					new_asset->lodInfo[i].numsurfs = asset->lodInfo[i].numsurfs;
					new_asset->lodInfo[i].surfIndex = asset->lodInfo[i].surfIndex;
					new_asset->lodInfo[i].modelSurfs = allocator.allocate<zonetool::iw7::XModelSurfs>();
					new_asset->lodInfo[i].modelSurfs->name = allocator.duplicate_string(asset->lodInfo[i].modelSurfs->name);
					std::memcpy(new_asset->lodInfo[i].partBits, asset->lodInfo[i].partBits, sizeof(new_asset->lodInfo[i].partBits));

					new_asset->lodInfo[i].subdivLodValidMask = asset->lodInfo[i].subdivLodValidMask;
					new_asset->lodInfo[i].flags = asset->lodInfo[i].flags;
				}

				REINTERPRET_CAST_SAFE(boneInfo);

				//new_asset->invHighMipRadius = allocator.allocate_array<unsigned short>(asset->numsurfs);
				//for (unsigned char i = 0; i < asset->numsurfs; i++)
				//{
				//	new_asset->invHighMipRadius[i] = 0xFFFF;
				//}
				REINTERPRET_CAST_SAFE(invHighMipRadius);

				new_asset->physicsAsset = nullptr;
				new_asset->physicsFXShape = nullptr;

				new_asset->unknownIndex = 0xFF;
				new_asset->unknownIndex2 = 0xFF;

				return new_asset;
			}

			void dump(zonetool::h1::XModel* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::iw7::xmodel::dump(converted_asset);
			}
		}
	}
}
