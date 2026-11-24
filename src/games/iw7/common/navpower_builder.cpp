#include <std_include.hpp>
#include "iw7.hpp"
#include "navpower_builder.hpp"

#include <algorithm>
#include <array>
#include <cfloat>
#include <climits>
#include <cmath>
#include <cstring>
#include <map>
#include <queue>
#include <tuple>

namespace zonetool::iw7
{
	namespace navpower::builder
	{
		namespace
		{
			const int direction_x[4] = { 1, 0, -1, 0 };
			const int direction_y[4] = { 0, 1, 0, -1 };

			float normal_z(const triangle& tri)
			{
				const auto* a = tri.verts[0];
				const auto* b = tri.verts[1];
				const auto* c = tri.verts[2];
				const float u[3] = { b[0] - a[0], b[1] - a[1], b[2] - a[2] };
				const float v[3] = { c[0] - a[0], c[1] - a[1], c[2] - a[2] };
				const auto nx = u[1] * v[2] - u[2] * v[1];
				const auto ny = u[2] * v[0] - u[0] * v[2];
				const auto nz = u[0] * v[1] - u[1] * v[0];
				const auto length = std::sqrt(nx * nx + ny * ny + nz * nz);
				return length > 0.0f ? nz / length : 0.0f;
			}

			bool height_at(const triangle& tri, const float x, const float y, float& z)
			{
				const auto* a = tri.verts[0];
				const auto* b = tri.verts[1];
				const auto* c = tri.verts[2];
				const auto det = (b[1] - c[1]) * (a[0] - c[0]) + (c[0] - b[0]) * (a[1] - c[1]);
				if (std::abs(det) < 1e-6f)
				{
					return false;
				}

				const auto l0 = ((b[1] - c[1]) * (x - c[0]) + (c[0] - b[0]) * (y - c[1])) / det;
				const auto l1 = ((c[1] - a[1]) * (x - c[0]) + (a[0] - c[0]) * (y - c[1])) / det;
				const auto l2 = 1.0f - l0 - l1;
				if (l0 < -1e-4f || l1 < -1e-4f || l2 < -1e-4f)
				{
					return false;
				}

				z = l0 * a[2] + l1 * b[2] + l2 * c[2];
				return true;
			}

			bool segment_hits(const triangle& tri, const float* from, const float* to)
			{
				const auto* a = tri.verts[0];
				const float dir[3] = { to[0] - from[0], to[1] - from[1], to[2] - from[2] };
				const float e1[3] = { tri.verts[1][0] - a[0], tri.verts[1][1] - a[1], tri.verts[1][2] - a[2] };
				const float e2[3] = { tri.verts[2][0] - a[0], tri.verts[2][1] - a[1], tri.verts[2][2] - a[2] };
				const float p[3] = { dir[1] * e2[2] - dir[2] * e2[1], dir[2] * e2[0] - dir[0] * e2[2], dir[0] * e2[1] - dir[1] * e2[0] };
				const auto det = e1[0] * p[0] + e1[1] * p[1] + e1[2] * p[2];
				if (std::abs(det) < 1e-8f)
				{
					return false;
				}

				const auto inv_det = 1.0f / det;
				const float s[3] = { from[0] - a[0], from[1] - a[1], from[2] - a[2] };
				const auto u = (s[0] * p[0] + s[1] * p[1] + s[2] * p[2]) * inv_det;
				if (u < 0.0f || u > 1.0f)
				{
					return false;
				}

				const float q[3] = { s[1] * e1[2] - s[2] * e1[1], s[2] * e1[0] - s[0] * e1[2], s[0] * e1[1] - s[1] * e1[0] };
				const auto v = (dir[0] * q[0] + dir[1] * q[1] + dir[2] * q[2]) * inv_det;
				if (v < 0.0f || u + v > 1.0f)
				{
					return false;
				}

				const auto t = (e2[0] * q[0] + e2[1] * q[1] + e2[2] * q[2]) * inv_det;
				return t >= 0.0f && t <= 1.0f;
			}

			triangle_buckets make_buckets(const std::vector<triangle>& triangles, const float bucket_size)
			{
				float mins[2] = { FLT_MAX, FLT_MAX };
				float maxs[2] = { -FLT_MAX, -FLT_MAX };
				for (const auto& tri : triangles)
				{
					for (const auto& vert : tri.verts)
					{
						for (auto axis = 0; axis < 2; axis++)
						{
							mins[axis] = std::min(mins[axis], vert[axis]);
							maxs[axis] = std::max(maxs[axis], vert[axis]);
						}
					}
				}

				triangle_buckets buckets{};
				buckets.origin[0] = mins[0];
				buckets.origin[1] = mins[1];
				buckets.bucket_size = bucket_size;
				buckets.width = static_cast<int>((maxs[0] - mins[0]) / bucket_size) + 1;
				buckets.height = static_cast<int>((maxs[1] - mins[1]) / bucket_size) + 1;
				buckets.triangles.resize(static_cast<std::size_t>(buckets.width) * buckets.height);

				for (auto i = 0u; i < triangles.size(); i++)
				{
					const auto& tri = triangles[i];
					float tri_mins[2] = { FLT_MAX, FLT_MAX };
					float tri_maxs[2] = { -FLT_MAX, -FLT_MAX };
					for (const auto& vert : tri.verts)
					{
						for (auto axis = 0; axis < 2; axis++)
						{
							tri_mins[axis] = std::min(tri_mins[axis], vert[axis]);
							tri_maxs[axis] = std::max(tri_maxs[axis], vert[axis]);
						}
					}

					const auto bx0 = static_cast<int>((tri_mins[0] - mins[0]) / bucket_size);
					const auto by0 = static_cast<int>((tri_mins[1] - mins[1]) / bucket_size);
					const auto bx1 = std::min(static_cast<int>((tri_maxs[0] - mins[0]) / bucket_size), buckets.width - 1);
					const auto by1 = std::min(static_cast<int>((tri_maxs[1] - mins[1]) / bucket_size), buckets.height - 1);
					for (auto by = by0; by <= by1; by++)
					{
						for (auto bx = bx0; bx <= bx1; bx++)
						{
							buckets.triangles[static_cast<std::size_t>(by) * buckets.width + bx].push_back(i);
						}
					}
				}

				return buckets;
			}

			const std::vector<int>* bucket_at(const triangle_buckets& buckets, const float x, const float y)
			{
				const auto bx = static_cast<int>(std::floor((x - buckets.origin[0]) / buckets.bucket_size));
				const auto by = static_cast<int>(std::floor((y - buckets.origin[1]) / buckets.bucket_size));
				if (bx < 0 || by < 0 || bx >= buckets.width || by >= buckets.height)
				{
					return nullptr;
				}
				return &buckets.triangles[static_cast<std::size_t>(by) * buckets.width + bx];
			}

			bool floor_blocked(const std::vector<column_hit>& hits, const float z)
			{
				for (const auto& hit : hits)
				{
					if (hit.z > z + 1.0f && hit.z < z + 72.0f) // agent height
					{
						return true;
					}
					if (hit.normal_z < 0.0f && std::abs(hit.z - z) <= 0.1f)
					{
						return true;
					}
				}
				return false;
			}

			span_grid make_spans(const std::vector<triangle>& triangles, const triangle_buckets& buckets, const float cell_size)
			{
				span_grid grid{};
				grid.origin[0] = buckets.origin[0];
				grid.origin[1] = buckets.origin[1];
				grid.cell_size = cell_size;
				grid.width = static_cast<int>(buckets.width * buckets.bucket_size / cell_size) + 1;
				grid.height = static_cast<int>(buckets.height * buckets.bucket_size / cell_size) + 1;
				grid.cell_spans.resize(static_cast<std::size_t>(grid.width) * grid.height);

				std::vector<column_hit> hits;
				for (auto cy = 0; cy < grid.height; cy++)
				{
					for (auto cx = 0; cx < grid.width; cx++)
					{
						const auto x = grid.origin[0] + (cx + 0.5f) * cell_size;
						const auto y = grid.origin[1] + (cy + 0.5f) * cell_size;
						const auto* bucket = bucket_at(buckets, x, y);
						if (!bucket)
						{
							continue;
						}

						hits.clear();
						for (const auto index : *bucket)
						{
							float z;
							if (height_at(triangles[index], x, y, z))
							{
								hits.push_back({ z, normal_z(triangles[index]) });
							}
						}

						std::sort(hits.begin(), hits.end(), [](const column_hit& a, const column_hit& b)
						{
							return a.z < b.z;
						});

						auto last_z = -FLT_MAX;
						for (const auto& hit : hits)
						{
							if (hit.normal_z < 0.7f || hit.z - last_z < 1.0f || floor_blocked(hits, hit.z))
							{
								continue;
							}

							last_z = hit.z;
							grid.cell_spans[static_cast<std::size_t>(cy) * grid.width + cx].push_back(static_cast<int>(grid.spans.size()));
							grid.spans.push_back({ hit.z, { -1, -1, -1, -1 } });
						}
					}
				}

				return grid;
			}

			bool wall_between(const std::vector<triangle>& triangles, const triangle_buckets& buckets,
				const float* from, const float* to)
			{
				const std::vector<int>* searched[2] = { bucket_at(buckets, from[0], from[1]), bucket_at(buckets, to[0], to[1]) };
				for (auto b = 0; b < 2; b++)
				{
					if (!searched[b] || (b == 1 && searched[1] == searched[0]))
					{
						continue;
					}

					for (const auto index : *searched[b])
					{
						if (segment_hits(triangles[index], from, to))
						{
							return true;
						}
					}
				}
				return false;
			}

			void link_spans(span_grid& grid, const std::vector<triangle>& triangles, const triangle_buckets& buckets)
			{
				for (auto cy = 0; cy < grid.height; cy++)
				{
					for (auto cx = 0; cx < grid.width; cx++)
					{
						for (const auto span_index : grid.cell_spans[static_cast<std::size_t>(cy) * grid.width + cx])
						{
							for (auto dir = 0; dir < 2; dir++)
							{
								const auto nx = cx + direction_x[dir];
								const auto ny = cy + direction_y[dir];
								if (nx >= grid.width || ny >= grid.height)
								{
									continue;
								}

								auto& span = grid.spans[span_index];
								auto best = -1;
								auto best_dz = 18.0f; // agent step height
								for (const auto other : grid.cell_spans[static_cast<std::size_t>(ny) * grid.width + nx])
								{
									const auto dz = std::abs(grid.spans[other].z - span.z);
									if (dz <= best_dz && grid.spans[other].links[dir + 2] < 0)
									{
										best = other;
										best_dz = dz;
									}
								}

								if (best < 0)
								{
									continue;
								}

								const auto base_z = std::max(span.z, grid.spans[best].z);
								auto blocked = false;
								for (const auto clearance : { 20.0f, 40.0f, 64.0f })
								{
									const float from[3] = { grid.origin[0] + (cx + 0.5f) * grid.cell_size, grid.origin[1] + (cy + 0.5f) * grid.cell_size, base_z + clearance };
									const float to[3] = { grid.origin[0] + (nx + 0.5f) * grid.cell_size, grid.origin[1] + (ny + 0.5f) * grid.cell_size, base_z + clearance };
									if (wall_between(triangles, buckets, from, to))
									{
										blocked = true;
										break;
									}
								}

								if (!blocked)
								{
									span.links[dir] = best;
									grid.spans[best].links[dir + 2] = span_index;
								}
							}
						}
					}
				}
			}

			void unlink_removed(span_grid& grid)
			{
				for (auto& span : grid.spans)
				{
					for (auto& link : span.links)
					{
						if (link >= 0 && (span.removed || grid.spans[link].removed))
						{
							link = -1;
						}
					}
				}
			}

			bool segment_blocked(const std::vector<triangle>& triangles, const triangle_buckets& buckets, const float* from, const float* to)
			{
				const auto bx0 = std::max(static_cast<int>(std::floor((std::min(from[0], to[0]) - buckets.origin[0]) / buckets.bucket_size)), 0);
				const auto bx1 = std::min(static_cast<int>(std::floor((std::max(from[0], to[0]) - buckets.origin[0]) / buckets.bucket_size)), buckets.width - 1);
				const auto by0 = std::max(static_cast<int>(std::floor((std::min(from[1], to[1]) - buckets.origin[1]) / buckets.bucket_size)), 0);
				const auto by1 = std::min(static_cast<int>(std::floor((std::max(from[1], to[1]) - buckets.origin[1]) / buckets.bucket_size)), buckets.height - 1);
				for (auto by = by0; by <= by1; by++)
				{
					for (auto bx = bx0; bx <= bx1; bx++)
					{
						for (const auto index : buckets.triangles[static_cast<std::size_t>(by) * buckets.width + bx])
						{
							if (segment_hits(triangles[index], from, to))
							{
								return true;
							}
						}
					}
				}
				return false;
			}

			std::vector<int> span_components(const span_grid& grid)
			{
				std::vector<int> component(grid.spans.size(), -1);
				std::vector<int> open;
				auto count = 0;
				for (auto i = 0u; i < grid.spans.size(); i++)
				{
					if (grid.spans[i].removed || component[i] >= 0)
					{
						continue;
					}

					component[i] = count;
					open.push_back(i);
					while (!open.empty())
					{
						const auto index = open.back();
						open.pop_back();
						for (const auto link : grid.spans[index].links)
						{
							if (link >= 0 && component[link] < 0)
							{
								component[link] = count;
								open.push_back(link);
							}
						}
					}
					count++;
				}
				return component;
			}

			struct traversal_landing
			{
				int span = -1;
				int step = 0;
			};

			traversal_landing find_landing(const span_grid& grid, const std::vector<int>& component, const int start, const int cx, const int cy, const int dir)
			{
				const auto start_z = grid.spans[start].z;
				for (auto step = 1; step <= 10; step++)
				{
					const auto x = cx + direction_x[dir] * step;
					const auto y = cy + direction_y[dir] * step;
					if (x < 0 || y < 0 || x >= grid.width || y >= grid.height)
					{
						return {};
					}

					for (const auto index : grid.cell_spans[static_cast<std::size_t>(y) * grid.width + x])
					{
						const auto& span = grid.spans[index];
						if (!span.removed && std::abs(span.z - start_z) <= 18.0f) // agent step height
						{
							if (component[index] == component[start])
							{
								return {};
							}

							return { index, step };
						}
					}
				}
				return {};
			}

			std::vector<traversal> find_traversals(const span_grid& grid, const std::vector<triangle>& triangles, const triangle_buckets& buckets)
			{
				const auto component = span_components(grid);
				std::vector<traversal> traversals;
				std::vector<std::pair<int, int>> traversal_components;
				for (auto cy = 0; cy < grid.height; cy++)
				{
					for (auto cx = 0; cx < grid.width; cx++)
					{
						for (const auto start : grid.cell_spans[static_cast<std::size_t>(cy) * grid.width + cx])
						{
							const auto& span = grid.spans[start];
							for (auto dir = 0; dir < 4; dir++)
							{
								if (span.removed || span.links[dir] >= 0)
								{
									continue;
								}

								const auto landing = find_landing(grid, component, start, cx, cy, dir);
								if (landing.span < 0)
								{
									continue;
								}

								traversal result{};
								result.start[0] = grid.origin[0] + (cx + 0.5f) * grid.cell_size;
								result.start[1] = grid.origin[1] + (cy + 0.5f) * grid.cell_size;
								result.start[2] = span.z;
								result.end[0] = result.start[0] + direction_x[dir] * landing.step * grid.cell_size;
								result.end[1] = result.start[1] + direction_y[dir] * landing.step * grid.cell_size;
								result.end[2] = grid.spans[landing.span].z;

								const auto blocked_at = [&](const float height)
								{
									const float from[3] = { result.start[0], result.start[1], span.z + height };
									const float to[3] = { result.end[0], result.end[1], span.z + height };
									return segment_blocked(triangles, buckets, from, to);
								};
								if (!blocked_at(20.0f) || blocked_at(44.0f) || blocked_at(60.0f)) // jump_up_40 obstacle
								{
									continue;
								}

								const std::pair<int, int> components{ component[start], component[landing.span] };
								auto near_existing = false;
								for (auto i = 0u; i < traversals.size() && !near_existing; i++)
								{
									const auto dx = traversals[i].start[0] - result.start[0];
									const auto dy = traversals[i].start[1] - result.start[1];
									near_existing = traversal_components[i] == components && dx * dx + dy * dy < 128.0f * 128.0f;
								}
								if (!near_existing)
								{
									traversals.push_back(result);
									traversal_components.push_back(components);
								}
							}
						}
					}
				}
				return traversals;
			}

			void erode_spans(span_grid& grid, const float clearance)
			{
				std::vector<std::array<int, 2>> cells(grid.spans.size());
				for (auto cy = 0; cy < grid.height; cy++)
				{
					for (auto cx = 0; cx < grid.width; cx++)
					{
						for (const auto index : grid.cell_spans[static_cast<std::size_t>(cy) * grid.width + cx])
						{
							cells[index] = { cx, cy };
						}
					}
				}

				const auto center = [&](const int index, const int axis)
				{
					return grid.origin[axis] + (cells[index][axis] + 0.5f) * grid.cell_size;
				};

				using queue_entry = std::pair<float, int>;
				std::priority_queue<queue_entry, std::vector<queue_entry>, std::greater<>> open;
				std::vector<float> distance(grid.spans.size(), FLT_MAX);
				std::vector<std::array<float, 2>> nearest_wall(grid.spans.size());
				const auto offer = [&](const int index, const float x, const float y)
				{
					const auto d = std::hypot(center(index, 0) - x, center(index, 1) - y);
					if (d < distance[index])
					{
						distance[index] = d;
						nearest_wall[index] = { x, y };
						open.push({ d, index });
					}
				};

				const auto half = grid.cell_size * 0.5f;
				for (auto i = 0; i < static_cast<int>(grid.spans.size()); i++)
				{
					for (auto dir = 0; dir < 4; dir++)
					{
						if (grid.spans[i].links[dir] >= 0)
						{
							continue;
						}

						const auto edge_x = center(i, 0) + direction_x[dir] * half;
						const auto edge_y = center(i, 1) + direction_y[dir] * half;
						for (const auto side : { -1.0f, 0.0f, 1.0f })
						{
							offer(i, edge_x + direction_y[dir] * half * side, edge_y + direction_x[dir] * half * side);
						}
					}
				}

				while (!open.empty())
				{
					const auto [d, index] = open.top();
					open.pop();
					if (d > distance[index])
					{
						continue;
					}

					for (const auto link : grid.spans[index].links)
					{
						if (link >= 0)
						{
							offer(link, nearest_wall[index][0], nearest_wall[index][1]);
						}
					}
				}

				for (auto i = 0u; i < grid.spans.size(); i++)
				{
					grid.spans[i].removed = distance[i] < clearance;
				}
				unlink_removed(grid);
			}

			void remove_small_islands(span_grid& grid, const std::size_t min_spans)
			{
				std::vector<bool> visited(grid.spans.size(), false);
				std::vector<int> island;
				for (auto i = 0u; i < grid.spans.size(); i++)
				{
					if (visited[i] || grid.spans[i].removed)
					{
						continue;
					}

					island.clear();
					island.push_back(i);
					visited[i] = true;
					for (auto next = 0u; next < island.size(); next++)
					{
						for (const auto link : grid.spans[island[next]].links)
						{
							if (link >= 0 && !visited[link])
							{
								visited[link] = true;
								island.push_back(link);
							}
						}
					}

					if (island.size() < min_spans)
					{
						for (const auto index : island)
						{
							grid.spans[index].removed = true;
						}
					}
				}
				unlink_removed(grid);
			}

			int find_seed_span(const span_grid& grid, const std::array<float, 3>& seed)
			{
				const auto seed_x = static_cast<int>(std::floor((seed[0] - grid.origin[0]) / grid.cell_size));
				const auto seed_y = static_cast<int>(std::floor((seed[1] - grid.origin[1]) / grid.cell_size));
				auto best = -1;
				auto best_distance = FLT_MAX;
				for (auto cy = seed_y - 4; cy <= seed_y + 4; cy++)
				{
					for (auto cx = seed_x - 4; cx <= seed_x + 4; cx++)
					{
						if (cx < 0 || cy < 0 || cx >= grid.width || cy >= grid.height)
						{
							continue;
						}

						for (const auto index : grid.cell_spans[static_cast<std::size_t>(cy) * grid.width + cx])
						{
							const auto& span = grid.spans[index];
							const auto dz = seed[2] - span.z;
							if (span.removed || dz < -18.0f || dz > 72.0f)
							{
								continue;
							}

							const auto dx = static_cast<float>(cx - seed_x);
							const auto dy = static_cast<float>(cy - seed_y);
							const auto distance = (dx * dx + dy * dy) * grid.cell_size * grid.cell_size + dz * dz;
							if (distance < best_distance)
							{
								best = index;
								best_distance = distance;
							}
						}
					}
				}
				return best;
			}

			std::size_t keep_seeded_spans(span_grid& grid, const std::vector<std::array<float, 3>>& seeds)
			{
				std::vector<bool> reached(grid.spans.size(), false);
				std::vector<int> open;
				std::size_t found = 0;
				for (const auto& seed : seeds)
				{
					const auto start = find_seed_span(grid, seed);
					if (start < 0)
					{
						continue;
					}

					found++;
					if (!reached[start])
					{
						reached[start] = true;
						open.push_back(start);
					}
				}

				while (!open.empty())
				{
					const auto index = open.back();
					open.pop_back();
					for (const auto link : grid.spans[index].links)
					{
						if (link >= 0 && !reached[link])
						{
							reached[link] = true;
							open.push_back(link);
						}
					}
				}

				for (auto i = 0u; i < grid.spans.size(); i++)
				{
					if (!reached[i])
					{
						grid.spans[i].removed = true;
					}
				}
				unlink_removed(grid);
				return found;
			}

			bool fits_height_range(const float z, float& min_z, float& max_z, const float max_range)
			{
				const auto new_min = std::min(min_z, z);
				const auto new_max = std::max(max_z, z);
				if (new_max - new_min > max_range)
				{
					return false;
				}
				min_z = new_min;
				max_z = new_max;
				return true;
			}

			std::vector<area_rect> merge_rects(span_grid& grid, const int max_side, const float max_height_range)
			{
				std::vector<area_rect> rects;
				for (auto cy = 0; cy < grid.height; cy++)
				{
					for (auto cx = 0; cx < grid.width; cx++)
					{
						for (const auto start : grid.cell_spans[static_cast<std::size_t>(cy) * grid.width + cx])
						{
							if (grid.spans[start].removed || grid.spans[start].area >= 0)
							{
								continue;
							}

							auto min_z = grid.spans[start].z;
							auto max_z = min_z;

							std::vector<int> row = { start };
							while (static_cast<int>(row.size()) < max_side)
							{
								const auto next = grid.spans[row.back()].links[0];
								if (next < 0 || grid.spans[next].area >= 0
									|| !fits_height_range(grid.spans[next].z, min_z, max_z, max_height_range))
								{
									break;
								}
								row.push_back(next);
							}

							area_rect rect{};
							rect.rows.push_back(row);
							while (static_cast<int>(rect.rows.size()) < max_side)
							{
								auto row_min_z = min_z;
								auto row_max_z = max_z;
								std::vector<int> up;
								for (const auto below : rect.rows.back())
								{
									const auto next = grid.spans[below].links[1];
									if (next < 0 || grid.spans[next].area >= 0
										|| (!up.empty() && grid.spans[up.back()].links[0] != next)
										|| !fits_height_range(grid.spans[next].z, row_min_z, row_max_z, max_height_range))
									{
										up.clear();
										break;
									}
									up.push_back(next);
								}

								if (up.empty())
								{
									break;
								}

								min_z = row_min_z;
								max_z = row_max_z;
								rect.rows.push_back(std::move(up));
							}

							rect.x0 = cx;
							rect.y0 = cy;
							rect.x1 = cx + static_cast<int>(row.size());
							rect.y1 = cy + static_cast<int>(rect.rows.size());

							const auto area_index = static_cast<int>(rects.size());
							for (const auto& rect_row : rect.rows)
							{
								for (const auto span_index : rect_row)
								{
									grid.spans[span_index].area = area_index;
								}
							}
							rects.push_back(std::move(rect));
						}
					}
				}
				return rects;
			}

			float vertex_z(const span_grid& grid, const int span_index, const int cx, const int cy, const int vx, const int vy)
			{
				std::vector<std::tuple<int, int, int>> block = { { span_index, cx, cy } };
				auto sum = 0.0f;
				for (auto next = 0u; next < block.size(); next++)
				{
					const auto [index, x, y] = block[next];
					sum += grid.spans[index].z;
					for (auto dir = 0; dir < 4; dir++)
					{
						const auto link = grid.spans[index].links[dir];
						const auto nx = x + direction_x[dir];
						const auto ny = y + direction_y[dir];
						if (link < 0 || nx < vx - 1 || nx > vx || ny < vy - 1 || ny > vy)
						{
							continue;
						}

						if (std::none_of(block.begin(), block.end(), [&](const auto& entry) { return std::get<0>(entry) == link; }))
						{
							block.emplace_back(link, nx, ny);
						}
					}
				}
				return sum / static_cast<float>(block.size());
			}

			area_polygon make_polygon(const span_grid& grid, const area_rect& rect)
			{
				area_polygon polygon{};
				const auto width = rect.x1 - rect.x0;
				const auto height = rect.y1 - rect.y0;

				const auto add_segment = [&](const int span_index, const int cx, const int cy, const int vx, const int vy,
					const int dir, const bool side_start)
				{
					const auto link = grid.spans[span_index].links[dir];
					const auto neighbor = link >= 0 ? grid.spans[link].area : -1;
					if (!side_start && polygon.edges.back().neighbor == neighbor)
					{
						return;
					}

					area_edge edge{};
					edge.grid_vertex[0] = vx;
					edge.grid_vertex[1] = vy;
					edge.vertex[0] = grid.origin[0] + vx * grid.cell_size;
					edge.vertex[1] = grid.origin[1] + vy * grid.cell_size;
					edge.vertex[2] = vertex_z(grid, span_index, cx, cy, vx, vy);
					edge.neighbor = neighbor;
					polygon.edges.push_back(edge);
				};

				for (auto i = 0; i < width; i++)
				{
					add_segment(rect.rows[0][i], rect.x0 + i, rect.y0, rect.x0 + i, rect.y0, 3, i == 0);
				}
				for (auto j = 0; j < height; j++)
				{
					add_segment(rect.rows[j][width - 1], rect.x1 - 1, rect.y0 + j, rect.x1, rect.y0 + j, 0, j == 0);
				}
				for (auto i = width - 1; i >= 0; i--)
				{
					add_segment(rect.rows[height - 1][i], rect.x0 + i, rect.y1 - 1, rect.x0 + i + 1, rect.y1, 1, i == width - 1);
				}
				for (auto j = height - 1; j >= 0; j--)
				{
					add_segment(rect.rows[j][0], rect.x0, rect.y0 + j, rect.x0, rect.y0 + j + 1, 2, j == height - 1);
				}

				auto z_sum = 0.0f;
				for (auto axis = 0; axis < 3; axis++)
				{
					polygon.mins[axis] = FLT_MAX;
					polygon.maxs[axis] = -FLT_MAX;
				}
				for (const auto& edge : polygon.edges)
				{
					z_sum += edge.vertex[2];
					for (auto axis = 0; axis < 3; axis++)
					{
						polygon.mins[axis] = std::min(polygon.mins[axis], edge.vertex[axis]);
						polygon.maxs[axis] = std::max(polygon.maxs[axis], edge.vertex[axis]);
					}
				}

				polygon.center[0] = grid.origin[0] + (rect.x0 + rect.x1) * 0.5f * grid.cell_size;
				polygon.center[1] = grid.origin[1] + (rect.y0 + rect.y1) * 0.5f * grid.cell_size;
				polygon.center[2] = z_sum / static_cast<float>(polygon.edges.size());

				polygon.radius = 0.0f;
				for (const auto& edge : polygon.edges)
				{
					const auto dx = edge.vertex[0] - polygon.center[0];
					const auto dy = edge.vertex[1] - polygon.center[1];
					const auto dz = edge.vertex[2] - polygon.center[2];
					polygon.radius = std::max(polygon.radius, std::sqrt(dx * dx + dy * dy + dz * dz));
				}
				return polygon;
			}

			void assign_portals(std::vector<area_polygon>& polygons)
			{
				std::map<std::tuple<int, int, std::int64_t, std::int64_t>, std::uint32_t> portals;
				for (auto area = 0u; area < polygons.size(); area++)
				{
					auto& edges = polygons[area].edges;
					for (auto e = 0u; e < edges.size(); e++)
					{
						auto& edge = edges[e];
						if (edge.neighbor < 0)
						{
							edge.portal = 0;
							continue;
						}

						const auto& end = edges[(e + 1) % edges.size()];
						const auto start_key = (static_cast<std::int64_t>(edge.grid_vertex[0]) << 32) | static_cast<std::uint32_t>(edge.grid_vertex[1]);
						const auto end_key = (static_cast<std::int64_t>(end.grid_vertex[0]) << 32) | static_cast<std::uint32_t>(end.grid_vertex[1]);
						const auto key = std::make_tuple(std::min(static_cast<int>(area), edge.neighbor), std::max(static_cast<int>(area), edge.neighbor),
							std::min(start_key, end_key), std::max(start_key, end_key));

						const auto existing = portals.find(key);
						if (existing != portals.end())
						{
							edge.portal = existing->second;
							continue;
						}

						edge.portal = static_cast<std::uint32_t>(portals.size()) + 1;
						portals.emplace(key, edge.portal);
					}
				}
			}

			template <typename T>
			void append(std::vector<std::uint8_t>& out, const T& value)
			{
				const auto* bytes = reinterpret_cast<const std::uint8_t*>(&value);
				out.insert(out.end(), bytes, bytes + sizeof(T));
			}

			void write_kd_node(const std::vector<area_polygon>& polygons, const std::vector<std::uint32_t>& offsets,
				std::vector<int>& ids, const std::size_t begin, const std::size_t end, std::vector<std::uint8_t>& out)
			{
				if (end - begin == 1)
				{
					append(out, static_cast<std::uint32_t>(BFX_KD_TREE_LEAF | offsets[ids[begin]]));
					return;
				}

				float center_mins[3] = { FLT_MAX, FLT_MAX, FLT_MAX };
				float center_maxs[3] = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
				for (auto i = begin; i < end; i++)
				{
					for (auto axis = 0; axis < 3; axis++)
					{
						center_mins[axis] = std::min(center_mins[axis], polygons[ids[i]].center[axis]);
						center_maxs[axis] = std::max(center_maxs[axis], polygons[ids[i]].center[axis]);
					}
				}

				auto axis = 0;
				for (auto a = 1; a < 3; a++)
				{
					if (center_maxs[a] - center_mins[a] > center_maxs[axis] - center_mins[axis])
					{
						axis = a;
					}
				}

				const auto middle = begin + (end - begin) / 2;
				std::nth_element(ids.begin() + begin, ids.begin() + middle, ids.begin() + end, [&](const int a, const int b)
				{
					return polygons[a].center[axis] < polygons[b].center[axis];
				});

				bfx_kd_tree_split split{};
				split.left_max = -FLT_MAX;
				split.right_min = FLT_MAX;
				for (auto i = begin; i < middle; i++)
				{
					split.left_max = std::max(split.left_max, polygons[ids[i]].maxs[axis]);
				}
				for (auto i = middle; i < end; i++)
				{
					split.right_min = std::min(split.right_min, polygons[ids[i]].mins[axis]);
				}
				split.left_max += 1.0f / 128.0f;
				split.right_min -= 1.0f / 128.0f;

				const auto node = out.size();
				out.resize(node + sizeof(bfx_kd_tree_split));
				write_kd_node(polygons, offsets, ids, begin, middle, out);
				split.info = (static_cast<std::uint32_t>(axis) << 28) | static_cast<std::uint32_t>(out.size() - node);
				write_kd_node(polygons, offsets, ids, middle, end, out);
				std::memcpy(out.data() + node, &split, sizeof(split));
			}

			std::vector<std::uint8_t> write_resource(const std::vector<area_polygon>& polygons)
			{
				bfx_box bounds{};
				for (auto axis = 0; axis < 3; axis++)
				{
					bounds.mins[axis] = FLT_MAX;
					bounds.maxs[axis] = -FLT_MAX;
				}

				std::vector<std::uint32_t> offsets;
				std::uint32_t offset = sizeof(bfx_layer_header);
				for (const auto& polygon : polygons)
				{
					offsets.push_back(offset);
					offset += static_cast<std::uint32_t>(sizeof(bfx_area) + sizeof(bfx_area_edge) * polygon.edges.size());
					for (auto axis = 0; axis < 3; axis++)
					{
						bounds.mins[axis] = std::min(bounds.mins[axis], polygon.mins[axis]);
						bounds.maxs[axis] = std::max(bounds.maxs[axis], polygon.maxs[axis]);
					}
				}
				const auto area_bytes = offset - static_cast<std::uint32_t>(sizeof(bfx_layer_header));

				std::vector<int> ids(polygons.size());
				for (auto i = 0u; i < ids.size(); i++)
				{
					ids[i] = static_cast<int>(i);
				}
				std::vector<std::uint8_t> nodes;
				write_kd_node(polygons, offsets, ids, 0, ids.size(), nodes);

				bfx_kd_tree_header tree{};
				tree.bounds = bounds;
				for (auto axis = 0; axis < 3; axis++)
				{
					tree.bounds.mins[axis] -= 1.0f / 128.0f;
					tree.bounds.maxs[axis] += 1.0f / 128.0f;
				}
				tree.node_bytes = static_cast<std::uint32_t>(nodes.size());

				bfx_layer_header layer{};
				layer.version = 0x2B;
				layer.index = 0;
				layer.area_bytes = area_bytes;
				layer.kd_tree_bytes = static_cast<std::uint32_t>(sizeof(bfx_kd_tree_header) + nodes.size());
				layer.link_stride = 0x38;
				layer.layer_bytes = static_cast<std::uint32_t>(sizeof(bfx_layer_header)) + layer.area_bytes + layer.kd_tree_bytes;
				layer.agent_params[0] = 72.0f;
				layer.agent_params[1] = 5.0f;
				layer.agent_params[2] = 15.0f;
				layer.agent_params[3] = 18.0f;
				layer.agent_params[4] = 72.0f;
				layer.bounds = bounds;
				layer.unknown_50 = 2;

				bfx_graph_header graph{};
				graph.version = 0x2B;
				graph.layer_count = 1;

				bfx_section_header section{};
				section.type = BFX_SECTION_GRAPH_2D;
				section.size = static_cast<std::uint32_t>(sizeof(bfx_graph_header)) + layer.layer_bytes;
				section.count = 1;

				bfx_resource_header header{};
				header.version = 2;
				header.body_size = static_cast<std::uint32_t>(sizeof(bfx_section_header)) + section.size;
				header.flags = 0x01000001;

				std::vector<std::uint8_t> out;
				out.reserve(sizeof(bfx_resource_header) + header.body_size);
				append(out, header);
				append(out, section);
				append(out, graph);
				append(out, layer);

				for (auto i = 0u; i < polygons.size(); i++)
				{
					const auto& polygon = polygons[i];
					bfx_area area{};
					std::memcpy(area.center, polygon.center, sizeof(area.center));
					area.radius = polygon.radius;
					area.unknown_30 = 0xFFFFFFFF;
					area.edge_info = 0x1FFFF80 | static_cast<std::uint32_t>(polygon.edges.size());
					area.unknown_3c = 0x2110000;
					area.unknown_40 = 0x80000000;
					append(out, area);

					for (const auto& source : polygon.edges)
					{
						bfx_area_edge edge{};
						edge.neighbor = source.neighbor >= 0 ? offsets[source.neighbor] : 0;
						std::memcpy(edge.vertex, source.vertex, sizeof(edge.vertex));
						edge.flags = 0xFFFF0000;
						edge.portal = source.portal;
						append(out, edge);
					}
				}

				append(out, tree);
				out.insert(out.end(), nodes.begin(), nodes.end());

				std::uint32_t checksum = 0;
				for (auto i = sizeof(bfx_resource_header); i + 4 <= out.size(); i += 4)
				{
					std::uint32_t value;
					std::memcpy(&value, out.data() + i, sizeof(value));
					checksum += value;
				}
				reinterpret_cast<bfx_resource_header*>(out.data())->checksum = checksum;
				return out;
			}
		}

		navmesh_result build_graph(const std::vector<triangle>& solid_triangles, const std::vector<std::array<float, 3>>& seeds)
		{
			if (solid_triangles.empty())
			{
				return {};
			}

			const auto buckets = make_buckets(solid_triangles, 64.0f);
			auto grid = make_spans(solid_triangles, buckets, 8.0f);
			link_spans(grid, solid_triangles, buckets);
			erode_spans(grid, 19.0f); // agent radius + half cell
			if (seeds.empty())
			{
				remove_small_islands(grid, 16);
			}
			else if (keep_seeded_spans(grid, seeds) == 0)
			{
				ZONETOOL_WARNING("navpower: no seed is on a floor span");
				return {};
			}

			const auto rects = merge_rects(grid, 16, 10.0f);
			if (rects.empty())
			{
				return {};
			}

			std::vector<area_polygon> polygons;
			polygons.reserve(rects.size());
			for (const auto& rect : rects)
			{
				polygons.push_back(make_polygon(grid, rect));
			}
			assign_portals(polygons);

			navmesh_result result{};
			result.traversals = find_traversals(grid, solid_triangles, buckets);
			result.graph = write_resource(polygons);
			ZONETOOL_INFO("navpower: %zu floor spans, %zu areas, %zu traversals", grid.spans.size(), polygons.size(), result.traversals.size());
			return result;
		}
	}
}
