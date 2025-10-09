#include <std_include.hpp>
#include "zonetool/t7/converter/h1/include.hpp"
#include "xanim.hpp"

#include "zonetool/h1/assets/xanim.hpp"

#include <utils/string.hpp>
#include <utils/io.hpp>

namespace zonetool::t7
{
	namespace converter::h1
	{
		namespace xanim
		{
			zonetool::h1::XAnimParts* convert(XAnimParts* asset, utils::memory::allocator& allocator)
			{
				using u8 = unsigned char;
				using u16 = unsigned short;
				using u32 = unsigned int;

				auto* new_asset = allocator.allocate<zonetool::h1::XAnimParts >();
				std::memset(new_asset, 0, sizeof(*new_asset));
				new_asset->name = asset->name;

				auto fit_u16 = [](u32 v)->u16
				{
					assert(v <= std::numeric_limits<u16>::max());
					return static_cast<u16> (v);
				};

				new_asset->dataByteCount = fit_u16(asset->dataByteCount);
				new_asset->dataShortCount = fit_u16(asset->dataShortCount);
				new_asset->dataIntCount = fit_u16(asset->dataIntCount);
				new_asset->randomDataShortCount = asset->randomDataShortCount;
				new_asset->randomDataByteCount = asset->randomDataByteCount;
				new_asset->randomDataIntCount = asset->randomDataIntCount;
				new_asset->numframes = asset->numframes;

				new_asset->flags = 0;
				if (asset->bLoop) new_asset->flags |= zonetool::h1::ANIM_LOOP;
				if (asset->bDelta) new_asset->flags |= zonetool::h1::ANIM_DELTA;
				if (asset->bDelta3D) new_asset->flags |= zonetool::h1::ANIM_DELTA_3D;

				for (int i = 0; i < 10; ++i)
				{
					assert(asset->boneCount[i] <= std::numeric_limits<u8>::max());
					new_asset->boneCount[i] = static_cast<u8>(asset->boneCount[i]);
				}

				new_asset->notifyCount = asset->notifyCount;
				new_asset->assetType = asset->assetType;
				new_asset->framerate = asset->framerate;
				new_asset->frequency = asset->frequency;

				const int totalBones = new_asset->boneCount[TotalBoneCount];
				if (totalBones > 0)
				{
					new_asset->names = allocator.allocate_array<zonetool::h1::scr_string_t >(totalBones);
					for (int i = 0; i < totalBones; ++i)
					{
						new_asset->names[i] = static_cast<zonetool::h1::scr_string_t>(asset->names[i]);
					}
				}

				if (new_asset->dataByteCount)
				{
					new_asset->dataByte = allocator.allocate_array<char>(new_asset->dataByteCount);
					std::memcpy(new_asset->dataByte, asset->dataByte, new_asset->dataByteCount);
				}

				if (new_asset->dataIntCount)
				{
					new_asset->dataInt = allocator.allocate_array<int>(new_asset->dataIntCount);
					std::memcpy(new_asset->dataInt, asset->dataInt, new_asset->dataIntCount * sizeof(int));
				}

				if (new_asset->randomDataByteCount)
				{
					new_asset->randomDataByte = allocator.allocate_array<u8>(new_asset->randomDataByteCount);
					std::memcpy(new_asset->randomDataByte, asset->randomDataByte, new_asset->randomDataByteCount);
				}

				if (new_asset->randomDataIntCount)
				{
					new_asset->randomDataInt = allocator.allocate_array<int>(new_asset->randomDataIntCount);
					std::memcpy(new_asset->randomDataInt, asset->randomDataInt, new_asset->randomDataIntCount * sizeof(int));
				}

				if (new_asset->dataShortCount || new_asset->randomDataShortCount)
				{
					new_asset->dataShort = allocator.allocate_array<short>(new_asset->dataShortCount);
					new_asset->randomDataShort = allocator.allocate_array<short>(new_asset->randomDataShortCount);

					u16* const dataBegin = reinterpret_cast<u16*>(new_asset->dataShort);
					u16* dstShort = dataBegin;

					u16* const randBegin = reinterpret_cast<u16*>(new_asset->randomDataShort);
					u16* dstRand = randBegin;

					u16* srcShort = reinterpret_cast<u16*>(asset->dataShort);
					u16* srcRand = reinterpret_cast<u16*>(asset->randomDataShort);

					std::vector<u16> indices;

					auto readConvert = [&](u16*& dst, u16*& src, u32 count = 1)
					{
						for (u32 i = 0; i < count; ++i)
						{
							const float hf = half_float::half_to_float(*src++);
							*dst++ = QuatInt16::ToInt16(hf);
						}
					};

					auto readRaw = [&](u16*& dst, u16*& src, u32 count = 1)
					{
						for (u32 i = 0; i < count; ++i)
						{
							*dst++ = *src++;
						}
					};

					const u32 frameSize = (asset->numframes > 255) ? 2u : 1u;
					const u32 boneIdSize = (asset->boneCount[TotalBoneCount] > 255) ? 2u : 1u;

					auto processBoneData = [&](int boneCountIndex, int rotSize)
					{
						for (int i = 0; i < asset->boneCount[boneCountIndex]; ++i)
						{
							const u16 frameCount = *srcShort;
							readRaw(dstShort, srcShort); // frame count
							for (int f = 0; f < frameCount + 1; ++f)
							{
								if (frameSize == 2)
								{
									if (frameCount < 0x40)
									{
										readRaw(dstShort, srcShort); // frame index
									}
									else
									{
										indices.push_back(*srcShort);
										++srcShort;
									}
								}

								readConvert(dstRand, srcRand, rotSize);
							}
						}
					};

					processBoneData(TwoDRotatedBoneCount, 2);
					processBoneData(NormalRotatedBoneCount, 4);

					auto processStaticBoneData = [&](int boneCountIndex, int rotSize)
					{
						for (int i = 0; i < asset->boneCount[boneCountIndex]; ++i)
						{
							readConvert(dstShort, srcShort, rotSize);
						}
					};

					processStaticBoneData(TwoDStaticRotatedBoneCount, 2);
					processStaticBoneData(NormalStaticRotatedBoneCount, 4);

					auto processTranslatedBoneData = [&](int boneCountIndex, int translationSize)
					{
						for (int i = 0; i < asset->boneCount[boneCountIndex]; ++i)
						{
							if (boneIdSize == 2)
							{
								readRaw(dstShort, srcShort); // bone id
							}
							const u16 frameCount = *srcShort;
							readRaw(dstShort, srcShort); // frame count
							for (int f = 0; f < frameCount + 1; ++f)
							{
								if (frameSize == 2)
								{
									if (frameCount < 0x40)
									{
										readRaw(dstShort, srcShort); // frame index
									}
									else
									{
										indices.push_back(*srcShort);
										++srcShort;
									}
								}

								if (boneCountIndex == PreciseTranslatedBoneCount)
								{
									readRaw(dstRand, srcRand, translationSize);
								}
							}
						}
					};

					processTranslatedBoneData(NormalTranslatedBoneCount, 3);
					processTranslatedBoneData(PreciseTranslatedBoneCount, 3);

					auto processStaticTranslatedBoneData = [&](int boneCountIndex)
					{
						for (int i = 0; i < asset->boneCount[boneCountIndex]; ++i)
						{
							if (boneIdSize == 2)
							{
								readRaw(dstShort, srcShort); // bone id
							}
						}
					};

					processStaticTranslatedBoneData(StaticTranslatedBoneCount);

					assert(srcShort == reinterpret_cast<u16*>(asset->dataShort + asset->dataShortCount));
					assert(srcRand == reinterpret_cast<u16*>(asset->randomDataShort + asset->randomDataShortCount));

					u32 usedDataCount = 0;
					u32 usedRandomCount = 0;
					if (dataBegin) usedDataCount = static_cast<u32>(dstShort - dataBegin);
					if (randBegin) usedRandomCount = static_cast<u32>(dstRand - randBegin);

					new_asset->dataShortCount = static_cast<u16>(usedDataCount);
					new_asset->randomDataShortCount = static_cast<u32>(usedRandomCount);
					new_asset->dataShort = reinterpret_cast<short*>(dataBegin);
					new_asset->randomDataShort = reinterpret_cast<short*>(randBegin);

					if (!indices.empty())
					{
						new_asset->indexCount = static_cast<u32>(indices.size());
						if (asset->numframes >= 256)
						{
							new_asset->indices._2 = allocator.allocate_array<u16>(new_asset->indexCount);
							for (size_t i = 0; i < indices.size(); ++i)
							{
								const u16 idx = indices[i];
								assert(idx <= asset->numframes);
								new_asset->indices._2[i] = idx;
							}
						}
						else
						{
							new_asset->indices._1 = allocator.allocate_array<u8>(new_asset->indexCount);
							for (size_t i = 0; i < indices.size(); ++i)
							{
								new_asset->indices._1[i] = static_cast<u8>(indices[i]);
							}
						}

						// rebuild dataShort with two extra shorts at the front
						short* oldData = new_asset->dataShort;
						u16 oldCount = new_asset->dataShortCount;

						if (oldData)
						{
							new_asset->dataShortCount = static_cast<u16>(oldCount + 2);
							new_asset->dataShort = allocator.allocate_array<short>(new_asset->dataShortCount);
							new_asset->dataShort[0] = oldData[0];
							new_asset->dataShort[1] = 0;
							new_asset->dataShort[2] = static_cast<short> (asset->numframes);
							std::memcpy(new_asset->dataShort + 3, oldData + 1, (new_asset->dataShortCount - 3) * sizeof(short));
						}
					}
				}

				new_asset->notify = allocator.allocate_array<zonetool::h1::XAnimNotifyInfo>(new_asset->notifyCount);
				for (u32 i = 0; i < new_asset->notifyCount; ++i)
				{
					new_asset->notify[i].name = static_cast<zonetool::h1::scr_string_t>(asset->notify[i].param1);
					new_asset->notify[i].time = asset->notify[i].time;
				}

				static_assert(sizeof(XAnimPartTrans) == sizeof(zonetool::h1::XAnimPartTrans));
				static_assert(sizeof(XAnimDeltaPartQuat2) == sizeof(zonetool::h1::XAnimDeltaPartQuat2));
				static_assert(sizeof(XAnimDeltaPartQuat) == sizeof(zonetool::h1::XAnimDeltaPartQuat));
				REINTERPRET_CAST_SAFE(deltaPart);

				return new_asset;
			}

			void dump(XAnimParts* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::xanim_parts::dump(converted_asset);
			}
		}
	}
}
