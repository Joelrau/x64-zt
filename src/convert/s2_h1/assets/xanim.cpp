#include <std_include.hpp>

#include "../include.hpp"
#include "xanim.hpp"

namespace zonetool::s2::converter::h1::xanim
{
	namespace
	{
		std::unordered_set<std::string> dumped_anims;

		bool has_bone_index(const int type)
		{
			return type >= PART_TYPE_SMALL_TRANS;
		}

		bool has_key_frames(const int type)
		{
			return type == PART_TYPE_HALF_QUAT || type == PART_TYPE_FULL_QUAT || type == PART_TYPE_SMALL_TRANS || type == PART_TYPE_TRANS;
		}

		template <bool s2_layout, typename T>
		bool walk_tracks(const T* asset, const std::function<void(int type, int track_bytes, int track_shorts)>& visit)
		{
			const auto small_frames = asset->numframes < 256;
			auto bytes = 0, shorts = 0, indices = 0;
			const auto skip_key_frames = [&]()
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

			for (auto type = 0; type < PART_TYPE_ALL; type++)
			{
				for (auto i = 0; i < asset->boneCount[type]; i++)
				{
					const auto track_bytes = bytes, track_shorts = shorts;
					if (has_bone_index(type))
					{
						if (s2_layout ? shorts >= asset->dataShortCount : bytes >= asset->dataByteCount)
						{
							return false;
						}

						const auto bone_index = s2_layout ? asset->dataShort[shorts++] : static_cast<std::uint8_t>(asset->dataByte[bytes++]);
						if (bone_index < 0 || bone_index >= asset->boneCount[PART_TYPE_ALL])
						{
							return false;
						}
					}

					if (has_key_frames(type))
					{
						if (!skip_key_frames())
						{
							return false;
						}
					}
					else if (type == PART_TYPE_HALF_QUAT_NO_SIZE || type == PART_TYPE_FULL_QUAT_NO_SIZE)
					{
						shorts += type == PART_TYPE_HALF_QUAT_NO_SIZE ? 2 : 4;
						if (shorts > asset->dataShortCount)
						{
							return false;
						}
					}

					visit(type, track_bytes, track_shorts);
				}
			}

			return bytes == asset->dataByteCount && shorts == asset->dataShortCount && indices == static_cast<int>(asset->indexCount);
		}

		bool convert_track_data(const XAnimParts* asset, zonetool::h1::XAnimParts* new_asset, utils::memory::allocator& allocator)
		{
			std::vector<char> bytes;
			std::vector<short> shorts;
			auto copied_bytes = 0, copied_shorts = 0;
			const auto copy_until = [&](const int byte_end, const int short_end)
			{
				bytes.insert(bytes.end(), asset->dataByte + copied_bytes, asset->dataByte + byte_end);
				shorts.insert(shorts.end(), asset->dataShort + copied_shorts, asset->dataShort + short_end);
				copied_bytes = byte_end;
				copied_shorts = short_end;
			};

			const auto valid = walk_tracks<true>(asset, [&](const int type, const int track_bytes, const int track_shorts)
			{
				if (has_bone_index(type))
				{
					copy_until(track_bytes, track_shorts);
					bytes.push_back(static_cast<char>(asset->dataShort[copied_shorts++]));
				}
			});

			if (!valid)
			{
				ZONETOOL_WARNING("xanim \"%s\": s2 track data does not match its counts", asset->name);
				return false;
			}

			copy_until(asset->dataByteCount, asset->dataShortCount);
			if (bytes.size() > std::numeric_limits<std::uint16_t>::max())
			{
				ZONETOOL_WARNING("xanim \"%s\": %zu track bytes overflow the h1 count", asset->name, bytes.size());
				return false;
			}

			new_asset->dataByteCount = static_cast<std::uint16_t>(bytes.size());
			new_asset->dataShortCount = static_cast<std::uint16_t>(shorts.size());
			new_asset->dataByte = allocator.allocate_array<char>(bytes.size());
			new_asset->dataShort = allocator.allocate_array<short>(shorts.size());
			std::ranges::copy(bytes, new_asset->dataByte);
			std::ranges::copy(shorts, new_asset->dataShort);

			if (!walk_tracks<false>(new_asset, [](int, int, int) {}))
			{
				ZONETOOL_WARNING("xanim \"%s\": converted track data fails the h1 walk", asset->name);
				return false;
			}

			return true;
		}

		zonetool::h1::XAnimParts* convert(XAnimParts* asset, utils::memory::allocator& allocator)
		{
			for (auto i = 0; i <= PART_TYPE_ALL; i++)
			{
				if (asset->boneCount[i] > std::numeric_limits<std::uint8_t>::max())
				{
					ZONETOOL_WARNING("xanim \"%s\": %u bones of part type %d overflow the h1 count", asset->name, asset->boneCount[i], i);
					return nullptr;
				}
			}

			const auto new_asset = allocator.allocate<zonetool::h1::XAnimParts>();
			new_asset->name = asset->name;
			new_asset->dataIntCount = asset->dataIntCount;
			new_asset->numframes = asset->numframes;
			new_asset->flags = static_cast<unsigned char>(asset->flags);
			for (auto i = 0; i <= PART_TYPE_ALL; i++)
			{
				new_asset->boneCount[i] = static_cast<unsigned char>(asset->boneCount[i]);
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
			new_asset->names = reinterpret_cast<zonetool::h1::scr_string_t*>(asset->names);
			new_asset->dataInt = asset->dataInt;
			new_asset->randomDataShort = asset->randomDataShort;
			new_asset->randomDataByte = asset->randomDataByte;
			new_asset->randomDataInt = asset->randomDataInt;
			new_asset->indices.data = asset->indices.data;
			REINTERPRET_CAST_SAFE(notify);
			REINTERPRET_CAST_SAFE(deltaPart);
			new_asset->secondaryName = asset->secondaryName;
			if (!convert_track_data(asset, new_asset, allocator))
			{
				return nullptr;
			}

			if (asset->flags > std::numeric_limits<std::uint8_t>::max())
			{
				ZONETOOL_WARNING("xanim \"%s\": dropping s2 flags 0x%X above the h1 byte", asset->name, asset->flags & ~0xFFu);
			}

			if (asset->blendShapeCount)
			{
				ZONETOOL_WARNING("xanim \"%s\": dropping %u blend shapes, s2 blend shape weights are not converted", asset->name, asset->blendShapeCount);
			}

			return new_asset;
		}
	}

	void dump(XAnimParts* asset)
	{
		if (std::string_view(asset->name).starts_with("vm_") || !dumped_anims.insert(asset->name).second)
		{
			return;
		}

		utils::memory::allocator allocator;
		if (const auto new_asset = convert(asset, allocator))
		{
			zonetool::h1::xanim_parts::dump(new_asset);
			add_csv_row(zonetool::h1::type_to_string(zonetool::h1::ASSET_TYPE_XANIMPARTS), asset->name);
		}
	}

	bool dump_used(const std::string& name)
	{
		if (dumped_anims.contains(name))
		{
			return true;
		}

		const auto asset = db_find_x_asset_header_safe(ASSET_TYPE_XANIM, name).parts;
		if (!asset)
		{
			return false;
		}

		dump(asset);
		return dumped_anims.contains(name);
	}
}
