#include <std_include.hpp>
#include "../include.hpp"

#include "clipmap_collision.hpp"
#include "xmodel.hpp"

// Only for builder::convex_rejection, so the brush -> convex fallback decision here uses
// exactly the limits the builder enforces rather than a copy that could drift.
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

// Pulls IW5 world collision out of clipMap_t and flattens it to triangles for the IW7
// Havok mesh builder.
//
// IW5 keeps collision in two unrelated forms and both have to end up in the one IW7
// compressed mesh:
//
//   * trisoup  -- verts / triIndices, already triangles, material per CollisionAabbTree
//                 leaf. Straight passthrough.
//   * brushes  -- cbrush_t, a set of half-spaces (6 axial planes implied by brushBounds
//                 plus `numsides` non-axial cbrushside_t). These have to be turned into
//                 explicit convex hulls. The world blob (extract_world) keeps each hull
//                 whole as a convex custom primitive, which is the only thing IW7's player
//                 movement cast collides with; `extract` and the fallback path
//                 triangulate the hull's faces instead.
//
// IW7 Havok coordinates are CoD units / 32 for world, entity and model collision. Both
// engines are Z-up right-handed.
//
// Winding: CoD brush planes point *outward*, and Havok expects counter-clockwise winding
// seen from the front. face_to_triangles below emits CCW about the outward normal.

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

				// What a CoD4 solid brush becomes IF the experimental remap below is switched
				// on. It is OFF by default and source contents are preserved -- see
				// solid_as_clip_enabled(). This block records why the remap exists, not what
				// the converter does.
				//
				// The observation behind it was that on mp_test_h1 the palm-trunk and table
				// clip brushes (0x31640) blocked the player while the numbered 0x1 walls did
				// not. That diagnosis is very likely superseded: at the time the writer put 0
				// in ShapeTagData::userData for every surface, so nothing carried the brush
				// basis at bit 48 and IW7's player cast took the non-brush path through all of
				// it. Once the basis bit was populated the premise changed, and it has not
				// been retested. Stock is unambiguous that 0x1 is the normal solid mask: it is
				// the most common collisionFilterInfo in every shipped world blob (58/132 tags
				// in mp_afghan, 89/170 in cp_rave, 99/475 in cp_zmb).
				//
				// Stock does not mix the two: every shipped tag is 0x1 on its own, or clip
				// bits with no 0x1 (0x31640, 0x30200, 0x336C0, 0x2080).
				//
				// 0x31640 is the mask this map's own palm-trunk and table clip brushes carry,
				// and those are the only world surfaces that have ever blocked the player here:
				// player, monster, vehicle and item clip, AI-nosight and can-shoot clip.
				//
				// 0x336C0 (also shipped, 4 tags in mp_afghan) is a strict superset of it, adding
				// clip-shot 0x2000 and missile-clip 0x80 -- and walls tagged with it are walked
				// through. So on a per-surface tag those two bits appear to mark a surface
				// bullets-only, overriding the player bits; stock's shot-only props (0x3180)
				// behave the same way.
				// ON by default, and the in-game evidence beats the stock survey here. Two zones
				// differing only in this value, same session, same everything else:
				//
				//   solid -> 0x00031640   floor is solid
				//   solid -> 0x00000001   player falls through the floor
				//
				// and in mp_test_h1 the only world surfaces that have EVER collided -- the palm
				// trunks and plants -- are the ones whose CoD4 material already carried the clip
				// mask. So on a converted map the clip bits are what the player collides with.
				//
				// This is NOT what stock looks like, and the discrepancy is unexplained. Measured
				// over the six shipped world blobs, weighted by referencing primitives,
				// 0x00000001 is 68.0% of all world primitives and 71.4% of walkable ones, and a
				// working stock floor traces as
				//   contents 0x00000001  surfFlags 0x00280000  hitType 1  id 2046  WALKABLE
				// Both masks share a bit with the player's trace mask 0x00810011 (0x1 and
				// 0x10000 respectively) and the trace filter is a plain AND, so the filter does
				// not explain the difference. Something else about a converted body or shape
				// makes 0x1 unusable; until that is found, match what works in game.
				//
				// The cost is real and known: isCollisionEnabledSimulation examines only bits
				// 0x1, 0x10, 0x100 and 0x800000, none of which 0x31640 has, so these surfaces
				// are invisible to grenades, props and ragdolls -- see
				// iw7-collision-filter-two-predicates. That is the trade being made.
				//
				// Do not use this experimental remap by default.  Stock world meshes use
				// CONTENTS_SOLID for ordinary solid geometry, and preserving the source contents
				// keeps the world and brush-model paths faithful to their source.  The clip-mask
				// variant remains available for targeted diagnostics.
				//
				// ZT_HAVOK_SOLID_AS_CLIP=1 enables the remap on both the world and brush-model
				// paths (ClipMap.cpp reads the same variable); they must agree.
				constexpr auto CONTENTS_SOLID_AS_CLIP_DEFAULT = 0x00031640;

				bool solid_as_clip_enabled()
				{
					const auto* env = std::getenv("ZT_HAVOK_SOLID_AS_CLIP");
					return env && env[0] == '1';
				}

				// What CONTENTS_SOLID becomes. Every other contents value in a converted world
				// is copied from the source brush; this one is invented, so it is the one worth
				// sweeping -- and in mp_test_h1 it is also the value on all 52 brushes the
				// player walks through, while the four brushes at 0x00030200, the one at
				// 0x00010000, the six at 0x00000800 and the mantle brush at 0x01000000 all stop
				// them. The player coming to rest at z=-47.6 is standing on brush 58
				// (1792 x 1792 x 16 at z=-56, contents 0x00000800, top face z=-48) having
				// passed straight through brush 31, the real floor (1664 x 1664 x 16 at z=-8,
				// top face z=0, contents 0x00031640).
				//
				// ZT_HAVOK_SOLID_CONTENTS=0x30200 overrides it without a rebuild. Accepts hex
				// with or without 0x, or decimal.
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

				// Compile-time classification bits. The compiler uses them to split detail
				// from structural brushes and they mean nothing at runtime; stock IW7 shape
				// tags never carry them (no stock tag has a bit above 0x01000000, which is
				// CONTENTS_MANTLE -- the bit Quake used for origin brushes, reused by CoD).
				constexpr auto CONTENTS_COMPILE_ONLY = 0x08000000 /* DETAIL */
					| 0x10000000 /* STRUCTURAL */ | 0x20000000 /* TRANSPARENT */;

				// ShapeTagData::userData bits 32..39 are a per-glass-piece index, which is a
				// third region beside the surface flags in the low 32 and the brush basis at
				// bit 48. It is set on glass and nothing else: of the 842 stock tags carrying a
				// non-zero byte there, 832 are GLASS_PANE, 8 are GLASS_PANE with a different
				// contents mask, 2 more are GLASS_PANE and 6 are GLASS_SOLID. Values run 1..236
				// in cp_zmb, so it is 1-based and byte-wide, and 0 means "not a piece".
				//
				// IW7's glass system indexes panes by it, so leaving every pane at 0 makes them
				// indistinguishable to the shatter path -- the same subsystem this project
				// already detours around at 0x140B157B0.
				constexpr auto USERDATA_GLASS_PIECE_SHIFT = 32;
				constexpr auto USERDATA_GLASS_PIECE_MAX = 0xFFu;

				// The IW7 surface type is a 6-bit field at bits 19..24 (the enum steps by
				// 0x00080000), so mask before comparing.
				constexpr auto SURF_TYPE_MASK = 0x01F80000u;

				bool is_glass_surface(const std::uint64_t iw7_surface_flags)
				{
					const auto type = static_cast<unsigned int>(iw7_surface_flags) & SURF_TYPE_MASK;
					return type == 0x00480000u  /* SURFACE_FLAG_GLASS_PANE */
						|| type == 0x01380000u; /* SURFACE_FLAG_GLASS_SOLID */
				}

				// Trigger brush models are volumes an entity watches for touch events, not
				// geometry, and IW7 keeps them exactly where IW5 does: MapEnts::trigger, as
				// slab hulls referenced from the entity string by "model" "?N". They are
				// never part of the world collision blob -- no stock IW7 world shape contains
				// one -- so dropping them here is correct, not a stopgap. The conversion
				// happens in generate_mapents(). See docs/iw7-triggers.md.
				constexpr auto CONTENTS_TRIGGER = 0x40000000;

				constexpr auto PLANE_EPSILON = 0.01f;
				constexpr auto HULL_EXTENT = 131072.0f; // matches the engine broadphase

				struct plane
				{
					float normal[3];
					float dist;
				};

				struct winding
				{
					std::vector<std::array<float, 3>> points;
				};

				// Start from a huge quad lying on `p`, then clip it by every other plane of
				// the brush. What survives is that plane's face of the convex hull.
				winding base_winding_for_plane(const plane& p)
				{
					// Pick the axis the normal is least aligned with for a stable basis.
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

					// right = up x normal
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

				// Keep the half-space behind `p` (CoD brush planes face outward).
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

				// Fan a convex face into primitives, CCW about the outward normal. Two
				// consecutive fan triangles -- (p0,pi,pi+1) and (p0,pi+1,pi+2) -- are exactly
				// Havok's quad: one 4-index primitive fanned about its first corner. Emitting
				// them that way matches stock, where 90,924 of mp_afghan's 105,752 world
				// primitives are quads, and halves the primitive count.
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

						// OFF by default: emitting faces as quads matches stock's shape (96% of
						// its primitives are quads) but made collision visibly worse in game,
						// so it stays behind ZT_HAVOK_QUADS=1 until that is understood.
						if (quads_enabled() && i + 2 < w.points.size())
						{
							tri.is_quad = true;
							std::memcpy(tri.vert3, w.points[i + 2].data(), sizeof(float[3]));
							i++;
						}

						// Ensure the geometric normal agrees with the plane normal.
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
							// Reverse the corner order rather than swapping two of them: a quad
							// has to stay a ring, so (v0,v1,v2,v3) becomes (v0,v3,v2,v1).
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

				// Collect every brush reachable from one cmodel's leafbrush tree. cmodels[0] is
				// the world; cmodels[1..] are brush models -- doors, movers, triggers -- which
				// IW7 positions from their entity, so baking them into the static world blob
				// leaves a solid copy frozen at its compile-time position.
				//
				// Iterative with a visited set on purpose: a childOffset of 0 points a node at
				// itself, and a recursive walk spins forever on it.
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

				// Same walk, collecting indices for one cmodel rather than marking a shared
				// bitmap. Brush models need their brushes kept apart per model.
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

				// Is the leafbrush walk trustworthy enough to attribute brushes to brush
				// models? Both halves of the port depend on the SAME answer: `extract` drops the
				// brush-model brushes from the static world mesh, and `extract_brush_models`
				// emits them as entity shapes instead. If the two disagree the brushes are
				// either in the world twice -- once baked at the compile-time position and once
				// on the entity -- or in neither, so the decision is made here once.
				//
				// cmodels[0]'s own leafbrush tree usually reaches nothing: the world's brushes
				// hang off the BSP leaves, not off the world cmodel, which is why the H1
				// physics-world generator walks every cmodel -- the world included -- to find
				// brush-model brushes and treats the rest as world. So a world count of 0 is
				// normal and cannot gate the filter; mp_test_h1 reads 0 here, and refusing to
				// filter baked its four brush models into the world at their compile-time
				// position, the origin. What still guards against a mis-read tree is the brush
				// models staying a minority and not overlapping anything the world walk reached.
				//
				// The per-cmodel dump below is worth its cost: it is what shows the attribution
				// is RIGHT. In mp_test_h1 cmodel 0 (the world) has bounds 1814 x 1797 x 762 and
				// reaches nothing, while cmodel 1 has bounds 6146 x 6146 x 514 -- wrapping brush
				// 61, the 6144 x 6144 x 512 volume at the origin, to within the two units of
				// slack the compiler adds, with leafBounds matching it exactly. So that brush
				// really is a brush model's and belongs out of the static world, and a world
				// shape spanning +-906 units is the correct size for this map rather than
				// evidence of missing geometry.
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

				// CollisionAabbTree is an n-ary tree, not a binary one: an internal node has
				// `childCount` children laid out contiguously from u.firstChildIndex, and a leaf
				// (childCount == 0) names one partition in u.partitionIndex. Marks every
				// partition reachable from `root`.
				//
				// Iterative with a visited set for the same reason the leafbrush walk is: a
				// firstChildIndex that points a node at itself spins a recursive walk forever.
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

				// Two coincident half-spaces each clip to the same winding, so the polytope
				// comes out with two identical faces sharing every edge. The half-edge twin
				// lookup in the Havok builder is keyed on the directed vertex pair, so the
				// second copy overwrites the first's entry and both faces end up pointing at
				// the wrong twin. A repeated plane also carries no geometry -- the volume is
				// the same with or without it -- so drop it here rather than let it through.
				//
				// A brush side coincident with one of the bounds planes is the common case,
				// and it is why the axial planes go in first: their axialMaterialNum is what
				// IW5 authored for that face, so the first tag is the one to keep.
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
					// Six axial planes implied by the brush bounds. axialMaterialNum is
					// [side][axis] with side 0 = -axis, side 1 = +axis.
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

					// Non-axial sides.
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

				// ------------------------------------------------- convex polytopes

				// Vertices are shared between the faces of a hull, so the same corner has to
				// resolve to one index however many faces touch it. Clipping produces corners
				// that differ in the last bits, so match on a tolerance rather than equality.
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

				// Turn one brush's half-spaces into an explicit polytope: clip a huge quad on
				// each plane by all the others, and keep what survives as that plane's face.
				// This is the same construction `extract` uses, stopping before triangulation.
				//
				// Returns false if the brush is unusable -- degenerate, or past the uint8
				// index limits the Havok format imposes.
				bool build_convex_hull(const std::vector<plane>& planes, convex_hull& out)
				{
					// hknpConvexPolytopeShape indexes vertices with uint8, and
					// hknpConvexPolytopeShapeFace::numIndices is a uint8 too.
					//
					// The real ceiling is 252, not 255: hknpConvexShape::vertices is padded up to
					// a multiple of four, and the Havok builder rejects a padded count above 255
					// -- by failing the WHOLE ents blob, not just this hull, because dropping one
					// shape would shift every later shape index. 253..255 real vertices pad to
					// 256, so accepting them here turns one oversized brush into no brush-model
					// collision at all anywhere in the map. Stop at the largest multiple of four.
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
							continue; // plane contributes no face -- redundant half-space
						}

						convex_face face{};
						std::memcpy(face.plane, planes[i].normal, sizeof(float[3]));
						face.plane[3] = planes[i].dist;

						// Welding is destructive, and this face may still be rejected below.
						// Remember where the shared vertex list ended so a rejected face can
						// give back the corners only it introduced: a vertex no face indexes
						// is never assigned a half-edge, and Havok's support walk starts from
						// vertexEdges[v], so an orphan leaves the shape with an entry that
						// points at some unrelated face. Rejection happens before any later
						// face has run, so truncating here cannot cut a live vertex.
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

						// Welding can collapse adjacent corners onto the same index, which
						// would leave a zero-length edge in the face ring -- the half-edge
						// connectivity Havok builds from it then refers to an edge that does
						// not exist. Drop consecutive duplicates, cyclically, so the ring
						// stays a simple polygon.
						face.indices.erase(
							std::unique(face.indices.begin(), face.indices.end()),
							face.indices.end());
						while (face.indices.size() > 1
							&& face.indices.front() == face.indices.back())
						{
							face.indices.pop_back();
						}

						// A face that welded down to fewer than three distinct corners is a
						// sliver, not a face. Any duplicate still left is a pinched polygon
						// rather than a simple ring, and is not safe to emit either -- stock
						// has none, across every face of all 343 shipped convex shapes.
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

						// base_winding_for_plane builds its quad on a left-handed basis about
						// the plane normal, so the polygon comes out clockwise seen from
						// outside. Havok wants counter-clockwise about the outward normal.
						// Measure it rather than assuming, since clipping can reorder.
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

					// Fewer than four faces cannot bound a volume.
					return out.faces.size() >= 4 && out.verts.size() >= 4;
				}

				// An ents shape carries ONE ShapeTagData for the whole shape, not one per face,
				// so a brush model built from several brushes has to pick a representative
				// material. Take the one the most planes carry; ties go to the lowest index so
				// the choice is deterministic across runs. A world convex has the same
				// constraint -- one tag per custom primitive -- and uses the same rule.
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


			// ------------------------------------------------------------ debug output

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

				// Asset names are paths ("maps/mp/mp_test.d3dbsp"); flatten them so the
				// result is one file in the requested directory rather than a tree.
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

				// A quad writes four corners and a triangle three, so each primitive's first
				// OBJ vertex index has to be tracked rather than computed from its position.
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

				// Convex vertices go in the same vertex block, after the triangles', so every
				// face below references a vertex already defined.
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

				// The extents are the cheapest check there is: they have to match the
				// map's own size in whatever space the rest of the zone was written in.
				// A collision shell that is a clean multiple too big or too small is a
				// scale bug, not a geometry bug, and it shows up here before anything
				// has to be loaded.
				if (!triangles.empty() || !convexes.empty())
				{
					CLIPMAP_COLLISION_INFO("clipmap collision: world obj extents (%.1f %.1f %.1f) .. "
						"(%.1f %.1f %.1f), size %.1f x %.1f x %.1f",
						mn[0], mn[1], mn[2], mx[0], mx[1], mx[2],
						mx[0] - mn[0], mx[1] - mn[1], mx[2] - mn[2]);
				}

				// One object per (source, contents mask). Trisoup and brushes fail in
				// different ways, and a single bad contents class -- clip volumes turning
				// solid, say -- shows up as one object to hide rather than a haystack.
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

				// One object per contents mask for the convexes too, named apart from the
				// triangle groups so the two representations can be toggled independently.
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

			// -------------------------------------------------- surface materials
			//
			// `ShapeTagData::materialCRC` is what IW7 uses to pick footstep sounds, impact
			// effects and penetration behaviour. It is a CRC32 of a material name, but the
			// name space could not be recovered -- the values match no CRC32 variant of any
			// plausible surface-type name -- so the table below identifies each CRC by what
			// it is *used on* instead.
			//
			// That was read off the 2,224 shipped IW7 `PhysicsAsset`s, which are named after
			// their models, so the material each CRC represents is unambiguous:
			//
			//   0xCBF7A6C4  618 models  ladders, handrails, racks, bollards, guardrails  -> metal
			//   0x4B02BC9D  531 models  generator pipes, hoses, wires, thin panels       -> thin metal
			//   0x1AB7BC33  411 models  concrete walls, barriers, towers                 -> concrete
			//   0xE8F3FA9A  227 models  AC units, lockers, gutters, vents                -> painted metal
			//   0x0AD71E4E  130 models  crates, plywood, wooden chairs, paintings        -> wood
			//   0xA1F93A3B   54 models  books, cardboard, magazines, paper cups          -> paper
			//   0xCD123193   44 models  caps, t-shirts, curtains, tarps, sheets, bags    -> cloth
			//   0xF728E572   41 models  bottles, pint glass, broken glass, glasschunk    -> glass
			//   0x0103BCE1   37 models  rocks, cliffs, stone arches, rubble              -> rock
			//   0x63A1FDAD   22 models  bushes, trees, cactus, hedges, haybale           -> foliage
			//   0xF49C81BF   21 models  mugs, plates, bowls, vases, billiard ball        -> ceramic
			//   0xFFD772CD   14 models  balloons, tyres, hoses, yoga mat, bouncy balls   -> rubber
			//   0x98C096F9   11 models  sofas, pillows, insulation, teddy bear           -> cushion
			//   0x4724ADF2   10 models  zombie limbs and heads, ragdolls                 -> flesh
			//   0x4FE888BA    6 models  bananas, watermelon, cucumbers, lemons           -> fruit
			//   0x46FAE51C    6 models  asphalt rubble chunks                            -> asphalt
			//   0x96309552    5 models  sandbags                                         -> sand
			//   0xD8B39111    5 models  cobblestone rubble, broken brick                 -> brick
			//   0xAE2DE6F5    1 model   destruction_mud_pile_01                          -> mud
			//   0x00C9A2F0    1 model   zmb_alien_village_wishing_pool_01_water          -> water
			//
			// Entries marked "no direct evidence" fall back to the concrete/default CRC --
			// the one the shipped dummies themselves use -- because no shipped asset pins
			// them down. They are a guess at the *closest* material, not a reading.
			//
			// The guesses were revisited by enrichment analysis over all 2,335 shipped
			// PhysicsAssets: for a material, how much more often does a CRC appear on assets
			// whose names name that material than across the corpus? The method validates on
			// materials already known -- wood-named assets are 8.1x enriched for the wood
			// CRC, glass-named 10.7x for glass.
			//
			//   PLASTIC  -> RUBBER is CONFIRMED, not a guess: plastic-named assets
			//               (bucket_plastic, p7_bottle_plastic_*) are 21.6x enriched for the
			//               rubber CRC -- the strongest enrichment measured anywhere,
			//               stronger than either control. IW7 has no separate plastic
			//               material; it uses the rubber one.
			//
			//   SNOW, PLASTER, SLUSH  -- the corpus contains NO asset named for any of these.
			//               Not "weak evidence": no evidence. These cannot be settled from
			//               shipped IW7 data and will stay guesses until another source
			//               appears.
			//
			//   ICE      -- 30 name matches but the signal is noise (best is glass at 3.2x on
			//               2 assets) and the group is contaminated by substring matches like
			//               ice_cream_cooler. No conclusion.
			//
			//   RIOT_SHIELD -- the four "shield" assets are zombie-mode crafting props
			//               (p7_zm_ctl_shield_armory_*), not riot shields, so they are not
			//               evidence about this surface type at all and were discounted.
			// OFF by default. The physics material decides whether a surface stops the player,
			// not just what it sounds like: in game on mp_test_h1 the floor (mud, 0xAE2DE6F5)
			// blocks while walls tagged foliage (0x63A1FDAD) and asphalt-rubble (0x46FAE51C)
			// are walked straight through, with identical contents, body and geometry. Both of
			// those materials belong to decorative props -- bushes, hedges, rubble chunks --
			// and IW7 lets the player through them by design.
			//
			// ZT_HAVOK_ONE_MATERIAL=1 tags every world surface concrete (the CRC 411 shipped
			// wall, barrier and tower models use, and the one the stock dummies carry) to
			// confirm that reading. It costs per-surface footstep and impact variety, so it is
			// a diagnostic rather than the fix: the fix is to stop mapping solid world brushes
			// onto pass-through materials.
			bool one_material_enabled()
			{
				const auto* env = std::getenv("ZT_HAVOK_ONE_MATERIAL");
				return env && env[0] == '1';
			}

			unsigned int iw7_material_crc(const int surface_flags)
			{
				constexpr auto METAL = 0xCBF7A6C4u;
				constexpr auto PAINTED_METAL = 0xE8F3FA9Au;
				constexpr auto CONCRETE = 0x1AB7BC33u; // also the default
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

				// How these were identified, and why it supersedes the earlier reading:
				// ShapeTagData carries the IW7 surface type and the material CRC in the SAME
				// record, so cross-tabulating one against the other over the shipped world and
				// ents blobs names a material directly. The earlier pass inferred CRCs from
				// PhysicsAsset filenames instead, which is a much weaker channel and got
				// several of these wrong. Counts below are tags over the six stock maps
				// (mp_frontend, mp_afghan, mp_paris, mp_breakneck, cp_zmb, cp_rave).

				// Indexed by IW5 materialSurfType_t, which is surfaceFlags >> 20.
				static const unsigned int BY_SURFACE_TYPE[SURF_TYPE_COUNT] = {
					/* DEFAULT       */ CONCRETE,
					/* BARK          */ WOOD,
					/* BRICK         */ BRICK,
					/* CARPET        */ CARPET,   // 0x04E51705: 10/10 tags CARPET_SOLID, exclusive
					/* CLOTH         */ CLOTH,
					/* CONCRETE      */ CONCRETE,
					/* DIRT          */ MUD,
					/* FLESH         */ FLESH,
					/* FOLIAGE       */ FOLIAGE,
					/* GLASS         */ GLASS,
					/* GRASS         */ FOLIAGE,  // no grass material observed
					/* GRAVEL        */ ROCK,     // no gravel material observed
					/* ICE           */ ICE,      // 0x820231A4: 2/2 tags ICE_SOLID, exclusive but thin
					/* METAL         */ METAL,
					/* MUD           */ MUD,
					/* PAPER         */ PAPER,
					/* PLASTER       */ PLASTER,  // 0x8D07D363: 16/16 tags PLASTER, exclusive
					/* ROCK          */ ROCK,
					/* SAND          */ SAND,
					/* SNOW          */ CONCRETE, // stock SNOW tags carry only the default CRC
					/* WATER         */ CONCRETE, // stock WATER/_KNEE/_WAIST carry only the default;
					                              // 0x00C9A2F0, the old reading, is a PLASTER CRC
					/* WOOD          */ WOOD,
					/* ASPHALT       */ ROCK,     // stock ASPHALT_DRY uses ROCK (12) or the default (8);
					                              // 0x46FAE51C, the old reading, only ever tags DEFAULT
					/* CERAMIC       */ CERAMIC,
					/* PLASTIC       */ PLASTIC,  // 0x4B02BC9D: 50 PLASTIC tags, its dominant use.
					                              // RUBBER's CRC never tags a PLASTIC surface, so the
					                              // old "no separate plastic material" reading was wrong
					/* RUBBER        */ RUBBER,   // 0xFFD772CD: 30/30 tags RUBBER, exclusive
					/* CUSHION       */ CUSHION,
					/* FRUIT         */ FRUIT,
					/* PAINTED_METAL */ PAINTED_METAL,
					/* RIOT_SHIELD   */ RUBBER,   // no signal: 0 RIOTSHIELD tags in any stock blob
					/* SLUSH         */ MUD,      // no signal: stock SLUSH carries only the default
				};

				if (one_material_enabled())
				{
					return CONCRETE;
				}

				// The surface type is a 5-bit FIELD at bits 20..24 (SURF_FLAG_DEFAULT ..
				// SURF_FLAG_SLUSH step 0x00100000, so 0..30), not everything above bit 20:
				// SURF_FLAG_MANTLEON (0x02000000), SURF_FLAG_MANTLEOVER (0x04000000) and
				// SURF_FLAG_PORTAL (0x80000000) sit ABOVE it and are separate flags. An
				// unmasked >> 20 folds them into the type -- a concrete wall marked mantle
				// reads as type 0x25 -- so every mantle, mantle-over and portal surface in the
				// map silently lost its material and fell back to the default here.
				const auto type = (static_cast<unsigned int>(surface_flags) >> 20) & 0x1Fu;
				if (type >= SURF_TYPE_COUNT)
				{
					return CONCRETE;
				}

				return BY_SURFACE_TYPE[type];
			}

			// A collision-semantics change made during the P9 work is OFF by default: it was
			// reasoned from shipped data but never validated in game.
			//
			//   ZT_HAVOK_TRISOUP_MATERIAL=1 take trisoup contents from its ClipMaterial
			//                               instead of forcing CONTENTS_SOLID
			//
			// Its sibling, keeping clip volumes in the world mesh, is now always on: stock
			// world blobs carry the same clip masks (0x00031640, 0x00030200) as tags, and
			// without them nothing that CoD4 clipped -- palm trunks, table legs, clip walls
			// -- collides at all.
			bool trisoup_material_contents_enabled()
			{
				const auto* env = std::getenv("ZT_HAVOK_TRISOUP_MATERIAL");
				return env && env[0] == '1';
			}

			// The one implementation behind `extract` and `extract_world`. With
			// `brush_convexes` false every brush is triangulated into faces, which is
			// `extract`'s contract; with it true each brush becomes one convex, falling back
			// to faces only where the convex cannot be built or would be rejected.
			static world_collision extract_collision(clipMap_t* clipmap,
				const float scale_override, const bool brush_convexes)
			{
				world_collision result;
				auto& triangles = result.triangles;

				if (!clipmap)
				{
					return result;
				}

				// Static model collision is a separate asset in IW7 (a HavokPhysicsAsset per
				// XModel), not part of the world blob. If most of this map's surfaces are
				// models, the world blob legitimately only covers the brush shell.
				CLIPMAP_COLLISION_INFO("clipmap collision: source has %u verts, %d tris, %d partitions, "
					"%u brushes, %u static models, %u submodels",
					clipmap->vertCount, clipmap->triCount, clipmap->partitionCount,
					clipmap->info.numBrushes, clipmap->numStaticModels, clipmap->numSubModels);

				// ---------------------------------------------------------- trisoup
				//
				// triIndices index `verts`, but they are SEGMENT-RELATIVE, not absolute: the
				// vertex a triangle corner names is
				//
				//     verts[triIndices[3 * tri + c] + 1024 * partition->firstVertSegment]
				//
				// which is what the engine does and what the H1 physics-world generator in
				// src/H1/Utils/PhysWorld/generate.cpp does. The segment base is recorded ONLY on
				// the partition, so the soup has to be walked per partition rather than
				// 0..triCount -- there is no way to recover the base from a bare triangle index.
				//
				// The partition is also where the other two things a triangle needs come from:
				// its material (via the CollisionAabbTree leaf that owns the partition) and
				// whether it belongs to a brush model rather than to the world.
				std::vector<unsigned short> partition_material(
					std::max(clipmap->partitionCount, 0), 0);

				// Partitions owned by cmodels[1..]. A brush model is positioned from its entity,
				// so baking its trisoup into the static world blob leaves a solid copy frozen at
				// the compile-time position -- exactly what the brush-side filter below guards
				// against, which until now was not applied to trisoup at all.
				std::vector<bool> model_partitions(std::max(clipmap->partitionCount, 0), false);

				for (auto i = 0; i < clipmap->aabbTreeCount; i++)
				{
					const auto* tree = &clipmap->aabbTrees[i];
					if (tree->childCount != 0)
					{
						continue; // internal node
					}

					const auto partition_index = tree->u.partitionIndex;
					if (partition_index < 0 || partition_index >= clipmap->partitionCount)
					{
						continue;
					}

					partition_material[partition_index] = tree->materialIndex;
				}

				// cLeaf_t::firstCollAabbIndex / collAabbCount is the run of AABB tree roots a
				// cmodel owns. cmodels[0] -- the world -- normally has a count of 0 (world
				// trisoup hangs off the BSP leaves instead), so walking 1.. is both necessary
				// and sufficient.
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

				// Resolve each ClipMaterial once: surfaceFlags -> IW7 material CRC, and the
				// material's own contents mask.
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

					// A material with no contents would produce a surface that collides with
					// nothing, so keep solid as the floor.
					const auto contents = clipmap->info.materials[i].contents;
					material_contents[i] = contents ? contents : CONTENTS_SOLID;
				}
				// The surface-type field and the converted IW7 flags, per material. Prints the
				// old unmasked index beside the masked one so a map where they differ is obvious.
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

				// ShapeTagData::userData: the IW7 surface flags a trace hit reports, in the low
				// 32 bits, and bit 48 -- the brush basis -- on surfaces that came from a brush.
				// IW7's player cast sweeps a separate non-brush shape against any leaf without
				// the bit; stock marks every brush-derived surface with it and leaves terrain
				// and props clear. See the note where havok_builder writes the tags.
				// ZT_HAVOK_BRUSH_BASIS=0 clears it. The rule stock follows is structural --
				// brush-derived surfaces set it, terrain and mesh props clear it -- so do not
				// use a percentage as a sanity check: measured over the six shipped maps it is
				// 43-79% of world TAGS but only 6-25% of world PRIMITIVES, and an all-brush
				// map legitimately lands near 100% of both.
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

						// The one thing that makes this loop per-partition rather than global.
						const auto vert_base =
							1024u * static_cast<unsigned int>(partition->firstVertSegment);

						const auto material = partition_material[p];
						const auto crc = crc_for(material);
						const auto flags = surface_flags_for(material);

						// Trisoup has no per-triangle contents. Its ClipMaterial does, and using
						// it is more principled -- sky and water trisoup should not collide as
						// world geometry -- but it strips CONTENTS_SOLID from a large share of
						// surfaces and is not validated in game, so it is opt-in.
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

				// Every trisoup triangle should belong to exactly one partition. If the
				// partitions do not account for all of them the rest cannot be converted at
				// all -- their vertex segment is unknown -- so say so rather than lose them
				// quietly.
				if (trisoup_covered != clipmap->triCount)
				{
					ZONETOOL_WARNING("clipmap collision: partitions cover %d of %d trisoup "
						"triangles; the remainder have no known vertex segment and are dropped",
						trisoup_covered, clipmap->triCount);
				}

				const auto trisoup_count = triangles.size();
				result.trisoup_count = trisoup_count;

				// Stock world blobs are CoD units / 32, the same space as ents shapes and
				// per-model physics assets. Measured directly on mp_frontend's shipped
				// colmap.hkx: its mesh-tree domain is 212 x 772 x 84 against a map 6720 x 23040
				// CoD units across, and /32 predicts 210 x 772 - x and y match to within 1%.
				// At 1:1 that domain would read in the thousands.
				//
				// docs/iw7-havok-collision.md used to claim the world blob was 1:1, from an
				// order-of-magnitude comparison of a static-model origin cloud against the
				// collision domain that gave ratios ranging 0.9 to 4.1. That is nowhere near
				// tight enough to tell 1 from 32, and it disagreed with the two exact
				// measurements either side of it. Confirmed in game too: 1:1 is visibly worse.
				// Override with ZT_HAVOK_WORLD_SCALE.
				//
				// Do not be fooled by a ray test against the pre-scale geometry: it lines up
				// exactly with CoD-unit world positions, which shows the source geometry is
				// correct - not that the target space is 1:1.

				// This is the same 1/32 as ents_input::scale and XModel.cpp's model_scale,
				// and it has to stay the same: the world mesh, the brush-model hulls and a
				// model's own collision all have to end up in one space or they slide past
				// each other. If you change one, change all three.
				//
				// Decided before the brushes rather than after, because whether a brush can be
				// a convex depends on its flatness in Havok units, i.e. after this scale.
				auto world_scale = 0.03125f;
				if (scale_override > 0.0f)
				{
					// A caller that wants the geometry in CoD units - the hull fitter in the
					// GfxWorld converter does - passes 1 and skips the env override entirely.
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

				// ----------------------------------------------------------- brushes
				auto brushes_with_sides = 0;
				auto brushes_as_convex = 0;
				auto fallback_hull_failed = 0;
				std::map<std::string, int> fallback_rejected;
				auto total_nonaxial = 0;
				auto faces_emitted = 0;
				auto faces_clipped_away = 0;
				// Running 1-based glass piece id; see USERDATA_GLASS_PIECE_SHIFT.
				auto glass_pieces = 0u;
				auto skipped_nonsolid = 0;
				auto skipped_trigger = 0;
				auto skipped_model = 0;
				auto skipped_degenerate = 0;
				auto kept_clip = 0;

				// brushContents drives everything about how IW5 treats a brush: only some
				// of these are player-solid, the rest are clip/sky/trigger volumes that are
				// invisible in game. Converting them all makes IW7 collision far larger than
				// the visible surfaces, so log what is actually present before filtering.
				std::vector<std::pair<int, int>> contents_hist;

				// Only brushes we can positively attribute to a cmodel other than the world
				// get dropped, so a walk that under-reaches costs nothing rather than deleting
				// real geometry.
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

					// A solid brush keeps CONTENTS_SOLID, which is what 68% of stock world
					// primitives carry and what a working stock floor traces as; volumes that
					// were already clip keep exactly what they had.
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
						// Carried by MapEnts::trigger instead, see the note above.
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
						// Only drop what positively should not collide. Requiring
						// CONTENTS_SOLID here throws away every clip volume, and stock IW7 world
						// collision is full of them: 31 of mp_afghan's 132 surface tags and 28
						// of mp_paris's 316 carry PLAYERCLIP/MONSTERCLIP, and in paris only 84
						// tags are solid-only at all. Dropping those is how a converted map
						// ends up letting players walk through clip walls and off ledges -- and
						// through mp_test_h1's palms, whose trunks collide only through clip
						// brushes. Mantle and sky stay too; stock ships a tag for each. The
						// contents mask rides along into ShapeTagData::collisionFilterInfo,
						// so the runtime still filters a playerclip surface to players and not
						// to bullets.
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

					// One piece index per glass brush, allocated lazily so non-glass brushes
					// do not consume the 255 the byte can hold.
					std::uint64_t glass_piece = 0;

					if (brush_convexes)
					{
						// The brush's hull, built by the same half-space clipping as the faces
						// below. Its vertex set is welded at WELD_EPSILON = 0.05 CoD units, so
						// the convex's points are distinct to that tolerance.
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

							// Same limits the builder enforces, judged on the scaled points.
							if (const auto* reason =
								zonetool::iw7::havok::builder::convex_rejection(cvx.verts))
							{
								fallback_rejected[reason]++;
							}
							else
							{
								// One tag per custom primitive: the ClipMaterial most of the
								// brush's planes carry, ties to the lowest index.
								std::vector<unsigned short> tag_votes(
									clipmap->info.materials ? clipmap->info.numMaterials : 0u, 0);
								for (const auto t : tags)
								{
									if (t < tag_votes.size() && tag_votes[t] < 0xFFFF)
									{
										tag_votes[t]++;
									}
								}

								// With no plane naming a real material, keep the first plane's
								// tag -- what the face path would have given the first face.
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

								// Face indices address cvx.verts one-for-one with hull.verts.
								cvx.faces = std::move(hull.faces);

								result.convexes.emplace_back(std::move(cvx));
								brushes_as_convex++;
								continue;
							}
						}
						// Fall through: this brush's faces go in as triangles, exactly as they
						// would with ZT_HAVOK_BRUSH_CONVEX=0.
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

				// If every brush contributes exactly its six axial faces, the hulls are
				// bounding boxes rather than real brush shapes -- which shows up in game as
				// collision noticeably larger than the visible surfaces on any brush that is
				// not a box (ramps, angled walls, wedges).
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

				// Clip volumes are invisible but collidable, so a converted map's collision
				// legitimately covers more than its visible surfaces. Report how much of what
				// was kept is clip, since that is the number that used to be zero.
				if (kept_clip)
				{
					CLIPMAP_COLLISION_INFO("clipmap collision: %d of the kept brushes are clip "
						"volumes (player/monster clip)", kept_clip);
				}

				// The trigger brushes are not lost -- they leave through MapEnts::trigger.
				// Say so, so a large skipped_trigger count does not read as data loss.
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

				// world_scale was chosen before the brush loop; convexes are already scaled.
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

						// A quad's fourth corner is a vertex like any other. Missing it here
						// left it at CoD scale, 32x out, so every quad decoded as a sliver
						// reaching across the map -- 319 of 366 came out bent, and the sky box
						// reported a 21-degree normal in game.
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
				// The same two lookups `extract` does through its cached tables, as free
				// functions so the brush-model and trigger paths can tag their shapes with the
				// real material instead of a constant. Both fall back to the values a stock
				// blob uses for an untyped surface.
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

				// If the world mesh could not attribute these brushes to their cmodels it kept
				// them, so emitting them here as well would put every brush model in the map
				// twice: once baked into the static world shape at its compile-time position,
				// and once on the entity. One copy is recoverable, two are not.
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

				// cmodels[0] is the world; only 1.. are brush models an entity can reference
				// as "model" "*N".
				for (auto i = 1u; i < clipmap->numSubModels; i++)
				{
					const auto brushes = leafbrushes_of(clipmap, clipmap->cmodels[i].leaf.leafBrushNode);
					if (brushes.empty())
					{
						continue;
					}

					brush_model model{};
					model.index = i;

					// Track the hull union so the result can be rebased onto the cmodel's own
					// bounds below.
					float union_min[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
					float union_max[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

					// How many planes each ClipMaterial covers, to pick the shape's one tag.
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

						// The same contents filter the world mesh applies, for the same reasons:
						// the compile-only classification bits mean nothing at runtime and IW7's
						// collision filter rejects CONTENTS_DETAIL outright, a trigger volume is
						// carried by MapEnts::trigger rather than as geometry (turning one into a
						// convex hull here makes an invisible solid block), and a non-colliding
						// brush should not collide. CONTENTS_TRIGGER (0x40000000) is not even one
						// of the bits the Havok writer masks off, so it reached shapeContents.
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

					// Tag the shape with the material its planes mostly carry. Without this
					// every brush model shipped as untyped concrete, which also threw away the
					// LADDER / SLICK / NOPENETRATE / STAIRS / MANTLEON bits that ride in the
					// low 32 of userData -- on exactly the geometry most likely to need them.
					if (!tag_votes.empty())
					{
						const auto tag = dominant_tag(tag_votes);
						// An all-zero tally means no plane named a material -- keep the
						// defaults rather than silently adopting material 0.
						if (tag_votes[tag] > 0)
						{
							model.material_crc = material_crc_for(clipmap, tag);
							model.surface_flags = surface_flags_for(clipmap, tag);
						}
					}

					// IW7 stores brush-model shapes in entity space -- every stock shape AABB
					// matches its cmodel_t::bounds (which sit at ~0, not out in the world) to
					// within the one unit CoD pads those bounds by. Rather than assume which
					// space IW5 keeps brush-model brushes in, measure the offset between the
					// hulls and the cmodel bounds we are copying across, and cancel it. If the
					// brushes are already model-local this is a no-op.
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

							// Plane distances move with the geometry: d' = d - dot(n, offset).
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
						// A brush geom is already in model space: its bounds are the brush's own
						// axial box and its sides carry the non-axial cuts. The geom orientation
						// does not apply to it.
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

					// A primitive (box, cylinder, capsule) is described in its own frame:
					// `halfSize` along the local axes, centred on `midPoint` in model space and
					// turned by `orientation`, whose rows are the local axes (CoD's usual axis
					// layout, as MatrixTransformVector):
					//
					//     model = local.x * row0 + local.y * row1 + local.z * row2 + midPoint
					//
					// The rotation is about the geom's own centre, not the model origin. CoD4
					// cylinders keep their axis on local X and stand up through `orientation`, so
					// rotating about the origin swings com_pail_metal1's centre from (0,0,7.5) to
					// (-7.5,0,0) -- half into the table and off to one side. Rows rather than
					// columns is settled by com_pan_metal's handle, a box tilted about X: as rows
					// it rises out of the rim, as columns it starts above the pan and dips to
					// the floor.
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

					// Round geoms: the symmetry axis is the one half size that differs from the
					// other two, which are the radius. The IW3 port writes (halfHeight, radius,
					// radius). A geom that fits no such pattern is kept as its box.
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

						// 16 sides, as the H1 port and the retail cylinder builder use.
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

						// Flat caps. For a capsule these sit on the poles, and a ring of planes
						// tangent to the end spheres at 45 degrees takes the corner off, so it
						// rolls and slides like a capsule rather than catching like a drum.
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

					// The face planes have to move with the vertices, or the hull's planes
					// describe a different solid from its corners -- silent for the XModel path,
					// which only fans the indices into triangles, but wrong for anything that
					// writes hknpConvexPolytopeShape::planes. The distance is taken from a placed
					// corner so a non-orthonormal orientation still comes out consistent.
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

					// The six axial planes of the hull's own bounds.
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

					// Each slab is two parallel planes: the runtime test is
					// |dot(p, dir) - midPoint| < halfSize, so the volume is bounded at
					// midPoint + halfSize facing +dir and midPoint - halfSize facing -dir.
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
