#pragma once

#include <formats/cast/cast_scene.hpp>

namespace zonetool::h1::converter::s2::bone_rig
{
	constexpr auto epsilon = 0.0001f;
	constexpr auto body_tag = "tag_torso";
	constexpr std::array camera_bones{"tag_cambone", "tag_camera", "tag_player"};

	struct transform
	{
		cast::quat_t rotation;
		cast::vec3_t position;
	};

	struct bone_keys
	{
		std::vector<std::pair<std::uint32_t, cast::quat_t>> rotation;
		std::vector<std::pair<std::uint32_t, cast::vec3_t>> translation;
	};

	struct arm_pose
	{
		cast::vec3_t elbow;
		cast::vec3_t wrist;
		cast::vec3_t normal;
		cast::vec3_t hand;
	};

	struct stock_anim
	{
		cast::animation_t animation;
		std::uint32_t last_frame;
	};

	struct skeleton
	{
		std::vector<std::string> names;
		std::vector<int> parents;
		std::vector<transform> bind_world;
		std::vector<transform> bind_local;
		std::vector<transform> pose_local;
		std::vector<cast::quat_t> anatomy;
		std::unordered_map<std::string, int> indices;

		void finish();
		[[nodiscard]] int find(const std::string& name) const;
		[[nodiscard]] bool has(const std::string& name) const;
		[[nodiscard]] bool has_ancestor(int bone, int ancestor) const;
	};

	transform compose(const transform& parent, const transform& local);
	transform invert(const transform& value);

	cast::vec3_t add(const cast::vec3_t& a, const cast::vec3_t& b);
	cast::vec3_t sub(const cast::vec3_t& a, const cast::vec3_t& b);
	cast::vec3_t scale(const cast::vec3_t& a, float s);
	float dot(const cast::vec3_t& a, const cast::vec3_t& b);
	cast::vec3_t cross(const cast::vec3_t& a, const cast::vec3_t& b);
	float length(const cast::vec3_t& a);
	cast::vec3_t normalize(const cast::vec3_t& a);
	cast::vec3_t lerp(const cast::vec3_t& a, const cast::vec3_t& b, float t);
	cast::quat_t inverse(const cast::quat_t& q);
	cast::quat_t multiply(const cast::quat_t& a, const cast::quat_t& b);
	cast::vec3_t to_world(const transform& space, const cast::vec3_t& point);
	cast::vec3_t to_local(const transform& space, const cast::vec3_t& point);
	cast::quat_t frame_rotation(const cast::vec3_t& aim, const cast::vec3_t& up);
	cast::quat_t rotation_between(const cast::vec3_t& from, const cast::vec3_t& to);
	cast::quat_t twist_about(const cast::quat_t& q, const cast::vec3_t& axis);
	cast::vec3_t bend_normal(const cast::vec3_t& shoulder, const cast::vec3_t& elbow, const cast::vec3_t& wrist);
	arm_pose solve_two_bone(const cast::vec3_t& start, const cast::vec3_t& hand, float upper, float lower, const cast::vec3_t& pole);
	transform swing_to(const transform& bone, const cast::vec3_t& child_offset, const cast::vec3_t& start, const cast::vec3_t& child);
	transform blend(const transform& a, const transform& b, float t);
	float smoothstep(float value);
	std::string side_bone(const char* name, const char* side);

	std::unordered_map<std::string, bone_keys> collect_keys(const cast::animation_t& animation);
	transform sample_local(const std::unordered_map<std::string, bone_keys>& keys, const std::string& name, const transform& bind, std::uint32_t frame);
	std::vector<transform> sample_locals(const skeleton& rig, const std::unordered_map<std::string, bone_keys>& keys, std::uint32_t frame);
	std::vector<transform> forward_kinematics(const skeleton& rig, const std::vector<transform>& locals);
	std::vector<transform> to_locals(const skeleton& rig, const std::vector<transform>& world);
	void add_bone_curves(std::vector<cast::curve_t>& curves, const std::string& name, const std::vector<std::vector<transform>>& poses, int bone);
	cast::animation_t pose_animation(const skeleton& rig, const cast::animation_t& idle, const std::unordered_map<std::string, bone_keys>& idle_keys,
		const cast::animation_t& stock, const std::vector<std::vector<transform>>& poses, const std::string& name);

	template <typename AnimMat>
	transform to_transform(const AnimMat& mat)
	{
		return {cast::quat_normalize({{mat.quat[0], mat.quat[1], mat.quat[2], mat.quat[3]}}),
			{{mat.trans[0], mat.trans[1], mat.trans[2]}}};
	}

	std::optional<skeleton> load_h1_skeleton(const char* name);
	std::optional<skeleton> load_s2_skeleton(const char* name);
	std::optional<stock_anim> load_stock_anim(const std::string& name);
}
