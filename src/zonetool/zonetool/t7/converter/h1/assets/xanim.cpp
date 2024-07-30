#include <std_include.hpp>
#include "zonetool/t7/converter/h1/include.hpp"
#include "xanim.hpp"

#include "zonetool/h1/assets/xanim.hpp"

#include <utils/string.hpp>

namespace zonetool::t7
{
	namespace converter::h1
	{
		namespace xanim
		{
			zonetool::h1::XAnimParts* convert(XAnimParts* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::XAnimParts>();

				REINTERPRET_CAST_SAFE(name);

				new_asset->dataByteCount = static_cast<unsigned short>(asset->dataByteCount);
				new_asset->dataShortCount = static_cast<unsigned short>(asset->dataShortCount);
				new_asset->dataIntCount = static_cast<unsigned short>(asset->dataIntCount);
				new_asset->randomDataByteCount = asset->randomDataByteCount;
				new_asset->randomDataIntCount = asset->randomDataIntCount;
				new_asset->numframes = asset->numframes;
				new_asset->flags = 0;
				memcpy(&new_asset->boneCount, &asset->boneCount, sizeof(asset->boneCount));
				new_asset->notifyCount = asset->notifyCount;
				new_asset->assetType = asset->assetType;
				//new_asset->ikType = asset->ikType;
				new_asset->randomDataShortCount = asset->randomDataShortCount;
				new_asset->indexCount = asset->indexCount;
				new_asset->framerate = asset->framerate;
				new_asset->frequency = asset->frequency;
				new_asset->names = allocator.allocate_array<zonetool::h1::scr_string_t>(new_asset->boneCount[9]);
				for (auto i = 0; i < new_asset->boneCount[9]; i++)
				{
					new_asset->names[i] = static_cast<zonetool::h1::scr_string_t>(asset->names[i]);
				}
				new_asset->dataByte = reinterpret_cast<char* __ptr64>(asset->dataByte);
				new_asset->dataShort = reinterpret_cast<short* __ptr64>(asset->dataShort);
				new_asset->dataInt = reinterpret_cast<int* __ptr64>(asset->dataInt);
				new_asset->randomDataShort = reinterpret_cast<short* __ptr64>(asset->randomDataShort);
				new_asset->randomDataByte = reinterpret_cast<unsigned char* __ptr64>(asset->randomDataByte);
				new_asset->randomDataInt = reinterpret_cast<int* __ptr64>(asset->randomDataInt);

				new_asset->indices.data = reinterpret_cast<void*>(asset->indices.data);

				new_asset->notify = allocator.allocate_array<zonetool::h1::XAnimNotifyInfo>(new_asset->notifyCount);
				for (auto i = 0; i < asset->notifyCount; i++)
				{
					new_asset->notify[i].name = static_cast<zonetool::h1::scr_string_t>(asset->notify[i].param1);
					new_asset->notify[i].time = asset->notify[i].time;
				}

				if (asset->deltaPart)
				{
					new_asset->deltaPart = allocator.allocate<zonetool::h1::XAnimDeltaPart>();
					if (asset->deltaPart->trans)
					{
						auto extra_size = 0;

						if (asset->deltaPart->trans->size)
						{
							if (asset->numframes >= 256)
							{
								extra_size += (asset->deltaPart->trans->size * 2) + 2;
							}
							else
							{
								extra_size += asset->deltaPart->trans->size + 1;
							}
						}

						new_asset->deltaPart->trans = allocator.manual_allocate<zonetool::h1::XAnimPartTrans>(sizeof(zonetool::h1::XAnimPartTrans) + extra_size);

						new_asset->deltaPart->trans->size = asset->deltaPart->trans->size;
						new_asset->deltaPart->trans->smallTrans = static_cast<unsigned short>(asset->deltaPart->trans->smallTrans);

						if (asset->deltaPart->trans->size)
						{
							for (auto i = 0; i < 3; i++)
							{
								new_asset->deltaPart->trans->u.frames.mins[i] = asset->deltaPart->trans->u.frames.mins[i];
								new_asset->deltaPart->trans->u.frames.size[i] = asset->deltaPart->trans->u.frames.size[i];
							}
							if (asset->numframes >= 256)
							{
								for (auto i = 0; i < asset->deltaPart->trans->size + 1; i++)
								{
									new_asset->deltaPart->trans->u.frames.indices._2[i] = asset->deltaPart->trans->u.frames.indices._2[i];
								}
							}
							else
							{
								for (auto i = 0; i < asset->deltaPart->trans->size + 1; i++)
								{
									new_asset->deltaPart->trans->u.frames.indices._1[i] = static_cast<unsigned char>(asset->deltaPart->trans->u.frames.indices._1[i]);
								}
							}
							if (asset->deltaPart->trans->u.frames.frames._1)
							{
								if (asset->deltaPart->trans->smallTrans)
								{
									new_asset->deltaPart->trans->u.frames.frames._1 = allocator.allocate_array<unsigned char[3]>(asset->deltaPart->trans->size + 1);
									for (auto i = 0; i < asset->deltaPart->trans->size + 1; i++)
									{
										new_asset->deltaPart->trans->u.frames.frames._1[i][0] = static_cast<unsigned char>(asset->deltaPart->trans->u.frames.frames._1[i][0]);
										new_asset->deltaPart->trans->u.frames.frames._1[i][1] = static_cast<unsigned char>(asset->deltaPart->trans->u.frames.frames._1[i][1]);
										new_asset->deltaPart->trans->u.frames.frames._1[i][2] = static_cast<unsigned char>(asset->deltaPart->trans->u.frames.frames._1[i][2]);
									}
								}
								else
								{
									new_asset->deltaPart->trans->u.frames.frames._2 = allocator.allocate_array<unsigned short[3]>(asset->deltaPart->trans->size + 1);
									for (auto i = 0; i < asset->deltaPart->trans->size + 1; i++)
									{
										new_asset->deltaPart->trans->u.frames.frames._2[i][0] = asset->deltaPart->trans->u.frames.frames._2[i][0];
										new_asset->deltaPart->trans->u.frames.frames._2[i][1] = asset->deltaPart->trans->u.frames.frames._2[i][1];
										new_asset->deltaPart->trans->u.frames.frames._2[i][2] = asset->deltaPart->trans->u.frames.frames._2[i][2];
									}
								}
							}
						}
						else
						{
							for (auto i = 0; i < 3; i++)
							{
								new_asset->deltaPart->trans->u.frame0[i] = asset->deltaPart->trans->u.frame0[i];
							}
						}
					}

					if (asset->deltaPart->quat)
					{
						auto extra_size = 0;

						if (asset->deltaPart->quat->size)
						{
							if (asset->numframes >= 256)
							{
								extra_size += (asset->deltaPart->quat->size * 2) + 2;
							}
							else
							{
								extra_size += asset->deltaPart->quat->size + 1;
							}
						}
						else
						{
							// quat data contains 4 extra bytes
							extra_size += 4;
						}

						new_asset->deltaPart->quat = allocator.manual_allocate<zonetool::h1::XAnimDeltaPartQuat>(sizeof(zonetool::h1::XAnimDeltaPartQuat) + extra_size);
						new_asset->deltaPart->quat->size = asset->deltaPart->quat->size;

						if (asset->deltaPart->quat->size)
						{
							if (asset->numframes >= 256)
							{
								for (auto i = 0; i < asset->deltaPart->quat->size + 1; i++)
								{
									new_asset->deltaPart->quat->u.frames.indices._2[i] = asset->deltaPart->quat->u.frames.indices._2[i];
								}
							}
							else
							{
								for (auto i = 0; i < asset->deltaPart->quat->size + 1; i++)
								{
									new_asset->deltaPart->quat->u.frames.indices._1[i] = static_cast<unsigned char>(asset->deltaPart->quat->u.frames.indices._1[i]);
								}
							}
							if (asset->deltaPart->quat->u.frames.frames)
							{
								new_asset->deltaPart->quat->u.frames.frames = allocator.allocate_array<short[4]>(asset->deltaPart->quat->size + 1);
								for (auto i = 0; i < asset->deltaPart->quat->size + 1; i++)
								{
									new_asset->deltaPart->quat->u.frames.frames[i][0] = asset->deltaPart->quat->u.frames.frames[i][0];
									new_asset->deltaPart->quat->u.frames.frames[i][1] = asset->deltaPart->quat->u.frames.frames[i][1];
								}
							}
						}
						else
						{
							for (auto i = 0; i < 2; i++)
							{
								new_asset->deltaPart->quat->u.frame0[i] = asset->deltaPart->quat->u.frame0[i];
							}
						}
					}
				}

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
