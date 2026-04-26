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

				new_asset->hasLods = asset->numLods > 1;
				new_asset->maxLoadedLod = 0;
				COPY_VALUE(numLods);
				COPY_VALUE(collLod);
				new_asset->shadowCutoffLod = 0xFF;
				new_asset->characterCollBoundsType = 0;

				new_asset->flags = asset->flags;

				COPY_VALUE(numBones);
				COPY_VALUE(numRootBones);
				COPY_VALUE(numReactiveMotionParts);
				new_asset->numClientBones = 0; // bo3 uses this value, so this probably matters..
				new_asset->numsurfs = asset->numsurfs;

				COPY_VALUE(scale);
				COPY_VALUE(numCollSurfs);
				COPY_VALUE(contents);
				COPY_VALUE(radius);
				COPY_VALUE_CAST(bounds);
				COPY_VALUE(memUsage);

				new_asset->physicsLODDataSize = 0;
				new_asset->physicsUsageCounter = {};
				new_asset->numAimAssistBones = 0;
				new_asset->edgeLength = 0.0f;

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
					const auto material = allocator.allocate<zonetool::iw7::Material>();
					material->name = asset->materialHandles[i]->name;
					new_asset->materialHandles[i] = material;
				}

				for (auto i = 0; i < asset->numLods; i++)
				{
					new_asset->lodInfo[i].dist = asset->lodInfo[i].dist;
					new_asset->lodInfo[i].numsurfs = asset->lodInfo[i].numsurfs;
					new_asset->lodInfo[i].surfIndex = asset->lodInfo[i].surfIndex;
					std::memcpy(new_asset->lodInfo[i].partBits, asset->lodInfo[i].partBits, sizeof(new_asset->lodInfo[i].partBits));
					new_asset->lodInfo[i].subdivLodValidMask = asset->lodInfo[i].subdivLodValidMask;
					new_asset->lodInfo[i].flags = asset->lodInfo[i].flags;

					new_asset->lodInfo[i].modelSurfs = reinterpret_cast<zonetool::iw7::XModelSurfs*>(asset->lodInfo[i].modelSurfs);
					new_asset->lodInfo[i].surfs = reinterpret_cast<zonetool::iw7::XSurface*>(asset->lodInfo[i].surfs);
				}

				REINTERPRET_CAST_SAFE(collSurfs);
				REINTERPRET_CAST_SAFE(boneInfo);
				REINTERPRET_CAST_SAFE(invHighMipRadius);

				new_asset->physicsAsset = nullptr;
				new_asset->physicsFXShape = nullptr;

				new_asset->physicsLODData = nullptr;
				new_asset->physicsLODDataNameCount = 0;
				new_asset->physicsLODDataNames = nullptr;

				new_asset->impactType = 0;
				std::memset(new_asset->unk_03, 0, sizeof(new_asset->unk_03));

				// not sure if this is needed or anything, but set these anyways
				new_asset->unknown03Count = 0;
				new_asset->unknown03 = nullptr;
				new_asset->unknownIndex = 0;
				new_asset->unknownVec3Count = 0;
				std::memset(new_asset->unk_04, 0, sizeof(new_asset->unk_04));
				new_asset->unknownVec3 = nullptr;
				new_asset->unknownIndex2 = 0;
				new_asset->unknown04Count = 0;
				std::memset(new_asset->unk_05, 0, sizeof(new_asset->unk_05));
				new_asset->unknown04 = nullptr;

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
