#include <std_include.hpp>

#include "../../include.hpp"
#include "../common/stock_zone.hpp"
#include "bone_rig.hpp"

namespace zonetool::h1::converter::s2::bone_rig
{
	namespace
	{
		constexpr auto tag_angle_scale = 32767.0f;

		transform tag_transform(const short* angles, const float* positions)
		{
			return {cast::quat_normalize({{angles[0] / tag_angle_scale, angles[1] / tag_angle_scale, angles[2] / tag_angle_scale, angles[3] / tag_angle_scale}}),
				{{positions[0], positions[1], positions[2]}}};
		}

		template <typename Model, typename AnimMat>
		std::optional<skeleton> load_skeleton(const Model* model)
		{
			const auto has_children = model && model->numBones > model->numRootBones;
			if (!model || !model->boneNames || !model->baseMat || (has_children && (!model->parentList || !model->tagAngles || !model->tagPositions)))
			{
				return {};
			}

			const auto* mats = reinterpret_cast<const AnimMat*>(model->baseMat);
			const auto* angles = reinterpret_cast<const short*>(model->tagAngles);
			const auto* positions = reinterpret_cast<const float*>(model->tagPositions);

			skeleton rig;
			for (auto i = 0; i < model->numBones; i++)
			{
				const auto child = i - model->numRootBones;
				rig.names.emplace_back(SL_ConvertToString(model->boneNames[i]));
				rig.parents.emplace_back(child < 0 ? -1 : i - model->parentList[child]);
				rig.bind_world.emplace_back(to_transform(mats[i]));
				rig.pose_local.emplace_back(child < 0 ? rig.bind_world.back() : tag_transform(angles + 4 * child, positions + 3 * child));
			}

			rig.finish();
			return rig;
		}

		cast::quat_t quat_from_axes(const cast::vec3_t& x, const cast::vec3_t& y, const cast::vec3_t& z)
		{
			cast::quat_t q{};
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

			return cast::quat_normalize(q);
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
	}

	void skeleton::finish()
	{
		this->bind_local.resize(this->names.size());
		this->anatomy.resize(this->names.size());
		for (auto i = 0u; i < this->names.size(); i++)
		{
			this->indices.emplace(this->names[i], static_cast<int>(i));
			this->bind_local[i] = this->parents[i] < 0
				? this->bind_world[i]
				: compose(invert(this->bind_world[this->parents[i]]), this->bind_world[i]);
		}
	}

	int skeleton::find(const std::string& name) const
	{
		const auto found = this->indices.find(name);
		return found == this->indices.end() ? -1 : found->second;
	}

	bool skeleton::has(const std::string& name) const
	{
		return this->indices.find(name) != this->indices.end();
	}

	bool skeleton::has_ancestor(const int bone, const int ancestor) const
	{
		for (auto index = bone; index >= 0; index = this->parents[index])
		{
			if (index == ancestor)
			{
				return true;
			}
		}

		return false;
	}

	transform compose(const transform& parent, const transform& local)
	{
		const auto rotated = cast::quat_rotate(parent.rotation, local.position);
		return {cast::quat_normalize(cast::quat_multiply(parent.rotation, local.rotation)),
			{{parent.position.v[0] + rotated.v[0], parent.position.v[1] + rotated.v[1], parent.position.v[2] + rotated.v[2]}}};
	}

	transform invert(const transform& value)
	{
		const auto rotation = cast::quat_conjugate(value.rotation);
		const auto position = cast::quat_rotate(rotation, {{-value.position.v[0], -value.position.v[1], -value.position.v[2]}});
		return {rotation, position};
	}

	cast::vec3_t add(const cast::vec3_t& a, const cast::vec3_t& b)
	{
		return {{a.v[0] + b.v[0], a.v[1] + b.v[1], a.v[2] + b.v[2]}};
	}

	cast::vec3_t sub(const cast::vec3_t& a, const cast::vec3_t& b)
	{
		return {{a.v[0] - b.v[0], a.v[1] - b.v[1], a.v[2] - b.v[2]}};
	}

	cast::vec3_t scale(const cast::vec3_t& a, const float s)
	{
		return {{a.v[0] * s, a.v[1] * s, a.v[2] * s}};
	}

	float dot(const cast::vec3_t& a, const cast::vec3_t& b)
	{
		return a.v[0] * b.v[0] + a.v[1] * b.v[1] + a.v[2] * b.v[2];
	}

	cast::vec3_t cross(const cast::vec3_t& a, const cast::vec3_t& b)
	{
		return {{a.v[1] * b.v[2] - a.v[2] * b.v[1], a.v[2] * b.v[0] - a.v[0] * b.v[2], a.v[0] * b.v[1] - a.v[1] * b.v[0]}};
	}

	float length(const cast::vec3_t& a)
	{
		return std::sqrt(dot(a, a));
	}

	cast::vec3_t normalize(const cast::vec3_t& a)
	{
		const auto len = length(a);
		return len > epsilon ? scale(a, 1.0f / len) : cast::vec3_t{};
	}

	cast::vec3_t lerp(const cast::vec3_t& a, const cast::vec3_t& b, const float t)
	{
		return add(a, scale(sub(b, a), t));
	}

	cast::quat_t inverse(const cast::quat_t& q)
	{
		return cast::quat_conjugate(q);
	}

	cast::quat_t multiply(const cast::quat_t& a, const cast::quat_t& b)
	{
		return cast::quat_normalize(cast::quat_multiply(a, b));
	}

	cast::vec3_t to_world(const transform& space, const cast::vec3_t& point)
	{
		return add(space.position, cast::quat_rotate(space.rotation, point));
	}

	cast::vec3_t to_local(const transform& space, const cast::vec3_t& point)
	{
		return cast::quat_rotate(inverse(space.rotation), sub(point, space.position));
	}

	cast::quat_t frame_rotation(const cast::vec3_t& aim, const cast::vec3_t& up)
	{
		const auto x = normalize(aim);
		const auto z = normalize(sub(up, scale(x, dot(up, x))));
		return quat_from_axes(x, cross(z, x), z);
	}

	cast::quat_t rotation_between(const cast::vec3_t& from, const cast::vec3_t& to)
	{
		const auto a = normalize(from);
		const auto b = normalize(to);
		const auto axis = cross(a, b);
		return cast::quat_normalize({{axis.v[0], axis.v[1], axis.v[2], 1.0f + dot(a, b)}});
	}

	cast::quat_t twist_about(const cast::quat_t& q, const cast::vec3_t& axis)
	{
		const auto projected = scale(axis, dot({{q.v[0], q.v[1], q.v[2]}}, axis));
		const cast::quat_t twist{{projected.v[0], projected.v[1], projected.v[2], q.v[3]}};
		return cast::quat_dot(twist, twist) > epsilon ? cast::quat_normalize(twist) : cast::quat_t{};
	}

	cast::vec3_t bend_normal(const cast::vec3_t& shoulder, const cast::vec3_t& elbow, const cast::vec3_t& wrist)
	{
		return normalize(cross(sub(elbow, shoulder), sub(wrist, elbow)));
	}

	std::string side_bone(const char* name, const char* side)
	{
		return std::format("j_{}_{}", name, side);
	}

	arm_pose solve_two_bone(const cast::vec3_t& start, const cast::vec3_t& hand, const float upper, const float lower, const cast::vec3_t& pole)
	{
		const auto reach = sub(hand, start);
		const auto direction = normalize(reach);
		const auto distance = std::clamp(length(reach), std::abs(upper - lower) + epsilon, upper + lower - epsilon);
		const auto normal = normalize(sub(pole, scale(direction, dot(pole, direction))));
		const auto bend = cross(direction, normal);
		const auto cos_angle = std::clamp((upper * upper + distance * distance - lower * lower) / (2.0f * upper * distance), -1.0f, 1.0f);
		const auto sin_angle = std::sqrt(1.0f - cos_angle * cos_angle);
		return {add(start, scale(add(scale(direction, cos_angle), scale(bend, sin_angle)), upper)), add(start, scale(direction, distance)), normal, hand};
	}

	transform swing_to(const transform& bone, const cast::vec3_t& child_offset, const cast::vec3_t& start, const cast::vec3_t& child)
	{
		return {multiply(rotation_between(cast::quat_rotate(bone.rotation, child_offset), sub(child, start)), bone.rotation), start};
	}

	transform blend(const transform& a, const transform& b, const float t)
	{
		return {cast::quat_normalize(cast::quat_slerp(a.rotation, b.rotation, t)), lerp(a.position, b.position, t)};
	}

	float smoothstep(const float value)
	{
		const auto t = std::clamp(value, 0.0f, 1.0f);
		return t * t * (3.0f - 2.0f * t);
	}

	std::unordered_map<std::string, bone_keys> collect_keys(const cast::animation_t& animation)
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
					bone.rotation.emplace_back(frame, cast::quat_normalize({{value[0], value[1], value[2], value[3]}}));
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
			local.rotation = cast::quat_normalize(sample(found->second.rotation, frame, cast::quat_slerp));
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

	std::vector<transform> sample_locals(const skeleton& rig, const std::unordered_map<std::string, bone_keys>& keys, const std::uint32_t frame)
	{
		std::vector<transform> locals(rig.names.size());
		for (auto i = 0u; i < rig.names.size(); i++)
		{
			locals[i] = sample_local(keys, rig.names[i], rig.pose_local[i], frame);
		}

		return locals;
	}

	std::vector<transform> to_locals(const skeleton& rig, const std::vector<transform>& world)
	{
		std::vector<transform> locals(world.size());
		for (auto i = 0u; i < world.size(); i++)
		{
			locals[i] = rig.parents[i] < 0 ? world[i] : compose(invert(world[rig.parents[i]]), world[i]);
		}

		return locals;
	}

	void add_bone_curves(std::vector<cast::curve_t>& curves, const std::string& name, const std::vector<std::vector<transform>>& poses,
		const int bone)
	{
		cast::curve_t rotation{.node_name = name, .property = "rq", .components = 4};
		std::array<cast::curve_t, 3> translation{};
		for (auto axis = 0; axis < 3; axis++)
		{
			translation[axis] = {.node_name = name, .property = std::string("t") + "xyz"[axis]};
		}

		for (auto frame = 0u; frame < poses.size(); frame++)
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

		curves.emplace_back(std::move(rotation));
		std::ranges::move(translation, std::back_inserter(curves));
	}

	cast::animation_t pose_animation(const skeleton& rig, const cast::animation_t& idle, const std::unordered_map<std::string, bone_keys>& idle_keys,
		const cast::animation_t& stock, const std::vector<std::vector<transform>>& poses, const std::string& name)
	{
		const auto stock_keys = collect_keys(stock);
		cast::animation_t result{.name = name, .framerate = stock.framerate, .bones = idle.bones};
		for (const auto& curve : idle.curves)
		{
			if (!rig.has(curve.node_name) && !curve.key_frames.empty())
			{
				auto& held = result.curves.emplace_back(curve); // gun bones hold their first idle key
				held.key_frames = {0};
				held.key_values.resize(curve.components);
			}
		}

		for (auto bone = 0; bone < static_cast<int>(rig.names.size()); bone++)
		{
			const auto camera = std::ranges::find(camera_bones, rig.names[bone]) != camera_bones.end();
			if (idle_keys.contains(rig.names[bone]) || (camera && stock_keys.contains(rig.names[bone])))
			{
				add_bone_curves(result.curves, rig.names[bone], poses, bone);
			}
		}

		return result;
	}

	std::optional<skeleton> load_h1_skeleton(const char* name)
	{
		const auto* model = db_find_x_asset_header_safe(ASSET_TYPE_XMODEL, name).model;
		return load_skeleton<XModel, DObjAnimMat>(model);
	}

	std::optional<skeleton> load_s2_skeleton(const char* name)
	{
		db_scope _(stock_zone::common_zone().db());

		const auto* model = zonetool::s2::db_find_x_asset_header_safe(zonetool::s2::ASSET_TYPE_XMODEL, name).model;
		if (!model || (model->flags & zonetool::s2::XMODEL_FLAG_VARIANTS))
		{
			return {};
		}

		return load_skeleton<zonetool::s2::XModelBase, zonetool::s2::DObjAnimMat>(model->base);
	}

	std::optional<stock_anim> load_stock_anim(const std::string& name)
	{
		db_scope _(stock_zone::common_zone().db());
		const auto* parts = zonetool::s2::db_find_x_asset_header_safe(zonetool::s2::ASSET_TYPE_XANIM, name.data()).parts;
		if (!parts)
		{
			return {};
		}

		return stock_anim{zonetool::s2::cast_xanim::to_animation(parts), parts->numframes};
	}
}
