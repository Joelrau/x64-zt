#include "std_include.hpp"
#include "physcollmap.hpp"

namespace zonetool::h1
{
	PhysCollmap* phys_collmap::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "physcollmap\\"s + name + ".pc";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing physcollmap \"%s\"...", name.data());

		auto* asset = read.read_single<PhysCollmap>();
		asset->name = read.read_string();
		asset->geoms = read.read_array<PhysGeomInfo>();

		for (auto i = 0u; i < asset->count; i++)
		{
			asset->geoms[i].data = read.read_single<dmPolytopeData>();
			if (asset->geoms[i].data)
			{
				asset->geoms[i].data->m_aVertices = read.read_array<dmFloat4>();
				asset->geoms[i].data->m_aPlanes = read.read_array<dmPlane>();
				asset->geoms[i].data->m_surfaceTypes = read.read_array<dm_uint16>();
				asset->geoms[i].data->m_vertexMaterials = read.read_array<dm_uint16>();
				asset->geoms[i].data->m_aSubEdges = read.read_array<dmSubEdge>();
				asset->geoms[i].data->m_aFaceSubEdges = read.read_array<dm_uint8>();
			}
		}

		read.close();

		return asset;
	}

	void phys_collmap::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).physCollmap;
		}
	}

	void phys_collmap::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void phys_collmap::load_depending(zone_base* zone)
	{
	}

	std::string phys_collmap::name()
	{
		return this->name_;
	}

	std::int32_t phys_collmap::type()
	{
		return ASSET_TYPE_PHYSCOLLMAP;
	}

	void phys_collmap::read_polytope(zone_reader& reader, dmPolytopeData* polytope)
	{
		reader.read_array(polytope->m_aVertices, 15, polytope->m_vertexCount);
		reader.read_array(polytope->m_aPlanes, 15, polytope->m_faceCount);
		reader.read_array(polytope->m_aSubEdges, 3, polytope->m_subEdgeCount);
		reader.read_array(polytope->m_aFaceSubEdges, 0, polytope->m_faceCount);
		reader.read_array(polytope->m_surfaceTypes, 1, polytope->m_faceCount);
		reader.read_array(polytope->m_vertexMaterials, 1, polytope->m_vertexCount);
	}

	void phys_collmap::read(zone_reader& reader, PhysCollmap* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto geoms = reader.read_array(asset->geoms, 3, asset->count))
		{
			for (auto i = 0u; i < asset->count; i++)
			{
				if (const auto polytope = reader.read_single(geoms[i].data, 3))
				{
					read_polytope(reader, polytope);
				}
			}
		}

		reader.pop_stream();
	}

void phys_collmap::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->geoms)
		{
			buf->align(3);
			auto* data_geoms = data->geoms;
			auto* dest_geoms = buf->write(data_geoms, data->count);
			for (unsigned int i = 0; i < data->count; i++)
			{
				if (data_geoms[i].data)
				{
					buf->align(3);
					auto* data_poly_data = data_geoms[i].data;
					auto* dest_poly_data = buf->write(data_poly_data);

					if (data_poly_data->m_aVertices)
					{
						buf->align(15);
						buf->write(data_poly_data->m_aVertices, data_poly_data->m_vertexCount);
						buf->clear_pointer(&dest_poly_data->m_aVertices);
					}

					if (data_poly_data->m_aPlanes)
					{
						buf->align(15);
						buf->write(data_poly_data->m_aPlanes, data_poly_data->m_faceCount);
						buf->clear_pointer(&dest_poly_data->m_aPlanes);
					}

					if (data_poly_data->m_aSubEdges)
					{
						buf->align(3);
						buf->write(data_poly_data->m_aSubEdges, data_poly_data->m_subEdgeCount);
						buf->clear_pointer(&dest_poly_data->m_aSubEdges);
					}

					if (data_poly_data->m_aFaceSubEdges)
					{
						buf->align(0);
						buf->write(data_poly_data->m_aFaceSubEdges, data_poly_data->m_faceCount);
						buf->clear_pointer(&dest_poly_data->m_aFaceSubEdges);
					}

					if (data_poly_data->m_surfaceTypes)
					{
						buf->align(1);
						buf->write(data_poly_data->m_surfaceTypes, data_poly_data->m_faceCount);
						buf->clear_pointer(&dest_poly_data->m_surfaceTypes);
					}

					if (data_poly_data->m_vertexMaterials)
					{
						buf->align(1);
						buf->write(data_poly_data->m_vertexMaterials, data_poly_data->m_vertexCount);
						buf->clear_pointer(&dest_poly_data->m_vertexMaterials);
					}

					buf->clear_pointer(&dest_geoms[i].data);
				}
			}

			buf->clear_pointer(&dest->geoms);
		}

		buf->pop_stream();
	}

	void phys_collmap::dump(PhysCollmap* asset)
	{
		const auto path = "physcollmap\\"s + asset->name + ".pc";

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_array(asset->geoms, asset->count);

		for (auto i = 0u; i < asset->count; i++)
		{
			const auto data = asset->geoms[i].data;
			write.dump_single(data);
			if (data)
			{
				write.dump_array(data->m_aVertices, data->m_vertexCount);
				write.dump_array(data->m_aPlanes, data->m_faceCount);
				write.dump_array(data->m_surfaceTypes, data->m_faceCount);
				write.dump_array(data->m_vertexMaterials, data->m_vertexCount);
				write.dump_array(data->m_aSubEdges, data->m_subEdgeCount);
				write.dump_array(data->m_aFaceSubEdges, data->m_faceCount);
			}
		}
	}

	namespace polytope
	{
		namespace
		{
			struct polygon_t
			{
				std::vector<vec3> points;
				plane_t plane;
			};

			struct face_t
			{
				std::vector<int> verts;
				plane_t plane;
			};

			vec3 to_vec3(const dmFloat3& v)
			{
				return {v.x, v.y, v.z};
			}

			dmFloat3 to_float3(const vec3& v)
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

					if (!on_plane && result.points.size() >= 3)
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

				std::map<std::pair<int, int>, int> sub_edges;
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

			void set_mass_properties(dmPolytopeData* data, const std::vector<vec3>& verts, const std::vector<face_t>& faces)
			{
				auto volume = 0.0;
				auto area = 0.0;
				vec3 first{};
				vec3 squares{};
				vec3 products{};
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
		}
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

		dmPolytopeData* make_polytope(const vec3& mins, const vec3& maxs, const std::vector<plane_t>& planes,
			const bool material_arrays, utils::memory::allocator& allocator)
		{
			const auto scale = std::max({1.0, std::abs(mins.x), std::abs(mins.y), std::abs(mins.z), std::abs(maxs.x), std::abs(maxs.y), std::abs(maxs.z)});
			const auto epsilon = scale * 1e-5;
			const auto merge_distance = 10.0 * epsilon;
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
			if (verts.size() < 4 || faces.size() < 4 || !sub_edges || verts.size() > 256u
				|| faces.size() > 256u || sub_edges->size() > 256u)
			{
				return nullptr;
			}

			auto* data = allocator.allocate<dmPolytopeData>();
			data->m_vertexCount = static_cast<int>(verts.size());
			data->m_faceCount = static_cast<int>(faces.size());
			data->m_subEdgeCount = static_cast<int>(sub_edges->size());
			data->m_aVertices = allocator.allocate_array<dmFloat4>(verts.size());
			data->m_aPlanes = allocator.allocate_array<dmPlane>(faces.size());
			data->m_aSubEdges = allocator.allocate_array<dmSubEdge>(sub_edges->size());
			data->m_aFaceSubEdges = allocator.allocate_array<dm_uint8>(faces.size());
			if (material_arrays)
			{
				data->m_surfaceTypes = allocator.allocate_array<dm_uint16>(faces.size());
				data->m_vertexMaterials = allocator.allocate_array<dm_uint16>(verts.size());
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
				data->m_aFaceSubEdges[f] = static_cast<dm_uint8>(sub_edges->at({face.verts[0], face.verts[1]}));
				for (auto i = 0u; i < face.verts.size(); i++)
				{
					const auto tail = face.verts[i];
					const auto head = face.verts[(i + 1) % face.verts.size()];
					const auto next = face.verts[(i + 2) % face.verts.size()];
					const auto index = sub_edges->at({tail, head});
					data->m_aSubEdges[index] =
					{
						static_cast<dm_int8>(index % 2 ? -1 : 1),
						static_cast<dm_uint8>(tail),
						static_cast<dm_uint8>(f),
						static_cast<dm_uint8>(sub_edges->at({head, next})),
					};
				}
			}

			set_mass_properties(data, verts, faces);
			data->contents = -1;
			return data->m_volume > 0.0f ? data : nullptr;
		}

		void set_collmap_mass(PhysCollmap* asset)
		{
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
}
