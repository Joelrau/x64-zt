#pragma once

#include "cast.hpp"

namespace zonetool::cast
{
	struct vec2_t
	{
		float v[2]{};
	};

	struct vec3_t
	{
		float v[3]{};
	};

	struct quat_t
	{
		float v[4]{0.0f, 0.0f, 0.0f, 1.0f};
	};

	quat_t quat_normalize(const quat_t& q);
	quat_t quat_multiply(const quat_t& a, const quat_t& b);
	quat_t quat_conjugate(const quat_t& q);
	vec3_t quat_rotate(const quat_t& q, const vec3_t& v);
	float quat_dot(const quat_t& a, const quat_t& b);
	quat_t quat_slerp(const quat_t& a, const quat_t& b, float t);

	struct bone_t
	{
		std::string name;
		int parent_index = -1;

		vec3_t local_position{};
		quat_t local_rotation{};
		vec3_t world_position{};
		quat_t world_rotation{};
		vec3_t scale{{1.0f, 1.0f, 1.0f}};

		bool has_local = false;
		bool has_world = false;
		bool segment_scale_compensate = true;
	};

	struct influence_t
	{
		std::uint32_t bone = 0;
		float weight = 0.0f;
	};

	struct mesh_t
	{
		std::string name;
		std::string material;

		std::vector<vec3_t> positions;
		std::vector<vec3_t> normals;
		std::vector<vec3_t> tangents;
		std::vector<vec2_t> uvs;
		std::vector<std::uint32_t> colors;
		std::vector<std::uint32_t> faces;
		std::vector<std::vector<influence_t>> influences;

		std::size_t uv_layer_count = 0;
		std::size_t color_layer_count = 0;
		std::size_t max_influence = 0;
		bool quaternion_skinning = false;

		[[nodiscard]] std::size_t vertex_count() const
		{
			return this->positions.size();
		}

		[[nodiscard]] std::size_t triangle_count() const
		{
			return this->faces.size() / 3;
		}
	};

	enum class curve_mode_t
	{
		absolute,
		relative,
		additive,
	};

	struct curve_t
	{
		std::string node_name;
		std::string property;
		curve_mode_t mode = curve_mode_t::absolute;
		float additive_weight = 1.0f;

		std::vector<std::uint32_t> key_frames;
		std::vector<float> key_values;
		std::size_t components = 1;
	};

	struct curve_mode_override_t
	{
		std::string node_name;
		curve_mode_t mode = curve_mode_t::absolute;
		bool override_translation = false;
		bool override_rotation = false;
		bool override_scale = false;
	};

	struct notification_t
	{
		std::string name;
		std::vector<std::uint32_t> frames;
	};

	struct animation_t
	{
		std::string name;
		float framerate = 30.0f;
		bool looping = false;

		std::vector<bone_t> bones;
		std::vector<curve_t> curves;
		std::vector<curve_mode_override_t> overrides;
		std::vector<notification_t> notifications;
	};

	struct model_t
	{
		std::string name;
		std::vector<bone_t> bones;
		std::vector<mesh_t> meshes;

		float lod_distance = 0.0f;
		bool has_lod_distance = false;
	};

	struct scene_t
	{
		std::string display_name;
		std::string up_axis = "z";
		std::vector<model_t> models;
		std::vector<animation_t> animations;
	};

	scene_t build_scene(const file& source, const std::string& display_name);

	std::vector<std::uint32_t> normalize_skeleton(std::vector<bone_t>& bones, const std::string& owner);
}
