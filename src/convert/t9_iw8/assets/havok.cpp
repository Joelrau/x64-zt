#include <std_include.hpp>

#include "../include.hpp"
#include "xmodel.hpp"
#include "gfxworld.hpp"
#include "mapents.hpp"
#include "havok.hpp"
#include "havok_schema.hpp"

#include <formats/havok_tagfile.hpp>

namespace zonetool::t9::converter::iw8::havok
{
	namespace
	{
		using tag_writer = ::havok::tagfile::writer;
		using tag_member = ::havok::tagfile::type_member;

		const tag_member& member(const std::uint32_t type, const std::string_view name)
		{
			const auto& types = schema();
			for (auto current = type; current; current = types.at(current).parent)
			{
				for (const auto& field : types.at(current).members)
				{
					if (field.name == name)
					{
						return field;
					}
				}
			}

			throw std::runtime_error(std::format("havok schema: type {} has no member {}", type, name));
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

		void include(bounds& box, const point& value)
		{
			for (auto axis = 0; axis < 3; axis++)
			{
				box.min[axis] = std::min(box.min[axis], value[axis]);
				box.max[axis] = std::max(box.max[axis], value[axis]);
			}
		}

		void include(bounds& box, const bounds& other)
		{
			include(box, point{other.min[0], other.min[1], other.min[2]});
			include(box, point{other.max[0], other.max[1], other.max[2]});
		}

		point center_of(const bounds& box)
		{
			return {(box.min[0] + box.max[0]) * 0.5f, (box.min[1] + box.max[1]) * 0.5f, (box.min[2] + box.max[2]) * 0.5f};
		}

		std::uint32_t split_top(const std::vector<point>& centres, std::span<std::uint32_t> items, std::vector<top_node>& nodes)
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
				const auto va = centres[a][axis];
				const auto vb = centres[b][axis];
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
			std::array<std::uint8_t, 3> xyz{};
			for (auto axis = 0; axis < 3; axis++)
			{
				const auto step = (parent.max[axis] - parent.min[axis]) / 226.0f;
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

		bounds subtree_bounds(const std::vector<top_node>& nodes, const std::vector<bounds>& leaf_bounds, const std::uint32_t node)
		{
			if (nodes[node].leaf)
			{
				return leaf_bounds[nodes[node].value];
			}

			auto result = subtree_bounds(nodes, leaf_bounds, node + 1);
			include(result, subtree_bounds(nodes, leaf_bounds, node + 2 * nodes[node].value));
			return result;
		}

		void encode_top(const std::vector<top_node>& nodes, const std::vector<bounds>& section_bounds, const std::uint32_t node,
			const bounds& parent, std::vector<std::array<std::uint8_t, 5>>& out, std::vector<std::uint32_t>& leaves)
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

		void encode_section(const std::vector<top_node>& nodes, const std::vector<bounds>& primitive_bounds, const std::uint32_t node,
			const bounds& parent, std::vector<std::array<std::uint8_t, 4>>& out)
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
			output.domain = empty_bounds();
			std::vector<point> centres;
			for (const auto& box : primitive_bounds)
			{
				include(output.domain, box);
				centres.push_back(center_of(box));
			}

			std::vector<std::uint32_t> items(primitive_bounds.size());
			std::iota(items.begin(), items.end(), 0u);
			std::vector<top_node> nodes;
			split_top(centres, items, nodes);

			output.nodes.resize(nodes.size());
			encode_section(nodes, primitive_bounds, 0, output.domain, output.nodes);
		}

		bool is_degenerate(const grid_triangle& corners)
		{
			std::int64_t u[3]{};
			std::int64_t v[3]{};
			for (auto axis = 0; axis < 3; axis++)
			{
				u[axis] = static_cast<std::int64_t>(corners[1][axis]) - corners[0][axis];
				v[axis] = static_cast<std::int64_t>(corners[2][axis]) - corners[0][axis];
			}

			return u[1] * v[2] == u[2] * v[1] && u[2] * v[0] == u[0] * v[2] && u[0] * v[1] == u[1] * v[0];
		}

		grid_point snap_to_grid(const point& position, const float grid)
		{
			return {static_cast<std::int32_t>(std::lround(position[0] / grid)), static_cast<std::int32_t>(std::lround(position[1] / grid)),
				static_cast<std::int32_t>(std::lround(position[2] / grid))};
		}

		grid_triangle rotate_lowest_first(const grid_triangle& corners)
		{
			const auto lowest = std::ranges::min_element(corners) - corners.begin();
			return {corners[lowest], corners[(lowest + 1) % 3], corners[(lowest + 2) % 3]};
		}

		void add_snapped_triangle(const grid_triangle& corners, std::vector<grid_triangle>& triangles)
		{
			if (!is_degenerate(corners))
			{
				triangles.push_back(rotate_lowest_first(corners));
			}
		}

		dvec3 subtract(const dvec3& a, const dvec3& b)
		{
			return {a[0] - b[0], a[1] - b[1], a[2] - b[2]};
		}

		dvec3 cross(const dvec3& a, const dvec3& b)
		{
			return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
		}

		double dot(const dvec3& a, const dvec3& b)
		{
			return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
		}

		dvec3 normalized(const dvec3& value)
		{
			const auto length = std::sqrt(dot(value, value));
			return {value[0] / length, value[1] / length, value[2] / length};
		}

		bool has_plane(const std::vector<hull_plane>& planes, const hull_plane& plane, const double epsilon)
		{
			return std::ranges::any_of(planes, [&](const hull_plane& other)
			{
				return dot(other.normal, plane.normal) > 1.0 - 1e-6 && std::abs(other.distance - plane.distance) < epsilon;
			});
		}

		std::vector<hull_plane> find_hull_planes(const std::vector<dvec3>& points, const double epsilon, const double min_area)
		{
			std::vector<hull_plane> planes;
			for (auto i = 0u; i < points.size(); i++)
			{
				for (auto j = i + 1; j < points.size(); j++)
				{
					for (auto k = j + 1; k < points.size(); k++)
					{
						const auto normal = cross(subtract(points[j], points[i]), subtract(points[k], points[i]));
						if (dot(normal, normal) < min_area * min_area)
						{
							continue;
						}

						hull_plane plane{normalized(normal)};
						plane.distance = dot(plane.normal, points[i]);

						auto above = false;
						auto below = false;
						for (const auto& point : points)
						{
							const auto side = dot(plane.normal, point) - plane.distance;
							above |= side > epsilon;
							below |= side < -epsilon;
						}

						if (above && below)
						{
							continue;
						}

						if (above)
						{
							plane.normal = {-plane.normal[0], -plane.normal[1], -plane.normal[2]};
							plane.distance = -plane.distance;
						}

						if (!has_plane(planes, plane, epsilon))
						{
							planes.push_back(plane);
						}
					}
				}
			}

			return planes;
		}

		void add_hull_face(const std::vector<dvec3>& points, const hull_plane& plane, const double epsilon,
			std::vector<gfxworld::collision_triangle>& triangles)
		{
			std::vector<dvec3> face;
			dvec3 centre{};
			for (const auto& point : points)
			{
				if (std::abs(dot(plane.normal, point) - plane.distance) > epsilon)
				{
					continue;
				}

				face.push_back(point);
				for (auto axis = 0; axis < 3; axis++)
				{
					centre[axis] += point[axis];
				}
			}

			if (face.size() < 3)
			{
				return;
			}

			for (auto axis = 0; axis < 3; axis++)
			{
				centre[axis] /= static_cast<double>(face.size());
			}

			const auto& far_point = *std::ranges::max_element(face, {}, [&](const dvec3& point)
			{
				const auto offset = subtract(point, centre);
				return dot(offset, offset);
			});

			const auto u = normalized(subtract(far_point, centre));
			const auto v = cross(plane.normal, u);
			std::ranges::sort(face, {}, [&](const dvec3& point)
			{
				const auto offset = subtract(point, centre);
				return std::atan2(dot(offset, v), dot(offset, u));
			});

			const auto to_float = [](const dvec3& point)
			{
				return std::array<float, 3>{static_cast<float>(point[0]), static_cast<float>(point[1]), static_cast<float>(point[2])};
			};

			for (auto i = 1u; i + 1 < face.size(); i++)
			{
				triangles.push_back({to_float(face[0]), to_float(face[i]), to_float(face[i + 1])});
			}
		}

		void add_convex_hull(const std::vector<dvec3>& points, std::vector<gfxworld::collision_triangle>& triangles)
		{
			if (points.size() < 4)
			{
				return;
			}

			auto extent = 0.0;
			for (auto axis = 0; axis < 3; axis++)
			{
				const auto [low, high] = std::ranges::minmax(points | std::views::transform([&](const dvec3& vertex)
				{
					return vertex[axis];
				}));
				extent = std::max(extent, high - low);
			}

			const auto epsilon = 0.02 + extent * 1e-5;
			for (const auto& plane : find_hull_planes(points, epsilon, epsilon * epsilon))
			{
				add_hull_face(points, plane, epsilon, triangles);
			}
		}

		void add_brush_triangles(const zonetool::t9::ClipMapMesh& mesh, std::vector<gfxworld::collision_triangle>& triangles)
		{
			std::vector<dvec3> points;
			for (auto i = 0u; mesh.brushes && mesh.brushVertexRanges && i < mesh.brushCount; i++)
			{
				const auto& range = mesh.brushVertexRanges[i];
				const auto first_vertex = static_cast<std::uint32_t>(range.firstVertex);
				const auto end_vertex = first_vertex + range.vertexCount;
				if (end_vertex > mesh.brushVertexCount)
				{
					ZONETOOL_WARNING("havok: clip brush %u has vertices outside the vertex array", i);
					continue;
				}

				points.clear();
				for (auto vertex = first_vertex; vertex < end_vertex; vertex++)
				{
					const auto& position = mesh.brushVertices[vertex];
					points.push_back({position[0], position[1], position[2]});
				}

				add_convex_hull(points, triangles);
			}
		}

		void add_clip_mesh_triangles(const zonetool::t9::ClipMapMesh& mesh, std::vector<gfxworld::collision_triangle>& triangles)
		{
			for (auto i = 0u; mesh.meshLeaves && i < mesh.meshLeafCount; i++)
			{
				const auto& leaf = mesh.meshLeaves[i];
				const auto first = static_cast<std::uint32_t>(leaf.firstTriangleHigh) << 9 | leaf.firstTriangleLow;
				for (auto triangle = first; triangle < first + leaf.triangleCount && 3 * (triangle + 1) <= mesh.meshIndexCount; triangle++)
				{
					gfxworld::collision_triangle corners{};
					auto valid = true;
					for (auto corner = 0; corner < 3; corner++)
					{
						const auto vertex = static_cast<std::uint32_t>(leaf.firstVertex) + mesh.meshIndices[3 * triangle + corner];
						valid &= vertex < mesh.meshVertexCount;
						if (valid)
						{
							corners[corner] = {mesh.meshVertices[vertex][0], mesh.meshVertices[vertex][1], mesh.meshVertices[vertex][2]};
						}
					}

					if (valid)
					{
						triangles.push_back(corners);
					}
				}
			}
		}

		float model_size(const gfxworld::placed_model& placed)
		{
			const auto& half = placed.model->bounds.halfSize;
			return 2.0f * placed.transform.scale * std::sqrt(half[0] * half[0] + half[1] * half[1] + half[2] * half[2]);
		}

		std::size_t model_triangle_count(const gfxworld::placed_model& placed, const float box_size)
		{
			return model_size(placed) < box_size ? 12 : placed.model->indices.size() / 3;
		}

		void add_model_box(const gfxworld::placed_model& placed, std::vector<gfxworld::collision_triangle>& triangles)
		{
			const auto& bounds = placed.model->bounds;
			std::vector<dvec3> corners;
			for (auto corner = 0; corner < 8; corner++)
			{
				const point local{bounds.midPoint[0] + (corner & 1 ? bounds.halfSize[0] : -bounds.halfSize[0]),
					bounds.midPoint[1] + (corner & 2 ? bounds.halfSize[1] : -bounds.halfSize[1]),
					bounds.midPoint[2] + (corner & 4 ? bounds.halfSize[2] : -bounds.halfSize[2])};
				const auto position = gfxworld::transform_point(placed.transform, local);
				corners.push_back({position[0], position[1], position[2]});
			}

			add_convex_hull(corners, triangles);
		}

		void add_model_triangles(const gfxworld::placed_model& placed, std::vector<gfxworld::collision_triangle>& triangles)
		{
			const auto* model = placed.model;
			for (auto i = 0u; i + 2 < model->indices.size(); i += 3)
			{
				const auto a = model->indices[i];
				const auto b = model->indices[i + 1];
				const auto c = model->indices[i + 2];
				if (a >= model->positions.size() || b >= model->positions.size() || c >= model->positions.size())
				{
					continue;
				}

				triangles.push_back({gfxworld::transform_point(placed.transform, model->positions[a]),
					gfxworld::transform_point(placed.transform, model->positions[b]),
					gfxworld::transform_point(placed.transform, model->positions[c])});
			}
		}

		std::vector<const gfxworld::placed_model*> find_colliding_models(const zonetool::t9::ClipMap* clip_map, const std::vector<gfxworld::placed_model>& models)
		{
			std::unordered_set<std::string> names;
			for (auto i = 0u; clip_map->collisions && i < clip_map->collisionCount; i++)
			{
				if (clip_map->collisions[i])
				{
					names.emplace(xhash_name(clip_map->collisions[i]->name));
				}
			}

			std::vector<const gfxworld::placed_model*> colliding;
			for (const auto& placed : models)
			{
				if (placed.model && names.contains(placed.model->name))
				{
					colliding.push_back(&placed);
				}
			}

			ZONETOOL_INFO("havok: %zu of %zu static models have a T9 collision asset", colliding.size(), models.size());
			return colliding;
		}

		std::vector<gfxworld::collision_triangle> gather_clip_triangles(const zonetool::t9::ClipMapMesh& world)
		{
			std::vector<gfxworld::collision_triangle> triangles;
			add_brush_triangles(world, triangles);
			const auto brush_triangles = triangles.size();
			add_clip_mesh_triangles(world, triangles);
			ZONETOOL_INFO("havok: %u clip brushes give %zu triangles, the clip mesh gives %zu triangles", world.brushCount, brush_triangles,
				triangles.size() - brush_triangles);
			return triangles;
		}

		std::vector<gfxworld::collision_triangle> add_static_models(std::vector<gfxworld::collision_triangle> triangles,
			const std::vector<const gfxworld::placed_model*>& colliding, const std::size_t model_budget)
		{
			const auto count_triangles = [&](const float box_size)
			{
				std::size_t count = 0;
				for (const auto* placed : colliding)
				{
					count += model_triangle_count(*placed, box_size);
				}

				return count;
			};

			auto box_size = 0.0f;
			while (count_triangles(box_size) > model_budget && box_size < 4096.0f)
			{
				box_size = box_size > 0.0f ? box_size * 2.0f : 16.0f;
			}

			auto boxes = 0u;
			for (const auto* placed : colliding)
			{
				if (model_size(*placed) < box_size)
				{
					add_model_box(*placed, triangles);
					boxes++;
				}
				else
				{
					add_model_triangles(*placed, triangles);
				}
			}

			if (boxes)
			{
				ZONETOOL_WARNING("havok: %u static models smaller than %g units use their bounds box (collision is streamed in T9)", boxes, box_size);
			}

			return triangles;
		}

		std::vector<grid_triangle> snap_triangles(const std::vector<gfxworld::collision_triangle>& source, const float grid)
		{
			std::vector<grid_triangle> triangles;
			for (const auto& triangle : source)
			{
				add_snapped_triangle({snap_to_grid(triangle[0], grid), snap_to_grid(triangle[1], grid), snap_to_grid(triangle[2], grid)}, triangles);
			}

			std::ranges::sort(triangles);
			const auto duplicates = std::ranges::unique(triangles);
			triangles.erase(duplicates.begin(), duplicates.end());
			return triangles;
		}

		std::uint64_t spread_bits(std::uint64_t value)
		{
			value &= 0x1FFFFF;
			value = (value | value << 32) & 0x1F00000000FFFF;
			value = (value | value << 16) & 0x1F0000FF0000FF;
			value = (value | value << 8) & 0x100F00F00F00F00F;
			value = (value | value << 4) & 0x10C30C30C30C30C3;
			value = (value | value << 2) & 0x1249249249249249;
			return value;
		}

		std::uint64_t morton_order(const point& centre, const bounds& world)
		{
			std::uint64_t code = 0;
			for (auto axis = 0; axis < 3; axis++)
			{
				const auto extent = std::max(world.max[axis] - world.min[axis], 1.0f);
				const auto cell = std::clamp((centre[axis] - world.min[axis]) / extent, 0.0f, 1.0f) * 2097151.0f;
				code |= spread_bits(static_cast<std::uint64_t>(cell)) << axis;
			}

			return code;
		}

		std::vector<world_triangle> collect_triangles(const std::vector<gfxworld::collision_triangle>& source)
		{
			auto grid = 1.0f;
			auto snapped = snap_triangles(source, grid);
			while (snapped.size() > 2500000)
			{
				grid *= 2.0f;
				snapped = snap_triangles(source, grid);
			}

			ZONETOOL_INFO("havok: collision vertices snap to a %g unit grid", grid);

			std::vector<world_triangle> triangles;
			triangles.reserve(snapped.size());
			for (const auto& corners : snapped)
			{
				auto& triangle = triangles.emplace_back();
				for (auto corner = 0; corner < 3; corner++)
				{
					for (auto axis = 0; axis < 3; axis++)
					{
						triangle.points[corner][axis] = static_cast<float>(corners[corner][axis]) * grid / 32.0f; // inches to havok units
					}
				}
			}

			auto world = empty_bounds();
			for (const auto& triangle : triangles)
			{
				for (const auto& vertex : triangle.points)
				{
					include(world, vertex);
				}
			}

			for (auto& triangle : triangles)
			{
				const point centre{(triangle.points[0][0] + triangle.points[1][0] + triangle.points[2][0]) / 3.0f,
					(triangle.points[0][1] + triangle.points[1][1] + triangle.points[2][1]) / 3.0f,
					(triangle.points[0][2] + triangle.points[1][2] + triangle.points[2][2]) / 3.0f};
				triangle.order = morton_order(centre, world);
			}

			std::ranges::sort(triangles, {}, &world_triangle::order);
			return triangles;
		}

		std::vector<world_triangle> collect_world_triangles(const zonetool::t9::ClipMap* clip_map, const std::vector<gfxworld::placed_model>& models,
			const std::vector<gfxworld::collision_triangle>& world_surface_triangles)
		{
			if (!clip_map || !clip_map->meshes || !clip_map->meshCount)
			{
				ZONETOOL_WARNING("havok: the map has no T9 clip mesh, using %zu render triangles and every static model", world_surface_triangles.size());
				auto triangles = world_surface_triangles;
				for (const auto& placed : models)
				{
					if (placed.model)
					{
						add_model_triangles(placed, triangles);
					}
				}

				return collect_triangles(triangles);
			}

			const auto clip_triangles = gather_clip_triangles(clip_map->meshes[0]);
			const auto colliding = find_colliding_models(clip_map, models);
			auto model_budget = std::size_t{1000000};
			auto triangles = collect_triangles(add_static_models(clip_triangles, colliding, model_budget));
			while (triangles.size() > 800000 && model_budget > 10000) // about 15 MB colmap
			{
				model_budget /= 2;
				triangles = collect_triangles(add_static_models(clip_triangles, colliding, model_budget));
			}

			return triangles;
		}

		void close_section(const std::vector<point>& vertices, const std::vector<std::array<std::uint8_t, 3>>& triangles, mesh_shape& mesh)
		{
			auto box = empty_bounds();
			for (const auto& vertex : vertices)
			{
				include(box, vertex);
			}

			output_section output{};
			const float steps[3] = {2047.0f, 2047.0f, 1023.0f};
			for (auto axis = 0; axis < 3; axis++)
			{
				output.codec[axis] = box.min[axis];
				output.codec[axis + 3] = (box.max[axis] - box.min[axis]) / steps[axis];
			}

			output.first_packed = static_cast<std::uint32_t>(mesh.packed_vertices.size());
			output.packed_count = static_cast<std::uint8_t>(vertices.size());

			std::vector<point> decoded;
			for (const auto& vertex : vertices)
			{
				std::uint32_t q[3]{};
				point value{};
				for (auto axis = 0; axis < 3; axis++)
				{
					const auto scale = output.codec[axis + 3];
					if (scale > 0.0f)
					{
						q[axis] = static_cast<std::uint32_t>(std::clamp(std::round((vertex[axis] - box.min[axis]) / scale), 0.0f, steps[axis]));
					}

					value[axis] = output.codec[axis] + static_cast<float>(q[axis]) * scale;
				}

				mesh.packed_vertices.push_back(q[0] | q[1] << 11 | q[2] << 22);
				decoded.push_back(value);
			}

			std::vector<bounds> primitive_bounds;
			for (const auto& triangle : triangles)
			{
				output.primitives.push_back({triangle[0], triangle[1], triangle[2], triangle[2]});
				auto primitive_box = empty_bounds();
				for (const auto index : triangle)
				{
					include(primitive_box, decoded[index]);
				}

				primitive_bounds.push_back(primitive_box);
			}

			output.runs.push_back({0, 0, static_cast<unsigned char>(output.primitives.size())});
			build_section_tree(output, primitive_bounds);
			output.primitive_bounds = std::move(primitive_bounds);
			mesh.outputs.push_back(std::move(output));
		}

		void fill_sections(const std::vector<world_triangle>& triangles, mesh_shape& mesh)
		{
			std::vector<point> vertices;
			std::map<point, std::uint8_t> lookup;
			std::vector<std::array<std::uint8_t, 3>> section_triangles;
			auto box = empty_bounds();

			for (const auto& triangle : triangles)
			{
				auto new_vertices = 0u;
				auto grown = box;
				for (const auto& vertex : triangle.points)
				{
					new_vertices += lookup.contains(vertex) ? 0 : 1;
					include(grown, vertex);
				}

				const auto too_large = grown.max[0] - grown.min[0] > 32.0f || grown.max[1] - grown.min[1] > 32.0f
					|| grown.max[2] - grown.min[2] > 32.0f;
				if (!section_triangles.empty() && (section_triangles.size() == 128 || vertices.size() + new_vertices > 255 || too_large))
				{
					close_section(vertices, section_triangles, mesh);
					vertices.clear();
					lookup.clear();
					section_triangles.clear();
					grown = empty_bounds();
					for (const auto& vertex : triangle.points)
					{
						include(grown, vertex);
					}
				}

				box = grown;
				auto& indices = section_triangles.emplace_back();
				for (auto corner = 0; corner < 3; corner++)
				{
					const auto& vertex = triangle.points[corner];
					const auto [found, added] = lookup.try_emplace(vertex, static_cast<std::uint8_t>(vertices.size()));
					if (added)
					{
						vertices.push_back(vertex);
					}

					indices[corner] = found->second;
				}
			}

			if (!section_triangles.empty())
			{
				close_section(vertices, section_triangles, mesh);
			}
		}

		mesh_keys count_mesh_keys(const std::vector<output_section>& outputs)
		{
			mesh_keys keys{};
			keys.bits_per_key = static_cast<std::uint32_t>(std::bit_width(std::max<std::size_t>(outputs.size(), 1) - 1)) + 8;
			for (auto s = 0u; s < outputs.size(); s++)
			{
				for (auto p = 0u; p < outputs[s].primitives.size(); p++)
				{
					keys.max_key = std::max(keys.max_key, s << 8 | p << 1);
					keys.triangle_count++;
				}
			}

			return keys;
		}

		void put_simd_child(std::vector<std::byte>& nodes, const std::size_t base, const std::uint32_t slot, const bounds& box, const std::uint32_t data)
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
			const auto index = static_cast<std::uint32_t>(nodes.size() / 128);
			const auto base = nodes.size();
			nodes.resize(base + 128, std::byte{0});
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
					include(box, item.box);
				}

				const auto child = add_simd_node(nodes, child_items, child_items.size() <= 4);
				put_simd_child(nodes, base, slot, box, child);
			}

			return index;
		}

		std::vector<std::byte> make_simd_tree(const tag_writer& writer, const std::vector<output_section>& outputs)
		{
			if (writer.type_size(writer.type_id("hkcdSimdTree::Node")) != 128)
			{
				throw std::runtime_error("havok schema: the simd tree node size is wrong");
			}

			std::vector<simd_item> items;
			for (auto s = 0u; s < outputs.size(); s++)
			{
				for (auto p = 0u; p < outputs[s].primitive_bounds.size(); p++)
				{
					items.push_back({outputs[s].primitive_bounds[p], s << 8 | p << 1});
				}
			}

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

		tag_writer::reference write_box_shape(tag_writer& writer, const point& half_size)
		{
			const auto t_box = writer.type_id("hknpBoxShape");
			const auto t_face = writer.type_id("hknpConvexPolytopeShape::Face");
			const auto t_connectivity = writer.type_id("hknpConvexPolytopeShape::Connectivity");
			const auto t_edge = writer.type_id("hknpConvexPolytopeShape::Connectivity::Edge");

			std::array<std::array<float, 4>, 8> vertices{};
			for (auto i = 0u; i < vertices.size(); i++)
			{
				vertices[i] = {i & 1 ? -half_size[0] : half_size[0], i & 2 ? -half_size[1] : half_size[1], i & 4 ? -half_size[2] : half_size[2],
					std::bit_cast<float>(0x3F000000 | i)};
			}

			std::array<std::array<float, 4>, 6> planes{};
			for (auto i = 0u; i < planes.size(); i++)
			{
				planes[i][i / 2] = i & 1 ? -1.0f : 1.0f;
				planes[i][3] = -half_size[i / 2];
			}

			const std::array<std::uint8_t, 24> indices{2, 6, 4, 0, 1, 5, 7, 3, 1, 0, 4, 5, 7, 6, 2, 3, 3, 2, 0, 1, 7, 5, 4, 6};
			const auto face_size = writer.type_size(t_face);
			std::vector<std::byte> face_bytes(face_size * planes.size(), std::byte{0});
			for (auto i = 0u; i < planes.size(); i++)
			{
				put(face_bytes, i * face_size + member(t_face, "firstIndex").offset, static_cast<std::uint16_t>(i * 4));
				put(face_bytes, i * face_size + member(t_face, "numIndices").offset, static_cast<std::uint8_t>(4));
				put(face_bytes, i * face_size + member(t_face, "minHalfAngle").offset, static_cast<std::uint8_t>(127));
			}

			const auto add_edges = [&](const std::span<const std::array<std::uint8_t, 2>> edges)
			{
				const auto edge_size = writer.type_size(t_edge);
				std::vector<std::byte> edge_bytes(edge_size * edges.size(), std::byte{0});
				for (auto i = 0u; i < edges.size(); i++)
				{
					put(edge_bytes, i * edge_size + member(t_edge, "faceIndex").offset, static_cast<std::uint16_t>(edges[i][0]));
					put(edge_bytes, i * edge_size + member(t_edge, "edgeIndex").offset, edges[i][1]);
				}

				return writer.add_array(t_edge, edge_bytes.data(), edges.size());
			};

			const std::array<std::array<std::uint8_t, 2>, 8> vertex_edges{{{4, 2}, {2, 0}, {4, 1}, {4, 0}, {5, 2}, {5, 1}, {0, 1}, {5, 0}}};
			const std::array<std::array<std::uint8_t, 2>, 24> face_links{{{3, 1}, {5, 2}, {2, 1}, {4, 1}, {2, 3}, {5, 0}, {3, 3}, {4, 3}, {4, 2},
				{0, 2}, {5, 1}, {1, 0}, {5, 3}, {0, 0}, {4, 0}, {1, 2}, {3, 2}, {0, 3}, {2, 0}, {1, 3}, {1, 1}, {2, 2}, {0, 1}, {3, 0}}};

			std::vector<std::byte> box_bytes(writer.type_size(t_box), std::byte{0});
			put_member(box_bytes, t_box, "flags", static_cast<std::uint16_t>(0x09C3));
			put_member(box_bytes, t_box, "type", static_cast<std::uint8_t>(6));
			put_member(box_bytes, t_box, "dispatchType", static_cast<std::uint8_t>(2));
			put_member(box_bytes, t_box, "obb", std::array<float, 16>{1.0f, 0.0f, 0.0f, half_size[0], 0.0f, 1.0f, 0.0f, half_size[1],
				0.0f, 0.0f, 1.0f, half_size[2], 0.0f, 0.0f, 0.0f, 0.5f});
			const auto box = writer.add_object(t_box, box_bytes.data());

			const auto set = [&](const tag_writer::reference& object, const std::uint32_t type, const std::string_view name,
				const tag_writer::reference& reference)
			{
				const auto& field = member(type, name);
				writer.set_pointer(object.offset + field.offset, field.type, reference.item);
			};
			set(box, t_box, "vertices", writer.add_array(writer.type_id("hkVector4f"), vertices.data(), vertices.size()));
			set(box, t_box, "planes", writer.add_array(writer.type_id("hkVector4"), planes.data(), planes.size()));
			set(box, t_box, "faces", writer.add_array(t_face, face_bytes.data(), planes.size()));
			set(box, t_box, "indices", writer.add_array(writer.type_id("hkUint8"), indices.data(), indices.size()));

			const auto vertex_edges_item = add_edges(vertex_edges);
			const auto face_links_item = add_edges(face_links);
			const std::vector<std::byte> connectivity_bytes(writer.type_size(t_connectivity), std::byte{0});
			const auto connectivity = writer.add_object(t_connectivity, connectivity_bytes.data());
			set(connectivity, t_connectivity, "vertexEdges", vertex_edges_item);
			set(connectivity, t_connectivity, "faceLinks", face_links_item);
			set(box, t_box, "connectivity", connectivity);
			return box;
		}

		tag_writer::reference write_box_instances(tag_writer& writer, const std::vector<bounds>& boxes, const std::uint16_t shape_tag)
		{
			const auto t_instance = writer.type_id("hknpShapeInstance");
			const auto instance_size = writer.type_size(t_instance);
			std::vector<std::byte> instance_bytes(instance_size * boxes.size(), std::byte{0});
			for (auto i = 0u; i < boxes.size(); i++)
			{
				const auto base = i * instance_size;
				const auto centre = center_of(boxes[i]);
				put(instance_bytes, base + member(t_instance, "transform").offset, std::array<float, 16>{1.0f, 0.0f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f, centre[0], centre[1], centre[2], 1.0f});
				put(instance_bytes, base + member(t_instance, "scale").offset, std::array<float, 4>{1.0f, 1.0f, 1.0f, 1.0f});
				put(instance_bytes, base + member(t_instance, "shapeTag").offset, shape_tag);
				put(instance_bytes, base + member(t_instance, "destructionTag").offset, static_cast<std::uint16_t>(0xFFFF));
			}

			const auto instances = writer.add_array(t_instance, instance_bytes.data(), boxes.size());
			const auto& shape_field = member(t_instance, "shape");
			for (auto i = 0u; i < boxes.size(); i++)
			{
				const auto& box = boxes[i];
				const auto shape = write_box_shape(writer, {(box.max[0] - box.min[0]) * 0.5f, (box.max[1] - box.min[1]) * 0.5f,
					(box.max[2] - box.min[2]) * 0.5f});
				writer.set_pointer(instances.offset + i * instance_size + shape_field.offset, shape_field.type, shape.item);
			}

			return instances;
		}

		bounds to_bounds(const aabb& box)
		{
			return {{box.min[0], box.min[1], box.min[2]}, {box.max[0], box.max[1], box.max[2]}};
		}

		tag_writer::reference write_mesh_shape(tag_writer& writer, mesh_shape& mesh, const mesh_keys& keys)
		{
			auto& outputs = mesh.outputs;

			std::vector<std::array<std::uint8_t, 4>> all_primitives;
			std::vector<zonetool::iw8::hkcdPrimitiveDataRun> all_runs;
			std::vector<point> section_centres;
			std::vector<bounds> section_bounds;
			for (auto& output : outputs)
			{
				output.first_primitive = static_cast<std::uint32_t>(all_primitives.size());
				output.first_run = static_cast<std::uint32_t>(all_runs.size());
				all_primitives.insert(all_primitives.end(), output.primitives.begin(), output.primitives.end());
				all_runs.insert(all_runs.end(), output.runs.begin(), output.runs.end());
				section_centres.push_back(center_of(output.domain));
				section_bounds.push_back(output.domain);
			}

			std::vector<std::uint32_t> order(outputs.size());
			std::iota(order.begin(), order.end(), 0u);
			std::vector<top_node> top_nodes;
			split_top(section_centres, order, top_nodes);

			std::vector<std::array<std::uint8_t, 5>> top_bytes(top_nodes.size());
			std::vector<std::uint32_t> top_leaves(outputs.size(), 0);
			encode_top(top_nodes, section_bounds, 0, to_bounds(mesh.domain), top_bytes, top_leaves);
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
			const auto t_section = writer.type_id("hkcdStaticMeshTree::Section");
			const auto t_primitive = writer.type_id("hkcdStaticMeshTree::Primitive");
			const auto t_run = writer.type_id("hkcdDefaultStaticMeshTree::PrimitiveDataRun");
			const auto t_top_node = writer.type_id("hkcdCompressedAabbCodecs::Aabb5BytesCodec");
			const auto t_section_node = writer.type_id("hkcdCompressedAabbCodecs::Aabb4BytesCodec");
			const auto t_simd_node = writer.type_id("hkcdSimdTree::Node");
			const auto t_uint16 = writer.type_id("hkUint16");
			const auto t_uint32 = writer.type_id("unsigned int");
			const auto t_uint64 = writer.type_id("unsigned long long");

			if (writer.type_size(t_run) != sizeof(zonetool::iw8::hkcdPrimitiveDataRun))
			{
				throw std::runtime_error("havok schema: the primitive data run size is wrong");
			}

			const auto instances_item = write_box_instances(writer, mesh.boxes, mesh.box_shape_tag);
			const auto mesh_nodes_item = writer.add_array(t_top_node, top_bytes.data(), top_bytes.size());
			const auto mesh_primitives_item = writer.add_array(t_primitive, all_primitives.data(), all_primitives.size());
			const auto mesh_table_item = writer.add_array(t_uint16, mesh.shared_vertex_indices.data(), mesh.shared_vertex_indices.size());
			const auto mesh_runs_item = writer.add_array(t_run, all_runs.data(), all_runs.size());
			const auto mesh_packed_item = writer.add_array(t_uint32, mesh.packed_vertices.data(), mesh.packed_vertices.size());
			const auto mesh_shared_item = writer.add_array(t_uint64, nullptr, 0);

			std::vector<tag_writer::reference> section_nodes_items;
			section_nodes_items.reserve(outputs.size());
			for (const auto& output : outputs)
			{
				section_nodes_items.push_back(writer.add_array(t_section_node, output.nodes.data(), output.nodes.size()));
			}

			const auto section_size = writer.type_size(t_section);
			std::vector<std::byte> section_bytes(section_size * outputs.size(), std::byte{0});
			for (auto s = 0u; s < outputs.size(); s++)
			{
				const auto& output = outputs[s];
				const auto base = s * section_size;
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
				put(section_bytes, base + member(t_section, "firstSharedVertexIndex").offset, 0u);
				put(section_bytes, base + member(t_section, "firstPrimitiveIndex").offset, output.first_primitive);
				put(section_bytes, base + member(t_section, "firstDataRunIndex").offset, output.first_run);
				put(section_bytes, base + member(t_section, "numPackedVertices").offset, output.packed_count);
				put(section_bytes, base + member(t_section, "numPrimitives").offset, static_cast<std::uint8_t>(output.primitives.size()));
				put(section_bytes, base + member(t_section, "numDataRuns").offset, static_cast<std::uint8_t>(output.runs.size()));
				put(section_bytes, base + member(t_section, "page").offset, static_cast<std::uint8_t>(0));
				put(section_bytes, base + member(t_section, "leafIndex").offset, output.leaf_index);
				put(section_bytes, base + member(t_section, "layerData").offset, static_cast<std::uint8_t>(0));
				put(section_bytes, base + member(t_section, "flags").offset, static_cast<std::uint8_t>(1));
			}

			const auto sections_item = writer.add_array(t_section, section_bytes.data(), outputs.size());
			const auto& nodes_field = member(t_section, "nodes");
			for (auto s = 0u; s < outputs.size(); s++)
			{
				writer.set_pointer(sections_item.offset + s * section_size + nodes_field.offset, nodes_field.type, section_nodes_items[s].item);
			}

			auto data_bytes = std::vector<std::byte>(writer.type_size(t_data), std::byte{0});
			const auto& mesh_tree_field = member(t_data, "meshTree");
			const auto mesh_tree_offset = mesh_tree_field.offset;
			const auto mesh_tree_type = mesh_tree_field.type;
			put(data_bytes, mesh_tree_offset + member(mesh_tree_type, "domain").offset, mesh.domain);
			put(data_bytes, mesh_tree_offset + member(mesh_tree_type, "numPrimitiveKeys").offset, static_cast<std::int32_t>(keys.triangle_count));
			put(data_bytes, mesh_tree_offset + member(mesh_tree_type, "bitsPerKey").offset, static_cast<std::int32_t>(keys.bits_per_key));
			put(data_bytes, mesh_tree_offset + member(mesh_tree_type, "maxKeyValue").offset, keys.max_key);
			put(data_bytes, mesh_tree_offset + member(mesh_tree_type, "primitiveStoresIsFlatConvex").offset, static_cast<std::uint8_t>(0xFF));
			put(data_bytes, member(t_data, "hasSimdTree").offset, static_cast<std::uint8_t>(1));

			const auto& simd_tree_field = member(t_data, "simdTree");
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
			writer.set_pointer(data_offset.offset + simd_tree_field.offset + simd_nodes_field.offset, simd_nodes_field.type, simd_nodes_item.item);

			const auto interior_bits = keys.max_key + 1;
			std::vector<std::uint32_t> interior_words((interior_bits + 31) / 32, 0);
			const auto interior_words_item = writer.add_array(t_uint32, interior_words.data(), interior_words.size());

			auto shape_bytes = std::vector<std::byte>(writer.type_size(t_shape), std::byte{0});
			put_member(shape_bytes, t_shape, "flags", static_cast<std::uint16_t>(0x0004));
			put_member(shape_bytes, t_shape, "type", static_cast<std::uint8_t>(9));
			put_member(shape_bytes, t_shape, "numShapeKeyBits", static_cast<std::uint8_t>(keys.bits_per_key));
			put_member(shape_bytes, t_shape, "dispatchType", static_cast<std::uint8_t>(3));
			put_member(shape_bytes, t_shape, "convexRadius", 0.0f);
			put_member(shape_bytes, t_shape, "userData", static_cast<std::uint64_t>(0));
			put_member(shape_bytes, t_shape, "shapeTagCodecInfo", static_cast<std::uint32_t>(0xFFFFFFFF));

			const auto& interior_field = member(t_shape, "triangleIsInterior");
			const auto& storage_field = member(interior_field.type, "storage");
			put(shape_bytes, interior_field.offset + storage_field.offset + member(storage_field.type, "numBits").offset,
				static_cast<std::int32_t>(interior_bits));

			const auto shape_offset = writer.add_object(t_shape, shape_bytes.data());
			writer.set_pointer(shape_offset.offset + member(t_shape, "data").offset, member(t_shape, "data").type, data_offset.item);
			writer.set_pointer(shape_offset.offset + member(t_shape, "externShapes").offset, member(t_shape, "externShapes").type,
				instances_item.item);
			const auto& words_field = member(storage_field.type, "words");
			writer.set_pointer(shape_offset.offset + interior_field.offset + storage_field.offset + words_field.offset, words_field.type,
				interior_words_item.item);

			return shape_offset;
		}

		std::vector<zonetool::iw8::HavokPhysicsShapeTagData> make_world_tags()
		{
			zonetool::iw8::HavokPhysicsShapeTagData tag{};
			tag.collisionFilterInfo = zonetool::iw8::PHYSICS_CONTENTS_SOLID;
			tag.materialCRC = 0x1AB7BC33;
			tag.materialId = 0xFFFF;
			tag.userData = static_cast<std::uint64_t>(zonetool::iw8::SURFACE_TYPE_CONCRETE_DRY) << 19;
			return {tag};
		}

		struct list_arrays
		{
			tag_writer::reference shapes;
			tag_writer::reference shape_indices;
			tag_writer::reference shape_names;
			tag_writer::reference vert_counts;
			tag_writer::reference tri_counts;
			tag_writer::reference min_maxes;
			tag_writer::reference shape_tags;
			tag_writer::reference shape_contents;
			tag_writer::reference convex_counts;
		};

		void set_list_arrays(tag_writer& writer, const std::uint32_t t_list, const tag_writer::reference& list, const list_arrays& arrays)
		{
			const auto set = [&](const std::string_view name, const tag_writer::reference& reference)
			{
				const auto& field = member(t_list, name);
				writer.set_pointer(list.offset + field.offset, field.type, reference.item);
			};
			set("shapes", arrays.shapes);
			set("shapeIndices", arrays.shape_indices);
			set("shapeNames", arrays.shape_names);
			set("vertCounts", arrays.vert_counts);
			set("triCounts", arrays.tri_counts);
			set("minMaxes", arrays.min_maxes);
			set("shapeTagData", arrays.shape_tags);
			set("shapeContents", arrays.shape_contents);
			set("convexCounts", arrays.convex_counts);
		}

		std::string write_world_list(mesh_shape& mesh)
		{
			const auto keys = count_mesh_keys(mesh.outputs);
			const auto tags = make_world_tags();

			tag_writer writer(schema());
			const auto t_list = writer.type_id("HavokPhysicsShapeList");
			const auto t_tag = writer.type_id("HavokPhysicsShapeList::ShapeTagData");
			const auto t_vector4 = writer.type_id("hkVector4");
			const auto t_int = writer.type_id("int");
			const auto t_char = writer.type_id("char");
			const auto t_pointer = writer.type_id("T*");
			const auto t_string = writer.type_id("hkStringPtr");
			if (writer.type_size(t_tag) != sizeof(zonetool::iw8::HavokPhysicsShapeTagData))
			{
				throw std::runtime_error("havok schema: the shape tag size is wrong");
			}

			const auto list_bytes = std::vector<std::byte>(writer.type_size(t_list), std::byte{0});
			const auto list = writer.add_object(t_list, list_bytes.data());

			const std::string name = "world";
			const auto name_item = writer.add_array(t_char, name.data(), name.size() + 1);
			const std::uint64_t null_pointer = 0;
			list_arrays arrays{};
			arrays.shape_names = writer.add_array(t_string, &null_pointer, 1);
			writer.set_pointer(arrays.shape_names.offset, t_string, name_item.item);
			arrays.shapes = writer.add_array(t_pointer, &null_pointer, 1);

			const std::int32_t shape_index = -1;
			const auto vertex_count = static_cast<std::int32_t>(mesh.packed_vertices.size());
			const auto triangle_count = static_cast<std::int32_t>(keys.triangle_count);
			const auto contents = static_cast<std::int32_t>(tags.front().collisionFilterInfo);
			const std::int32_t convex_count = 0;
			arrays.shape_indices = writer.add_array(t_int, &shape_index, 1);
			arrays.vert_counts = writer.add_array(t_int, &vertex_count, 1);
			arrays.tri_counts = writer.add_array(t_int, &triangle_count, 1);
			arrays.shape_contents = writer.add_array(t_int, &contents, 1);
			arrays.convex_counts = writer.add_array(t_int, &convex_count, 1);

			const aabb min_maxes = mesh.domain;
			arrays.min_maxes = writer.add_array(t_vector4, &min_maxes, 2);
			arrays.shape_tags = writer.add_array(t_tag, tags.data(), tags.size());

			mesh.simd_nodes = make_simd_tree(writer, mesh.outputs);
			ZONETOOL_INFO("havok: the world simd tree has %zu nodes", mesh.simd_nodes.size() / 128);
			writer.set_pointer(arrays.shapes.offset, t_pointer, write_mesh_shape(writer, mesh, keys).item);

			set_list_arrays(writer, t_list, list, arrays);
			return writer.finish(list.item);
		}

		mesh_shape make_trigger_mesh(const mapents::trigger& trigger, const std::uint16_t shape_tag)
		{
			mesh_shape mesh{};
			mesh.box_shape_tag = shape_tag;
			output_section output{};
			output.codec = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
				-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()};

			for (const auto& hull : trigger.hulls)
			{
				const auto index = static_cast<std::uint8_t>(mesh.boxes.size());
				auto& box = mesh.boxes.emplace_back();
				for (auto axis = 0; axis < 3; axis++)
				{
					box.min[axis] = (hull.bounds.midPoint[axis] - hull.bounds.halfSize[axis]) / 32.0f;
					box.max[axis] = (hull.bounds.midPoint[axis] + hull.bounds.halfSize[axis]) / 32.0f;
				}

				const auto first_shared_index = static_cast<std::uint8_t>(index * 4);
				output.primitives.push_back({first_shared_index, first_shared_index, 4, first_shared_index});
				mesh.shared_vertex_indices.insert(mesh.shared_vertex_indices.end(), {146, 0, index, 0}); // stock extern shape reference
			}

			output.runs.push_back({shape_tag, 0, static_cast<unsigned char>(mesh.boxes.size())});
			build_section_tree(output, mesh.boxes);
			output.primitive_bounds = mesh.boxes;
			mesh.domain = {{output.domain.min[0], output.domain.min[1], output.domain.min[2], 0.0f},
				{output.domain.max[0], output.domain.max[1], output.domain.max[2], 0.0f}};
			mesh.outputs.push_back(std::move(output));
			return mesh;
		}

		std::string write_ents_list(const std::vector<mapents::trigger>& triggers, const std::string& map_name)
		{
			auto tags = make_world_tags(); // Physics_SetMainShapeList (0x1490C00)
			const auto trigger_tag = static_cast<std::uint16_t>(tags.size());
			auto& tag = tags.emplace_back();
			tag.collisionFilterInfo = 0xFFFFFFFF;
			tag.materialCRC = 0x1AB7BC33;
			tag.materialId = 0xFFFF;
			tag.userData = 0x3000000000000;

			tag_writer writer(schema());
			const auto t_list = writer.type_id("HavokPhysicsShapeList");
			const auto t_char = writer.type_id("char");
			const auto t_int = writer.type_id("int");
			const auto t_pointer = writer.type_id("T*");
			const auto t_string = writer.type_id("hkStringPtr");

			const auto list_bytes = std::vector<std::byte>(writer.type_size(t_list), std::byte{0});
			const auto list = writer.add_object(t_list, list_bytes.data());

			std::vector<std::int32_t> shape_indices;
			std::vector<std::int32_t> vertex_counts;
			std::vector<std::int32_t> convex_counts;
			std::vector<aabb> min_maxes;
			std::vector<mesh_shape> meshes;
			for (const auto& trigger : triggers)
			{
				const auto& mesh = meshes.emplace_back(make_trigger_mesh(trigger, trigger_tag));
				shape_indices.push_back(static_cast<std::int32_t>(shape_indices.size()));
				vertex_counts.push_back(static_cast<std::int32_t>(mesh.boxes.size() * 8));
				convex_counts.push_back(static_cast<std::int32_t>(mesh.boxes.size()));
				min_maxes.push_back(mesh.domain);
			}

			const std::vector<std::uint64_t> null_pointers(triggers.size(), 0);
			const std::vector<std::int32_t> triangle_counts(triggers.size(), 0);
			const std::vector<std::int32_t> contents(triggers.size(), -1);

			list_arrays arrays{};
			arrays.shape_names = writer.add_array(t_string, null_pointers.data(), triggers.size());
			arrays.shapes = writer.add_array(t_pointer, null_pointers.data(), triggers.size());
			arrays.shape_indices = writer.add_array(t_int, shape_indices.data(), triggers.size());
			arrays.vert_counts = writer.add_array(t_int, vertex_counts.data(), triggers.size());
			arrays.tri_counts = writer.add_array(t_int, triangle_counts.data(), triggers.size());
			arrays.shape_contents = writer.add_array(t_int, contents.data(), triggers.size());
			arrays.convex_counts = writer.add_array(t_int, convex_counts.data(), triggers.size());
			arrays.min_maxes = writer.add_array(writer.type_id("hkVector4"), min_maxes.data(), triggers.size() * 2);
			arrays.shape_tags = writer.add_array(writer.type_id("HavokPhysicsShapeList::ShapeTagData"), tags.data(), tags.size());

			for (auto i = 0u; i < meshes.size(); i++)
			{
				auto& mesh = meshes[i];
				const auto name = std::format("{}.map:entity {}", map_name, i);
				writer.set_pointer(arrays.shape_names.offset + i * sizeof(std::uint64_t), t_string,
					writer.add_array(t_char, name.data(), name.size() + 1).item);

				mesh_keys keys{};
				keys.max_key = static_cast<std::uint32_t>(mesh.boxes.size() - 1) << 1;
				keys.bits_per_key = std::max(1u, static_cast<std::uint32_t>(std::bit_width(keys.max_key)));
				keys.triangle_count = static_cast<std::uint32_t>(mesh.boxes.size());
				mesh.simd_nodes = make_simd_tree(writer, mesh.outputs);
				writer.set_pointer(arrays.shapes.offset + i * sizeof(std::uint64_t), t_pointer, write_mesh_shape(writer, mesh, keys).item);
			}

			set_list_arrays(writer, t_list, list, arrays);
			return writer.finish(list.item);
		}
	}

	std::string write_trigger_physics_asset()
	{
		const auto& types = physics_asset_schema();
		const auto field = [&](const std::uint32_t type, const std::string_view name) -> const tag_member&
		{
			for (auto current = type; current; current = types.at(current).parent)
			{
				for (const auto& candidate : types.at(current).members)
				{
					if (candidate.name == name)
					{
						return candidate;
					}
				}
			}

			throw std::runtime_error(std::format("havok schema: type {} has no member {}", type, name));
		};

		tag_writer writer(types);
		writer.use_compendium(0xC16B2E49324C3540, physics_asset_compendium_types()); // physicsglobaltypecompendium
		const auto t_asset = writer.type_id("HavokPhysicsAsset");
		const auto t_system = writer.type_id("hknpPhysicsSystemData");
		const auto t_body = writer.type_id("hknpPhysicsSystemData::bodyCinfoWithAttachment");
		const auto t_hull = writer.type_id("hknpConvexPolytopeShape");
		const auto t_face = writer.type_id("hknpConvexPolytopeShape::Face");
		const auto t_properties = writer.type_id("hkRefCountedProperties");
		const auto t_entry = writer.type_id("hkRefCountedProperties::Entry");
		const auto t_mass = writer.type_id("hknpShapeMassProperties");
		const auto t_mass_values = field(t_mass, "compressedMassProperties").type;
		const auto t_int = writer.type_id("int");
		const auto t_char = writer.type_id("char");
		const auto t_string = writer.type_id("hkStringPtr");

		const auto set = [&](const tag_writer::reference& object, const std::uint32_t type, const std::string_view name, const std::uint32_t item)
		{
			const auto& target_field = field(type, name);
			writer.set_pointer(object.offset + target_field.offset, target_field.type, item);
		};
		const auto add_string = [&](const std::string_view text)
		{
			const std::string value(text);
			return writer.add_array(t_char, value.data(), value.size() + 1).item;
		};

		std::vector<std::byte> asset_bytes(writer.type_size(t_asset), std::byte{0});
		put(asset_bytes, field(t_asset, "useCategory").offset, zonetool::iw8::PhysicsAssetUseCategory_Custom);
		const auto asset = writer.add_object(t_asset, asset_bytes.data());

		const auto set_int = [&](const std::string_view name, const std::int32_t value)
		{
			set(asset, t_asset, name, writer.add_array(t_int, &value, 1).item);
		};
		set_int("bodyQualityNameCRCLookup", 0x7923E35C);
		set_int("materialNameCRCLookup", 0x1AB7BC33);
		set_int("motionPropertiesNameCRCLookup", static_cast<std::int32_t>(0xE9D0CCAF));

		const std::uint64_t null_name = 0;
		set(asset, t_asset, "bodySFXAssetNames", writer.add_array(t_string, &null_name, 1).item);
		set(asset, t_asset, "bodyVFXAssetNames", writer.add_array(t_string, &null_name, 1).item);
		set_int("bodyServerUsage", 0);
		set_int("bodyDrivers", 0);
		set_int("simulationCategories", zonetool::iw8::PhysicsAssetSimulationCategory_Custom);

		std::vector<std::byte> system_bytes(writer.type_size(t_system), std::byte{0});
		put(system_bytes, field(t_system, "microStepMultiplier").offset, static_cast<std::uint8_t>(1));
		const auto system = writer.add_object(t_system, system_bytes.data());
		set(asset, t_asset, "physicsSystemData", system.item);
		set(system, t_system, "name", add_string("Default Physics System Data"));

		std::vector<std::byte> body_bytes(writer.type_size(t_body), std::byte{0});
		put(body_bytes, field(t_body, "collisionFilterInfo").offset, static_cast<std::uint32_t>(zonetool::iw8::PHYSICS_CONTENTS_SOLID));
		put(body_bytes, field(t_body, "materialId").offset, static_cast<std::uint16_t>(0xFFFF));
		put(body_bytes, field(t_body, "qualityId").offset, static_cast<std::uint8_t>(0xFF));
		put(body_bytes, field(t_body, "motionType").offset, static_cast<std::uint8_t>(1));
		put(body_bytes, field(t_body, "position").offset, std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
		put(body_bytes, field(t_body, "orientation").offset, std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
		put(body_bytes, field(t_body, "motionPropertiesId").offset, static_cast<std::uint16_t>(0xFFFF));
		put(body_bytes, field(t_body, "desiredBodyId").offset, static_cast<std::uint32_t>(0x00FFFFFF));
		put(body_bytes, field(t_body, "motionId").offset, static_cast<std::uint32_t>(0x7FFFFFFF));
		put(body_bytes, field(t_body, "activationPriority").offset, static_cast<std::int8_t>(5));
		put(body_bytes, field(t_body, "attachedBody").offset, static_cast<std::int32_t>(-1));
		const auto body = writer.add_array(t_body, body_bytes.data(), 1);
		set(system, t_system, "bodyCinfos", body.item);
		set(body, t_body, "name", add_string("triggermodeldummy"));

		const point half_size{0.615f, 0.7712f, 0.4587f};
		std::array<std::array<float, 4>, 8> vertices{};
		const std::array<std::uint8_t, 8> vertex_signs{0, 2, 4, 1, 6, 5, 3, 7};
		for (auto i = 0u; i < vertices.size(); i++)
		{
			const auto signs = vertex_signs[i];
			vertices[i] = {signs & 1 ? -half_size[0] : half_size[0], signs & 2 ? -half_size[1] : half_size[1],
				signs & 4 ? -half_size[2] : half_size[2], std::bit_cast<float>(0x3F000000 | i)};
		}

		const std::array<std::uint8_t, 6> plane_axes{2, 2, 0, 0, 1, 1};
		std::array<std::array<float, 4>, 6> planes{};
		for (auto i = 0u; i < planes.size(); i++)
		{
			planes[i][plane_axes[i]] = i & 1 ? -1.0f : 1.0f;
			planes[i][3] = -half_size[plane_axes[i]];
		}

		const std::array<std::uint8_t, 24> indices{0, 3, 6, 1, 7, 5, 2, 4, 0, 1, 4, 2, 3, 5, 7, 6, 5, 3, 0, 2, 7, 4, 1, 6};
		const auto face_size = writer.type_size(t_face);
		std::vector<std::byte> face_bytes(face_size * planes.size(), std::byte{0});
		for (auto i = 0u; i < planes.size(); i++)
		{
			put(face_bytes, i * face_size + field(t_face, "firstIndex").offset, static_cast<std::uint16_t>(i * 4));
			put(face_bytes, i * face_size + field(t_face, "numIndices").offset, static_cast<std::uint8_t>(4));
			put(face_bytes, i * face_size + field(t_face, "minHalfAngle").offset, static_cast<std::uint8_t>(127));
		}

		std::vector<std::byte> hull_bytes(writer.type_size(t_hull), std::byte{0});
		put(hull_bytes, field(t_hull, "flags").offset, static_cast<std::uint16_t>(0x01C3));
		put(hull_bytes, field(t_hull, "type").offset, static_cast<std::uint8_t>(1));
		put(hull_bytes, field(t_hull, "dispatchType").offset, static_cast<std::uint8_t>(2));
		put(hull_bytes, field(t_hull, "convexRadius").offset, 0.01f);
		put(hull_bytes, field(t_hull, "maxAllowedPenetration").offset, static_cast<std::uint16_t>(0x3E1F));
		const auto hull = writer.add_object(t_hull, hull_bytes.data());
		set(body, t_body, "shape", hull.item);
		set(hull, t_hull, "vertices", writer.add_array(writer.type_id("hkVector4f"), vertices.data(), vertices.size()).item);
		set(hull, t_hull, "planes", writer.add_array(writer.type_id("hkVector4"), planes.data(), planes.size()).item);
		set(hull, t_hull, "faces", writer.add_array(t_face, face_bytes.data(), planes.size()).item);
		set(hull, t_hull, "indices", writer.add_array(writer.type_id("hkUint8"), indices.data(), indices.size()).item);

		std::vector<std::byte> mass_bytes(writer.type_size(t_mass), std::byte{0});
		const auto mass_offset = field(t_mass, "compressedMassProperties").offset;
		put(mass_bytes, mass_offset + field(t_mass_values, "centerOfMass").offset, std::array<std::uint16_t, 4>{0xBEC2, 0x51D3, 0x13FA, 0x2880});
		put(mass_bytes, mass_offset + field(t_mass_values, "inertia").offset, std::array<std::uint16_t, 4>{0x0000, 0x0000, 0x0000, 0x1980});
		put(mass_bytes, mass_offset + field(t_mass_values, "majorAxisSpace").offset, std::array<std::uint16_t, 4>{0x800C, 0x7FED, 0x7FD7, 0xF52F});
		put(mass_bytes, mass_offset + field(t_mass_values, "mass").offset, 5.0f);
		put(mass_bytes, mass_offset + field(t_mass_values, "volume").offset, 1.8309879f);
		const auto mass = writer.add_object(t_mass, mass_bytes.data());

		std::vector<std::byte> entry_bytes(writer.type_size(t_entry), std::byte{0});
		put(entry_bytes, field(t_entry, "key").offset, static_cast<std::uint16_t>(0xF100));
		const auto entry = writer.add_array(t_entry, entry_bytes.data(), 1);
		set(entry, t_entry, "object", mass.item);

		const std::vector<std::byte> properties_bytes(writer.type_size(t_properties), std::byte{0});
		const auto properties = writer.add_object(t_properties, properties_bytes.data());
		set(properties, t_properties, "entries", entry.item);
		set(hull, t_hull, "properties", properties.item);

		return writer.finish(asset.item);
	}

	world_collision build_world_collision(const zonetool::t9::ClipMap* clip_map, const std::vector<gfxworld::placed_model>& models,
		const std::vector<gfxworld::collision_triangle>& world_surface_triangles, const std::vector<mapents::trigger>& triggers,
		const std::string& map_name)
	{
		const auto triangles = collect_world_triangles(clip_map, models, world_surface_triangles);
		if (triangles.empty())
		{
			throw std::runtime_error("havok: the world has no collision triangles");
		}

		mesh_shape mesh{};
		fill_sections(triangles, mesh);

		auto domain = empty_bounds();
		for (const auto& output : mesh.outputs)
		{
			include(domain, output.domain);
		}

		mesh.domain = {{domain.min[0], domain.min[1], domain.min[2], 0.0f}, {domain.max[0], domain.max[1], domain.max[2], 0.0f}};
		ZONETOOL_INFO("havok: %zu world triangles, %zu sections, %zu packed vertices", triangles.size(), mesh.outputs.size(),
			mesh.packed_vertices.size());

		if (mesh.outputs.size() > 32768)
		{
			throw std::runtime_error(std::format("havok: {} sections, the IW8 limit is 32768", mesh.outputs.size()));
		}

		world_collision result{};
		result.world_shape = write_world_list(mesh);
		result.ents_shape = write_ents_list(triggers, map_name);
		return result;
	}
}
