#include <std_include.hpp>
#include "../include.hpp"
#include "physcollmap.hpp"

#include <h1/assets/physcollmap.hpp>

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace physcollmap
		{
			namespace
			{
				constexpr auto cylinder_sides = 16;
				constexpr auto min_box_half_size = 0.19685; // retail SetAsBox clamp
				constexpr auto relative_epsilon = 1e-5;
				constexpr auto max_polytope_elements = 256u; // sub edge, vertex and face indices are 8 bit

				struct vec3
				{
					double x;
					double y;
					double z;
				};

				struct plane_t
				{
					vec3 normal;
					double offset;
				};

				struct polygon_t
				{
					std::vector<vec3> points; // counter-clockwise around the outward normal
					plane_t plane;
				};

				struct face_t
				{
					std::vector<int> verts;
					plane_t plane;
				};

				vec3 operator+(const vec3& a, const vec3& b)
				{
					return {a.x + b.x, a.y + b.y, a.z + b.z};
				}

				vec3 operator-(const vec3& a, const vec3& b)
				{
					return {a.x - b.x, a.y - b.y, a.z - b.z};
				}

				vec3 operator*(const vec3& a, const double scale)
				{
					return {a.x * scale, a.y * scale, a.z * scale};
				}

				double dot(const vec3& a, const vec3& b)
				{
					return a.x * b.x + a.y * b.y + a.z * b.z;
				}

				vec3 cross(const vec3& a, const vec3& b)
				{
					return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
				}

				vec3 normalize(const vec3& a)
				{
					return a * (1.0 / std::sqrt(dot(a, a)));
				}

				vec3 to_vec3(const float* v)
				{
					return {v[0], v[1], v[2]};
				}

				vec3 to_vec3(const zonetool::h1::dmFloat3& v)
				{
					return {v.x, v.y, v.z};
				}

				zonetool::h1::dmFloat3 to_float3(const vec3& v)
				{
					return {static_cast<float>(v.x), static_cast<float>(v.y), static_cast<float>(v.z)};
				}

				void store(const vec3& v, float* out)
				{
					out[0] = static_cast<float>(v.x);
					out[1] = static_cast<float>(v.y);
					out[2] = static_cast<float>(v.z);
				}

				double distance(const plane_t& plane, const vec3& point)
				{
					return dot(plane.normal, point) - plane.offset;
				}

				bool is_near(const vec3& a, const vec3& b, const double epsilon)
				{
					const auto delta = a - b;
					return dot(delta, delta) < epsilon * epsilon;
				}

				std::vector<polygon_t> box_polygons(const vec3& mins, const vec3& maxs)
				{
					const vec3 corners[8]
					{
						{mins.x, mins.y, mins.z}, {maxs.x, mins.y, mins.z}, {maxs.x, maxs.y, mins.z}, {mins.x, maxs.y, mins.z},
						{mins.x, mins.y, maxs.z}, {maxs.x, mins.y, maxs.z}, {maxs.x, maxs.y, maxs.z}, {mins.x, maxs.y, maxs.z},
					};

					return
					{
						{{corners[1], corners[2], corners[6], corners[5]}, {{1.0, 0.0, 0.0}, maxs.x}},
						{{corners[0], corners[4], corners[7], corners[3]}, {{-1.0, 0.0, 0.0}, -mins.x}},
						{{corners[2], corners[3], corners[7], corners[6]}, {{0.0, 1.0, 0.0}, maxs.y}},
						{{corners[0], corners[1], corners[5], corners[4]}, {{0.0, -1.0, 0.0}, -mins.y}},
						{{corners[4], corners[5], corners[6], corners[7]}, {{0.0, 0.0, 1.0}, maxs.z}},
						{{corners[0], corners[3], corners[2], corners[1]}, {{0.0, 0.0, -1.0}, -mins.z}},
					};
				}

				bool is_same_plane(const plane_t& a, const plane_t& b, const std::vector<polygon_t>& box, const double tolerance)
				{
					return dot(a.normal, b.normal) > 0.0 && std::ranges::all_of(box, [&](const polygon_t& face)
					{
						return std::ranges::all_of(face.points, [&](const vec3& point) { return std::abs(distance(a, point) - distance(b, point)) < tolerance; });
					});
				}

				void sort_counter_clockwise(std::vector<vec3>& points, const vec3& normal)
				{
					vec3 center{};
					for (const auto& point : points)
					{
						center = center + point;
					}

					center = center * (1.0 / static_cast<double>(points.size()));
					const auto u = normalize(cross(std::abs(normal.z) < 0.9 ? vec3{0.0, 0.0, 1.0} : vec3{1.0, 0.0, 0.0}, normal));
					const auto w = cross(normal, u);
					std::ranges::sort(points, {}, [&](const vec3& point)
					{
						const auto delta = point - center;
						return std::atan2(dot(delta, w), dot(delta, u));
					});
				}

				std::vector<polygon_t> clip(const std::vector<polygon_t>& polygons, const plane_t& plane, const double epsilon)
				{
					// points within epsilon of the plane are on it and never split, so neighbour faces always agree
					const auto is_cut = [&](const polygon_t& polygon)
					{
						return std::ranges::any_of(polygon.points, [&](const vec3& point) { return distance(plane, point) > epsilon; });
					};

					if (std::ranges::none_of(polygons, is_cut))
					{
						return polygons;
					}

					std::vector<polygon_t> clipped;
					std::vector<vec3> cap;
					const auto add_cap_point = [&](const vec3& point)
					{
						if (std::ranges::none_of(cap, [&](const vec3& cap_point) { return is_near(cap_point, point, epsilon); }))
						{
							cap.emplace_back(point);
						}
					};

					for (const auto& polygon : polygons)
					{
						polygon_t result{{}, polygon.plane};
						auto on_plane = true;
						for (auto i = 0u; i < polygon.points.size(); i++)
						{
							const auto& a = polygon.points[i];
							const auto& b = polygon.points[(i + 1) % polygon.points.size()];
							const auto da = distance(plane, a);
							const auto db = distance(plane, b);
							on_plane = on_plane && std::abs(da) <= epsilon;
							if (da <= epsilon)
							{
								result.points.emplace_back(a);
								if (da >= -epsilon)
								{
									add_cap_point(a);
								}
							}

							if ((da < -epsilon && db > epsilon) || (da > epsilon && db < -epsilon))
							{
								const auto point = a + (b - a) * (da / (da - db));
								result.points.emplace_back(point);
								add_cap_point(point);
							}
						}

						if (!on_plane && result.points.size() >= 3) // a polygon on the plane comes back as the cap
						{
							clipped.emplace_back(std::move(result));
						}
					}

					if (cap.size() >= 3)
					{
						sort_counter_clockwise(cap, plane.normal);
						clipped.push_back({std::move(cap), plane});
					}

					return clipped;
				}

				std::vector<face_t> index_faces(const std::vector<polygon_t>& polygons, std::vector<vec3>& verts, const double merge_distance)
				{
					std::vector<face_t> faces;
					for (const auto& polygon : polygons)
					{
						face_t face{{}, polygon.plane};
						for (const auto& point : polygon.points)
						{
							const auto vert = std::ranges::find_if(verts, [&](const vec3& v) { return is_near(v, point, merge_distance); });
							const auto index = static_cast<int>(vert - verts.begin());
							if (vert == verts.end())
							{
								verts.emplace_back(point);
							}

							if (face.verts.empty() || face.verts.back() != index)
							{
								face.verts.emplace_back(index);
							}
						}

						if (face.verts.size() > 1 && face.verts.front() == face.verts.back())
						{
							face.verts.pop_back();
						}

						faces.emplace_back(std::move(face));
					}

					return faces;
				}

				bool is_flat(const std::vector<vec3>& verts, const std::vector<int>& face, const double epsilon)
				{
					vec3 normal{};
					auto length = 0.0;
					const auto& origin = verts[face[0]];
					for (auto i = 1u; i < face.size(); i++)
					{
						const auto delta = verts[face[i]] - origin;
						length = std::max(length, std::sqrt(dot(delta, delta)));
						if (i + 1 < face.size())
						{
							normal = normal + cross(delta, verts[face[i + 1]] - origin);
						}
					}

					return std::sqrt(dot(normal, normal)) < epsilon * length;
				}

				void remove_degenerate_faces(const std::vector<vec3>& verts, std::vector<face_t>& faces, const double epsilon)
				{
					// a flat face and a vertex on fewer than three faces are an edge seen from the side
					for (auto removed = true; removed;)
					{
						std::erase_if(faces, [&](const face_t& face) { return face.verts.size() < 3 || is_flat(verts, face.verts, epsilon); });

						std::vector<int> face_count(verts.size());
						for (const auto& face : faces)
						{
							for (const auto vert : face.verts)
							{
								face_count[vert]++;
							}
						}

						removed = false;
						for (auto& face : faces)
						{
							removed = std::erase_if(face.verts, [&](const int vert) { return face_count[vert] < 3; }) > 0 || removed;
						}
					}
				}

				void remove_unused_vertices(std::vector<vec3>& verts, std::vector<face_t>& faces)
				{
					std::vector<int> remap(verts.size(), -1);
					std::vector<vec3> used;
					for (auto& face : faces)
					{
						for (auto& vert : face.verts)
						{
							if (remap[vert] < 0)
							{
								remap[vert] = static_cast<int>(used.size());
								used.emplace_back(verts[vert]);
							}

							vert = remap[vert];
						}
					}

					verts = std::move(used);
				}

				std::optional<std::map<std::pair<int, int>, int>> index_sub_edges(const std::vector<face_t>& faces)
				{
					std::set<std::pair<int, int>> edges;
					for (const auto& face : faces)
					{
						for (auto i = 0u; i < face.verts.size(); i++)
						{
							if (!edges.emplace(face.verts[i], face.verts[(i + 1) % face.verts.size()]).second)
							{
								return {};
							}
						}
					}

					std::map<std::pair<int, int>, int> sub_edges; // twins are adjacent, so twinOffset is 1 or -1 like retail
					for (const auto& [tail, head] : edges)
					{
						if (!edges.contains({head, tail}))
						{
							return {};
						}

						if (!sub_edges.contains({tail, head}))
						{
							const auto index = static_cast<int>(sub_edges.size());
							sub_edges[{tail, head}] = index;
							sub_edges[{head, tail}] = index + 1;
						}
					}

					return sub_edges;
				}

				void set_mass_properties(zonetool::h1::dmPolytopeData* data, const std::vector<vec3>& verts, const std::vector<face_t>& faces)
				{
					// unit density and inertia about the model origin, like retail polytopes
					auto volume = 0.0;
					auto area = 0.0;
					vec3 first{};
					vec3 squares{}; // xx yy zz
					vec3 products{}; // xy zx yz
					for (const auto& face : faces)
					{
						const auto& a = verts[face.verts[0]];
						for (auto i = 1u; i + 1 < face.verts.size(); i++)
						{
							const auto& b = verts[face.verts[i]];
							const auto& c = verts[face.verts[i + 1]];
							const auto normal = cross(b - a, c - a);
							area += std::sqrt(dot(normal, normal)) / 2.0;

							const auto det = dot(a, cross(b, c));
							const auto s = a + b + c;
							volume += det / 6.0;
							first = first + s * det;
							squares = squares + vec3{s.x * s.x + a.x * a.x + b.x * b.x + c.x * c.x,
								s.y * s.y + a.y * a.y + b.y * b.y + c.y * c.y, s.z * s.z + a.z * a.z + b.z * b.z + c.z * c.z} * det;
							products = products + vec3{s.x * s.y + a.x * a.y + b.x * b.y + c.x * c.y,
								s.z * s.x + a.z * a.x + b.z * b.x + c.z * c.x, s.y * s.z + a.y * a.z + b.y * b.z + c.y * c.z} * det;
						}
					}

					squares = squares * (1.0 / 120.0);
					data->m_volume = static_cast<float>(volume);
					data->m_area = static_cast<float>(area);
					data->m_centroid = to_float3(first * (1.0 / (24.0 * volume)));
					data->m_inertiaMoments = to_float3({squares.y + squares.z, squares.x + squares.z, squares.x + squares.y});
					data->m_inertiaProducts = to_float3(products * (-1.0 / 120.0));
				}

				zonetool::h1::dmPolytopeData* make_polytope(const vec3& mins, const vec3& maxs, const std::vector<plane_t>& planes,
					const bool material_arrays, utils::memory::allocator& allocator)
				{
					const auto scale = std::max({1.0, std::abs(mins.x), std::abs(mins.y), std::abs(mins.z), std::abs(maxs.x), std::abs(maxs.y), std::abs(maxs.z)});
					const auto epsilon = scale * relative_epsilon;
					const auto merge_distance = 10.0 * epsilon; // planes and vertices closer than this leave overlapping faces and tiny edges
					const auto box = box_polygons(mins, maxs);

					std::vector<plane_t> cuts;
					for (const auto& plane : planes)
					{
						const auto is_same = [&](const plane_t& other) { return is_same_plane(plane, other, box, merge_distance); };
						if (std::ranges::none_of(box, is_same, &polygon_t::plane) && std::ranges::none_of(cuts, is_same))
						{
							cuts.emplace_back(plane);
						}
					}

					auto polygons = box;
					for (const auto& plane : cuts)
					{
						polygons = clip(polygons, plane, epsilon);
					}

					std::vector<vec3> verts;
					auto faces = index_faces(polygons, verts, merge_distance);
					remove_degenerate_faces(verts, faces, epsilon);
					remove_unused_vertices(verts, faces);

					const auto sub_edges = index_sub_edges(faces);
					if (verts.size() < 4 || faces.size() < 4 || !sub_edges || verts.size() > max_polytope_elements
						|| faces.size() > max_polytope_elements || sub_edges->size() > max_polytope_elements)
					{
						return nullptr;
					}

					auto* data = allocator.allocate<zonetool::h1::dmPolytopeData>();
					data->m_vertexCount = static_cast<int>(verts.size());
					data->m_faceCount = static_cast<int>(faces.size());
					data->m_subEdgeCount = static_cast<int>(sub_edges->size());
					data->m_aVertices = allocator.allocate_array<zonetool::h1::dmFloat4>(verts.size());
					data->m_aPlanes = allocator.allocate_array<zonetool::h1::dmPlane>(faces.size());
					data->m_aSubEdges = allocator.allocate_array<zonetool::h1::dmSubEdge>(sub_edges->size());
					data->m_aFaceSubEdges = allocator.allocate_array<zonetool::h1::dm_uint8>(faces.size());
					if (material_arrays) // retail boxes and cylinders have none, brush hulls have zeroed arrays
					{
						data->m_surfaceTypes = allocator.allocate_array<zonetool::h1::dm_uint16>(faces.size());
						data->m_vertexMaterials = allocator.allocate_array<zonetool::h1::dm_uint16>(verts.size());
					}

					for (auto i = 0u; i < verts.size(); i++)
					{
						const auto vert = to_float3(verts[i]);
						data->m_aVertices[i] = {vert.x, vert.y, vert.z, 0.0f};
					}

					for (auto f = 0u; f < faces.size(); f++)
					{
						const auto& face = faces[f];
						data->m_aPlanes[f] = {to_float3(face.plane.normal), static_cast<float>(face.plane.offset)};
						data->m_aFaceSubEdges[f] = static_cast<zonetool::h1::dm_uint8>(sub_edges->at({face.verts[0], face.verts[1]}));
						for (auto i = 0u; i < face.verts.size(); i++)
						{
							const auto tail = face.verts[i];
							const auto head = face.verts[(i + 1) % face.verts.size()];
							const auto next = face.verts[(i + 2) % face.verts.size()];
							const auto index = sub_edges->at({tail, head});
							data->m_aSubEdges[index] =
							{
								static_cast<zonetool::h1::dm_int8>(index % 2 ? -1 : 1),
								static_cast<zonetool::h1::dm_uint8>(tail),
								static_cast<zonetool::h1::dm_uint8>(f),
								static_cast<zonetool::h1::dm_uint8>(sub_edges->at({head, next})),
							};
						}
					}

					set_mass_properties(data, verts, faces);
					data->contents = -1;
					return data->m_volume > 0.0f ? data : nullptr;
				}

				zonetool::h1::dmPolytopeData* make_brush(const BrushWrapper& brush, utils::memory::allocator& allocator)
				{
					std::vector<plane_t> sides; // the axial sides are the bounds, they are not in the side list
					for (auto i = 0; brush.brush.sides && i < brush.brush.numsides; i++)
					{
						if (const auto* side = brush.brush.sides[i].plane)
						{
							sides.push_back({to_vec3(side->normal), side->dist});
						}
					}

					const auto center = to_vec3(brush.bounds.midPoint);
					const auto half_size = to_vec3(brush.bounds.halfSize);
					return make_polytope(center - half_size, center + half_size, sides, true, allocator);
				}

				zonetool::h1::dmPolytopeData* make_box(const PhysGeomInfo& geom, utils::memory::allocator& allocator)
				{
					const auto center = to_vec3(geom.bounds.midPoint);
					std::vector<plane_t> planes;
					vec3 extent{};
					for (auto i = 0; i < 3; i++)
					{
						const auto axis = normalize(to_vec3(geom.orientation[i]));
						const auto half_size = std::max(static_cast<double>(geom.bounds.halfSize[i]), min_box_half_size);
						planes.push_back({axis, dot(axis, center) + half_size});
						planes.push_back({axis * -1.0, half_size - dot(axis, center)});
						extent = extent + vec3{std::abs(axis.x), std::abs(axis.y), std::abs(axis.z)} * half_size;
					}

					return make_polytope(center - extent, center + extent, planes, false, allocator);
				}

				zonetool::h1::dmPolytopeData* make_cylinder(const PhysGeomInfo& geom, utils::memory::allocator& allocator)
				{
					// iw6 Phys_DistanceOfCylinderFromPlane: the axis is orientation[0], halfSize[0] the radius and halfSize[2] the half height
					const auto center = to_vec3(geom.bounds.midPoint);
					const auto axis = normalize(to_vec3(geom.orientation[0]));
					const auto side = normalize(to_vec3(geom.orientation[1]));
					const auto up = normalize(to_vec3(geom.orientation[2]));
					const double radius = geom.bounds.halfSize[0];
					const double half_height = geom.bounds.halfSize[2];
					const auto step = 2.0 * std::numbers::pi / cylinder_sides;

					std::vector<plane_t> planes;
					for (auto i = 0; i < cylinder_sides; i++)
					{
						const auto angle = (i + 0.5) * step - std::numbers::pi / 2.0; // retail SetAsCylinder starts its vertices at -90 degrees
						const auto normal = side * std::cos(angle) + up * std::sin(angle);
						planes.push_back({normal, dot(normal, center) + radius * std::cos(step / 2.0)});
					}

					planes.push_back({axis * -1.0, half_height - dot(axis, center)});
					planes.push_back({axis, half_height + dot(axis, center)});

					const auto disc_extent = [&](const double axis_part) { return std::sqrt(std::max(0.0, 1.0 - axis_part * axis_part)) * radius; };
					const vec3 extent{std::abs(axis.x) * half_height + disc_extent(axis.x), std::abs(axis.y) * half_height + disc_extent(axis.y),
						std::abs(axis.z) * half_height + disc_extent(axis.z)};
					return make_polytope(center - extent, center + extent, planes, false, allocator);
				}

				zonetool::h1::dmPolytopeData* convert_geom(const PhysCollmap* asset, const PhysGeomInfo& geom, utils::memory::allocator& allocator)
				{
					// iw6 Phys_CollideWorldWithCollmap: a brush wrapper makes a brush, type box a box and every other type a cylinder
					auto* polytope = geom.brushWrapper ? make_brush(*geom.brushWrapper, allocator)
						: geom.type == PHYS_GEOM_BOX ? make_box(geom, allocator) : make_cylinder(geom, allocator);
					if (!polytope)
					{
						ZONETOOL_WARNING("physcollmap \"%s\": dropping a geom of type %d that makes no closed hull", asset->name, geom.type);
					}

					return polytope;
				}

				void set_collmap_mass(zonetool::h1::PhysCollmap* asset)
				{
					// retail collmaps store the polytope sums per unit volume and the bounds of all vertices
					auto volume = 0.0;
					vec3 center{};
					vec3 moments{};
					vec3 products{};
					vec3 mins{std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
					vec3 maxs = mins * -1.0;
					for (auto i = 0u; i < asset->count; i++)
					{
						const auto* data = asset->geoms[i].data;
						volume += data->m_volume;
						center = center + to_vec3(data->m_centroid) * data->m_volume;
						moments = moments + to_vec3(data->m_inertiaMoments);
						products = products + to_vec3(data->m_inertiaProducts);
						for (auto v = 0; v < data->m_vertexCount; v++)
						{
							const vec3 vert{data->m_aVertices[v].x, data->m_aVertices[v].y, data->m_aVertices[v].z};
							mins = {std::min(mins.x, vert.x), std::min(mins.y, vert.y), std::min(mins.z, vert.z)};
							maxs = {std::max(maxs.x, vert.x), std::max(maxs.y, vert.y), std::max(maxs.z, vert.z)};
						}
					}

					store(center * (1.0 / volume), asset->mass.centerOfMass);
					store(moments * (1.0 / volume), asset->mass.momentsOfInertia);
					store(products * (1.0 / volume), asset->mass.productsOfInertia);
					store((mins + maxs) * 0.5, asset->bounds.midPoint);
					store((maxs - mins) * 0.5, asset->bounds.halfSize);
				}
			}

			zonetool::h1::PhysCollmap* convert(PhysCollmap* asset, utils::memory::allocator& allocator)
			{
				std::vector<zonetool::h1::dmPolytopeData*> polytopes;
				for (auto i = 0u; asset->geoms && i < asset->count; i++)
				{
					if (auto* polytope = convert_geom(asset, asset->geoms[i], allocator))
					{
						polytopes.emplace_back(polytope);
					}
				}

				if (polytopes.empty())
				{
					ZONETOOL_ERROR("physcollmap \"%s\": no geom could be converted", asset->name);
					return nullptr;
				}

				auto* new_asset = allocator.allocate<zonetool::h1::PhysCollmap>();
				new_asset->name = asset->name;
				new_asset->count = static_cast<unsigned int>(polytopes.size());
				new_asset->geoms = allocator.allocate_array<zonetool::h1::PhysGeomInfo>(polytopes.size());
				for (auto i = 0u; i < new_asset->count; i++)
				{
					new_asset->geoms[i].data = polytopes[i];
				}

				set_collmap_mass(new_asset);
				return new_asset;
			}

			void dump(PhysCollmap* asset)
			{
				utils::memory::allocator allocator;
				if (auto* converted_asset = convert(asset, allocator))
				{
					zonetool::h1::phys_collmap::dump(converted_asset);
				}
			}
		}
	}
}
