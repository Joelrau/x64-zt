#include <std_include.hpp>

#include "../include.hpp"
#include "xanim_parts.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace xanim_parts
	{
		zonetool::iw4::XAnimParts* convert(zonetool::iw3::XAnimParts* asset, x86::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw4::XAnimParts>();

			COPY_POINTER(name);
			COPY_VALUE(dataByteCount);
			COPY_VALUE(dataShortCount);
			COPY_VALUE(dataIntCount);
			COPY_VALUE(randomDataByteCount);
			COPY_VALUE(randomDataIntCount);
			new_asset->framecount = asset->numframes;

			new_asset->flags = 0;
			if (asset->bLoop)
			{
				new_asset->flags |= zonetool::iw4::ANIM_LOOP;
			}

			if (asset->bDelta)
			{
				new_asset->flags |= zonetool::iw4::ANIM_DELTA;
			}

			for (auto i = 0; i < 10; i++)
			{
				new_asset->boneCount[i] = static_cast<char>(asset->boneCount[i]);
			}

			new_asset->notifyCount = static_cast<char>(asset->notifyCount);
			new_asset->assetType = static_cast<char>(asset->assetType);
			COPY_VALUE(isDefault);
			COPY_VALUE(randomDataShortCount);
			new_asset->indexcount = asset->indexCount;
			COPY_VALUE(framerate);
			COPY_VALUE(frequency);
			new_asset->tagnames.value = asset->names.value;
			COPY_POINTER(dataByte);
			COPY_POINTER(dataShort);
			COPY_POINTER(dataInt);
			COPY_POINTER(randomDataShort);
			COPY_POINTER(randomDataByte);
			COPY_POINTER(randomDataInt);
			COPY_POINTER_RAW(indices.data);
			COPY_POINTER(notify);

			if (asset->deltaPart)
			{
				const auto delta = allocator.allocate<zonetool::iw4::XAnimDeltaPart>();
				new_asset->delta = delta;
				delta->trans.value = asset->deltaPart->trans.value;
				delta->quat2.value = asset->deltaPart->quat.value;
			}

			return new_asset;
		}
	}
}
