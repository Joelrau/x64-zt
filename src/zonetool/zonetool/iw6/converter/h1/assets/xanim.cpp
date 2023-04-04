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
			zonetool::h1::XAnimParts* convert(XAnimParts* asset, ZoneMemory* mem)
			{
				auto* new_asset = mem->Alloc<zonetool::h1::XAnimParts>();

				REINTERPRET_CAST_SAFE(name);

				COPY_VALUE(dataByteCount);
				COPY_VALUE(dataShortCount);
				COPY_VALUE(dataIntCount);
				COPY_VALUE(numframes);
				COPY_VALUE(flags); // convert?
				COPY_ARR(boneCount);
				new_asset->u1 = 0;
				new_asset->u2 = 0;
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
				new_asset->u3 = 0;
				new_asset->u4 = 0;
				new_asset->blendShapeWeightCount = 0;
				new_asset->blendShapeWeightNames = nullptr;
				new_asset->blendShapeWeightUnknown1 = nullptr;
				new_asset->blendShapeWeightUnknown2 = nullptr;
				new_asset->blendShapeWeightUnknown3 = nullptr;
				new_asset->blendShapeWeightUnknown4 = nullptr;
				new_asset->blendShapeWeights = nullptr;
				new_asset->u5 = 0;
				new_asset->scriptedViewmodelAnimData = nullptr;

				return new_asset;
			}

			void dump(XAnimParts* asset, ZoneMemory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::IXAnimParts::dump(converted_asset);
			}
		}
	}
}