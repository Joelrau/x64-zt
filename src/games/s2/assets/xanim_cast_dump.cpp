#include <std_include.hpp>
#include "s2.hpp"

#include <formats/cast/cast_writer.hpp>

#include <utils/string.hpp>

namespace zonetool::s2::cast_xanim
{
	namespace
	{
		namespace cs = zonetool::cast;

		constexpr float quat_scale = 32767.0f;

		constexpr std::size_t index_pool_threshold = 65;
		constexpr std::size_t index_checkpoint_stride = 256;

		constexpr const char* delta_bone_name = "tag_origin";

		struct quat_key
		{
			std::uint32_t frame;
			float value[4];
		};

		struct trans_key
		{
			std::uint32_t frame;
			float value[3];
		};

		struct decoded_track
		{
			std::vector<quat_key> rotation;
			std::vector<trans_key> translation;
		};

		quat_key make_quat_key(const std::uint32_t frame, const short* components, const std::size_t width)
		{
			quat_key key{frame, {}};
			if (width == 2)
			{
				key.value[2] = static_cast<float>(components[0]) / quat_scale;
				key.value[3] = static_cast<float>(components[1]) / quat_scale;
				return key;
			}

			for (auto component = 0; component < 4; component++)
			{
				key.value[component] = static_cast<float>(components[component]) / quat_scale;
			}
			return key;
		}

		class part_reader
		{
		public:
			explicit part_reader(const XAnimParts* parts)
				: parts_(parts)
				, big_frames_(parts->numframes >= 256)
			{
			}

			std::vector<decoded_track> decode()
			{
				const auto& counts = this->parts_->boneCount;
				std::vector<decoded_track> tracks(counts[PART_TYPE_ALL]);

				std::size_t bone = counts[PART_TYPE_NO_QUAT];
				for (std::size_t i = 0; i < counts[PART_TYPE_HALF_QUAT]; i++)
				{
					this->read_animated_quat(this->track_at(tracks, bone++), 2);
				}
				for (std::size_t i = 0; i < counts[PART_TYPE_FULL_QUAT]; i++)
				{
					this->read_animated_quat(this->track_at(tracks, bone++), 4);
				}
				for (std::size_t i = 0; i < counts[PART_TYPE_HALF_QUAT_NO_SIZE]; i++)
				{
					this->read_static_quat(this->track_at(tracks, bone++), 2);
				}
				for (std::size_t i = 0; i < counts[PART_TYPE_FULL_QUAT_NO_SIZE]; i++)
				{
					this->read_static_quat(this->track_at(tracks, bone++), 4);
				}

				for (std::size_t i = 0; i < counts[PART_TYPE_SMALL_TRANS]; i++)
				{
					this->read_animated_trans(tracks, true);
				}
				for (std::size_t i = 0; i < counts[PART_TYPE_TRANS]; i++)
				{
					this->read_animated_trans(tracks, false);
				}
				for (std::size_t i = 0; i < counts[PART_TYPE_TRANS_NO_SIZE]; i++)
				{
					auto& track = this->track_at(tracks, this->bone_index());

					trans_key key{};
					for (auto& value : key.value)
					{
						value = this->float_value();
					}
					track.translation.emplace_back(key);
				}
				for (std::size_t i = 0; i < counts[PART_TYPE_NO_TRANS]; i++)
				{
					this->bone_index();
				}

				this->verify_consumption();
				return tracks;
			}

		private:
			[[noreturn]] void fail(const std::string& message) const
			{
				throw std::runtime_error(utils::string::va("XAnim \"%s\": %s", this->parts_->name, message.data()));
			}

			decoded_track& track_at(std::vector<decoded_track>& tracks, const std::size_t index) const
			{
				if (index >= tracks.size())
				{
					this->fail(utils::string::va("a part references bone %llu of %llu", index, tracks.size()));
				}
				return tracks[index];
			}

			std::uint8_t byte()
			{
				if (this->byte_pos_ >= this->parts_->dataByteCount)
				{
					this->fail("the byte stream ran out");
				}
				return static_cast<std::uint8_t>(this->parts_->dataByte[this->byte_pos_++]);
			}

			std::int16_t short_value()
			{
				if (this->short_pos_ >= this->parts_->dataShortCount)
				{
					this->fail("the short stream ran out");
				}
				return this->parts_->dataShort[this->short_pos_++];
			}

			std::size_t bone_index()
			{
				return static_cast<std::uint16_t>(this->short_value());
			}

			std::size_t key_count()
			{
				return static_cast<std::size_t>(static_cast<std::uint16_t>(this->short_value())) + 1;
			}

			float float_value()
			{
				if (this->int_pos_ >= this->parts_->dataIntCount)
				{
					this->fail("the int stream ran out");
				}
				return std::bit_cast<float>(this->parts_->dataInt[this->int_pos_++]);
			}

			std::int16_t random_short()
			{
				if (this->random_short_pos_ >= this->parts_->randomDataShortCount)
				{
					this->fail("the random short stream ran out");
				}
				return this->parts_->randomDataShort[this->random_short_pos_++];
			}

			std::uint8_t random_byte()
			{
				if (this->random_byte_pos_ >= this->parts_->randomDataByteCount)
				{
					this->fail("the random byte stream ran out");
				}
				return this->parts_->randomDataByte[this->random_byte_pos_++];
			}

			std::uint16_t pool_index()
			{
				if (this->index_pos_ >= this->parts_->indexCount)
				{
					this->fail("the index pool ran out");
				}
				return this->parts_->indices._2[this->index_pos_++];
			}

			std::vector<std::uint32_t> read_indices(const std::size_t key_count)
			{
				std::vector<std::uint32_t> indices;
				indices.reserve(key_count);

				if (!this->big_frames_)
				{
					for (std::size_t i = 0; i < key_count; i++)
					{
						indices.emplace_back(this->byte());
					}
					return indices;
				}

				if (key_count >= index_pool_threshold)
				{
					const auto checkpoints = ((key_count - 2) / index_checkpoint_stride) + 1;
					for (std::size_t i = 0; i < checkpoints + 1; i++)
					{
						this->short_value();
					}
					for (std::size_t i = 0; i < key_count; i++)
					{
						indices.emplace_back(this->pool_index());
					}
					return indices;
				}

				for (std::size_t i = 0; i < key_count; i++)
				{
					indices.emplace_back(static_cast<std::uint16_t>(this->short_value()));
				}
				return indices;
			}

			void read_animated_quat(decoded_track& track, const std::size_t width)
			{
				const auto indices = this->read_indices(this->key_count());
				for (const auto frame : indices)
				{
					short components[4]{};
					for (std::size_t component = 0; component < width; component++)
					{
						components[component] = this->random_short();
					}
					track.rotation.emplace_back(make_quat_key(frame, components, width));
				}
			}

			void read_static_quat(decoded_track& track, const std::size_t width)
			{
				short components[4]{};
				for (std::size_t component = 0; component < width; component++)
				{
					components[component] = this->short_value();
				}
				track.rotation.emplace_back(make_quat_key(0, components, width));
			}

			void read_animated_trans(std::vector<decoded_track>& tracks, const bool is_small)
			{
				auto& track = this->track_at(tracks, this->bone_index());
				const auto indices = this->read_indices(this->key_count());

				float mins[3];
				float size[3];
				for (auto& value : mins)
				{
					value = this->float_value();
				}
				for (auto& value : size)
				{
					value = this->float_value();
				}

				for (const auto frame : indices)
				{
					trans_key key{frame, {}};
					for (auto axis = 0; axis < 3; axis++)
					{
						const auto stored = is_small
							? static_cast<float>(this->random_byte())
							: static_cast<float>(static_cast<std::uint16_t>(this->random_short()));
						key.value[axis] = mins[axis] + stored * size[axis];
					}
					track.translation.emplace_back(key);
				}
			}

			void verify_consumption() const
			{
				const auto check = [this](const char* name, const std::size_t used, const std::size_t stored)
				{
					if (used != stored)
					{
						this->fail(utils::string::va("the %s stream decoded %llu of %llu entries", name, used, stored));
					}
				};

				check("byte", this->byte_pos_, this->parts_->dataByteCount);
				check("short", this->short_pos_, this->parts_->dataShortCount);
				check("int", this->int_pos_, this->parts_->dataIntCount);
				check("random short", this->random_short_pos_, this->parts_->randomDataShortCount);
				check("random byte", this->random_byte_pos_, this->parts_->randomDataByteCount);
				check("index", this->index_pos_, this->parts_->indexCount);
				check("random int", 0, this->parts_->randomDataIntCount);
			}

			const XAnimParts* parts_;
			bool big_frames_;

			std::size_t byte_pos_ = 0;
			std::size_t short_pos_ = 0;
			std::size_t int_pos_ = 0;
			std::size_t random_short_pos_ = 0;
			std::size_t random_byte_pos_ = 0;
			std::size_t index_pos_ = 0;
		};

		std::vector<std::uint32_t> read_inline_indices(const XAnimDynamicIndices& indices, const std::size_t key_count, const bool big_frames)
		{
			std::vector<std::uint32_t> result;
			result.reserve(key_count);
			for (std::size_t i = 0; i < key_count; i++)
			{
				result.emplace_back(big_frames ? indices._2[i] : indices._1[i]);
			}
			return result;
		}

		std::vector<trans_key> decode_delta_trans(const XAnimPartTrans* trans, const bool big_frames)
		{
			if (!trans->size)
			{
				return {trans_key{0, {trans->u.frame0[0], trans->u.frame0[1], trans->u.frame0[2]}}};
			}

			const auto& frames = trans->u.frames;
			const auto indices = read_inline_indices(frames.indices, trans->size + 1, big_frames);

			std::vector<trans_key> keys;
			for (std::size_t i = 0; i < indices.size(); i++)
			{
				trans_key key{indices[i], {}};
				for (auto axis = 0; axis < 3; axis++)
				{
					const auto stored = trans->smallTrans ? static_cast<float>(frames.frames._1[i][axis]) : static_cast<float>(frames.frames._2[i][axis]);
					key.value[axis] = frames.mins[axis] + stored * frames.size[axis];
				}
				keys.emplace_back(key);
			}
			return keys;
		}

		template <typename T>
		std::vector<quat_key> decode_delta_quat(const T* quat, const bool big_frames)
		{
			constexpr auto width = std::extent_v<decltype(quat->u.frame0)>;
			if (!quat->size)
			{
				return {make_quat_key(0, quat->u.frame0, width)};
			}

			const auto indices = read_inline_indices(quat->u.frames.indices, quat->size + 1, big_frames);

			std::vector<quat_key> keys;
			for (std::size_t i = 0; i < indices.size(); i++)
			{
				keys.emplace_back(make_quat_key(indices[i], quat->u.frames.frames[i], width));
			}
			return keys;
		}

		const char* curve_mode(const XAnimParts* parts)
		{
			return parts->assetType == XANIM_ASSET_TYPE_ADDITIVE ? "additive" : "absolute";
		}

		void write_rotation_curve(cs::writer_node& animation, const std::string& bone_name, const char* mode, const std::vector<quat_key>& keys)
		{
			auto& curve = animation.add_child(cs::id_curve);
			curve.add_string("nn", bone_name);
			curve.add_string("kp", "rq");
			curve.add_string("m", mode);

			std::vector<std::uint32_t> frames;
			std::vector<float> values;
			for (const auto& key : keys)
			{
				frames.emplace_back(key.frame);
				values.insert(values.end(), std::begin(key.value), std::end(key.value));
			}

			curve.add_integer_array("kb", frames);
			curve.add_vector4_array("kv", values);
		}

		void write_translation_curves(cs::writer_node& animation, const std::string& bone_name, const char* mode, const std::vector<trans_key>& keys)
		{
			constexpr const char* properties[3]{"tx", "ty", "tz"};

			std::vector<std::uint32_t> frames;
			for (const auto& key : keys)
			{
				frames.emplace_back(key.frame);
			}

			for (auto axis = 0; axis < 3; axis++)
			{
				auto& curve = animation.add_child(cs::id_curve);
				curve.add_string("nn", bone_name);
				curve.add_string("kp", properties[axis]);
				curve.add_string("m", mode);

				std::vector<float> values;
				for (const auto& key : keys)
				{
					values.emplace_back(key.value[axis]);
				}

				curve.add_integer_array("kb", frames);
				curve.add_float_array("kv", values);
			}
		}

		std::string bone_name(const XAnimParts* parts, const std::size_t bone)
		{
			const auto* name = parts->names ? SL_ConvertToString(parts->names[bone]) : nullptr;
			return (name && *name) ? name : utils::string::va("bone_%llu", bone);
		}

		void write_animation(const XAnimParts* parts, const std::vector<decoded_track>& tracks)
		{
			cs::writer writer;
			auto& root = writer.root();

			auto& metadata = root.add_child(cs::id_metadata);
			metadata.add_string("a", "zonetool");
			metadata.add_string("s", parts->name);
			metadata.add_string("up", "z");

			auto& animation = root.add_child(cs::id_animation);
			animation.add_string("n", parts->name);
			animation.add_float("fr", parts->framerate);
			animation.add_byte("lo", (parts->flags & ANIM_LOOP) ? 1 : 0);

			cs::writer_node* skeleton = nullptr;
			for (std::size_t bone = 0; bone < tracks.size(); bone++)
			{
				if (!tracks[bone].rotation.empty() || !tracks[bone].translation.empty())
				{
					continue;
				}

				if (!skeleton)
				{
					skeleton = &animation.add_child(cs::id_skeleton);
				}

				auto& entry = skeleton->add_child(cs::id_bone);
				entry.add_string("n", bone_name(parts, bone));
				entry.add_integer32("p", 0xFFFFFFFF);
			}

			const auto mode = curve_mode(parts);
			auto delta_name_taken = false;
			auto last_keyed_frame = 0u;
			for (std::size_t bone = 0; bone < tracks.size(); bone++)
			{
				for (const auto& key : tracks[bone].rotation)
				{
					last_keyed_frame = std::max(last_keyed_frame, key.frame);
				}
				for (const auto& key : tracks[bone].translation)
				{
					last_keyed_frame = std::max(last_keyed_frame, key.frame);
				}

				const auto name = bone_name(parts, bone);
				delta_name_taken |= utils::string::to_lower(name) == delta_bone_name;

				if (!tracks[bone].rotation.empty())
				{
					write_rotation_curve(animation, name, mode, tracks[bone].rotation);
				}
				if (!tracks[bone].translation.empty())
				{
					write_translation_curves(animation, name, mode, tracks[bone].translation);
				}
			}

			if (const auto* delta = parts->deltaPart)
			{
				const auto big_frames = parts->numframes >= 256;
				if (delta_name_taken)
				{
					ZONETOOL_WARNING("CAST XAnim \"%s\": the animation drives \"%s\" and has an entity delta, the delta is not written", parts->name, delta_bone_name);
				}
				else
				{
					if (delta->quat2)
					{
						write_rotation_curve(animation, delta_bone_name, "absolute", decode_delta_quat(delta->quat2, big_frames));
					}
					else if (delta->quat)
					{
						write_rotation_curve(animation, delta_bone_name, "absolute", decode_delta_quat(delta->quat, big_frames));
					}
					if (delta->trans)
					{
						write_translation_curves(animation, delta_bone_name, "absolute", decode_delta_trans(delta->trans, big_frames));
					}
				}
			}

			const auto keys_carry_length = last_keyed_frame >= parts->numframes;
			for (auto i = 0; parts->notify && i < parts->notifyCount; i++)
			{
				const auto* name = SL_ConvertToString(parts->notify[i].name);
				if (!name || !*name)
				{
					continue;
				}

				const auto time = parts->notify[i].time;
				if (std::string_view(name) == "end" && std::abs(time - 1.0f) < 0.0001f && keys_carry_length)
				{
					continue;
				}

				auto& track = animation.add_child(cs::id_notification_track);
				track.add_string("n", name);
				track.add_integer_array("kb", {static_cast<std::uint32_t>(time * static_cast<float>(parts->numframes) + 0.5f)});
			}

			const auto path = dump_path(ASSET_TYPE_XANIM, parts->name, ".cast");
			if (!writer.write(path))
			{
				ZONETOOL_ERROR("CAST XAnim \"%s\": cannot write \"%s\"", parts->name, path.data());
			}
		}
	}

	void dump(XAnimParts* parts)
	{
		if (!parts || !parts->name)
		{
			return;
		}

		if (parts->blendShapeCount)
		{
			ZONETOOL_WARNING("CAST XAnim \"%s\": cast cannot store the %u blend shape weights, they are not written", parts->name, parts->blendShapeCount);
		}

		try
		{
			part_reader reader(parts);
			write_animation(parts, reader.decode());
		}
		catch (const std::exception& ex)
		{
			ZONETOOL_ERROR("CAST XAnim \"%s\": %s", parts->name, ex.what());
		}
	}
}
