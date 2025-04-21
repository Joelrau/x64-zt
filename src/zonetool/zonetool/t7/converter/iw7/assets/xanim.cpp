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
				new_asset->assetType = 0; //asset->assetType;
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
				//new_asset->dataShort = reinterpret_cast<short*>(asset->dataShort);
				new_asset->dataInt = reinterpret_cast<int*>(asset->dataInt);
				//new_asset->randomDataShort = reinterpret_cast<short*>(asset->randomDataShort);
				new_asset->randomDataByte = reinterpret_cast<char*>(asset->randomDataByte);
				new_asset->randomDataInt = reinterpret_cast<int*>(asset->randomDataInt);

				// Credits: Scobalula/Greyhound CoDXAnimTranslator::TranslateXAnim
				if (new_asset->dataShortCount)
				{
					new_asset->dataShort = allocator.allocate_array<short>(new_asset->dataShortCount);
					new_asset->randomDataShort = allocator.allocate_array<short>(new_asset->randomDataShortCount);

					std::memcpy(new_asset->dataShort, asset->dataShort, sizeof(short) * new_asset->dataShortCount);
					std::memcpy(new_asset->randomDataShort, asset->randomDataShort, sizeof(short) * new_asset->randomDataShortCount);

					short* newData = new_asset->dataShort;
					short* newRandom = new_asset->randomDataShort;
					short* data = asset->dataShort;
					short* random = asset->randomDataShort;

					auto readConvert = [](short*& dst, short*& src, uint32_t count)
					{
						for (uint32_t i = 0; i < count; ++i)
							*dst++ = QuatInt16::ToInt16(half_float::half_to_float(*src++));
					};

					auto readRaw = [](short*& dst, short*& src, uint32_t count = 1)
					{
						for (uint32_t i = 0; i < count; ++i)
							*dst++ = *src++;
					};

					const uint32_t frameSize = (asset->numframes > 255) ? 2 : 1;

					auto processBoneData = [&](int boneCountIndex, int rotSize)
					{
						for (int i = 0; i < asset->boneCount[boneCountIndex]; ++i)
						{
							const uint16_t frameCount = *data;
							readRaw(newData, data); // Frame count

							if (frameSize == 2)
								readRaw(newData, data); // Frame index

							for (int f = 0; f < frameCount + 1; ++f)
								readConvert(newRandom, random, rotSize); // Rotation data
						}
					};

					processBoneData(TwoDRotatedBoneCount, 2);
					processBoneData(NormalRotatedBoneCount, 4);

					auto processStaticBoneData = [&](int boneCountIndex, int rotSize)
					{
						for (int i = 0; i < asset->boneCount[boneCountIndex]; ++i)
							readConvert(newData, data, rotSize); // Static rotation data
					};

					processStaticBoneData(TwoDStaticRotatedBoneCount, 2);
					processStaticBoneData(NormalStaticRotatedBoneCount, 4);
				}

				new_asset->indices.data = reinterpret_cast<void*>(asset->indices.data);

				new_asset->notify = allocator.allocate_array<zonetool::iw7::XAnimNotifyInfo>(new_asset->notifyCount);
				for (auto i = 0; i < asset->notifyCount; i++)
				{
					new_asset->notify[i].name = static_cast<zonetool::iw7::scr_string_t>(asset->notify[i].param1);
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
