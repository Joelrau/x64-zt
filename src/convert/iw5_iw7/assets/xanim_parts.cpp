#include <std_include.hpp>

#include "../include.hpp"
#include "xanim_parts.hpp"

#include <iw7/assets/xanim.hpp>

namespace zonetool::iw5::converter::iw7
{
	namespace xanim_parts
	{
		namespace
		{
			std::size_t index_bytes(const zonetool::iw5::XAnimParts* asset, const unsigned int count)
			{
				return asset->numframes >= 256 ? count * 2 : count;
			}

			template <typename Source, typename Target>
			void copy_indices(const zonetool::iw5::XAnimParts* asset, const Source& source, Target& target, const unsigned int count)
			{
				if (asset->numframes >= 256)
				{
					for (unsigned int i = 0; i < count; i++)
					{
						target._2[i] = source._2[i];
					}
				}
				else
				{
					for (unsigned int i = 0; i < count; i++)
					{
						target._1[i] = static_cast<unsigned char>(source._1[i]);
					}
				}
			}

			zonetool::iw7::XAnimPartTrans* convert_trans(const zonetool::iw5::XAnimParts* asset, const zonetool::iw5::XAnimPartTrans* trans,
				utils::memory::allocator& allocator)
			{
				const auto extra_size = trans->size ? index_bytes(asset, trans->size) + (asset->numframes >= 256 ? 2 : 1) : 0;
				const auto result = allocator.manual_allocate<zonetool::iw7::XAnimPartTrans>(sizeof(zonetool::iw7::XAnimPartTrans) + extra_size);

				result->size = trans->size;
				result->smallTrans = static_cast<unsigned char>(trans->smallTrans);

				if (!trans->size)
				{
					std::memcpy(result->u.frame0, trans->u.frame0, sizeof(result->u.frame0));
					return result;
				}

				std::memcpy(result->u.frames.mins, trans->u.frames.mins, sizeof(result->u.frames.mins));
				std::memcpy(result->u.frames.size, trans->u.frames.size, sizeof(result->u.frames.size));
				copy_indices(asset, trans->u.frames.indices, result->u.frames.indices, trans->size + 1);

				const auto count = trans->size + 1;
				if (trans->smallTrans)
				{
					const auto source = trans->u.frames.frames._1.get();
					const auto frames = allocator.allocate_array<unsigned char[3]>(count);
					for (auto i = 0; i < count; i++)
					{
						for (auto axis = 0; axis < 3; axis++)
						{
							frames[i][axis] = static_cast<unsigned char>(source[i][axis]);
						}
					}

					result->u.frames.frames._1 = frames;
				}
				else
				{
					const auto source = trans->u.frames.frames._2.get();
					const auto frames = allocator.allocate_array<unsigned short[3]>(count);
					std::memcpy(frames, source, sizeof(unsigned short[3]) * count);
					result->u.frames.frames._2 = frames;
				}

				return result;
			}

			template <std::size_t Shorts, typename Target, typename Source>
			Target* convert_quat(const zonetool::iw5::XAnimParts* asset, const Source* quat, utils::memory::allocator& allocator,
				const std::size_t empty_extra)
			{
				const auto extra_size = quat->size ? index_bytes(asset, quat->size) + (asset->numframes >= 256 ? 2 : 1) : empty_extra;
				const auto result = allocator.manual_allocate<Target>(sizeof(Target) + extra_size);
				result->size = quat->size;

				if (!quat->size)
				{
					std::memcpy(result->u.frame0, quat->u.frame0, sizeof(short) * Shorts);
					return result;
				}

				copy_indices(asset, quat->u.frames.indices, result->u.frames.indices, quat->size + 1);

				const auto count = quat->size + 1;
				const auto source = quat->u.frames.frames.get();
				const auto frames = allocator.allocate_array<short[Shorts]>(count);
				std::memcpy(frames, source, sizeof(short[Shorts]) * count);
				result->u.frames.frames = frames;
				return result;
			}
		}

		zonetool::iw7::XAnimParts* convert(zonetool::iw5::XAnimParts* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw7::XAnimParts>();

			new_asset->name = asset->name.get();
			COPY_VALUE(dataByteCount);
			COPY_VALUE(dataShortCount);
			COPY_VALUE(dataIntCount);
			new_asset->randomDataByteCount = asset->randomDataByteCount;
			COPY_VALUE(randomDataIntCount);
			COPY_VALUE(numframes);
			new_asset->flags = static_cast<unsigned char>(asset->flags);
			std::memcpy(new_asset->boneCount, asset->boneCount, sizeof(asset->boneCount));
			new_asset->notifyCount = static_cast<unsigned char>(asset->notifyCount);
			new_asset->assetType = static_cast<unsigned char>(asset->assetType);
			COPY_VALUE(randomDataShortCount);
			COPY_VALUE(indexCount);
			COPY_VALUE(framerate);
			COPY_VALUE(frequency);

			if (asset->names)
			{
				new_asset->names = allocator.allocate_array<zonetool::iw7::scr_string_t>(asset->boneCount[9]);
				for (auto i = 0; i < asset->boneCount[9]; i++)
				{
					new_asset->names[i] = asset->names.get()[i];
				}
			}

			new_asset->dataByte = asset->dataByte.get();
			new_asset->dataShort = reinterpret_cast<short*>(asset->dataShort.get());
			new_asset->dataInt = asset->dataInt.get();
			new_asset->randomDataShort = reinterpret_cast<short*>(asset->randomDataShort.get());
			new_asset->randomDataByte = asset->randomDataByte.get();
			new_asset->randomDataInt = asset->randomDataInt.get();
			new_asset->indices.data = asset->indices.data.get();

			if (asset->notify)
			{
				new_asset->notify = allocator.allocate_array<zonetool::iw7::XAnimNotifyInfo>(asset->notifyCount);
				for (auto i = 0; i < asset->notifyCount; i++)
				{
					new_asset->notify[i].name = asset->notify.get()[i].name;
					new_asset->notify[i].time = asset->notify.get()[i].time;
				}
			}

			if (asset->deltaPart)
			{
				const auto delta = allocator.allocate<zonetool::iw7::XAnimDeltaPart>();
				new_asset->deltaPart = delta;

				if (asset->deltaPart->trans)
				{
					delta->trans = convert_trans(asset, asset->deltaPart->trans.get(), allocator);
				}

				if (asset->deltaPart->quat2)
				{
					delta->quat2 = convert_quat<2, zonetool::iw7::XAnimDeltaPartQuat2>(asset, asset->deltaPart->quat2.get(), allocator, 0);
				}

				if (asset->deltaPart->quat)
				{
					delta->quat = convert_quat<4, zonetool::iw7::XAnimDeltaPartQuat>(asset, asset->deltaPart->quat.get(), allocator, 4);
				}
			}

			return new_asset;
		}

		void dump(zonetool::iw5::XAnimParts* asset)
		{
			utils::memory::allocator allocator;
			zonetool::iw7::xanim_parts::dump(convert(asset, allocator));
		}
	}
}
