#include <std_include.hpp>

#include "../include.hpp"
#include "rig/xanim_dive.hpp"
#include "rig/xanim_melee.hpp"
#include "rig/xanim_retarget.hpp"
#include "xanim.hpp"

namespace zonetool::h1::converter::s2::xanim
{
	namespace
	{

		std::unordered_set<std::string> dumped_anims;
		std::unordered_map<std::string, bool> built_dives;
		std::unordered_map<std::string, bool> built_melees;

		bool has_bone_index(const int type)
		{
			return type >= zonetool::s2::PART_TYPE_SMALL_TRANS;
		}

		bool has_key_frames(const int type)
		{
			return type == zonetool::s2::PART_TYPE_HALF_QUAT || type == zonetool::s2::PART_TYPE_FULL_QUAT ||
				type == zonetool::s2::PART_TYPE_SMALL_TRANS || type == zonetool::s2::PART_TYPE_TRANS;
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
					if (has_bone_index(type))
					{
						if (s2_layout ? shorts >= asset->dataShortCount : bytes >= asset->dataByteCount)
						{
							return false;
						}

						const auto bone_index = s2_layout ? asset->dataShort[shorts++] : static_cast<std::uint8_t>(asset->dataByte[bytes++]);
						if (bone_index < 0 || bone_index >= asset->boneCount[zonetool::s2::PART_TYPE_ALL])
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
					else if (type == zonetool::s2::PART_TYPE_HALF_QUAT_NO_SIZE || type == zonetool::s2::PART_TYPE_FULL_QUAT_NO_SIZE)
					{
						shorts += type == zonetool::s2::PART_TYPE_HALF_QUAT_NO_SIZE ? 2 : 4;
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

		void convert_track_data(XAnimParts* asset, zonetool::s2::XAnimParts* new_asset, utils::memory::allocator& allocator)
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

			const auto valid = walk_tracks<false>(asset, [&](const int type, const int track_bytes, const int track_shorts)
			{
				if (has_bone_index(type))
				{
					copy_until(track_bytes, track_shorts);
					const auto h1_bone_index = static_cast<std::uint8_t>(asset->dataByte[copied_bytes++]);
					shorts.push_back(h1_bone_index);
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
			new_asset->dataByte = copy_array(bytes.data(), bytes.size(), allocator);
			new_asset->dataShort = copy_array(shorts.data(), shorts.size(), allocator);

			if (!walk_tracks<true>(new_asset, [](int, int, int) {}))
			{
				ZONETOOL_FATAL("xanim \"%s\": converted track data fails the s2 walk", asset->name);
			}
		}

		void convert_notify(const XAnimParts* asset, zonetool::s2::XAnimParts* new_asset, utils::memory::allocator& allocator)
		{
			new_asset->notify = asset->notify ? allocator.allocate_array<zonetool::s2::XAnimNotifyInfo>(asset->notifyCount) : nullptr;
			new_asset->notifyCount = 0;
			for (auto i = 0; asset->notify && i < asset->notifyCount; i++)
			{
				// the s2 client reads "fx_<fx>;<tag>" and crashes on a missing tag (MS 0x14039CEC0)
				const std::string_view name = SL_ConvertToString(asset->notify[i].name);
				if (name.starts_with("fx_") && name.find(';') == std::string_view::npos)
				{
					ZONETOOL_WARNING("xanim \"%s\": dropping notetrack \"%s\", s2 needs \"fx_<fx>;<tag>\"", asset->name, name.data());
					continue;
				}

				new_asset->notify[new_asset->notifyCount++] = {static_cast<zonetool::s2::scr_string_t>(asset->notify[i].name), asset->notify[i].time};
			}
		}

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
			convert_notify(asset, new_asset, allocator);
			new_asset->deltaPart = retype<zonetool::s2::XAnimDeltaPart>(asset->deltaPart);
			new_asset->secondaryName = asset->secondaryName;
			convert_track_data(asset, new_asset, allocator);

			if (asset->blendShapeCount)
			{
				ZONETOOL_WARNING("xanim \"%s\": dropping %u blend shapes, h1 blend shape keys are not converted", asset->name, asset->blendShapeCount);
			}

			return new_asset;
		}

		std::string idle_base(const XAnimParts* idle)
		{
			std::string base = idle->name;
			if (constexpr std::string_view suffix = "_idle"; base.ends_with(suffix))
			{
				base.resize(base.size() - suffix.size());
			}

			return base;
		}

		void dump_generated(zonetool::s2::XAnimParts* anim)
		{
			zonetool::s2::xanim_parts::dump(anim);
			add_csv_row(zonetool::s2::type_to_string(zonetool::s2::ASSET_TYPE_XANIM), anim->name);
		}

		// generated anims start from the converted and retargeted h1 idle
		template <typename Build>
		bool build_from_idle(XAnimParts* idle, const Build& build)
		{
			utils::memory::allocator conversion_allocator;
			std::optional<zone_memory> retarget_memory;
			auto* converted = convert(idle, conversion_allocator);
			return xanim_retarget::retarget(converted, retarget_memory) && build(converted);
		}
	}

	void dump(XAnimParts* asset)
	{
		if (!dumped_anims.insert(asset->name).second)
		{
			return;
		}

		utils::memory::allocator allocator;
		std::optional<zone_memory> retarget_memory;
		const auto new_asset = convert(asset, allocator);
		xanim_retarget::retarget(new_asset, retarget_memory);
		zonetool::s2::xanim_parts::dump(new_asset);
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

	zonetool::s2::XAnimParts* reference(const XAnimParts* asset, utils::memory::allocator& allocator)
	{
		if (!asset || !asset->name)
		{
			return nullptr;
		}

		if (!dump_used(asset->name))
		{
			ZONETOOL_WARNING("xanim \"%s\" not found in the loaded h1 zones", asset->name);
			return nullptr;
		}

		return make_stub<zonetool::s2::XAnimParts>(asset->name, allocator);
	}

	std::array<zonetool::s2::XAnimParts*, 2> dive_anims(XAnimParts* idle, const char* donor, utils::memory::allocator& allocator)
	{
		std::array<zonetool::s2::XAnimParts*, 2> result{};
		if (!idle || !idle->name)
		{
			return result;
		}

		const auto base = idle_base(idle);
		const std::array names{base + "_dive", base + "_dive_end"};
		const auto [entry, inserted] = built_dives.try_emplace(names[0], false);
		if (inserted)
		{
			entry->second = build_from_idle(idle, [&](const zonetool::s2::XAnimParts* converted)
			{
				std::optional<zone_memory> dive_memory;
				const auto anims = xanim_dive::build(converted, donor, names, dive_memory);
				if (anims)
				{
					std::ranges::for_each(*anims, dump_generated);
				}

				return anims.has_value();
			});
		}

		for (auto i = 0u; entry->second && i < result.size(); i++)
		{
			result[i] = make_stub<zonetool::s2::XAnimParts>(allocator.duplicate_string(names[i]), allocator);
		}

		return result;
	}

	zonetool::s2::XAnimParts* melee_anim(XAnimParts* idle, const char* donor, utils::memory::allocator& allocator)
	{
		if (!idle || !idle->name)
		{
			return nullptr;
		}

		const auto name = idle_base(idle) + "_melee";
		const auto [entry, inserted] = built_melees.try_emplace(name, false);
		if (inserted)
		{
			entry->second = build_from_idle(idle, [&](const zonetool::s2::XAnimParts* converted)
			{
				std::optional<zone_memory> melee_memory;
				auto* anim = xanim_melee::build(converted, donor, name, melee_memory);
				if (anim)
				{
					dump_generated(anim);
				}

				return anim != nullptr;
			});
		}

		return entry->second ? make_stub<zonetool::s2::XAnimParts>(allocator.duplicate_string(name), allocator) : nullptr;
	}

	void clear()
	{
		dumped_anims.clear();
		built_dives.clear();
		built_melees.clear();
		xanim_retarget::clear();
		xanim_dive::clear();
		xanim_melee::clear();
	}
}
