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

		const tag_member* find_member(const std::vector<tag_type>& types, std::uint32_t type, const std::string_view name)
		{
			while (type)
			{
				for (const auto& member : types.at(type).members)
				{
					if (member.name == name)
					{
						return &member;
					}
				}

				type = types.at(type).parent;
			}

			return nullptr;
		}

		const tag_member* find_member(const std::uint32_t type, const std::string_view name)
		{
			return find_member(schema(), type, name);
		}

		const tag_member& member(const std::vector<tag_type>& types, const std::uint32_t type, const std::string_view name)
		{
			const auto* found = find_member(types, type, name);
			if (!found)
			{
				throw std::runtime_error(std::format("havok schema: type {} has no member {}", type, name));
			}

			return *found;
		}

		const tag_member& member(const std::uint32_t type, const std::string_view name)
		{
			return member(schema(), type, name);
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

		bool touches(const bounds& a, const bounds& b, const float gap)
		{
			return a.min[0] <= b.max[0] + gap && b.min[0] <= a.max[0] + gap && a.min[1] <= b.max[1] + gap && b.min[1] <= a.max[1] + gap;
		}

		std::vector<bounds> find_treads(const std::vector<convex_result>& hulls, const std::vector<std::uint16_t>& hull_tags,
			const iw7::HavokPhysicsShapeList* list, std::vector<std::uint16_t>& tread_tags)
		{
			std::vector<bounds> pieces;
			std::vector<std::uint16_t> piece_tags;
			for (auto i = 0u; i < hulls.size(); i++)
			{
				if (hull_tags[i] >= static_cast<std::uint32_t>(list->shapeTagData.size)
					|| !(list->shapeTagData.data[hull_tags[i]].collisionFilterInfo & iw7::PHYSICS_CONTENTS_SOLID))
				{
					continue;
				}

				auto local = empty_bounds();
				for (const auto& vertex : hulls[i].vertices)
				{
					include(local, vertex);
				}

				const bounds box{{local.min[0] * 32.0f, local.min[1] * 32.0f, local.min[2] * 32.0f},
					{local.max[0] * 32.0f, local.max[1] * 32.0f, local.max[2] * 32.0f}};
				if (box.max[2] - box.min[2] <= 24.0f && is_box(hulls[i].vertices, local))
				{
					pieces.push_back(box);
					piece_tags.push_back(hull_tags[i]);
				}
			}

			std::vector<std::int32_t> owner(pieces.size(), -1);
			std::vector<bounds> treads;
			for (auto i = 0u; i < pieces.size(); i++)
			{
				if (owner[i] >= 0)
				{
					continue;
				}

				auto tread = pieces[i];
				owner[i] = static_cast<std::int32_t>(treads.size());
				for (auto grown = true; grown;)
				{
					grown = false;
					for (auto j = 0u; j < pieces.size(); j++)
					{
						if (owner[j] < 0 && std::abs(pieces[j].max[2] - tread.max[2]) < 0.5f && touches(pieces[j], tread, 1.0f))
						{
							include(tread, {pieces[j].min[0], pieces[j].min[1], pieces[j].min[2]});
							include(tread, {pieces[j].max[0], pieces[j].max[1], pieces[j].max[2]});
							owner[j] = owner[i];
							grown = true;
						}
					}
				}

				treads.push_back(tread);
				tread_tags.push_back(piece_tags[i]);
			}

			return treads;
		}

		std::optional<std::int32_t> find_next_tread(const std::vector<bounds>& treads, const std::uint32_t current, const std::int32_t run_axis,
			std::int32_t& direction)
		{
			const auto& a = treads[current];
			std::optional<std::int32_t> best;
			auto best_rise = 0.0f;
			for (auto i = 0u; i < treads.size(); i++)
			{
				const auto& b = treads[i];
				const auto rise = b.max[2] - a.max[2];
				if (rise < 3.0f || rise > 18.0f || (best && rise >= best_rise))
				{
					continue;
				}

				for (auto axis = 0; axis < 2; axis++)
				{
					if (run_axis >= 0 && axis != run_axis)
					{
						continue;
					}

					const auto across = 1 - axis;
					const auto run = (b.min[axis] + b.max[axis] - a.min[axis] - a.max[axis]) * 0.5f;
					const auto offset = (b.min[across] + b.max[across] - a.min[across] - a.max[across]) * 0.5f;
					const auto width = a.max[across] - a.min[across];
					const auto step_direction = run > 0.0f ? 1 : -1;
					if (std::abs(run) < 4.0f || std::abs(run) > 24.0f || std::abs(offset) > 4.0f || width < 16.0f
						|| std::abs(b.max[across] - b.min[across] - width) > 4.0f || a.max[axis] - a.min[axis] > 24.0f
						|| (run_axis >= 0 && step_direction != direction))
					{
						continue;
					}

					best = static_cast<std::int32_t>(i);
					best_rise = rise;
					direction = step_direction;
				}
			}

			return best;
		}

		convex_result make_stair_ramp(const std::vector<bounds>& treads, const std::vector<std::uint32_t>& flight, const std::int32_t axis,
			const std::int32_t direction)
		{
			const auto across = 1 - axis;
			const auto front_of = [&](const bounds& tread) { return direction > 0 ? tread.min[axis] : tread.max[axis]; };
			const auto& first = treads[flight.front()];
			const auto& last = treads[flight.back()];
			const auto steps = static_cast<float>(flight.size() - 1);
			const auto rise = (last.max[2] - first.max[2]) / steps;
			const auto run = (front_of(last) - front_of(first)) / steps;

			auto low = first.min[across];
			auto high = first.max[across];
			for (const auto index : flight)
			{
				low = std::min(low, treads[index].min[across]);
				high = std::max(high, treads[index].max[across]);
			}

			const auto foot = front_of(first) - run;
			const auto foot_height = first.max[2] - rise;
			const auto thickness = 2.0f * rise;
			const std::array<std::array<float, 2>, 5> profile{{
				{foot, foot_height},
				{front_of(last), last.max[2]},
				{direction > 0 ? last.max[axis] : last.min[axis], last.max[2]},
				{direction > 0 ? last.max[axis] : last.min[axis], last.max[2] - thickness},
				{foot + 2.0f * run, foot_height},
			}};

			std::vector<vec3> points;
			for (const auto side : {low, high})
			{
				for (const auto& [along, height] : profile)
				{
					std::array<float, 3> point{};
					point[axis] = along / 32.0f;
					point[across] = side / 32.0f;
					point[2] = height / 32.0f;
					points.push_back({point[0], point[1], point[2]});
				}
			}

			return make_convex_hull(points);
		}

		std::vector<std::pair<convex_result, std::uint16_t>> make_stair_ramps(const std::vector<convex_result>& hulls,
			const std::vector<std::uint16_t>& hull_tags, const iw7::HavokPhysicsShapeList* list)
		{
			std::vector<std::uint16_t> tread_tags;
			const auto treads = find_treads(hulls, hull_tags, list, tread_tags);

			std::vector<bool> has_previous(treads.size(), false);
			for (auto i = 0u; i < treads.size(); i++)
			{
				auto direction = 0;
				if (const auto next = find_next_tread(treads, i, -1, direction))
				{
					has_previous[*next] = true;
				}
			}

			std::vector<std::pair<convex_result, std::uint16_t>> ramps;
			for (auto i = 0u; i < treads.size(); i++)
			{
				auto direction = 0;
				const auto second = has_previous[i] ? std::nullopt : find_next_tread(treads, i, -1, direction);
				if (!second)
				{
					continue;
				}

				const auto& a = treads[i];
				const auto& b = treads[*second];
				const auto axis = std::abs(b.min[0] + b.max[0] - a.min[0] - a.max[0]) > std::abs(b.min[1] + b.max[1] - a.min[1] - a.max[1]) ? 0 : 1;
				std::vector<std::uint32_t> flight{i, static_cast<std::uint32_t>(*second)};
				while (flight.size() < 128)
				{
					const auto next = find_next_tread(treads, flight.back(), axis, direction);
					if (!next)
					{
						break;
					}

					flight.push_back(*next);
				}

				if (flight.size() >= 3)
				{
					ramps.emplace_back(make_stair_ramp(treads, flight, axis, direction), tread_tags[i]);
				}
			}

			return ramps;
		}

		struct mesh_shape
		{
			iw7::hkAabb domain{};
			std::vector<output_section> outputs;
			std::vector<convex_result> hulls;
			std::vector<std::uint16_t> hull_tags;
			std::vector<std::uint32_t> packed_vertices;
			std::vector<std::uint64_t> shared_vertices;
			std::vector<std::byte> simd_nodes;
			bool simd_compact{};
			std::uint64_t user_data{};
		};

		struct mesh_keys
		{
			std::uint32_t bits_per_key;
			std::uint32_t max_key;
			std::uint32_t triangle_count;
			std::uint32_t convex_count;
		};

		bounds to_bounds(const iw7::hkAabb& box)
		{
			return {{box.min.x, box.min.y, box.min.z}, {box.max.x, box.max.y, box.max.z}};
		}

		mesh_keys count_mesh_keys(const std::vector<output_section>& outputs)
		{
			const auto primitive_bits = 8u;
			mesh_keys keys{};
			keys.bits_per_key = static_cast<std::uint32_t>(std::bit_width(std::max<std::size_t>(outputs.size(), 1) - 1)) + primitive_bits;
			for (auto s = 0u; s < outputs.size(); s++)
			{
				const auto& output = outputs[s];
				for (auto p = 0u; p < output.primitives.size(); p++)
				{
					const auto key = (s << primitive_bits) | (p << 1);
					keys.max_key = std::max(keys.max_key, key);
					const auto& indices = output.primitives[p];
					if (indices[1] == indices[3] && indices[2] == 4)
					{
						keys.convex_count++;
					}
					else if (indices[2] == indices[3])
					{
						keys.triangle_count++;
					}
					else
					{
						keys.triangle_count += 2;
					}
				}
			}

			return keys;
		}

		std::vector<std::byte> make_inactive_simd_nodes(const tag_writer& writer, const std::uint32_t count)
		{
			const auto node_size = writer.type_size(writer.type_id("hkcdSimdTree::Node"));
			std::vector<std::byte> nodes(node_size * count, std::byte{0});
			const auto max_float = std::numeric_limits<float>::max();
			for (auto i = 0u; i < count; i++)
			{
				const auto base = i * node_size;
				for (auto box = 0u; box < 3; box++)
				{
					for (auto f = 0u; f < 4; f++)
					{
						put(nodes, base + box * 32 + f * 4, max_float);
						put(nodes, base + box * 32 + 16 + f * 4, -max_float);
					}
				}
			}

			return nodes;
		}

		tag_writer::reference write_mesh_shape(tag_writer& writer, mesh_shape& mesh, const mesh_keys& keys)
		{
			auto& outputs = mesh.outputs;
			const auto& hulls = mesh.hulls;
			const auto& hull_tags = mesh.hull_tags;
			const auto [bits_per_key, max_key, triangle_count, convex_count] = keys;

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
				encode_top(top_nodes, section_bounds, 0, to_bounds(mesh.domain), top_bytes, top_leaves);
			}

			for (auto s = 0u; s < outputs.size(); s++)
			{
				if (top_leaves[s] > 0xFFFF)
				{
					throw std::runtime_error("havok: the section tree is too large");
				}

				outputs[s].leaf_index = static_cast<std::uint16_t>(top_leaves[s]);
			}

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

			const auto mesh_packed_item = writer.add_array(t_uint32, mesh.packed_vertices.data(), mesh.packed_vertices.size());
			const auto mesh_shared_item = writer.add_array(t_uint64, mesh.shared_vertices.data(), mesh.shared_vertices.size());

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
			std::memcpy(data_bytes.data() + mesh_tree_offset + member(mesh_tree_type, "domain").offset, &mesh.domain,
				sizeof(mesh.domain));
			put(data_bytes, mesh_tree_offset + member(mesh_tree_type, "numPrimitiveKeys").offset,
				static_cast<std::int32_t>(triangle_count + convex_count));
			put(data_bytes, mesh_tree_offset + member(mesh_tree_type, "bitsPerKey").offset,
				static_cast<std::int32_t>(bits_per_key));
			put(data_bytes, mesh_tree_offset + member(mesh_tree_type, "maxKeyValue").offset, max_key);
			put(data_bytes, mesh_tree_offset + member(mesh_tree_type, "primitiveStoresIsFlatConvex").offset,
				static_cast<std::uint8_t>(0xFF));
			put(data_bytes, member(t_data, "hasSimdTree").offset, static_cast<std::uint8_t>(1));
			const auto& simd_tree_field = member(t_data, "simdTree");
			put(data_bytes, simd_tree_field.offset + member(simd_tree_field.type, "isCompact").offset, static_cast<std::uint8_t>(mesh.simd_compact));

			const auto simd_nodes_item = writer.add_array(t_simd_node, mesh.simd_nodes.data(), mesh.simd_nodes.size() / writer.type_size(t_simd_node));

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
			put_member(shape_bytes, t_shape, "userData", mesh.user_data);
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
			const auto& interior_words_field = member(storage_type, "words");
			writer.set_pointer(shape_offset.offset + interior_field.offset + storage_field.offset
				+ interior_words_field.offset, interior_words_field.type, interior_words_item.item);

			return shape_offset;
		}
		struct simd_item
		{
			bounds box;
			std::uint32_t key;
		};

		void put_simd_child(std::vector<std::byte>& nodes, const std::size_t base, const std::uint32_t slot, const bounds& box,
			const std::uint32_t data)
		{
			for (auto axis = 0u; axis < 3; axis++)
			{
				put(nodes, base + axis * 32 + slot * 4, box.min[axis]);
				put(nodes, base + axis * 32 + 16 + slot * 4, box.max[axis]);
			}

			put(nodes, base + 96 + slot * 4, data);
		}

		std::uint32_t add_simd_node(std::vector<std::byte>& nodes, const std::span<simd_item> items, const bool leaf)
		{
			const auto node_size = 128u;
			const auto index = static_cast<std::uint32_t>(nodes.size() / node_size);
			const auto base = static_cast<std::size_t>(index) * node_size;
			nodes.resize(nodes.size() + node_size, std::byte{0});
			put(nodes, base + 112, static_cast<std::uint8_t>(leaf));

			if (leaf)
			{
				for (auto slot = 0u; slot < 4; slot++)
				{
					if (slot < items.size())
					{
						put_simd_child(nodes, base, slot, items[slot].box, items[slot].key);
					}
					else
					{
						put_simd_child(nodes, base, slot, empty_bounds(), 0xFFFFFFFF);
					}
				}

				return index;
			}

			auto spread = empty_bounds();
			for (const auto& item : items)
			{
				include(spread, center_of(item.box));
			}

			auto axis = 0;
			for (auto a = 1; a < 3; a++)
			{
				if (spread.max[a] - spread.min[a] > spread.max[axis] - spread.min[axis])
				{
					axis = a;
				}
			}

			std::ranges::stable_sort(items, [&](const simd_item& a, const simd_item& b)
			{
				return a.box.min[axis] + a.box.max[axis] < b.box.min[axis] + b.box.max[axis];
			});

			std::size_t child_size = 4;
			while (child_size * 4 < items.size())
			{
				child_size *= 4;
			}

			for (auto slot = 0u; slot < 4; slot++)
			{
				const auto first = slot * child_size;
				if (first >= items.size())
				{
					put_simd_child(nodes, base, slot, empty_bounds(), 0);
					continue;
				}

				const auto child_items = items.subspan(first, std::min(child_size, items.size() - first));
				auto box = empty_bounds();
				for (const auto& item : child_items)
				{
					include(box, {item.box.min[0], item.box.min[1], item.box.min[2]});
					include(box, {item.box.max[0], item.box.max[1], item.box.max[2]});
				}

				const auto child = add_simd_node(nodes, child_items, child_items.size() <= 4);
				put_simd_child(nodes, base, slot, box, child);
			}

			return index;
		}

		std::vector<std::byte> make_simd_tree(std::vector<simd_item> items)
		{
			std::vector<std::byte> nodes(128, std::byte{0});
			for (auto slot = 0u; slot < 4; slot++)
			{
				put_simd_child(nodes, 0, slot, empty_bounds(), 0);
			}

			if (!items.empty())
			{
				add_simd_node(nodes, items, false);
			}

			return nodes;
		}

		std::vector<vec3> read_instance_points(const iw7::hknpShapeInstance& instance)
		{
			const auto* shape = static_cast<const iw7::hknpConvexPolytopeShape*>(instance.shape);
			std::vector<vec3> points;
			for (auto i = 0u; i < shape->vertices.size; i++)
			{
				const auto& vertex = shape->vertices.data()[i];
				points.push_back(to_vec3(&instance.rotation[0].x) * (vertex.x * instance.scale.x)
					+ to_vec3(&instance.rotation[1].x) * (vertex.y * instance.scale.y)
					+ to_vec3(&instance.rotation[2].x) * (vertex.z * instance.scale.z) + to_vec3(&instance.translation.x));
			}

			return points;
		}

		mesh_shape convert_compound_shape(const iw7::havok::binary::packfile_reader& reader, const iw7::hknpDynamicCompoundShape* shape,
			const std::size_t world_tag_count, const std::size_t ramp_tag_count)
		{
			mesh_shape mesh{};
			mesh.simd_compact = true;
			mesh.user_data = shape->userData;

			auto domain = empty_bounds();
			std::vector<bounds> hull_bounds;
			for (auto i = 0; i < shape->instances.size; i++)
			{
				const auto& instance = shape->instances.data[i];
				if (!instance.shape || reader.class_name(instance.shape) != "hknpConvexPolytopeShape")
				{
					throw std::runtime_error("havok: a map_ents shape instance is not a convex polytope");
				}

				auto hull = make_convex_hull(read_instance_points(instance));
				auto box = empty_bounds();
				for (const auto& vertex : hull.vertices)
				{
					include(box, vertex);
					include(domain, vertex);
				}

				hull_bounds.push_back(box);
				mesh.hulls.push_back(std::move(hull));
				const auto is_entity_tag = instance.shapeTag >= world_tag_count && instance.shapeTag != 0xFFFF;
				mesh.hull_tags.push_back(static_cast<std::uint16_t>(instance.shapeTag + (is_entity_tag ? ramp_tag_count : 0)));
			}

			if (mesh.hulls.empty())
			{
				throw std::runtime_error("havok: a map_ents shape has no convex piece");
			}

			mesh.domain.min = {domain.min[0], domain.min[1], domain.min[2], 0.0f};
			mesh.domain.max = {domain.max[0], domain.max[1], domain.max[2], 0.0f};

			std::vector<simd_item> simd_items;
			const auto max_float = std::numeric_limits<float>::max();
			const auto infinity = std::numeric_limits<float>::infinity();
			for (auto begin = 0u; begin < mesh.hulls.size(); begin += 64)
			{
				const auto end = std::min<std::size_t>(begin + 64, mesh.hulls.size());
				output_section section{};
				section.codec = {max_float, max_float, max_float, -infinity, -infinity, -infinity};
				section.flags = 1;

				std::vector<bounds> section_bounds;
				std::vector<std::uint16_t> section_tags;
				for (auto hull = begin; hull < end; hull++)
				{
					const auto primitive = static_cast<std::uint32_t>(hull - begin);
					const auto local = static_cast<std::uint8_t>(primitive * 4);
					section.table.push_back(146);
					section.table.push_back(0);
					section.table.push_back(static_cast<std::uint16_t>(hull));
					section.table.push_back(static_cast<std::uint16_t>(hull >> 16));
					section.primitives.push_back({local, local, 4, local});
					section_bounds.push_back(hull_bounds[hull]);
					section_tags.push_back(mesh.hull_tags[hull]);
					simd_items.push_back({hull_bounds[hull], static_cast<std::uint32_t>(mesh.outputs.size() << 8 | primitive << 1)});
				}

				section.runs = make_runs(section_tags);
				build_section_tree(section, section_bounds);
				mesh.outputs.push_back(std::move(section));
			}

			mesh.simd_nodes = make_simd_tree(std::move(simd_items));
			return mesh;
		}

		std::vector<source_section> read_source_sections(const iw7::hknpCompressedMeshShapeTree& tree)
		{
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

			return sources;
		}

		std::vector<convex_result> read_convex_hulls(const iw7::hknpCompressedMeshShapeTree& tree, std::vector<source_section>& sources,
			std::vector<std::uint16_t>& hull_tags)
		{
			std::vector<convex_result> hulls;
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

			return hulls;
		}

		std::vector<std::uint16_t> find_ramp_source_tags(const std::vector<std::pair<convex_result, std::uint16_t>>& ramps)
		{
			std::vector<std::uint16_t> source_tags;
			for (const auto& [ramp, source_tag] : ramps)
			{
				if (std::ranges::find(source_tags, source_tag) == source_tags.end())
				{
					source_tags.push_back(source_tag);
				}
			}

			return source_tags;
		}

		std::vector<std::uint16_t> find_world_ramp_source_tags(const iw7::HavokPhysicsShapeList* list)
		{
			const auto& tree = read_world_mesh(list);
			auto sources = read_source_sections(tree);
			std::vector<std::uint16_t> hull_tags;
			const auto hulls = read_convex_hulls(tree, sources, hull_tags);
			return find_ramp_source_tags(make_stair_ramps(hulls, hull_tags, list));
		}

		std::vector<iw7::HavokPhysicsShapeTagData> make_ramp_tags(const std::vector<iw7::HavokPhysicsShapeTagData>& tags,
			const std::vector<std::uint16_t>& ramp_source_tags)
		{
			std::vector<iw7::HavokPhysicsShapeTagData> ramp_tags;
			for (const auto source_tag : ramp_source_tags)
			{
				auto& ramp_tag = ramp_tags.emplace_back(tags[source_tag]);
				ramp_tag.collisionFilterInfo = zonetool::iw8::PHYSICS_CONTENTS_PLAYERCLIP | zonetool::iw8::PHYSICS_CONTENTS_AICLIP
					| zonetool::iw8::PHYSICS_CONTENTS_VEHICLECLIP;
				ramp_tag.userData = (ramp_tag.userData & (0x3Full << 19)) | zonetool::iw8::SURFACE_FLAG_STAIRS;
			}

			return ramp_tags;
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

		auto sources = read_source_sections(tree);
		std::vector<std::uint16_t> hull_tags;
		auto hulls = read_convex_hulls(tree, sources, hull_tags);

		auto ramps = make_stair_ramps(hulls, hull_tags, list);
		const auto ramp_source_tags = find_ramp_source_tags(ramps);
		std::vector<std::uint32_t> ramp_hulls;
		for (auto& [ramp, source_tag] : ramps)
		{
			const auto slot = std::ranges::find(ramp_source_tags, source_tag) - ramp_source_tags.begin();
			ramp_hulls.push_back(static_cast<std::uint32_t>(hulls.size()));
			hulls.push_back(std::move(ramp));
			hull_tags.push_back(static_cast<std::uint16_t>(list->shapeTagData.size + slot));
		}

		ZONETOOL_INFO("havok: %zu stair ramps", ramp_hulls.size());

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

		const auto& ramp_template = sources.front();
		for (auto begin = 0u; begin < ramp_hulls.size(); begin += 64)
		{
			const auto end = std::min<std::size_t>(begin + 64, ramp_hulls.size());
			output_section ramp_section{};
			ramp_section.codec = ramp_template.codec;
			ramp_section.page = ramp_template.page;
			ramp_section.layer_data = ramp_template.layer_data;
			ramp_section.flags = ramp_template.flags;

			std::vector<bounds> ramp_bounds;
			std::vector<std::uint16_t> ramp_tags;
			for (auto k = begin; k < end; k++)
			{
				const auto hull = ramp_hulls[k];
				const auto local = static_cast<std::uint8_t>((k - begin) * 4);
				ramp_section.table.push_back(146);
				ramp_section.table.push_back(0);
				ramp_section.table.push_back(static_cast<std::uint16_t>(hull));
				ramp_section.table.push_back(static_cast<std::uint16_t>(hull >> 16));
				ramp_section.primitives.push_back({local, local, 4, local});

				auto box = empty_bounds();
				for (const auto& vertex : hulls[hull].vertices)
				{
					include(box, vertex);
				}
				ramp_bounds.push_back(box);
				ramp_tags.push_back(hull_tags[hull]);
			}

			ramp_section.runs = make_runs(ramp_tags);
			build_section_tree(ramp_section, ramp_bounds);
			outputs.push_back(std::move(ramp_section));
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

		const auto keys = count_mesh_keys(outputs);

		tag_writer writer(schema());
		const auto t_list = writer.type_id("HavokPhysicsShapeList");
		const auto t_tag = writer.type_id("HavokPhysicsShapeList::ShapeTagData");
		const auto t_vector4 = writer.type_id("hkVector4");
		const auto t_int = writer.type_id("int");
		const auto t_char = writer.type_id("char");
		const auto t_pointer = writer.type_id("T*");
		const auto t_string = writer.type_id("hkStringPtr");

		auto list_bytes = std::vector<std::byte>(writer.type_size(t_list), std::byte{0});
		const auto list_offset = writer.add_object(t_list, list_bytes.data());

		auto tags = convert_shape_tags(list);
		const auto ramp_tags = make_ramp_tags(tags, ramp_source_tags);
		tags.insert(tags.end(), ramp_tags.begin(), ramp_tags.end());

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
		const auto tri_counts_item = writer.add_array(t_int, &keys.triangle_count, 1);
		const auto contents_item = writer.add_array(t_int, &world_contents, 1);
		const auto convex_counts_item = writer.add_array(t_int, &keys.convex_count, 1);

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

		mesh_shape mesh{};
		mesh.domain = tree.domain;
		mesh.outputs = std::move(outputs);
		mesh.hulls = std::move(hulls);
		mesh.hull_tags = std::move(hull_tags);
		mesh.packed_vertices.assign(tree.packedVertices.data, tree.packedVertices.data + tree.packedVertices.size);
		mesh.shared_vertices.assign(tree.sharedVertices.data, tree.sharedVertices.data + tree.sharedVertices.size);
		mesh.simd_nodes = make_inactive_simd_nodes(writer, 3238);
		mesh.user_data = shape->userData;
		writer.set_pointer(shapes_item.offset, t_pointer, write_mesh_shape(writer, mesh, keys).item);

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

	std::string convert_ents_shape_list(const std::string_view packfile, const std::string_view world_packfile)
	{
		const iw7::havok::binary::packfile_reader reader(packfile);
		const auto* list = read_shape_list(reader);
		const iw7::havok::binary::packfile_reader world_reader(world_packfile);
		const auto* world_list = read_shape_list(world_reader);

		auto tags = convert_shape_tags(world_list);
		const auto world_tag_count = tags.size();
		const auto ramp_tags = make_ramp_tags(tags, find_world_ramp_source_tags(world_list));
		tags.insert(tags.end(), ramp_tags.begin(), ramp_tags.end());
		const auto entity_tags = convert_shape_tags(list);
		if (entity_tags.size() > world_tag_count)
		{
			tags.insert(tags.end(), entity_tags.begin() + world_tag_count, entity_tags.end());
		}

		const auto shape_count = static_cast<std::size_t>(std::max(list->shapes.size, 0));

		tag_writer writer(schema());
		const auto t_list = writer.type_id("HavokPhysicsShapeList");
		const auto t_tag = writer.type_id("HavokPhysicsShapeList::ShapeTagData");
		const auto t_int = writer.type_id("int");
		const auto t_char = writer.type_id("char");
		const auto t_pointer = writer.type_id("T*");
		const auto t_string = writer.type_id("hkStringPtr");
		const auto t_vector4 = writer.type_id("hkVector4");

		auto list_bytes = std::vector<std::byte>(writer.type_size(t_list), std::byte{0});
		const auto list_offset = writer.add_object(t_list, list_bytes.data());

		std::vector<mesh_shape> meshes;
		std::vector<mesh_keys> keys;
		std::vector<std::int32_t> shape_indices;
		std::vector<std::int32_t> vert_counts;
		std::vector<std::int32_t> convex_counts;
		std::vector<std::uint32_t> contents;
		std::vector<std::array<float, 4>> min_maxes;
		for (auto i = 0u; i < shape_count; i++)
		{
			const auto* shape = list->shapes.data[i];
			if (!shape || reader.class_name(shape) != "hknpDynamicCompoundShape")
			{
				throw std::runtime_error(std::format("havok: map_ents shape {} is not a dynamic compound shape", i));
			}

			auto& mesh = meshes.emplace_back(convert_compound_shape(reader, static_cast<const iw7::hknpDynamicCompoundShape*>(shape),
				world_tag_count, ramp_tags.size()));
			auto& mesh_keys = keys.emplace_back(count_mesh_keys(mesh.outputs));
			mesh_keys.bits_per_key = std::max(std::bit_width(mesh_keys.max_key), 1);
			shape_indices.push_back(static_cast<std::int32_t>(i));
			convex_counts.push_back(static_cast<std::int32_t>(mesh.hulls.size()));

			auto& vert_count = vert_counts.emplace_back(0);
			auto& shape_contents = contents.emplace_back(0u);
			for (auto hull = 0u; hull < mesh.hulls.size(); hull++)
			{
				vert_count += static_cast<std::int32_t>(mesh.hulls[hull].vertices.size());
				if (mesh.hull_tags[hull] < tags.size())
				{
					shape_contents |= tags[mesh.hull_tags[hull]].collisionFilterInfo;
				}
			}

			min_maxes.push_back({mesh.domain.min.x, mesh.domain.min.y, mesh.domain.min.z, 0.0f});
			min_maxes.push_back({mesh.domain.max.x, mesh.domain.max.y, mesh.domain.max.z, 0.0f});
		}

		const std::vector<std::uint64_t> null_pointers(shape_count, 0);
		const std::vector<std::int32_t> tri_counts(shape_count, 0);
		const auto names_item = writer.add_array(t_string, null_pointers.data(), shape_count);
		for (auto i = 0u; i < shape_count; i++)
		{
			const std::string name = i < static_cast<std::uint32_t>(list->shapeNames.size) && list->shapeNames.data[i]
				? list->shapeNames.data[i]
				: std::format("entity {}", i);
			writer.set_pointer(names_item.offset + i * 8, t_string, writer.add_array(t_char, name.data(), name.size() + 1).item);
		}

		const auto shapes_item = writer.add_array(t_pointer, null_pointers.data(), shape_count);
		const auto indices_item = writer.add_array(t_int, shape_indices.data(), shape_count);
		const auto vert_counts_item = writer.add_array(t_int, vert_counts.data(), shape_count);
		const auto tri_counts_item = writer.add_array(t_int, tri_counts.data(), shape_count);
		const auto contents_item = writer.add_array(t_int, contents.data(), shape_count);
		const auto convex_counts_item = writer.add_array(t_int, convex_counts.data(), shape_count);
		const auto min_maxes_item = writer.add_array(t_vector4, min_maxes.data(), min_maxes.size());
		const auto tag_item = writer.add_array(t_tag, tags.data(), tags.size());

		for (auto i = 0u; i < shape_count; i++)
		{
			writer.set_pointer(shapes_item.offset + i * 8, t_pointer, write_mesh_shape(writer, meshes[i], keys[i]).item);
		}

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

	namespace
	{
		float get_body_mass(const iw7::hknpPhysicsSystemData* system, const iw7::hknpBodyCinfo& body)
		{
			if (body.reservedMotionId >= static_cast<unsigned int>(system->motionCinfos.size))
			{
				return 0.0f;
			}

			const auto& motion = system->motionCinfos.data[body.reservedMotionId];
			return motion.inverseMass > 0.0f ? 1.0f / motion.inverseMass : body.mass;
		}

		bool is_polytope_body(const iw7::havok::binary::packfile_reader& reader, const iw7::hknpBodyCinfo& body)
		{
			return body.shape && reader.class_name(body.shape) == "hknpConvexPolytopeShape";
		}
	}

	std::optional<physics_asset> convert_physics_asset(const std::string_view packfile, const std::string& name)
	{
		const iw7::havok::binary::packfile_reader reader(packfile);
		if (reader.root_class_name() != "HavokPhysicsAsset")
		{
			return {};
		}

		const auto* source = reader.root<iw7::HavokPhysicsAsset>();
		const auto* system = source->physicsSystemData;
		if (!system || system->bodyCinfos.size < 1)
		{
			return {};
		}

		const auto body_count = static_cast<std::size_t>(system->bodyCinfos.size);
		const std::span bodies(system->bodyCinfos.data, body_count);
		if (!std::ranges::all_of(bodies, [&](const iw7::hknpBodyCinfo& body) { return is_polytope_body(reader, body); }))
		{
			return {};
		}

		const auto dynamic_count = static_cast<std::size_t>(std::ranges::count_if(bodies, [&](const iw7::hknpBodyCinfo& body)
		{
			return body.reservedMotionId < static_cast<unsigned int>(system->motionCinfos.size);
		}));
		if (dynamic_count && dynamic_count != body_count)
		{
			return {};
		}

		physics_asset result{};
		result.dynamic = dynamic_count > 0;

		if (system->constraintCinfos.size > 0)
		{
			ZONETOOL_WARNING("physicsasset \"%s\": %d constraints are not converted", name.data(), system->constraintCinfos.size);
			return {};
		}

		const auto& types = physics_asset_schema();
		const auto field = [&](const std::uint32_t type, const std::string_view member_name) -> const tag_member&
		{
			return member(types, type, member_name);
		};
		const auto put_field = [&](std::vector<std::byte>& object, const std::size_t base, const std::uint32_t type,
			const std::string_view member_name, const auto& value)
		{
			put(object, base + field(type, member_name).offset, value);
		};

		tag_writer writer(types);
		writer.use_compendium(0xC16B2E49324C3540, physics_asset_compendium_types()); // physicsglobaltypecompendium
		const auto t_asset = writer.type_id("HavokPhysicsAsset");
		const auto t_system = writer.type_id("hknpPhysicsSystemData");
		const auto t_body = writer.type_id("hknpPhysicsSystemData::bodyCinfoWithAttachment");
		const auto t_hull = writer.type_id("hknpConvexPolytopeShape");
		const auto t_face = writer.type_id("hknpConvexPolytopeShape::Face");
		const auto t_connectivity = writer.type_id("hknpConvexPolytopeShape::Connectivity");
		const auto t_edge = writer.type_id("hknpConvexPolytopeShape::Connectivity::Edge");
		const auto t_properties = writer.type_id("hkRefCountedProperties");
		const auto t_entry = writer.type_id("hkRefCountedProperties::Entry");
		const auto t_mass = writer.type_id("hknpShapeMassProperties");
		const auto t_vector4f = writer.type_id("hkVector4f");
		const auto t_vector4 = writer.type_id("hkVector4");
		const auto t_uint8 = writer.type_id("hkUint8");
		const auto t_int = writer.type_id("int");
		const auto t_char = writer.type_id("char");
		const auto t_string = writer.type_id("hkStringPtr");

		std::vector<std::byte> asset_bytes(writer.type_size(t_asset), std::byte{0});
		put_field(asset_bytes, 0, t_asset, "useCategory",
			result.dynamic ? zonetool::iw8::PhysicsAssetUseCategory_Dynent : zonetool::iw8::PhysicsAssetUseCategory_Custom);
		const auto asset_offset = writer.add_object(t_asset, asset_bytes.data());

		const auto set_field = [&](const tag_writer::reference& object, const std::size_t base, const std::uint32_t type,
			const std::string_view member_name, const std::uint32_t item)
		{
			const auto& target_field = field(type, member_name);
			writer.set_pointer(object.offset + base + target_field.offset, target_field.type, item);
		};

		const auto add_string = [&](const std::string_view text)
		{
			const std::string value(text);
			return writer.add_array(t_char, value.data(), value.size() + 1).item;
		};

		const auto set_int_array = [&](const std::string_view member_name, const std::vector<std::int32_t>& values)
		{
			if (!values.empty())
			{
				set_field(asset_offset, 0, t_asset, member_name, writer.add_array(t_int, values.data(), values.size()).item);
			}
		};

		const auto copy_int_array = [](const iw7::hkArray<int>& array)
		{
			return std::vector<std::int32_t>(array.data, array.data + std::max(array.size, 0));
		};

		set_int_array("bodyQualityNameCRCLookup", copy_int_array(source->bodyQualityNameCRCLookup));
		set_int_array("materialNameCRCLookup", copy_int_array(source->materialNameCRCLookup));
		set_int_array("motionPropertiesNameCRCLookup", copy_int_array(source->motionPropertiesNameCRCLookup));

		const std::vector<std::uint64_t> null_names(body_count, 0);
		set_field(asset_offset, 0, t_asset, "bodySFXAssetNames", writer.add_array(t_string, null_names.data(), body_count).item);
		set_field(asset_offset, 0, t_asset, "bodyVFXAssetNames", writer.add_array(t_string, null_names.data(), body_count).item);
		set_int_array("bodyServerUsage", std::vector<std::int32_t>(body_count, 0));
		set_int_array("bodyDrivers", std::vector<std::int32_t>(body_count, 0));
		set_int_array("simulationCategories", std::vector<std::int32_t>(body_count, result.dynamic
			? zonetool::iw8::PhysicsAssetSimulationCategory_DynamicEntity
			: zonetool::iw8::PhysicsAssetSimulationCategory_Custom));

		std::vector<std::byte> system_bytes(writer.type_size(t_system), std::byte{0});
		put_field(system_bytes, 0, t_system, "microStepMultiplier", static_cast<std::uint8_t>(1));
		const auto system_offset = writer.add_object(t_system, system_bytes.data());
		set_field(asset_offset, 0, t_asset, "physicsSystemData", system_offset.item);
		set_field(system_offset, 0, t_system, "name", add_string(system->name ? system->name : "Default Physics System Data"));

		const auto body_size = writer.type_size(t_body);
		std::vector<std::byte> body_bytes(body_size * body_count, std::byte{0});
		for (auto i = 0u; i < body_count; i++)
		{
			const auto& body = bodies[i];
			const auto base = i * body_size;
			auto contents = convert_contents(body.collisionFilterInfo);
			if (result.dynamic && contents & zonetool::iw8::PHYSICS_CONTENTS_AINOSIGHT)
			{
				contents |= zonetool::iw8::PHYSICS_CONTENTS_PLAYERNOSIGHT; // stock dynamic bodies have both
			}

			result.body_contents.push_back(contents);

			put_field(body_bytes, base, t_body, "flags", static_cast<std::int32_t>(body.flags));
			put_field(body_bytes, base, t_body, "collisionFilterInfo", contents);
			put_field(body_bytes, base, t_body, "materialId", body.materialId);
			put_field(body_bytes, base, t_body, "qualityId", body.qualityId);
			put_field(body_bytes, base, t_body, "userData", body.userData);
			put_field(body_bytes, base, t_body, "motionType", static_cast<std::uint8_t>(result.dynamic ? 2 : 1));
			put_field(body_bytes, base, t_body, "position", body.position);
			put_field(body_bytes, base, t_body, "orientation", body.orientation);
			put_field(body_bytes, base, t_body, "linearVelocity", body.linearVelocity);
			put_field(body_bytes, base, t_body, "angularVelocity", body.angularVelocity);
			put_field(body_bytes, base, t_body, "mass", get_body_mass(system, body));
			put_field(body_bytes, base, t_body, "motionPropertiesId", body.motionPropertiesId);
			put_field(body_bytes, base, t_body, "desiredBodyId", static_cast<std::uint32_t>(0x00FFFFFF));
			put_field(body_bytes, base, t_body, "motionId", static_cast<std::uint32_t>(0x7FFFFFFF));
			put_field(body_bytes, base, t_body, "collisionLookAheadDistance", body.collisionLookAheadDistance);
			put_field(body_bytes, base, t_body, "activationPriority", static_cast<std::int8_t>(5));
			put_field(body_bytes, base, t_body, "attachedBody", static_cast<std::int32_t>(-1));
		}

		const auto bodies_offset = writer.add_array(t_body, body_bytes.data(), body_count);
		set_field(system_offset, 0, t_system, "bodyCinfos", bodies_offset.item);

		for (auto i = 0u; i < body_count; i++)
		{
			const auto& body = bodies[i];
			const auto* shape = static_cast<const iw7::hknpConvexPolytopeShape*>(body.shape);
			const auto base = i * body_size;
			set_field(bodies_offset, base, t_body, "name", add_string(body.name ? body.name : "tag_origin"));

			std::vector<std::byte> hull_bytes(writer.type_size(t_hull), std::byte{0});
			put_field(hull_bytes, 0, t_hull, "flags", static_cast<std::uint16_t>(result.dynamic ? 0x0943 : 0x01C3));
			put_field(hull_bytes, 0, t_hull, "type", static_cast<std::uint8_t>(1));
			put_field(hull_bytes, 0, t_hull, "numShapeKeyBits", shape->numShapeKeyBits);
			put_field(hull_bytes, 0, t_hull, "dispatchType", static_cast<std::uint8_t>(2));
			put_field(hull_bytes, 0, t_hull, "convexRadius", shape->convexRadius);
			put_field(hull_bytes, 0, t_hull, "userData", convert_user_data(shape->userData));
			put_field(hull_bytes, 0, t_hull, "maxAllowedPenetration", static_cast<std::uint16_t>(0x3EC6));
			const auto hull_offset = writer.add_object(t_hull, hull_bytes.data());
			set_field(bodies_offset, base, t_body, "shape", hull_offset.item);

			set_field(hull_offset, 0, t_hull, "vertices", writer.add_array(t_vector4f, shape->vertices.data(), shape->vertices.size).item);
			set_field(hull_offset, 0, t_hull, "planes", writer.add_array(t_vector4, shape->planes.data(), shape->planes.size).item);
			set_field(hull_offset, 0, t_hull, "faces", writer.add_array(t_face, shape->faces.data(), shape->faces.size).item);
			set_field(hull_offset, 0, t_hull, "indices", writer.add_array(t_uint8, shape->indices.data(), shape->indices.size).item);

			if (const auto* connectivity = shape->connectivity)
			{
				const auto vertex_edges_item = writer.add_array(t_edge, connectivity->vertexEdges.data, connectivity->vertexEdges.size).item;
				const auto face_links_item = writer.add_array(t_edge, connectivity->faceLinks.data, connectivity->faceLinks.size).item;
				std::vector<std::byte> connectivity_bytes(writer.type_size(t_connectivity), std::byte{0});
				const auto connectivity_offset = writer.add_object(t_connectivity, connectivity_bytes.data());
				set_field(connectivity_offset, 0, t_connectivity, "vertexEdges", vertex_edges_item);
				set_field(connectivity_offset, 0, t_connectivity, "faceLinks", face_links_item);
				set_field(hull_offset, 0, t_hull, "connectivity", connectivity_offset.item);
			}

			if (!shape->properties || shape->properties->entries.size < 1)
			{
				continue;
			}

			const std::span entries(shape->properties->entries.data, static_cast<std::size_t>(shape->properties->entries.size));
			const auto mass_entry = std::ranges::find_if(entries, [](const iw7::hkRefCountedPropertiesEntry& entry)
			{
				return entry.object && entry.key == 0xF100;
			});
			if (mass_entry == entries.end())
			{
				continue;
			}

			const auto* mass = static_cast<const iw7::hknpShapeMassProperties*>(mass_entry->object);
			std::vector<std::byte> mass_bytes(writer.type_size(t_mass), std::byte{0});
			put_field(mass_bytes, 0, t_mass, "compressedMassProperties", mass->compressedMassProperties);
			const auto mass_offset = writer.add_object(t_mass, mass_bytes.data());

			std::vector<std::byte> entry_bytes(writer.type_size(t_entry), std::byte{0});
			put_field(entry_bytes, 0, t_entry, "key", mass_entry->key);
			put_field(entry_bytes, 0, t_entry, "flags", mass_entry->flags);
			const auto entry_offset = writer.add_array(t_entry, entry_bytes.data(), 1);
			set_field(entry_offset, 0, t_entry, "object", mass_offset.item);

			std::vector<std::byte> properties_bytes(writer.type_size(t_properties), std::byte{0});
			const auto properties_offset = writer.add_object(t_properties, properties_bytes.data());
			set_field(properties_offset, 0, t_properties, "entries", entry_offset.item);
			set_field(hull_offset, 0, t_hull, "properties", properties_offset.item);
		}

		result.havok_data = writer.finish(asset_offset.item);
		return result;
	}
}
