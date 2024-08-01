#include <std_include.hpp>
#include "zonetool/t7/converter/iw7/include.hpp"
#include "xanim.hpp"

#include "zonetool/iw7/assets/xanim.hpp"

#include <utils/string.hpp>

namespace zonetool::t7
{
	namespace converter::iw7
	{
		namespace xanim
		{
			zonetool::iw7::XAnimParts* convert(XAnimParts* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::iw7::XAnimParts>();

				REINTERPRET_CAST_SAFE(name);

				new_asset->dataByteCount = static_cast<unsigned short>(asset->dataByteCount);
				new_asset->dataShortCount = static_cast<unsigned short>(asset->dataShortCount);
				new_asset->dataIntCount = static_cast<unsigned short>(asset->dataIntCount);
				new_asset->randomDataShortCount = asset->randomDataShortCount;
				new_asset->randomDataByteCount = asset->randomDataByteCount;
				new_asset->randomDataIntCount = static_cast<unsigned short>(asset->randomDataIntCount);
				new_asset->numframes = asset->numframes;

				new_asset->flags = 0;
				if (asset->bLoop)
				{
					new_asset->flags |= zonetool::iw7::ANIM_LOOP;
				}
				if (asset->bDelta)
				{
					new_asset->flags |= zonetool::iw7::ANIM_DELTA;
				}
				if (asset->bDelta3D)
				{
					new_asset->flags |= zonetool::iw7::ANIM_DELTA_3D;
				}

				for (auto i = 0; i < 10; i++)
				{
					assert(asset->boneCount[i] <= std::numeric_limits<unsigned char>::max());
					new_asset->boneCount[i] = static_cast<unsigned char>(asset->boneCount[i]);
				}

				new_asset->notifyCount = asset->notifyCount;
				new_asset->assetType = asset->assetType;
				//new_asset->ikType = asset->ikType;
				new_asset->indexCount = asset->indexCount;
				new_asset->framerate = asset->framerate;
				new_asset->frequency = asset->frequency;
				new_asset->names = allocator.allocate_array<zonetool::iw7::scr_string_t>(new_asset->boneCount[9]);
				for (auto i = 0; i < new_asset->boneCount[9]; i++)
				{
					new_asset->names[i] = static_cast<zonetool::iw7::scr_string_t>(asset->names[i]);
				}
				new_asset->dataByte = reinterpret_cast<char*>(asset->dataByte);
				new_asset->dataShort = reinterpret_cast<short*>(asset->dataShort);
				new_asset->dataInt = reinterpret_cast<int*>(asset->dataInt);
				new_asset->randomDataShort = reinterpret_cast<short*>(asset->randomDataShort);
				new_asset->randomDataByte = reinterpret_cast<char*>(asset->randomDataByte);
				new_asset->randomDataInt = reinterpret_cast<int*>(asset->randomDataInt);

				new_asset->indices.data = reinterpret_cast<void*>(asset->indices.data);

				new_asset->notify = allocator.allocate_array<zonetool::iw7::XAnimNotifyInfo>(new_asset->notifyCount);
				for (auto i = 0; i < asset->notifyCount; i++)
				{
					new_asset->notify[i].name = static_cast<zonetool::iw7::scr_string_t>(asset->notify[i].type);
					new_asset->notify[i].time = asset->notify[i].time;
				}

				static_assert(sizeof(XAnimPartTrans) == sizeof(zonetool::iw7::XAnimPartTrans));
				static_assert(sizeof(XAnimDeltaPartQuat2) == sizeof(zonetool::iw7::XAnimDeltaPartQuat2));
				static_assert(sizeof(XAnimDeltaPartQuat) == sizeof(zonetool::iw7::XAnimDeltaPartQuat));
				REINTERPRET_CAST_SAFE(deltaPart);

				return new_asset;
			}

			void dump(XAnimParts* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::iw7::xanim_parts::dump(converted_asset);
			}
		}
	}
}
