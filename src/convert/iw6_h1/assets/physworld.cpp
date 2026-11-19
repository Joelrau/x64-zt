#include <std_include.hpp>
#include "../include.hpp"
#include "physworld.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace physworld
		{
			namespace
			{
				using point_t = std::array<float, 3>;
				using plane_t = std::array<float, 4>;
				using axial_planes_t = std::array<plane_t, 6>;

				struct mesh_source
				{
					std::vector<point_t> verts;
					std::unordered_map<std::array<int, 3>, int, position_hash> vert_lookup;
					std::vector<triangle_t> triangles;
					std::set<std::array<int, 3>> known_triangles;
				};

				struct tree_node
				{
					point_t lower;
					point_t upper;
					unsigned int axis;
					unsigned int triangle_count;
					unsigned int index;
				};

				point_t subtract(const point_t& a, const point_t& b)
				{
					return {a[0] - b[0], a[1] - b[1], a[2] - b[2]};
				}

				float distance_squared(const point_t& a, const point_t& b)
				{
					const auto delta = subtract(a, b);
					return delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2];
				}

				int add_vertex(mesh_source& mesh, const point_t& point)
				{
					const std::array<int, 3> key{static_cast<int>(std::lround(point[0] * 64.0f)),
						static_cast<int>(std::lround(point[1] * 64.0f)), static_cast<int>(std::lround(point[2] * 64.0f))};
					const auto [entry, inserted] = mesh.vert_lookup.try_emplace(key, static_cast<int>(mesh.verts.size()));
					if (inserted)
					{
						mesh.verts.emplace_back(point);
					}

					return entry->second;
				}

				void add_triangle(mesh_source& mesh, const point_t& a, const point_t& b, const point_t& c, const int material)
				{
					std::array<int, 3> verts{add_vertex(mesh, a), add_vertex(mesh, b), add_vertex(mesh, c)};
					if (verts[0] == verts[1] || verts[1] == verts[2] || verts[2] == verts[0])
					{
						return;
					}

					std::ranges::rotate(verts, std::ranges::min_element(verts));
					if (mesh.known_triangles.insert(verts).second)
					{
						mesh.triangles.push_back({{verts[0], verts[1], verts[2]}, material});
					}
				}

				axial_planes_t get_axial_planes(const zonetool::h1::Bounds& bounds)
				{
					const auto& mid = bounds.midPoint;
					const auto& half = bounds.halfSize;
					return
					{{
						{-1.0f, 0.0f, 0.0f, half[0] - mid[0]},
						{1.0f, 0.0f, 0.0f, mid[0] + half[0]},
						{0.0f, -1.0f, 0.0f, half[1] - mid[1]},
						{0.0f, 1.0f, 0.0f, mid[1] + half[1]},
						{0.0f, 0.0f, -1.0f, half[2] - mid[2]},
						{0.0f, 0.0f, 1.0f, mid[2] + half[2]},
					}};
				}

				plane_t side_plane(const zonetool::h1::ClipInfo* info, const zonetool::h1::cbrush_t& brush, const axial_planes_t& axial_planes, const unsigned int side)
				{
					if (side < 6)
					{
						return axial_planes[side];
					}

					const auto& plane = info->planes[brush.sides[side - 6].planeIndex];
					return {plane.normal[0], plane.normal[1], plane.normal[2], plane.dist};
				}

				int side_material(const zonetool::h1::cbrush_t& brush, const unsigned int side)
				{
					return side < 6 ? brush.axialMaterialNum[side & 1][side >> 1] : brush.sides[side - 6].materialNum;
				}

				bool intersect_planes(const plane_t& a, const plane_t& b, const plane_t& c, point_t& out)
				{
					const point_t bc{b[1] * c[2] - b[2] * c[1], b[2] * c[0] - b[0] * c[2], b[0] * c[1] - b[1] * c[0]};
					const point_t ca{c[1] * a[2] - c[2] * a[1], c[2] * a[0] - c[0] * a[2], c[0] * a[1] - c[1] * a[0]};
					const point_t ab{a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
					const auto det = a[0] * bc[0] + a[1] * bc[1] + a[2] * bc[2];
					if (std::abs(det) < 0.001f)
					{
						return false;
					}

					for (auto i = 0; i < 3; i++)
					{
						out[i] = (a[3] * bc[i] + b[3] * ca[i] + c[3] * ab[i]) / det;
					}

					return true;
				}

				std::vector<point_t> brush_face_winding(const zonetool::h1::ClipInfo* info, const zonetool::h1::cbrush_t& brush,
					const axial_planes_t& axial_planes, const unsigned int side)
				{
					const auto axial = side < 6;
					const auto first_adjacent = axial ? brush.firstAdjacentSideOffsets[side & 1][side >> 1] : brush.sides[side - 6].firstAdjacentSideOffset;
					const auto edge_count = axial ? brush.edgeCount[side & 1][side >> 1] : brush.sides[side - 6].edgeCount;
					if (edge_count < 3)
					{
						return {};
					}

					const auto* adjacent = &brush.baseAdjacentSide[first_adjacent];
					const auto face = side_plane(info, brush, axial_planes, side);
					auto previous = side_plane(info, brush, axial_planes, adjacent[edge_count - 1]);

					std::vector<point_t> points;
					for (auto i = 0; i < edge_count; i++)
					{
						const auto current = side_plane(info, brush, axial_planes, adjacent[i]);
						point_t point{};
						if (intersect_planes(face, previous, current, point) && (points.empty() || distance_squared(points.back(), point) >= 1.0f))
						{
							points.emplace_back(point);
						}

						previous = current;
					}

					if (points.size() > 1 && distance_squared(points.front(), points.back()) < 1.0f)
					{
						points.pop_back();
					}

					if (points.size() < 3)
					{
						points.clear();
					}

					return points;
				}

				void add_brush(mesh_source& mesh, const zonetool::h1::clipMap_t* asset, const int brush_index, const point_t& origin)
				{
					const auto& brush = asset->info.bCollisionData.brushes[brush_index];
					const auto axial_planes = get_axial_planes(asset->info.bCollisionData.brushBounds[brush_index]);
					for (auto side = 0u; side < brush.numsides + 6u; side++)
					{
						const auto winding = brush_face_winding(&asset->info, brush, axial_planes, side);
						for (auto i = 1u; i + 1 < winding.size(); i++)
						{
							add_triangle(mesh, subtract(winding[i + 1], origin), subtract(winding[i], origin), subtract(winding[0], origin), side_material(brush, side));
						}
					}
				}

				void add_partition(mesh_source& mesh, const zonetool::h1::clipMap_t* asset, const int partition_index, const int material, const point_t& origin)
				{
					const auto& data = asset->info.pCollisionData;
					const auto& partition = data.partitions[partition_index];
					const auto* indices = &data.triIndices[3 * partition.firstTri];
					const auto first_vert = 1024 * partition.firstVertSegment;
					const auto vert = [&](const int corner)
					{
						const auto* position = data.verts[indices[corner] + first_vert];
						return subtract({position[0], position[1], position[2]}, origin);
					};

					for (auto i = 0u; i < partition.triCount; i++, indices += 3)
					{
						add_triangle(mesh, vert(2), vert(1), vert(0), material);
					}
				}

				void collect_brushes(const zonetool::h1::cLeafBrushNode_s* node, std::set<int>& brushes)
				{
					if (node->leafBrushCount > 0)
					{
						for (auto i = 0; i < node->leafBrushCount; i++)
						{
							brushes.insert(node->data.leaf.brushes[i]);
						}

						return;
					}

					if (node->leafBrushCount)
					{
						collect_brushes(&node[1], brushes);
					}

					for (const auto child_offset : node->data.children.childOffset)
					{
						if (child_offset)
						{
							collect_brushes(&node[child_offset], brushes);
						}
					}
				}

				void collect_partitions(const zonetool::h1::clipMap_t* asset, const zonetool::h1::CollisionAabbTree& tree, std::map<int, int>& partitions)
				{
					if (!tree.childCount)
					{
						partitions.emplace(tree.u.partitionIndex, tree.materialIndex);
						return;
					}

					for (auto i = 0u; i < tree.childCount; i++)
					{
						collect_partitions(asset, asset->info.pCollisionTree.aabbTrees[tree.u.firstChildIndex + i], partitions);
					}
				}

				void collect_model(const zonetool::h1::clipMap_t* asset, const zonetool::h1::cmodel_t& model, std::set<int>& brushes, std::map<int, int>& partitions)
				{
					collect_brushes(&asset->info.bCollisionTree.leafbrushNodes[model.leaf.leafBrushNode], brushes);
					for (auto i = 0u; i < model.leaf.collAabbCount; i++)
					{
						collect_partitions(asset, asset->info.pCollisionTree.aabbTrees[model.leaf.firstCollAabbIndex + i], partitions);
					}
				}

				std::map<int, point_t> script_brushmodel_origins(const MapEnts* map_ents)
				{
					std::map<int, point_t> origins;
					if (!map_ents || !map_ents->entityString)
					{
						return origins;
					}

					std::unordered_map<std::string, std::string> entity;
					const std::string_view text(map_ents->entityString);
					for (std::size_t position = 0; position < text.size();)
					{
						const auto end = std::min(text.find('\n', position), text.size());
						const auto line = text.substr(position, end - position);
						position = end + 1;

						if (line.starts_with('{'))
						{
							entity.clear();
						}
						else if (line.starts_with('}'))
						{
							const auto model = entity.find("model");
							if (entity["classname"] == "script_brushmodel" && model != entity.end() && model->second.starts_with('*'))
							{
								point_t origin{};
								std::istringstream(entity["origin"]) >> origin[0] >> origin[1] >> origin[2];
								origins.emplace(std::atoi(model->second.data() + 1), origin);
							}
						}
						else if (const auto key_end = line.find('"', 1); line.starts_with('"') && key_end != std::string_view::npos)
						{
							const auto value_start = line.find('"', key_end + 1);
							const auto value_end = value_start == std::string_view::npos ? value_start : line.find('"', value_start + 1);
							if (value_end != std::string_view::npos)
							{
								entity[std::string(line.substr(1, key_end - 1))] = line.substr(value_start + 1, value_end - value_start - 1);
							}
						}
					}

					return origins;
				}

				unsigned int build_tree(const std::vector<point_t>& centroids, const mesh_source& mesh, std::vector<int>& order, const int begin, const int end,
					std::vector<tree_node>& nodes, const int depth, int& max_depth)
				{
					const auto node_index = static_cast<unsigned int>(nodes.size());
					nodes.emplace_back();
					max_depth = std::max(max_depth, depth);

					point_t lower{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
					point_t upper{-lower[0], -lower[1], -lower[2]};
					auto centroid_lower = lower;
					auto centroid_upper = upper;
					for (auto i = begin; i < end; i++)
					{
						for (const auto vert : mesh.triangles[order[i]].verts)
						{
							for (auto axis = 0; axis < 3; axis++)
							{
								lower[axis] = std::min(lower[axis], mesh.verts[vert][axis]);
								upper[axis] = std::max(upper[axis], mesh.verts[vert][axis]);
							}
						}

						for (auto axis = 0; axis < 3; axis++)
						{
							centroid_lower[axis] = std::min(centroid_lower[axis], centroids[order[i]][axis]);
							centroid_upper[axis] = std::max(centroid_upper[axis], centroids[order[i]][axis]);
						}
					}

					nodes[node_index].lower = lower;
					nodes[node_index].upper = upper;
					if (end - begin <= 15)
					{
						nodes[node_index].axis = 3u;
						nodes[node_index].triangle_count = static_cast<unsigned int>(end - begin);
						nodes[node_index].index = static_cast<unsigned int>(begin);
						return node_index;
					}

					auto split_axis = 0u;
					for (auto axis = 1u; axis < 3; axis++)
					{
						if (centroid_upper[axis] - centroid_lower[axis] > centroid_upper[split_axis] - centroid_lower[split_axis])
						{
							split_axis = axis;
						}
					}

					const auto middle = begin + (end - begin) / 2;
					std::nth_element(order.begin() + begin, order.begin() + middle, order.begin() + end, [&](const int a, const int b)
					{
						return centroids[a][split_axis] < centroids[b][split_axis];
					});

					nodes[node_index].axis = split_axis;
					build_tree(centroids, mesh, order, begin, middle, nodes, depth + 1, max_depth);
					nodes[node_index].index = build_tree(centroids, mesh, order, middle, end, nodes, depth + 1, max_depth) - node_index;
					return node_index;
				}

				void write_mesh(const zonetool::h1::clipMap_t* asset, const mesh_source& mesh, zonetool::h1::dmMeshData* data, utils::memory::allocator& allocator)
				{
					std::vector<int> order(mesh.triangles.size());
					std::vector<point_t> centroids(mesh.triangles.size());
					for (auto i = 0u; i < mesh.triangles.size(); i++)
					{
						order[i] = static_cast<int>(i);
						for (const auto vert : mesh.triangles[i].verts)
						{
							for (auto axis = 0; axis < 3; axis++)
							{
								centroids[i][axis] += mesh.verts[vert][axis] / 3.0f;
							}
						}
					}

					std::vector<tree_node> nodes;
					auto max_depth = 0;
					build_tree(centroids, mesh, order, 0, static_cast<int>(order.size()), nodes, 1, max_depth);

					point_t center{};
					point_t extents{};
					point_t unquantize{};
					for (auto axis = 0; axis < 3; axis++)
					{
						center[axis] = (nodes[0].lower[axis] + nodes[0].upper[axis]) * 0.5f;
						extents[axis] = (nodes[0].upper[axis] - nodes[0].lower[axis]) * 0.5f + 0.5f;
						unquantize[axis] = extents[axis] / 32766.0f;
					}

					data->m_center = {center[0], center[1], center[2]};
					data->m_extents = {extents[0], extents[1], extents[2]};
					data->m_unquantize = {unquantize[0], unquantize[1], unquantize[2]};
					data->m_height = max_depth;

					data->m_vertexCount = static_cast<int>(mesh.verts.size());
					data->m_aVertices = allocator.allocate_array<zonetool::h1::dmFloat4>(mesh.verts.size());
					for (auto i = 0u; i < mesh.verts.size(); i++)
					{
						const auto position = subtract(mesh.verts[i], center);
						data->m_aVertices[i] = {position[0], position[1], position[2], 0.0f};
					}

					std::unordered_map<std::uint64_t, int> opposite_verts;
					const auto edge_key = [](const int a, const int b)
					{
						return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(a)) << 32) | static_cast<std::uint32_t>(b);
					};

					for (const auto& triangle : mesh.triangles)
					{
						for (auto i = 0; i < 3; i++)
						{
							const auto [entry, inserted] = opposite_verts.try_emplace(edge_key(triangle.verts[i], triangle.verts[(i + 1) % 3]), triangle.verts[(i + 2) % 3]);
							if (!inserted)
							{
								entry->second = -1;
							}
						}
					}

					data->m_triangleCount = static_cast<int>(mesh.triangles.size());
					data->m_aTriangles = allocator.allocate_array<zonetool::h1::dmMeshTriangle>(mesh.triangles.size());
					for (auto i = 0u; i < order.size(); i++)
					{
						const auto& triangle = mesh.triangles[order[i]];
						const auto has_material = triangle.material >= 0 && triangle.material < static_cast<int>(asset->info.numMaterials);
						const auto material = has_material ? asset->info.materials[triangle.material] : zonetool::h1::ClipMaterial{};
						const auto winged = [&](const int a, const int b)
						{
							const auto opposite = opposite_verts.find(edge_key(b, a));
							return opposite != opposite_verts.end() ? opposite->second : -1;
						};

						auto& out = data->m_aTriangles[i];
						out.i1 = triangle.verts[0];
						out.i2 = triangle.verts[1];
						out.i3 = triangle.verts[2];
						out.w1 = winged(triangle.verts[0], triangle.verts[1]);
						out.w2 = winged(triangle.verts[1], triangle.verts[2]);
						out.w3 = winged(triangle.verts[2], triangle.verts[0]);
						out.materialIndex = (material.surfaceFlags >> 20) & 0x3F;
						out.collisionFlags = material.contents;
						data->contents |= material.contents;
					}

					const auto quantize = [&](const float value, const int axis, const bool upper)
					{
						const auto relative = value - center[axis];
						auto steps = upper ? std::ceil(relative / unquantize[axis]) : std::floor(relative / unquantize[axis]);
						if (upper ? steps * unquantize[axis] < relative : steps * unquantize[axis] > relative)
						{
							steps += upper ? 1.0f : -1.0f;
						}

						return static_cast<zonetool::h1::dm_int16>(std::clamp(steps, -32767.0f, 32767.0f));
					};

					data->m_nodeCount = static_cast<int>(nodes.size());
					data->m_pRoot = allocator.allocate_array<zonetool::h1::dmMeshNode>(nodes.size());
					for (auto i = 0u; i < nodes.size(); i++)
					{
						auto& out = data->m_pRoot[i];
						out.lowerX = quantize(nodes[i].lower[0], 0, false);
						out.lowerY = quantize(nodes[i].lower[1], 1, false);
						out.lowerZ = quantize(nodes[i].lower[2], 2, false);
						out.upperX = quantize(nodes[i].upper[0], 0, true);
						out.upperY = quantize(nodes[i].upper[1], 1, true);
						out.upperZ = quantize(nodes[i].upper[2], 2, true);
						out.anon.fields.axis = nodes[i].axis;
						out.anon.fields.triangleCount = nodes[i].triangle_count;
						out.anon.fields.index = nodes[i].index;
					}
				}
			}

			zonetool::h1::PhysWorld* generate(zonetool::h1::clipMap_t* asset, const MapEnts* map_ents, utils::memory::allocator& allocator)
			{
				std::set<int> model_brushes;
				std::map<int, int> model_partitions;
				for (auto i = 0u; i < asset->numSubModels; i++)
				{
					collect_model(asset, asset->cmodels[i], model_brushes, model_partitions);
				}

				std::vector<mesh_source> meshes(1);
				for (auto i = 0; i < static_cast<int>(asset->info.bCollisionData.numBrushes); i++)
				{
					if (!model_brushes.contains(i))
					{
						add_brush(meshes[0], asset, i, {});
					}
				}

				std::map<int, int> partition_materials;
				for (auto i = 0; i < asset->info.pCollisionTree.aabbTreeCount; i++)
				{
					const auto& tree = asset->info.pCollisionTree.aabbTrees[i];
					if (!tree.childCount)
					{
						partition_materials.emplace(tree.u.partitionIndex, tree.materialIndex);
					}
				}

				for (auto i = 0; i < asset->info.pCollisionData.partitionCount; i++)
				{
					if (!model_partitions.contains(i))
					{
						add_partition(meshes[0], asset, i, partition_materials[i], {});
					}
				}

				auto* new_asset = allocator.allocate<zonetool::h1::PhysWorld>();
				new_asset->name = allocator.duplicate_string(asset->name);
				new_asset->brushModelCount = asset->numSubModels;
				new_asset->brushModels = allocator.allocate_array<zonetool::h1::PhysBrushModel>(new_asset->brushModelCount);
				for (auto i = 0u; i < new_asset->brushModelCount; i++)
				{
					new_asset->brushModels[i].fields = {-1, -1, 0, static_cast<short>(i == 0 && !meshes[0].triangles.empty() ? 0 : -1)};
				}

				for (const auto& [model, origin] : script_brushmodel_origins(map_ents))
				{
					if (model <= 0 || model >= static_cast<int>(asset->numSubModels) || new_asset->brushModels[model].fields.meshIndex >= 0)
					{
						continue;
					}

					std::set<int> brushes;
					std::map<int, int> partitions;
					collect_model(asset, asset->cmodels[model], brushes, partitions);

					mesh_source mesh;
					for (const auto brush : brushes)
					{
						add_brush(mesh, asset, brush, origin);
					}

					for (const auto& [partition, material] : partitions)
					{
						add_partition(mesh, asset, partition, material, origin);
					}

					if (!mesh.triangles.empty())
					{
						new_asset->brushModels[model].fields.meshIndex = static_cast<short>(meshes.size());
						meshes.emplace_back(std::move(mesh));
					}
				}

				if (meshes[0].triangles.empty())
				{
					meshes.erase(meshes.begin());
					for (auto i = 1u; i < new_asset->brushModelCount; i++)
					{
						if (new_asset->brushModels[i].fields.meshIndex > 0)
						{
							new_asset->brushModels[i].fields.meshIndex--;
						}
					}
				}

				new_asset->meshDataCount = static_cast<unsigned int>(meshes.size());
				new_asset->meshDatas = allocator.allocate_array<zonetool::h1::dmMeshData>(meshes.size());
				for (auto i = 0u; i < meshes.size(); i++)
				{
					write_mesh(asset, meshes[i], &new_asset->meshDatas[i], allocator);
				}

				ZONETOOL_INFO("physworld \"%s\": %u meshes, %zu world triangles", asset->name, new_asset->meshDataCount, meshes.empty() ? 0 : meshes[0].triangles.size());
				return new_asset;
			}
		}
	}
}
