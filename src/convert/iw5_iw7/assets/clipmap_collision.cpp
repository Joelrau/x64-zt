#include <std_include.hpp>
#include "../include.hpp"

#include "clipmap_collision.hpp"
#include "xmodel.hpp"

#include <iw7/common/havok_builder.hpp>

#include <array>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cfloat>
#include <cstdio>
#include <fstream>
#include <map>
#include <string>

#define CLIPMAP_COLLISION_INFO(__FMT__,...) \
	do { if (verbose_logging_enabled()) { ZONETOOL_INFO(__FMT__, __VA_ARGS__); } } while (false)

namespace zonetool::iw5
{
	namespace converter::iw7
	{
		namespace collision
		{
			namespace
			{
				bool verbose_logging_enabled()
				{
					const auto* env = std::getenv("ZT_HAVOK_VERBOSE");
					return env && *env && *env != '0';
				}

				constexpr auto CONTENTS_SOLID = 0x00000001;

				constexpr auto CONTENTS_NONCOLLIDING = 0x00000004;

				constexpr auto CONTENTS_SOLID_AS_CLIP_DEFAULT = 0x00031640;

				bool solid_as_clip_enabled()
				{
					const auto* env = std::getenv("ZT_HAVOK_SOLID_AS_CLIP");
					return env && env[0] == '1';
				}

				unsigned int solid_as_clip_contents()
				{
					const auto* env = std::getenv("ZT_HAVOK_SOLID_CONTENTS");
					if (!env || !env[0])
					{
						return CONTENTS_SOLID_AS_CLIP_DEFAULT;
					}

					char* end = nullptr;
					const auto value = std::strtoul(env, &end, 0);
					if (end == env || !value)
					{
						ZONETOOL_WARNING("clipmap collision: ZT_HAVOK_SOLID_CONTENTS=\"%s\" is not "
							"a usable mask, keeping 0x%08X", env,
							CONTENTS_SOLID_AS_CLIP_DEFAULT);
						return CONTENTS_SOLID_AS_CLIP_DEFAULT;
					}

					return static_cast<unsigned int>(value);
				}

				constexpr auto CONTENTS_COMPILE_ONLY = 0x08000000 /* DETAIL */
					| 0x10000000 /* STRUCTURAL */ | 0x20000000 /* TRANSPARENT */;

				constexpr auto USERDATA_GLASS_PIECE_SHIFT = 32;
				constexpr auto USERDATA_GLASS_PIECE_MAX = 0xFFu;

				constexpr auto SURF_TYPE_MASK = 0x01F80000u;

				bool is_glass_surface(const std::uint64_t iw7_surface_flags)
				{
					const auto type = static_cast<unsigned int>(iw7_surface_flags) & SURF_TYPE_MASK;
					return type == 0x00480000u  /* SURFACE_FLAG_GLASS_PANE */
						|| type == 0x01380000u; /* SURFACE_FLAG_GLASS_SOLID */
				}

				constexpr auto CONTENTS_TRIGGER = 0x40000000;

				constexpr auto PLANE_EPSILON = 0.01f;
				constexpr auto HULL_EXTENT = 131072.0f;

				struct plane
				{
					float normal[3];
					float dist;
				};

				struct winding
				{
					std::vector<std::array<float, 3>> points;
				};

				winding base_winding_for_plane(const plane& p)
				{
					auto axis = 0;
					auto best = std::fabs(p.normal[0]);
					for (auto i = 1; i < 3; i++)
					{
						if (std::fabs(p.normal[i]) < best)
						{
							best = std::fabs(p.normal[i]);
							axis = i;
						}
					}

					float up[3] = {0.0f, 0.0f, 0.0f};
					up[axis] = 1.0f;

					const auto dot = up[0] * p.normal[0] + up[1] * p.normal[1] + up[2] * p.normal[2];
					for (auto i = 0; i < 3; i++)
					{
						up[i] -= dot * p.normal[i];
					}

					auto len = std::sqrt(up[0] * up[0] + up[1] * up[1] + up[2] * up[2]);
					if (len < 1e-6f)
					{
						return {};
					}
					for (auto i = 0; i < 3; i++)
					{
						up[i] /= len;
					}

					float right[3] = {
						up[1] * p.normal[2] - up[2] * p.normal[1],
						up[2] * p.normal[0] - up[0] * p.normal[2],
						up[0] * p.normal[1] - up[1] * p.normal[0],
					};

					float org[3];
					for (auto i = 0; i < 3; i++)
					{
						org[i] = p.normal[i] * p.dist;
					}

					winding w;
					w.points.resize(4);
					for (auto c = 0; c < 4; c++)
					{
						const auto su = (c == 0 || c == 3) ? -HULL_EXTENT : HULL_EXTENT;
						const auto sr = (c < 2) ? -HULL_EXTENT : HULL_EXTENT;
						for (auto i = 0; i < 3; i++)
						{
							w.points[c][i] = org[i] + up[i] * su + right[i] * sr;
						}
					}

					return w;
				}

				void clip_winding(winding& w, const plane& p)
				{
					if (w.points.empty())
					{
						return;
					}

					const auto count = w.points.size();
					std::vector<float> dists(count);
					for (auto i = 0u; i < count; i++)
					{
						dists[i] = w.points[i][0] * p.normal[0] + w.points[i][1] * p.normal[1]
							+ w.points[i][2] * p.normal[2] - p.dist;
					}

					winding out;
					for (auto i = 0u; i < count; i++)
					{
						const auto j = (i + 1) % count;
						const auto di = dists[i];
						const auto dj = dists[j];

						if (di <= PLANE_EPSILON)
						{
							out.points.push_back(w.points[i]);
						}

						if ((di > PLANE_EPSILON && dj < -PLANE_EPSILON) ||
							(di < -PLANE_EPSILON && dj > PLANE_EPSILON))
						{
							const auto t = di / (di - dj);
							std::array<float, 3> mid{};
							for (auto c = 0; c < 3; c++)
							{
								mid[c] = w.points[i][c] + t * (w.points[j][c] - w.points[i][c]);
							}
							out.points.push_back(mid);
						}
					}

					w = std::move(out);
				}

				bool quads_enabled()
				{
					const auto* env = std::getenv("ZT_HAVOK_QUADS");
					return env && env[0] == '1';
				}

				void face_to_triangles(const winding& w, const plane& p,
					const unsigned short tag, const int contents, const unsigned int crc,
					const std::uint64_t user_data, std::vector<havok_triangle>& out)
				{
					if (w.points.size() < 3)
					{
						return;
					}

					for (auto i = 1u; i + 1 < w.points.size(); i++)
					{
						havok_triangle tri{};
						tri.surface_tag = tag;
						tri.contents = contents;
						tri.material_crc = crc;
						tri.user_data = user_data;

						std::memcpy(tri.verts[0], w.points[0].data(), sizeof(float[3]));
						std::memcpy(tri.verts[1], w.points[i].data(), sizeof(float[3]));
						std::memcpy(tri.verts[2], w.points[i + 1].data(), sizeof(float[3]));

						if (quads_enabled() && i + 2 < w.points.size())
						{
							tri.is_quad = true;
							std::memcpy(tri.vert3, w.points[i + 2].data(), sizeof(float[3]));
							i++;
						}

						float e1[3], e2[3], n[3];
						for (auto c = 0; c < 3; c++)
						{
							e1[c] = tri.verts[1][c] - tri.verts[0][c];
							e2[c] = tri.verts[2][c] - tri.verts[0][c];
						}
						n[0] = e1[1] * e2[2] - e1[2] * e2[1];
						n[1] = e1[2] * e2[0] - e1[0] * e2[2];
						n[2] = e1[0] * e2[1] - e1[1] * e2[0];

						if (n[0] * p.normal[0] + n[1] * p.normal[1] + n[2] * p.normal[2] < 0.0f)
						{
							if (tri.is_quad)
							{
								float spare[3];
								std::memcpy(spare, tri.verts[1], sizeof(spare));
								std::memcpy(tri.verts[1], tri.vert3, sizeof(spare));
								std::memcpy(tri.vert3, spare, sizeof(spare));
							}
							else
							{
								std::swap(tri.verts[1][0], tri.verts[2][0]);
								std::swap(tri.verts[1][1], tri.verts[2][1]);
								std::swap(tri.verts[1][2], tri.verts[2][2]);
							}
						}

						out.emplace_back(tri);
					}
				}

				void walk_leafbrush_nodes(const clipMap_t* clipmap, const int root,
					std::vector<bool>& out)
				{
					const auto node_count = clipmap->info.leafbrushNodesCount;
					if (!clipmap->info.leafbrushNodes || node_count == 0)
					{
						return;
					}

					std::vector<bool> visited(node_count, false);
					std::vector<int> stack;
					stack.emplace_back(root);

					while (!stack.empty())
					{
						const auto index = stack.back();
						stack.pop_back();

						if (index < 0 || static_cast<unsigned int>(index) >= node_count
							|| visited[index])
						{
							continue;
						}
						visited[index] = true;

						const auto* node = &clipmap->info.leafbrushNodes[index];

						if (node->leafBrushCount > 0)
						{
							if (!node->data.leaf.brushes)
							{
								continue;
							}

							for (auto i = 0; i < node->leafBrushCount; i++)
							{
								const auto brush = node->data.leaf.brushes[i];
								if (brush < clipmap->info.numBrushes)
								{
									out[brush] = true;
								}
							}
							continue;
						}

						for (auto c = 0; c < 2; c++)
						{
							stack.emplace_back(index + node->data.children.childOffset[c]);
						}
					}
				}

				std::vector<int> leafbrushes_of(const clipMap_t* clipmap, const int root)
				{
					std::vector<bool> marked(clipmap->info.numBrushes, false);
					walk_leafbrush_nodes(clipmap, root, marked);

					std::vector<int> out;
					for (auto b = 0; b < clipmap->info.numBrushes; b++)
					{
						if (marked[b])
						{
							out.emplace_back(b);
						}
					}
					return out;
				}

				bool brush_model_attribution(const clipMap_t* clipmap,
					std::vector<bool>& world_brushes, std::vector<bool>& model_brushes,
					bool log)
				{
					world_brushes.assign(clipmap->info.numBrushes, false);
					model_brushes.assign(clipmap->info.numBrushes, false);

					if (!clipmap->cmodels || clipmap->numSubModels == 0
						|| !clipmap->info.brushBounds)
					{
						return false;
					}

					walk_leafbrush_nodes(clipmap, clipmap->cmodels[0].leaf.leafBrushNode,
						world_brushes);

					if (log)
					{
						for (auto i = 0u; i < clipmap->numSubModels; i++)
						{
							const auto& cb = clipmap->cmodels[i].bounds;
							const auto& lb = clipmap->cmodels[i].leaf.bounds;
							std::vector<bool> hit(clipmap->info.numBrushes, false);
							walk_leafbrush_nodes(clipmap, clipmap->cmodels[i].leaf.leafBrushNode,
								hit);
							std::string list;
							for (auto b = 0; b < clipmap->info.numBrushes; b++)
							{
								if (hit[b])
								{
									list += " " + std::to_string(b);
								}
							}
							CLIPMAP_COLLISION_INFO("clipmap collision: cmodel %u node %d bounds "
								"%.0f x %.0f x %.0f at (%.0f %.0f %.0f) leafBounds "
								"%.0f x %.0f x %.0f at (%.0f %.0f %.0f) radius %.0f "
								"aabb[%u..+%u] brushContents 0x%08X reaches:%s",
								i, clipmap->cmodels[i].leaf.leafBrushNode,
								cb.halfSize[0] * 2.0f, cb.halfSize[1] * 2.0f,
								cb.halfSize[2] * 2.0f, cb.midPoint[0], cb.midPoint[1],
								cb.midPoint[2],
								lb.halfSize[0] * 2.0f, lb.halfSize[1] * 2.0f,
								lb.halfSize[2] * 2.0f, lb.midPoint[0], lb.midPoint[1],
								lb.midPoint[2], clipmap->cmodels[i].radius,
								clipmap->cmodels[i].leaf.firstCollAabbIndex,
								clipmap->cmodels[i].leaf.collAabbCount,
								clipmap->cmodels[i].leaf.brushContents,
								list.empty() ? " (nothing)" : list.c_str());
						}
					}

					for (auto i = 1u; i < clipmap->numSubModels; i++)
					{
						walk_leafbrush_nodes(clipmap, clipmap->cmodels[i].leaf.leafBrushNode,
							model_brushes);
					}

					auto world_count = 0, model_count = 0, overlap = 0;
					for (auto b = 0; b < clipmap->info.numBrushes; b++)
					{
						world_count += world_brushes[b] ? 1 : 0;
						model_count += model_brushes[b] ? 1 : 0;
						overlap += (world_brushes[b] && model_brushes[b]) ? 1 : 0;
					}

					const auto sane = model_count > 0 && overlap == 0
						&& model_count * 2 < clipmap->info.numBrushes;

					if (log)
					{
						CLIPMAP_COLLISION_INFO("clipmap collision: leafbrush walk -- world %d, brush models "
							"%d, overlap %d of %u brushes across %u cmodels%s",
							world_count, model_count, overlap, clipmap->info.numBrushes,
							clipmap->numSubModels,
							sane ? "" : " -- implausible, not filtering on it");
					}

					return sane;
				}

				void collect_tree_partitions(const clipMap_t* clipmap, const int root,
					std::vector<bool>& out)
				{
					if (!clipmap->aabbTrees || clipmap->aabbTreeCount <= 0
						|| !clipmap->partitions || clipmap->partitionCount <= 0)
					{
						return;
					}

					std::vector<bool> visited(clipmap->aabbTreeCount, false);
					std::vector<int> stack;
					stack.emplace_back(root);

					while (!stack.empty())
					{
						const auto index = stack.back();
						stack.pop_back();

						if (index < 0 || index >= clipmap->aabbTreeCount || visited[index])
						{
							continue;
						}
						visited[index] = true;

						const auto* tree = &clipmap->aabbTrees[index];

						if (tree->childCount == 0)
						{
							const auto partition = tree->u.partitionIndex;
							if (partition >= 0 && partition < clipmap->partitionCount)
							{
								out[partition] = true;
							}
							continue;
						}

						for (auto c = 0u; c < tree->childCount; c++)
						{
							stack.emplace_back(tree->u.firstChildIndex + static_cast<int>(c));
						}
					}
				}

				constexpr auto PLANE_NORMAL_EPSILON = 1e-4f;
				constexpr auto PLANE_DIST_EPSILON = PLANE_EPSILON;

				bool add_plane(std::vector<plane>& planes, const plane& p)
				{
					for (const auto& existing : planes)
					{
						if (std::fabs(existing.normal[0] - p.normal[0]) < PLANE_NORMAL_EPSILON
							&& std::fabs(existing.normal[1] - p.normal[1]) < PLANE_NORMAL_EPSILON
							&& std::fabs(existing.normal[2] - p.normal[2]) < PLANE_NORMAL_EPSILON
							&& std::fabs(existing.dist - p.dist) < PLANE_DIST_EPSILON)
						{
							return false;
						}
					}

					planes.emplace_back(p);
					return true;
				}

				void collect_brush_planes(const clipMap_t* clipmap, const cbrush_t* brush,
					const Bounds& bounds, std::vector<plane>& planes,
					std::vector<unsigned short>& tags)
				{
					for (auto side = 0; side < 2; side++)
					{
						for (auto axis = 0; axis < 3; axis++)
						{
							plane p{};
							p.normal[axis] = (side == 0) ? -1.0f : 1.0f;

							const auto centre = bounds.midPoint[axis];
							const auto half = bounds.halfSize[axis];
							p.dist = (side == 0) ? -(centre - half) : (centre + half);

							if (add_plane(planes, p))
							{
								tags.emplace_back(brush->axialMaterialNum[side][axis]);
							}
						}
					}

					for (auto i = 0; i < brush->numsides; i++)
					{
						const auto* side = &brush->sides[i];
						if (!side->plane)
						{
							continue;
						}

						plane p{};
						std::memcpy(p.normal, side->plane->normal, sizeof(float[3]));
						p.dist = side->plane->dist;

						if (add_plane(planes, p))
						{
							tags.emplace_back(side->materialNum);
						}
					}
				}

				constexpr auto WELD_EPSILON = 0.05f;

				unsigned int weld_vertex(std::vector<std::array<float, 3>>& verts,
					const std::array<float, 3>& p)
				{
					for (auto i = 0u; i < verts.size(); i++)
					{
						if (std::fabs(verts[i][0] - p[0]) <= WELD_EPSILON
							&& std::fabs(verts[i][1] - p[1]) <= WELD_EPSILON
							&& std::fabs(verts[i][2] - p[2]) <= WELD_EPSILON)
						{
							return i;
						}
					}

					verts.emplace_back(p);
					return static_cast<unsigned int>(verts.size() - 1);
				}

				bool build_convex_hull(const std::vector<plane>& planes, convex_hull& out)
				{
					constexpr auto MAX_VERTS = 252u;
					constexpr auto MAX_FACE_INDICES = 255u;

					for (auto i = 0u; i < planes.size(); i++)
					{
						auto w = base_winding_for_plane(planes[i]);

						for (auto j = 0u; j < planes.size() && !w.points.empty(); j++)
						{
							if (i != j)
							{
								clip_winding(w, planes[j]);
							}
						}

						if (w.points.size() < 3)
						{
							continue;
						}

						convex_face face{};
						std::memcpy(face.plane, planes[i].normal, sizeof(float[3]));
						face.plane[3] = planes[i].dist;

						const auto committed_verts = out.verts.size();

						for (const auto& point : w.points)
						{
							const auto index = weld_vertex(out.verts, point);
							if (index >= MAX_VERTS)
							{
								return false;
							}
							face.indices.emplace_back(static_cast<unsigned char>(index));
						}

						face.indices.erase(
							std::unique(face.indices.begin(), face.indices.end()),
							face.indices.end());
						while (face.indices.size() > 1
							&& face.indices.front() == face.indices.back())
						{
							face.indices.pop_back();
						}

						auto distinct = face.indices;
						std::sort(distinct.begin(), distinct.end());
						distinct.erase(std::unique(distinct.begin(), distinct.end()),
							distinct.end());
						if (distinct.size() < 3 || distinct.size() != face.indices.size()
							|| face.indices.size() > MAX_FACE_INDICES)
						{
							out.verts.resize(committed_verts);
							continue;
						}

						float area[3] = {0.0f, 0.0f, 0.0f};
						for (auto k = 0u; k < w.points.size(); k++)
						{
							const auto& a = w.points[k];
							const auto& b = w.points[(k + 1) % w.points.size()];
							area[0] += a[1] * b[2] - a[2] * b[1];
							area[1] += a[2] * b[0] - a[0] * b[2];
							area[2] += a[0] * b[1] - a[1] * b[0];
						}

						if (area[0] * face.plane[0] + area[1] * face.plane[1]
							+ area[2] * face.plane[2] < 0.0f)
						{
							std::reverse(face.indices.begin(), face.indices.end());
						}

						out.faces.emplace_back(std::move(face));
					}

					return out.faces.size() >= 4 && out.verts.size() >= 4;
				}

				unsigned short dominant_tag(const std::vector<unsigned short>& counts)
				{
					auto best = 0u;
					for (auto i = 1u; i < counts.size(); i++)
					{
						if (counts[i] > counts[best])
						{
							best = i;
						}
					}
					return static_cast<unsigned short>(best);
				}

				bool brush_convex_enabled()
				{
					const auto* env = std::getenv("ZT_HAVOK_BRUSH_CONVEX");
					return !(env && env[0] == '0');
				}
			}

			bool obj_dump_enabled()
			{
				const auto* env = std::getenv("ZT_HAVOK_OBJ_DIR");
				return env && env[0];
			}

			std::string obj_dump_path(const char* asset_name, const char* suffix)
			{
				const auto* dir = std::getenv("ZT_HAVOK_OBJ_DIR");
				if (!dir || !dir[0])
				{
					return {};
				}

				std::string stem = asset_name ? asset_name : "unknown";
				for (auto& c : stem)
				{
					if (c == '/' || c == '\\' || c == ':' || c == '*' || c == '?'
						|| c == '"' || c == '<' || c == '>' || c == '|')
					{
						c = '_';
					}
				}

				std::string path = dir;
				if (!path.empty() && path.back() != '/' && path.back() != '\\')
				{
					path += '\\';
				}
				return path + stem + suffix;
			}

			void write_triangles_obj(const std::string& path,
				const std::vector<havok_triangle>& triangles, const std::size_t trisoup_count,
				const std::vector<havok_convex>& convexes)
			{
				std::ofstream file(path, std::ios::out | std::ios::trunc);
				if (!file)
				{
					ZONETOOL_ERROR("clipmap collision: cannot write \"%s\"", path.data());
					return;
				}

				file << "# ZoneTool IW5 -> IW7 world collision\n";
				file << "# CoD units, exactly as handed to the Havok mesh builder.\n";
				file << "# Corners are per-triangle rather than welded: this is the triangle\n";
				file << "# soup the builder is given, and merging here would hide cracks and\n";
				file << "# T-junctions that are really in the input.\n";
				file << "# " << triangles.size() << " triangles -- " << trisoup_count
					<< " from trisoup, " << (triangles.size() - trisoup_count)
					<< " from brushes.\n";
				file << "# " << convexes.size() << " brush convexes (convex custom primitives), "
					"welded hull vertices with their hull faces.\n";

				float mn[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
				float mx[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

				std::vector<std::size_t> first_vert(triangles.size(), 0);
				std::size_t written = 0;

				for (auto i = 0u; i < triangles.size(); i++)
				{
					const auto& tri = triangles[i];
					first_vert[i] = written + 1;

					const float* corners[4] = {
						tri.verts[0], tri.verts[1], tri.verts[2], tri.vert3
					};

					for (auto c = 0; c < (tri.is_quad ? 4 : 3); c++)
					{
						char line[128];
						std::snprintf(line, sizeof(line), "v %.4f %.4f %.4f\n",
							corners[c][0], corners[c][1], corners[c][2]);
						file << line;
						written++;

						for (auto k = 0; k < 3; k++)
						{
							mn[k] = std::min(mn[k], corners[c][k]);
							mx[k] = std::max(mx[k], corners[c][k]);
						}
					}
				}

				std::vector<std::size_t> convex_first_vert(convexes.size(), 0);
				for (auto i = 0u; i < convexes.size(); i++)
				{
					convex_first_vert[i] = written + 1;
					for (const auto& v : convexes[i].verts)
					{
						char line[128];
						std::snprintf(line, sizeof(line), "v %.4f %.4f %.4f\n", v[0], v[1], v[2]);
						file << line;
						written++;

						for (auto k = 0; k < 3; k++)
						{
							mn[k] = std::min(mn[k], v[k]);
							mx[k] = std::max(mx[k], v[k]);
						}
					}
				}

				if (!triangles.empty() || !convexes.empty())
				{
					CLIPMAP_COLLISION_INFO("clipmap collision: world obj extents (%.1f %.1f %.1f) .. "
						"(%.1f %.1f %.1f), size %.1f x %.1f x %.1f",
						mn[0], mn[1], mn[2], mx[0], mx[1], mx[2],
						mx[0] - mn[0], mx[1] - mn[1], mx[2] - mn[2]);
				}

				std::vector<std::pair<bool, int>> groups;
				for (auto i = 0u; i < triangles.size(); i++)
				{
					const std::pair<bool, int> key{i >= trisoup_count, triangles[i].contents};
					if (std::find(groups.begin(), groups.end(), key) == groups.end())
					{
						groups.emplace_back(key);
					}
				}

				for (const auto& group : groups)
				{
					char name[128];
					std::snprintf(name, sizeof(name), "o %s_contents_0x%08X\n",
						group.first ? "brushes" : "trisoup", group.second);
					file << name;

					for (auto i = 0u; i < triangles.size(); i++)
					{
						if ((i >= trisoup_count) != group.first
							|| triangles[i].contents != group.second)
						{
							continue;
						}

						const auto base = first_vert[i];
						char line[64];
						if (triangles[i].is_quad)
						{
							std::snprintf(line, sizeof(line), "f %zu %zu %zu %zu\n",
								base, base + 1, base + 2, base + 3);
						}
						else
						{
							std::snprintf(line, sizeof(line), "f %zu %zu %zu\n",
								base, base + 1, base + 2);
						}
						file << line;
					}
				}

				std::vector<int> convex_groups;
				for (const auto& cvx : convexes)
				{
					if (std::find(convex_groups.begin(), convex_groups.end(), cvx.contents)
						== convex_groups.end())
					{
						convex_groups.emplace_back(cvx.contents);
					}
				}

				for (const auto contents : convex_groups)
				{
					char name[128];
					std::snprintf(name, sizeof(name), "o brush_convex_contents_0x%08X\n",
						static_cast<unsigned int>(contents));
					file << name;

					for (auto i = 0u; i < convexes.size(); i++)
					{
						if (convexes[i].contents != contents)
						{
							continue;
						}

						for (const auto& face : convexes[i].faces)
						{
							std::string line = "f";
							for (const auto index : face.indices)
							{
								char corner[32];
								std::snprintf(corner, sizeof(corner), " %zu",
									convex_first_vert[i] + static_cast<std::size_t>(index));
								line += corner;
							}
							file << line << "\n";
						}
					}
				}

				CLIPMAP_COLLISION_INFO("clipmap collision: wrote \"%s\" (%zu triangles, %zu convexes, "
					"%zu objects)", path.data(), triangles.size(), convexes.size(),
					groups.size() + convex_groups.size());
			}

			void write_hulls_obj(const std::string& path, const std::vector<hull_group>& groups,
				const float scale)
			{
				std::ofstream file(path, std::ios::out | std::ios::trunc);
				if (!file)
				{
					ZONETOOL_ERROR("clipmap collision: cannot write \"%s\"", path.data());
					return;
				}

				file << "# ZoneTool IW5 -> IW7 convex hulls, scaled by " << scale
					<< " to match what the builder stores.\n";
				file << "# Faces are written as n-gons, one per polytope face, so the face\n";
				file << "# structure the Havok builder serialises is what you see -- not a\n";
				file << "# triangulation of it. A face that looks wrong here is wrong there.\n";

				auto base = 1u;
				auto total_hulls = 0u;
				for (const auto& group : groups)
				{
					file << "o " << group.name << "\n";

					for (const auto& hull : group.hulls)
					{
						for (const auto& v : hull.verts)
						{
							char line[128];
							std::snprintf(line, sizeof(line), "v %.4f %.4f %.4f\n",
								v[0] * scale, v[1] * scale, v[2] * scale);
							file << line;
						}

						for (const auto& face : hull.faces)
						{
							std::string line = "f";
							for (const auto index : face.indices)
							{
								char corner[32];
								std::snprintf(corner, sizeof(corner), " %u",
									base + static_cast<unsigned int>(index));
								line += corner;
							}
							file << line << "\n";
						}

						base += static_cast<unsigned int>(hull.verts.size());
						total_hulls++;
					}
				}

				CLIPMAP_COLLISION_INFO("clipmap collision: wrote \"%s\" (%u hulls, %zu objects)",
					path.data(), total_hulls, groups.size());
			}

			void write_blob(const std::string& path, const std::uint8_t* data,
				const std::size_t size)
			{
				if (!data || !size)
				{
					return;
				}

				std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::trunc);
				if (!file)
				{
					ZONETOOL_ERROR("clipmap collision: cannot write \"%s\"", path.data());
					return;
				}

				file.write(reinterpret_cast<const char*>(data), size);
				CLIPMAP_COLLISION_INFO("clipmap collision: wrote \"%s\" (%zu bytes)", path.data(), size);
			}

			bool one_material_enabled()
			{
				const auto* env = std::getenv("ZT_HAVOK_ONE_MATERIAL");
				return env && env[0] == '1';
			}

			unsigned int iw7_material_crc(const int surface_flags)
			{
				constexpr auto METAL = 0xCBF7A6C4u;
				constexpr auto PAINTED_METAL = 0xE8F3FA9Au;
				constexpr auto CONCRETE = 0x1AB7BC33u;
				constexpr auto WOOD = 0x0AD71E4Eu;
				constexpr auto PAPER = 0xA1F93A3Bu;
				constexpr auto CLOTH = 0xCD123193u;
				constexpr auto GLASS = 0xF728E572u;
				constexpr auto ROCK = 0x0103BCE1u;
				constexpr auto FOLIAGE = 0x63A1FDADu;
				constexpr auto CERAMIC = 0xF49C81BFu;
				constexpr auto RUBBER = 0xFFD772CDu;
				constexpr auto CUSHION = 0x98C096F9u;
				constexpr auto FLESH = 0x4724ADF2u;
				constexpr auto FRUIT = 0x4FE888BAu;
				constexpr auto SAND = 0x96309552u;
				constexpr auto BRICK = 0xD8B39111u;
				constexpr auto MUD = 0xAE2DE6F5u;
				constexpr auto PLASTIC = 0x4B02BC9Du;
				constexpr auto PLASTER = 0x8D07D363u;
				constexpr auto CARPET = 0x04E51705u;
				constexpr auto ICE = 0x820231A4u;

				static const unsigned int BY_SURFACE_TYPE[SURF_TYPE_COUNT] = {
					/* DEFAULT       */ CONCRETE,
					/* BARK          */ WOOD,
					/* BRICK         */ BRICK,
					/* CARPET        */ CARPET,  
					/* CLOTH         */ CLOTH,
					/* CONCRETE      */ CONCRETE,
					/* DIRT          */ MUD,
					/* FLESH         */ FLESH,
					/* FOLIAGE       */ FOLIAGE,
					/* GLASS         */ GLASS,
					/* GRASS         */ FOLIAGE, 
					/* GRAVEL        */ ROCK,    
					/* ICE           */ ICE,     
					/* METAL         */ METAL,
					/* MUD           */ MUD,
					/* PAPER         */ PAPER,
					/* PLASTER       */ PLASTER, 
					/* ROCK          */ ROCK,
					/* SAND          */ SAND,
					/* SNOW          */ CONCRETE,
					/* WATER         */ CONCRETE,
					/* WOOD          */ WOOD,
					/* ASPHALT       */ ROCK,    
					/* CERAMIC       */ CERAMIC,
					/* PLASTIC       */ PLASTIC, 
					/* RUBBER        */ RUBBER,  
					/* CUSHION       */ CUSHION,
					/* FRUIT         */ FRUIT,
					/* PAINTED_METAL */ PAINTED_METAL,
					/* RIOT_SHIELD   */ RUBBER,  
					/* SLUSH         */ MUD,     
				};

				if (one_material_enabled())
				{
					return CONCRETE;
				}

				const auto type = (static_cast<unsigned int>(surface_flags) >> 20) & 0x1Fu;
				if (type >= SURF_TYPE_COUNT)
				{
					return CONCRETE;
				}

				return BY_SURFACE_TYPE[type];
			}

			bool trisoup_material_contents_enabled()
			{
				const auto* env = std::getenv("ZT_HAVOK_TRISOUP_MATERIAL");
				return env && env[0] == '1';
			}

			static world_collision extract_collision(clipMap_t* clipmap,
				const float scale_override, const bool brush_convexes)
			{
				world_collision result;
				auto& triangles = result.triangles;

				if (!clipmap)
				{
					return result;
				}

				CLIPMAP_COLLISION_INFO("clipmap collision: source has %u verts, %d tris, %d partitions, "
					"%u brushes, %u static models, %u submodels",
					clipmap->vertCount, clipmap->triCount, clipmap->partitionCount,
					clipmap->info.numBrushes, clipmap->numStaticModels, clipmap->numSubModels);

				std::vector<unsigned short> partition_material(
					std::max(clipmap->partitionCount, 0), 0);

				std::vector<bool> model_partitions(std::max(clipmap->partitionCount, 0), false);

				for (auto i = 0; i < clipmap->aabbTreeCount; i++)
				{
					const auto* tree = &clipmap->aabbTrees[i];
					if (tree->childCount != 0)
					{
						continue;
					}

					const auto partition_index = tree->u.partitionIndex;
					if (partition_index < 0 || partition_index >= clipmap->partitionCount)
					{
						continue;
					}

					partition_material[partition_index] = tree->materialIndex;
				}

				if (clipmap->cmodels && clipmap->numSubModels > 1)
				{
					for (auto m = 1u; m < clipmap->numSubModels; m++)
					{
						const auto& leaf = clipmap->cmodels[m].leaf;
						for (auto o = 0u; o < leaf.collAabbCount; o++)
						{
							collect_tree_partitions(clipmap,
								static_cast<int>(leaf.firstCollAabbIndex) + static_cast<int>(o),
								model_partitions);
						}
					}
				}

				std::vector<unsigned int> material_crc(clipmap->info.numMaterials,
					iw7_material_crc(0));
				std::vector<int> material_contents(clipmap->info.numMaterials, CONTENTS_SOLID);
				for (auto i = 0u; i < clipmap->info.numMaterials; i++)
				{
					if (!clipmap->info.materials)
					{
						break;
					}

					material_crc[i] = iw7_material_crc(clipmap->info.materials[i].surfaceFlags);

					const auto contents = clipmap->info.materials[i].contents;
					material_contents[i] = contents ? contents : CONTENTS_SOLID;
				}
				for (auto i = 0u; i < clipmap->info.numMaterials && clipmap->info.materials; i++)
				{
					const auto sf = clipmap->info.materials[i].surfaceFlags;
					const auto old_index = static_cast<unsigned int>(sf) >> 20;
					const auto new_index = old_index & 0x1Fu;
					CLIPMAP_COLLISION_INFO("  material %2u \"%s\" surfaceFlags 0x%08X contents 0x%08X "
						"type %u->%u%s iw7flags 0x%08X crc 0x%08X", i,
						clipmap->info.materials[i].name ? clipmap->info.materials[i].name : "?",
						sf, clipmap->info.materials[i].contents, old_index, new_index,
						old_index != new_index ? " CHANGED" : "",
						static_cast<unsigned int>(xmodel::convert_surface_flags(sf)), material_crc[i]);
				}

				const auto crc_for = [&](const unsigned short index)
				{
					return index < material_crc.size() ? material_crc[index] : 0x1AB7BC33u;
				};
				const auto contents_for = [&](const unsigned short index)
				{
					return index < material_contents.size()
						? material_contents[index] : CONTENTS_SOLID;
				};

				const auto brush_basis_enabled = []
				{
					const auto* env = std::getenv("ZT_HAVOK_BRUSH_BASIS");
					return !(env && env[0] == '0');
				}();

				const std::uint64_t USERDATA_BRUSH_BASIS =
					brush_basis_enabled ? (1ull << 48) : 0ull;

				CLIPMAP_COLLISION_INFO("clipmap collision: brush basis bit %s (ZT_HAVOK_BRUSH_BASIS)",
					brush_basis_enabled ? "set" : "CLEARED");
				const auto surface_flags_for = [&](const unsigned short index) -> std::uint64_t
				{
					if (!clipmap->info.materials || index >= clipmap->info.numMaterials)
					{
						return 0;
					}
					return static_cast<std::uint32_t>(
						xmodel::convert_surface_flags(clipmap->info.materials[index].surfaceFlags));
				};

				auto trisoup_emitted = 0;
				auto trisoup_covered = 0;
				auto trisoup_skipped_model = 0;
				auto trisoup_skipped_oob = 0;
				auto trisoup_partitions = 0;

				if (clipmap->partitions && clipmap->triIndices && clipmap->verts
					&& clipmap->triCount > 0 && clipmap->vertCount > 0)
				{
					for (auto p = 0; p < clipmap->partitionCount; p++)
					{
						const auto* partition = &clipmap->partitions[p];
						trisoup_covered += partition->triCount;

						if (model_partitions[p])
						{
							trisoup_skipped_model += partition->triCount;
							continue;
						}

						trisoup_partitions++;

						const auto vert_base =
							1024u * static_cast<unsigned int>(partition->firstVertSegment);

						const auto material = partition_material[p];
						const auto crc = crc_for(material);
						const auto flags = surface_flags_for(material);

						auto contents = trisoup_material_contents_enabled()
							? contents_for(material)
							: CONTENTS_SOLID;
						if ((contents & CONTENTS_SOLID) && solid_as_clip_enabled())
						{
							contents = (contents & ~CONTENTS_SOLID)
								| static_cast<int>(solid_as_clip_contents());
						}

						for (auto o = 0; o < partition->triCount; o++)
						{
							const auto tri_index = partition->firstTri + o;
							if (tri_index < 0 || tri_index >= clipmap->triCount)
							{
								trisoup_skipped_oob++;
								continue;
							}

							havok_triangle tri{};
							tri.surface_tag = material;
							tri.material_crc = crc;
							tri.user_data = flags;
							tri.contents = contents;

							auto degenerate = false;
							for (auto c = 0; c < 3; c++)
							{
								const auto index = vert_base
									+ clipmap->triIndices[tri_index * 3 + c];
								if (index >= clipmap->vertCount)
								{
									degenerate = true;
									break;
								}
								std::memcpy(tri.verts[c], clipmap->verts[index],
									sizeof(float[3]));
							}

							if (degenerate)
							{
								trisoup_skipped_oob++;
								continue;
							}

							triangles.emplace_back(tri);
							trisoup_emitted++;
						}
					}
				}

				CLIPMAP_COLLISION_INFO("clipmap collision: trisoup -- %d triangles from %d of %d "
					"partitions (%d skipped as brush-model geometry, %d out of range)",
					trisoup_emitted, trisoup_partitions, clipmap->partitionCount,
					trisoup_skipped_model, trisoup_skipped_oob);

				if (trisoup_covered != clipmap->triCount)
				{
					ZONETOOL_WARNING("clipmap collision: partitions cover %d of %d trisoup "
						"triangles; the remainder have no known vertex segment and are dropped",
						trisoup_covered, clipmap->triCount);
				}

				const auto trisoup_count = triangles.size();
				result.trisoup_count = trisoup_count;

				auto world_scale = 0.03125f;
				if (scale_override > 0.0f)
				{
					world_scale = scale_override;
				}
				else if (const auto* env = std::getenv("ZT_HAVOK_WORLD_SCALE"))
				{
					const auto parsed = static_cast<float>(std::atof(env));
					if (parsed > 0.0f)
					{
						world_scale = parsed;
					}
				}

				CLIPMAP_COLLISION_INFO("clipmap collision: brushes emitted as %s",
					brush_convexes
						? "convex custom primitives, faces only as a fallback "
						  "(ZT_HAVOK_BRUSH_CONVEX default on)"
						: "triangle faces (ZT_HAVOK_BRUSH_CONVEX=0, or a triangle-only caller)");

				auto brushes_with_sides = 0;
				auto brushes_as_convex = 0;
				auto fallback_hull_failed = 0;
				std::map<std::string, int> fallback_rejected;
				auto total_nonaxial = 0;
				auto faces_emitted = 0;
				auto faces_clipped_away = 0;
				auto glass_pieces = 0u;
				auto skipped_nonsolid = 0;
				auto skipped_trigger = 0;
				auto skipped_model = 0;
				auto skipped_degenerate = 0;
				auto kept_clip = 0;

				std::vector<std::pair<int, int>> contents_hist;

				std::vector<bool> world_brushes;
				std::vector<bool> model_brushes;
				const auto filter_models =
					brush_model_attribution(clipmap, world_brushes, model_brushes,
						verbose_logging_enabled());

				for (auto b = 0; b < clipmap->info.numBrushes; b++)
				{
					const auto* brush = &clipmap->info.brushes[b];

					if (brush->numsides > 0)
					{
						brushes_with_sides++;
						total_nonaxial += brush->numsides;
					}

					auto contents = (clipmap->info.brushContents
						? clipmap->info.brushContents[b] : 0) & ~CONTENTS_COMPILE_ONLY;

					if ((contents & CONTENTS_SOLID) && solid_as_clip_enabled())
					{
						contents = (contents & ~CONTENTS_SOLID)
							| static_cast<int>(solid_as_clip_contents());
					}
					if (verbose_logging_enabled())
					{
						auto found = false;
						for (auto& entry : contents_hist)
						{
							if (entry.first == contents)
							{
								entry.second++;
								found = true;
								break;
							}
						}
						if (!found)
						{
							contents_hist.emplace_back(contents, 1);
						}
					}

					const auto& bb = clipmap->info.brushBounds[b];

					const char* skip = nullptr;

					if (bb.halfSize[0] <= 0.0f || bb.halfSize[1] <= 0.0f || bb.halfSize[2] <= 0.0f)
					{
						skip = "degenerate";
						skipped_degenerate++;
					}
					else if (contents & CONTENTS_TRIGGER)
					{
						skip = "trigger";
						skipped_trigger++;
					}
					else if (filter_models && model_brushes[b] && !world_brushes[b])
					{
						skip = "brush model";
						skipped_model++;
					}
					else if ((contents & CONTENTS_NONCOLLIDING) || !contents)
					{
						skip = "non-colliding";
						skipped_nonsolid++;
					}

					CLIPMAP_COLLISION_INFO("  brush %3d contents 0x%08X sides %2d  size %.0f x %.0f x %.0f "
						"at (%.0f %.0f %.0f)%s%s", b, contents, brush->numsides,
						bb.halfSize[0] * 2.0f, bb.halfSize[1] * 2.0f, bb.halfSize[2] * 2.0f,
						bb.midPoint[0], bb.midPoint[1], bb.midPoint[2],
						skip ? "  SKIPPED: " : "", skip ? skip : "");

					if (skip)
					{
						continue;
					}

					if (contents & 0x00030000)
					{
						kept_clip++;
					}

					std::vector<plane> planes;
					std::vector<unsigned short> tags;
					collect_brush_planes(clipmap, brush, clipmap->info.brushBounds[b], planes, tags);

					std::uint64_t glass_piece = 0;

					if (brush_convexes)
					{
						convex_hull hull{};
						if (!build_convex_hull(planes, hull))
						{
							fallback_hull_failed++;
						}
						else
						{
							havok_convex cvx{};
							cvx.verts.reserve(hull.verts.size());
							for (const auto& v : hull.verts)
							{
								cvx.verts.push_back({v[0] * world_scale, v[1] * world_scale,
									v[2] * world_scale});
							}

							if (const auto* reason =
								zonetool::iw7::havok::builder::convex_rejection(cvx.verts))
							{
								fallback_rejected[reason]++;
							}
							else
							{
								std::vector<unsigned short> tag_votes(
									clipmap->info.materials ? clipmap->info.numMaterials : 0u, 0);
								for (const auto t : tags)
								{
									if (t < tag_votes.size() && tag_votes[t] < 0xFFFF)
									{
										tag_votes[t]++;
									}
								}

								auto tag = tags.empty() ? static_cast<unsigned short>(0) : tags[0];
								if (!tag_votes.empty())
								{
									const auto dominant = dominant_tag(tag_votes);
									if (tag_votes[dominant] > 0)
									{
										tag = dominant;
									}
								}

								const auto flags = surface_flags_for(tag);
								auto user_data = USERDATA_BRUSH_BASIS | flags;
								if (is_glass_surface(flags))
								{
									if (!glass_piece && glass_pieces < USERDATA_GLASS_PIECE_MAX)
									{
										glass_piece = static_cast<std::uint64_t>(++glass_pieces)
											<< USERDATA_GLASS_PIECE_SHIFT;
									}
									user_data |= glass_piece;
								}

								cvx.surface_tag = tag;
								cvx.contents = contents;
								cvx.material_crc = crc_for(tag);
								cvx.user_data = user_data;

								cvx.faces = std::move(hull.faces);

								result.convexes.emplace_back(std::move(cvx));
								brushes_as_convex++;
								continue;
							}
						}
					}

					for (auto i = 0u; i < planes.size(); i++)
					{
						auto w = base_winding_for_plane(planes[i]);

						for (auto j = 0u; j < planes.size() && !w.points.empty(); j++)
						{
							if (i != j)
							{
								clip_winding(w, planes[j]);
							}
						}

						if (w.points.size() >= 3)
						{
							faces_emitted++;
						}
						else
						{
							faces_clipped_away++;
						}

						const auto face_flags = surface_flags_for(tags[i]);
						auto face_user_data = USERDATA_BRUSH_BASIS | face_flags;

						if (is_glass_surface(face_flags))
						{
							if (!glass_piece && glass_pieces < USERDATA_GLASS_PIECE_MAX)
							{
								glass_piece = static_cast<std::uint64_t>(++glass_pieces)
									<< USERDATA_GLASS_PIECE_SHIFT;
							}
							face_user_data |= glass_piece;
						}

						face_to_triangles(w, planes[i], tags[i], contents, crc_for(tags[i]),
							face_user_data, triangles);
					}
				}

				for (const auto& entry : contents_hist)
				{
					CLIPMAP_COLLISION_INFO("clipmap collision: contents 0x%08X -> %d brushes",
						entry.first, entry.second);
				}

				CLIPMAP_COLLISION_INFO("clipmap collision: %d brushes, %d with non-axial sides "
					"(%d planes total); %d hull faces emitted, %d fully clipped away",
					clipmap->info.numBrushes, brushes_with_sides, total_nonaxial,
					faces_emitted, faces_clipped_away);

				if (glass_pieces)
				{
					CLIPMAP_COLLISION_INFO("clipmap collision: %u glass piece(s) indexed in userData "
						"bits 32..39%s", glass_pieces,
						glass_pieces >= USERDATA_GLASS_PIECE_MAX
							? " -- CAPPED at the byte's range; further panes are left "
							  "unindexed (0), as stock leaves non-glass" : "");
				}

				CLIPMAP_COLLISION_INFO("clipmap collision: skipped %d brush-model, %d trigger, "
					"%d non-colliding, %d degenerate brushes", skipped_model, skipped_trigger,
					skipped_nonsolid, skipped_degenerate);

				if (kept_clip)
				{
					CLIPMAP_COLLISION_INFO("clipmap collision: %d of the kept brushes are clip "
						"volumes (player/monster clip)", kept_clip);
				}

				if (skipped_trigger > 0)
				{
					CLIPMAP_COLLISION_INFO("clipmap collision: the %d trigger brushes are carried by "
						"MapEnts::trigger (%u trigger models), not by the world shape",
						skipped_trigger,
						clipmap->mapEnts ? clipmap->mapEnts->trigger.count : 0u);
				}

				CLIPMAP_COLLISION_INFO("clipmap collision: %zu triangles from trisoup, %zu from %d brushes",
					trisoup_count, triangles.size() - trisoup_count, clipmap->info.numBrushes);

				if (brush_convexes)
				{
					auto fallback_total = fallback_hull_failed;
					std::string reasons = std::to_string(fallback_hull_failed)
						+ " hull could not be built";
					for (const auto& [reason, count] : fallback_rejected)
					{
						fallback_total += count;
						reasons += ", " + std::to_string(count) + " " + reason;
					}

					CLIPMAP_COLLISION_INFO("clipmap collision: %d brushes emitted as convex custom "
						"primitives", brushes_as_convex);
					if (fallback_total)
					{
						ZONETOOL_WARNING("clipmap collision: %d brushes fell back to triangle "
							"faces (reasons: %s) -- the player movement cast does not collide "
							"with those", fallback_total, reasons.c_str());
					}
					else
					{
						CLIPMAP_COLLISION_INFO("clipmap collision: 0 brushes fell back to triangle faces");
					}
				}

				if (world_scale != 1.0f)
				{
					for (auto& tri : triangles)
					{
						for (auto c = 0; c < 3; c++)
						{
							for (auto k = 0; k < 3; k++)
							{
								tri.verts[c][k] *= world_scale;
							}
						}

						if (tri.is_quad)
						{
							for (auto k = 0; k < 3; k++)
							{
								tri.vert3[k] *= world_scale;
							}
						}
					}
				}

				CLIPMAP_COLLISION_INFO("clipmap collision: world scale %g (ZT_HAVOK_WORLD_SCALE)",
					world_scale);

				if (obj_dump_enabled())
				{
					write_triangles_obj(obj_dump_path(clipmap->name, ".world.obj"),
						triangles, trisoup_count, result.convexes);
				}

				return result;
			}

			std::vector<havok_triangle> extract(clipMap_t* clipmap, float scale_override)
			{
				return extract_collision(clipmap, scale_override, false).triangles;
			}

			world_collision extract_world(clipMap_t* clipmap)
			{
				return extract_collision(clipmap, 0.0f, brush_convex_enabled());
			}

			namespace
			{
				unsigned int material_crc_for(const clipMap_t* clipmap,
					const unsigned short index)
				{
					if (!clipmap || !clipmap->info.materials
						|| index >= clipmap->info.numMaterials)
					{
						return 0x1AB7BC33u;
					}
					return iw7_material_crc(clipmap->info.materials[index].surfaceFlags);
				}

				std::uint64_t surface_flags_for(const clipMap_t* clipmap,
					const unsigned short index)
				{
					if (!clipmap || !clipmap->info.materials
						|| index >= clipmap->info.numMaterials)
					{
						return 0;
					}
					return static_cast<std::uint32_t>(
						xmodel::convert_surface_flags(clipmap->info.materials[index].surfaceFlags));
				}

			}

			std::vector<brush_model> extract_brush_models(clipMap_t* clipmap)
			{
				std::vector<brush_model> models;

				if (!clipmap || !clipmap->cmodels || clipmap->numSubModels < 2
					|| !clipmap->info.brushes || !clipmap->info.brushBounds)
				{
					return models;
				}

				auto skipped_brushes = 0;
				auto total_hulls = 0;

				{
					std::vector<bool> world_brushes;
					std::vector<bool> model_brushes;
					if (!brush_model_attribution(clipmap, world_brushes, model_brushes, false))
					{
						ZONETOOL_WARNING("clipmap collision: brush models cannot be attributed to "
							"their cmodels, so they stay in the world mesh -- emitting no entity "
							"shapes for them rather than duplicating the collision");
						return models;
					}
				}

				for (auto i = 1u; i < clipmap->numSubModels; i++)
				{
					const auto brushes = leafbrushes_of(clipmap, clipmap->cmodels[i].leaf.leafBrushNode);
					if (brushes.empty())
					{
						continue;
					}

					brush_model model{};
					model.index = i;

					float union_min[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
					float union_max[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

					std::vector<unsigned short> tag_votes(
						clipmap->info.materials ? clipmap->info.numMaterials : 0u, 0);

					for (const auto b : brushes)
					{
						const auto& bb = clipmap->info.brushBounds[b];
						if (bb.halfSize[0] <= 0.0f || bb.halfSize[1] <= 0.0f
							|| bb.halfSize[2] <= 0.0f)
						{
							skipped_brushes++;
							continue;
						}

						const auto brush_contents = (clipmap->info.brushContents
							? clipmap->info.brushContents[b] : 0) & ~CONTENTS_COMPILE_ONLY;

						if ((brush_contents & CONTENTS_TRIGGER)
							|| (brush_contents & CONTENTS_NONCOLLIDING)
							|| !brush_contents)
						{
							skipped_brushes++;
							continue;
						}

						std::vector<plane> planes;
						std::vector<unsigned short> tags;
						collect_brush_planes(clipmap, &clipmap->info.brushes[b], bb, planes, tags);

						convex_hull hull{};
						if (!build_convex_hull(planes, hull))
						{
							skipped_brushes++;
							continue;
						}

						for (const auto& v : hull.verts)
						{
							for (auto c = 0; c < 3; c++)
							{
								union_min[c] = std::min(union_min[c], v[c]);
								union_max[c] = std::max(union_max[c], v[c]);
							}
						}

						for (const auto t : tags)
						{
							if (t < tag_votes.size() && tag_votes[t] < 0xFFFF)
							{
								tag_votes[t]++;
							}
						}

						model.contents |= brush_contents;
						model.hulls.emplace_back(std::move(hull));
					}

					if (model.hulls.empty())
					{
						continue;
					}

					if (!tag_votes.empty())
					{
						const auto tag = dominant_tag(tag_votes);
						if (tag_votes[tag] > 0)
						{
							model.material_crc = material_crc_for(clipmap, tag);
							model.surface_flags = surface_flags_for(clipmap, tag);
						}
					}

					float offset[3];
					auto shifted = false;
					for (auto c = 0; c < 3; c++)
					{
						const auto hull_mid = (union_min[c] + union_max[c]) * 0.5f;
						offset[c] = hull_mid - clipmap->cmodels[i].bounds.midPoint[c];
						if (std::fabs(offset[c]) > 0.5f)
						{
							shifted = true;
						}
					}

					if (shifted)
					{
						for (auto& hull : model.hulls)
						{
							for (auto& v : hull.verts)
							{
								for (auto c = 0; c < 3; c++)
								{
									v[c] -= offset[c];
								}
							}

							for (auto& face : hull.faces)
							{
								face.plane[3] -= face.plane[0] * offset[0]
									+ face.plane[1] * offset[1] + face.plane[2] * offset[2];
							}
						}
					}

					total_hulls += static_cast<int>(model.hulls.size());
					models.emplace_back(std::move(model));
				}

				CLIPMAP_COLLISION_INFO("clipmap collision: %zu brush models with geometry (%d hulls, "
					"%d brushes skipped) out of %u submodels", models.size(), total_hulls,
					skipped_brushes, clipmap->numSubModels);

				return models;
			}

			std::vector<convex_hull> extract_phys_collmap(const PhysCollmap* collmap)
			{
				std::vector<convex_hull> out;
				if (!collmap || !collmap->geoms || !collmap->count)
				{
					return out;
				}

				for (auto g = 0u; g < collmap->count; g++)
				{
					const auto* geom = &collmap->geoms[g];

					convex_hull hull{};

					if (geom->brushWrapper)
					{
						const auto& bounds = geom->brushWrapper->bounds;

						std::vector<plane> planes;
						for (auto side = 0; side < 2; side++)
						{
							for (auto axis = 0; axis < 3; axis++)
							{
								plane pl{};
								pl.normal[axis] = (side == 0) ? -1.0f : 1.0f;
								const auto centre = bounds.midPoint[axis];
								const auto half = bounds.halfSize[axis];
								pl.dist = (side == 0) ? -(centre - half) : (centre + half);
								add_plane(planes, pl);
							}
						}

						const auto& brush = geom->brushWrapper->brush;
						for (auto i = 0; i < brush.numsides; i++)
						{
							if (!brush.sides || !brush.sides[i].plane)
							{
								continue;
							}
							plane pl{};
							std::memcpy(pl.normal, brush.sides[i].plane->normal, sizeof(float[3]));
							pl.dist = brush.sides[i].plane->dist;
							add_plane(planes, pl);
						}

						if (!build_convex_hull(planes, hull) || hull.verts.empty())
						{
							continue;
						}

						out.emplace_back(std::move(hull));
						continue;
					}

					const auto& half = geom->bounds.halfSize;

					std::vector<plane> planes;
					const auto add_local = [&planes](const float x, const float y, const float z,
						const float dist)
					{
						plane pl{};
						pl.normal[0] = x;
						pl.normal[1] = y;
						pl.normal[2] = z;
						pl.dist = dist;
						add_plane(planes, pl);
					};

					auto axis = -1;
					if (geom->type == PHYS_GEOM_CYLINDER || geom->type == PHYS_GEOM_CAPSULE)
					{
						const auto same = [](const float a, const float b)
						{
							return std::fabs(a - b) <= 0.05f * std::max(1.0f, std::max(a, b));
						};
						if (same(half[1], half[2])) axis = 0;
						else if (same(half[0], half[2])) axis = 1;
						else if (same(half[0], half[1])) axis = 2;
					}

					if (axis < 0)
					{
						for (auto a = 0; a < 3; a++)
						{
							float n[3] = {};
							n[a] = 1.0f;
							add_local(n[0], n[1], n[2], half[a]);
							add_local(-n[0], -n[1], -n[2], half[a]);
						}
					}
					else
					{
						const auto u = (axis + 1) % 3;
						const auto v = (axis + 2) % 3;
						const auto radius = 0.5f * (half[u] + half[v]);
						const auto half_height = half[axis];

						constexpr auto sides = 16;
						constexpr auto two_pi = 6.28318530717958f;

						for (auto i = 0; i < sides; i++)
						{
							const auto a = two_pi * static_cast<float>(i) / sides;
							float n[3] = {};
							n[u] = std::cos(a);
							n[v] = std::sin(a);
							add_local(n[0], n[1], n[2], radius);
						}

						for (auto end = -1; end <= 1; end += 2)
						{
							float n[3] = {};
							n[axis] = static_cast<float>(end);
							add_local(n[0], n[1], n[2], half_height);

							if (geom->type != PHYS_GEOM_CAPSULE)
							{
								continue;
							}

							constexpr auto diag = 0.70710678f;
							const auto sphere = std::max(0.0f, half_height - radius);
							for (auto i = 0; i < sides; i++)
							{
								const auto a = two_pi * (static_cast<float>(i) + 0.5f) / sides;
								float c[3] = {};
								c[u] = std::cos(a) * diag;
								c[v] = std::sin(a) * diag;
								c[axis] = static_cast<float>(end) * diag;
								add_local(c[0], c[1], c[2], sphere * diag + radius);
							}
						}
					}

					if (!build_convex_hull(planes, hull) || hull.verts.empty())
					{
						continue;
					}

					const auto& m = geom->orientation;
					const auto& mid = geom->bounds.midPoint;
					const auto to_model = [&m](const float x, const float y, const float z,
						float(&dst)[3])
					{
						dst[0] = x * m[0][0] + y * m[1][0] + z * m[2][0];
						dst[1] = x * m[0][1] + y * m[1][1] + z * m[2][1];
						dst[2] = x * m[0][2] + y * m[1][2] + z * m[2][2];
					};

					for (auto& vert : hull.verts)
					{
						float placed[3];
						to_model(vert[0], vert[1], vert[2], placed);
						for (auto c = 0; c < 3; c++)
						{
							vert[c] = placed[c] + mid[c];
						}
					}

					for (auto& face : hull.faces)
					{
						float normal[3];
						to_model(face.plane[0], face.plane[1], face.plane[2], normal);

						const auto length = std::sqrt(normal[0] * normal[0]
							+ normal[1] * normal[1] + normal[2] * normal[2]);
						if (length < 1e-6f || face.indices.empty())
						{
							continue;
						}

						for (auto c = 0; c < 3; c++)
						{
							face.plane[c] = normal[c] / length;
						}

						const auto& anchor = hull.verts[face.indices[0]];
						face.plane[3] = face.plane[0] * anchor[0]
							+ face.plane[1] * anchor[1] + face.plane[2] * anchor[2];
					}

					out.emplace_back(std::move(hull));
				}

				return out;
			}

			std::vector<convex_hull> extract_trigger_hulls(const MapTriggers& triggers,
				unsigned int model)
			{
				std::vector<convex_hull> out;

				if (model >= triggers.count || !triggers.models || !triggers.hulls)
				{
					return out;
				}

				const auto& trigger = triggers.models[model];

				for (auto h = 0; h < trigger.hullCount; h++)
				{
					const auto index = trigger.firstHull + h;
					if (index >= triggers.hullCount)
					{
						break;
					}

					const auto& hull = triggers.hulls[index];
					if (hull.bounds.halfSize[0] <= 0.0f || hull.bounds.halfSize[1] <= 0.0f
						|| hull.bounds.halfSize[2] <= 0.0f)
					{
						continue;
					}

					std::vector<plane> planes;

					for (auto side = 0; side < 2; side++)
					{
						for (auto axis = 0; axis < 3; axis++)
						{
							plane p{};
							p.normal[axis] = (side == 0) ? -1.0f : 1.0f;

							const auto centre = hull.bounds.midPoint[axis];
							const auto half = hull.bounds.halfSize[axis];
							p.dist = (side == 0) ? -(centre - half) : (centre + half);

							add_plane(planes, p);
						}
					}

					for (auto sl = 0; sl < hull.slabCount; sl++)
					{
						const auto slab_index = hull.firstSlab + sl;
						if (slab_index >= triggers.slabCount || !triggers.slabs)
						{
							break;
						}

						const auto& slab = triggers.slabs[slab_index];

						plane high{};
						std::memcpy(high.normal, slab.dir, sizeof(float[3]));
						high.dist = slab.midPoint + slab.halfSize;
						add_plane(planes, high);

						plane low{};
						for (auto c = 0; c < 3; c++)
						{
							low.normal[c] = -slab.dir[c];
						}
						low.dist = -(slab.midPoint - slab.halfSize);
						add_plane(planes, low);
					}

					convex_hull built{};
					if (build_convex_hull(planes, built))
					{
						out.emplace_back(std::move(built));
					}
				}

				return out;
			}
		}
	}
}

#undef CLIPMAP_COLLISION_INFO
