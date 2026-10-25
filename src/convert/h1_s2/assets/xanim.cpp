#include <std_include.hpp>

#include "../include.hpp"
#include "xanim.hpp"

namespace zonetool::h1::converter::s2::xanim
{
	namespace
	{
		std::unordered_set<std::string> dumped_anims;

		// walks the per-track streams in part type order, per track: key count in dataShort, then numKeys+1 frame indices
		// (u8 in dataByte below 256 frames, else u16 inline, or a block table in dataShort plus the indices stream from 64 keys)
		// h1 1.4 sub_140527140 keeps the bone index of translation tracks (types 5-8) as a u8 in dataByte,
		// s2 2.20 sub_1407CFA20 as an s16 in dataShort ahead of the key count
		template <bool s2_layout, typename T>
		bool walk_tracks(const T* asset, const std::function<void(int type, int index, int bytes, int shorts)>& visit)
		{
			const auto small_frames = asset->numframes < 256;
			auto bytes = 0, shorts = 0, indices = 0;
			const auto frame_keys = [&]()
			{
				if (shorts >= asset->dataShortCount)
				{
					return false;
				}

				const int num_keys = static_cast<std::uint16_t>(asset->dataShort[shorts++]);
				if (small_frames)
				{
					if (bytes + num_keys + 1 > asset->dataByteCount)
					{
						return false;
					}

					for (auto i = 1; i <= num_keys; i++)
					{
						if (static_cast<std::uint8_t>(asset->dataByte[bytes + i]) <= static_cast<std::uint8_t>(asset->dataByte[bytes + i - 1]))
						{
							return false;
						}
					}

					bytes += num_keys + 1;
				}
				else if (num_keys >= 64)
				{
					shorts += ((num_keys - 1) >> 8) + 2;
					indices += num_keys + 1;
				}
				else
				{
					shorts += num_keys + 1;
				}

				return num_keys <= asset->numframes && shorts <= asset->dataShortCount && indices <= static_cast<int>(asset->indexCount);
			};

			for (auto type = 0; type < zonetool::s2::PART_TYPE_ALL; type++)
			{
				for (auto i = 0; i < asset->boneCount[type]; i++)
				{
					const auto track_bytes = bytes, track_shorts = shorts;
					if (type >= zonetool::s2::PART_TYPE_SMALL_TRANS)
					{
						if (s2_layout ? shorts >= asset->dataShortCount : bytes >= asset->dataByteCount)
						{
							return false;
						}

						const auto bone = s2_layout ? asset->dataShort[shorts++] : static_cast<std::uint8_t>(asset->dataByte[bytes++]);
						if (bone < 0 || bone >= asset->boneCount[zonetool::s2::PART_TYPE_ALL])
						{
							return false;
						}
					}

					if (type == zonetool::s2::PART_TYPE_HALF_QUAT || type == zonetool::s2::PART_TYPE_FULL_QUAT ||
						type == zonetool::s2::PART_TYPE_SMALL_TRANS || type == zonetool::s2::PART_TYPE_TRANS)
					{
						if (!frame_keys())
						{
							return false;
						}
					}
					else if (type == zonetool::s2::PART_TYPE_HALF_QUAT_NO_SIZE || type == zonetool::s2::PART_TYPE_FULL_QUAT_NO_SIZE)
					{
						shorts += type == zonetool::s2::PART_TYPE_HALF_QUAT_NO_SIZE ? 2 : 4;
						if (shorts > asset->dataShortCount)
						{
							return false;
						}
					}

					visit(type, i, track_bytes, track_shorts);
				}
			}

			return bytes == asset->dataByteCount && shorts == asset->dataShortCount && indices == static_cast<int>(asset->indexCount);
		}

		void convert_track_data(XAnimParts* asset, zonetool::s2::XAnimParts* new_asset, utils::memory::allocator& allocator)
		{
			std::vector<char> bytes;
			std::vector<short> shorts;
			auto byte_end = 0, short_end = 0;
			const auto copy_until = [&](const int byte_pos, const int short_pos)
			{
				bytes.insert(bytes.end(), asset->dataByte + byte_end, asset->dataByte + byte_pos);
				shorts.insert(shorts.end(), asset->dataShort + short_end, asset->dataShort + short_pos);
				byte_end = byte_pos;
				short_end = short_pos;
			};

			const auto valid = walk_tracks<false>(asset, [&](const int type, int, const int byte_pos, const int short_pos)
			{
				if (type >= zonetool::s2::PART_TYPE_SMALL_TRANS)
				{
					copy_until(byte_pos, short_pos);
					shorts.push_back(static_cast<std::uint8_t>(asset->dataByte[byte_end++]));
				}
			});

			if (!valid)
			{
				ZONETOOL_FATAL("xanim \"%s\": h1 track data does not match its counts", asset->name);
			}

			copy_until(asset->dataByteCount, asset->dataShortCount);
			if (shorts.size() > std::numeric_limits<std::uint16_t>::max())
			{
				ZONETOOL_FATAL("xanim \"%s\": %zu track shorts overflow the s2 count", asset->name, shorts.size());
			}

			new_asset->dataByteCount = static_cast<std::uint16_t>(bytes.size());
			new_asset->dataShortCount = static_cast<std::uint16_t>(shorts.size());
			new_asset->dataByte = bytes.empty() ? nullptr : allocator.allocate_array<char>(bytes.size());
			new_asset->dataShort = shorts.empty() ? nullptr : allocator.allocate_array<short>(shorts.size());
			std::memcpy(new_asset->dataByte, bytes.data(), bytes.size());
			std::memcpy(new_asset->dataShort, shorts.data(), shorts.size() * sizeof(short));

			if (!walk_tracks<true>(new_asset, [](int, int, int, int) {}))
			{
				ZONETOOL_FATAL("xanim \"%s\": converted track data fails the s2 walk", asset->name);
			}
		}

		// same 10 part types and delta parts as h1, s2 widens the bone counts and flags to u16 and moves translation bone indices
		zonetool::s2::XAnimParts* convert(XAnimParts* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::s2::XAnimParts>();
			new_asset->name = asset->name;
			new_asset->dataIntCount = asset->dataIntCount;
			new_asset->numframes = asset->numframes;
			new_asset->flags = asset->flags;
			for (auto i = 0; i <= zonetool::s2::PART_TYPE_ALL; i++)
			{
				new_asset->boneCount[i] = asset->boneCount[i];
			}

			new_asset->notifyCount = asset->notifyCount;
			new_asset->assetType = asset->assetType;
			new_asset->ikType = asset->ikType;
			new_asset->randomDataByteCount = asset->randomDataByteCount;
			new_asset->randomDataShortCount = asset->randomDataShortCount;
			new_asset->randomDataIntCount = asset->randomDataIntCount;
			new_asset->indexCount = asset->indexCount;
			new_asset->framerate = asset->framerate;
			new_asset->frequency = asset->frequency;
			new_asset->names = retype<zonetool::s2::scr_string_t>(asset->names);
			new_asset->dataInt = asset->dataInt;
			new_asset->randomDataShort = asset->randomDataShort;
			new_asset->randomDataByte = asset->randomDataByte;
			new_asset->randomDataInt = asset->randomDataInt;
			new_asset->indices.data = asset->indices.data;
			new_asset->notify = retype<zonetool::s2::XAnimNotifyInfo>(asset->notify);
			new_asset->deltaPart = retype<zonetool::s2::XAnimDeltaPart>(asset->deltaPart);
			new_asset->secondaryName = asset->secondaryName;
			convert_track_data(asset, new_asset, allocator);

			// h1 stores compressed blend shape keys, s2 a weight per frame
			if (asset->blendShapeCount)
			{
				ZONETOOL_WARNING("xanim \"%s\": dropping %u blend shapes, h1 blend shape keys are not converted", asset->name, asset->blendShapeCount);
			}

			return new_asset;
		}
	}

	void dump(XAnimParts* asset)
	{
		if (!dumped_anims.insert(asset->name).second)
		{
			return;
		}

		utils::memory::allocator allocator;
		zonetool::s2::xanim_parts::dump(convert(asset, allocator));
		add_csv_row(zonetool::s2::type_to_string(zonetool::s2::ASSET_TYPE_XANIM), asset->name);
	}

	bool dump_used(const std::string& name)
	{
		if (dumped_anims.contains(name))
		{
			return true;
		}

		const auto asset = db_find_x_asset_header_safe(ASSET_TYPE_XANIMPARTS, name).parts;
		if (!asset)
		{
			return false;
		}

		dump(asset);
		return true;
	}

	void clear()
	{
		dumped_anims.clear();
	}
}
