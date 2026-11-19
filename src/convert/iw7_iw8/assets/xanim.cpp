#include <std_include.hpp>

#include "../include.hpp"
#include "xanim.hpp"

#include <iw8/assets/xanim.hpp>
#include <iw8/zone/loader.hpp>

#include <io/game_files.hpp>
#include <xfile/oodle.hpp>

namespace zonetool::iw7::converter::iw8::xanim
{
	namespace
	{
		namespace target = zonetool::iw8;

		using bind_pose = std::unordered_map<std::string, std::array<float, 3>>;

		bind_pose load_bind_pose(const char* model_name)
		{
			xfile::oodle::load(iw8_folder());
			auto previous_files = game_files::exchange(game_files::open_casc(iw8_folder()));
			const auto _ = gsl::finally([&previous_files] { game_files::set(std::move(previous_files)); });

			bind_pose pose;
			const auto path = game_files::find_zone("common_mp");
			if (!path)
			{
				ZONETOOL_WARNING("IW8 zone \"common_mp\" is missing, view anims keep the IW7 translations");
				return pose;
			}

			const auto database = db_create();
			db_scope scope(*database);
			const auto zone = target::load_zone(*path);

			for (const auto& asset : zone->assets)
			{
				const auto* model = static_cast<const target::XModel*>(asset.header);
				if (asset.referenced || asset.type != target::ASSET_TYPE_XMODEL || std::strcmp(model->name, model_name))
				{
					continue;
				}

				for (auto bone = model->numRootBones; bone < model->numBones; bone++)
				{
					const auto* trans = &model->trans[(bone - model->numRootBones) * 3];
					if (const auto* name = SL_ConvertToString(model->boneNames[bone]))
					{
						pose.try_emplace(name, std::array{trans[0], trans[1], trans[2]});
					}
				}

				return pose;
			}

			ZONETOOL_WARNING("IW8 xmodel \"%s\" is missing, view anims keep the IW7 translations", model_name);
			return pose;
		}

		const bind_pose& view_bind_pose()
		{
			static const auto pose = load_bind_pose("viewhands_base_iw8");
			return pose;
		}

		class track_reader
		{
		public:
			explicit track_reader(const target::XAnimParts* parts)
				: parts_(parts)
				, big_frames_(parts->numframes >= 256)
			{
			}

			anim_tracks read()
			{
				anim_tracks tracks;
				const auto& counts = this->parts_->boneCount;

				std::size_t name = 0;
				for (auto type = target::PART_TYPE_NO_QUAT; type <= target::PART_TYPE_FULL_QUAT_NO_SIZE; type = static_cast<target::XAnimPartType>(type + 1))
				{
					for (std::size_t i = 0; i < counts[type]; i++)
					{
						auto& track = tracks.quats.emplace_back(this->parts_->names[name++], type);
						this->read_quat(track);
					}
				}

				for (auto type = target::PART_TYPE_SMALL_TRANS; type <= target::PART_TYPE_NO_TRANS; type = static_cast<target::XAnimPartType>(type + 1))
				{
					for (std::size_t i = 0; i < counts[type]; i++)
					{
						const auto bone = static_cast<std::uint8_t>(this->take(this->parts_->dataByte, this->byte_pos_, 1, this->parts_->dataByteCount));
						if (bone >= counts[target::PART_TYPE_ALL])
						{
							throw std::runtime_error("a translation references a missing bone");
						}

						auto& track = tracks.trans.emplace_back(this->parts_->names[bone], type);
						this->read_trans(track);
					}
				}

				if (name != counts[target::PART_TYPE_ALL] || tracks.trans.size() != tracks.quats.size()
					|| this->byte_pos_ != this->parts_->dataByteCount || this->short_pos_ != this->parts_->dataShortCount
					|| this->int_pos_ != this->parts_->dataIntCount || this->random_short_pos_ != this->parts_->randomDataShortCount
					|| this->random_byte_pos_ != this->parts_->randomDataByteCount || this->index_pos_ != this->parts_->indexCount
					|| this->parts_->randomDataIntCount)
				{
					throw std::runtime_error("the track streams do not match the part counts");
				}

				return tracks;
			}

		private:
			template <typename T>
			static T take(const T* stream, std::size_t& pos, const std::size_t count, const std::size_t stored, std::vector<T>* out = nullptr)
			{
				if (pos + count > stored)
				{
					throw std::runtime_error("a track stream ran out");
				}

				if (out)
				{
					out->insert(out->end(), stream + pos, stream + pos + count);
				}

				pos += count;
				return count ? stream[pos - count] : T{};
			}

			std::size_t read_key_count(anim_track& track)
			{
				const auto value = this->take(this->parts_->dataShort, this->short_pos_, 1, this->parts_->dataShortCount, &track.data_short);
				return static_cast<std::size_t>(static_cast<std::uint16_t>(value)) + 1;
			}

			void read_indices(anim_track& track, const std::size_t key_count)
			{
				if (!this->big_frames_)
				{
					this->take(this->parts_->dataByte, this->byte_pos_, key_count, this->parts_->dataByteCount, &track.data_byte);
				}
				else if (key_count >= 65)
				{
					this->take(this->parts_->dataShort, this->short_pos_, ((key_count - 2) / 256) + 2, this->parts_->dataShortCount, &track.data_short);
					this->take(this->parts_->indices._2, this->index_pos_, key_count, this->parts_->indexCount, &track.indices);
				}
				else
				{
					this->take(this->parts_->dataShort, this->short_pos_, key_count, this->parts_->dataShortCount, &track.data_short);
				}
			}

			void read_floats(anim_track& track, const std::size_t count)
			{
				std::vector<int> values;
				this->take(this->parts_->dataInt, this->int_pos_, count, this->parts_->dataIntCount, &values);
				for (const auto value : values)
				{
					track.data_float.emplace_back(std::bit_cast<float>(value));
				}
			}

			void read_quat(anim_track& track)
			{
				const auto width = track.type == target::PART_TYPE_HALF_QUAT || track.type == target::PART_TYPE_HALF_QUAT_NO_SIZE ? 2u : 4u;
				if (track.type == target::PART_TYPE_HALF_QUAT || track.type == target::PART_TYPE_FULL_QUAT)
				{
					const auto key_count = this->read_key_count(track);
					this->read_indices(track, key_count);
					this->take(this->parts_->randomDataShort, this->random_short_pos_, width * key_count, this->parts_->randomDataShortCount, &track.random_short);
				}
				else if (track.type != target::PART_TYPE_NO_QUAT)
				{
					this->take(this->parts_->dataShort, this->short_pos_, width, this->parts_->dataShortCount, &track.data_short);
				}
			}

			void read_trans(anim_track& track)
			{
				if (track.type == target::PART_TYPE_SMALL_TRANS || track.type == target::PART_TYPE_TRANS)
				{
					const auto key_count = this->read_key_count(track);
					this->read_indices(track, key_count);
					this->read_floats(track, 6);
					if (track.type == target::PART_TYPE_SMALL_TRANS)
					{
						this->take(this->parts_->randomDataByte, this->random_byte_pos_, 3 * key_count, this->parts_->randomDataByteCount, &track.random_byte);
					}
					else
					{
						this->take(this->parts_->randomDataShort, this->random_short_pos_, 3 * key_count, this->parts_->randomDataShortCount, &track.random_short);
					}
				}
				else if (track.type == target::PART_TYPE_TRANS_NO_SIZE)
				{
					this->read_floats(track, 3);
				}
			}

			const target::XAnimParts* parts_;
			bool big_frames_;

			std::size_t byte_pos_ = 0;
			std::size_t short_pos_ = 0;
			std::size_t int_pos_ = 0;
			std::size_t random_short_pos_ = 0;
			std::size_t random_byte_pos_ = 0;
			std::size_t index_pos_ = 0;
		};

		template <typename T>
		T* copy_array(const std::vector<T>& values, utils::memory::allocator& allocator)
		{
			if (values.empty())
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<T>(values.size());
			std::memcpy(result, values.data(), values.size() * sizeof(T));
			return result;
		}

		void write_tracks(target::XAnimParts* parts, anim_tracks& tracks, utils::memory::allocator& allocator)
		{
			const auto by_type = [](const anim_track& a, const anim_track& b) { return a.type < b.type; };
			std::stable_sort(tracks.quats.begin(), tracks.quats.end(), by_type);

			std::unordered_map<std::uint32_t, std::uint8_t> bone_indices;
			std::vector<target::scr_string_t> names;
			for (const auto& track : tracks.quats)
			{
				bone_indices.emplace(track.name, static_cast<std::uint8_t>(names.size()));
				names.emplace_back(static_cast<target::scr_string_t>(track.name));
			}

			std::stable_sort(tracks.trans.begin(), tracks.trans.end(), [&](const anim_track& a, const anim_track& b)
			{
				return a.type != b.type ? a.type < b.type : bone_indices.at(a.name) < bone_indices.at(b.name);
			});

			std::vector<char> data_byte;
			std::vector<short> data_short;
			std::vector<int> data_int;
			std::vector<short> random_short;
			std::vector<char> random_byte;
			std::vector<unsigned short> indices;
			std::memset(parts->boneCount, 0, sizeof(parts->boneCount));

			const auto append = [&](const anim_track& track)
			{
				data_byte.insert(data_byte.end(), track.data_byte.begin(), track.data_byte.end());
				data_short.insert(data_short.end(), track.data_short.begin(), track.data_short.end());
				for (const auto value : track.data_float)
				{
					data_int.emplace_back(std::bit_cast<int>(value));
				}
				random_short.insert(random_short.end(), track.random_short.begin(), track.random_short.end());
				random_byte.insert(random_byte.end(), track.random_byte.begin(), track.random_byte.end());
				indices.insert(indices.end(), track.indices.begin(), track.indices.end());
				parts->boneCount[track.type]++;
			};

			for (const auto& track : tracks.quats)
			{
				append(track);
			}

			for (const auto& track : tracks.trans)
			{
				data_byte.emplace_back(static_cast<char>(bone_indices.at(track.name)));
				append(track);
			}

			parts->boneCount[target::PART_TYPE_ALL] = static_cast<unsigned char>(names.size());
			parts->names = copy_array(names, allocator);
			parts->dataByte = copy_array(data_byte, allocator);
			parts->dataShort = copy_array(data_short, allocator);
			parts->dataInt = copy_array(data_int, allocator);
			parts->randomDataShort = copy_array(random_short, allocator);
			parts->randomDataByte = copy_array(random_byte, allocator);
			parts->indices._2 = copy_array(indices, allocator);
			parts->dataByteCount = static_cast<unsigned int>(data_byte.size());
			parts->dataShortCount = static_cast<unsigned short>(data_short.size());
			parts->dataIntCount = static_cast<unsigned short>(data_int.size());
			parts->randomDataShortCount = static_cast<unsigned int>(random_short.size());
			parts->randomDataByteCount = static_cast<unsigned int>(random_byte.size());
			parts->indexCount = static_cast<unsigned int>(indices.size());
		}

		anim_track* find_track(std::vector<anim_track>& tracks, const std::uint32_t name)
		{
			const auto track = std::ranges::find(tracks, name, &anim_track::name);
			return track != tracks.end() ? &*track : nullptr;
		}

		void subtract_bind(anim_track& track, const std::array<float, 3>& bind)
		{
			if (track.type == target::PART_TYPE_NO_TRANS)
			{
				if (bind != std::array{0.0f, 0.0f, 0.0f})
				{
					track.type = target::PART_TYPE_TRANS_NO_SIZE;
					track.data_float = {-bind[0], -bind[1], -bind[2]};
				}
				return;
			}

			for (auto axis = 0; axis < 3; axis++)
			{
				track.data_float[axis] -= bind[axis];
			}
		}

		void retarget_to_bind_pose(anim_tracks& tracks, const bind_pose& pose)
		{
			for (auto& track : tracks.trans)
			{
				if (const auto bind = pose.find(SL_ConvertToString(track.name)); bind != pose.end())
				{
					subtract_bind(track, bind->second);
				}
			}

			const auto gun = SL_GetString("j_gun");
			const auto weapon_right = SL_GetString("tag_weapon_right");
			const auto* weapon_quat = find_track(tracks.quats, weapon_right);
			const auto* weapon_trans = find_track(tracks.trans, weapon_right);
			if (!find_track(tracks.quats, gun) && weapon_quat && weapon_trans)
			{
				auto gun_quat = *weapon_quat;
				auto gun_trans = *weapon_trans;
				gun_quat.name = gun;
				gun_trans.name = gun;
				tracks.quats.emplace_back(std::move(gun_quat));
				tracks.trans.emplace_back(std::move(gun_trans));
			}

			const auto view = SL_GetString("tag_view");
			if (const auto bind = pose.find("tag_view"); bind != pose.end() && !find_track(tracks.quats, view))
			{
				tracks.quats.emplace_back(view, target::PART_TYPE_NO_QUAT);
				auto& view_trans = tracks.trans.emplace_back(view, target::PART_TYPE_TRANS_NO_SIZE);
				view_trans.data_float = {-bind->second[0], -bind->second[1], -bind->second[2]};
			}
		}

		bool is_view_anim(const target::XAnimParts* parts)
		{
			if (parts->assetType != target::XANIM_ASSET_TYPE_ABSOLUTE)
			{
				return false;
			}

			const auto cambone = SL_GetString("tag_cambone");
			return std::ranges::find(parts->names, parts->names + parts->boneCount[target::PART_TYPE_ALL], static_cast<target::scr_string_t>(cambone))
				!= parts->names + parts->boneCount[target::PART_TYPE_ALL];
		}

		void retarget_view_anim(target::XAnimParts* parts, utils::memory::allocator& allocator)
		{
			if (!is_view_anim(parts) || view_bind_pose().empty())
			{
				return;
			}

			try
			{
				auto tracks = track_reader(parts).read();
				retarget_to_bind_pose(tracks, view_bind_pose());

				if (tracks.quats.size() > 255)
				{
					throw std::runtime_error("the retargeted anim has more than 255 bones");
				}

				write_tracks(parts, tracks, allocator);
			}
			catch (const std::exception& e)
			{
				ZONETOOL_WARNING("XAnim \"%s\" keeps the IW7 translations: %s", parts->name, e.what());
			}
		}

		bool is_melee_anim(const XAnimParts* asset)
		{
			auto melee = false;
			db_enum_assets(ASSET_TYPE_ANIM_PACKAGE, [&](void* header)
			{
				const auto* package = static_cast<const WeaponAnimPackage*>(header);
				if (!package->anims)
				{
					return;
				}

				for (const auto slot : {WEAP_ANIM_MELEE_MISS, WEAP_ANIM_MELEE_HIT, WEAP_ANIM_MELEE_FATAL})
				{
					melee |= package->anims[slot] && !std::strcmp(package->anims[slot]->name, asset->name);
				}
			});

			return melee;
		}

		void add_melee_notify(target::XAnimParts* parts, utils::memory::allocator& allocator)
		{
			const auto name = static_cast<target::scr_string_t>(SL_GetString("melee_attack_sfx"));
			std::vector<target::XAnimNotifyInfo> notifies(parts->notify, parts->notify + parts->notifyCount);
			if (std::ranges::find(notifies, name, &target::XAnimNotifyInfo::name) != notifies.end())
			{
				return;
			}

			const auto time = 0.1026f; // vm_pi_mike1911_melee_hit_01
			notifies.insert(std::ranges::upper_bound(notifies, time, {}, &target::XAnimNotifyInfo::time), {name, time});
			parts->notify = copy_array(notifies, allocator);
			parts->notifyCount = static_cast<unsigned char>(notifies.size());
		}

		target::XAnimParts* convert(const XAnimParts* asset, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::XAnimParts>();
			result->name = asset->name;
			result->names = reinterpret_cast<target::scr_string_t*>(asset->names);
			result->dataByte = asset->dataByte;
			result->dataShort = asset->dataShort;
			result->dataInt = asset->dataInt;
			result->randomDataShort = asset->randomDataShort;
			result->randomDataByte = asset->randomDataByte;
			result->randomDataInt = asset->randomDataInt;
			result->indices.data = asset->indices.data;
			result->notify = reinterpret_cast<target::XAnimNotifyInfo*>(asset->notify);
			result->deltaPart = reinterpret_cast<target::XAnimDeltaPart*>(asset->deltaPart);
			result->randomDataShortCount = asset->randomDataShortCount;
			result->randomDataByteCount = asset->randomDataByteCount;
			result->indexCount = asset->indexCount;
			result->framerate = asset->framerate;
			result->frequency = asset->frequency;
			result->dataByteCount = asset->dataByteCount;
			result->dataShortCount = asset->dataShortCount;
			result->dataIntCount = asset->dataIntCount;
			result->randomDataIntCount = asset->randomDataIntCount;
			result->numframes = asset->numframes;
			result->flags = asset->flags;
			std::memcpy(result->boneCount, asset->boneCount, sizeof(result->boneCount));
			result->notifyCount = asset->notifyCount;
			result->assetType = asset->assetType;
			result->ikType = asset->ikType;
			result->fingerPoseType = asset->fingerPoseType;

			retarget_view_anim(result, allocator);
			if (is_melee_anim(asset))
			{
				add_melee_notify(result, allocator);
			}

			return result;
		}
	}

	void dump(XAnimParts* asset)
	{
		if (!asset || !mark_dumped(ASSET_TYPE_XANIMPARTS, asset->name))
		{
			return;
		}

		utils::memory::allocator allocator;
		target::xanim_parts::dump(convert(asset, allocator));
		add_csv_row("xanim", asset->name);
	}
}
