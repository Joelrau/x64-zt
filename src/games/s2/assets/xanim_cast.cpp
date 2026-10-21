#include <std_include.hpp>
#include "s2.hpp"

#include <formats/cast/cast_scene.hpp>
#include <io/filesystem.hpp>

#include <utils/string.hpp>

namespace zonetool::s2::cast_xanim
{
	namespace
	{
		namespace cs = zonetool::cast;

		constexpr float quat_scale = 32767.0f;
		constexpr float full_trans_quantization = 65535.0f;

		constexpr std::size_t index_pool_threshold = 65;
		constexpr std::size_t index_checkpoint_stride = 256;

		constexpr const char* delta_bone_name = "tag_origin";

		struct quat_track
		{
			XAnimPartType type = PART_TYPE_NO_QUAT;
			std::vector<std::uint16_t> indices;
			std::vector<std::array<std::int16_t, 4>> frames;
		};

		struct trans_track
		{
			XAnimPartType type = PART_TYPE_NO_TRANS;
			std::vector<std::uint16_t> indices;
			float mins[3]{};
			float size[3]{};
			float constant[3]{};
			std::vector<std::array<std::uint16_t, 3>> frames;
		};

		struct bone_track
		{
			std::string name;
			quat_track quat;
			trans_track trans;
		};

		struct flat_data
		{
			std::vector<std::uint8_t> data_byte;
			std::vector<std::int16_t> data_short;
			std::vector<std::int32_t> data_int;
			std::vector<std::int16_t> random_data_short;
			std::vector<std::uint16_t> indices;
		};

		std::int16_t quantize_quat_component(const float value)
		{
			const auto scaled = std::clamp(value, -1.0f, 1.0f) * quat_scale;
			return static_cast<std::int16_t>(scaled >= 0.0f ? scaled + 0.5f : scaled - 0.5f);
		}

		bool equals_ignore_case(const std::string& a, const char* b)
		{
			return utils::string::to_lower(a) == b;
		}

		scr_string_t script_string(const std::string& value)
		{
			return static_cast<scr_string_t>(SL_GetString(value.data()));
		}

		class curve_sampler
		{
		public:
			curve_sampler() = default;

			explicit curve_sampler(const cs::curve_t& curve)
				: curve_(&curve)
			{
				this->order_.resize(curve.key_frames.size());
				std::iota(this->order_.begin(), this->order_.end(), 0u);
				std::stable_sort(this->order_.begin(), this->order_.end(), [&curve](const std::size_t a, const std::size_t b)
				{
					return curve.key_frames[a] < curve.key_frames[b];
				});
			}

			bool valid() const
			{
				return this->curve_ != nullptr && !this->order_.empty();
			}

			std::size_t key_count() const
			{
				return this->order_.size();
			}

			std::uint32_t frame_at(const std::size_t key) const
			{
				return this->curve_->key_frames[this->order_[key]];
			}

			float value_at(const std::size_t key, const std::size_t component) const
			{
				return this->curve_->key_values[this->order_[key] * this->curve_->components + component];
			}

			cs::quat_t quat_at(const std::size_t key) const
			{
				return cs::quat_t{{this->value_at(key, 0), this->value_at(key, 1), this->value_at(key, 2), this->value_at(key, 3)}};
			}

			float evaluate(const std::uint32_t frame, const std::size_t component) const
			{
				const auto count = this->order_.size();
				if (this->frame_at(0) >= frame)
				{
					return this->value_at(0, component);
				}
				if (this->frame_at(count - 1) <= frame)
				{
					return this->value_at(count - 1, component);
				}

				std::size_t upper = 1;
				while (upper < count && this->frame_at(upper) < frame)
				{
					upper++;
				}

				const auto lower_frame = this->frame_at(upper - 1);
				const auto span = static_cast<float>(this->frame_at(upper) - lower_frame);
				const auto t = span > 0.0f ? static_cast<float>(frame - lower_frame) / span : 0.0f;

				const auto lower_value = this->value_at(upper - 1, component);
				return lower_value + (this->value_at(upper, component) - lower_value) * t;
			}

			const cs::curve_t* source() const
			{
				return this->curve_;
			}

			std::uint32_t last_frame() const
			{
				return this->frame_at(this->order_.size() - 1);
			}

		private:
			const cs::curve_t* curve_ = nullptr;
			std::vector<std::size_t> order_;
		};

		struct bone_curves
		{
			curve_sampler rotation;
			curve_sampler translation[3];
			bool has_rotation = false;
			bool has_translation = false;

			cs::curve_mode_t rotation_mode = cs::curve_mode_t::absolute;
			cs::curve_mode_t translation_mode = cs::curve_mode_t::absolute;
		};

		struct rest_pose
		{
			bool valid = false;
			cs::vec3_t position{};
			cs::quat_t rotation{};
		};

		class animation_converter
		{
		public:
			animation_converter(const cs::animation_t& animation, std::string name, const bool notetracks_from_sidecar)
				: animation_(animation)
				, name_(std::move(name))
				, notetracks_from_sidecar_(notetracks_from_sidecar)
			{
			}

			XAnimParts* convert(zone_memory* mem)
			{
				this->collect_rest_pose();
				this->collect_curves();
				this->resolve_frame_count();
				this->build_bone_tracks();
				this->sort_bone_tracks();

				auto* parts = mem->allocate<XAnimParts>();
				parts->name = mem->duplicate_string(this->name_);
				parts->numframes = static_cast<unsigned short>(this->num_frames_);
				parts->framerate = this->animation_.framerate;
				parts->frequency = this->num_frames_ > 0 ? this->animation_.framerate / static_cast<float>(this->num_frames_) : 0.0f;
				parts->assetType = XANIM_ASSET_TYPE_ABSOLUTE;
				parts->ikType = 0;

				if (this->animation_.looping)
				{
					parts->flags |= ANIM_LOOP;
				}

				this->write_delta(parts, mem);
				if (this->additive_)
				{
					parts->assetType = XANIM_ASSET_TYPE_ADDITIVE;
				}

				this->write_bones(parts, mem);
				this->write_notifies(parts, mem);
				this->write_flat_data(parts, mem);

				this->log_summary(parts);
				return parts;
			}

		private:
			[[noreturn]] void fail(const std::string& message) const
			{
				throw cs::cast_error(utils::string::va("CAST XAnim \"%s\": %s", this->name_.data(), message.data()));
			}

			void collect_claim_bones()
			{
				std::set<std::size_t> parents;
				for (const auto& bone : this->animation_.bones)
				{
					if (bone.parent_index >= 0)
					{
						parents.insert(static_cast<std::size_t>(bone.parent_index));
					}
				}

				std::vector<cs::bone_t> rest;
				std::vector<int> remap(this->animation_.bones.size(), -1);

				for (std::size_t i = 0; i < this->animation_.bones.size(); i++)
				{
					const auto& bone = this->animation_.bones[i];
					const auto claims = !bone.has_local && !bone.has_world && bone.parent_index < 0 && !parents.contains(i);

					if (claims)
					{
						if (!cs::is_bone_ignored(bone.name) && !equals_ignore_case(bone.name, delta_bone_name))
						{
							this->claim_bones_.insert(bone.name);
						}
						continue;
					}

					remap[i] = static_cast<int>(rest.size());
					rest.emplace_back(bone);
				}

				for (auto& bone : rest)
				{
					if (bone.parent_index >= 0)
					{
						bone.parent_index = remap[bone.parent_index];
					}
				}

				this->rest_input_ = std::move(rest);
			}

			void collect_rest_pose()
			{
				this->collect_claim_bones();

				if (this->rest_input_.empty())
				{
					return;
				}

				this->rest_bones_ = this->rest_input_;
				cs::normalize_skeleton(this->rest_bones_, utils::string::va("XAnim \"%s\"", this->name_.data()));

				for (const auto& bone : this->rest_bones_)
				{
					rest_pose pose{};
					pose.valid = true;
					pose.position = bone.local_position;
					pose.rotation = bone.local_rotation;

					this->rest_.insert_or_assign(bone.name, pose);
					this->rest_parent_.insert_or_assign(bone.name, bone.parent_index >= 0 ? this->rest_bones_[bone.parent_index].name : std::string());
				}
			}

			bool override_applies(const cs::curve_mode_override_t& entry, const std::string& bone_name) const
			{
				auto current = bone_name;
				for (std::size_t depth = 0; depth <= this->rest_bones_.size(); depth++)
				{
					if (current == entry.node_name)
					{
						return true;
					}

					const auto parent = this->rest_parent_.find(current);
					if (parent == this->rest_parent_.end() || parent->second.empty())
					{
						return false;
					}
					current = parent->second;
				}

				return false;
			}

			cs::curve_mode_t resolve_mode(const std::string& bone_name, const bool rotation, const bool translation, const cs::curve_mode_t curve_mode) const
			{
				for (const auto& entry : this->animation_.overrides)
				{
					if ((rotation && !entry.override_rotation) || (translation && !entry.override_translation))
					{
						continue;
					}

					if (this->override_applies(entry, bone_name))
					{
						return entry.mode;
					}
				}

				return curve_mode;
			}

			static bool same_curve(const cs::curve_t& a, const cs::curve_t& b)
			{
				return a.mode == b.mode && a.components == b.components && a.key_frames == b.key_frames && a.key_values == b.key_values;
			}

			void collect_curves()
			{
				this->additive_ = !this->animation_.curves.empty() && std::ranges::all_of(this->animation_.curves, [](const cs::curve_t& curve)
				{
					return curve.mode == cs::curve_mode_t::additive;
				});

				auto warned_scale = false;
				auto warned_unsupported = false;
				std::set<std::string> ignored;

				for (const auto& curve : this->animation_.curves)
				{
					if (cs::is_bone_ignored(curve.node_name))
					{
						ignored.insert(curve.node_name);
						continue;
					}

					auto& bone = this->curves_[curve.node_name];

					if (curve.property == "rq")
					{
						if (curve.components < 4)
						{
							this->fail(utils::string::va("the rotation curve of bone \"%s\" does not store quaternions", curve.node_name.data()));
						}

						if (bone.has_rotation)
						{
							if (!same_curve(*bone.rotation.source(), curve))
							{
								ZONETOOL_WARNING("CAST XAnim \"%s\": bone \"%s\" has more than one differing rotation curve, only the first one is used",
									this->name_.data(), curve.node_name.data());
							}
							continue;
						}

						bone.rotation = curve_sampler(curve);
						bone.has_rotation = bone.rotation.valid();
						bone.rotation_mode = this->resolve_mode(curve.node_name, true, false, curve.mode);
					}
					else if (curve.property == "tx" || curve.property == "ty" || curve.property == "tz")
					{
						const auto axis = static_cast<std::size_t>(curve.property[1] - 'x');
						if (bone.translation[axis].valid())
						{
							if (!same_curve(*bone.translation[axis].source(), curve))
							{
								ZONETOOL_WARNING("CAST XAnim \"%s\": bone \"%s\" has more than one differing \"%s\" curve, only the first one is used",
									this->name_.data(), curve.node_name.data(), curve.property.data());
							}
							continue;
						}

						bone.translation[axis] = curve_sampler(curve);
						if (bone.translation[axis].valid())
						{
							if (!bone.has_translation)
							{
								bone.translation_mode = this->resolve_mode(curve.node_name, false, true, curve.mode);
							}
							bone.has_translation = true;
						}
					}
					else if (curve.property == "sx" || curve.property == "sy" || curve.property == "sz")
					{
						if (!warned_scale)
						{
							ZONETOOL_WARNING("CAST XAnim \"%s\": XAnimParts cannot store scale curves (first one on bone \"%s\"), they are dropped",
								this->name_.data(), curve.node_name.data());
							warned_scale = true;
						}
					}
					else if (!warned_unsupported)
					{
						ZONETOOL_WARNING("CAST XAnim \"%s\": curve property \"%s\" of bone \"%s\" is not supported and is dropped",
							this->name_.data(), curve.property.data(), curve.node_name.data());
						warned_unsupported = true;
					}
				}

				for (const auto& bone : ignored)
				{
					ZONETOOL_INFO("CAST XAnim \"%s\": bone \"%s\" is ignored", this->name_.data(), bone.data());
				}

				std::erase_if(this->curves_, [](const auto& entry)
				{
					return !entry.second.has_rotation && !entry.second.has_translation;
				});

				for (auto itr = this->curves_.begin(); itr != this->curves_.end(); ++itr)
				{
					if (equals_ignore_case(itr->first, delta_bone_name))
					{
						this->delta_name_ = itr->first;
						this->delta_ = itr->second;
						this->curves_.erase(itr);
						break;
					}
				}

				for (const auto& bone_name : this->curves_ | std::views::keys)
				{
					this->claim_bones_.erase(bone_name);
				}

				if (this->curves_.empty() && this->delta_name_.empty() && this->claim_bones_.empty() && this->animation_.notifications.empty())
				{
					this->fail("the animation contains neither an animated bone nor a notetrack");
				}
			}

			void resolve_frame_count()
			{
				std::uint32_t last_frame = 0;
				const auto track_frames = [&last_frame](const bone_curves& bone)
				{
					if (bone.has_rotation)
					{
						last_frame = std::max(last_frame, bone.rotation.last_frame());
					}
					for (const auto& axis : bone.translation)
					{
						if (axis.valid())
						{
							last_frame = std::max(last_frame, axis.last_frame());
						}
					}
				};

				for (const auto& bone : this->curves_ | std::views::values)
				{
					track_frames(bone);
				}
				if (!this->delta_name_.empty())
				{
					track_frames(this->delta_);
				}

				if (!this->notetracks_from_sidecar_)
				{
					for (const auto& notification : this->animation_.notifications)
					{
						for (const auto frame : notification.frames)
						{
							last_frame = std::max(last_frame, frame);
						}
					}
				}

				this->num_frames_ = last_frame;

				if (this->animation_.looping && last_frame > 0 && !this->loop_is_closed(last_frame))
				{
					ZONETOOL_WARNING("CAST XAnim \"%s\": the looping animation does not return to its first pose, the wrap from frame %u to frame 0 will pop",
						this->name_.data(), last_frame);
				}

				constexpr std::size_t max_frames = std::numeric_limits<decltype(XAnimParts::numframes)>::max();
				if (this->num_frames_ > max_frames)
				{
					this->fail(utils::string::va("the animation has %llu frames, the engine supports at most %llu", this->num_frames_ + 1, max_frames + 1));
				}

				this->use_byte_indices_ = this->num_frames_ < 256;
			}

			bool loop_is_closed(const std::uint32_t last_frame) const
			{
				const auto closed = [this, last_frame](const std::string& bone_name, const bone_curves& bone)
				{
					if (bone.has_rotation)
					{
						const auto first = cs::quat_normalize(this->sample_rotation(bone, 0));
						const auto last = cs::quat_normalize(this->sample_rotation(bone, last_frame));
						if (std::abs(cs::quat_dot(first, last)) < 0.9998f)
						{
							return false;
						}
					}

					if (bone.has_translation)
					{
						const auto first = this->sample_translation(bone_name, bone, 0);
						const auto last = this->sample_translation(bone_name, bone, last_frame);
						for (auto axis = 0; axis < 3; axis++)
						{
							const auto magnitude = std::max(std::abs(first.v[axis]), std::abs(last.v[axis]));
							if (std::abs(first.v[axis] - last.v[axis]) > std::max(0.01f, magnitude * 0.0005f))
							{
								return false;
							}
						}
					}

					return true;
				};

				for (const auto& [bone_name, bone] : this->curves_)
				{
					if (!closed(bone_name, bone))
					{
						return false;
					}
				}

				return this->delta_name_.empty() || closed(this->delta_name_, this->delta_);
			}

			cs::quat_t sample_rotation(const bone_curves& bone, const std::uint32_t frame) const
			{
				const auto count = bone.rotation.key_count();
				for (std::size_t key = 0; key < count; key++)
				{
					if (bone.rotation.frame_at(key) == frame)
					{
						return bone.rotation.quat_at(key);
					}
				}

				if (bone.rotation.frame_at(0) > frame)
				{
					return bone.rotation.quat_at(0);
				}
				if (bone.rotation.frame_at(count - 1) < frame)
				{
					return bone.rotation.quat_at(count - 1);
				}

				std::size_t upper = 1;
				while (upper < count && bone.rotation.frame_at(upper) < frame)
				{
					upper++;
				}

				const auto lower_frame = bone.rotation.frame_at(upper - 1);
				const auto span = static_cast<float>(bone.rotation.frame_at(upper) - lower_frame);
				const auto t = span > 0.0f ? static_cast<float>(frame - lower_frame) / span : 0.0f;
				return cs::quat_slerp(bone.rotation.quat_at(upper - 1), bone.rotation.quat_at(upper), t);
			}

			rest_pose get_rest(const std::string& bone_name) const
			{
				const auto rest = this->rest_.find(bone_name);
				return rest == this->rest_.end() ? rest_pose{} : rest->second;
			}

			cs::quat_t resolve_rotation(const std::string& bone_name, const bone_curves& bone, const std::uint32_t frame) const
			{
				const auto value = cs::quat_normalize(this->sample_rotation(bone, frame));
				if (bone.rotation_mode == cs::curve_mode_t::absolute || this->additive_)
				{
					return value;
				}

				const auto rest = this->get_rest(bone_name);
				return rest.valid ? cs::quat_normalize(cs::quat_multiply(rest.rotation, value)) : value;
			}

			cs::vec3_t sample_translation(const std::string& bone_name, const bone_curves& bone, const std::uint32_t frame) const
			{
				const auto rest = this->get_rest(bone_name);
				const auto relative = bone.translation_mode != cs::curve_mode_t::absolute && !this->additive_;

				cs::vec3_t value{};
				for (auto axis = 0; axis < 3; axis++)
				{
					if (bone.translation[axis].valid())
					{
						value.v[axis] = bone.translation[axis].evaluate(frame, 0);
					}
					else if (!relative && rest.valid)
					{
						value.v[axis] = rest.position.v[axis];
					}

					if (relative && rest.valid)
					{
						value.v[axis] += rest.position.v[axis];
					}
				}

				return value;
			}

			void build_bone_tracks()
			{
				for (const auto& [bone_name, bone] : this->curves_)
				{
					bone_track track{};
					track.name = bone_name;

					if (bone.has_rotation)
					{
						this->build_quat_track(bone_name, bone, track.quat);
					}
					if (bone.has_translation)
					{
						this->build_trans_track(bone_name, bone, track.trans);
					}

					this->tracks_.emplace_back(std::move(track));
				}

				for (const auto& bone_name : this->claim_bones_)
				{
					this->tracks_.emplace_back(bone_track{.name = bone_name});
				}

				constexpr std::size_t max_bones = std::numeric_limits<std::remove_extent_t<decltype(XAnimParts::boneCount)>>::max();
				if (this->tracks_.size() > max_bones)
				{
					this->fail(utils::string::va("the animation drives %llu bones, the engine supports at most %llu", this->tracks_.size(), max_bones));
				}
			}

			std::vector<std::uint32_t> rotation_frames(const bone_curves& bone) const
			{
				std::vector<std::uint32_t> frames;
				for (std::size_t key = 0; key < bone.rotation.key_count(); key++)
				{
					const auto frame = bone.rotation.frame_at(key);
					if (frame <= this->num_frames_ && (frames.empty() || frames.back() != frame))
					{
						frames.emplace_back(frame);
					}
				}

				if (frames.empty())
				{
					frames.emplace_back(0);
				}

				return frames;
			}

			std::vector<std::uint32_t> translation_frames(const bone_curves& bone) const
			{
				std::vector<std::uint32_t> frames;
				for (const auto& axis : bone.translation)
				{
					for (std::size_t key = 0; key < axis.key_count(); key++)
					{
						if (axis.frame_at(key) <= this->num_frames_)
						{
							frames.emplace_back(axis.frame_at(key));
						}
					}
				}

				std::ranges::sort(frames);
				frames.erase(std::unique(frames.begin(), frames.end()), frames.end());

				if (frames.empty())
				{
					frames.emplace_back(0);
				}

				return frames;
			}

			void build_quat_track(const std::string& bone_name, const bone_curves& bone, quat_track& track) const
			{
				const auto frames = this->rotation_frames(bone);

				std::vector<cs::quat_t> values;
				values.reserve(frames.size());
				for (const auto frame : frames)
				{
					values.emplace_back(this->resolve_rotation(bone_name, bone, frame));
				}

				for (std::size_t i = 1; i < values.size(); i++)
				{
					if (cs::quat_dot(values[i - 1], values[i]) < 0.0f)
					{
						for (auto& component : values[i].v)
						{
							component = -component;
						}
					}
				}

				auto constant = true;
				for (std::size_t i = 1; i < values.size() && constant; i++)
				{
					constant = cs::quat_dot(values[0], values[i]) > 0.99999f;
				}

				std::vector<std::array<std::int16_t, 4>> quantized;
				for (const auto& value : values)
				{
					quantized.push_back({quantize_quat_component(value.v[0]), quantize_quat_component(value.v[1]),
						quantize_quat_component(value.v[2]), quantize_quat_component(value.v[3])});
					if (constant)
					{
						break;
					}
				}

				const auto half = std::ranges::all_of(quantized, [](const auto& value)
				{
					return value[0] == 0 && value[1] == 0;
				});

				if (constant)
				{
					track.type = half ? PART_TYPE_HALF_QUAT_NO_SIZE : PART_TYPE_FULL_QUAT_NO_SIZE;
					track.frames.emplace_back(quantized[0]);
					return;
				}

				track.type = half ? PART_TYPE_HALF_QUAT : PART_TYPE_FULL_QUAT;
				track.indices.assign(frames.begin(), frames.end());
				track.frames = std::move(quantized);
			}

			void build_trans_track(const std::string& bone_name, const bone_curves& bone, trans_track& track) const
			{
				const auto frames = this->translation_frames(bone);

				std::vector<cs::vec3_t> values;
				values.reserve(frames.size());
				for (const auto frame : frames)
				{
					values.emplace_back(this->sample_translation(bone_name, bone, frame));
				}

				float mins[3]{};
				float maxs[3]{};
				for (auto axis = 0; axis < 3; axis++)
				{
					mins[axis] = maxs[axis] = values[0].v[axis];
					for (const auto& value : values)
					{
						mins[axis] = std::min(mins[axis], value.v[axis]);
						maxs[axis] = std::max(maxs[axis], value.v[axis]);
					}
				}

				auto constant = true;
				for (auto axis = 0; axis < 3 && constant; axis++)
				{
					constant = (maxs[axis] - mins[axis]) <= 0.000001f;
				}

				if (constant)
				{
					track.type = PART_TYPE_TRANS_NO_SIZE;
					std::memcpy(track.constant, values[0].v, sizeof(track.constant));
					return;
				}

				track.type = PART_TYPE_TRANS;
				for (auto axis = 0; axis < 3; axis++)
				{
					track.mins[axis] = mins[axis];
					track.size[axis] = (maxs[axis] - mins[axis]) / full_trans_quantization;
				}

				track.indices.assign(frames.begin(), frames.end());
				for (const auto& value : values)
				{
					std::array<std::uint16_t, 3> frame{};
					for (auto axis = 0; axis < 3; axis++)
					{
						const auto extent = maxs[axis] - mins[axis];
						if (extent > 0.0f)
						{
							const auto normalized = std::clamp((value.v[axis] - mins[axis]) / extent, 0.0f, 1.0f);
							frame[axis] = static_cast<std::uint16_t>(normalized * full_trans_quantization + 0.5f);
						}
					}
					track.frames.emplace_back(frame);
				}
			}

			void sort_bone_tracks()
			{
				std::ranges::stable_sort(this->tracks_, {}, [](const bone_track& track)
				{
					return track.quat.type;
				});

				this->trans_order_.resize(this->tracks_.size());
				std::iota(this->trans_order_.begin(), this->trans_order_.end(), 0u);
				std::ranges::stable_sort(this->trans_order_, {}, [this](const std::size_t index)
				{
					return this->tracks_[index].trans.type;
				});
			}

			std::size_t index_bytes(const std::size_t count) const
			{
				return count * (this->use_byte_indices_ ? sizeof(unsigned char) : sizeof(unsigned short));
			}

			void write_inline_indices(XAnimDynamicIndices& dest, const std::vector<std::uint16_t>& indices) const
			{
				for (std::size_t i = 0; i < indices.size(); i++)
				{
					if (this->use_byte_indices_)
					{
						dest._1[i] = static_cast<unsigned char>(indices[i]);
					}
					else
					{
						dest._2[i] = indices[i];
					}
				}
			}

			template <typename T>
			T* allocate_delta(zone_memory* mem, const std::size_t key_count) const
			{
				return mem->manual_allocate<T>(sizeof(T) + this->index_bytes(key_count));
			}

			void write_delta_trans(XAnimDeltaPart* delta, zone_memory* mem) const
			{
				trans_track track{};
				this->build_trans_track(this->delta_name_, this->delta_, track);

				if (track.type == PART_TYPE_TRANS_NO_SIZE)
				{
					delta->trans = mem->allocate<XAnimPartTrans>();
					std::memcpy(delta->trans->u.frame0, track.constant, sizeof(track.constant));
					return;
				}

				const auto keys = track.indices.size();
				auto* trans = this->allocate_delta<XAnimPartTrans>(mem, keys);
				trans->size = static_cast<unsigned short>(keys - 1);
				std::memcpy(trans->u.frames.mins, track.mins, sizeof(track.mins));
				std::memcpy(trans->u.frames.size, track.size, sizeof(track.size));
				this->write_inline_indices(trans->u.frames.indices, track.indices);

				trans->u.frames.frames._2 = mem->allocate<unsigned short[3]>(keys);
				for (std::size_t i = 0; i < keys; i++)
				{
					std::ranges::copy(track.frames[i], trans->u.frames.frames._2[i]);
				}

				delta->trans = trans;
			}

			void write_delta_quat(XAnimDeltaPart* delta, zone_memory* mem) const
			{
				quat_track track{};
				this->build_quat_track(this->delta_name_, this->delta_, track);

				const auto half = track.type == PART_TYPE_HALF_QUAT || track.type == PART_TYPE_HALF_QUAT_NO_SIZE;
				const auto animated = track.type == PART_TYPE_HALF_QUAT || track.type == PART_TYPE_FULL_QUAT;
				const auto keys = track.frames.size();

				if (half)
				{
					auto* quat2 = animated ? this->allocate_delta<XAnimDeltaPartQuat2>(mem, keys) : mem->allocate<XAnimDeltaPartQuat2>();
					if (!animated)
					{
						quat2->u.frame0[0] = track.frames[0][2];
						quat2->u.frame0[1] = track.frames[0][3];
					}
					else
					{
						quat2->size = static_cast<unsigned short>(keys - 1);
						this->write_inline_indices(quat2->u.frames.indices, track.indices);
						quat2->u.frames.frames = mem->allocate<short[2]>(keys);
						for (std::size_t i = 0; i < keys; i++)
						{
							quat2->u.frames.frames[i][0] = track.frames[i][2];
							quat2->u.frames.frames[i][1] = track.frames[i][3];
						}
					}

					delta->quat2 = quat2;
					return;
				}

				auto* quat = animated ? this->allocate_delta<XAnimDeltaPartQuat>(mem, keys) : mem->allocate<XAnimDeltaPartQuat>();
				if (!animated)
				{
					std::ranges::copy(track.frames[0], quat->u.frame0);
				}
				else
				{
					quat->size = static_cast<unsigned short>(keys - 1);
					this->write_inline_indices(quat->u.frames.indices, track.indices);
					quat->u.frames.frames = mem->allocate<short[4]>(keys);
					for (std::size_t i = 0; i < keys; i++)
					{
						std::ranges::copy(track.frames[i], quat->u.frames.frames[i]);
					}
				}

				delta->quat = quat;
			}

			void write_delta(XAnimParts* parts, zone_memory* mem) const
			{
				if (this->delta_name_.empty())
				{
					return;
				}

				parts->flags |= ANIM_DELTA;
				parts->assetType = XANIM_ASSET_TYPE_DELTA;
				parts->deltaPart = mem->allocate<XAnimDeltaPart>();

				if (this->delta_.has_translation)
				{
					this->write_delta_trans(parts->deltaPart, mem);
				}
				if (this->delta_.has_rotation)
				{
					this->write_delta_quat(parts->deltaPart, mem);
				}
			}

			void write_bones(XAnimParts* parts, zone_memory* mem) const
			{
				parts->names = mem->allocate<scr_string_t>(this->tracks_.size());
				for (std::size_t i = 0; i < this->tracks_.size(); i++)
				{
					parts->names[i] = script_string(this->tracks_[i].name);
					parts->boneCount[this->tracks_[i].quat.type]++;
					parts->boneCount[this->tracks_[i].trans.type]++;
				}

				parts->boneCount[PART_TYPE_ALL] = static_cast<unsigned short>(this->tracks_.size());
			}

			void write_notifies(XAnimParts* parts, zone_memory* mem) const
			{
				std::vector<std::pair<float, std::string>> notifies;
				for (const auto& notification : this->animation_.notifications)
				{
					for (const auto frame : notification.frames)
					{
						if (frame > this->num_frames_)
						{
							ZONETOOL_WARNING("CAST XAnim \"%s\": notetrack \"%s\" is on frame %u but the animation ends on frame %llu, it is dropped",
								this->name_.data(), notification.name.data(), frame, this->num_frames_);
							continue;
						}

						const auto time = this->num_frames_ > 0 ? static_cast<float>(frame) / static_cast<float>(this->num_frames_) : 0.0f;
						notifies.emplace_back(time, notification.name);
					}
				}

				std::ranges::stable_sort(notifies, {}, &std::pair<float, std::string>::first);

				const auto has_end = std::ranges::any_of(notifies, [](const auto& entry)
				{
					return entry.second == "end";
				});

				constexpr std::size_t max_notifies = std::numeric_limits<decltype(XAnimParts::notifyCount)>::max();
				const auto capacity = has_end ? max_notifies : max_notifies - 1;
				if (notifies.size() > capacity)
				{
					ZONETOOL_WARNING("CAST XAnim \"%s\": the animation has %llu notetracks, only the first %llu fit", this->name_.data(), notifies.size(), capacity);
					notifies.resize(capacity);
				}

				if (!has_end)
				{
					notifies.emplace_back(1.0f, "end");
				}

				parts->notifyCount = static_cast<unsigned char>(notifies.size());
				parts->notify = mem->allocate<XAnimNotifyInfo>(notifies.size());
				for (std::size_t i = 0; i < notifies.size(); i++)
				{
					parts->notify[i].name = script_string(notifies[i].second);
					parts->notify[i].time = notifies[i].first;
				}
			}

			void write_packed_indices(flat_data& data, const std::vector<std::uint16_t>& indices) const
			{
				const auto index_count = indices.size();
				data.data_short.emplace_back(static_cast<std::int16_t>(index_count - 1));

				if (this->use_byte_indices_)
				{
					data.data_byte.insert(data.data_byte.end(), indices.begin(), indices.end());
				}
				else if (index_count >= index_pool_threshold)
				{
					data.indices.insert(data.indices.end(), indices.begin(), indices.end());

					const auto checkpoints = ((index_count - 2) / index_checkpoint_stride) + 1;
					for (std::size_t i = 0; i < checkpoints; i++)
					{
						data.data_short.emplace_back(static_cast<std::int16_t>(indices[index_checkpoint_stride * i]));
					}
					data.data_short.emplace_back(static_cast<std::int16_t>(indices[index_count - 1]));
				}
				else
				{
					data.data_short.insert(data.data_short.end(), indices.begin(), indices.end());
				}
			}

			static void write_float(flat_data& data, const float value)
			{
				data.data_int.emplace_back(std::bit_cast<std::int32_t>(value));
			}

			void build_flat_data(flat_data& data) const
			{
				for (const auto& track : this->tracks_)
				{
					const auto& frames = track.quat.frames;
					switch (track.quat.type)
					{
					case PART_TYPE_HALF_QUAT:
						this->write_packed_indices(data, track.quat.indices);
						for (const auto& frame : frames)
						{
							data.random_data_short.insert(data.random_data_short.end(), {frame[2], frame[3]});
						}
						break;
					case PART_TYPE_FULL_QUAT:
						this->write_packed_indices(data, track.quat.indices);
						for (const auto& frame : frames)
						{
							data.random_data_short.insert(data.random_data_short.end(), frame.begin(), frame.end());
						}
						break;
					case PART_TYPE_HALF_QUAT_NO_SIZE:
						data.data_short.insert(data.data_short.end(), {frames[0][2], frames[0][3]});
						break;
					case PART_TYPE_FULL_QUAT_NO_SIZE:
						data.data_short.insert(data.data_short.end(), frames[0].begin(), frames[0].end());
						break;
					default:
						break;
					}
				}

				for (const auto index : this->trans_order_)
				{
					const auto& track = this->tracks_[index].trans;
					data.data_short.emplace_back(static_cast<std::int16_t>(index));

					switch (track.type)
					{
					case PART_TYPE_TRANS:
						this->write_packed_indices(data, track.indices);
						for (const auto value : track.mins)
						{
							write_float(data, value);
						}
						for (const auto value : track.size)
						{
							write_float(data, value);
						}
						for (const auto& frame : track.frames)
						{
							data.random_data_short.insert(data.random_data_short.end(), frame.begin(), frame.end());
						}
						break;
					case PART_TYPE_TRANS_NO_SIZE:
						for (const auto value : track.constant)
						{
							write_float(data, value);
						}
						break;
					default:
						break;
					}
				}
			}

			template <typename T, typename Count>
			static void copy_stream(zone_memory* mem, const std::vector<T>& source, auto*& dest, Count& count)
			{
				if (source.empty())
				{
					return;
				}

				count = static_cast<Count>(source.size());
				dest = mem->allocate<std::remove_pointer_t<std::remove_reference_t<decltype(dest)>>>(source.size());
				std::memcpy(dest, source.data(), source.size() * sizeof(T));
			}

			void write_flat_data(XAnimParts* parts, zone_memory* mem) const
			{
				flat_data data{};
				this->build_flat_data(data);

				constexpr std::size_t max_count = std::numeric_limits<unsigned short>::max();
				if (data.data_byte.size() > max_count || data.data_short.size() > max_count || data.data_int.size() > max_count)
				{
					this->fail(utils::string::va("the animation needs %llu bytes, %llu shorts and %llu ints of key data, the engine stores at most %llu of each",
						data.data_byte.size(), data.data_short.size(), data.data_int.size(), max_count));
				}

				copy_stream(mem, data.data_byte, parts->dataByte, parts->dataByteCount);
				copy_stream(mem, data.data_short, parts->dataShort, parts->dataShortCount);
				copy_stream(mem, data.data_int, parts->dataInt, parts->dataIntCount);
				copy_stream(mem, data.random_data_short, parts->randomDataShort, parts->randomDataShortCount);
				copy_stream(mem, data.indices, parts->indices._2, parts->indexCount);
			}

			void log_summary(const XAnimParts* parts) const
			{
				const auto rotated = std::ranges::count_if(this->tracks_, [](const bone_track& track)
				{
					return track.quat.type != PART_TYPE_NO_QUAT;
				});
				const auto translated = std::ranges::count_if(this->tracks_, [](const bone_track& track)
				{
					return track.trans.type != PART_TYPE_NO_TRANS;
				});

				ZONETOOL_INFO("CAST XAnim \"%s\": %u bones (%lld rotated, %lld translated), %u frames at %.2f fps, %s%s, %u notetracks",
					this->name_.data(), parts->boneCount[PART_TYPE_ALL], rotated, translated, parts->numframes + 1, parts->framerate,
					(parts->flags & ANIM_LOOP) ? "looping" : "not looping", (parts->flags & ANIM_DELTA) ? ", with entity delta" : "", parts->notifyCount);
			}

			const cs::animation_t& animation_;
			std::string name_;

			std::vector<cs::bone_t> rest_input_;
			std::set<std::string> claim_bones_;
			std::vector<cs::bone_t> rest_bones_;
			std::unordered_map<std::string, rest_pose> rest_;
			std::unordered_map<std::string, std::string> rest_parent_;

			std::map<std::string, bone_curves> curves_;
			std::string delta_name_;
			bone_curves delta_;
			std::vector<bone_track> tracks_;
			std::vector<std::size_t> trans_order_;

			bool notetracks_from_sidecar_ = false;
			bool additive_ = false;
			std::size_t num_frames_ = 0;
			bool use_byte_indices_ = true;
		};

		std::string trim(const std::string& value)
		{
			const auto first = value.find_first_not_of(" \t\r");
			const auto last = value.find_last_not_of(" \t\r");
			return first == std::string::npos ? std::string() : value.substr(first, last - first + 1);
		}

		bool load_notetrack_overrides(const std::string& name, std::vector<cs::notification_t>& notifications)
		{
			const auto path = dump_path(ASSET_TYPE_XANIM, name.data(), ".notetracks");

			filesystem::file source(path);
			source.open("rb");
			if (!source.get_fp())
			{
				return false;
			}

			const auto bytes = source.read_bytes(source.size());
			source.close();

			std::vector<cs::notification_t> parsed;
			std::istringstream text(std::string(bytes.begin(), bytes.end()));
			std::string line;
			for (auto line_number = 1; std::getline(text, line); line_number++)
			{
				line = trim(line.substr(0, line.find('#')));
				if (line.empty())
				{
					continue;
				}

				const auto separator = line.find(',');
				const auto note_name = trim(line.substr(0, separator));
				const auto frame_text = separator == std::string::npos ? std::string() : trim(line.substr(separator + 1));

				char* end = nullptr;
				const auto frame = frame_text.empty() ? -1 : std::strtol(frame_text.data(), &end, 10);
				if (note_name.empty() || frame < 0 || *end != 0)
				{
					ZONETOOL_WARNING("%s line %d: expected \"<name>,<frame>\", ignoring \"%s\"", path.data(), line_number, line.data());
					continue;
				}

				parsed.emplace_back(cs::notification_t{.name = note_name, .frames = {static_cast<std::uint32_t>(frame)}});
			}

			ZONETOOL_INFO("CAST XAnim \"%s\": taking %llu notetracks from \"%s\"", name.data(), parsed.size(), path.data());
			notifications = std::move(parsed);
			return true;
		}
	}

	XAnimParts* parse(const std::string& name, zone_memory* mem)
	{
		const auto path = dump_path(ASSET_TYPE_XANIM, name.data(), ".cast");
		const auto source = cs::load(path);
		if (!source)
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing cast xanim \"%s\"...", name.data());

		const auto scene = cs::build_scene(*source, path);
		if (scene.animations.empty())
		{
			throw cs::cast_error(utils::string::va("CAST XAnim \"%s\": the file does not contain an animation node", name.data()));
		}

		if (scene.animations.size() > 1)
		{
			ZONETOOL_WARNING("CAST XAnim \"%s\": the file contains %llu animations, only the first one is used", name.data(), scene.animations.size());
		}

		auto animation = scene.animations[0];
		const auto from_sidecar = load_notetrack_overrides(name, animation.notifications);

		animation_converter converter(animation, name, from_sidecar);
		return converter.convert(mem);
	}
}
