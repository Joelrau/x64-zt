#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "xanim.hpp"

#include "zonetool/h1/assets/xanim.hpp"

#include "zonetool/iw6/functions.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace xanim
		{
			zonetool::h1::XAnimParts* convert(XAnimParts* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::XAnimParts>();

				REINTERPRET_CAST_SAFE(name);

				COPY_VALUE(dataByteCount);
				COPY_VALUE(dataShortCount);
				COPY_VALUE(dataIntCount);
				COPY_VALUE(numframes);
				COPY_VALUE(flags); // convert?
				memcpy(new_asset->boneCount, asset->boneCount, sizeof(char[10]));
				new_asset->boneCount[10] = 0;
				new_asset->boneCount[11] = 0;
				COPY_VALUE(notifyCount);
				COPY_VALUE(assetType);
				COPY_VALUE(ikType);
				new_asset->randomDataByteCount = static_cast<int>(asset->randomDataByteCount);
				new_asset->randomDataShortCount = static_cast<int>(asset->randomDataShortCount);
				new_asset->randomDataIntCount = static_cast<int>(asset->randomDataIntCount);
				COPY_VALUE(indexCount);
				COPY_VALUE(framerate);
				COPY_VALUE(frequency);
				REINTERPRET_CAST_SAFE(names);
				REINTERPRET_CAST_SAFE(dataByte);
				REINTERPRET_CAST_SAFE(dataShort);
				REINTERPRET_CAST_SAFE(dataInt);
				REINTERPRET_CAST_SAFE(randomDataShort);
				REINTERPRET_CAST_SAFE(randomDataByte);
				REINTERPRET_CAST_SAFE(randomDataInt);
				new_asset->indices._1 = asset->indices._1;
				REINTERPRET_CAST_SAFE(notify);
				REINTERPRET_CAST_SAFE(deltaPart);

				new_asset->secondaryName = nullptr;
				new_asset->alternateAnimWeight = 0;
				new_asset->numAlternatives = 0;
				new_asset->blendShapeCount = 0;
				new_asset->blendShapeWeightNames = nullptr;
				new_asset->blendShapeCoefficientMagnitudes = nullptr;
				new_asset->numberOfBlendShapeKeys = nullptr;
				new_asset->blendShapeKeys = nullptr;
				new_asset->compressedBlendShapeCoefficients = nullptr;
				new_asset->blendShapeWeights = nullptr;
				new_asset->alternativeAnim = 0;
				new_asset->svAmimData = nullptr;

				return new_asset;
			}

			void dump(XAnimParts* asset)
			{
				utils::memory::allocator allocator;
				auto* converted_asset = convert(asset, allocator);
				zonetool::h1::xanim_parts::dump(converted_asset);
			}
		}
	}
}