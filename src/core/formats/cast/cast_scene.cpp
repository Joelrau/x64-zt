#include <std_include.hpp>
#include "cast_scene.hpp"

#include "../../log.hpp"

#include <utils/string.hpp>

#include <numeric>

namespace zonetool::cast
{
	namespace
	{
		[[noreturn]] void fail(const std::string& message)
		{
			throw cast_error(message);
		}

		curve_mode_t parse_curve_mode(const std::string& mode, const std::string& owner)
		{
			if (mode == "absolute" || mode.empty())
			{
				return curve_mode_t::absolute;
			}
			if (mode == "relative")
			{
				return curve_mode_t::relative;
			}
			if (mode == "additive")
			{
				return curve_mode_t::additive;
			}

			ZONETOOL_WARNING("CAST %s: unknown curve mode \"%s\", treating it as absolute",
				owner.data(), mode.data());
			return curve_mode_t::absolute;
		}

		class basis_converter
		{
		public:
			explicit basis_converter(const std::string& up_axis)
				: y_up_(up_axis == "y" || up_axis == "Y")
				, scale_(get_import_scale())
			{
			}

			[[nodiscard]] bool active() const
			{
				return this->y_up_;
			}

			[[nodiscard]] float scale() const
			{
				return this->scale_;
			}

			[[nodiscard]] vec3_t convert_position(const vec3_t& in) const
			{
				auto out = this->convert(in);
				out.v[0] *= this->scale_;
				out.v[1] *= this->scale_;
				out.v[2] *= this->scale_;
				return out;
			}

			[[nodiscard]] vec3_t convert(const vec3_t& in) const
			{
				if (!this->y_up_)
				{
					return in;
				}

				vec3_t out{};
				out.v[0] = in.v[0];
				out.v[1] = -in.v[2];
				out.v[2] = in.v[1];
				return out;
			}

			[[nodiscard]] quat_t convert(const quat_t& in) const
			{
				if (!this->y_up_)
				{
					return in;
				}

				constexpr float half_sqrt2 = 0.70710678118654752440f;
				const quat_t rotation{{half_sqrt2, 0.0f, 0.0f, half_sqrt2}};
				return quat_multiply(quat_multiply(rotation, in), quat_conjugate(rotation));
			}

		private:
			bool y_up_;
			float scale_;
		};

		std::string get_up_axis(const file& source)
		{
			const auto metadata = source.all_of_type(id_metadata);
			for (const auto* node : metadata)
			{
				const auto up = node->get_string("up");
				if (!up.empty())
				{
					return up;
				}
			}
			return "z";
		}

		void read_bone(const node& bone_node, bone_t& bone, const basis_converter& basis)
		{
			bone.name = bone_node.get_string("n");

			const auto* parent = bone_node.get_property("p");
			if (parent && parent->value_count() >= 1)
			{
				const auto raw = static_cast<std::uint32_t>(parent->as_integer(0));
				bone.parent_index = static_cast<int>(static_cast<std::int32_t>(raw));
			}

			bone.segment_scale_compensate = bone_node.get_bool("ssc", true);

			float value[4]{};
			if (bone_node.get_vec3("lp", value))
			{
				bone.local_position = basis.convert_position(vec3_t{{value[0], value[1], value[2]}});
				bone.has_local = true;
			}
			if (bone_node.get_vec4("lr", value))
			{
				bone.local_rotation = basis.convert(quat_normalize(quat_t{{value[0], value[1], value[2], value[3]}}));
				bone.has_local = true;
			}
			if (bone_node.get_vec3("wp", value))
			{
				bone.world_position = basis.convert_position(vec3_t{{value[0], value[1], value[2]}});
				bone.has_world = true;
			}
			if (bone_node.get_vec4("wr", value))
			{
				bone.world_rotation = basis.convert(quat_normalize(quat_t{{value[0], value[1], value[2], value[3]}}));
				bone.has_world = true;
			}
			if (bone_node.get_vec3("s", value))
			{
				bone.scale = vec3_t{{value[0], value[1], value[2]}};
			}
		}

		std::vector<bone_t> read_skeleton(const node* skeleton, const basis_converter& basis)
		{
			std::vector<bone_t> bones;
			if (!skeleton)
			{
				return bones;
			}

			const auto bone_nodes = skeleton->children_of_type(id_bone);
			bones.resize(bone_nodes.size());
			for (std::size_t i = 0; i < bone_nodes.size(); i++)
			{
				read_bone(*bone_nodes[i], bones[i], basis);
			}

			return bones;
		}

		std::string resolve_material_name(const node& mesh_node, const node& model_node, const file& source,
			const std::string& owner)
		{
			const auto* material = mesh_node.get_property("m");
			if (!material || material->value_count() < 1)
			{
				return {};
			}

			const auto hash = material->as_integer(0);

			const node* material_node = nullptr;
			for (const auto& child : model_node.children)
			{
				if (child->id == id_material && child->hash == hash)
				{
					material_node = child.get();
					break;
				}
			}

			if (!material_node)
			{
				const auto* found = source.node_by_hash(hash);
				if (found && found->id == id_material)
				{
					material_node = found;
				}
			}

			if (!material_node)
			{
				ZONETOOL_WARNING("CAST %s: mesh \"%s\" references material hash 0x%llX which does not exist in the file",
					owner.data(), mesh_node.get_string("n", "<unnamed>").data(), hash);
				return {};
			}

			return material_node->get_string("n");
		}

		void read_mesh_influences(const node& mesh_node, mesh_t& mesh, const std::string& owner)
		{
			const auto* bones = mesh_node.get_property("wb");
			const auto* weights = mesh_node.get_property("wv");

			if (!bones && !weights)
			{
				return;
			}

			if (!bones || !weights)
			{
				ZONETOOL_WARNING("CAST %s: mesh \"%s\" only provides one half of its skinning data, ignoring weights",
					owner.data(), mesh.name.data());
				return;
			}

			const auto vertex_count = mesh.vertex_count();
			auto max_influence = mesh.max_influence;
			if (max_influence == 0)
			{
				if (vertex_count == 0 || bones->value_count() % vertex_count != 0)
				{
					fail(utils::string::va(
						"CAST %s: mesh \"%s\" does not declare a maximum weight influence and it cannot be derived "
						"(%llu weights for %llu vertices)",
						owner.data(), mesh.name.data(), bones->value_count(), vertex_count));
				}
				max_influence = bones->value_count() / vertex_count;
				mesh.max_influence = max_influence;
			}

			const auto expected = vertex_count * max_influence;
			if (bones->value_count() < expected || weights->value_count() < expected)
			{
				fail(utils::string::va(
					"CAST %s: mesh \"%s\" declares %llu influences for %llu vertices but only provides %llu bone "
					"indices and %llu weight values",
					owner.data(), mesh.name.data(), max_influence, vertex_count,
					bones->value_count(), weights->value_count()));
			}

			mesh.influences.resize(vertex_count);
			for (std::size_t vertex = 0; vertex < vertex_count; vertex++)
			{
				auto& target = mesh.influences[vertex];
				target.reserve(max_influence);

				for (std::size_t i = 0; i < max_influence; i++)
				{
					const auto index = vertex * max_influence + i;
					const auto weight = weights->as_float(index);
					if (weight <= 0.0f)
					{
						continue;
					}

					influence_t influence{};
					influence.bone = static_cast<std::uint32_t>(bones->as_integer(index));
					influence.weight = weight;
					target.emplace_back(influence);
				}
			}
		}

		void read_mesh(const node& mesh_node, const node& model_node, const file& source, mesh_t& mesh,
			const basis_converter& basis, const std::string& owner)
		{
			mesh.name = mesh_node.get_string("n", "<unnamed>");

			const auto* positions = mesh_node.get_property("vp");
			if (!positions || positions->type != property_type::vector3)
			{
				fail(utils::string::va("CAST %s: mesh \"%s\" is missing its vertex position buffer",
					owner.data(), mesh.name.data()));
			}

			const auto vertex_count = positions->element_count;
			mesh.positions.resize(vertex_count);
			for (std::size_t i = 0; i < vertex_count; i++)
			{
				mesh.positions[i] = basis.convert_position(vec3_t{{
					positions->as_float(i * 3 + 0),
					positions->as_float(i * 3 + 1),
					positions->as_float(i * 3 + 2),
				}});
			}

			const auto* normals = mesh_node.get_property("vn");
			if (normals && normals->type == property_type::vector3)
			{
				if (normals->element_count != vertex_count)
				{
					fail(utils::string::va("CAST %s: mesh \"%s\" has %llu normals but %llu vertices",
						owner.data(), mesh.name.data(), normals->element_count, vertex_count));
				}

				mesh.normals.resize(vertex_count);
				for (std::size_t i = 0; i < vertex_count; i++)
				{
					mesh.normals[i] = basis.convert(vec3_t{{
						normals->as_float(i * 3 + 0),
						normals->as_float(i * 3 + 1),
						normals->as_float(i * 3 + 2),
					}});
				}
			}

			const auto* tangents = mesh_node.get_property("vt");
			if (tangents && tangents->type == property_type::vector3)
			{
				if (tangents->element_count != vertex_count)
				{
					ZONETOOL_WARNING("CAST %s: mesh \"%s\" has %llu tangents but %llu vertices, tangents are ignored",
						owner.data(), mesh.name.data(), tangents->element_count, vertex_count);
				}
				else
				{
					mesh.tangents.resize(vertex_count);
					for (std::size_t i = 0; i < vertex_count; i++)
					{
						mesh.tangents[i] = basis.convert(vec3_t{{
							tangents->as_float(i * 3 + 0),
							tangents->as_float(i * 3 + 1),
							tangents->as_float(i * 3 + 2),
						}});
					}
				}
			}

			mesh.uv_layer_count = static_cast<std::size_t>(mesh_node.get_integer("ul", 0));
			if (mesh.uv_layer_count == 0 && mesh_node.get_property("u0"))
			{
				mesh.uv_layer_count = 1;
			}

			if (mesh.uv_layer_count > 0)
			{
				const auto* uvs = mesh_node.get_property("u0");
				if (uvs && uvs->type == property_type::vector2 && uvs->element_count == vertex_count)
				{
					mesh.uvs.resize(vertex_count);
					for (std::size_t i = 0; i < vertex_count; i++)
					{
						mesh.uvs[i].v[0] = uvs->as_float(i * 2 + 0);
						mesh.uvs[i].v[1] = uvs->as_float(i * 2 + 1);
					}
				}
				else
				{
					ZONETOOL_WARNING("CAST %s: mesh \"%s\" declares %llu uv layers but layer 0 is missing or invalid",
						owner.data(), mesh.name.data(), mesh.uv_layer_count);
					mesh.uv_layer_count = 0;
				}

				if (mesh.uv_layer_count > 1)
				{
					ZONETOOL_WARNING("CAST %s: mesh \"%s\" has %llu uv layers, only the first one is used",
						owner.data(), mesh.name.data(), mesh.uv_layer_count);
				}
			}

			mesh.color_layer_count = static_cast<std::size_t>(mesh_node.get_integer("cl", 0));
			const auto* colors = mesh_node.get_property("c0");
			if (!colors)
			{
				colors = mesh_node.get_property("vc");
				if (colors && mesh.color_layer_count == 0)
				{
					mesh.color_layer_count = 1;
				}
			}

			if (colors && colors->element_count == vertex_count)
			{
				mesh.colors.resize(vertex_count);
				if (colors->type == property_type::vector4)
				{
					for (std::size_t i = 0; i < vertex_count; i++)
					{
						std::uint32_t packed = 0;
						for (std::size_t c = 0; c < 4; c++)
						{
							const auto value = std::clamp(colors->as_float(i * 4 + c), 0.0f, 1.0f);
							packed |= static_cast<std::uint32_t>(value * 255.0f + 0.5f) << (c * 8);
						}
						mesh.colors[i] = packed;
					}
				}
				else
				{
					for (std::size_t i = 0; i < vertex_count; i++)
					{
						mesh.colors[i] = static_cast<std::uint32_t>(colors->as_integer(i));
					}
				}

				if (mesh.color_layer_count == 0)
				{
					mesh.color_layer_count = 1;
				}
			}
			else if (colors)
			{
				ZONETOOL_WARNING("CAST %s: mesh \"%s\" has %llu vertex colors but %llu vertices, colors are ignored",
					owner.data(), mesh.name.data(), colors->element_count, vertex_count);
				mesh.color_layer_count = 0;
			}

			const auto* faces = mesh_node.get_property("f");
			if (!faces || !faces->is_integer())
			{
				fail(utils::string::va("CAST %s: mesh \"%s\" is missing its face buffer",
					owner.data(), mesh.name.data()));
			}

			if (faces->value_count() % 3 != 0)
			{
				fail(utils::string::va("CAST %s: mesh \"%s\" has %llu face indices which is not a multiple of 3",
					owner.data(), mesh.name.data(), faces->value_count()));
			}

			mesh.faces.resize(faces->value_count());
			for (std::size_t i = 0; i < mesh.faces.size(); i++)
			{
				const auto index = faces->as_integer(i);
				if (index >= vertex_count)
				{
					fail(utils::string::va(
						"CAST %s: mesh \"%s\" face index %llu references vertex %llu, but the mesh only has %llu vertices",
						owner.data(), mesh.name.data(), i, index, vertex_count));
				}
				mesh.faces[i] = static_cast<std::uint32_t>(index);
			}

			mesh.max_influence = static_cast<std::size_t>(mesh_node.get_integer("mi", 0));

			const auto skinning_method = mesh_node.get_string("sm", "linear");
			if (skinning_method != "linear" && !skinning_method.empty())
			{
				ZONETOOL_WARNING("CAST %s: mesh \"%s\" uses \"%s\" skinning, the engine only supports linear blend "
					"skinning and the weights are used as is",
					owner.data(), mesh.name.data(), skinning_method.data());
				mesh.quaternion_skinning = true;
			}

			read_mesh_influences(mesh_node, mesh, owner);
			mesh.material = resolve_material_name(mesh_node, model_node, source, owner);
		}

		void read_curve(const node& curve_node, curve_t& curve, const basis_converter& basis,
			const std::string& owner)
		{
			curve.node_name = curve_node.get_string("nn");
			curve.property = curve_node.get_string("kp");
			curve.mode = parse_curve_mode(curve_node.get_string("m"), owner);
			curve.additive_weight = curve_node.get_float("ab", 1.0f);

			if (curve.node_name.empty() || curve.property.empty())
			{
				fail(utils::string::va("CAST %s: a curve is missing its node name or key property name",
					owner.data()));
			}

			const auto* frames = curve_node.get_property("kb");
			const auto* values = curve_node.get_property("kv");
			if (!frames || !values)
			{
				fail(utils::string::va("CAST %s: curve \"%s\" of node \"%s\" is missing its key buffers",
					owner.data(), curve.property.data(), curve.node_name.data()));
			}

			if (!frames->is_integer())
			{
				fail(utils::string::va("CAST %s: curve \"%s\" of node \"%s\" has a non integer key frame buffer",
					owner.data(), curve.property.data(), curve.node_name.data()));
			}

			curve.components = values->components();
			if (values->element_count != frames->value_count())
			{
				fail(utils::string::va(
					"CAST %s: curve \"%s\" of node \"%s\" has %llu key frames but %llu key values",
					owner.data(), curve.property.data(), curve.node_name.data(),
					frames->value_count(), values->element_count));
			}

			curve.key_frames.resize(frames->value_count());
			for (std::size_t i = 0; i < curve.key_frames.size(); i++)
			{
				curve.key_frames[i] = static_cast<std::uint32_t>(frames->as_integer(i));
			}

			curve.key_values.resize(values->value_count());
			for (std::size_t i = 0; i < curve.key_values.size(); i++)
			{
				curve.key_values[i] = values->as_float(i);
			}

			const auto is_translation = curve.property == "tx" || curve.property == "ty" ||
				curve.property == "tz";
			if (is_translation && basis.scale() != 1.0f)
			{
				for (auto& value : curve.key_values)
				{
					value *= basis.scale();
				}
			}
		}

		void read_animation(const node& animation_node, animation_t& animation, const basis_converter& basis,
			const std::string& display_name)
		{
			animation.name = animation_node.get_string("n");
			animation.framerate = animation_node.get_float("fr", 0.0f);
			animation.looping = animation_node.get_bool("lo", false);

			const auto owner = utils::string::va("%s (animation \"%s\")", display_name.data(),
				animation.name.empty() ? "<unnamed>" : animation.name.data());

			if (animation.framerate <= 0.0f)
			{
				fail(utils::string::va("CAST %s: the animation does not declare a valid framerate", owner));
			}

			animation.bones = read_skeleton(animation_node.first_child_of_type(id_skeleton), basis);

			if (basis.active())
			{
				ZONETOOL_WARNING("CAST %s: the scene is y up, the translation channels of an animation are not "
					"rotated into engine space and will be wrong", owner);
			}

			for (const auto* curve_node : animation_node.children_of_type(id_curve))
			{
				curve_t curve{};
				read_curve(*curve_node, curve, basis, owner);
				animation.curves.emplace_back(std::move(curve));
			}

			for (const auto* override_node : animation_node.children_of_type(id_curve_mode_override))
			{
				curve_mode_override_t entry{};
				entry.node_name = override_node->get_string("nn");
				entry.mode = parse_curve_mode(override_node->get_string("m"), owner);
				entry.override_translation = override_node->get_bool("ot", false);
				entry.override_rotation = override_node->get_bool("or", false);
				entry.override_scale = override_node->get_bool("os", false);

				if (!entry.node_name.empty())
				{
					animation.overrides.emplace_back(std::move(entry));
				}
			}

			for (const auto* notification_node : animation_node.children_of_type(id_notification_track))
			{
				notification_t notification{};
				notification.name = notification_node->get_string("n");

				const auto* frames = notification_node->get_property("kb");
				if (notification.name.empty() || !frames || !frames->is_integer())
				{
					ZONETOOL_WARNING("CAST %s: skipping a malformed notification track", owner);
					continue;
				}

				notification.frames.resize(frames->value_count());
				for (std::size_t i = 0; i < notification.frames.size(); i++)
				{
					notification.frames[i] = static_cast<std::uint32_t>(frames->as_integer(i));
				}

				animation.notifications.emplace_back(std::move(notification));
			}
		}
	}

	namespace
	{
		bool is_engine_order(const std::vector<bone_t>& bones)
		{
			auto past_roots = false;
			for (std::size_t i = 0; i < bones.size(); i++)
			{
				const auto parent = bones[i].parent_index;
				if (parent < 0)
				{
					if (past_roots)
					{
						return false;
					}

					continue;
				}

				past_roots = true;
				if (parent >= static_cast<int>(i))
				{
					return false;
				}
			}

			return true;
		}

		std::vector<std::uint32_t> engine_bone_order(const std::vector<bone_t>& bones, const std::string& owner)
		{
			const auto bone_count = bones.size();
			std::vector<std::uint32_t> order;
			order.reserve(bone_count);

			if (is_engine_order(bones))
			{
				order.resize(bone_count);
				std::iota(order.begin(), order.end(), 0u);
				return order;
			}


			std::vector<bool> emitted(bone_count, false);
			for (std::size_t i = 0; i < bone_count; i++)
			{
				if (bones[i].parent_index < 0)
				{
					order.emplace_back(static_cast<std::uint32_t>(i));
					emitted[i] = true;
				}
			}

			if (order.empty())
			{
				fail(utils::string::va("CAST %s: the skeleton does not contain a root bone, every one of its %llu bones "
					"declares a parent so the hierarchy is cyclic", owner.data(), bone_count));
			}

			for (std::size_t head = 0; head < order.size(); head++)
			{
				const auto parent = order[head];
				for (std::size_t i = 0; i < bone_count; i++)
				{
					if (!emitted[i] && bones[i].parent_index == static_cast<int>(parent))
					{
						order.emplace_back(static_cast<std::uint32_t>(i));
						emitted[i] = true;
					}
				}
			}

			if (order.size() != bone_count)
			{
				for (std::size_t i = 0; i < bone_count; i++)
				{
					if (!emitted[i])
					{
						fail(utils::string::va("CAST %s: bone \"%s\" is part of a cyclic or unreachable hierarchy",
							owner.data(), bones[i].name.data()));
					}
				}
			}

			return order;
		}

		void adopt_stray_roots(std::vector<bone_t>& bones, const std::string& owner)
		{
			const auto& root_name = get_import_root();
			if (root_name.empty())
			{
				return;
			}

			auto root_index = -1;
			for (std::size_t i = 0; i < bones.size(); i++)
			{
				if (bones[i].name == root_name)
				{
					root_index = static_cast<int>(i);
					break;
				}
			}

			if (root_index < 0)
			{
				ZONETOOL_WARNING("CAST %s: the configured root bone \"%s\" does not exist in this skeleton, "
					"the hierarchy is left alone", owner.data(), root_name.data());
				return;
			}

			if (bones[root_index].parent_index >= 0)
			{
				ZONETOOL_WARNING("CAST %s: the configured root bone \"%s\" is a child of \"%s\" and not a root "
					"itself, the hierarchy is left alone", owner.data(), root_name.data(),
					bones[bones[root_index].parent_index].name.data());
				return;
			}

			std::size_t adopted = 0;
			for (std::size_t i = 0; i < bones.size(); i++)
			{
				auto& bone = bones[i];
				if (static_cast<int>(i) == root_index || bone.parent_index >= 0)
				{
					continue;
				}

				if (!bone.has_world)
				{
					bone.world_position = bone.local_position;
					bone.world_rotation = bone.local_rotation;
					bone.has_world = true;
				}
				bone.has_local = false;

				bone.parent_index = root_index;
				adopted++;
			}

			if (adopted)
			{
				ZONETOOL_INFO("CAST %s: adopted %llu stray root bones under \"%s\"",
					owner.data(), adopted, root_name.data());
			}
		}
	}

	float quat_dot(const quat_t& a, const quat_t& b)
	{
		return a.v[0] * b.v[0] + a.v[1] * b.v[1] + a.v[2] * b.v[2] + a.v[3] * b.v[3];
	}

	quat_t quat_normalize(const quat_t& q)
	{
		const auto length_squared = quat_dot(q, q);
		if (length_squared <= std::numeric_limits<float>::epsilon())
		{
			return quat_t{};
		}

		const auto inverse = 1.0f / std::sqrt(length_squared);
		return quat_t{{q.v[0] * inverse, q.v[1] * inverse, q.v[2] * inverse, q.v[3] * inverse}};
	}

	quat_t quat_conjugate(const quat_t& q)
	{
		return quat_t{{-q.v[0], -q.v[1], -q.v[2], q.v[3]}};
	}

	quat_t quat_multiply(const quat_t& a, const quat_t& b)
	{
		const auto ax = a.v[0], ay = a.v[1], az = a.v[2], aw = a.v[3];
		const auto bx = b.v[0], by = b.v[1], bz = b.v[2], bw = b.v[3];

		return quat_t{{
			aw * bx + ax * bw + ay * bz - az * by,
			aw * by - ax * bz + ay * bw + az * bx,
			aw * bz + ax * by - ay * bx + az * bw,
			aw * bw - ax * bx - ay * by - az * bz,
		}};
	}

	vec3_t quat_rotate(const quat_t& q, const vec3_t& v)
	{
		const auto x = q.v[0], y = q.v[1], z = q.v[2], w = q.v[3];

		const float t[3]{
			y * v.v[2] - z * v.v[1] + w * v.v[0],
			z * v.v[0] - x * v.v[2] + w * v.v[1],
			x * v.v[1] - y * v.v[0] + w * v.v[2],
		};

		vec3_t out{};
		out.v[0] = v.v[0] + 2.0f * (y * t[2] - z * t[1]);
		out.v[1] = v.v[1] + 2.0f * (z * t[0] - x * t[2]);
		out.v[2] = v.v[2] + 2.0f * (x * t[1] - y * t[0]);
		return out;
	}


	quat_t quat_slerp(const quat_t& a, const quat_t& b, const float t)
	{
		auto target = b;
		auto dot = quat_dot(a, b);

		if (dot < 0.0f)
		{
			dot = -dot;
			target.v[0] = -target.v[0];
			target.v[1] = -target.v[1];
			target.v[2] = -target.v[2];
			target.v[3] = -target.v[3];
		}

		float scale0, scale1;
		if (dot > 0.9995f)
		{
			scale0 = 1.0f - t;
			scale1 = t;
		}
		else
		{
			const auto angle = std::acos(std::clamp(dot, -1.0f, 1.0f));
			const auto sin_angle = std::sin(angle);
			scale0 = std::sin((1.0f - t) * angle) / sin_angle;
			scale1 = std::sin(t * angle) / sin_angle;
		}

		return quat_normalize(quat_t{{
			a.v[0] * scale0 + target.v[0] * scale1,
			a.v[1] * scale0 + target.v[1] * scale1,
			a.v[2] * scale0 + target.v[2] * scale1,
			a.v[3] * scale0 + target.v[3] * scale1,
		}});
	}

	std::vector<std::uint32_t> normalize_skeleton(std::vector<bone_t>& bones, const std::string& owner)
	{
		const auto bone_count = bones.size();
		std::vector<std::uint32_t> remap(bone_count, 0);

		if (bones.empty())
		{
			return remap;
		}

		for (std::size_t i = 0; i < bone_count; i++)
		{
			auto& bone = bones[i];
			if (bone.name.empty())
			{
				fail(utils::string::va("CAST %s: bone %llu does not have a name", owner.data(), i));
			}

			if (bone.parent_index >= static_cast<int>(bone_count))
			{
				fail(utils::string::va("CAST %s: bone \"%s\" references parent index %d, but the skeleton only "
					"contains %llu bones", owner.data(), bone.name.data(), bone.parent_index, bone_count));
			}

			if (bone.parent_index == static_cast<int>(i))
			{
				fail(utils::string::va("CAST %s: bone \"%s\" is its own parent", owner.data(), bone.name.data()));
			}

			if (bone.parent_index < -1)
			{
				fail(utils::string::va("CAST %s: bone \"%s\" has an invalid parent index %d",
					owner.data(), bone.name.data(), bone.parent_index));
			}
		}

		adopt_stray_roots(bones, owner);

		const auto order = engine_bone_order(bones, owner);

		for (std::size_t i = 0; i < bone_count; i++)
		{
			remap[order[i]] = static_cast<std::uint32_t>(i);
		}

		std::vector<bone_t> sorted;
		sorted.reserve(bone_count);
		for (const auto index : order)
		{
			auto bone = bones[index];
			if (bone.parent_index >= 0)
			{
				bone.parent_index = static_cast<int>(remap[bone.parent_index]);
			}
			sorted.emplace_back(std::move(bone));
		}
		bones = std::move(sorted);

		for (std::size_t i = 0; i < bone_count; i++)
		{
			auto& bone = bones[i];

			if (!bone.has_world && !bone.has_local)
			{
				ZONETOOL_WARNING("CAST %s: bone \"%s\" does not provide any transform, using identity",
					owner.data(), bone.name.data());
				bone.has_local = true;
				bone.has_world = true;
			}

			if (bone.parent_index < 0)
			{
				if (!bone.has_local)
				{
					bone.local_position = bone.world_position;
					bone.local_rotation = bone.world_rotation;
				}
				else if (!bone.has_world)
				{
					bone.world_position = bone.local_position;
					bone.world_rotation = bone.local_rotation;
				}
			}
			else
			{
				const auto& parent = bones[bone.parent_index];

				if (!bone.has_world)
				{
					bone.world_rotation = quat_normalize(quat_multiply(parent.world_rotation, bone.local_rotation));

					const auto rotated = quat_rotate(parent.world_rotation, bone.local_position);
					for (auto axis = 0; axis < 3; axis++)
					{
						bone.world_position.v[axis] = parent.world_position.v[axis] + rotated.v[axis];
					}
				}
				else if (!bone.has_local)
				{
					const auto inverse_parent = quat_conjugate(parent.world_rotation);
					bone.local_rotation = quat_normalize(quat_multiply(inverse_parent, bone.world_rotation));

					vec3_t delta{};
					for (auto axis = 0; axis < 3; axis++)
					{
						delta.v[axis] = bone.world_position.v[axis] - parent.world_position.v[axis];
					}
					bone.local_position = quat_rotate(inverse_parent, delta);
				}
			}

			bone.local_rotation = quat_normalize(bone.local_rotation);
			bone.world_rotation = quat_normalize(bone.world_rotation);
			bone.has_local = true;
			bone.has_world = true;
		}

		return remap;
	}

	scene_t build_scene(const file& source, const std::string& display_name)
	{
		scene_t scene{};
		scene.display_name = display_name;
		scene.up_axis = get_up_axis(source);

		const basis_converter basis(scene.up_axis);
		if (basis.scale() != 1.0f)
		{
			ZONETOOL_INFO("CAST %s: importing at a scale of %g", display_name.data(), basis.scale());
		}

		if (basis.active())
		{
			ZONETOOL_INFO("CAST %s: scene is y up, rotating it into engine space", display_name.data());
		}
		else if (scene.up_axis != "z" && scene.up_axis != "Z")
		{
			ZONETOOL_WARNING("CAST %s: unsupported up axis \"%s\", the scene is used as is",
				display_name.data(), scene.up_axis.data());
		}

		for (const auto* model_node : source.all_of_type(id_model))
		{
			model_t model{};
			model.name = model_node->get_string("n");

			if (const auto* distance = model_node->get_property("ld"))
			{
				model.lod_distance = distance->as_float(0);
				model.has_lod_distance = model.lod_distance > 0.0f;
			}
			model.bones = read_skeleton(model_node->first_child_of_type(id_skeleton), basis);

			for (const auto* mesh_node : model_node->children_of_type(id_mesh))
			{
				mesh_t mesh{};
				read_mesh(*mesh_node, *model_node, source, mesh, basis, display_name);
				model.meshes.emplace_back(std::move(mesh));
			}

			const auto hair = model_node->children_of_type(id_hair);
			if (!hair.empty())
			{
				ZONETOOL_WARNING("CAST %s: model \"%s\" contains %llu hair definitions which cannot be converted",
					display_name.data(), model.name.data(), hair.size());
			}

			const auto blend_shapes = model_node->children_of_type(id_blend_shape);
			if (!blend_shapes.empty())
			{
				ZONETOOL_WARNING("CAST %s: model \"%s\" contains %llu blend shapes which are not converted",
					display_name.data(), model.name.data(), blend_shapes.size());
			}

			scene.models.emplace_back(std::move(model));
		}

		for (const auto* animation_node : source.all_of_type(id_animation))
		{
			animation_t animation{};
			read_animation(*animation_node, animation, basis, display_name);
			scene.animations.emplace_back(std::move(animation));
		}

		return scene;
	}
}
