#include <std_include.hpp>

#include "../include.hpp"
#include "bone_rig.hpp"
#include "techset.hpp"
#include "xanim_retarget.hpp"

namespace zonetool::h1::converter::s2::xanim_retarget
{
	namespace
	{
		namespace cs = zonetool::cast;

		constexpr auto rig_name = "viewmodel_base_viewhands"; // the base viewmodel arm rig of both games
		constexpr auto s2_arm_root = "j_mainroot";
		constexpr auto body_tag = "tag_torso";
		constexpr std::array sides{"le", "ri"};
		constexpr std::array fingers{"index", "mid", "ring", "pinky", "thumb"};
		constexpr auto finger_joints = 3;
		constexpr auto epsilon = 0.0001f;

		using bone_rig::compose;
		using bone_rig::invert;
		using bone_rig::skeleton;
		using bone_rig::transform;

		struct rigs
		{
			skeleton h1;
			skeleton s2;
			std::unordered_map<std::string, std::string> s2_to_h1;
			std::unordered_set<std::string> h1_arm_bones;
			std::vector<bool> s2_arm_bones;
			std::vector<bool> s2_body_bones; // bones under tag_torso outside the arms that h1 also has
			int h1_body;
			int s2_body;
		};

		struct arm_pose
		{
			cs::vec3_t elbow;
			cs::vec3_t wrist;
			cs::vec3_t normal;
		};

		struct bone_keys
		{
			std::vector<std::pair<std::uint32_t, cs::quat_t>> rotation;
			std::vector<std::pair<std::uint32_t, cs::vec3_t>> translation;
		};

		std::optional<rigs> loaded_rigs;

		cs::vec3_t add(const cs::vec3_t& a, const cs::vec3_t& b)
		{
			return {{a.v[0] + b.v[0], a.v[1] + b.v[1], a.v[2] + b.v[2]}};
		}

		cs::vec3_t sub(const cs::vec3_t& a, const cs::vec3_t& b)
		{
			return {{a.v[0] - b.v[0], a.v[1] - b.v[1], a.v[2] - b.v[2]}};
		}

		cs::vec3_t scale(const cs::vec3_t& a, const float s)
		{
			return {{a.v[0] * s, a.v[1] * s, a.v[2] * s}};
		}

		float dot(const cs::vec3_t& a, const cs::vec3_t& b)
		{
			return a.v[0] * b.v[0] + a.v[1] * b.v[1] + a.v[2] * b.v[2];
		}

		cs::vec3_t cross(const cs::vec3_t& a, const cs::vec3_t& b)
		{
			return {{a.v[1] * b.v[2] - a.v[2] * b.v[1], a.v[2] * b.v[0] - a.v[0] * b.v[2], a.v[0] * b.v[1] - a.v[1] * b.v[0]}};
		}

		float length(const cs::vec3_t& a)
		{
			return std::sqrt(dot(a, a));
		}

		cs::vec3_t normalize(const cs::vec3_t& a)
		{
			const auto len = length(a);
			return len > epsilon ? scale(a, 1.0f / len) : cs::vec3_t{};
		}

		cs::vec3_t lerp(const cs::vec3_t& a, const cs::vec3_t& b, const float t)
		{
			return add(a, scale(sub(b, a), t));
		}

		cs::quat_t inverse(const cs::quat_t& q)
		{
			return cs::quat_conjugate(q);
		}

		cs::quat_t multiply(const cs::quat_t& a, const cs::quat_t& b)
		{
			return cs::quat_normalize(cs::quat_multiply(a, b));
		}

		cs::vec3_t to_world(const transform& space, const cs::vec3_t& point)
		{
			return add(space.position, cs::quat_rotate(space.rotation, point));
		}

		cs::vec3_t to_local(const transform& space, const cs::vec3_t& point)
		{
			return cs::quat_rotate(inverse(space.rotation), sub(point, space.position));
		}

		cs::quat_t quat_from_axes(const cs::vec3_t& x, const cs::vec3_t& y, const cs::vec3_t& z)
		{
			cs::quat_t q{};
			const auto trace = x.v[0] + y.v[1] + z.v[2];
			if (trace > 0.0f)
			{
				const auto s = std::sqrt(trace + 1.0f) * 2.0f;
				q = {{(y.v[2] - z.v[1]) / s, (z.v[0] - x.v[2]) / s, (x.v[1] - y.v[0]) / s, 0.25f * s}};
			}
			else if (x.v[0] > y.v[1] && x.v[0] > z.v[2])
			{
				const auto s = std::sqrt(1.0f + x.v[0] - y.v[1] - z.v[2]) * 2.0f;
				q = {{0.25f * s, (y.v[0] + x.v[1]) / s, (z.v[0] + x.v[2]) / s, (y.v[2] - z.v[1]) / s}};
			}
			else if (y.v[1] > z.v[2])
			{
				const auto s = std::sqrt(1.0f + y.v[1] - x.v[0] - z.v[2]) * 2.0f;
				q = {{(y.v[0] + x.v[1]) / s, 0.25f * s, (z.v[1] + y.v[2]) / s, (z.v[0] - x.v[2]) / s}};
			}
			else
			{
				const auto s = std::sqrt(1.0f + z.v[2] - x.v[0] - y.v[1]) * 2.0f;
				q = {{(z.v[0] + x.v[2]) / s, (z.v[1] + y.v[2]) / s, 0.25f * s, (x.v[1] - y.v[0]) / s}};
			}

			return cs::quat_normalize(q);
		}

		cs::quat_t frame_rotation(const cs::vec3_t& aim, const cs::vec3_t& up)
		{
			const auto x = normalize(aim);
			const auto z = normalize(sub(up, scale(x, dot(up, x))));
			return quat_from_axes(x, cross(z, x), z);
		}

		cs::quat_t rotation_between(const cs::vec3_t& from, const cs::vec3_t& to)
		{
			const auto a = normalize(from);
			const auto b = normalize(to);
			const auto axis = cross(a, b);
			return cs::quat_normalize({{axis.v[0], axis.v[1], axis.v[2], 1.0f + dot(a, b)}});
		}

		cs::quat_t twist_about(const cs::quat_t& q, const cs::vec3_t& axis)
		{
			const auto projected = scale(axis, dot({{q.v[0], q.v[1], q.v[2]}}, axis));
			const cs::quat_t twist{{projected.v[0], projected.v[1], projected.v[2], q.v[3]}};
			return cs::quat_dot(twist, twist) > epsilon ? cs::quat_normalize(twist) : cs::quat_t{};
		}

		std::optional<skeleton> load_h1_rig()
		{
			return bone_rig::load_h1_model(rig_name);
		}

		std::optional<skeleton> load_s2_rig()
		{
			return bone_rig::load_s2_model(rig_name);
		}

		std::string side_bone(const char* name, const char* side)
		{
			return std::format("j_{}_{}", name, side);
		}

		std::string finger_bone(const char* finger, const char* side, const int joint)
		{
			return std::format("j_{}_{}_{}", finger, side, joint);
		}

		std::unordered_map<std::string, std::string> build_bone_map(const skeleton& h1, const skeleton& s2)
		{
			// h1 fingers count from _0 and s2 fingers from _1, the h1 palm bones match the s2 metacarpals of the ring and pinky fingers
			std::unordered_map<std::string, std::string> map;
			for (const auto* side : sides)
			{
				for (const auto* joint : {"shoulder", "elbow", "wrist"})
				{
					map.emplace(side_bone(joint, side), side_bone(joint, side));
				}

				map.emplace(finger_bone("metapinky", side, 1), side_bone("pinkypalm", side));
				map.emplace(finger_bone("metaring", side, 1), side_bone("ringpalm", side));
				for (const auto* finger : fingers)
				{
					for (auto joint = 0; joint < finger_joints; joint++)
					{
						map.emplace(finger_bone(finger, side, joint + 1), finger_bone(finger, side, joint));
					}
				}
			}

			const auto s2_root = s2.find(s2_arm_root);
			for (auto i = 0u; i < s2.names.size(); i++)
			{
				if (s2.names[i].starts_with("tag_") && s2.has_ancestor(static_cast<int>(i), s2_root) && h1.find(s2.names[i]) >= 0)
				{
					map.emplace(s2.names[i], s2.names[i]);
				}
			}

			std::erase_if(map, [&](const auto& entry)
			{
				return s2.find(entry.first) < 0 || h1.find(entry.second) < 0;
			});

			return map;
		}

		int aim_child(const skeleton& rig, const int bone)
		{
			auto result = -1;
			for (auto i = 0u; i < rig.names.size(); i++)
			{
				if (rig.parents[i] == bone && rig.names[i].starts_with("j_") && rig.names[i].find("twist") == std::string::npos)
				{
					if (result >= 0)
					{
						return -1;
					}

					result = static_cast<int>(i);
				}
			}

			return result;
		}

		bool compute_anatomy(skeleton& rig, const std::unordered_map<std::string, std::string>& names)
		{
			const auto bone = [&](const std::string& s2_name)
			{
				const auto found = names.find(s2_name);
				return found == names.end() ? -1 : rig.find(found->second);
			};

			const auto position = [&](const int index)
			{
				return rig.bind_world[index].position;
			};

			const auto set_anatomy = [&](const int index, const cs::quat_t& frame)
			{
				rig.anatomy[index] = multiply(inverse(rig.bind_world[index].rotation), frame);
			};

			for (const auto* side : sides)
			{
				const auto shoulder = bone(side_bone("shoulder", side));
				const auto elbow = bone(side_bone("elbow", side));
				const auto wrist = bone(side_bone("wrist", side));
				const auto index = bone(finger_bone("index", side, 1));
				const auto mid = bone(finger_bone("mid", side, 1));
				const auto pinky = bone(finger_bone("pinky", side, 1));
				if (shoulder < 0 || elbow < 0 || wrist < 0 || index < 0 || mid < 0 || pinky < 0)
				{
					return false;
				}

				const auto arm_normal = normalize(cross(sub(position(elbow), position(shoulder)), sub(position(wrist), position(elbow))));
				const auto palm_normal = normalize(cross(sub(position(index), position(wrist)), sub(position(pinky), position(wrist))));
				set_anatomy(shoulder, frame_rotation(sub(position(elbow), position(shoulder)), arm_normal));
				set_anatomy(elbow, frame_rotation(sub(position(wrist), position(elbow)), arm_normal));
				set_anatomy(wrist, frame_rotation(sub(position(mid), position(wrist)), palm_normal));

				for (const auto& [s2_name, rig_bone_name] : names)
				{
					const auto joint = rig.find(rig_bone_name);
					if (joint == shoulder || joint == elbow || joint == wrist || !rig_bone_name.starts_with("j_") || !rig.has_ancestor(joint, wrist))
					{
						continue;
					}

					const auto child = aim_child(rig, joint);
					const auto aim = child >= 0 ? sub(position(child), position(joint)) : sub(position(joint), position(rig.parents[joint]));
					set_anatomy(joint, frame_rotation(aim, palm_normal));
				}
			}

			return true;
		}

		std::optional<rigs> load_rigs()
		{
			auto h1 = load_h1_rig();
			auto s2 = load_s2_rig();
			if (!h1 || !s2 || s2->find(s2_arm_root) < 0 || h1->find(body_tag) < 0 || s2->find(body_tag) < 0)
			{
				ZONETOOL_WARNING("the h1 or s2 \"%s\" arm rig is not loaded, viewmodel anims keep the h1 arm bones", rig_name);
				return {};
			}

			const auto h1_body = h1->find(body_tag);
			const auto s2_body = s2->find(body_tag);
			rigs result{.h1 = std::move(*h1), .s2 = std::move(*s2), .h1_body = h1_body, .s2_body = s2_body};
			result.s2_to_h1 = build_bone_map(result.h1, result.s2);

			std::unordered_map<std::string, std::string> s2_names;
			for (const auto& s2_name : result.s2_to_h1 | std::views::keys)
			{
				s2_names.emplace(s2_name, s2_name);
			}

			if (!compute_anatomy(result.h1, result.s2_to_h1) || !compute_anatomy(result.s2, s2_names))
			{
				ZONETOOL_WARNING("the h1 and s2 \"%s\" arm rigs do not have the expected arm and finger bones", rig_name);
				return {};
			}

			for (const auto* side : sides)
			{
				const auto shoulder = result.h1.find(side_bone("shoulder", side));
				for (auto i = 0u; i < result.h1.names.size(); i++)
				{
					if (result.h1.has_ancestor(static_cast<int>(i), shoulder))
					{
						result.h1_arm_bones.emplace(result.h1.names[i]);
					}
				}
			}

			const auto s2_root = result.s2.find(s2_arm_root);
			for (auto i = 0u; i < result.s2.names.size(); i++)
			{
				result.s2_arm_bones.emplace_back(result.s2.has_ancestor(static_cast<int>(i), s2_root));
				result.s2_body_bones.emplace_back(!result.s2_arm_bones.back() && result.s2.has_ancestor(static_cast<int>(i), result.s2_body)
					&& result.h1.find(result.s2.names[i]) >= 0);
			}

			return result;
		}

		const rigs* get_rigs()
		{
			static auto attempted = false;
			if (!attempted)
			{
				attempted = true;
				loaded_rigs = load_rigs();
			}

			return loaded_rigs ? &*loaded_rigs : nullptr;
		}

		std::unordered_map<std::string, bone_keys> collect_keys(const cs::animation_t& animation)
		{
			std::unordered_map<std::string, bone_keys> keys;
			for (const auto& curve : animation.curves)
			{
				auto& bone = keys[curve.node_name];
				for (auto i = 0u; i < curve.key_frames.size(); i++)
				{
					const auto frame = curve.key_frames[i];
					if (curve.property == "rq")
					{
						const auto* value = &curve.key_values[i * 4];
						bone.rotation.emplace_back(frame, cs::quat_normalize({{value[0], value[1], value[2], value[3]}}));
						continue;
					}

					const auto axis = static_cast<std::size_t>(curve.property[1] - 'x');
					if (bone.translation.size() <= i)
					{
						bone.translation.resize(i + 1, {frame, {}});
					}

					bone.translation[i].second.v[axis] = curve.key_values[i];
				}
			}

			return keys;
		}

		template <typename T, typename Blend>
		T sample(const std::vector<std::pair<std::uint32_t, T>>& keys, const std::uint32_t frame, const Blend& blend)
		{
			if (keys.size() == 1 || frame <= keys.front().first)
			{
				return keys.front().second;
			}

			for (auto i = 1u; i < keys.size(); i++)
			{
				if (frame <= keys[i].first)
				{
					const auto span = static_cast<float>(keys[i].first - keys[i - 1].first);
					return blend(keys[i - 1].second, keys[i].second, span > 0.0f ? static_cast<float>(frame - keys[i - 1].first) / span : 0.0f);
				}
			}

			return keys.back().second;
		}

		transform sample_local(const std::unordered_map<std::string, bone_keys>& keys, const std::string& name, const transform& bind,
			const std::uint32_t frame)
		{
			auto local = bind;
			const auto found = keys.find(name);
			if (found == keys.end())
			{
				return local;
			}

			if (!found->second.rotation.empty())
			{
				local.rotation = cs::quat_normalize(sample(found->second.rotation, frame, cs::quat_slerp));
			}

			if (!found->second.translation.empty())
			{
				local.position = sample(found->second.translation, frame, lerp);
			}

			return local;
		}

		std::vector<transform> forward_kinematics(const skeleton& rig, const std::vector<transform>& locals)
		{
			std::vector<transform> world(locals.size());
			for (auto i = 0u; i < locals.size(); i++)
			{
				world[i] = rig.parents[i] < 0 ? locals[i] : compose(world[rig.parents[i]], locals[i]);
			}

			return world;
		}

		class arm_solver
		{
		public:
			arm_solver(const rigs& rigs, const std::unordered_map<std::string, bone_keys>& keys, const std::vector<bool>& shared_bones)
				: rigs_(rigs)
				, keys_(keys)
				, shared_bones_(shared_bones)
			{
				for (const auto* side : sides)
				{
					const auto& s2 = this->rigs_.s2;
					const auto shoulder = s2.find(side_bone("shoulder", side));
					const auto elbow = s2.find(side_bone("elbow", side));
					const auto wrist = s2.find(side_bone("wrist", side));
					this->last_normal_.emplace(side, normalize(cross(sub(s2.bind_world[elbow].position, s2.bind_world[shoulder].position),
						sub(s2.bind_world[wrist].position, s2.bind_world[elbow].position))));
				}
			}

			std::vector<transform> solve(const std::uint32_t frame)
			{
				const auto& h1 = this->rigs_.h1;
				const auto& s2 = this->rigs_.s2;

				std::vector<transform> h1_locals(h1.names.size());
				for (auto i = 0u; i < h1.names.size(); i++)
				{
					h1_locals[i] = sample_local(this->keys_, h1.names[i], h1.bind_local[i], frame);
				}

				this->h1_world_ = forward_kinematics(h1, h1_locals);
				this->s2_world_.assign(s2.names.size(), {});
				for (auto i = 0u; i < s2.names.size(); i++)
				{
					const auto parent = s2.parents[i] < 0 ? transform{} : this->s2_world_[s2.parents[i]];
					if (static_cast<int>(i) == this->rigs_.s2_body)
					{
						this->s2_world_[i] = compose(parent, this->body_local(h1_locals[this->rigs_.h1_body]));
					}
					else if (this->shared_bones_[i])
					{
						this->s2_world_[i] = this->h1_world_[h1.find(s2.names[i])];
					}
					else
					{
						this->s2_world_[i] = compose(parent, s2.bind_local[i]);
					}
				}

				for (auto i = 0u; i < s2.names.size(); i++)
				{
					if (this->rigs_.s2_arm_bones[i] && !is_twist(s2.names[i]))
					{
						this->s2_world_[i] = this->solve_bone(static_cast<int>(i));
					}
				}

				std::vector<transform> s2_locals(s2.names.size());
				for (auto i = 0u; i < s2.names.size(); i++)
				{
					if (this->rigs_.s2_arm_bones[i] && is_twist(s2.names[i]))
					{
						s2_locals[i] = this->solve_twist(static_cast<int>(i));
						this->s2_world_[i] = compose(this->s2_world_[s2.parents[i]], s2_locals[i]);
					}
				}

				for (auto i = 0u; i < s2.names.size(); i++)
				{
					if (!this->rigs_.s2_arm_bones[i] || !is_twist(s2.names[i]))
					{
						s2_locals[i] = s2.parents[i] < 0 ? this->s2_world_[i] : compose(invert(this->s2_world_[s2.parents[i]]), this->s2_world_[i]);
					}
				}

				return s2_locals;
			}

		private:
			static bool is_twist(const std::string& name)
			{
				return name.find("twist") != std::string::npos;
			}

			transform body_local(const transform& h1_local) const
			{
				const auto& h1_bind = this->rigs_.h1.bind_local[this->rigs_.h1_body];
				const auto& s2_bind = this->rigs_.s2.bind_local[this->rigs_.s2_body];
				return {multiply(multiply(h1_local.rotation, inverse(h1_bind.rotation)), s2_bind.rotation),
					add(s2_bind.position, sub(h1_local.position, h1_bind.position))};
			}

			transform h1_anatomy(const int bone) const
			{
				const auto& world = this->h1_world_[bone];
				return {multiply(world.rotation, this->rigs_.h1.anatomy[bone]), world.position};
			}

			transform s2_anatomy(const int bone) const
			{
				const auto& world = this->s2_world_[bone];
				return {multiply(world.rotation, this->rigs_.s2.anatomy[bone]), world.position};
			}

			cs::quat_t s2_rotation(const int bone, const cs::quat_t& anatomy) const
			{
				return multiply(anatomy, inverse(this->rigs_.s2.anatomy[bone]));
			}

			transform bind_child(const int bone) const
			{
				return compose(this->s2_world_[this->rigs_.s2.parents[bone]], this->rigs_.s2.bind_local[bone]);
			}

			int h1_bone(const int s2_bone) const
			{
				const auto found = this->rigs_.s2_to_h1.find(this->rigs_.s2.names[s2_bone]);
				return found == this->rigs_.s2_to_h1.end() ? -1 : this->rigs_.h1.find(found->second);
			}

			const char* side_of(const int s2_bone) const
			{
				for (const auto* side : sides)
				{
					if (this->rigs_.s2.has_ancestor(s2_bone, this->rigs_.s2.find(side_bone("shoulder", side))))
					{
						return side;
					}
				}

				return nullptr;
			}

			transform solve_bone(const int bone)
			{
				const auto& s2 = this->rigs_.s2;
				const auto& name = s2.names[bone];
				for (const auto* clavicle_side : sides)
				{
					if (name == side_bone("clavicle", clavicle_side))
					{
						return this->solve_clavicle(bone, clavicle_side);
					}
				}

				const auto* side = side_of(bone);
				if (side && name == side_bone("shoulder", side))
				{
					return this->solve_shoulder(bone, side);
				}

				if (side && name == side_bone("elbow", side))
				{
					const auto& arm = this->arms_.at(side);
					return {this->s2_rotation(bone, frame_rotation(sub(arm.wrist, arm.elbow), arm.normal)), arm.elbow};
				}

				const auto h1 = this->h1_bone(bone);
				if (h1 < 0)
				{
					return this->bind_child(bone);
				}

				if (side && name == side_bone("wrist", side))
				{
					return {this->s2_rotation(bone, this->h1_anatomy(h1).rotation), this->h1_world_[h1].position};
				}

				auto ancestor = s2.parents[bone];
				while (this->h1_bone(ancestor) < 0)
				{
					ancestor = s2.parents[ancestor];
				}

				const auto relative = compose(invert(this->h1_anatomy(this->h1_bone(ancestor))), this->h1_anatomy(h1));
				const auto anatomy = compose(this->s2_anatomy(ancestor), relative);
				if (name.starts_with("tag_"))
				{
					return {this->s2_rotation(bone, anatomy.rotation), anatomy.position};
				}

				const auto position = this->bind_child(bone).position;
				const auto child = aim_child(s2, bone);
				const auto h1_child = child >= 0 ? this->h1_bone(child) : -1;
				if (h1_child < 0)
				{
					return {this->s2_rotation(bone, anatomy.rotation), position};
				}

				const auto palm_normal = cs::quat_rotate(this->h1_anatomy(h1).rotation, {{0.0f, 0.0f, 1.0f}});
				return {this->s2_rotation(bone, frame_rotation(sub(this->h1_world_[h1_child].position, position), palm_normal)), position};
			}

			transform solve_clavicle(const int clavicle, const char* side) const
			{
				const auto& h1 = this->rigs_.h1;
				const auto& s2 = this->rigs_.s2;
				const auto h1_shoulder = h1.find(side_bone("shoulder", side));
				const auto s2_shoulder = s2.find(side_bone("shoulder", side));
				const auto bind = this->bind_child(clavicle);
				if (s2.parents[s2_shoulder] != clavicle)
				{
					return bind;
				}

				const auto h1_offset = sub(to_local(this->h1_world_[this->rigs_.h1_body], this->h1_world_[h1_shoulder].position),
					to_local(h1.bind_world[this->rigs_.h1_body], h1.bind_world[h1_shoulder].position));
				const auto s2_offset = to_local(s2.bind_world[this->rigs_.s2_body], s2.bind_world[s2_shoulder].position);
				const auto target = to_world(this->s2_world_[this->rigs_.s2_body], add(s2_offset, h1_offset));
				const auto swing = rotation_between(cs::quat_rotate(bind.rotation, s2.bind_local[s2_shoulder].position), sub(target, bind.position));
				return {multiply(swing, bind.rotation), bind.position};
			}

			transform solve_shoulder(const int shoulder, const char* side)
			{
				const auto& h1 = this->rigs_.h1;
				const auto& s2 = this->rigs_.s2;
				const auto elbow = s2.find(side_bone("elbow", side));
				const auto wrist = s2.find(side_bone("wrist", side));
				const auto h1_shoulder = this->h1_world_[h1.find(side_bone("shoulder", side))].position;
				const auto h1_elbow = this->h1_world_[h1.find(side_bone("elbow", side))].position;
				const auto h1_wrist = this->h1_world_[h1.find(side_bone("wrist", side))].position;

				const auto start = this->bind_child(shoulder).position;
				const auto upper = length(s2.bind_local[elbow].position);
				const auto lower = length(s2.bind_local[wrist].position);

				auto& last_normal = this->last_normal_.at(side);
				if (const auto normal = normalize(cross(sub(h1_elbow, h1_shoulder), sub(h1_wrist, h1_elbow))); length(normal) > epsilon)
				{
					last_normal = normal;
				}

				const auto reach = sub(h1_wrist, start);
				const auto direction = normalize(reach);
				const auto distance = std::clamp(length(reach), std::abs(upper - lower) + epsilon, upper + lower - epsilon);
				const auto normal = normalize(sub(last_normal, scale(direction, dot(last_normal, direction))));
				const auto bend = cross(direction, normal);
				const auto cos_angle = std::clamp((upper * upper + distance * distance - lower * lower) / (2.0f * upper * distance), -1.0f, 1.0f);
				const auto sin_angle = std::sqrt(1.0f - cos_angle * cos_angle);

				auto& arm = this->arms_[side];
				arm.elbow = add(start, scale(add(scale(direction, cos_angle), scale(bend, sin_angle)), upper));
				arm.wrist = add(start, scale(direction, distance));
				arm.normal = normal;
				return {this->s2_rotation(shoulder, frame_rotation(sub(arm.elbow, start), normal)), start};
			}

			transform solve_twist(const int twist) const
			{
				const auto& s2 = this->rigs_.s2;
				const auto driver = aim_child(s2, s2.parents[twist]);
				if (driver < 0)
				{
					return s2.bind_local[twist];
				}

				const auto driver_local = compose(invert(this->s2_world_[s2.parents[twist]]), this->s2_world_[driver]);
				const auto delta = multiply(driver_local.rotation, inverse(s2.bind_local[driver].rotation));
				const auto axis = normalize(s2.bind_local[driver].position);
				const auto weight = std::clamp(length(s2.bind_local[twist].position) / std::max(length(s2.bind_local[driver].position), epsilon), 0.0f, 1.0f);
				const auto twist_rotation = cs::quat_slerp(cs::quat_t{}, twist_about(delta, axis), weight);
				return {multiply(twist_rotation, s2.bind_local[twist].rotation), s2.bind_local[twist].position};
			}

			const rigs& rigs_;
			const std::unordered_map<std::string, bone_keys>& keys_;
			const std::vector<bool>& shared_bones_;
			std::vector<transform> h1_world_;
			std::vector<transform> s2_world_;
			std::unordered_map<std::string, arm_pose> arms_;
			std::unordered_map<std::string, cs::vec3_t> last_normal_;
		};

		bool drives_h1_arms(const cs::animation_t& animation, const rigs& rigs)
		{
			return std::ranges::any_of(animation.curves, [&](const cs::curve_t& curve)
			{
				return rigs.h1_arm_bones.contains(curve.node_name);
			});
		}

		cs::animation_t retarget_animation(const cs::animation_t& animation, const rigs& rigs, const std::uint32_t last_frame)
		{
			const auto& s2 = rigs.s2;
			const auto keys = collect_keys(animation);

			std::vector<bool> shared_bones(s2.names.size());
			for (auto bone = 0u; bone < s2.names.size(); bone++)
			{
				const auto& name = s2.names[bone];
				shared_bones[bone] = rigs.s2_body_bones[bone] || (!rigs.s2_arm_bones[bone] && keys.contains(name) && rigs.h1.find(name) >= 0);
			}

			const auto written = [&](const int bone)
			{
				return rigs.s2_arm_bones[bone] || shared_bones[bone];
			};

			const auto replaced = [&](const std::string& name)
			{
				const auto s2_bone = s2.find(name);
				return rigs.h1_arm_bones.contains(name) || (s2_bone >= 0 && written(s2_bone));
			};

			cs::animation_t result{.name = animation.name, .framerate = animation.framerate, .looping = animation.looping};
			std::ranges::copy_if(animation.bones, std::back_inserter(result.bones), [&](const cs::bone_t& bone)
			{
				return !replaced(bone.name);
			});
			std::ranges::copy_if(animation.curves, std::back_inserter(result.curves), [&](const cs::curve_t& curve)
			{
				return !replaced(curve.node_name);
			});

			std::vector<std::vector<transform>> poses;
			arm_solver solver(rigs, keys, shared_bones);
			for (auto frame = 0u; frame <= last_frame; frame++)
			{
				poses.emplace_back(solver.solve(frame));
			}

			for (auto bone = 0; bone < static_cast<int>(s2.names.size()); bone++)
			{
				if (!written(bone))
				{
					continue;
				}

				cs::curve_t rotation{.node_name = s2.names[bone], .property = "rq", .components = 4};
				std::array<cs::curve_t, 3> translation{};
				for (auto axis = 0; axis < 3; axis++)
				{
					translation[axis] = {.node_name = s2.names[bone], .property = std::string("t") + "xyz"[axis]};
				}

				for (auto frame = 0u; frame <= last_frame; frame++)
				{
					const auto& local = poses[frame][bone];
					rotation.key_frames.emplace_back(frame);
					rotation.key_values.insert(rotation.key_values.end(), std::begin(local.rotation.v), std::end(local.rotation.v));
					for (auto axis = 0; axis < 3; axis++)
					{
						translation[axis].key_frames.emplace_back(frame);
						translation[axis].key_values.emplace_back(local.position.v[axis]);
					}
				}

				result.curves.emplace_back(std::move(rotation));
				std::ranges::move(translation, std::back_inserter(result.curves));
			}

			return result;
		}

		void copy_bone_data(const zonetool::s2::XAnimParts* source, zonetool::s2::XAnimParts* target)
		{
			target->dataByteCount = source->dataByteCount;
			target->dataShortCount = source->dataShortCount;
			target->dataIntCount = source->dataIntCount;
			std::memcpy(target->boneCount, source->boneCount, sizeof(target->boneCount));
			target->randomDataByteCount = source->randomDataByteCount;
			target->randomDataShortCount = source->randomDataShortCount;
			target->randomDataIntCount = source->randomDataIntCount;
			target->indexCount = source->indexCount;
			target->names = source->names;
			target->dataByte = source->dataByte;
			target->dataShort = source->dataShort;
			target->dataInt = source->dataInt;
			target->randomDataShort = source->randomDataShort;
			target->randomDataByte = source->randomDataByte;
			target->randomDataInt = source->randomDataInt;
			target->indices = source->indices;
		}
	}

	bool retarget(zonetool::s2::XAnimParts* parts, zone_memory& memory)
	{
		const auto* rigs = get_rigs();
		if (!rigs)
		{
			return false;
		}

		cs::animation_t animation;
		try
		{
			animation = zonetool::s2::cast_xanim::to_animation(parts);
		}
		catch (const std::exception& ex)
		{
			ZONETOOL_FATAL("xanim \"%s\": %s", parts->name, ex.what());
		}

		if (!drives_h1_arms(animation, *rigs))
		{
			return false;
		}

		if (parts->assetType == zonetool::s2::XANIM_ASSET_TYPE_ADDITIVE)
		{
			ZONETOOL_WARNING("xanim \"%s\": additive anims are not retargeted, its h1 arm bones do not match the s2 arm rig", parts->name);
			return false;
		}

		const auto* retargeted = zonetool::s2::cast_xanim::from_animation(retarget_animation(animation, *rigs, parts->numframes), &memory);
		if (retargeted->numframes != parts->numframes)
		{
			ZONETOOL_FATAL("xanim \"%s\": the retargeted anim has %u frames, the h1 anim has %u", parts->name, retargeted->numframes, parts->numframes);
		}

		copy_bone_data(retargeted, parts);
		return true;
	}

	void clear()
	{
		loaded_rigs.reset();
	}
}
