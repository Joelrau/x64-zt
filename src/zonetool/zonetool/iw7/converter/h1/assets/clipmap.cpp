#include <std_include.hpp>
#include "zonetool/iw7/converter/h1/include.hpp"
#include "clipmap.hpp"
#include "physworld.hpp"

#include "zonetool/h1/assets/clipmap.hpp"
#include "zonetool/h1/assets/physworld.hpp"
#include "zonetool/h1/assets/fxworld.hpp"
#include "zonetool/h1/assets/glassworld.hpp"

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace clipmap
		{
			namespace
			{
				struct tri_t
				{
					float p[3][3];
					unsigned int mat;
					float c[3];
				};

				unsigned int surface_type_for(const std::string& n)
				{
					static const std::pair<const char*, unsigned int> table[] = {
						{"grass", 10}, {"dirt", 6}, {"sand", 19}, {"gravel", 11}, {"mud", 15}, {"snow", 20}, {"ice", 12},
						{"wood", 22}, {"metal", 13}, {"grate", 14}, {"glass", 9}, {"brick", 2}, {"rock", 18}, {"stone", 18},
						{"plaster", 17}, {"carpet", 3}, {"cloth", 4}, {"fabric", 4}, {"paper", 16}, {"asphalt", 23},
						{"ceramic", 24}, {"tile", 24}, {"plastic", 25}, {"rubber", 26}, {"bark", 1}, {"foliage", 8},
						{"concrete", 5}, {"cement", 5},
					};
					for (const auto& e : table)
					{
						if (n.find(e.first) != std::string::npos) return e.second;
					}
					return 5; // concrete
				}

				bool skip_material(const std::string& n)
				{
					static const char* skip[] = { "sky", "water", "decal", "glow", "shaft", "billboard", "nodraw", "fog", "particle", "volumetric" };
					for (const auto* s : skip)
					{
						if (n.find(s) != std::string::npos) return true;
					}
					return false;
				}

				struct builder
				{
					std::vector<tri_t> tris;
					std::vector<zonetool::h1::CollisionAabbTree> nodes;
					std::vector<zonetool::h1::CollisionPartition> partitions;
					std::vector<float> verts; // xyz
					std::vector<unsigned short> tri_indices;
					bool overflow = false;
					unsigned long long dropped = 0;
					unsigned long long dropped_small = 0;

					static void bounds_of(const std::vector<unsigned int>& ids, const std::vector<tri_t>& t, float mn[3], float mx[3])
					{
						mn[0] = mn[1] = mn[2] = FLT_MAX;
						mx[0] = mx[1] = mx[2] = -FLT_MAX;
						for (const auto id : ids)
						{
							for (int v = 0; v < 3; v++)
							{
								for (int k = 0; k < 3; k++)
								{
									mn[k] = std::min(mn[k], t[id].p[v][k]);
									mx[k] = std::max(mx[k], t[id].p[v][k]);
								}
							}
						}
					}

					void set_bounds(unsigned int node, const float mn[3], const float mx[3])
					{
						for (int k = 0; k < 3; k++)
						{
							nodes[node].midPoint[k] = (mn[k] + mx[k]) * 0.5f;
							nodes[node].halfSize[k] = (mx[k] - mn[k]) * 0.5f;
						}
					}

					void make_leaf(unsigned int node, const std::vector<unsigned int>& ids, unsigned int mat)
					{
						// weld verts within this partition
						std::unordered_map<unsigned long long, unsigned int> local;
						std::vector<unsigned int> local_verts; // indices into verts (global)
						const unsigned int base_count = static_cast<unsigned int>(verts.size() / 3);
						const unsigned int seg = base_count / 1024;
						const unsigned int seg_base = seg * 1024;

						if (seg > 255) { overflow = true; dropped += ids.size(); }

						nodes[node].childCount = 0;
						nodes[node].materialIndex = static_cast<unsigned short>(mat);

						if (overflow)
						{
							// empty partition so the tree stays valid
							zonetool::h1::CollisionPartition p{};
							p.triCount = 0; p.borderCount = 0; p.firstVertSegment = 255;
							p.firstTri = static_cast<int>(tri_indices.size() / 3);
							p.borders = nullptr;
							nodes[node].u.partitionIndex = static_cast<int>(partitions.size());
							partitions.push_back(p);
							return;
						}

						const auto first_tri = static_cast<int>(tri_indices.size() / 3);
						std::vector<unsigned short> idx;
						for (const auto id : ids)
						{
							for (int v = 0; v < 3; v++)
							{
								const auto* pt = tris[id].p[v];
								const auto qx = static_cast<long long>(std::llround(pt[0] * 64.0f));
								const auto qy = static_cast<long long>(std::llround(pt[1] * 64.0f));
								const auto qz = static_cast<long long>(std::llround(pt[2] * 64.0f));
								const unsigned long long key = (static_cast<unsigned long long>(qx & 0x1FFFFF) << 42) |
									(static_cast<unsigned long long>(qy & 0x1FFFFF) << 21) | static_cast<unsigned long long>(qz & 0x1FFFFF);
								auto it = local.find(key);
								unsigned int gi;
								if (it == local.end())
								{
									gi = base_count + static_cast<unsigned int>(local_verts.size());
									local_verts.push_back(gi);
									local.emplace(key, gi);
									verts.push_back(pt[0]); verts.push_back(pt[1]); verts.push_back(pt[2]);
								}
								else gi = it->second;
								idx.push_back(static_cast<unsigned short>(gi - seg_base));
							}
						}
						for (const auto i : idx) tri_indices.push_back(i);

						zonetool::h1::CollisionPartition p{};
						p.triCount = static_cast<unsigned char>(ids.size());
						p.borderCount = 0;
						p.firstVertSegment = static_cast<unsigned char>(seg);
						p.firstTri = first_tri;
						p.borders = nullptr;
						nodes[node].u.partitionIndex = static_cast<int>(partitions.size());
						partitions.push_back(p);
					}

					void build(unsigned int node, std::vector<unsigned int> ids, unsigned int mat, int depth)
					{
						float mn[3], mx[3];
						bounds_of(ids, tris, mn, mx);
						set_bounds(node, mn, mx);
						nodes[node].materialIndex = static_cast<unsigned short>(mat);

						if (ids.size() <= 255 || depth > 40)
						{
							if (ids.size() > 255)
							{
								// degenerate depth: chunk into children
								const auto n = (ids.size() + 254) / 255;
								const auto first = static_cast<unsigned int>(nodes.size());
								nodes.resize(nodes.size() + n);
								nodes[node].childCount = static_cast<unsigned short>(n);
								nodes[node].u.firstChildIndex = static_cast<int>(first);
								for (size_t c = 0; c < n; c++)
								{
									std::vector<unsigned int> sub(ids.begin() + c * 255, ids.begin() + std::min(ids.size(), (c + 1) * 255));
									float smn[3], smx[3];
									bounds_of(sub, tris, smn, smx);
									set_bounds(first + static_cast<unsigned int>(c), smn, smx);
									make_leaf(first + static_cast<unsigned int>(c), sub, mat);
								}
								return;
							}
							make_leaf(node, ids, mat);
							return;
						}

						int axis = 0;
						float ext[3] = { mx[0] - mn[0], mx[1] - mn[1], mx[2] - mn[2] };
						if (ext[1] > ext[axis]) axis = 1;
						if (ext[2] > ext[axis]) axis = 2;
						const auto mid = ids.begin() + ids.size() / 2;
						std::nth_element(ids.begin(), mid, ids.end(), [&](unsigned int a, unsigned int b) { return tris[a].c[axis] < tris[b].c[axis]; });
						std::vector<unsigned int> left(ids.begin(), mid), right(mid, ids.end());
						ids.clear(); ids.shrink_to_fit();

						const auto first = static_cast<unsigned int>(nodes.size());
						nodes.resize(nodes.size() + 2);
						nodes[node].childCount = 2;
						nodes[node].u.firstChildIndex = static_cast<int>(first);
						build(first, std::move(left), mat, depth + 1);
						build(first + 1, std::move(right), mat, depth + 1);
					}
				};
			}

			zonetool::h1::clipMap_t* generate_from_gfxworld(GfxWorld* asset, utils::memory::allocator& allocator)
			{
				ZONETOOL_INFO("generating collision from render geometry...");

				const auto h1_asset = allocator.allocate<zonetool::h1::clipMap_t>();
				h1_asset->name = asset->name;
				h1_asset->isInUse = 0;

				builder b;
				std::vector<std::string> mat_names;
				std::unordered_map<std::string, unsigned int> mat_lookup;

				const unsigned int zone_count = std::min<unsigned int>(asset->draw.transientZoneCount, 32);

				float wmn[3], wmx[3];
				static const float v115_thresholds[] = { 1e-6f, 4.0f, 16.0f, 64.0f, 256.0f, 1024.0f, 4096.0f };
				for (int v115_attempt = 0; ; v115_attempt++)
				{
				const float v115_thr = v115_thresholds[v115_attempt];
				b = builder{};
				mat_names.clear();
				mat_lookup.clear();
				for (unsigned int i = 0; i < asset->surfaceCount; i++)
				{
					const auto& s = asset->dpvs.surfaces[i];
					if (!s.material || !s.material->name) continue;
					const std::string mname = s.material->name;
					if (skip_material(mname)) continue;
					if (s.transientZone >= zone_count) continue;
					const auto* zone = asset->draw.transientZones[s.transientZone];
					if (!zone || !zone->vd.vertices) continue;

					auto it = mat_lookup.find(mname);
					unsigned int mi;
					if (it == mat_lookup.end())
					{
						mi = static_cast<unsigned int>(mat_names.size());
						mat_names.push_back(mname);
						mat_lookup.emplace(mname, mi);
					}
					else mi = it->second;

					for (unsigned int t = 0; t < s.tris.triCount; t++)
					{
						tri_t tri{};
						tri.mat = mi;
						bool ok = true;
						for (int v = 0; v < 3; v++)
						{
							const unsigned int ii = s.tris.baseIndex + t * 3 + v;
							if (ii >= asset->draw.indexCount) { ok = false; break; }
							const unsigned int vi = s.tris.firstVertex + asset->draw.indices[ii];
							if (vi >= zone->vertexCount) { ok = false; break; }
							const auto& gv = zone->vd.vertices[vi];
							tri.p[v][0] = gv.xyz[0]; tri.p[v][1] = gv.xyz[1]; tri.p[v][2] = gv.xyz[2];
						}
						if (!ok) continue;
						// drop degenerate triangles
						const float e1[3] = { tri.p[1][0] - tri.p[0][0], tri.p[1][1] - tri.p[0][1], tri.p[1][2] - tri.p[0][2] };
						const float e2[3] = { tri.p[2][0] - tri.p[0][0], tri.p[2][1] - tri.p[0][1], tri.p[2][2] - tri.p[0][2] };
						const float nx_ = e1[1] * e2[2] - e1[2] * e2[1], ny_ = e1[2] * e2[0] - e1[0] * e2[2], nz_ = e1[0] * e2[1] - e1[1] * e2[0];
						// v111: also drop tiny detail triangles (area < 2 units^2): mp_flip went 53 verts past the 256 vertex segments and the
						//       last material groups (spawn floors among them) lost collision -> players fell through the map
						if (nx_ * nx_ + ny_ * ny_ + nz_ * nz_ < v115_thr) { b.dropped_small++; continue; }
						for (int k = 0; k < 3; k++) tri.c[k] = (tri.p[0][k] + tri.p[1][k] + tri.p[2][k]) / 3.0f;
						b.tris.push_back(tri);
					}
				}

				ZONETOOL_INFO("collision triangles: %llu, materials: %llu (v111: %llu tiny tris dropped)", static_cast<unsigned long long>(b.tris.size()), static_cast<unsigned long long>(mat_names.size()), b.dropped_small);

				if (b.tris.empty() || mat_names.empty())
				{
					mat_names.push_back("default");
				}

				// group by material
				std::vector<std::vector<unsigned int>> groups(mat_names.size());
				for (unsigned int i = 0; i < b.tris.size(); i++) groups[b.tris[i].mat].push_back(i);

				std::vector<unsigned int> used;
				for (unsigned int m = 0; m < groups.size(); m++) if (!groups[m].empty()) used.push_back(m);

				b.nodes.resize(1 + used.size());
				b.nodes[0].childCount = static_cast<unsigned short>(used.size());
				b.nodes[0].u.firstChildIndex = 1;
				for (int k = 0; k < 3; k++) { wmn[k] = FLT_MAX; wmx[k] = -FLT_MAX; }
				for (size_t g = 0; g < used.size(); g++)
				{
					b.build(1 + static_cast<unsigned int>(g), std::move(groups[used[g]]), used[g], 0);
					for (int k = 0; k < 3; k++)
					{
						const auto& n = b.nodes[1 + g];
						wmn[k] = std::min(wmn[k], n.midPoint[k] - n.halfSize[k]);
						wmx[k] = std::max(wmx[k], n.midPoint[k] + n.halfSize[k]);
					}
				}
				if (used.size() > 2)
				{
					// engine traversal keeps child ranges on a 63-entry stack: keep fan-out at 2 (binary spine over material subtrees)
					unsigned int lvl_first = 1;
					unsigned int lvl_count = static_cast<unsigned int>(used.size());
					while (lvl_count > 2)
					{
						const unsigned int parents = (lvl_count + 1) / 2;
						const unsigned int new_first = static_cast<unsigned int>(b.nodes.size());
						b.nodes.resize(b.nodes.size() + parents);
						for (unsigned int pi = 0; pi < parents; pi++)
						{
							const unsigned int c0 = lvl_first + pi * 2;
							const unsigned int cn = std::min(2u, lvl_count - pi * 2);
							float mn[3] = { FLT_MAX, FLT_MAX, FLT_MAX }, mx[3] = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
							for (unsigned int ci = 0; ci < cn; ci++)
							{
								const auto& cnode = b.nodes[c0 + ci];
								for (int k = 0; k < 3; k++)
								{
									mn[k] = std::min(mn[k], cnode.midPoint[k] - cnode.halfSize[k]);
									mx[k] = std::max(mx[k], cnode.midPoint[k] + cnode.halfSize[k]);
								}
							}
							auto& pn = b.nodes[new_first + pi];
							pn = {};
							b.set_bounds(new_first + pi, mn, mx);
							pn.materialIndex = 0;
							pn.childCount = static_cast<unsigned short>(cn);
							pn.u.firstChildIndex = static_cast<int>(c0);
						}
						lvl_first = new_first;
						lvl_count = parents;
					}
					b.nodes[0].childCount = static_cast<unsigned short>(lvl_count);
					b.nodes[0].u.firstChildIndex = static_cast<int>(lvl_first);
				}
				if (used.empty())
				{
					for (int k = 0; k < 3; k++) { wmn[k] = -16.0f; wmx[k] = 16.0f; }
					b.nodes.resize(1);
					b.nodes[0].childCount = 0;
					zonetool::h1::CollisionPartition p{};
					p.firstVertSegment = 0;
					b.nodes[0].u.partitionIndex = 0;
					b.partitions.push_back(p);
					b.verts = { 0, 0, 0 };
					b.tri_indices = {};
				}
				// v115: only drop small detail when the vertex segments overflow (v111 always dropped < 2 units^2); retry with a growing threshold
				if (!b.overflow || v115_attempt + 1 >= static_cast<int>(std::size(v115_thresholds))) break;
				ZONETOOL_INFO("v115 collision: vertex segments overflowed (min tri area %.1f), retrying with a larger threshold", v115_thr * 0.5f);
				}
				for (int k = 0; k < 3; k++)
				{
					b.nodes[0].midPoint[k] = (wmn[k] + wmx[k]) * 0.5f;
					b.nodes[0].halfSize[k] = (wmx[k] - wmn[k]) * 0.5f;
				}

				ZONETOOL_INFO("collision: nodes %llu, partitions %llu, verts %llu, tris %llu%s",
					static_cast<unsigned long long>(b.nodes.size()), static_cast<unsigned long long>(b.partitions.size()),
					static_cast<unsigned long long>(b.verts.size() / 3), static_cast<unsigned long long>(b.tri_indices.size() / 3),
					b.overflow ? " (VERT SEGMENT OVERFLOW, geometry dropped)" : "");

				auto* info = &h1_asset->info;

				info->planeCount = 0;
				info->planes = nullptr;

				info->numMaterials = static_cast<unsigned int>(mat_names.size());
				info->materials = allocator.allocate_array<zonetool::h1::ClipMaterial>(info->numMaterials);
				for (unsigned int i = 0; i < info->numMaterials; i++)
				{
					info->materials[i].name = allocator.duplicate_string(mat_names[i]);
					info->materials[i].surfaceFlags = surface_type_for(mat_names[i]) << 20;
					info->materials[i].contents = 1;
				}

				// a single empty brush tree node
				info->bCollisionTree.leafbrushNodesCount = 1;
				info->bCollisionTree.leafbrushNodes = allocator.allocate_array<zonetool::h1::cLeafBrushNode_s>(1);
				info->bCollisionTree.numLeafBrushes = 0;
				info->bCollisionTree.leafbrushes = nullptr;

				info->pCollisionTree.aabbTreeCount = static_cast<int>(b.nodes.size());
				info->pCollisionTree.aabbTrees = allocator.allocate_array<zonetool::h1::CollisionAabbTree>(b.nodes.size());
				std::memcpy(info->pCollisionTree.aabbTrees, b.nodes.data(), b.nodes.size() * sizeof(zonetool::h1::CollisionAabbTree));

				info->sCollisionTree.numStaticModels = 0;
				info->sCollisionTree.smodelNodeCount = 1;
				info->sCollisionTree.smodelNodes = allocator.allocate_array<zonetool::h1::SModelAabbNode>(1);
				for (int k = 0; k < 3; k++)
				{
					info->sCollisionTree.smodelNodes[0].bounds.midPoint[k] = b.nodes[0].midPoint[k];
					info->sCollisionTree.smodelNodes[0].bounds.halfSize[k] = b.nodes[0].halfSize[k];
				}
				info->sCollisionTree.smodelNodes[0].firstChild = 0;
				info->sCollisionTree.smodelNodes[0].childCount = 0;

				info->bCollisionData.numBrushSides = 0;
				info->bCollisionData.brushSides = nullptr;
				info->bCollisionData.numBrushEdges = 0;
				info->bCollisionData.brushEdges = nullptr;
				info->bCollisionData.numBrushes = 0;
				info->bCollisionData.brushes = nullptr;
				info->bCollisionData.brushBounds = nullptr;
				info->bCollisionData.brushContents = nullptr;

				const auto vert_count = static_cast<unsigned int>(b.verts.size() / 3);
				info->pCollisionData.vertCount = vert_count;
				info->pCollisionData.verts = reinterpret_cast<zonetool::h1::vec3_t*>(allocator.allocate_array<float>(std::max<size_t>(vert_count, 1) * 3));
				std::memcpy(info->pCollisionData.verts, b.verts.data(), vert_count * sizeof(float) * 3);

				const auto tri_count = static_cast<int>(b.tri_indices.size() / 3);
				info->pCollisionData.triCount = tri_count;
				info->pCollisionData.triIndices = allocator.allocate_array<unsigned short>(std::max<size_t>(b.tri_indices.size(), 1));
				std::memcpy(info->pCollisionData.triIndices, b.tri_indices.data(), b.tri_indices.size() * sizeof(unsigned short));

				const size_t walk_bytes = (((static_cast<size_t>(tri_count) * 3 + static_cast<size_t>(tri_count)) + 31) >> 3) & 0x1FFFFFFC;
				// matches clip_map::dump: ((tc + 2*tc + 31) >> 3) & ~3
				const size_t walk_bytes2 = (((static_cast<size_t>(tri_count) + 2 * static_cast<size_t>(tri_count)) + 31) >> 3) & 0x1FFFFFFC;
				(void)walk_bytes;
				info->pCollisionData.triEdgeIsWalkable = allocator.allocate_array<unsigned char>(std::max<size_t>(walk_bytes2, 4));
				std::memset(info->pCollisionData.triEdgeIsWalkable, 0xFF, std::max<size_t>(walk_bytes2, 4));

				info->pCollisionData.borderCount = 0;
				info->pCollisionData.borders = nullptr;
				info->pCollisionData.partitionCount = static_cast<int>(b.partitions.size());
				info->pCollisionData.partitions = allocator.allocate_array<zonetool::h1::CollisionPartition>(b.partitions.size());
				std::memcpy(info->pCollisionData.partitions, b.partitions.data(), b.partitions.size() * sizeof(zonetool::h1::CollisionPartition));

				info->sCollisionData.numStaticModels = 0;
				info->sCollisionData.staticModelList = nullptr;

				h1_asset->pInfo = &h1_asset->info;

				// bsp: one node, two empty-ish leafs; leaf 1 holds the whole terrain tree
				zonetool::h1::Bounds world{};
				for (int k = 0; k < 3; k++)
				{
					world.midPoint[k] = b.nodes[0].midPoint[k];
					world.halfSize[k] = b.nodes[0].halfSize[k];
				}

				h1_asset->numNodes = 1;
				h1_asset->nodes = allocator.allocate_array<zonetool::h1::cNode_t>(1);
				h1_asset->nodes[0].plane = allocator.allocate<zonetool::h1::cplane_s>();
				h1_asset->nodes[0].plane->normal[2] = 1.0f;
				h1_asset->nodes[0].plane->dist = 1.0e6f;
				h1_asset->nodes[0].plane->type = 2;
				h1_asset->nodes[0].children[0] = -2;
				h1_asset->nodes[0].children[1] = -2;

				h1_asset->numLeafs = 2;
				h1_asset->leafs = allocator.allocate_array<zonetool::h1::cLeaf_t>(2);
				h1_asset->leafs[1].firstCollAabbIndex = 0;
				h1_asset->leafs[1].collAabbCount = 1;
				h1_asset->leafs[1].brushContents = 0;
				h1_asset->leafs[1].terrainContents = 1;
				h1_asset->leafs[1].bounds = world;
				h1_asset->leafs[1].leafBrushNode = 0;

				// v53: one cmodel per GfxWorld brush model (script_brushmodel "*N"); the zone writer shares the ClipInfo.
				//      Their triangles are already part of the world collision tree, so submodels get an empty leaf.
				const int sub_count = std::max(1, asset->modelCount);
				h1_asset->numSubModels = sub_count;
				h1_asset->cmodels = allocator.allocate_array<zonetool::h1::cmodel_t>(sub_count);
				for (int sm = 1; sm < sub_count; sm++)
				{
					auto& cm = h1_asset->cmodels[sm];
					for (int k = 0; k < 3; k++)
					{
						cm.bounds.midPoint[k] = asset->models[sm].bounds.midPoint[k];
						cm.bounds.halfSize[k] = asset->models[sm].bounds.halfSize[k];
					}
					cm.radius = asset->models[sm].radius;
					cm.info = nullptr;
					cm.leaf.firstCollAabbIndex = 0;
					cm.leaf.collAabbCount = 0;
					cm.leaf.brushContents = 0;
					cm.leaf.terrainContents = 0;
					cm.leaf.bounds = cm.bounds;
					cm.leaf.leafBrushNode = 0;
				}
				h1_asset->cmodels[0].bounds = world;
				h1_asset->cmodels[0].radius = std::sqrt(world.halfSize[0] * world.halfSize[0] + world.halfSize[1] * world.halfSize[1] + world.halfSize[2] * world.halfSize[2]);
				h1_asset->cmodels[0].info = nullptr; // mapped in dump
				h1_asset->cmodels[0].leaf = h1_asset->leafs[1];

				h1_asset->mapEnts = allocator.allocate<zonetool::h1::MapEnts>();
				h1_asset->mapEnts->name = asset->name;

				h1_asset->stageCount = 1;
				h1_asset->stages = allocator.allocate_array<zonetool::h1::Stage>(1);
				h1_asset->stages[0].name = allocator.duplicate_string("unknown");
				h1_asset->stages[0].radiometricScale = 0.001f;

				h1_asset->checksum = 0;

				return h1_asset;
			}

			void dump_from_gfxworld(GfxWorld* asset)
			{
				utils::memory::allocator allocator;

				const auto converted_asset = generate_from_gfxworld(asset, allocator);
				zonetool::h1::clip_map::dump(converted_asset);

				const auto pw = physworld::generate(converted_asset, allocator);
				zonetool::h1::phys_world::dump(pw);

				// IW7 has no equivalent of the IW6/H1 glass + fx world data: emit empty ones
				const auto fx = allocator.allocate<zonetool::h1::FxWorld>();
				fx->name = asset->name;
				zonetool::h1::fx_world::dump(fx);

				const auto glass = allocator.allocate<zonetool::h1::GlassWorld>();
				glass->name = asset->name;
				glass->g_glassData = allocator.allocate<zonetool::h1::G_GlassData>();
				zonetool::h1::glass_world::dump(glass);
			}
		}
	}
}
