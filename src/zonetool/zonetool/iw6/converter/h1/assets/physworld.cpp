#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "physworld.hpp"

#pragma warning(push)
#pragma warning(disable: 4459)
#include "zonetool/utils/earcut.hpp"
#pragma warning(pop)

#include <utils/string.hpp>
#include <utils/cryptography.hpp>

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace physworld
		{
			namespace
			{
				struct mesh_node
				{
					float lower[3];
					float upper[3];
					bool is_max;
					zonetool::h1::dmMeshNode node;
				};

				struct bounding_box
				{
					float lower[3];
					float upper[3];
				};

				struct triangle_t
				{
					int index;
					int verts[3];
				};

				struct polygon_t
				{
					float(*pts)[3];
					unsigned int ptCount;
				};

				struct axial_planes_t
				{
					float planes[6][4];
				};

				void get_winding_for_brush_face(zonetool::h1::ClipInfo* info, const zonetool::h1::Bounds* bounds,
					const zonetool::h1::cbrush_t* brush, unsigned int brush_side,
					polygon_t* out_winding, int max_verts, const axial_planes_t* axial_planes)
				{
					float plane_1[4]{};
					float plane_2[4]{};
					float plane_3[4]{};

					float v38;
					float v39;
					float v40;
					float v41;
					float v43;
					float v44;
					float v45;
					float v51;
					float v52;
					float v53;

					auto adjacent_side_offset = 0;
					auto edge_count = 0;

					if (brush_side >= 6)
					{
						const auto side = &brush->sides[brush_side - 6];
						adjacent_side_offset = side->firstAdjacentSideOffset;
						edge_count = side->edgeCount;

						const auto plane = &info->planes[side->planeIndex];
						plane_1[0] = plane->normal[0];
						plane_1[1] = plane->normal[1];
						plane_1[2] = plane->normal[2];
						plane_1[3] = plane->dist;
					}
					else
					{
						const auto index = 3 * (brush_side & 1) + (brush_side >> 1);
						adjacent_side_offset = brush->firstAdjacentSideOffsets[0][index];
						edge_count = brush->edgeCount[0][index];

						plane_1[0] = axial_planes->planes[brush_side][0];
						plane_1[1] = axial_planes->planes[brush_side][1];
						plane_1[2] = axial_planes->planes[brush_side][2];
						plane_1[3] = axial_planes->planes[brush_side][3];
					}

					v51 = plane_1[3];
					v52 = plane_1[2];
					v53 = plane_1[0];

					if (edge_count < 3 || edge_count > max_verts)
					{
						out_winding->ptCount = 0;
						return;
					}

					const auto adjacent_side = &brush->baseAdjacentSide[adjacent_side_offset];
					const auto side_index = adjacent_side[edge_count - 1];

					if (adjacent_side[edge_count - 1] >= 6u)
					{
						const auto plane = &info->planes[brush->sides[(side_index - 6)].planeIndex];
						plane_2[0] = plane->normal[0];
						plane_2[1] = plane->normal[1];
						plane_2[2] = plane->normal[2];
						plane_2[3] = plane->dist;
					}
					else
					{
						plane_2[0] = axial_planes->planes[side_index][0];
						plane_2[1] = axial_planes->planes[side_index][1];
						plane_2[2] = axial_planes->planes[side_index][2];
						plane_2[3] = axial_planes->planes[side_index][3];
					}

					out_winding->ptCount = 0;
					const auto points = out_winding->pts;
					auto current_point = 0;

					for (auto i = 0; i < edge_count; i++)
					{
						const auto current_side = adjacent_side[i];
						if (current_side >= 6u)
						{
							const auto plane = &info->planes[brush->sides[(current_side - 6)].planeIndex];
							plane_3[0] = plane->normal[0];
							plane_3[1] = plane->normal[1];
							plane_3[2] = plane->normal[2];
							plane_3[3] = plane->dist;
						}
						else
						{
							plane_3[0] = axial_planes->planes[current_side][0];
							plane_3[1] = axial_planes->planes[current_side][1];
							plane_3[2] = axial_planes->planes[current_side][2];
							plane_3[3] = axial_planes->planes[current_side][3];
						}

						v38 = (plane_3[2] * plane_2[1]) - (plane_2[2] * plane_3[1]);
						v39 = (plane_1[2] * plane_3[1]) - (plane_1[1] * plane_3[2]);
						v40 = (plane_1[1] * plane_2[2]) - (plane_1[2] * plane_2[1]);
						v41 = (plane_2[0] * v39) + (plane_1[0] * v38) + (plane_3[0] * v40);

						const auto abs = std::fabs(v41);
						if (abs >= 0.001f)
						{
							points[current_point][0] = (((v39 * plane_2[3]) + (v38 * v51)) + (v40 * plane_3[3])) * (1.f / v41);
							points[current_point][1] = (((((v53 * plane_3[2]) - (plane_3[0] * v52)) * plane_2[3]) + (((plane_3[0] * plane_2[2]) - (plane_2[0] * plane_3[2])) * v51))
								+ (((plane_2[0] * v52) - (v53 * plane_2[2])) * plane_3[3]))
								* (1.f / v41);
							points[current_point][2] = (((((plane_3[0] * plane_1[1]) - (v53 * plane_3[1])) * plane_2[3]) + (((plane_2[0] * plane_3[1]) - (plane_3[0] * plane_2[1])) * v51))
								+ (((v53 * plane_2[1]) - (plane_2[0] * plane_1[1])) * plane_3[3]))
								* (1.f / v41);

							if (!current_point
								|| (v43 = points[(current_point - 1)][0],
								v44 = points[(current_point - 1)][1] - points[current_point][1],
								v45 = points[(current_point - 1)][2] - points[current_point][2],
								(((v44 * v44) + ((v43 - points[current_point][0]) * (v43 - points[current_point][0]))) + (v45 * v45)) >= 1.0))
							{
								current_point++;
							}

							plane_1[0] = v53;
						}

						plane_1[2] = v52;
						plane_2[0] = plane_3[0];
						plane_2[1] = plane_3[1];
						plane_2[2] = plane_3[2];
						plane_2[3] = plane_3[3];
					}

					out_winding->ptCount = current_point;
					const auto point_count = out_winding->ptCount;

					if (point_count > 1)
					{
						const auto pt_index = point_count - 1;

						float point[3]{};
						point[0] = out_winding->pts[0][0] - out_winding->pts[pt_index][0];
						point[1] = out_winding->pts[0][1] - out_winding->pts[pt_index][1];
						point[2] = out_winding->pts[0][2] - out_winding->pts[pt_index][2];

						if ((point[0] * point[0]) + (point[1] * point[1]) + (point[2] * point[2]) < 1.0)
						{
							out_winding->ptCount = pt_index;
						}
					}

					if (out_winding->ptCount < 3)
					{
						out_winding->ptCount = 0;
					}
				}

				axial_planes_t get_axial_planes(const zonetool::h1::Bounds* bounds)
				{
					axial_planes_t axial_planes{};
					axial_planes.planes[0][0] = -1.f;
					axial_planes.planes[0][1] = 0.f;
					axial_planes.planes[0][2] = 0.f;
					axial_planes.planes[0][3] = -1 * (bounds->midPoint[0] - bounds->halfSize[0]);

					axial_planes.planes[1][0] = 1.f;
					axial_planes.planes[1][1] = 0.f;
					axial_planes.planes[1][2] = 0.f;
					axial_planes.planes[1][3] = bounds->midPoint[0] + bounds->halfSize[0];

					axial_planes.planes[2][0] = 0.f;
					axial_planes.planes[2][1] = -1.f;
					axial_planes.planes[2][2] = 0.f;
					axial_planes.planes[2][3] = -1 * (bounds->midPoint[1] - bounds->halfSize[1]);

					axial_planes.planes[3][0] = 0.f;
					axial_planes.planes[3][1] = 1.f;
					axial_planes.planes[3][2] = 0.f;
					axial_planes.planes[3][3] = (bounds->midPoint[1] + bounds->halfSize[1]);

					axial_planes.planes[4][0] = 0.f;
					axial_planes.planes[4][1] = 0.f;
					axial_planes.planes[4][2] = -1.f;
					axial_planes.planes[4][3] = -1 * (bounds->midPoint[2] - bounds->halfSize[2]);

					axial_planes.planes[5][0] = 0.f;
					axial_planes.planes[5][1] = 0.f;
					axial_planes.planes[5][2] = 1.f;
					axial_planes.planes[5][3] = (bounds->midPoint[2] + bounds->halfSize[2]);

					return axial_planes;
				}

				void build_windings_for_brush(zonetool::h1::ClipInfo* info, const zonetool::h1::Bounds* bounds,
					const zonetool::h1::cbrush_t* brush, polygon_t* out_polys,
					unsigned int max_polys, float(*out_verts)[3], unsigned int max_verts)
				{
					auto point_count = 0;
					const auto axial_planes = get_axial_planes(bounds);

					for (auto side_index = 0; side_index < brush->numsides + 6; side_index++)
					{
						const auto out_poly = &out_polys[side_index];
						out_poly->pts = &out_verts[point_count];
						get_winding_for_brush_face(info, bounds, brush, side_index, out_poly, max_verts - point_count, &axial_planes);
						point_count += out_poly->ptCount;
					}
				}

				int max_nodes = 0;
				mesh_node* alloc_node(mesh_node* nodes, int& node_index)
				{
					if (node_index + 1 >= max_nodes)
					{
						ZONETOOL_FATAL("out of nodes for physworld generation");
					}

					return &nodes[node_index++];
				}

				bounding_box compute_bounding_box(zonetool::h1::dmMeshData* mesh, const std::vector<triangle_t>& triangles)
				{
					bounding_box box{};

					const auto verts = &mesh->m_aVertices[0];

					if (triangles.size() > 0)
					{
						box.lower[0] = verts[triangles[0].verts[0]].x;
						box.lower[1] = verts[triangles[0].verts[0]].y;
						box.lower[2] = verts[triangles[0].verts[0]].z;
						box.upper[0] = verts[triangles[0].verts[0]].x;
						box.upper[1] = verts[triangles[0].verts[0]].y;
						box.upper[2] = verts[triangles[0].verts[0]].z;
					}

					for (const auto& tri : triangles)
					{
						for (auto i = 0; i < 3; i++)
						{
							for (auto o = 0; o < 3; o++)
							{
								const auto value = reinterpret_cast<float*>(&verts[tri.verts[i]].x)[o];
								if (value > box.upper[o])
								{
									box.upper[o] = value;
								}

								if (value < box.lower[o])
								{
									box.lower[o] = value;
								}
							}
						}
					}

					return box;
				}

				void cross_product(const float* a, const float* b, float* out)
				{
					out[0] = a[1] * b[2] - a[2] * b[1];
					out[1] = a[2] * b[0] - a[0] * b[2];
					out[2] = a[0] * b[1] - a[1] * b[0];
				}

				float dot_product(const float* a, const float* b)
				{
					return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
				}

				void vector_sub(const float* a, const float* b, float* out)
				{
					out[0] = a[0] - b[0];
					out[1] = a[1] - b[1];
					out[2] = a[2] - b[2];
				}

				void vector_add(const float* a, const float* b, float* out)
				{
					out[0] = a[0] + b[0];
					out[1] = a[1] + b[1];
					out[2] = a[2] + b[2];
				}

				void vector_mul(const float* a, const float mul, float* out)
				{
					out[0] = a[0] * mul;
					out[1] = a[1] * mul;
					out[2] = a[2] * mul;
				}

				int hash_triangle(const triangle_t& tri, const std::vector<zonetool::h1::dmFloat4>& phys_verticies)
				{
					float verts[3][3]{};
					std::memcpy(&verts[0], &phys_verticies[tri.verts[0]], sizeof(float[3]));
					std::memcpy(&verts[1], &phys_verticies[tri.verts[1]], sizeof(float[3]));
					std::memcpy(&verts[2], &phys_verticies[tri.verts[2]], sizeof(float[3]));

					std::string data;
					data.append(reinterpret_cast<char*>(&verts[0]), sizeof(float[3][3]));

					return utils::cryptography::jenkins_one_at_a_time::compute(data);
				}

				bool is_script_brushmodel_brush_r(zonetool::h1::clipMap_t* asset, zonetool::h1::cLeafBrushNode_s* node, zonetool::h1::cbrush_t* brush)
				{
					if (node->leafBrushCount > 0)
					{
						for (auto o = 0; o < node->leafBrushCount; o++)
						{
							const auto brush_idx = node->data.leaf.brushes[o];
							if (brush == &asset->info.bCollisionData.brushes[brush_idx])
							{
								return true;
							}
						}

						return false;
					}

					if (node->leafBrushCount)
					{
						if (is_script_brushmodel_brush_r(asset, &node[1], brush))
						{
							return true;
						}
					}

					if (node->data.children.childOffset[0] &&
						is_script_brushmodel_brush_r(asset, &node[node->data.children.childOffset[0]], brush))
					{
						return true;
					}

					if (node->data.children.childOffset[1] &&
						is_script_brushmodel_brush_r(asset, &node[node->data.children.childOffset[1]], brush))
					{
						return true;
					}

					return false;
				}

				bool is_script_brushmodel_brush(zonetool::h1::clipMap_t* asset, zonetool::h1::cbrush_t* brush)
				{
					for (auto i = 0u; i < asset->numSubModels; i++)
					{
						const auto cmodel = &asset->cmodels[i];
						const auto leaf_brush_node = &asset->info.bCollisionTree.leafbrushNodes[cmodel->leaf.leafBrushNode];
						if (is_script_brushmodel_brush_r(asset, leaf_brush_node, brush))
						{
							return true;
						}
					}

					return false;
				}

				bool find_partition_in_tree(zonetool::h1::clipMap_t* asset, zonetool::h1::CollisionAabbTree* tree, zonetool::h1::CollisionPartition* partition)
				{
					if (tree->childCount != 0)
					{
						for (auto i = 0u; i < tree->childCount; i++)
						{
							const auto child = &asset->info.pCollisionTree.aabbTrees[tree->u.firstChildIndex + i];
							if (find_partition_in_tree(asset, child, partition))
							{
								return true;
							}
						}
					}
					else
					{
						return partition == &asset->info.pCollisionData.partitions[tree->u.partitionIndex];
					}

					return false;
				}

				bool is_script_brushmodel_partition(zonetool::h1::clipMap_t* asset, zonetool::h1::CollisionPartition* partition)
				{
					for (auto i = 0u; i < asset->numSubModels; i++)
					{
						const auto cmodel = &asset->cmodels[i];
						for (auto o = 0u; o < cmodel->leaf.collAabbCount; o++)
						{
							const auto tree = &asset->info.pCollisionTree.aabbTrees[cmodel->leaf.firstCollAabbIndex + o];
							if (find_partition_in_tree(asset, tree, partition))
							{
								return true;
							}
						}
					}

					return false;
				}

				std::vector<triangle_t> generate_brush_triangles(zonetool::h1::clipMap_t* asset, std::vector<zonetool::h1::dmFloat4>& vertices, int brush_index)
				{
					std::vector<triangle_t> triangles;

					auto brush = &asset->info.bCollisionData.brushes[brush_index];
					auto brush_bounds = asset->info.bCollisionData.brushBounds[brush_index];

					if (is_script_brushmodel_brush(asset, brush))
					{
						return {};
					}

					const auto add_points = [&](int side_index, polygon_t* poly)
					{
						const auto base_index = static_cast<int>(vertices.size());

						for (auto i = 0u; i < poly->ptCount; i++)
						{
							zonetool::h1::dmFloat4 vert{};
							vert.x = poly->pts[i][0];
							vert.y = poly->pts[i][1];
							vert.z = poly->pts[i][2];
							vertices.emplace_back(vert);
						}

						if (poly->ptCount == 3)
						{
							triangle_t triangle{};
							triangle.verts[0] = base_index + 0;
							triangle.verts[1] = base_index + 1;
							triangle.verts[2] = base_index + 2;
							triangles.emplace_back(triangle);
						}
						else if (poly->ptCount == 4)
						{
							triangle_t triangle_1{};
							triangle_1.verts[0] = base_index + 0;
							triangle_1.verts[1] = base_index + 1;
							triangle_1.verts[2] = base_index + 2;
							triangles.emplace_back(triangle_1);

							triangle_t triangle_2{};
							triangle_2.verts[0] = base_index + 2;
							triangle_2.verts[1] = base_index + 3;
							triangle_2.verts[2] = base_index + 0;
							triangles.emplace_back(triangle_2);
						}
						else
						{
							using point = std::array<float, 2>;
							std::vector<point> points;
							std::vector<std::vector<point>> polygon;

							for (auto i = 0u; i < poly->ptCount; i++)
							{
								point p{};
								p[0] = poly->pts[i][0];
								p[1] = poly->pts[i][1];

								points.push_back(p);
							}

							polygon.push_back(points);

							const auto indices = mapbox::earcut<int>(polygon);
							for (auto i = 0u; i < indices.size(); i += 3)
							{
								triangle_t triangle{};
								triangle.verts[0] = base_index + indices[i + 2];
								triangle.verts[1] = base_index + indices[i + 1];
								triangle.verts[2] = base_index + indices[i + 0];
								triangles.emplace_back(triangle);
							}
						}
					};

					constexpr auto max_verts = 0x1000;
					constexpr auto max_polys = 0x1000;
					float verts[max_verts][3]{};
					polygon_t polys[max_verts]{};

					build_windings_for_brush(asset->pInfo, &brush_bounds, brush, polys, max_polys, verts, max_verts);
					for (auto side_index = 0; side_index < brush->numsides + 6; side_index++)
					{
						const auto poly = &polys[side_index];
						add_points(side_index, poly);
					}

					return triangles;
				}

				std::vector<triangle_t> generate_triangles(zonetool::h1::clipMap_t* asset, std::vector<zonetool::h1::dmFloat4>& vertices)
				{
					std::vector<triangle_t> triangles;
					std::unordered_set<int> triangle_hashes;

					auto triangle_index = 0;

					const auto add_triangle = [&](triangle_t& triangle)
					{
						const auto hash = hash_triangle(triangle, vertices);
						if (triangle_hashes.contains(hash))
						{
							return;
						}

						triangle_hashes.insert(hash);
						triangle.index = triangle_index++;
						triangles.emplace_back(triangle);
					};

					for (auto i = 0u; i < asset->info.bCollisionData.numBrushes; i++)
					{
						const auto brush_tris = generate_brush_triangles(asset, vertices, i);
						for (auto tri : brush_tris)
						{
							add_triangle(tri);
						}
					}

					for (auto i = 0; i < asset->info.pCollisionData.partitionCount; i++)
					{
						const auto partition = &asset->info.pCollisionData.partitions[i];
						if (is_script_brushmodel_partition(asset, partition))
						{
							continue;
						}

						auto tri_indices = &asset->info.pCollisionData.triIndices[3 * partition->firstTri];
						for (auto o = 0u; o < partition->triCount; o++)
						{
							triangle_t triangle{};
							triangle.verts[0] = tri_indices[0] + 1024 * partition->firstVertSegment;
							triangle.verts[1] = tri_indices[1] + 1024 * partition->firstVertSegment;
							triangle.verts[2] = tri_indices[2] + 1024 * partition->firstVertSegment;
							add_triangle(triangle);

							tri_indices += 3;
						}
					}

					return triangles;
				}

				// https://github.com/autonomousvision/occupancy_flow/blob/master/im2mesh/utils/libvoxelize/tribox2.h


#define FINDMINMAX(x0, x1, x2, min, max) \
		min = max = x0; \
		if (x1 < min) min = x1; \
		if (x1 > max) max = x1; \
		if (x2 < min) min = x2; \
		if (x2 > max) max = x2; \

#define AXISTEST_X01(a, b, fa, fb) \
		p0 = a * v0[1] - b * v0[2]; \
		p2 = a * v2[1] - b * v2[2]; \
		if (p0 < p2) \
		{ \
			min = p0; \
			max = p2; \
		} \
		else \
		{ \
			min = p2; \
			max = p0; \
		} \
		rad = fa * boxhalfsize[1] + fb * boxhalfsize[2]; \
		if (min > rad || max < -rad) \
		{ \
			return false; \
		} \

#define AXISTEST_X2(a, b, fa, fb) \
		p0 = a * v0[1] - b * v0[2]; \
		p1 = a * v1[1] - b * v1[2]; \
		if (p0 < p1) \
		{ \
			min = p0;\
			max = p1; \
		} \
		else \
		{ \
			min = p1; \
			max = p0; \
		} \
		rad = fa * boxhalfsize[1] + fb * boxhalfsize[2]; \
		if (min > rad || max < -rad) \
		{ \
			return false; \
		} \

#define AXISTEST_Y02(a, b, fa, fb) \
		p0 = -a * v0[0] + b * v0[2]; \
		p2 = -a * v2[0] + b * v2[2]; \
		if (p0 < p2) \
		{ \
			min = p0; \
			max = p2; \
		}\
		else \
		{ \
			min = p2; \
			max = p0; \
		} \
		rad = fa * boxhalfsize[0] + fb * boxhalfsize[2]; \
		if (min > rad || max < -rad) \
		{ \
			return false; \
		} \

#define AXISTEST_Y1(a, b, fa, fb) \
		p0 = -a * v0[0] + b * v0[2]; \
		p1 = -a * v1[0] + b * v1[2]; \
		if (p0 < p1) \
		{ \
			min = p0; \
			max = p1; \
		} \
		else \
		{ \
			min = p1;  \
			max = p0; \
		} \
		rad = fa * boxhalfsize[0] + fb * boxhalfsize[2]; \
		if (min > rad || max < -rad) \
		{ \
			return false; \
		} \

#define AXISTEST_Z12(a, b, fa, fb) \
		p1 = a * v1[0] - b * v1[1]; \
		p2 = a * v2[0] - b * v2[1]; \
		if (p2 < p1) \
		{ \
			min = p2; \
			max = p1; \
		} \
		else \
		{ \
			min = p1; \
			max = p2; \
		} \
		rad = fa * boxhalfsize[0] + fb * boxhalfsize[1]; \
		if (min > rad || max < -rad) \
		{ \
			return false; \
		} \

#define AXISTEST_Z0(a, b, fa, fb) \
		p0 = a * v0[0] - b * v0[1]; \
		p1 = a * v1[0] - b * v1[1]; \
		if (p0 < p1) \
		{ \
			min = p0; \
			max = p1; \
		} \
		else \
		{ \
			min = p1; \
			max = p0; \
		} \
		rad = fa * boxhalfsize[0] + fb * boxhalfsize[1]; \
		if (min > rad || max < -rad) \
		{ \
			return false; \
		} \

				bool plane_box_overlap(float normal[3], float vert[3], float maxbox[3])
				{
					float vmin[3]{};
					float vmax[3]{};

					for (auto q = 0; q < 3; q++)
					{
						const auto v = vert[q];
						if (normal[q] > 0.0f)
						{
							vmin[q] = -maxbox[q] - v;
							vmax[q] = maxbox[q] - v;

						}
						else
						{
							vmin[q] = maxbox[q] - v;
							vmax[q] = -maxbox[q] - v;
						}
					}

					if (dot_product(normal, vmin) > 0.0f)
					{
						return false;
					}

					if (dot_product(normal, vmax) >= 0.0f)
					{
						return true;
					}

					return false;
				}

				bool tri_box_overlap(float boxcenter[3], float boxhalfsize[3], float triverts[3][3])
				{
					float v0[3]{};
					float v1[3]{};
					float v2[3]{};

					float min{}, max{};
					float p0{}, p1{}, p2{};
					float rad{};
					float fex{}, fey{}, fez{};

					float normal[3]{}, e0[3]{}, e1[3]{}, e2[3]{};

					vector_sub(triverts[0], boxcenter, v0);
					vector_sub(triverts[1], boxcenter, v1);
					vector_sub(triverts[2], boxcenter, v2);

					vector_sub(v1, v0, e0);
					vector_sub(v2, v1, e1);
					vector_sub(v0, v2, e2);

					fex = std::fabsf(e0[0]);
					fey = std::fabsf(e0[1]);
					fez = std::fabsf(e0[2]);

					AXISTEST_X01(e0[2], e0[1], fez, fey);
					AXISTEST_Y02(e0[2], e0[0], fez, fex);
					AXISTEST_Z12(e0[1], e0[0], fey, fex);

					fex = std::fabsf(e1[0]);
					fey = std::fabsf(e1[1]);
					fez = std::fabsf(e1[2]);

					AXISTEST_X01(e1[2], e1[1], fez, fey);
					AXISTEST_Y02(e1[2], e1[0], fez, fex);
					AXISTEST_Z0(e1[1], e1[0], fey, fex);

					fex = std::fabsf(e2[0]);
					fey = std::fabsf(e2[1]);
					fez = std::fabsf(e2[2]);

					AXISTEST_X2(e2[2], e2[1], fez, fey);
					AXISTEST_Y1(e2[2], e2[0], fez, fex);
					AXISTEST_Z12(e2[1], e2[0], fey, fex);

					FINDMINMAX(v0[0], v1[0], v2[0], min, max);

					if (min >= boxhalfsize[0] || max <= -boxhalfsize[0])
					{
						return false;
					}

					FINDMINMAX(v0[1], v1[1], v2[1], min, max);

					if (min >= boxhalfsize[1] || max <= -boxhalfsize[1])
					{
						return false;
					}

					FINDMINMAX(v0[2], v1[2], v2[2], min, max);

					if (min >= boxhalfsize[2] || max <= -boxhalfsize[2])
					{
						return false;
					}

					cross_product(e0, e1, normal);

					if (!plane_box_overlap(normal, v0, boxhalfsize))
					{
						return false;
					}

					return true;
				}

				zonetool::h1::Bounds corners_to_bounds(bounding_box box)
				{
					zonetool::h1::Bounds res{};

					res.midPoint[0] = (box.upper[0] + box.lower[0]) * 0.5f;
					res.midPoint[1] = (box.upper[1] + box.lower[1]) * 0.5f;
					res.midPoint[2] = (box.upper[2] + box.lower[2]) * 0.5f;
					res.halfSize[0] = (box.upper[0] - box.lower[0]) * 0.5f;
					res.halfSize[1] = (box.upper[1] - box.lower[1]) * 0.5f;
					res.halfSize[2] = (box.upper[2] - box.lower[2]) * 0.5f;

					return res;
				}

				bool tri_intersects_box(float tri[3][3], bounding_box box)
				{
					auto bounds = corners_to_bounds(box);
					return tri_box_overlap(bounds.midPoint, bounds.halfSize, tri);
				}

				bool triangle_box_overlay(zonetool::h1::dmMeshData* mesh, const triangle_t& triangle, const bounding_box& box)
				{
					const auto verts = &mesh->m_aVertices[0];

					zonetool::h1::vec3_t p1{};
					zonetool::h1::vec3_t p3{};
					zonetool::h1::vec3_t p2{};
					zonetool::h1::vec3_t points[3]{};

					p1[0] = verts[triangle.verts[0]].x;
					p1[1] = verts[triangle.verts[0]].y;
					p1[2] = verts[triangle.verts[0]].z;

					p2[0] = verts[triangle.verts[1]].x;
					p2[1] = verts[triangle.verts[1]].y;
					p2[2] = verts[triangle.verts[1]].z;

					p3[0] = verts[triangle.verts[2]].x;
					p3[1] = verts[triangle.verts[2]].y;
					p3[2] = verts[triangle.verts[2]].z;

					points[0][0] = p1[0];
					points[0][1] = p1[1];
					points[0][2] = p1[2];

					points[1][0] = p2[0];
					points[1][1] = p2[1];
					points[1][2] = p2[2];

					points[2][0] = p3[0];
					points[2][1] = p3[1];
					points[2][2] = p3[2];

					return tri_intersects_box(points, box);
				}

				std::vector<triangle_t> get_triangles_in_box(zonetool::h1::dmMeshData* mesh, const std::vector<triangle_t>& triangles,
					const bounding_box& root)
				{
					std::vector<triangle_t> res;

					for (const auto& tri : triangles)
					{
						if (triangle_box_overlay(mesh, tri, root))
						{
							res.emplace_back(tri);
						}
					}

					return res;
				}

				unsigned int write_triangles(std::vector<triangle_t>& dest_triangles, const std::vector<triangle_t>& triangles)
				{
					auto index = dest_triangles.size();

					for (const auto& tri : triangles)
					{
						triangle_t dest_tri{};
						std::memcpy(&dest_tri, &tri, sizeof(triangle_t));
						dest_tri.index = static_cast<int>(dest_triangles.size());
						dest_triangles.emplace_back(dest_tri);
					}

					return static_cast<unsigned int>(index);
				}

				int find_best_axis(zonetool::h1::dmMeshData* mesh, const std::vector<triangle_t>& triangles,
					const bounding_box& box, float* value)
				{
					auto length = 0.f;
					auto axis = 0;

					for (auto i = 0; i < 3; i++)
					{
						const auto l = std::abs(box.upper[i] - box.lower[i]);
						if (l > length)
						{
							axis = i;
							length = l;
						}
					}

					*value = box.lower[axis] + length / 2;
					return axis;
				}

				constexpr auto max_tris_per_leaf = 15;
				constexpr auto max_tree_depth = 128;

				float calculate_surface_area(const bounding_box& box)
				{
					const auto l = box.upper[0] - box.lower[0];
					const auto w = box.upper[1] - box.lower[1];
					const auto h = box.upper[2] - box.lower[2];

					const auto a1 = l * w;
					const auto a2 = l * h;
					const auto a3 = w * h;

					return 2 * (a1 + a2 + a3);
				}

				void compute_aabb_tree(zonetool::h1::dmMeshData* mesh, std::vector<triangle_t>& dest_triangles,
					const std::vector<triangle_t>& parent_triangles,
					const bounding_box& root, mesh_node* nodes, int& node_index, int& tree_depth)
				{
					const auto surface_area = calculate_surface_area(root);
					const auto triangles = get_triangles_in_box(mesh, parent_triangles, root);

					if (triangles.size() > max_tris_per_leaf && tree_depth < max_tree_depth && surface_area > 1.f)
					{
						float value{};
						const auto axis = find_best_axis(mesh, triangles, root, &value);

						bounding_box left{};
						bounding_box right{};

						std::memcpy(&left, &root, sizeof(bounding_box));
						std::memcpy(&right, &root, sizeof(bounding_box));

						left.upper[axis] = value;
						right.lower[axis] = value;

						const auto left_tris = get_triangles_in_box(mesh, triangles, left);
						const auto right_tris = get_triangles_in_box(mesh, triangles, right);

						if (left_tris.size() == 0 || right_tris.size() == 0)
						{
							if (left_tris.size() > 0)
							{
								return compute_aabb_tree(mesh, dest_triangles, left_tris, left, nodes, node_index, tree_depth);
							}
							else if (right_tris.size() > 0)
							{
								return compute_aabb_tree(mesh, dest_triangles, right_tris, right, nodes, node_index, tree_depth);
							}
						}

						const auto initial_index = node_index;
						const auto parent_node = alloc_node(nodes, node_index);
						std::memcpy(parent_node->lower, root.lower, sizeof(float[3]));
						std::memcpy(parent_node->upper, root.upper, sizeof(float[3]));
						tree_depth++;

						compute_aabb_tree(mesh, dest_triangles, triangles, left, nodes, node_index, tree_depth);
						parent_node->node.anon.fields.index = node_index - initial_index;
						compute_aabb_tree(mesh, dest_triangles, triangles, right, nodes, node_index, tree_depth);

						tree_depth--;
					}
					else if (triangles.size() > 0)
					{
						const auto leaf = alloc_node(nodes, node_index);
						const auto box = compute_bounding_box(mesh, triangles);
						std::memcpy(leaf->lower, box.lower, sizeof(float[3]));
						std::memcpy(leaf->upper, box.upper, sizeof(float[3]));
						leaf->node.anon.fields.triangleCount = static_cast<unsigned int>(std::min((int)triangles.size(), max_tris_per_leaf));
						leaf->node.anon.fields.index = write_triangles(dest_triangles, triangles);
					}
					else
					{
						const auto leaf = alloc_node(nodes, node_index);
						leaf->node.anon.fields.triangleCount = 1;
						leaf->node.anon.fields.index = 0;
					}
				}
			}

			zonetool::h1::PhysWorld* generate(zonetool::h1::clipMap_t* asset, utils::memory::allocator& allocator)
			{
				ZONETOOL_INFO("generating physworld...");

				const auto new_asset = allocator.allocate<zonetool::h1::PhysWorld>();

				new_asset->name = allocator.duplicate_string(asset->name);
				new_asset->brushModelCount = asset->numSubModels;
				new_asset->brushModels = allocator.allocate_array<zonetool::h1::PhysBrushModel>(new_asset->brushModelCount);

				for (auto i = 0u; i < new_asset->brushModelCount; i++)
				{
					const auto model = &new_asset->brushModels[i];
					model->fields.polytopeIndex = -1;
					model->fields.unk = -1;
					model->fields.worldIndex = 0;
					model->fields.meshIndex = i == 0 ? 0 : -1;
				}

				new_asset->meshDatas = allocator.allocate_array<zonetool::h1::dmMeshData>(1);
				new_asset->meshDataCount = 1;

				const auto mesh = &new_asset->meshDatas[0];

				std::vector<zonetool::h1::dmFloat4> vertices;

				for (auto i = 0u; i < asset->info.pCollisionData.vertCount; i++)
				{
					zonetool::h1::dmFloat4 vertex{};
					vertex.x = asset->info.pCollisionData.verts[i][0];
					vertex.y = asset->info.pCollisionData.verts[i][1];
					vertex.z = asset->info.pCollisionData.verts[i][2];
					vertex.w = 0.f;
					vertices.emplace_back(vertex);
				}

				auto triangles = generate_triangles(asset, vertices);

				ZONETOOL_INFO("total tris: %lli, verts: %lli", triangles.size(), vertices.size());

				mesh->m_vertexCount = static_cast<int>(vertices.size());
				mesh->m_aVertices = allocator.allocate_array<zonetool::h1::dmFloat4>(mesh->m_vertexCount);
				std::memcpy(mesh->m_aVertices, vertices.data(), vertices.size() * sizeof(zonetool::h1::dmFloat4));

				max_nodes = static_cast<int>(triangles.size()) * max_tree_depth;
				const auto nodes = allocator.allocate_array<mesh_node>(max_nodes);

				auto node_index = 0;

				std::vector<triangle_t> dest_triangles;

				for (auto& tri : triangles)
				{
					dest_triangles.emplace_back(tri);
				}

				const auto root = compute_bounding_box(mesh, triangles);
				int tree_depth = 0;
				compute_aabb_tree(mesh, dest_triangles, triangles, root, nodes, node_index, tree_depth);

				mesh->m_triangleCount = static_cast<int>(dest_triangles.size());
				mesh->m_aTriangles = allocator.allocate_array<zonetool::h1::dmMeshTriangle>(mesh->m_triangleCount);

				for (const auto& tri : dest_triangles)
				{
					mesh->m_aTriangles[tri.index].i1 = tri.verts[2];
					mesh->m_aTriangles[tri.index].i2 = tri.verts[1];
					mesh->m_aTriangles[tri.index].i3 = tri.verts[0];
					mesh->m_aTriangles[tri.index].w1 = -1;
					mesh->m_aTriangles[tri.index].w2 = -1;
					mesh->m_aTriangles[tri.index].w3 = -1;
					mesh->m_aTriangles[tri.index].materialIndex = 0;
					mesh->m_aTriangles[tri.index].collisionFlags = 1;
				}

				mesh->m_nodeCount = node_index;
				mesh->m_pRoot = allocator.allocate_array<zonetool::h1::dmMeshNode>(mesh->m_nodeCount);

				float maxs[3]{};
				float mins[3]{};
				float unquantize[3]{};

				for (auto i = 0; i < node_index; i++)
				{
					const auto node = &nodes[i];

					for (auto o = 0; o < 3; o++)
					{
						if (node->lower[o] > maxs[o])
						{
							maxs[o] = node->lower[o];
						}

						if (node->lower[o] < mins[o])
						{
							mins[o] = node->lower[o];
						}

						if (node->upper[o] > maxs[o])
						{
							maxs[o] = node->upper[o];
						}

						if (node->upper[o] < mins[o])
						{
							mins[o] = node->upper[o];
						}
					}
				}

				for (auto i = 0; i < 3; i++)
				{
					unquantize[i] = std::max(
						maxs[i] / static_cast<float>(std::numeric_limits<std::int16_t>().max()),
						mins[i] / static_cast<float>(std::numeric_limits<std::int16_t>().min())
					);
				}

				ZONETOOL_INFO("mins: %f %f %f", mins[0], mins[1], mins[2]);
				ZONETOOL_INFO("maxs: %f %f %f", maxs[0], maxs[1], maxs[2]);
				ZONETOOL_INFO("unquantize: %f %f %f", unquantize[0], unquantize[1], unquantize[2]);

				mesh->m_unquantize.x = unquantize[0];
				mesh->m_unquantize.y = unquantize[1];
				mesh->m_unquantize.z = unquantize[2];

				mesh->m_center.x = 0.f;
				mesh->m_center.y = 0.f;
				mesh->m_center.z = 0.f;

				mesh->m_extents.x = std::max(maxs[0], std::fabs(mins[0]));
				mesh->m_extents.y = std::max(maxs[1], std::fabs(mins[1]));
				mesh->m_extents.z = std::max(maxs[2], std::fabs(mins[2]));

				mesh->m_height = 127;
				mesh->contents = 1;

				for (auto i = 0; i < node_index; i++)
				{
					const auto node = &nodes[i];

					mesh->m_pRoot[i].lowerX = static_cast<zonetool::h1::dm_int16>(node->lower[0] / mesh->m_unquantize.x);
					mesh->m_pRoot[i].lowerY = static_cast<zonetool::h1::dm_int16>(node->lower[1] / mesh->m_unquantize.y);
					mesh->m_pRoot[i].lowerZ = static_cast<zonetool::h1::dm_int16>(node->lower[2] / mesh->m_unquantize.z);
					mesh->m_pRoot[i].upperX = static_cast<zonetool::h1::dm_int16>(node->upper[0] / mesh->m_unquantize.x);
					mesh->m_pRoot[i].upperY = static_cast<zonetool::h1::dm_int16>(node->upper[1] / mesh->m_unquantize.y);
					mesh->m_pRoot[i].upperZ = static_cast<zonetool::h1::dm_int16>(node->upper[2] / mesh->m_unquantize.z);

					mesh->m_pRoot[i].anon.fields.axis = node->node.anon.fields.axis;
					mesh->m_pRoot[i].anon.fields.triangleCount = node->node.anon.fields.triangleCount;
					mesh->m_pRoot[i].anon.fields.index = node->node.anon.fields.index;
				}

				allocator.free(nodes);

				ZONETOOL_INFO("generation complete, total nodes: %i", node_index);

				return new_asset;
			}
		}
	}
}
