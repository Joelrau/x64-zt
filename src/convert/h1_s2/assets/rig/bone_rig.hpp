#pragma once

#include <formats/cast/cast_scene.hpp>

namespace zonetool::h1::converter::s2::bone_rig
{
	struct transform
	{
		cast::quat_t rotation;
		cast::vec3_t position;
	};

	struct skeleton
	{
		std::vector<std::string> names;
		std::vector<int> parents;
		std::vector<transform> bind_world;
		std::vector<transform> bind_local;
		std::vector<cast::quat_t> anatomy;
		std::unordered_map<std::string, int> indices;

		void finish();
		[[nodiscard]] int find(const std::string& name) const;
		[[nodiscard]] bool has(const std::string& name) const;
		[[nodiscard]] bool has_ancestor(int bone, int ancestor) const;
	};

	transform compose(const transform& parent, const transform& local);
	transform invert(const transform& value);

	template <typename AnimMat>
	transform to_transform(const AnimMat& mat)
	{
		return {cast::quat_normalize({{mat.quat[0], mat.quat[1], mat.quat[2], mat.quat[3]}}),
			{{mat.trans[0], mat.trans[1], mat.trans[2]}}};
	}

	std::optional<skeleton> load_h1_skeleton(const char* name);
	std::optional<skeleton> load_s2_skeleton(const char* name);
}
