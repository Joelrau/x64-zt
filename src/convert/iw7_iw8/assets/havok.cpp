#include <std_include.hpp>

#include "../include.hpp"
#include "havok.hpp"
#include "havok_schema.hpp"

#include <formats/havok_tagfile.hpp>
#include <iw7/common/havok.hpp>

namespace zonetool::iw7::converter::iw8::havok
{
	namespace
	{
		using tag_writer = ::havok::tagfile::writer;
		using tag_type = ::havok::tagfile::type_info;
		using tag_member = ::havok::tagfile::type_member;

		struct vec3
		{
			float x;
			float y;
			float z;
		};

		struct bounds
		{
			float min[3];
			float max[3];
		};

		const tag_type& type_info(const std::uint32_t type)
		{
			return schema().at(type);
		}

		const tag_member* find_member(std::uint32_t type, const std::string_view name)
		{
			while (type)
			{
				for (const auto& member : type_info(type).members)
				{
					if (member.name == name)
					{
						return &member;
					}
				}

				type = type_info(type).parent;
			}

			return nullptr;
		}

		const tag_member& member(const std::uint32_t type, const std::string_view name)
		{
			const auto* found = find_member(type, name);
			if (!found)
			{
				throw std::runtime_error(std::format("havok schema: type {} has no member {}", type, name));
			}

			return *found;
		}

		template <typename T>
		void put(std::vector<std::byte>& object, const std::size_t offset, const T& value)
		{
			if (offset + sizeof(T) > object.size())
			{
				throw std::runtime_error("havok schema: field is outside the object");
			}

			std::memcpy(object.data() + offset, &value, sizeof(T));
		}

		template <typename T>
		void put_member(std::vector<std::byte>& object, const std::uint32_t type, const std::string_view name, const T& value)
		{
			put(object, member(type, name).offset, value);
		}

		vec3 to_vec3(const float* values)
		{
			return {values[0], values[1], values[2]};
		}

		vec3 operator-(const vec3& a, const vec3& b)
		{
			return {a.x - b.x, a.y - b.y, a.z - b.z};
		}

		vec3 operator+(const vec3& a, const vec3& b)
		{
			return {a.x + b.x, a.y + b.y, a.z + b.z};
		}

		vec3 operator*(const vec3& a, const float b)
		{
			return {a.x * b, a.y * b, a.z * b};
		}

		float dot(const vec3& a, const vec3& b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		vec3 cross(const vec3& a, const vec3& b)
		{
			return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
		}

		float length(const vec3& a)
		{
			return std::sqrt(dot(a, a));
		}

		vec3 normalize(const vec3& a)
		{
			const auto len = length(a);
			return len > 0.0f ? a * (1.0f / len) : vec3{0.0f, 0.0f, 0.0f};
		}

		bounds empty_bounds()
		{
			bounds result{};
			for (auto axis = 0; axis < 3; axis++)
			{
				result.min[axis] = std::numeric_limits<float>::max();
				result.max[axis] = std::numeric_limits<float>::lowest();
			}

			return result;
		}

		void include(bounds& box, const vec3& point)
		{
			for (auto axis = 0; axis < 3; axis++)
			{
				const auto value = axis == 0 ? point.x : axis == 1 ? point.y : point.z;
				box.min[axis] = std::min(box.min[axis], value);
				box.max[axis] = std::max(box.max[axis], value);
			}
		}

		vec3 center_of(const bounds& box)
		{
			return {(box.min[0] + box.max[0]) * 0.5f, (box.min[1] + box.max[1]) * 0.5f, (box.min[2] + box.max[2]) * 0.5f};
		}

		struct hull_face
		{
			std::vector<std::uint32_t> vertices;
			vec3 normal;
			float distance;
		};

		bool same_point(const vec3& a, const vec3& b)
		{
			return a.x == b.x && a.y == b.y && a.z == b.z;
		}

		std::vector<vec3> unique_points(const std::vector<vec3>& points)
		{
			std::vector<vec3> result;
			for (const auto& point : points)
			{
				if (std::ranges::find_if(result, [&](const vec3& other) { return same_point(point, other); }) == result.end())
				{
					result.push_back(point);
				}
			}

			return result;
		}

		bool is_box(const std::vector<vec3>& points, const bounds& box)
		{
			if (points.size() != 8)
			{
				return false;
			}

			for (const auto& point : points)
			{
				for (auto axis = 0; axis < 3; axis++)
				{
					const auto value = axis == 0 ? point.x : axis == 1 ? point.y : point.z;
					if (value != box.min[axis] && value != box.max[axis])
					{
						return false;
					}
				}
			}

			return true;
		}

		struct convex_result
		{
			std::vector<vec3> vertices;
			std::vector<hull_face> faces;
		};

		convex_result box_convex(const bounds& box)
		{
			const std::array<vec3, 8> vertices{{
				{box.max[0], box.max[1], box.max[2]},
				{box.min[0], box.max[1], box.max[2]},
				{box.max[0], box.min[1], box.max[2]},
				{box.min[0], box.min[1], box.max[2]},
				{box.max[0], box.max[1], box.min[2]},
				{box.min[0], box.max[1], box.min[2]},
				{box.max[0], box.min[1], box.min[2]},
				{box.min[0], box.min[1], box.min[2]},
			}};

			const std::array<std::array<std::uint32_t, 4>, 6> faces{{
				{2, 6, 4, 0},
				{1, 5, 7, 3},
				{1, 0, 4, 5},
				{7, 6, 2, 3},
				{3, 2, 0, 1},
				{7, 5, 4, 6},
			}};

			const std::array<vec3, 6> normals{{
				{1.0f, 0.0f, 0.0f},
				{-1.0f, 0.0f, 0.0f},
				{0.0f, 1.0f, 0.0f},
				{0.0f, -1.0f, 0.0f},
				{0.0f, 0.0f, 1.0f},
				{0.0f, 0.0f, -1.0f},
			}};

			convex_result result;
			result.vertices.assign(vertices.begin(), vertices.end());
			for (auto i = 0u; i < faces.size(); i++)
			{
				hull_face face{};
				face.vertices.assign(faces[i].begin(), faces[i].end());
				face.normal = normals[i];
				face.distance = -dot(normals[i], vertices[faces[i][0]]);
				result.faces.push_back(std::move(face));
			}

			return result;
		}

		struct triangle
		{
			std::array<std::uint32_t, 3> vertices;
			vec3 normal;
			float distance;
			bool alive;
		};

		std::vector<hull_face> quick_hull(const std::vector<vec3>& points)
		{
			if (points.size() < 4)
			{
				throw std::runtime_error("havok: a convex piece has fewer than four points");
			}

			auto box = empty_bounds();
			for (const auto& point : points)
			{
				include(box, point);
			}

			const auto extent = std::max({box.max[0] - box.min[0], box.max[1] - box.min[1], box.max[2] - box.min[2]});
			const auto epsilon = std::max(1e-4f, extent * 1e-5f);

			std::size_t first{}, second{};
			auto best = -1.0f;
			for (auto i = 0u; i < points.size(); i++)
			{
				for (auto j = i + 1; j < points.size(); j++)
				{
					const auto distance = length(points[i] - points[j]);
					if (distance > best)
					{
						best = distance;
						first = i;
						second = j;
					}
				}
			}

			std::size_t third{};
			best = -1.0f;
			const auto line = points[second] - points[first];
			for (auto i = 0u; i < points.size(); i++)
			{
				const auto distance = length(cross(line, points[i] - points[first])) / std::max(length(line), 1e-6f);
				if (distance > best)
				{
					best = distance;
					third = i;
				}
			}

			if (best <= epsilon)
			{
				throw std::runtime_error("havok: a convex piece is flat");
			}

			std::size_t fourth{};
			best = -1.0f;
			const auto base_normal = normalize(cross(points[second] - points[first], points[third] - points[first]));
			for (auto i = 0u; i < points.size(); i++)
			{
				const auto distance = std::abs(dot(base_normal, points[i] - points[first]));
				if (distance > best)
				{
					best = distance;
					fourth = i;
				}
			}

			if (best <= epsilon)
			{
				throw std::runtime_error("havok: a convex piece is flat");
			}

			const std::array<std::uint32_t, 4> tetra{{static_cast<std::uint32_t>(first), static_cast<std::uint32_t>(second),
				static_cast<std::uint32_t>(third), static_cast<std::uint32_t>(fourth)}};
			const auto interior = (points[tetra[0]] + points[tetra[1]] + points[tetra[2]] + points[tetra[3]]) * 0.25f;

			std::vector<triangle> faces;
			const auto add_face = [&](const std::uint32_t a, const std::uint32_t b, const std::uint32_t c)
			{
				auto normal = normalize(cross(points[b] - points[a], points[c] - points[a]));
				auto distance = dot(normal, points[a]);
				if (dot(normal, interior) - distance > 0.0f)
				{
					normal = normal * -1.0f;
					distance = -distance;
					faces.push_back({{a, c, b}, normal, distance, true});
					return;
				}

				faces.push_back({{a, b, c}, normal, distance, true});
			};

			add_face(tetra[0], tetra[1], tetra[2]);
			add_face(tetra[0], tetra[1], tetra[3]);
			add_face(tetra[0], tetra[2], tetra[3]);
			add_face(tetra[1], tetra[2], tetra[3]);

			for (auto i = 0u; i < points.size(); i++)
			{
				if (i == tetra[0] || i == tetra[1] || i == tetra[2] || i == tetra[3])
				{
					continue;
				}

				const auto& point = points[i];
				std::vector<std::pair<std::uint32_t, std::uint32_t>> horizon;
				for (auto& face : faces)
				{
					if (!face.alive || dot(face.normal, point) - face.distance <= epsilon)
					{
						continue;
					}

					face.alive = false;
					for (auto edge = 0; edge < 3; edge++)
					{
						const auto a = face.vertices[edge];
						const auto b = face.vertices[(edge + 1) % 3];
						const auto reverse = std::ranges::find(horizon, std::pair{b, a});
						if (reverse != horizon.end())
						{
							horizon.erase(reverse);
						}
						else
						{
							horizon.emplace_back(a, b);
						}
					}
				}

				for (const auto& [a, b] : horizon)
				{
					add_face(a, b, static_cast<std::uint32_t>(i));
				}
			}

			std::vector<hull_face> result;
			for (const auto& face : faces)
			{
				if (!face.alive)
				{
					continue;
				}

				result.push_back({{face.vertices[0], face.vertices[1], face.vertices[2]}, face.normal, face.distance});
			}

			return result;
		}

		std::vector<hull_face> merge_faces(const std::vector<hull_face>& triangles, const std::vector<vec3>& points)
		{
			struct group
			{
				std::vector<std::uint32_t> vertices;
				vec3 normal;
				float distance;
			};

			std::vector<group> groups;
			for (const auto& face : triangles)
			{
				auto found = groups.end();
				for (auto it = groups.begin(); it != groups.end(); ++it)
				{
					if (dot(it->normal, face.normal) > 0.9999f && std::abs(it->distance - face.distance) < 1e-3f)
					{
						found = it;
						break;
					}
				}

				if (found == groups.end())
				{
					groups.push_back({{}, face.normal, face.distance});
					found = groups.end() - 1;
				}

				for (const auto vertex : face.vertices)
				{
					if (std::ranges::find(found->vertices, vertex) == found->vertices.end())
					{
						found->vertices.push_back(vertex);
					}
				}
			}

			std::vector<hull_face> result;
			for (auto& group : groups)
			{
				if (group.vertices.size() < 3)
				{
					continue;
				}

				auto centroid = vec3{0.0f, 0.0f, 0.0f};
				for (const auto vertex : group.vertices)
				{
					centroid = centroid + points[vertex];
				}
				centroid = centroid * (1.0f / static_cast<float>(group.vertices.size()));

				const auto normal = normalize(group.normal);
				auto tangent = cross(normal, std::abs(normal.z) < 0.9f ? vec3{0.0f, 0.0f, 1.0f} : vec3{1.0f, 0.0f, 0.0f});
				tangent = normalize(tangent);
				const auto bitangent = cross(normal, tangent);

				struct projected_point
				{
					float x;
					float y;
					std::uint32_t index;
				};

				std::vector<projected_point> projected;
				projected.reserve(group.vertices.size());
				for (const auto vertex : group.vertices)
				{
					const auto direction = points[vertex] - centroid;
					projected.push_back({dot(direction, tangent), dot(direction, bitangent), vertex});
				}

				std::ranges::sort(projected, [](const projected_point& a, const projected_point& b)
				{
					return a.x != b.x ? a.x < b.x : a.y < b.y;
				});

				const auto cross_2d = [](const projected_point& a, const projected_point& b, const projected_point& c)
				{
					return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
				};

				std::vector<projected_point> lower;
				std::vector<projected_point> upper;
				for (const auto& point : projected)
				{
					while (lower.size() >= 2 && cross_2d(lower[lower.size() - 2], lower.back(), point) <= 0.0f)
					{
						lower.pop_back();
					}
					lower.push_back(point);
				}
				for (auto it = projected.rbegin(); it != projected.rend(); ++it)
				{
					while (upper.size() >= 2 && cross_2d(upper[upper.size() - 2], upper.back(), *it) <= 0.0f)
					{
						upper.pop_back();
					}
					upper.push_back(*it);
				}

				lower.pop_back();
				upper.pop_back();
				lower.insert(lower.end(), upper.begin(), upper.end());
				group.vertices.clear();
				for (const auto& point : lower)
				{
					group.vertices.push_back(point.index);
				}

				if (group.vertices.size() < 3)
				{
					continue;
				}

				if (dot(cross(points[group.vertices[1]] - points[group.vertices[0]],
					points[group.vertices[2]] - points[group.vertices[0]]), normal) < 0.0f)
				{
					std::ranges::reverse(group.vertices);
				}

				hull_face face{};
				face.vertices = std::move(group.vertices);
				face.normal = normal;
				face.distance = -dot(normal, centroid);
				result.push_back(std::move(face));
			}

			return result;
		}

		bool is_manifold(const std::vector<hull_face>& faces)
		{
			for (const auto& face : faces)
			{
				for (auto i = 0u; i < face.vertices.size(); i++)
				{
					const auto a = face.vertices[i];
					const auto b = face.vertices[(i + 1) % face.vertices.size()];
					auto found = false;
					for (const auto& other : faces)
					{
						for (auto j = 0u; j < other.vertices.size(); j++)
						{
							if (other.vertices[j] == b && other.vertices[(j + 1) % other.vertices.size()] == a)
							{
								found = true;
								break;
							}
						}
						if (found)
						{
							break;
						}
					}

					if (!found)
					{
						return false;
					}
				}
			}

			return true;
		}

		convex_result make_convex_hull(const std::vector<vec3>& source_points)
		{
			const auto points = unique_points(source_points);
			if (points.size() < 4)
			{
				throw std::runtime_error("havok: a convex piece has fewer than four points");
			}

			auto box = empty_bounds();
			for (const auto& point : points)
			{
				include(box, point);
			}

			std::vector<hull_face> faces;
			if (is_box(points, box))
			{
				return box_convex(box);
			}

			try
			{
				faces = merge_faces(quick_hull(points), points);
			}
			catch (const std::exception&)
			{
				const auto padding = std::max(1e-3f, std::max({box.max[0] - box.min[0], box.max[1] - box.min[1],
					box.max[2] - box.min[2]}) * 1e-3f);
				for (auto axis = 0; axis < 3; axis++)
				{
					box.min[axis] -= padding;
					box.max[axis] += padding;
				}
				return box_convex(box);
			}

			if (!is_manifold(faces))
			{
				return box_convex(box);
			}

			std::vector<std::uint32_t> used;
			for (const auto& face : faces)
			{
				for (const auto vertex : face.vertices)
				{
					if (std::ranges::find(used, vertex) == used.end())
					{
						used.push_back(vertex);
					}
				}
			}

			convex_result result;
			result.vertices.reserve(used.size());
			for (const auto vertex : used)
			{
				result.vertices.push_back(points[vertex]);
			}

			for (auto& face : faces)
			{
				for (auto& vertex : face.vertices)
				{
					vertex = static_cast<std::uint32_t>(std::ranges::find(used, vertex) - used.begin());
				}
				result.faces.push_back(std::move(face));
			}

			return result;
		}

		struct connectivity_edge
		{
			std::uint16_t face;
			std::uint8_t index;
		};

		void add_connectivity(const convex_result& hull, std::vector<connectivity_edge>& vertex_edges,
			std::vector<connectivity_edge>& face_links)
		{
			vertex_edges.assign(hull.vertices.size(), connectivity_edge{0xFFFF, 0});
			for (auto v = 0u; v < hull.vertices.size(); v++)
			{
				for (auto f = 0u; f < hull.faces.size(); f++)
				{
					const auto& face = hull.faces[f];
					for (auto i = 0u; i < face.vertices.size(); i++)
					{
						if (face.vertices[i] == v)
						{
							vertex_edges[v] = {static_cast<std::uint16_t>(f), static_cast<std::uint8_t>(i)};
							goto next_vertex;
						}
					}
				}
			next_vertex:;
			}

			std::size_t total = 0;
			for (const auto& face : hull.faces)
			{
				total += face.vertices.size();
			}
			face_links.assign(total, connectivity_edge{0xFFFF, 0});

			std::size_t base = 0;
			for (auto f = 0u; f < hull.faces.size(); f++)
			{
				const auto& face = hull.faces[f];
				for (auto i = 0u; i < face.vertices.size(); i++)
				{
					const auto a = face.vertices[i];
					const auto b = face.vertices[(i + 1) % face.vertices.size()];
					for (auto g = 0u; g < hull.faces.size(); g++)
					{
						const auto& other = hull.faces[g];
						for (auto j = 0u; j < other.vertices.size(); j++)
						{
							if (other.vertices[j] == b && other.vertices[(j + 1) % other.vertices.size()] == a)
							{
								face_links[base + i] = {static_cast<std::uint16_t>(g), static_cast<std::uint8_t>(j)};
							}
						}
					}
				}

				base += face.vertices.size();
			}

		}
	}

	namespace
	{
		struct source_section
		{
			std::array<float, 6> codec{};
			bounds domain{};
			std::uint32_t first_packed{};
			std::uint8_t packed_count{};
			std::uint32_t first_shared{};
			std::uint32_t shared_count{};
			std::uint32_t first_primitive{};
			std::uint8_t primitive_count{};
			std::uint32_t first_run{};
			std::uint8_t run_count{};
			std::uint16_t leaf_index{};
			std::uint8_t page{};
			std::uint8_t layer_data{};
			std::uint8_t flags{};
			std::vector<std::array<std::uint8_t, 4>> nodes;
			std::vector<std::array<std::uint8_t, 4>> primitives;
			std::vector<iw7::hknpCompressedMeshShapeTreeDataRun> runs;
			std::vector<std::uint16_t> tags;
			std::vector<std::int32_t> convex_of_primitive;
			std::vector<bool> convex_slots;
		};

		struct output_section
		{
			std::array<float, 6> codec{};
			bounds domain{};
			std::uint16_t leaf_index{};
			std::uint8_t page{};
			std::uint8_t layer_data{};
			std::uint8_t flags{};
			std::uint32_t first_packed{};
			std::uint8_t packed_count{};
			std::vector<std::array<std::uint8_t, 4>> nodes;
			std::vector<std::array<std::uint8_t, 4>> primitives;
			std::vector<iw7::hknpCompressedMeshShapeTreeDataRun> runs;
			std::vector<std::uint16_t> table;
			std::uint32_t first_shared{};
			std::uint32_t first_primitive{};
			std::uint32_t first_run{};
		};

		struct top_node
		{
			bool leaf;
			std::uint32_t value;
		};

		std::uint32_t split_top(const std::vector<vec3>& centres, std::span<std::uint32_t> items, std::vector<top_node>& nodes)
		{
			const auto index = static_cast<std::uint32_t>(nodes.size());
			nodes.emplace_back();
			if (items.size() == 1)
			{
				nodes[index] = {true, items.front()};
				return 1;
			}

			auto spread = empty_bounds();
			for (const auto item : items)
			{
				include(spread, centres[item]);
			}

			auto axis = 0;
			for (auto a = 1; a < 3; a++)
			{
				if (spread.max[a] - spread.min[a] > spread.max[axis] - spread.min[axis])
				{
					axis = a;
				}
			}

			std::ranges::nth_element(items, items.begin() + items.size() / 2, [&](const std::uint32_t a, const std::uint32_t b)
			{
				const auto va = axis == 0 ? centres[a].x : axis == 1 ? centres[a].y : centres[a].z;
				const auto vb = axis == 0 ? centres[b].x : axis == 1 ? centres[b].y : centres[b].z;
				return va != vb ? va < vb : a < b;
			});

			const auto middle = items.size() / 2;
			const auto left = split_top(centres, items.first(middle), nodes);
			nodes[index].leaf = false;
			nodes[index].value = left;
			return left + split_top(centres, items.subspan(middle), nodes);
		}

		std::array<std::uint8_t, 3> encode_child(const bounds& parent, const bounds& box, bounds& decoded)
		{
			constexpr auto step_factor = 1.0f / 226.0f;
			std::array<std::uint8_t, 3> xyz{};
			for (auto axis = 0; axis < 3; axis++)
			{
				const auto step = (parent.max[axis] - parent.min[axis]) * step_factor;
				auto low = 15;
				while (low > 0 && parent.min[axis] + static_cast<float>(low * low) * step > box.min[axis])
				{
					low--;
				}

				auto high = 15;
				while (high > 0 && parent.max[axis] - static_cast<float>(high * high) * step < box.max[axis])
				{
					high--;
				}

				xyz[axis] = static_cast<std::uint8_t>(low << 4 | high);
				decoded.min[axis] = parent.min[axis] + static_cast<float>(low * low) * step;
				decoded.max[axis] = parent.max[axis] - static_cast<float>(high * high) * step;
			}

			return xyz;
		}

		bounds subtree_bounds(const std::vector<top_node>& nodes, const std::vector<bounds>& section_bounds, const std::uint32_t node)
		{
			if (nodes[node].leaf)
			{
				return section_bounds[nodes[node].value];
			}

			auto result = subtree_bounds(nodes, section_bounds, node + 1);
			const auto& right = subtree_bounds(nodes, section_bounds, node + 2 * nodes[node].value);
			for (auto axis = 0; axis < 3; axis++)
			{
				result.min[axis] = std::min(result.min[axis], right.min[axis]);
				result.max[axis] = std::max(result.max[axis], right.max[axis]);
			}

			return result;
		}

		void encode_top(const std::vector<top_node>& nodes, const std::vector<bounds>& section_bounds,
			const std::uint32_t node, const bounds& parent, std::vector<std::array<std::uint8_t, 5>>& out,
			std::vector<std::uint32_t>& leaves)
		{
			auto decoded = parent;
			const auto xyz = encode_child(parent, subtree_bounds(nodes, section_bounds, node), decoded);
			const auto value = nodes[node].leaf
				? static_cast<std::uint16_t>(nodes[node].value)
				: static_cast<std::uint16_t>(0x8000 | nodes[node].value);
			out[node] = {xyz[0], xyz[1], xyz[2], static_cast<std::uint8_t>(value >> 8), static_cast<std::uint8_t>(value)};
			if (nodes[node].leaf)
			{
				leaves[nodes[node].value] = node;
				return;
			}

			encode_top(nodes, section_bounds, node + 1, decoded, out, leaves);
			encode_top(nodes, section_bounds, node + 2 * nodes[node].value, decoded, out, leaves);
		}

		void encode_section(const std::vector<top_node>& nodes, const std::vector<bounds>& primitive_bounds,
			const std::uint32_t node, const bounds& parent, std::vector<std::array<std::uint8_t, 4>>& out)
		{
			auto decoded = parent;
			const auto xyz = encode_child(parent, subtree_bounds(nodes, primitive_bounds, node), decoded);
			const auto data = nodes[node].leaf ? nodes[node].value << 1 : nodes[node].value << 1 | 1;
			out[node] = {xyz[0], xyz[1], xyz[2], static_cast<std::uint8_t>(data)};
			if (nodes[node].leaf)
			{
				return;
			}

			encode_section(nodes, primitive_bounds, node + 1, decoded, out);
			encode_section(nodes, primitive_bounds, node + 2 * nodes[node].value, decoded, out);
		}

		void build_section_tree(output_section& output, const std::vector<bounds>& primitive_bounds)
		{
			if (primitive_bounds.empty() || primitive_bounds.size() > 128)
			{
				throw std::runtime_error("havok: a section tree needs 1 to 128 primitives");
			}

			output.domain = empty_bounds();
			std::vector<vec3> centres;
			for (const auto& box : primitive_bounds)
			{
				include(output.domain, {box.min[0], box.min[1], box.min[2]});
				include(output.domain, {box.max[0], box.max[1], box.max[2]});
				centres.push_back(center_of(box));
			}

			std::vector<std::uint32_t> items(primitive_bounds.size());
			std::iota(items.begin(), items.end(), 0u);
			std::vector<top_node> nodes;
			split_top(centres, items, nodes);

			output.nodes.resize(nodes.size());
			encode_section(nodes, primitive_bounds, 0, output.domain, output.nodes);
		}

		std::vector<iw7::hknpCompressedMeshShapeTreeDataRun> make_runs(const std::vector<std::uint16_t>& tags)
		{
			std::vector<iw7::hknpCompressedMeshShapeTreeDataRun> runs;
			for (auto p = 0u; p < tags.size(); p++)
			{
				if (tags[p] == 0xFFFF)
				{
					continue;
				}

				if (!runs.empty() && runs.back().value == tags[p] && runs.back().index + runs.back().count == static_cast<int>(p))
				{
					runs.back().count++;
					continue;
				}

				runs.push_back({tags[p], static_cast<unsigned char>(p), 1});
			}

			return runs;
		}

		std::uint32_t convert_contents(const std::uint32_t contents)
		{
			static constexpr std::uint32_t bits[] =
			{
				0x1, 0x2, 0x8, 0x10, 0x20, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000,
				0x10000, 0x20000, 0x40000, 0x100000, 0x400000, 0x800000, 0x1000000, 0x2000000, 0x40000000,
			};

			std::uint32_t result = 0;
			for (const auto bit : bits)
			{
				if (contents & bit)
				{
					result |= bit;
				}
			}

			return result;
		}

		std::uint32_t convert_surface_type(const std::uint32_t type)
		{
			if (type >= 44 && type <= 49)
			{
				return 13;
			}

			return type;
		}

		std::uint32_t convert_surface_flags(const std::uint32_t flags)
		{
			std::uint32_t result = 0;
			if (flags & iw7::SURFACE_FLAG_SKY) result |= zonetool::iw8::SURFACE_FLAG_SKY;
			if (flags & iw7::SURFACE_FLAG_LADDER) result |= zonetool::iw8::SURFACE_FLAG_LADDER;
			if (flags & iw7::SURFACE_FLAG_NOIMPACT) result |= zonetool::iw8::SURFACE_FLAG_NOIMPACT;
			if (flags & iw7::SURFACE_FLAG_NOPENETRATE) result |= zonetool::iw8::SURFACE_FLAG_NOPENETRATE;
			if (flags & iw7::SURFACE_FLAG_STAIRS) result |= zonetool::iw8::SURFACE_FLAG_STAIRS;
			return result;
		}

		std::uint64_t convert_user_data(const std::uint64_t user_data)
		{
			const auto low = static_cast<std::uint32_t>(user_data);
			const auto type = convert_surface_type((low >> 19) & 0x3F);
			const auto glass_piece = user_data & 0x0000FFFF00000000ull;
			return glass_piece | (static_cast<std::uint64_t>(type) << 19) | convert_surface_flags(low);
		}

		std::uint16_t get_glass_piece(const std::uint64_t user_data)
		{
			return static_cast<std::uint16_t>(user_data >> 32);
		}

		std::vector<iw7::HavokPhysicsShapeTagData> convert_shape_tags(const iw7::HavokPhysicsShapeList* list)
		{
			std::vector<iw7::HavokPhysicsShapeTagData> tags(list->shapeTagData.size);
			for (auto i = 0; i < list->shapeTagData.size; i++)
			{
				const auto& source = list->shapeTagData.data[i];
				auto& tag = tags[i];
				tag.collisionFilterInfo = convert_contents(source.collisionFilterInfo);
				tag.materialCRC = source.materialCRC;
				tag.materialId = source.materialId;
				tag.userData = convert_user_data(source.userData);
				if (get_glass_piece(source.userData))
				{
					tag.collisionFilterInfo = zonetool::iw8::PHYSICS_CONTENTS_SOLID | zonetool::iw8::PHYSICS_CONTENTS_GLASS
						| zonetool::iw8::PHYSICS_CONTENTS_SOUNDCLIP | zonetool::iw8::PHYSICS_CONTENTS_SOUNDREFLECT; // TODO: stock mp_backlot2 has no GLASS bit, the piece check (0x1632690) needs it
					tag.userData = (tag.userData & ~(0x3Full << 19)) | (static_cast<std::uint64_t>(zonetool::iw8::SURFACE_TYPE_GLASS_SOLID) << 19);
				}
			}

			return tags;
		}

		const iw7::HavokPhysicsShapeList* read_shape_list(const iw7::havok::binary::packfile_reader& reader)
		{
			if (reader.root_class_name() != "HavokPhysicsShapeList")
			{
				throw std::runtime_error(std::format("havok: root is {}", reader.root_class_name()));
			}

			return reader.root<iw7::HavokPhysicsShapeList>();
		}

		const iw7::hknpCompressedMeshShapeTree& read_world_mesh(const iw7::HavokPhysicsShapeList* list)
		{
			if (!list->shapes.data || list->shapes.size < 1)
			{
				throw std::runtime_error("havok: the world shape list has no shape");
			}

			const auto* shape = static_cast<const iw7::hknpCompressedMeshShape*>(list->shapes.data[0]);
			if (!shape || !shape->data)
			{
				throw std::runtime_error("havok: the world shape has no mesh");
			}

			return shape->data->meshTree;
		}

		vec3 read_shared_vertex(const iw7::hknpCompressedMeshShapeTree& tree, const std::uint32_t index)
		{
			if (index >= static_cast<std::uint32_t>(tree.sharedVertices.size))
			{
				throw std::runtime_error("havok: a shape names a shared vertex that the mesh does not have");
			}

			const auto& min = tree.domain.min;
			const auto& max = tree.domain.max;
			const std::array<float, 3> scale{{
				(max.x - min.x) / static_cast<float>((1u << 21) - 1),
				(max.y - min.y) / static_cast<float>((1u << 21) - 1),
				(max.z - min.z) / static_cast<float>((1u << 22) - 1),
			}};

			const auto word = tree.sharedVertices.data[index];
			return {
				min.x + static_cast<float>(word & 0x1FFFFF) * scale[0],
				min.y + static_cast<float>(word >> 21 & 0x1FFFFF) * scale[1],
				min.z + static_cast<float>(word >> 42) * scale[2],
			};
		}

		bool is_convex_primitive(const std::array<std::uint8_t, 4>& indices)
		{
			return indices[1] == indices[2] && indices[2] == indices[3];
		}

		std::vector<vec3> read_convex_points(const iw7::hknpCompressedMeshShapeTree& tree, const std::uint32_t packed_count,
			const std::uint32_t first_shared, const std::uint8_t page, const std::array<std::uint8_t, 4>& indices)
		{
			if (indices[0] < packed_count)
			{
				throw std::runtime_error("havok: a convex primitive names a packed vertex");
			}

			const auto slot = first_shared + indices[0] - packed_count;
			if (slot + 1 >= static_cast<std::uint32_t>(tree.sharedVerticesIndex.size))
			{
				throw std::runtime_error("havok: a convex primitive names a table entry that the mesh does not have");
			}

			const auto header = tree.sharedVerticesIndex.data[slot];
			const auto type = header & 0xFF;
			const auto format = type & 0x30;
			const auto count = static_cast<std::uint32_t>(header >> 8);
			if ((type & 0xCF) != 2 || format != 0 || count < 4)
			{
				throw std::runtime_error(std::format("havok: unsupported convex primitive type {:#x}", header));
			}

			const auto first = static_cast<std::uint32_t>(page) << 16 | tree.sharedVerticesIndex.data[slot + 1];
			std::vector<vec3> points;
			points.reserve(count);
			for (auto v = 0u; v < count; v++)
			{
				points.push_back(read_shared_vertex(tree, first + v));
			}

			return points;
		}
	}

	std::string convert_world_shape(const std::string_view packfile)
	{
		const iw7::havok::binary::packfile_reader reader(packfile);
		const auto* list = read_shape_list(reader);
		const auto& tree = read_world_mesh(list);
		const auto* shape = static_cast<const iw7::hknpCompressedMeshShape*>(list->shapes.data[0]);
		bounds domain{};
		for (auto axis = 0; axis < 3; axis++)
		{
			domain.min[axis] = (&tree.domain.min.x)[axis];
			domain.max[axis] = (&tree.domain.max.x)[axis];
		}

		const auto shared_vertex = [&](const std::uint32_t index)
		{
			return read_shared_vertex(tree, index);
		};

		std::vector<source_section> sources(tree.sections.size);
		for (auto s = 0u; s < sources.size(); s++)
		{
			const auto& section = tree.sections.data[s];
			auto& source = sources[s];
			for (auto i = 0; i < 6; i++)
			{
				source.codec[i] = section.codecParms[i];
			}
			for (auto axis = 0; axis < 3; axis++)
			{
				source.domain.min[axis] = (&section.domain.min.x)[axis];
				source.domain.max[axis] = (&section.domain.max.x)[axis];
			}
			source.first_packed = section.firstPackedVertex;
			source.packed_count = section.numPackedVertices;
			source.first_shared = section.sharedVertices >> 8;
			source.shared_count = section.numSharedIndices;
			source.first_primitive = section.primitives >> 8;
			source.primitive_count = static_cast<std::uint8_t>(section.primitives & 0xFF);
			source.first_run = section.dataRuns >> 8;
			source.run_count = static_cast<std::uint8_t>(section.dataRuns & 0xFF);
			source.leaf_index = section.leafIndex;
			source.page = section.page;
			source.layer_data = section.layerData;
			source.flags = section.flags;

			source.nodes.resize(section.nodes.size);
			for (auto n = 0; n < section.nodes.size; n++)
			{
				std::memcpy(source.nodes[n].data(), &section.nodes.data[n], 4);
			}

			source.primitives.resize(source.primitive_count);
			for (auto p = 0; p < source.primitive_count; p++)
			{
				std::memcpy(source.primitives[p].data(), &tree.primitives.data[source.first_primitive + p], 4);
			}

			source.runs.assign(tree.primitiveDataRuns.data + source.first_run,
				tree.primitiveDataRuns.data + source.first_run + source.run_count);
			source.tags.assign(source.primitive_count, 0xFFFF);
			for (const auto& run : source.runs)
			{
				for (auto k = run.index; k < run.index + run.count && k < source.primitive_count; k++)
				{
					source.tags[k] = run.value;
				}
			}

			source.convex_of_primitive.assign(source.primitive_count, -1);
		}

		std::vector<convex_result> hulls;
		std::vector<std::uint16_t> hull_tags;
		for (auto& source : sources)
		{
			source.convex_slots.assign(source.shared_count, false);
			for (auto p = 0u; p < source.primitives.size(); p++)
			{
				const auto& indices = source.primitives[p];
				if (!is_convex_primitive(indices))
				{
					continue;
				}

				const auto points = read_convex_points(tree, source.packed_count, source.first_shared, source.page, indices);
				hulls.push_back(make_convex_hull(points));
				hull_tags.push_back(source.tags[p]);
				source.convex_of_primitive[p] = static_cast<std::int32_t>(hulls.size() - 1);
				source.convex_slots[indices[0] - source.packed_count] = true;
				source.convex_slots[indices[0] - source.packed_count + 1] = true;
			}
		}

		const auto primitive_bounds = [&](const source_section& source, const std::uint32_t p) -> bounds
		{
			auto box = empty_bounds();
			if (source.convex_of_primitive[p] >= 0)
			{
				for (const auto& vertex : hulls[source.convex_of_primitive[p]].vertices)
				{
					include(box, vertex);
				}

				return box;
			}

			for (const auto index : source.primitives[p])
			{
				if (index < source.packed_count)
				{
					const auto word = tree.packedVertices.data[source.first_packed + index];
					include(box, {
						source.codec[0] + static_cast<float>(word & 0x7FF) * source.codec[3],
						source.codec[1] + static_cast<float>(word >> 11 & 0x7FF) * source.codec[4],
						source.codec[2] + static_cast<float>(word >> 22) * source.codec[5],
					});
					continue;
				}

				const auto value = tree.sharedVerticesIndex.data[source.first_shared + index - source.packed_count];
				include(box, shared_vertex(static_cast<std::uint32_t>(source.page) << 16 | value));
			}

			return box;
		};

		std::vector<output_section> outputs;
		std::size_t convex_total = 0;
		for (auto& source : sources)
		{
			std::vector<std::uint16_t> regular_values;
			std::vector<std::int32_t> slot_regular(source.shared_count, -1);
			std::vector<std::int32_t> value_regular(0x10000, -1);
			for (auto s = 0u; s < source.shared_count; s++)
			{
				if (s < source.convex_slots.size() && source.convex_slots[s])
				{
					continue;
				}

				const auto value = tree.sharedVerticesIndex.data[source.first_shared + s];
				if (value_regular[value] < 0)
				{
					value_regular[value] = static_cast<std::int32_t>(regular_values.size());
					regular_values.push_back(value);
				}
				slot_regular[s] = value_regular[value];
			}

			auto primitives = source.primitives;
			for (auto p = 0u; p < primitives.size(); p++)
			{
				if (source.convex_of_primitive[p] >= 0)
				{
					continue;
				}

				for (auto i = 0; i < 4; i++)
				{
					if (primitives[p][i] < source.packed_count)
					{
						continue;
					}

					const auto old_slot = static_cast<std::uint32_t>(primitives[p][i] - source.packed_count);
					if (old_slot >= source.shared_count || slot_regular[old_slot] < 0)
					{
						throw std::runtime_error("havok: a primitive names a shared vertex that its section does not have");
					}

					const auto local = static_cast<std::uint32_t>(source.packed_count) + slot_regular[old_slot];
					if (local > 0xFF)
					{
						throw std::runtime_error("havok: a section has more than 256 vertex entries");
					}

					primitives[p][i] = static_cast<std::uint8_t>(local);
				}
			}

			std::vector<std::uint32_t> convex_local;
			for (auto p = 0u; p < source.primitives.size(); p++)
			{
				if (source.convex_of_primitive[p] >= 0)
				{
					convex_local.push_back(p);
				}
			}

			convex_total += convex_local.size();

			const auto fits = source.packed_count + regular_values.size() + 4 * convex_local.size() <= 256;
			if (fits)
			{
				output_section output{};
				output.codec = source.codec;
				output.domain = source.domain;
				output.leaf_index = source.leaf_index;
				output.page = source.page;
				output.layer_data = source.layer_data;
				output.flags = source.flags;
				output.first_packed = source.first_packed;
				output.packed_count = source.packed_count;
				output.nodes = source.nodes;
				output.primitives = primitives;
				output.runs = source.runs;
				output.table = regular_values;

				auto record = regular_values.size();
				for (const auto p : convex_local)
				{
					const auto local = static_cast<std::uint32_t>(source.packed_count) + record;
					if (local > 0xFF)
					{
						throw std::runtime_error("havok: a section has more than 256 vertex entries");
					}

					record += 4;
					const auto hull = source.convex_of_primitive[p];
					output.table.push_back(146);
					output.table.push_back(0);
					output.table.push_back(static_cast<std::uint16_t>(hull));
					output.table.push_back(static_cast<std::uint16_t>(hull >> 16));
					output.primitives[p] = {static_cast<std::uint8_t>(local), static_cast<std::uint8_t>(local), 4,
						static_cast<std::uint8_t>(local)};
				}

				outputs.push_back(std::move(output));
				continue;
			}

			output_section regular{};
			regular.codec = source.codec;
			regular.leaf_index = source.leaf_index;
			regular.page = source.page;
			regular.layer_data = source.layer_data;
			regular.flags = source.flags;
			regular.first_packed = source.first_packed;
			regular.packed_count = source.packed_count;
			regular.table = regular_values;

			std::vector<bounds> regular_bounds;
			std::vector<std::uint16_t> regular_tags;
			for (auto p = 0u; p < primitives.size(); p++)
			{
				if (source.convex_of_primitive[p] >= 0)
				{
					continue;
				}

				regular.primitives.push_back(primitives[p]);
				regular_bounds.push_back(primitive_bounds(source, p));
				regular_tags.push_back(source.tags[p]);
			}

			if (!regular.primitives.empty())
			{
				regular.runs = make_runs(regular_tags);
				build_section_tree(regular, regular_bounds);
				outputs.push_back(std::move(regular));
			}

			for (auto begin = 0u; begin < convex_local.size(); begin += 64)
			{
				const auto end = std::min<std::size_t>(begin + 64, convex_local.size());
				output_section convex{};
				convex.codec = source.codec;
				convex.leaf_index = source.leaf_index;
				convex.page = source.page;
				convex.layer_data = source.layer_data;
				convex.flags = source.flags;
				convex.first_packed = 0;
				convex.packed_count = 0;

				std::vector<bounds> convex_bounds;
				std::vector<std::uint16_t> convex_tags;
				for (auto k = begin; k < end; k++)
				{
					const auto p = convex_local[k];
					const auto local = static_cast<std::uint32_t>((k - begin) * 4);
					const auto hull = source.convex_of_primitive[p];
					convex.table.push_back(146);
					convex.table.push_back(0);
					convex.table.push_back(static_cast<std::uint16_t>(hull));
					convex.table.push_back(static_cast<std::uint16_t>(hull >> 16));
					convex.primitives.push_back({static_cast<std::uint8_t>(local), static_cast<std::uint8_t>(local), 4,
						static_cast<std::uint8_t>(local)});
					convex_bounds.push_back(primitive_bounds(source, p));
					convex_tags.push_back(source.tags[p]);
				}

				convex.runs = make_runs(convex_tags);
				build_section_tree(convex, convex_bounds);
				outputs.push_back(std::move(convex));
			}
		}

		{
			std::size_t externs = 0;
			std::size_t degenerate = 0;
			for (const auto& output : outputs)
			{
				for (const auto& indices : output.primitives)
				{
					if (indices[1] == indices[3] && indices[2] == 4) externs++;
					if (indices[0] == 0 && indices[1] == 0 && indices[2] == 1 && indices[3] == 1) degenerate++;
				}
			}
			ZONETOOL_INFO("havok: %zu hulls, %zu outputs, %zu convex, %zu externs, %zu degenerates", hulls.size(), outputs.size(),
				convex_total, externs, degenerate);
		}

		std::vector<std::array<std::uint8_t, 4>> all_primitives;
		std::vector<std::uint16_t> all_table;
		std::vector<iw7::hknpCompressedMeshShapeTreeDataRun> all_runs;
		for (auto& output : outputs)
		{
			output.first_shared = static_cast<std::uint32_t>(all_table.size());
			output.first_primitive = static_cast<std::uint32_t>(all_primitives.size());
			output.first_run = static_cast<std::uint32_t>(all_runs.size());
			all_table.insert(all_table.end(), output.table.begin(), output.table.end());
			all_primitives.insert(all_primitives.end(), output.primitives.begin(), output.primitives.end());
			all_runs.insert(all_runs.end(), output.runs.begin(), output.runs.end());
		}

		const auto section_bits = static_cast<std::uint32_t>(std::bit_width(std::max<std::size_t>(outputs.size(), 1) - 1));
		const auto primitive_bits = 8u;
		const auto bits_per_key = section_bits + primitive_bits;

		std::uint32_t max_key = 0;
		std::uint32_t triangle_count = 0;
		std::uint32_t convex_count = 0;
		for (auto s = 0u; s < outputs.size(); s++)
		{
			const auto& output = outputs[s];
			for (auto p = 0u; p < output.primitives.size(); p++)
			{
				const auto key = (s << primitive_bits) | (p << 1);
				max_key = std::max(max_key, key);
				const auto& indices = output.primitives[p];
				if (indices[1] == indices[3] && indices[2] == 4)
				{
					convex_count++;
				}
				else if (indices[2] == indices[3])
				{
					triangle_count++;
				}
				else
				{
					triangle_count += 2;
				}
			}
		}

		std::vector<vec3> section_centres;
		std::vector<bounds> section_bounds;
		std::vector<std::uint32_t> order(outputs.size());
		std::iota(order.begin(), order.end(), 0u);
		for (const auto& output : outputs)
		{
			section_centres.push_back(center_of(output.domain));
			section_bounds.push_back(output.domain);
		}

		std::vector<top_node> top_nodes;
		if (!order.empty())
		{
			split_top(section_centres, order, top_nodes);
		}

		std::vector<std::array<std::uint8_t, 5>> top_bytes(top_nodes.size());
		std::vector<std::uint32_t> top_leaves(outputs.size(), 0);
		if (!top_nodes.empty())
		{
			encode_top(top_nodes, section_bounds, 0, domain, top_bytes, top_leaves);
		}

		for (auto s = 0u; s < outputs.size(); s++)
		{
			if (top_leaves[s] > 0xFFFF)
			{
				throw std::runtime_error("havok: the section tree is too large");
			}

			outputs[s].leaf_index = static_cast<std::uint16_t>(top_leaves[s]);
		}

		tag_writer writer(schema());
		const auto t_list = writer.type_id("HavokPhysicsShapeList");
		const auto t_tag = writer.type_id("HavokPhysicsShapeList::ShapeTagData");
		const auto t_shape = writer.type_id("hknpCompressedMeshShape");
		const auto t_data = writer.type_id("hknpCompressedMeshShapeData");
		const auto t_instance = writer.type_id("hknpShapeInstance");
		const auto t_hull = writer.type_id("hknpConvexPolytopeShape");
		const auto t_face = writer.type_id("hknpConvexPolytopeShape::Face");
		const auto t_connectivity = writer.type_id("hknpConvexPolytopeShape::Connectivity");
		const auto t_edge = writer.type_id("hknpConvexPolytopeShape::Connectivity::Edge");
		const auto t_section = writer.type_id("hkcdStaticMeshTree::Section");
		const auto t_primitive = writer.type_id("hkcdStaticMeshTree::Primitive");
		const auto t_run = writer.type_id("hkcdDefaultStaticMeshTree::PrimitiveDataRun");
		const auto t_top_node = writer.type_id("hkcdCompressedAabbCodecs::Aabb5BytesCodec");
		const auto t_section_node = writer.type_id("hkcdCompressedAabbCodecs::Aabb4BytesCodec");
		const auto t_simd_node = writer.type_id("hkcdSimdTree::Node");
		const auto t_vector4f = writer.type_id("hkVector4f");
		const auto t_vector4 = writer.type_id("hkVector4");
		const auto t_uint8 = writer.type_id("hkUint8");
		const auto t_uint16 = writer.type_id("hkUint16");
		const auto t_uint32 = writer.type_id("unsigned int");
		const auto t_uint64 = writer.type_id("unsigned long long");
		const auto t_int = writer.type_id("int");
		const auto t_char = writer.type_id("char");
		const auto t_pointer = writer.type_id("T*");
		const auto t_string = writer.type_id("hkStringPtr");

		auto list_bytes = std::vector<std::byte>(writer.type_size(t_list), std::byte{0});
		const auto list_offset = writer.add_object(t_list, list_bytes.data());

		const auto tags = convert_shape_tags(list);
		std::uint32_t world_contents = 0;
		for (const auto& tag : tags)
		{
			world_contents |= tag.collisionFilterInfo;
		}

		std::string name;
		if (list->shapeNames.size > 0 && list->shapeNames.data[0])
		{
			name = list->shapeNames.data[0];
		}
		if (name.empty())
		{
			name = "world";
		}

		const auto name_item = writer.add_array(t_char, name.data(), name.size() + 1);
		std::vector<std::uint64_t> name_pointers(1, 0);
		const auto names_item = writer.add_array(t_string, name_pointers.data(), name_pointers.size());
		writer.set_pointer(names_item.offset, t_string, name_item.item);

		std::vector<std::uint64_t> shape_pointers(1, 0);
		const auto shapes_item = writer.add_array(t_pointer, shape_pointers.data(), shape_pointers.size());

		const std::int32_t shape_index = -1;
		const auto root_indices_item = writer.add_array(t_int, &shape_index, 1);

		const std::int32_t vertex_count = list->vertCounts.size > 0
			? list->vertCounts.data[0]
			: static_cast<std::int32_t>(tree.sharedVertices.size + tree.packedVertices.size);
		const auto vert_counts_item = writer.add_array(t_int, &vertex_count, 1);
		const auto tri_counts_item = writer.add_array(t_int, &triangle_count, 1);
		const auto contents_item = writer.add_array(t_int, &world_contents, 1);
		const auto convex_counts_item = writer.add_array(t_int, &convex_count, 1);

		std::vector<std::array<float, 4>> min_maxes;
		if (list->minMaxes.size == 2)
		{
			for (auto i = 0; i < 2; i++)
			{
				min_maxes.push_back({list->minMaxes.data[i].x, list->minMaxes.data[i].y, list->minMaxes.data[i].z,
					list->minMaxes.data[i].w});
			}
		}
		else
		{
			min_maxes.push_back({domain.min[0], domain.min[1], domain.min[2], 0.0f});
			min_maxes.push_back({domain.max[0], domain.max[1], domain.max[2], 0.0f});
		}
		const auto min_maxes_item = writer.add_array(t_vector4, min_maxes.data(), min_maxes.size());
		const auto tag_item = writer.add_array(t_tag, tags.data(), tags.size());

		std::vector<tag_writer::reference> hull_items;
		hull_items.reserve(hulls.size());
		for (auto i = 0u; i < hulls.size(); i++)
		{
			const auto& hull = hulls[i];

			auto hull_bytes = std::vector<std::byte>(writer.type_size(t_hull), std::byte{0});
			put_member(hull_bytes, t_hull, "flags", static_cast<std::uint16_t>(0x0943));
			put_member(hull_bytes, t_hull, "type", static_cast<std::uint8_t>(1));
			put_member(hull_bytes, t_hull, "numShapeKeyBits", static_cast<std::uint8_t>(0));
			put_member(hull_bytes, t_hull, "dispatchType", static_cast<std::uint8_t>(2));
			put_member(hull_bytes, t_hull, "convexRadius", 0.0f);
			put_member(hull_bytes, t_hull, "userData", static_cast<std::uint64_t>(0));
			put_member(hull_bytes, t_hull, "maxAllowedPenetration", static_cast<std::uint16_t>(0x3EC6));

			const auto hull_offset = writer.add_object(t_hull, hull_bytes.data());

			std::vector<std::array<float, 4>> vertices;
			vertices.reserve(hull.vertices.size());
			for (const auto& vertex : hull.vertices)
			{
				vertices.push_back({vertex.x, vertex.y, vertex.z, 0.5f});
			}
			const auto vertices_item = writer.add_array(t_vector4f, vertices.data(), vertices.size());

			std::vector<std::array<float, 4>> planes;
			planes.reserve(hull.faces.size());
			std::vector<std::array<std::uint8_t, 4>> faces;
			std::vector<std::uint8_t> indices;
			std::uint16_t first_index = 0;
			for (const auto& face : hull.faces)
			{
				planes.push_back({face.normal.x, face.normal.y, face.normal.z, face.distance});
				faces.push_back({static_cast<std::uint8_t>(first_index), static_cast<std::uint8_t>(first_index >> 8),
					static_cast<std::uint8_t>(face.vertices.size()), 127});
				for (const auto vertex : face.vertices)
				{
					indices.push_back(static_cast<std::uint8_t>(vertex));
				}
				first_index = static_cast<std::uint16_t>(first_index + face.vertices.size());
			}

			const auto planes_item = writer.add_array(t_vector4, planes.data(), planes.size());
			const auto faces_item = writer.add_array(t_face, faces.data(), faces.size());
			const auto indices_item = writer.add_array(t_uint8, indices.data(), indices.size());

			writer.set_pointer(hull_offset.offset + member(t_hull, "vertices").offset, member(t_hull, "vertices").type,
				vertices_item.item);
			writer.set_pointer(hull_offset.offset + member(t_hull, "planes").offset, member(t_hull, "planes").type,
				planes_item.item);
			writer.set_pointer(hull_offset.offset + member(t_hull, "faces").offset, member(t_hull, "faces").type,
				faces_item.item);
			writer.set_pointer(hull_offset.offset + member(t_hull, "indices").offset, member(t_hull, "indices").type,
				indices_item.item);

			std::vector<connectivity_edge> vertex_edges;
			std::vector<connectivity_edge> face_links;
			add_connectivity(hull, vertex_edges, face_links);
			std::vector<std::array<std::uint8_t, 4>> vertex_edge_bytes;
			std::vector<std::array<std::uint8_t, 4>> face_link_bytes;
			for (const auto& edge : vertex_edges)
			{
				vertex_edge_bytes.push_back({static_cast<std::uint8_t>(edge.face), static_cast<std::uint8_t>(edge.face >> 8),
					edge.index, 0});
			}
			for (const auto& edge : face_links)
			{
				face_link_bytes.push_back({static_cast<std::uint8_t>(edge.face), static_cast<std::uint8_t>(edge.face >> 8),
					edge.index, 0});
			}

			const auto vertex_edges_item = writer.add_array(t_edge, vertex_edge_bytes.data(), vertex_edge_bytes.size());
			const auto face_links_item = writer.add_array(t_edge, face_link_bytes.data(), face_link_bytes.size());

			auto connectivity_bytes = std::vector<std::byte>(writer.type_size(t_connectivity), std::byte{0});
			const auto connectivity_offset = writer.add_object(t_connectivity, connectivity_bytes.data());
			const auto& connectivity_vertex_edges = member(t_connectivity, "vertexEdges");
			const auto& connectivity_face_links = member(t_connectivity, "faceLinks");
			writer.set_pointer(connectivity_offset.offset + connectivity_vertex_edges.offset,
				connectivity_vertex_edges.type, vertex_edges_item.item);
			writer.set_pointer(connectivity_offset.offset + connectivity_face_links.offset,
				connectivity_face_links.type, face_links_item.item);

			writer.set_pointer(hull_offset.offset + member(t_hull, "connectivity").offset,
				member(t_hull, "connectivity").type, connectivity_offset.item);
			hull_items.push_back(hull_offset);
		}

		std::vector<std::byte> instance_bytes(writer.type_size(t_instance) * hulls.size(), std::byte{0});
		for (auto i = 0u; i < hulls.size(); i++)
		{
			const auto base = i * writer.type_size(t_instance);
			const auto identity = 1.0f;
			put(instance_bytes, base + 0, identity);
			put(instance_bytes, base + 20, identity);
			put(instance_bytes, base + 40, identity);
			put(instance_bytes, base + 64, identity);
			put(instance_bytes, base + 68, identity);
			put(instance_bytes, base + 72, identity);
			put(instance_bytes, base + 76, identity);
			put(instance_bytes, base + member(t_instance, "shapeTag").offset, hull_tags[i]);
			put(instance_bytes, base + member(t_instance, "destructionTag").offset, static_cast<std::uint16_t>(0xFFFF));
			put(instance_bytes, base + member(t_instance, "isEmpty").offset, static_cast<std::uint8_t>(0));
			put(instance_bytes, base + member(t_instance, "nextEmptyElement").offset, static_cast<std::uint32_t>(0));
			put(instance_bytes, base + member(t_instance, "instanceId").offset, static_cast<std::uint16_t>(0));
		}

		const auto instances_item = writer.add_array(t_instance, instance_bytes.data(), hulls.size());
		for (auto i = 0u; i < hulls.size(); i++)
		{
			const auto& shape_field = member(t_instance, "shape");
			writer.set_pointer(instances_item.offset + i * writer.type_size(t_instance) + shape_field.offset,
				shape_field.type, hull_items[i].item);
		}

		const auto mesh_nodes_item = writer.add_array(t_top_node, top_bytes.data(), top_bytes.size());
		const auto mesh_primitives_item = writer.add_array(t_primitive, all_primitives.data(), all_primitives.size());
		const auto mesh_table_item = writer.add_array(t_uint16, all_table.data(), all_table.size());
		const auto mesh_runs_item = writer.add_array(t_run, all_runs.data(), all_runs.size());

		std::vector<std::uint32_t> packed(tree.packedVertices.size);
		for (auto i = 0; i < tree.packedVertices.size; i++)
		{
			packed[i] = tree.packedVertices.data[i];
		}
		const auto mesh_packed_item = writer.add_array(t_uint32, packed.data(), packed.size());

		std::vector<std::uint64_t> shared(tree.sharedVertices.size);
		for (auto i = 0; i < tree.sharedVertices.size; i++)
		{
			shared[i] = tree.sharedVertices.data[i];
		}
		const auto mesh_shared_item = writer.add_array(t_uint64, shared.data(), shared.size());

		std::vector<tag_writer::reference> section_nodes_items;
		section_nodes_items.reserve(outputs.size());
		for (const auto& output : outputs)
		{
			section_nodes_items.push_back(writer.add_array(t_section_node, output.nodes.data(), output.nodes.size()));
		}

		std::vector<std::byte> section_bytes(writer.type_size(t_section) * outputs.size(), std::byte{0});
		for (auto s = 0u; s < outputs.size(); s++)
		{
			const auto& output = outputs[s];
			const auto base = s * writer.type_size(t_section);
			const auto& domain_field = member(t_section, "domain");
			for (auto axis = 0; axis < 3; axis++)
			{
				put(section_bytes, base + domain_field.offset + axis * 4, output.domain.min[axis]);
				put(section_bytes, base + domain_field.offset + 16 + axis * 4, output.domain.max[axis]);
			}

			const auto& codec_field = member(t_section, "codecParms");
			for (auto i = 0; i < 6; i++)
			{
				put(section_bytes, base + codec_field.offset + i * 4, output.codec[i]);
			}

			put(section_bytes, base + member(t_section, "firstPackedVertexIndex").offset, output.first_packed);
			put(section_bytes, base + member(t_section, "firstSharedVertexIndex").offset, output.first_shared);
			put(section_bytes, base + member(t_section, "firstPrimitiveIndex").offset, output.first_primitive);
			put(section_bytes, base + member(t_section, "firstDataRunIndex").offset, output.first_run);
			put(section_bytes, base + member(t_section, "numPackedVertices").offset, output.packed_count);
			put(section_bytes, base + member(t_section, "numPrimitives").offset,
				static_cast<std::uint8_t>(output.primitives.size()));
			put(section_bytes, base + member(t_section, "numDataRuns").offset,
				static_cast<std::uint8_t>(output.runs.size()));
			put(section_bytes, base + member(t_section, "page").offset, output.page);
			put(section_bytes, base + member(t_section, "leafIndex").offset, output.leaf_index);
			put(section_bytes, base + member(t_section, "layerData").offset, output.layer_data);
			put(section_bytes, base + member(t_section, "flags").offset, output.flags);
		}

		const auto sections_item = writer.add_array(t_section, section_bytes.data(), outputs.size());
		for (auto s = 0u; s < outputs.size(); s++)
		{
			const auto& nodes_field = member(t_section, "nodes");
			writer.set_pointer(sections_item.offset + s * writer.type_size(t_section) + nodes_field.offset,
				nodes_field.type, section_nodes_items[s].item);
		}

		auto data_bytes = std::vector<std::byte>(writer.type_size(t_data), std::byte{0});
		const auto& mesh_tree_field = member(t_data, "meshTree");
		const auto mesh_tree_offset = mesh_tree_field.offset;
		const auto mesh_tree_type = mesh_tree_field.type;
		std::memcpy(data_bytes.data() + mesh_tree_offset + member(mesh_tree_type, "domain").offset, &tree.domain,
			sizeof(tree.domain));
		put(data_bytes, mesh_tree_offset + member(mesh_tree_type, "numPrimitiveKeys").offset,
			static_cast<std::int32_t>(triangle_count + convex_count));
		put(data_bytes, mesh_tree_offset + member(mesh_tree_type, "bitsPerKey").offset,
			static_cast<std::int32_t>(bits_per_key));
		put(data_bytes, mesh_tree_offset + member(mesh_tree_type, "maxKeyValue").offset, max_key);
		put(data_bytes, mesh_tree_offset + member(mesh_tree_type, "primitiveStoresIsFlatConvex").offset,
			static_cast<std::uint8_t>(0xFF));
		put(data_bytes, member(t_data, "hasSimdTree").offset, static_cast<std::uint8_t>(1));

		const auto simd_node_count = 3238u;
		const auto simd_node_size = writer.type_size(t_simd_node);
		std::vector<std::byte> simd_bytes(simd_node_size * simd_node_count, std::byte{0});
		const auto max_float = std::numeric_limits<float>::max();
		for (auto i = 0u; i < simd_node_count; i++)
		{
			const auto base = i * simd_node_size;
			for (auto box = 0u; box < 3; box++)
			{
				for (auto f = 0u; f < 4; f++)
				{
					put(simd_bytes, base + box * 32 + f * 4, max_float);
					put(simd_bytes, base + box * 32 + 16 + f * 4, -max_float);
				}
			}
		}
		const auto simd_nodes_item = writer.add_array(t_simd_node, simd_bytes.data(), simd_node_count);

		const auto data_offset = writer.add_object(t_data, data_bytes.data());
		const auto set_tree_array = [&](const std::string_view name, const tag_writer::reference& reference)
		{
			const auto& field = member(mesh_tree_type, name);
			writer.set_pointer(data_offset.offset + mesh_tree_offset + field.offset, field.type, reference.item);
		};
		set_tree_array("nodes", mesh_nodes_item);
		set_tree_array("sections", sections_item);
		set_tree_array("primitives", mesh_primitives_item);
		set_tree_array("sharedVerticesIndex", mesh_table_item);
		set_tree_array("packedVertices", mesh_packed_item);
		set_tree_array("sharedVertices", mesh_shared_item);
		set_tree_array("primitiveDataRuns", mesh_runs_item);
		const auto& simd_tree_field = member(t_data, "simdTree");
		const auto& simd_nodes_field = member(simd_tree_field.type, "nodes");
		writer.set_pointer(data_offset.offset + simd_tree_field.offset + simd_nodes_field.offset,
			simd_nodes_field.type, simd_nodes_item.item);

		const auto interior_bits = max_key + 1;
		std::vector<std::uint32_t> interior_words((interior_bits + 31) / 32, 0);
		const auto interior_words_item = writer.add_array(t_uint32, interior_words.data(), interior_words.size());

		auto shape_bytes = std::vector<std::byte>(writer.type_size(t_shape), std::byte{0});
		put_member(shape_bytes, t_shape, "flags", static_cast<std::uint16_t>(0x0004));
		put_member(shape_bytes, t_shape, "type", static_cast<std::uint8_t>(9));
		put_member(shape_bytes, t_shape, "numShapeKeyBits", static_cast<std::uint8_t>(bits_per_key));
		put_member(shape_bytes, t_shape, "dispatchType", static_cast<std::uint8_t>(3));
		put_member(shape_bytes, t_shape, "convexRadius", 0.0f);
		put_member(shape_bytes, t_shape, "userData", shape->userData);
		put_member(shape_bytes, t_shape, "shapeTagCodecInfo", static_cast<std::uint32_t>(0xFFFFFFFF));

		const auto& interior_field = member(t_shape, "triangleIsInterior");
		const auto interior_type = interior_field.type;
		const auto& storage_field = member(interior_type, "storage");
		const auto storage_type = storage_field.type;
		put(shape_bytes, interior_field.offset + storage_field.offset + member(storage_type, "numBits").offset,
			static_cast<std::int32_t>(interior_bits));

		const auto shape_offset = writer.add_object(t_shape, shape_bytes.data());
		writer.set_pointer(shape_offset.offset + member(t_shape, "data").offset, member(t_shape, "data").type,
			data_offset.item);
		writer.set_pointer(shape_offset.offset + member(t_shape, "externShapes").offset,
			member(t_shape, "externShapes").type, instances_item.item);
		writer.set_pointer(shapes_item.offset, t_pointer, shape_offset.item);
		const auto& interior_words_field = member(storage_type, "words");
		writer.set_pointer(shape_offset.offset + interior_field.offset + storage_field.offset
			+ interior_words_field.offset, interior_words_field.type, interior_words_item.item);

		const auto set_list_array = [&](const std::string_view name, const tag_writer::reference& reference)
		{
			const auto& field = member(t_list, name);
			writer.set_pointer(list_offset.offset + field.offset, field.type, reference.item);
		};
		set_list_array("shapes", shapes_item);
		set_list_array("shapeIndices", root_indices_item);
		set_list_array("shapeNames", names_item);
		set_list_array("vertCounts", vert_counts_item);
		set_list_array("triCounts", tri_counts_item);
		set_list_array("minMaxes", min_maxes_item);
		set_list_array("shapeTagData", tag_item);
		set_list_array("shapeContents", contents_item);
		set_list_array("convexCounts", convex_counts_item);

		return writer.finish(list_offset.item);
	}

	std::string convert_ents_shape_list(const std::string_view packfile)
	{
		const iw7::havok::binary::packfile_reader reader(packfile);
		const auto tags = convert_shape_tags(read_shape_list(reader));

		tag_writer writer(schema());
		const auto t_list = writer.type_id("HavokPhysicsShapeList");
		const auto t_tag = writer.type_id("HavokPhysicsShapeList::ShapeTagData");
		const auto t_int = writer.type_id("int");
		const auto t_uint32 = writer.type_id("unsigned int");
		const auto t_pointer = writer.type_id("T*");
		const auto t_string = writer.type_id("hkStringPtr");
		const auto t_vector4 = writer.type_id("hkVector4");

		auto list_bytes = std::vector<std::byte>(writer.type_size(t_list), std::byte{0});
		const auto list_offset = writer.add_object(t_list, list_bytes.data());

		std::uint64_t dummy64 = 0;
		std::int32_t dummy32 = 0;
		std::uint32_t dummy_uint = 0;
		std::array<float, 4> dummy_vec{};

		const auto shapes_item = writer.add_array(t_pointer, &dummy64, 0);
		const auto indices_item = writer.add_array(t_int, &dummy32, 0);
		const auto names_item = writer.add_array(t_string, &dummy64, 0);
		const auto vert_counts_item = writer.add_array(t_int, &dummy32, 0);
		const auto tri_counts_item = writer.add_array(t_int, &dummy32, 0);
		const auto min_maxes_item = writer.add_array(t_vector4, &dummy_vec, 0);
		const auto tag_item = writer.add_array(t_tag, tags.data(), tags.size());
		const auto contents_item = writer.add_array(t_uint32, &dummy_uint, 0);
		const auto convex_counts_item = writer.add_array(t_int, &dummy32, 0);

		const auto set_list_array = [&](const std::string_view name, const tag_writer::reference& reference)
		{
			const auto& field = member(t_list, name);
			writer.set_pointer(list_offset.offset + field.offset, field.type, reference.item);
		};
		set_list_array("shapes", shapes_item);
		set_list_array("shapeIndices", indices_item);
		set_list_array("shapeNames", names_item);
		set_list_array("vertCounts", vert_counts_item);
		set_list_array("triCounts", tri_counts_item);
		set_list_array("minMaxes", min_maxes_item);
		set_list_array("shapeTagData", tag_item);
		set_list_array("shapeContents", contents_item);
		set_list_array("convexCounts", convex_counts_item);

		return writer.finish(list_offset.item);
	}


	namespace
	{
		bool is_inside(const convex_result& hull, const vec3& point)
		{
			return std::ranges::all_of(hull.faces, [&](const hull_face& face)
			{
				return dot(face.normal, point) + face.distance <= 0.0f;
			});
		}

		float face_area(const convex_result& hull, const hull_face& face)
		{
			vec3 sum{};
			for (auto i = 1u; i + 1 < face.vertices.size(); i++)
			{
				const auto& origin = hull.vertices[face.vertices[0]];
				sum = sum + cross(hull.vertices[face.vertices[i]] - origin, hull.vertices[face.vertices[i + 1]] - origin);
			}

			return length(sum) * 0.5f;
		}

		vec3 face_center(const convex_result& hull, const hull_face& face)
		{
			vec3 sum{};
			for (const auto vertex : face.vertices)
			{
				sum = sum + hull.vertices[vertex];
			}

			return sum * (1.0f / static_cast<float>(face.vertices.size()));
		}

		std::uint32_t count_blocked_probes(const std::vector<convex_result>& solids, const vec3& center, const vec3& normal,
			const float bottom, const float top)
		{
			auto blocked = 0u;
			for (const auto distance : {1.0f, 4.0f, 8.0f, 16.0f, 24.0f})
			{
				for (const auto height : {0.25f, 0.5f, 0.75f})
				{
					auto probe = center + normal * (distance / 32.0f);
					probe.z = bottom + (top - bottom) * height;
					blocked += static_cast<std::uint32_t>(std::ranges::count_if(solids, [&](const convex_result& solid)
					{
						return is_inside(solid, probe);
					}));
				}
			}

			return blocked;
		}

		std::optional<ladder> make_ladder(const convex_result& hull, const std::vector<convex_result>& solids)
		{
			auto box = empty_bounds();
			for (const auto& vertex : hull.vertices)
			{
				include(box, vertex);
			}

			std::vector<const hull_face*> sides;
			for (const auto& face : hull.faces)
			{
				if (std::abs(face.normal.z) < 0.1f)
				{
					sides.push_back(&face);
				}
			}

			std::ranges::sort(sides, [&](const hull_face* a, const hull_face* b)
			{
				return face_area(hull, *a) > face_area(hull, *b);
			});

			if (sides.size() < 2)
			{
				return {};
			}

			const auto* front = sides[0];
			const auto* back = sides[1];
			const auto front_blocked = count_blocked_probes(solids, face_center(hull, *front), front->normal, box.min[2], box.max[2]);
			const auto back_blocked = count_blocked_probes(solids, face_center(hull, *back), back->normal, box.min[2], box.max[2]);
			if (back_blocked < front_blocked)
			{
				std::swap(front, back);
			}
			else if (back_blocked == front_blocked)
			{
				ZONETOOL_WARNING("havok: both sides of the ladder at (%g %g %g) are open", box.min[0] * 32.0f, box.min[1] * 32.0f, box.min[2] * 32.0f);
			}

			const auto facing = normalize({front->normal.x, front->normal.y, 0.0f});
			const auto across = cross({0.0f, 0.0f, 1.0f}, facing);
			auto min_across = std::numeric_limits<float>::max();
			auto max_across = std::numeric_limits<float>::lowest();
			for (const auto vertex : front->vertices)
			{
				const auto value = dot(hull.vertices[vertex], across);
				min_across = std::min(min_across, value);
				max_across = std::max(max_across, value);
			}

			const auto center = face_center(hull, *front);
			ladder result{};
			result.bottom = {center.x * 32.0f, center.y * 32.0f, box.min[2] * 32.0f};
			result.height = (box.max[2] - box.min[2]) * 32.0f;
			result.facing = {facing.x, facing.y, facing.z};
			result.width = (max_across - min_across) * 32.0f;
			return result;
		}
	}

	std::vector<ladder> find_ladders(const std::string_view packfile)
	{
		const iw7::havok::binary::packfile_reader reader(packfile);
		const auto* list = read_shape_list(reader);
		const auto& tree = read_world_mesh(list);

		std::vector<convex_result> ladder_hulls;
		std::vector<convex_result> solids;
		auto skipped = 0u;
		for (auto s = 0; s < tree.sections.size; s++)
		{
			const auto& section = tree.sections.data[s];
			const auto first_primitive = section.primitives >> 8;
			const auto primitive_count = section.primitives & 0xFF;
			std::vector<std::uint32_t> tags(primitive_count, 0xFFFF);
			for (auto r = 0u; r < (section.dataRuns & 0xFF); r++)
			{
				const auto& run = tree.primitiveDataRuns.data[(section.dataRuns >> 8) + r];
				const auto run_end = std::min(static_cast<std::uint32_t>(run.index + run.count), primitive_count);
				for (auto p = static_cast<std::uint32_t>(run.index); p < run_end; p++)
				{
					tags[p] = run.value;
				}
			}

			for (auto p = 0u; p < primitive_count; p++)
			{
				const auto is_ladder = tags[p] < static_cast<std::uint32_t>(list->shapeTagData.size)
					&& list->shapeTagData.data[tags[p]].userData & iw7::SURFACE_FLAG_LADDER;

				std::array<std::uint8_t, 4> indices{};
				std::memcpy(indices.data(), &tree.primitives.data[first_primitive + p], 4);
				if (!is_convex_primitive(indices))
				{
					skipped += is_ladder;
					continue;
				}

				auto hull = make_convex_hull(read_convex_points(tree, section.numPackedVertices, section.sharedVertices >> 8, section.page, indices));
				(is_ladder ? ladder_hulls : solids).push_back(std::move(hull));
			}
		}

		if (skipped)
		{
			ZONETOOL_WARNING("havok: %u ladder triangles are not convex and have no ladder edges", skipped);
		}

		std::vector<ladder> ladders;
		for (const auto& hull : ladder_hulls)
		{
			if (const auto result = make_ladder(hull, solids))
			{
				ladders.push_back(*result);
			}
		}

		return ladders;
	}
}
