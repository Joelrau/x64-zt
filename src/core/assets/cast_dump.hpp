#pragma once

#include <formats/cast/cast_writer.hpp>

#include <utils/string.hpp>

namespace zonetool::cast_dump
{
	namespace detail
	{
		namespace cs = zonetool::cast;

		constexpr float quat_scale = 32767.0f;
		constexpr std::uint32_t bone_offset_stride = 64;
		constexpr std::size_t index_pool_threshold = 65;
		constexpr std::size_t index_checkpoint_stride = 256;
		constexpr std::uint8_t additive_asset_type = 6;
		constexpr std::uint8_t anim_loop_flag = 1;
		constexpr const char* delta_bone_name = "tag_origin";

		enum part_type
		{
			no_quat,
			half_quat,
			full_quat,
			half_quat_no_size,
			full_quat_no_size,
			small_trans,
			trans,
			trans_no_size,
			no_trans,
			all,
		};

		constexpr std::uint16_t surf_flag_vertcol_none = 2;
		constexpr std::uint16_t surf_flag_skinned = 4;
		constexpr std::uint16_t surf_flag_reactive_motion = 8;

		inline std::string cast_path(const char* folder, const char* name)
		{
			auto file = std::string(name);
			std::ranges::replace(file, '*', '_');
			return std::string(folder) + "\\" + file + ".cast";
		}

		template <typename String>
		std::string bone_name(const String* names, const std::size_t bone)
		{
			const auto* name = names ? SL_ConvertToString(static_cast<std::uint32_t>(names[bone])) : nullptr;
			return (name && *name) ? name : utils::string::va("bone_%llu", bone);
		}

		inline void add_metadata(cs::writer_node& root, const char* name)
		{
			auto& metadata = root.add_child(cs::id_metadata);
			metadata.add_string("a", "zonetool");
			metadata.add_string("s", name);
			metadata.add_string("up", "z");
		}

		inline void unpack_unit_vec(const std::uint32_t packed, float* out)
		{
			for (auto i = 0; i < 3; i++)
			{
				out[i] = static_cast<float>((packed >> (i * 10)) & 0x3FF) / 1023.0f * 2.0f - 1.0f;
			}
		}

		template <typename Model>
		int bone_count(const Model* model)
		{
			if constexpr (requires { model->numClientBones; })
			{
				return model->numBones + model->numClientBones;
			}
			else
			{
				return model->numBones;
			}
		}

		struct influence
		{
			std::uint32_t bone = 0;
			float weight = 0.0f;
		};

		template <typename Surface>
		std::vector<std::vector<influence>> read_influences(const Surface& surface)
		{
			std::vector<std::vector<influence>> result(surface.vertCount);
			std::size_t vertex = 0;

			constexpr auto index_blend = requires { surface.blendVertSize; };

			if ((surface.flags & surf_flag_skinned) && surface.blendVerts)
			{
				auto blend_end = std::numeric_limits<std::size_t>::max();
				if constexpr (index_blend)
				{
					blend_end = surface.blendVertSize / sizeof(*surface.blendVerts);
				}

				const auto bone_id = [](const std::uint32_t value)
				{
					return index_blend ? value : value / bone_offset_stride;
				};

				const auto* start = surface.blendVerts;
				const auto* blend = surface.blendVerts;
				for (auto bone_count = 1u; bone_count <= 8; bone_count++)
				{
					for (auto i = 0; i < surface.blendVertCounts[bone_count - 1] && vertex < surface.vertCount; i++, vertex++)
					{
						const auto stride = index_blend ? bone_count * 2 : bone_count * 2 - 1;
						if (static_cast<std::size_t>(blend - start) + stride > blend_end)
						{
							throw std::runtime_error("the blend verts ran out");
						}

						auto remainder = 1.0f;
						result[vertex].emplace_back(influence{bone_id(blend[0]), 0.0f});
						for (auto bone = 1u; bone < bone_count; bone++)
						{
							const auto weight = static_cast<float>(blend[bone * 2]) / 65535.0f;
							result[vertex].emplace_back(influence{bone_id(blend[bone * 2 - 1]), weight});
							remainder -= weight;
						}
						result[vertex][0].weight = (std::max)(remainder, 0.0f);
						blend += stride;
					}
				}
			}
			else
			{
				for (auto list = 0; surface.rigidVertLists && list < surface.rigidVertListCount; list++)
				{
					const auto& entry = surface.rigidVertLists[list];
					std::uint32_t bone;
					if constexpr (requires { entry.boneOffsetIndex; })
					{
						bone = entry.boneOffsetIndex;
					}
					else
					{
						bone = entry.boneOffset / bone_offset_stride;
					}
					for (auto i = 0; i < entry.vertCount && vertex < surface.vertCount; i++, vertex++)
					{
						result[vertex].emplace_back(influence{bone, 1.0f});
					}
				}
			}

			for (; vertex < surface.vertCount; vertex++)
			{
				result[vertex].emplace_back(influence{0, 1.0f});
			}

			return result;
		}

		template <typename Model>
		void write_skeleton(cs::writer_node& model_node, const Model* model)
		{
			auto& skeleton = model_node.add_child(cs::id_skeleton);

			for (auto i = 0; i < bone_count(model); i++)
			{
				auto& bone = skeleton.add_child(cs::id_bone);
				bone.add_string("n", bone_name(model->boneNames, i));

				float position[3]{};
				float rotation[4]{0.0f, 0.0f, 0.0f, 1.0f};

				if (i < model->numRootBones || !model->parentList)
				{
					bone.add_integer32("p", 0xFFFFFFFF);
					std::memcpy(position, model->baseMat[i].trans, sizeof(position));
					std::memcpy(rotation, model->baseMat[i].quat, sizeof(rotation));
				}
				else
				{
					const auto child = i - model->numRootBones;
					bone.add_integer32("p", static_cast<std::uint32_t>(i - model->parentList[child]));

					if (model->tagPositions && model->tagAngles)
					{
						position[0] = model->tagPositions[child].x;
						position[1] = model->tagPositions[child].y;
						position[2] = model->tagPositions[child].z;
						rotation[0] = static_cast<float>(model->tagAngles[child].x) / quat_scale;
						rotation[1] = static_cast<float>(model->tagAngles[child].y) / quat_scale;
						rotation[2] = static_cast<float>(model->tagAngles[child].z) / quat_scale;
						rotation[3] = static_cast<float>(model->tagAngles[child].base) / quat_scale;
					}
				}

				bone.add_vector3_array("lp", {position[0], position[1], position[2]});
				bone.add_vector4_array("lr", {rotation[0], rotation[1], rotation[2], rotation[3]});
			}
		}

		template <typename Vertex, typename Surface>
		void read_vertices(const Surface& surface, const Vertex* verts, std::vector<float>& positions, std::vector<float>& normals,
			std::vector<float>& tangents, std::vector<float>& uvs, std::vector<std::uint32_t>& colors)
		{
			for (auto i = 0; i < surface.vertCount; i++)
			{
				const auto& vertex = verts[i];
				positions.insert(positions.end(), std::begin(vertex.xyz), std::end(vertex.xyz));

				float value[3]{};
				unpack_unit_vec(vertex.normal.packed, value);
				normals.insert(normals.end(), std::begin(value), std::end(value));
				unpack_unit_vec(vertex.tangent.packed, value);
				tangents.insert(tangents.end(), std::begin(value), std::end(value));

				uvs.emplace_back(half_float::half_to_float(static_cast<std::uint16_t>(vertex.texCoord.packed & 0xFFFF)));
				uvs.emplace_back(half_float::half_to_float(static_cast<std::uint16_t>(vertex.texCoord.packed >> 16)));

				if constexpr (requires { vertex.color.packed; })
				{
					colors.emplace_back(vertex.color.packed);
				}
			}
		}

		template <typename Surface>
		bool write_mesh(cs::writer_node& model_node, const Surface& surface, const std::uint64_t material_hash, const std::string& name)
		{
			if (!surface.verts0.verts0 || !surface.triIndices)
			{
				ZONETOOL_WARNING("CAST XModel: surface \"%s\" has no vertex data", name.data());
				return false;
			}

			auto& mesh = model_node.add_child(cs::id_mesh);
			mesh.add_string("n", name);
			if (material_hash)
			{
				mesh.add_integer64("m", material_hash);
			}

			std::vector<float> positions, normals, tangents, uvs;
			std::vector<std::uint32_t> colors;
			if (surface.flags & surf_flag_reactive_motion)
			{
				read_vertices(surface, surface.verts0.packedMotionVerts0, positions, normals, tangents, uvs, colors);
			}
			else
			{
				read_vertices(surface, surface.verts0.packedVerts0, positions, normals, tangents, uvs, colors);
			}

			mesh.add_vector3_array("vp", positions);
			mesh.add_vector3_array("vn", normals);
			mesh.add_vector3_array("vt", tangents);
			mesh.add_byte("ul", 1);
			mesh.add_vector2_array("u0", uvs);

			if ((surface.flags & surf_flag_vertcol_none) == 0 && !colors.empty())
			{
				mesh.add_byte("cl", 1);
				mesh.add_integer_array("c0", colors);
			}

			std::vector<std::uint32_t> faces;
			for (auto i = 0; i < surface.triCount; i++)
			{
				const auto& face = surface.triIndices[i];
				if (face.v1 == face.v2 && face.v2 == face.v3)
				{
					continue;
				}

				faces.insert(faces.end(), {face.v1, face.v3, face.v2});
			}
			mesh.add_integer_array("f", faces);

			const auto influences = read_influences(surface);
			std::size_t max_influence = 0;
			for (const auto& entry : influences)
			{
				max_influence = (std::max)(max_influence, entry.size());
			}

			std::vector<std::uint32_t> bones(surface.vertCount * max_influence, 0);
			std::vector<float> weights(surface.vertCount * max_influence, 0.0f);
			for (std::size_t i = 0; i < influences.size(); i++)
			{
				for (std::size_t k = 0; k < influences[i].size(); k++)
				{
					bones[i * max_influence + k] = influences[i][k].bone;
					weights[i * max_influence + k] = influences[i][k].weight;
				}
			}

			mesh.add_byte("mi", static_cast<std::uint8_t>(max_influence));
			mesh.add_integer_array("wb", bones);
			mesh.add_float_array("wv", weights);
			mesh.add_string("sm", "linear");
			return true;
		}

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

		inline quat_key make_quat_key(const std::uint32_t frame, const short* components, const std::size_t width)
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

		template <typename Parts>
		class part_reader
		{
		public:
			part_reader(const Parts* parts, const bool short_bone_index)
				: parts_(parts)
				, big_frames_(parts->numframes >= 256)
				, short_bone_index_(short_bone_index)
			{
			}

			std::vector<decoded_track> decode()
			{
				const auto& counts = this->parts_->boneCount;
				std::vector<decoded_track> tracks(counts[all]);

				std::size_t bone = counts[no_quat];
				for (std::size_t i = 0; i < counts[half_quat]; i++)
				{
					this->read_animated_quat(this->track_at(tracks, bone++), 2);
				}
				for (std::size_t i = 0; i < counts[full_quat]; i++)
				{
					this->read_animated_quat(this->track_at(tracks, bone++), 4);
				}
				for (std::size_t i = 0; i < counts[half_quat_no_size]; i++)
				{
					this->read_static_quat(this->track_at(tracks, bone++), 2);
				}
				for (std::size_t i = 0; i < counts[full_quat_no_size]; i++)
				{
					this->read_static_quat(this->track_at(tracks, bone++), 4);
				}

				for (std::size_t i = 0; i < counts[small_trans]; i++)
				{
					this->read_animated_trans(tracks, true);
				}
				for (std::size_t i = 0; i < counts[trans]; i++)
				{
					this->read_animated_trans(tracks, false);
				}
				for (std::size_t i = 0; i < counts[trans_no_size]; i++)
				{
					auto& track = this->track_at(tracks, this->bone_index());

					trans_key key{};
					for (auto& value : key.value)
					{
						value = this->float_value();
					}
					track.translation.emplace_back(key);
				}
				for (std::size_t i = 0; i < counts[no_trans]; i++)
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
				return this->short_bone_index_ ? static_cast<std::uint16_t>(this->short_value()) : this->byte();
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

			const Parts* parts_;
			bool big_frames_;
			bool short_bone_index_;

			std::size_t byte_pos_ = 0;
			std::size_t short_pos_ = 0;
			std::size_t int_pos_ = 0;
			std::size_t random_short_pos_ = 0;
			std::size_t random_byte_pos_ = 0;
			std::size_t index_pos_ = 0;
		};

		template <typename Indices>
		std::vector<std::uint32_t> read_inline_indices(const Indices& indices, const std::size_t key_count, const bool big_frames)
		{
			std::vector<std::uint32_t> result;
			result.reserve(key_count);
			for (std::size_t i = 0; i < key_count; i++)
			{
				result.emplace_back(big_frames ? indices._2[i] : indices._1[i]);
			}
			return result;
		}

		template <typename Trans>
		std::vector<trans_key> decode_delta_trans(const Trans* trans, const bool big_frames)
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

		template <typename Quat>
		std::vector<quat_key> decode_delta_quat(const Quat* quat, const bool big_frames)
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

		inline void write_rotation_curve(cs::writer_node& animation, const std::string& bone_name, const char* mode, const std::vector<quat_key>& keys)
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

		inline void write_translation_curves(cs::writer_node& animation, const std::string& bone_name, const char* mode, const std::vector<trans_key>& keys)
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

		template <typename Parts>
		void write_animation(const Parts* parts, const std::vector<decoded_track>& tracks)
		{
			cs::writer writer;
			auto& root = writer.root();
			add_metadata(root, parts->name);

			auto& animation = root.add_child(cs::id_animation);
			animation.add_string("n", parts->name);
			animation.add_float("fr", parts->framerate);
			animation.add_byte("lo", (parts->flags & anim_loop_flag) ? 1 : 0);

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
				entry.add_string("n", bone_name(parts->names, bone));
				entry.add_integer32("p", 0xFFFFFFFF);
			}

			const auto mode = parts->assetType == additive_asset_type ? "additive" : "absolute";
			auto delta_name_taken = false;
			auto last_keyed_frame = 0u;
			for (std::size_t bone = 0; bone < tracks.size(); bone++)
			{
				for (const auto& key : tracks[bone].rotation)
				{
					last_keyed_frame = (std::max)(last_keyed_frame, key.frame);
				}
				for (const auto& key : tracks[bone].translation)
				{
					last_keyed_frame = (std::max)(last_keyed_frame, key.frame);
				}

				const auto name = bone_name(parts->names, bone);
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
				const auto* name = SL_ConvertToString(static_cast<std::uint32_t>(parts->notify[i].name));
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

			const auto path = cast_path("xanim", parts->name);
			if (!writer.write(path))
			{
				ZONETOOL_ERROR("CAST XAnim \"%s\": cannot write \"%s\"", parts->name, path.data());
			}
		}
	}

	template <typename Model>
	void dump_xmodel(const Model* model)
	{
		namespace cs = zonetool::cast;
		using namespace detail;

		if (!model || !model->name || !model->baseMat)
		{
			return;
		}

		try
		{
			cs::writer writer;
			auto& root = writer.root();
			add_metadata(root, model->name);

			const auto num_lods = (std::min)(static_cast<int>(model->numLods), static_cast<int>(std::size(model->lodInfo)));
			auto lods_written = 0;
			for (auto lod = 0; lod < num_lods; lod++)
			{
				const auto& info = model->lodInfo[lod];
				if (!info.modelSurfs || !info.modelSurfs->surfs || !info.numsurfs)
				{
					continue;
				}

				auto& model_node = root.add_child(cs::id_model);
				model_node.add_string("n", utils::string::va("%s_lod%d", model->name, lod));
				if (lod + 1 < num_lods)
				{
					model_node.add_float("ld", info.dist);
				}

				write_skeleton(model_node, model);

				std::vector<std::uint64_t> material_hashes(info.numsurfs, 0);
				for (auto i = 0; i < info.numsurfs; i++)
				{
					const auto index = static_cast<std::size_t>(info.surfIndex) + i;
					const auto* material = model->materialHandles && index < model->numsurfs ? model->materialHandles[index] : nullptr;
					if (!material || !material->name)
					{
						continue;
					}

					auto& material_node = model_node.add_child(cs::id_material);
					material_node.add_string("n", material->name);
					material_node.add_string("t", "pbr");
					material_hashes[i] = material_node.hash();
				}

				const auto surf_count = (std::min)(static_cast<int>(info.numsurfs), static_cast<int>(info.modelSurfs->numsurfs));
				for (auto i = 0; i < surf_count; i++)
				{
					if (!write_mesh(model_node, info.modelSurfs->surfs[i], material_hashes[i], utils::string::va("%s_lod%d_surf%d", model->name, lod, i)))
					{
						return;
					}
				}

				lods_written++;
			}

			if (!lods_written)
			{
				return;
			}

			const auto path = cast_path("xmodel", model->name);
			if (!writer.write(path))
			{
				ZONETOOL_ERROR("CAST XModel \"%s\": cannot write \"%s\"", model->name, path.data());
				return;
			}

			ZONETOOL_INFO("CAST XModel \"%s\": wrote %d lods to \"%s\"", model->name, lods_written, path.data());
		}
		catch (const std::exception& ex)
		{
			ZONETOOL_ERROR("CAST XModel \"%s\": %s", model->name, ex.what());
		}
	}

	template <typename Parts>
	void dump_xanim(const Parts* parts, const bool short_bone_index = false)
	{
		using namespace detail;

		if (!parts || !parts->name)
		{
			return;
		}

		try
		{
			part_reader reader(parts, short_bone_index);
			write_animation(parts, reader.decode());
		}
		catch (const std::exception& ex)
		{
			ZONETOOL_ERROR("CAST XAnim \"%s\": %s", parts->name, ex.what());
		}
	}
}
