#include <set>
#include <std_include.hpp>
#include "zonetool/iw7/converter/h1/include.hpp"
#include "gfxworld.hpp"
#include <map>
#include <unordered_map>
#include "clipmap.hpp"
#include <utils/string.hpp>

#include "zonetool/h1/assets/gfxworld.hpp"

// IW7 -> H1 GfxWorld, FIRST PASS (untested).
//
// What is converted for real:  planes/nodes, skies, cells + aabb trees (merged from the transient zones),
//   vertices / vertex layer data (merged from the transient zones), indices, surfaces, brush models,
//   sun, static model placements, bounds.
// What is STUBBED (IW7 data has no H1 equivalent):
//   lightmaps (none), light grid (flat, 1-node), reflection probes (1 stub), vis data (zeroed),
//   static model lighting (default light grid colour), portal groups, mdao.

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace gfxworld
		{
			namespace
			{
				constexpr unsigned char NO_LIGHTMAP = 0xFF; // TODO verify against H1 behaviour
			}

			zonetool::h1::GfxWorld* convert(GfxWorld* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::GfxWorld>();

				REINTERPRET_CAST_SAFE(name);
				REINTERPRET_CAST_SAFE(baseName);
				new_asset->bspVersion = 111;
				COPY_VALUE(planeCount);
				COPY_VALUE(nodeCount);
				COPY_VALUE(surfaceCount);

				// zones holding the streamed world data
				const unsigned int zone_count = std::min<unsigned int>(asset->draw.transientZoneCount, 32);
				std::vector<unsigned int> vertex_base(zone_count + 1, 0);
				std::vector<unsigned int> layer_base(zone_count + 1, 0);
				for (unsigned int z = 0; z < zone_count; z++)
				{
					const auto* zone = asset->draw.transientZones[z];
					vertex_base[z + 1] = vertex_base[z] + (zone ? zone->vertexCount : 0);
					layer_base[z + 1] = layer_base[z] + (zone ? zone->vertexLayerDataSize : 0);
				}

				// skies
				COPY_VALUE(skyCount);
				new_asset->skies = allocator.allocate_array<zonetool::h1::GfxSky>(asset->skyCount);
				for (int i = 0; i < asset->skyCount; i++)
				{
					if (asset->skies[i].skyImage)
					{
						new_asset->skies[i].skyImage = allocator.allocate<zonetool::h1::GfxImage>();
						new_asset->skies[i].skyImage->name = asset->skies[i].skyImage->name;
					}
					new_asset->skies[i].skySurfCount = asset->skies[i].skySurfCount;
					new_asset->skies[i].skyStartSurfs = asset->skies[i].skyStartSurfs;
					new_asset->skies[i].skySamplerState = asset->skies[i].skySamplerState;
					if (asset->skies[i].skySurfCount > 0 && asset->dpvs.sortedSurfIndex && asset->dpvs.surfacesBounds)
					{
						const auto index = asset->dpvs.sortedSurfIndex[asset->skies[i].skyStartSurfs[0]];
						memcpy(&new_asset->skies[i].bounds, &asset->dpvs.surfacesBounds[index].bounds, sizeof(Bounds));
					}
				}

				COPY_VALUE(lastSunPrimaryLightIndex);
				COPY_VALUE(primaryLightCount);
				new_asset->primaryLightEnvCount = 0; // set from ComWorld at runtime; see comworld converter
				new_asset->sortKeyLitDecal = 7;
				new_asset->sortKeyEffectDecal = 43;
				new_asset->sortKeyTopDecal = 17;
				new_asset->sortKeyEffectAuto = 53;
				new_asset->sortKeyDistortion = 48;
				new_asset->sortKeyHair = 18;
				new_asset->sortKeyEffectBlend = 33;

				COPY_VALUE(dpvsPlanes.cellCount);
				REINTERPRET_CAST_SAFE(dpvsPlanes.planes);
				REINTERPRET_CAST_SAFE(dpvsPlanes.nodes);
				REINTERPRET_CAST_SAFE(dpvsPlanes.sceneEntCellBits);

				// cells: each cell's aabb trees live in the transient zone named by cellTransientInfos
				const int cell_count = asset->dpvsPlanes.cellCount;
				new_asset->aabbTreeCounts = allocator.allocate_array<zonetool::h1::GfxCellTreeCount>(cell_count);
				new_asset->aabbTrees = allocator.allocate_array<zonetool::h1::GfxCellTree>(cell_count);
				for (int i = 0; i < cell_count; i++)
				{
					if (!asset->cellTransientInfos) break;

					const auto& info = asset->cellTransientInfos[i];
					if (info.transientZone >= zone_count) continue;
					const auto* zone = asset->draw.transientZones[info.transientZone];
					if (!zone || !zone->aabbTreeCounts || !zone->aabbTrees) continue;

					const int tree_count = zone->aabbTreeCounts[info.aabbTreeIndex].aabbTreeCount;
					const auto* src_trees = zone->aabbTrees[info.aabbTreeIndex].aabbTree;
					new_asset->aabbTreeCounts[i].aabbTreeCount = tree_count;
					new_asset->aabbTrees[i].aabbTree = allocator.allocate_array<zonetool::h1::GfxAabbTree>(tree_count);
					for (int j = 0; j < tree_count; j++)
					{
						auto& d = new_asset->aabbTrees[i].aabbTree[j];
						const auto& s = src_trees[j];
						memcpy(&d.bounds, &s.bounds, sizeof(Bounds));
						d.startSurfIndex = static_cast<int>(s.startSurfIndex);
						d.surfaceCount = s.surfaceCount;
						d.smodelIndexCount = s.smodelIndexCount;
						REINTERPRET_CAST_SAFE_TO_FROM(d.smodelIndexes, s.smodelIndexes);
						d.childCount = s.childCount;
						if (s.childrenOffset)
						{
							// IW7 node is 56 bytes, H1 node is 48 bytes
							const int child_index = s.childrenOffset / static_cast<int>(sizeof(GfxAabbTree));
							d.childrenOffset = child_index * static_cast<int>(sizeof(zonetool::h1::GfxAabbTree));
						}
					}
				}

				// v76: IW7 keeps never-cull static models (cullDist 0: lion logos, HAIL sign) out of the cell aabb trees,
				//      so H1 never draws them. Append every unreferenced static model to the root node of the cells containing it.
				if (asset->cellTransientInfos && asset->dpvs.smodelCount && asset->dpvs.smodelDrawInsts)
				{
					std::vector<bool> referenced(asset->dpvs.smodelCount, false);
					for (int i = 0; i < cell_count; i++)
					{
						for (int j = 0; j < new_asset->aabbTreeCounts[i].aabbTreeCount; j++)
						{
							const auto& t = new_asset->aabbTrees[i].aabbTree[j];
							for (int k = 0; t.smodelIndexes && k < t.smodelIndexCount; k++)
								if (t.smodelIndexes[k] < referenced.size()) referenced[t.smodelIndexes[k]] = true;
						}
					}
					std::vector<std::vector<unsigned short>> extra(cell_count);
					int added = 0;
					for (unsigned int m = 0; m < asset->dpvs.smodelCount; m++)
					{
						if (referenced[m]) continue;
						const float* o = asset->dpvs.smodelDrawInsts[m].placement.origin;
						bool placed = false;
						for (int i = 0; i < cell_count; i++)
						{
							const auto& b = asset->cells[i].bounds;
							if (std::abs(o[0] - b.midPoint[0]) <= b.halfSize[0] && std::abs(o[1] - b.midPoint[1]) <= b.halfSize[1] && std::abs(o[2] - b.midPoint[2]) <= b.halfSize[2])
							{
								extra[i].push_back(static_cast<unsigned short>(m)); placed = true;
							}
						}
						if (!placed) for (int i = 0; i < cell_count; i++) extra[i].push_back(static_cast<unsigned short>(m));
						added++;
					}
					for (int i = 0; i < cell_count; i++)
					{
						if (extra[i].empty() || new_asset->aabbTreeCounts[i].aabbTreeCount <= 0) continue;
						auto& root = new_asset->aabbTrees[i].aabbTree[0];
						const int n = root.smodelIndexCount + static_cast<int>(extra[i].size());
						auto* idx = allocator.allocate_array<unsigned short>(n);
						for (int k = 0; k < root.smodelIndexCount; k++) idx[k] = root.smodelIndexes[k];
						for (size_t k = 0; k < extra[i].size(); k++) idx[root.smodelIndexCount + k] = extra[i][k];
						root.smodelIndexes = idx;
						root.smodelIndexCount = static_cast<unsigned short>(n);
					}
					ZONETOOL_INFO("v76 unreferenced static models added to cells: %d", added);
				}

				new_asset->cells = allocator.allocate_array<zonetool::h1::GfxCell>(cell_count);
				for (int i = 0; i < cell_count; i++)
				{
					memcpy(&new_asset->cells[i].bounds, &asset->cells[i].bounds, sizeof(Bounds));
					new_asset->cells[i].portalCount = asset->cells[i].portalCount;
					REINTERPRET_CAST_SAFE_TO_FROM(new_asset->cells[i].portals, asset->cells[i].portals);
					new_asset->cells[i].reflectionProbeCount = 0;
					new_asset->cells[i].reflectionProbes = nullptr;
					new_asset->cells[i].reflectionProbeReferenceCount = 0;
					new_asset->cells[i].reflectionProbeReferences = nullptr;
				}

				new_asset->portalGroupCount = 0;
				new_asset->portalGroup = nullptr;
				new_asset->portalDistanceAnchorCount = 0;
				new_asset->portalDistanceAnchorsAndCloseDistSquared = nullptr;

				// v101: real IW7 reflection probes. IW7 keeps them in one BC6H cube array; iw7_port2.ps1 installs one H1 cube per
				//      probe (_reflection_probeN.h1Image, decoded offline from _reflection_probe_array). Surfaces and static models
				//      use the nearest probe (IW7 picks probes through volumes we do not convert).
				const unsigned int probe_count = std::clamp(asset->draw.reflectionProbeData.reflectionProbeCount, 1u, 255u);
				new_asset->draw.reflectionProbeCount = probe_count;
				new_asset->draw.reflectionProbes = allocator.allocate_array<zonetool::h1::GfxImage*>(probe_count);
				new_asset->draw.reflectionProbeOrigins = allocator.allocate_array<zonetool::h1::GfxReflectionProbe>(probe_count);
				new_asset->draw.reflectionProbeTextures = allocator.allocate_array<zonetool::h1::GfxRawTexture>(probe_count);
				for (unsigned int pi = 0; pi < probe_count; pi++)
				{
					new_asset->draw.reflectionProbes[pi] = allocator.allocate<zonetool::h1::GfxImage>();
					new_asset->draw.reflectionProbes[pi]->name = allocator.duplicate_string(::utils::string::va("*reflection_probe%u", pi));
					if (asset->draw.reflectionProbeData.reflectionProbes && pi < asset->draw.reflectionProbeData.reflectionProbeCount)
						memcpy(new_asset->draw.reflectionProbeOrigins[pi].origin, asset->draw.reflectionProbeData.reflectionProbes[pi].origin, sizeof(float) * 3);
				}
				ZONETOOL_INFO("v101 reflection probes: %u", probe_count);
				const auto nearest_probe = [&](const float* p) -> unsigned char
				{
					unsigned int best = 0; float bd = 3.4e38f;
					for (unsigned int pi = 0; pi < probe_count; pi++)
					{
						const float* o = new_asset->draw.reflectionProbeOrigins[pi].origin;
						const float d = (o[0] - p[0]) * (o[0] - p[0]) + (o[1] - p[1]) * (o[1] - p[1]) + (o[2] - p[2]) * (o[2] - p[2]);
						if (d < bd) { bd = d; best = pi; }
					}
					return static_cast<unsigned char>(best);
				};
				new_asset->draw.reflectionProbeReferenceCount = 0;
				new_asset->draw.reflectionProbeReferenceOrigins = nullptr;
				new_asset->draw.reflectionProbeReferences = nullptr;

				// v79: lightmap diagnostics (IW7 packs lightmaps; surfaces index lightmapReindexData)
				{
					const auto& rd = asset->draw.lightmapReindexData;
					ZONETOOL_INFO("v79 lm: lightMapCount=%u imagePixelSize=%u reindexCount=%u packedCount=%u", asset->draw.lightMapCount, rd.imagePixelSize, rd.reindexCount, rd.packedLightmapCount);
					for (unsigned int i = 0; i < asset->draw.lightMapCount && i < 8; i++)
					{
						const auto* lm = asset->draw.lightMaps ? asset->draw.lightMaps[i] : nullptr;
						if (!lm) continue;
						for (int t = 0; t < 3; t++)
						{
							const auto* im = lm->textures[t];
							if (im) ZONETOOL_INFO("v79 lm %u tex%d %s %ux%ux%u fmt=%u elems=%u lv=%u streamed=%u", i, t, im->name ? im->name : "?", (unsigned)im->width, (unsigned)im->height, (unsigned)im->depth, (unsigned)im->imageFormat, (unsigned)im->numElements, (unsigned)im->levelCount, (unsigned)im->streamed);
						}
					}
					for (unsigned int i = 0; i < rd.packedLightmapCount && rd.packedLightmap; i++)
						ZONETOOL_INFO("v79 packed %u: %ux%u", i, rd.packedLightmap[i].imageWidth, rd.packedLightmap[i].imageHeight);
					for (unsigned int i = 0; i < rd.reindexCount && rd.reindexElement && i < 64; i++)
						ZONETOOL_INFO("v79 reindex %u: index=%u pos=%u,%u size=%ux%u", i, rd.reindexElement[i].index, rd.reindexElement[i].posX, rd.reindexElement[i].posY, rd.reindexElement[i].width, rd.reindexElement[i].height);
					std::map<unsigned, unsigned> hist;
					for (unsigned int i = 0; i < asset->surfaceCount; i++) hist[asset->dpvs.surfaces[i].lightmapIndex]++;
					for (auto& [k, v] : hist) ZONETOOL_INFO("v79 surf lightmapIndex %u: %u surfaces", k, v);
					// lmapCoord ranges for a few surfaces per lightmapIndex
					std::map<unsigned, int> shown;
					for (unsigned int i = 0; i < asset->surfaceCount; i++)
					{
						const auto& su = asset->dpvs.surfaces[i];
						if (shown[su.lightmapIndex]++ >= 3) continue;
						const auto z = su.transientZone;
						if (z >= zone_count || !asset->draw.transientZones[z] || !asset->draw.transientZones[z]->vd.vertices) continue;
						const auto* vv = asset->draw.transientZones[z]->vd.vertices;
						float mn[2] = { 1e9f, 1e9f }, mx[2] = { -1e9f, -1e9f };
						for (unsigned int k = 0; k < su.tris.vertexCount; k++)
						{
							const auto& v = vv[su.tris.firstVertex + k];
							for (int c = 0; c < 2; c++) { mn[c] = std::min(mn[c], v.lmapCoord[c]); mx[c] = std::max(mx[c], v.lmapCoord[c]); }
						}
						ZONETOOL_INFO("v79 surf %u lm=%u flags=%u verts=%u lmap u[%f..%f] v[%f..%f] mat=%s", i, su.lightmapIndex, su.flags, su.tris.vertexCount, mn[0], mx[0], mn[1], mx[1], su.material && su.material->name ? su.material->name : "?");
					}
				}

				// v121 bake: export lightmapped world triangles (xyz, packed normal, lightmap uv) for the offline lightmap baker
				{
					std::string buf;
					const auto put = [&](const void* p, std::size_t n) { buf.append(static_cast<const char*>(p), n); };
					unsigned int tri_total = 0;
					put("BK21", 4);
					put(&tri_total, 4);
					for (unsigned int i = 0; i < asset->surfaceCount; i++)
					{
						const auto& su = asset->dpvs.surfaces[i];
						const auto z = su.transientZone;
						if (su.lightmapIndex != 0 || z >= zone_count || !asset->draw.transientZones[z] || !asset->draw.transientZones[z]->vd.vertices || !asset->draw.indices) continue;
						const auto* vv = asset->draw.transientZones[z]->vd.vertices;
						const unsigned int caster = (su.material && su.material->info.sortKey < 7) ? 1u : 0u;
						for (unsigned int t = 0; t < su.tris.triCount; t++)
						{
							put(&caster, 4);
							for (int k = 0; k < 3; k++)
							{
								const auto& v = vv[su.tris.firstVertex + asset->draw.indices[su.tris.baseIndex + t * 3 + k]];
								put(v.xyz, 12); put(&v.normal.packed, 4); put(v.lmapCoord, 8);
							}
							tri_total++;
						}
					}
					std::memcpy(&buf[4], &tri_total, 4);
					filesystem::file file("v121_bake.bin");
					file.open("wb");
					file.write(buf);
					file.close();
					ZONETOOL_INFO("v121 bake: %u lightmapped triangles exported", tri_total);
				}

				// v129 model sun: every static model was drawn fully sunlit (primary light env from IW7, no shadow: the model
				//      techsets' baked visibility comes from the stub light grid), so props under roofs (the gallery statue) glowed.
				//      Take IW7's baked sun visibility (the BC4 primary lightmap, fastfile_iw7 light.rs: 0.5 = shadow edge) on the
				//      floor right under each model (the highest upward-facing lightmapped triangle below its origin, as
				//      fastfile_iw7's ground_light picks it) and drop the sun for models standing in the shade.
				struct v129_floor_t { float p[3][3]; float uv[3][2]; float mn[2]; float mx[2]; };
				std::vector<v129_floor_t> v129_floors;
				std::unordered_map<long long, std::vector<unsigned int>> v129_cells;
				std::vector<unsigned char> v129_prim;
				unsigned int v129_pw = 0, v129_ph = 0;
				{
					for (unsigned int i = 0; i < asset->surfaceCount; i++)
					{
						const auto& su = asset->dpvs.surfaces[i];
						const auto z = su.transientZone;
						if (su.lightmapIndex != 0 || z >= zone_count || !asset->draw.transientZones[z] || !asset->draw.transientZones[z]->vd.vertices || !asset->draw.indices) continue;
						if (!su.material || su.material->info.sortKey >= 7) continue;
						const auto* vv = asset->draw.transientZones[z]->vd.vertices;
						for (unsigned int t = 0; t < su.tris.triCount; t++)
						{
							v129_floor_t f{};
							for (int k = 0; k < 3; k++)
							{
								const auto& v = vv[su.tris.firstVertex + asset->draw.indices[su.tris.baseIndex + t * 3 + k]];
								for (int c = 0; c < 3; c++) f.p[k][c] = v.xyz[c];
								f.uv[k][0] = v.lmapCoord[0]; f.uv[k][1] = v.lmapCoord[1];
							}
							const float e1[3] = { f.p[1][0] - f.p[0][0], f.p[1][1] - f.p[0][1], f.p[1][2] - f.p[0][2] };
							const float e2[3] = { f.p[2][0] - f.p[0][0], f.p[2][1] - f.p[0][1], f.p[2][2] - f.p[0][2] };
							const float nx = e1[1] * e2[2] - e1[2] * e2[1], ny = e1[2] * e2[0] - e1[0] * e2[2], nz = e1[0] * e2[1] - e1[1] * e2[0];
							const float nl = std::sqrt(nx * nx + ny * ny + nz * nz);
							if (nl < 1e-4f || std::abs(nz) / nl < 0.6f) continue;
							f.mn[0] = std::min({ f.p[0][0], f.p[1][0], f.p[2][0] }); f.mx[0] = std::max({ f.p[0][0], f.p[1][0], f.p[2][0] });
							f.mn[1] = std::min({ f.p[0][1], f.p[1][1], f.p[2][1] }); f.mx[1] = std::max({ f.p[0][1], f.p[1][1], f.p[2][1] });
							if (f.mx[0] - f.mn[0] > 8192.0f || f.mx[1] - f.mn[1] > 8192.0f) continue;
							const auto idx = static_cast<unsigned int>(v129_floors.size());
							v129_floors.push_back(f);
							for (int gx = int(std::floor(f.mn[0] / 256.0f)); gx <= int(std::floor(f.mx[0] / 256.0f)); gx++)
								for (int gy = int(std::floor(f.mn[1] / 256.0f)); gy <= int(std::floor(f.mx[1] / 256.0f)); gy++)
									v129_cells[(static_cast<long long>(gx) << 32) ^ static_cast<unsigned int>(gy)].push_back(idx);
						}
					}
					std::string mapname = asset->name ? asset->name : "";
					if (const auto sl = mapname.find_last_of("/\\"); sl != std::string::npos) mapname = mapname.substr(sl + 1);
					if (const auto dt = mapname.find('.'); dt != std::string::npos) mapname = mapname.substr(0, dt);
					const std::string rel = "iw7_assets\\" + mapname + "\\_lightmap0_primary.h1Image";
					for (const auto& cand : { "..\\mwr_game_files\\" + rel, "..\\..\\mwr_game_files\\" + rel, std::string("C:\\Users\\Shadow\\Documents\\mwr_game_files\\") + rel })
					{
						std::ifstream pf(cand, std::ios::binary);
						if (!pf.is_open()) continue;
						std::string all((std::istreambuf_iterator<char>(pf)), std::istreambuf_iterator<char>());
						const std::size_t bytes = 2048u * 4096u / 2u;
						if (all.size() < bytes) continue;
						v129_prim.assign(all.end() - bytes, all.end());
						v129_pw = 2048; v129_ph = 4096;
						ZONETOOL_INFO("v129 model sun: primary lightmap %s, %zu floor triangles", cand.data(), v129_floors.size());
						break;
					}
					if (v129_prim.empty()) ZONETOOL_WARNING("v129 model sun: no primary lightmap found (models stay sunlit)");
				}
				// v133 ground lighting: static models drew far brighter than the world around them whatever the stub light grid
				//      held (v131/v132). Our model techset is an IW6 lprobe set, which reads LIGHT_PROBE_AMBIENT = the instance's
				//      ground lighting (docs/H1_NATIVE.md, IW6_NATIVE.md). Give each model the baked light of the floor under it
				//      (our H1 secondary at that lightmap texel, upper page + 0.8 x directional page), as fastfile_iw7's ground_light.
				float v133_last_uv[2] = { 0.0f, 0.0f };
				std::vector<std::uint16_t> v133_sec;
				{
					std::string mapname = asset->name ? asset->name : "";
					if (const auto sl = mapname.find_last_of("/\\"); sl != std::string::npos) mapname = mapname.substr(sl + 1);
					if (const auto dt = mapname.find('.'); dt != std::string::npos) mapname = mapname.substr(0, dt);
					const std::string rel = "iw7_assets\\" + mapname + "\\_lightmap0_secondary.h1Image";
					for (const auto& cand : { "..\\mwr_game_files\\" + rel, "..\\..\\mwr_game_files\\" + rel, std::string("C:\\Users\\Shadow\\Documents\\mwr_game_files\\") + rel })
					{
						std::ifstream pf(cand, std::ios::binary);
						if (!pf.is_open()) continue;
						std::string all((std::istreambuf_iterator<char>(pf)), std::istreambuf_iterator<char>());
						const std::size_t bytes = 1024u * 4096u * 8u;
						if (all.size() < bytes) continue;
						v133_sec.resize(bytes / 2);
						std::memcpy(v133_sec.data(), all.data() + (all.size() - bytes), bytes);
						break;
					}
				}
				const auto v133_ground = [&](float q_u, float q_v, float* q_rgb) -> bool
				{
					if (v133_sec.empty()) return false;
					const int q_x = std::clamp(int(q_u * 1024.0f), 0, 1023), q_y = std::clamp(int(q_v * 2048.0f), 0, 2047);
					const std::size_t q_top = (std::size_t(q_y) * 1024 + q_x) * 4, q_bot = (std::size_t(q_y + 2048) * 1024 + q_x) * 4;
					for (int q_c = 0; q_c < 3; q_c++)
						q_rgb[q_c] = half_float::half_to_float(v133_sec[q_top + q_c]) + 0.8f * half_float::half_to_float(v133_sec[q_bot + q_c]);
					return true;
				};
				const auto v129_bc4 = [&](float q_u, float q_v) -> float
				{
					const int q_x = std::clamp(int(q_u * v129_pw), 0, int(v129_pw) - 1), q_y = std::clamp(int(q_v * v129_ph), 0, int(v129_ph) - 1);
					const unsigned char* q_b = &v129_prim[(std::size_t(q_y / 4) * (v129_pw / 4) + std::size_t(q_x / 4)) * 8];
					const int q_r0 = q_b[0], q_r1 = q_b[1];
					unsigned long long q_bits = 0;
					for (int q_k = 0; q_k < 6; q_k++) q_bits |= static_cast<unsigned long long>(q_b[2 + q_k]) << (8 * q_k);
					const int q_sel = static_cast<int>((q_bits >> (3 * ((q_y % 4) * 4 + (q_x % 4)))) & 7);
					float q_val;
					if (q_sel == 0) q_val = float(q_r0);
					else if (q_sel == 1) q_val = float(q_r1);
					else if (q_r0 > q_r1) q_val = ((8 - q_sel) * q_r0 + (q_sel - 1) * q_r1) / 7.0f;
					else if (q_sel < 6) q_val = ((6 - q_sel) * q_r0 + (q_sel - 1) * q_r1) / 5.0f;
					else q_val = q_sel == 6 ? 0.0f : 255.0f;
					return q_val / 255.0f;
				};
				// sun visibility under a point: -1 when no floor or no primary lightmap
				const auto v129_sun_at = [&](const float* q_o) -> float
				{
					if (v129_prim.empty()) return -1.0f;
					const long long q_key = (static_cast<long long>(int(std::floor(q_o[0] / 256.0f))) << 32) ^ static_cast<unsigned int>(int(std::floor(q_o[1] / 256.0f)));
					const auto q_it = v129_cells.find(q_key);
					if (q_it == v129_cells.end()) return -1.0f;
					float q_best_z = -1e30f, q_best = -1.0f;
					for (const auto q_idx : q_it->second)
					{
						const auto& q_f = v129_floors[q_idx];
						if (q_o[0] < q_f.mn[0] || q_o[0] > q_f.mx[0] || q_o[1] < q_f.mn[1] || q_o[1] > q_f.mx[1]) continue;
						const float q_x0 = q_f.p[0][0], q_y0 = q_f.p[0][1], q_x1 = q_f.p[1][0], q_y1 = q_f.p[1][1], q_x2 = q_f.p[2][0], q_y2 = q_f.p[2][1];
						const float q_den = (q_y1 - q_y2) * (q_x0 - q_x2) + (q_x2 - q_x1) * (q_y0 - q_y2);
						if (std::abs(q_den) < 1e-6f) continue;
						const float q_w0 = ((q_y1 - q_y2) * (q_o[0] - q_x2) + (q_x2 - q_x1) * (q_o[1] - q_y2)) / q_den;
						const float q_w1 = ((q_y2 - q_y0) * (q_o[0] - q_x2) + (q_x0 - q_x2) * (q_o[1] - q_y2)) / q_den;
						const float q_w2 = 1.0f - q_w0 - q_w1;
						if (q_w0 < -0.01f || q_w1 < -0.01f || q_w2 < -0.01f) continue;
						const float q_zf = q_w0 * q_f.p[0][2] + q_w1 * q_f.p[1][2] + q_w2 * q_f.p[2][2];
						if (q_zf > q_o[2] + 8.0f || q_o[2] - q_zf > 512.0f || q_zf <= q_best_z) continue;
						q_best_z = q_zf;
						const float q_u = q_w0 * q_f.uv[0][0] + q_w1 * q_f.uv[1][0] + q_w2 * q_f.uv[2][0];
						const float q_v = q_w0 * q_f.uv[0][1] + q_w1 * q_f.uv[1][1] + q_w2 * q_f.uv[2][1];
						q_best = v129_bc4(q_u, q_v);
						v133_last_uv[0] = q_u; v133_last_uv[1] = q_v;
					}
					return q_best;
				};
				unsigned int v129_shaded = 0, v129_lit = 0, v129_none = 0, v133_grounded = 0;
				constexpr bool v133_ground_lighting = true;

				// lightmaps: STUB (none)
				new_asset->draw.lightmapCount = 1;
				new_asset->draw.lightmaps = allocator.allocate_array<zonetool::h1::GfxLightmapArray>(1);
				new_asset->draw.lightmaps[0].primary = allocator.allocate<zonetool::h1::GfxImage>();
				new_asset->draw.lightmaps[0].primary->name = allocator.duplicate_string("*lightmap0_primary");
				new_asset->draw.lightmaps[0].secondary = allocator.allocate<zonetool::h1::GfxImage>();
				new_asset->draw.lightmaps[0].secondary->name = allocator.duplicate_string("*lightmap0_secondary");
				new_asset->draw.lightmapPrimaryTextures = allocator.allocate_array<zonetool::h1::GfxRawTexture>(1);
				new_asset->draw.lightmapSecondaryTextures = allocator.allocate_array<zonetool::h1::GfxRawTexture>(1);
				new_asset->draw.lightmapOverridePrimary = nullptr;
				new_asset->draw.lightmapOverrideSecondary = nullptr;
				new_asset->draw.lightmapParameters.lightmapWidthPrimary = 1024;
				new_asset->draw.lightmapParameters.lightmapHeightPrimary = 1024;
				new_asset->draw.lightmapParameters.lightmapWidthSecondary = 512;
				new_asset->draw.lightmapParameters.lightmapHeightSecondary = 512;
				new_asset->draw.lightmapParameters.lightmapModelUnitsPerTexel = 8;
				new_asset->draw.trisType = 0;

				// vertices + layer data, concatenated across zones
				new_asset->draw.vertexCount = vertex_base[zone_count];
				new_asset->draw.vd.vertices = allocator.allocate_array<zonetool::h1::GfxWorldVertex>(new_asset->draw.vertexCount);
				new_asset->draw.vertexLayerDataSize = layer_base[zone_count];
				new_asset->draw.vld.data = allocator.allocate_array<unsigned char>(new_asset->draw.vertexLayerDataSize);
				for (unsigned int z = 0; z < zone_count; z++)
				{
					const auto* zone = asset->draw.transientZones[z];
					if (!zone) continue;
					if (zone->vd.vertices && zone->vertexCount)
					{
						memcpy(&new_asset->draw.vd.vertices[vertex_base[z]], zone->vd.vertices,
							sizeof(zonetool::h1::GfxWorldVertex) * zone->vertexCount);
					}
					if (zone->vld.data && zone->vertexLayerDataSize)
					{
						memcpy(&new_asset->draw.vld.data[layer_base[z]], zone->vld.data, zone->vertexLayerDataSize);
					}
				}

				COPY_VALUE(draw.indexCount);
				REINTERPRET_CAST_SAFE(draw.indices);
				new_asset->draw.displacementParmsCount = 0;
				new_asset->draw.displacementParms = nullptr;

				// light grid: STUB, flat 1-node tree (IW7 probe/tetrahedra grid has no H1 equivalent)
				new_asset->lightGrid.hasLightRegions = false;
				new_asset->lightGrid.useSkyForLowZ = false;
				new_asset->lightGrid.lastSunPrimaryLightIndex = asset->lastSunPrimaryLightIndex;
				new_asset->lightGrid.colorCount = 1;
				new_asset->lightGrid.colors = allocator.allocate_array<zonetool::h1::GfxLightGridColors>(1);
				new_asset->lightGrid.entryCount = 1;
				new_asset->lightGrid.entries = allocator.allocate_array<zonetool::h1::GfxLightGridEntry>(1);
				new_asset->lightGrid.rawRowDataSize = 4;
				new_asset->lightGrid.rawRowData = allocator.allocate_array<unsigned char>(4);
				new_asset->lightGrid.rowDataStart = allocator.allocate_array<unsigned short>(1);
				new_asset->lightGrid.missingGridColorIndex = 0;
				new_asset->lightGrid.tableVersion = asset->lightGrid.tableVersion;
				new_asset->lightGrid.paletteVersion = asset->lightGrid.paletteVersion;
				new_asset->lightGrid.rangeExponent8BitsEncoding = asset->lightGrid.rangeExponent8BitsEncoding;
				new_asset->lightGrid.rangeExponent12BitsEncoding = asset->lightGrid.rangeExponent12BitsEncoding;
				new_asset->lightGrid.rangeExponent16BitsEncoding = asset->lightGrid.rangeExponent16BitsEncoding;
				new_asset->lightGrid.stageCount = 0;
				new_asset->lightGrid.stageLightingContrastGain = nullptr;
				for (unsigned int i = 0; i < 56; i++)
				{
					for (unsigned int j = 0; j < 3; j++)
					{
						new_asset->lightGrid.defaultLightGridColors.rgb[i][j] = half_float::float_to_half(asset->lightGrid.defaultLightGridColors.rgb[i][j]);
						new_asset->lightGrid.skyLightGridColors.rgb[i][j] = half_float::float_to_half(asset->lightGrid.skyLightGridColors.rgb[i][j]);
					}
				}
				{
					float v132_d = 0.0f, v132_s = 0.0f;
					for (unsigned int i = 0; i < 56; i++) for (unsigned int j = 0; j < 3; j++) { v132_d += asset->lightGrid.defaultLightGridColors.rgb[i][j]; v132_s += asset->lightGrid.skyLightGridColors.rgb[i][j]; }
					filesystem::file v132_f("v132_grid.txt");
					v132_f.open("wb");
					v132_f.write(std::string(utils::string::va("iw7 default grid mean %f sky mean %f\n", v132_d / 168.0f, v132_s / 168.0f)));
					v132_f.close();
				}
				// v58: the stub grid had all-zero colours, so static models (STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING) were black
				// wherever the sun didn't hit them. Give the single grid entry + defaults a flat ambient instead.
				for (unsigned int i = 0; i < 56; i++)
				{
					for (unsigned int j = 0; j < 3; j++)
					{
						new_asset->lightGrid.colors[0].rgb[i][j] = 0x3400; // v131: 0.25 (was 0.5): with the toned lightmap (v129) props read far brighter than the world around them
						// v132: the gallery statue kept the same brightness when the grid entry halved (v131), so static models read the
						//       default/sky colours, which were IW7's own HDR values. Same flat 0.25 for all three.
						new_asset->lightGrid.defaultLightGridColors.rgb[i][j] = 0x3400;
						new_asset->lightGrid.skyLightGridColors.rgb[i][j] = 0x3400;
					}
				}
				for (auto i = 0; i < 3; i++)
				{
					auto& tree = new_asset->lightGrid.tree[i];
					tree.index = static_cast<unsigned char>(i);
					tree.maxDepth = 0;
					tree.unused[0] = 0;
					tree.nodeCount = 1;
					tree.leafCount = 1;
					tree.p_nodeTable = allocator.allocate_array<unsigned int>(1);
					tree.leafTableSize = 4;
					tree.p_leafTable = allocator.allocate_array<unsigned char>(4);
				}

				COPY_VALUE(modelCount);
				new_asset->models = allocator.allocate_array<zonetool::h1::GfxBrushModel>(asset->modelCount);
				for (int i = 0; i < asset->modelCount; i++)
				{
					memset(&new_asset->models[i].writable, 0, sizeof(zonetool::h1::GfxBrushModelWritable));
					memcpy(&new_asset->models[i].bounds, &asset->models[i].bounds, sizeof(Bounds));
					new_asset->models[i].radius = asset->models[i].radius;
					new_asset->models[i].startSurfIndex = asset->models[i].startSurfIndex;
					new_asset->models[i].surfaceCount = asset->models[i].surfaceCount;
					new_asset->models[i].mdaoVolumeIndex = -1;
				}

				memcpy(&new_asset->bounds, &asset->bounds, sizeof(Bounds));
				memcpy(&new_asset->shadowBounds, &asset->bounds, sizeof(Bounds));

				COPY_VALUE(checksum);
				COPY_VALUE(materialMemoryCount);
				REINTERPRET_CAST_SAFE(materialMemory);
				COPY_VALUE_CAST(sun);
				COPY_ARR(outdoorLookupMatrix);
				if (asset->outdoorImage)
				{
					new_asset->outdoorImage = allocator.allocate<zonetool::h1::GfxImage>();
					new_asset->outdoorImage->name = asset->outdoorImage->name;
				}
				// runtime-block bit arrays: engine memsets them at load, so they must exist (IW7 stores none)
				{
					const unsigned int cc = std::max<int>(new_asset->dpvsPlanes.cellCount, 1);
					const unsigned int words = (cc + 31) >> 5;
					new_asset->cellCasterBits = allocator.allocate_array<unsigned int>(cc * words);
					new_asset->cellHasSunLitSurfsBits = allocator.allocate_array<unsigned int>(words);
				}
				new_asset->sceneDynModel = nullptr;
				new_asset->sceneDynBrush = nullptr;
				{
					// v30: engine clears/sets bits here per non-sun primary light (65536 entity bits each); size matches the H1 writer
					const unsigned int span = new_asset->primaryLightCount > static_cast<unsigned int>(asset->lastSunPrimaryLightIndex)
						? new_asset->primaryLightCount - static_cast<unsigned int>(asset->lastSunPrimaryLightIndex) : 1u;
					const unsigned int bytes = (span << 13) - 0x2000;
					new_asset->primaryLightEntityShadowVis = allocator.allocate_array<unsigned int>(std::max<unsigned int>(bytes, 0x800)); // dump_array count is in elements
				}
				new_asset->primaryLightDynEntShadowVis[0] = nullptr;
				new_asset->primaryLightDynEntShadowVis[1] = nullptr;
				new_asset->nonSunPrimaryLightForModelDynEnt = nullptr;
				// engine zeroes one entry per primary light at load -> must be allocated (no shadow surfaces)
				new_asset->shadowGeom = allocator.allocate_array<zonetool::h1::GfxShadowGeometry>(std::max<unsigned int>(asset->primaryLightCount, 1));
				new_asset->shadowGeomOptimized = allocator.allocate_array<zonetool::h1::GfxShadowGeometry>(std::max<unsigned int>(asset->primaryLightCount, 1));
				new_asset->lightRegion = allocator.allocate_array<zonetool::h1::GfxLightRegion>(asset->primaryLightCount);

				// dpvs
				{
					auto& d = new_asset->dpvs;
					const auto& s = asset->dpvs;

					d.smodelCount = s.smodelCount;
					d.subdivVertexLightingInfoCount = 0;
					d.subdivVertexLighting = nullptr;
					d.staticSurfaceCount = s.staticSurfaceCount;
					d.litOpaqueSurfsBegin = s.litOpaqueSurfsBegin;
					d.litOpaqueSurfsEnd = s.litOpaqueSurfsEnd;
					d.unkSurfsBegin = 0;
					d.unkSurfsEnd = 0;
					d.litDecalSurfsBegin = s.litDecalSurfsBegin;
					d.litDecalSurfsEnd = s.litDecalSurfsEnd;
					d.litTransSurfsBegin = s.litTransSurfsBegin;
					d.litTransSurfsEnd = s.litTransSurfsEnd;
					d.shadowCasterSurfsBegin = 0;
					d.shadowCasterSurfsEnd = 0;
					d.emissiveSurfsBegin = s.emissiveSurfsBegin;
					d.emissiveSurfsEnd = s.emissiveSurfsEnd;
					d.smodelVisDataCount = s.smodelVisDataCount;
					d.surfaceVisDataCount = s.surfaceVisDataCount;

					// vis data is runtime-filled; allocate zeroed buffers of the right size
					for (auto i = 0; i < 4; i++)
					{
						d.smodelVisData[i] = allocator.allocate_array<unsigned int>(d.smodelVisDataCount);
						d.surfaceVisData[i] = allocator.allocate_array<unsigned int>(d.surfaceVisDataCount);
						d.smodelUmbraVisData[i] = allocator.allocate_array<unsigned int>(d.smodelVisDataCount);
						d.surfaceUmbraVisData[i] = allocator.allocate_array<unsigned int>(d.surfaceVisDataCount);
					}
					for (auto i = 0; i < 27; i++)
					{
						d.smodelUnknownVisData[i] = allocator.allocate_array<unsigned int>(d.smodelVisDataCount);
						d.surfaceUnknownVisData[i] = allocator.allocate_array<unsigned int>(d.surfaceVisDataCount);
					}
					d.unknownSModelVisData1 = allocator.allocate_array<unsigned int>(d.smodelVisDataCount);
					d.unknownSModelVisData2 = allocator.allocate_array<unsigned int>(d.smodelVisDataCount * 2);
					d.lodData = s.lodData;
					d.tessellationCutoffVisData = allocator.allocate_array<unsigned int>(d.surfaceVisDataCount);

					REINTERPRET_CAST_SAFE(dpvs.sortedSurfIndex);
					REINTERPRET_CAST_SAFE(dpvs.smodelInsts);

					// surfaces: vertex/layer offsets are zone-relative in IW7, make them global
					d.surfaces = allocator.allocate_array<zonetool::h1::GfxSurface>(asset->surfaceCount);
					for (unsigned int i = 0; i < asset->surfaceCount; i++)
					{
						const auto& src_surf = s.surfaces[i];
						auto& dst_surf = d.surfaces[i];
						const unsigned int z = std::min<unsigned int>(src_surf.transientZone, zone_count);

						dst_surf.tris.vertexLayerData = src_surf.tris.vertexLayerData + layer_base[z];
						dst_surf.tris.firstVertex = src_surf.tris.firstVertex + vertex_base[z];
						dst_surf.tris.maxEdgeLength = src_surf.tris.maxEdgeLength;
						dst_surf.tris.unk = -1;
						dst_surf.tris.vertexCount = src_surf.tris.vertexCount;
						dst_surf.tris.triCount = src_surf.tris.triCount;
						dst_surf.tris.baseIndex = src_surf.tris.baseIndex;
						dst_surf.material = reinterpret_cast<zonetool::h1::Material*>(src_surf.material);
						// v37: KOTH/hardpoint zone grid slabs are script-toggled in IW7 but would render permanently (solid red) here
						if (src_surf.material && src_surf.material->name && (std::string(src_surf.material->name).find("caulk") != std::string::npos || std::string(src_surf.material->name).find("koth_grid") != std::string::npos || std::string(src_surf.material->name).find("blockout") != std::string::npos /* v53: KOTH zone grids/blockout are script-toggled brush models; hide again now that brush models render */ ))
						{
							{ static std::set<std::string> logged_hidden; if (logged_hidden.insert(src_surf.material->name).second) ZONETOOL_INFO("v46 hidden surface material: %s tech=%s", src_surf.material->name, (src_surf.material->techniqueSet && src_surf.material->techniqueSet->name) ? src_surf.material->techniqueSet->name : "?"); }
							dst_surf.tris.triCount = 0;
						}
						// v118: what draws around the tower right of the TDM spawn (-660,-1452): it shows its unlit interior
						if (asset->dpvs.surfacesBounds && src_surf.material && src_surf.material->name)
						{
							const float* mp = asset->dpvs.surfacesBounds[i].bounds.midPoint;
							const float* hs = asset->dpvs.surfacesBounds[i].bounds.halfSize;
							const float v118x = mp[0] + 660.0f, v118y = mp[1] + 1452.0f;
							if (v118x * v118x + v118y * v118y < 1400.0f * 1400.0f && (hs[0] > 64.0f || hs[1] > 64.0f || hs[2] > 64.0f))
							{
								ZONETOOL_INFO("v118 near: surf %u %s (%s) mid %.0f %.0f %.0f half %.0f %.0f %.0f tris %u", i, src_surf.material->name,
									(src_surf.material->techniqueSet && src_surf.material->techniqueSet->name) ? src_surf.material->techniqueSet->name : "?",
									mp[0], mp[1], mp[2], hs[0], hs[1], hs[2], static_cast<unsigned>(src_surf.tris.triCount));
							}
						}
						dst_surf.laf.fields.lightmapIndex = 0; // v32: placeholder lightmap 0 exists
						dst_surf.laf.fields.reflectionProbeIndex = (asset->dpvs.surfacesBounds) ? nearest_probe(asset->dpvs.surfacesBounds[i].bounds.midPoint) : 0;
						// v81: env 0 has no lights (numIndices 0); env 1 = the sun (as in mp_dig). With 0 the world got no sun light or sun shadows at all.
						dst_surf.laf.fields.primaryLightEnvIndex = 1;
						// v123: the vista city far below the map (tower surfaces centred thousands of units down) drew full-sun white; IW7 has it
						//       in shade/fog. Keep only the baked lightmap there.
						if (asset->dpvs.surfacesBounds && asset->dpvs.surfacesBounds[i].bounds.midPoint[2] < -1000.0f)
						{
							dst_surf.laf.fields.primaryLightEnvIndex = 0;
						}
						dst_surf.laf.fields.flags = src_surf.flags; // TODO convert
					}

					d.surfacesBounds = allocator.allocate_array<zonetool::h1::GfxSurfaceBounds>(asset->surfaceCount);
					for (unsigned int i = 0; i < asset->surfaceCount; i++)
					{
						memcpy(&d.surfacesBounds[i], &s.surfacesBounds[i], sizeof(zonetool::h1::GfxSurfaceBounds));
						{ // v86: surface -> material + bounds table for locating surfaces by position
							static FILE* sf = fopen("v86_surfaces.txt", "w");
							if (sf) { const auto& b = s.surfacesBounds[i].bounds; const auto* sm = s.surfaces[i].material;
								fprintf(sf, "%u %s %.0f %.0f %.0f %.0f %.0f %.0f %u\n", i, (sm && sm->name) ? sm->name : "?", b.midPoint[0], b.midPoint[1], b.midPoint[2], b.halfSize[0], b.halfSize[1], b.halfSize[2], (unsigned)s.surfaces[i].lightmapIndex);
								if (i + 1 == asset->surfaceCount) fflush(sf); }
						}
					}

					d.smodelDrawInsts = allocator.allocate_array<zonetool::h1::GfxStaticModelDrawInst>(s.smodelCount);
					d.smodelLightingInsts = allocator.allocate_array<zonetool::h1::GfxStaticModelLighting>(s.smodelCount);
					for (unsigned int i = 0; i < s.smodelCount; i++)
					{
						const auto& src_inst = s.smodelDrawInsts[i];
						auto& dst_inst = d.smodelDrawInsts[i];

						memcpy(&dst_inst.placement, &src_inst.placement, sizeof(zonetool::h1::GfxPackedPlacement));
						dst_inst.model = reinterpret_cast<zonetool::h1::XModel*>(src_inst.model);
						{
							// v40: nodraw/blockout/caulk proxy models render as solid red (their image is missing) -> scale to zero
							const auto* xm = src_inst.model;
							bool all_proxy = xm && xm->numsurfs > 0;
							for (unsigned int mi = 0; xm && mi < xm->numsurfs; mi++)
							{
								const auto* mh = xm->materialHandles ? xm->materialHandles[mi] : nullptr;
								const std::string mn = (mh && mh->name) ? mh->name : "";
								if (mn.find("nodraw") == std::string::npos && mn.find("blockout") == std::string::npos &&
									mn.find("caulk") == std::string::npos && mn.find("koth_grid") == std::string::npos)
								{
									all_proxy = false;
									break;
								}
							}
							if (all_proxy) dst_inst.placement.scale = 0.0f;
						}
						// v76: IW7 cullDist 0 = never cull; H1 culls at 0 (the lion emblem / HAIL sign never drew)
						dst_inst.cullDist = src_inst.cullDist ? src_inst.cullDist : 0xFFFF;
						dst_inst.reactiveMotionCullDist = dst_inst.cullDist;
						dst_inst.reactiveMotionLOD = 0;
						dst_inst.lightingHandle = src_inst.lightingHandle;
						dst_inst.staticModelId = static_cast<unsigned short>(i);
						dst_inst.primaryLightEnvIndex = src_inst.primaryLightEnvIndex;
						if (src_inst.placement.origin[2] < -1000.0f) dst_inst.primaryLightEnvIndex = 0; // v123: vista city models, no sun
						{
							const float sv = v129_sun_at(src_inst.placement.origin);
							if (sv < 0.0f) v129_none++;
							else if (sv < 0.5f) { dst_inst.primaryLightEnvIndex = 0; v129_shaded++; }
							else v129_lit++;
						}
						bool v133_has_ground = false;
						float v133_rgb[3] = { 0.0f, 0.0f, 0.0f };
						if (v133_ground_lighting && v129_sun_at(src_inst.placement.origin) >= 0.0f)
							v133_has_ground = v133_ground(v133_last_uv[0], v133_last_uv[1], v133_rgb);
						dst_inst.reflectionProbeIndex = nearest_probe(src_inst.placement.origin);
						{
							const float* o = src_inst.placement.origin;
							const float v118x = o[0] + 660.0f, v118y = o[1] + 1452.0f;
							if (v118x * v118x + v118y * v118y < 1400.0f * 1400.0f && src_inst.model && src_inst.model->name)
							{
								ZONETOOL_INFO("v118 near: smodel %u %s at %.0f %.0f %.0f scale %.2f", i, src_inst.model->name, o[0], o[1], o[2], dst_inst.placement.scale);
							}
						}
						dst_inst.firstMtlSkinIndex = 0; // v28: IW7 skin index (e.g. 11) indexes a table H1 does not have
						dst_inst.sunShadowFlags = src_inst.sunShadowFlags;

						// IW7 flag bits do not map to H1 yet: use light-grid lighting with the (stub) default colour
						dst_inst.flags = zonetool::h1::StaticModelFlag::STATIC_MODEL_FLAG_SCALE_9
							| zonetool::h1::StaticModelFlag::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING;
						d.smodelLightingInsts[i].ambientLightingInfo.colorIndex = 0;
						d.smodelLightingInsts[i].ambientLightingInfo.primaryLightWeight = 1.0f;
						if (v133_has_ground)
						{
							dst_inst.flags = zonetool::h1::StaticModelFlag::STATIC_MODEL_FLAG_SCALE_9 | zonetool::h1::StaticModelFlag::STATIC_MODEL_FLAG_GROUND_LIGHTING;
							for (int c = 0; c < 3; c++) d.smodelLightingInsts[i].ambientLightingInfo.groundLighting.array[c] = half_float::float_to_half(v133_rgb[c]);
							d.smodelLightingInsts[i].ambientLightingInfo.groundLighting.array[3] = half_float::float_to_half(1.0f);
							v133_grounded++;
						}
					}

					ZONETOOL_INFO("v129 model sun: %u static models shaded, %u sunlit, %u with no floor found", v129_shaded, v129_lit, v129_none);
					{
						const std::string v131_msg = utils::string::va("primary %s, floors %zu, shaded %u, sunlit %u, no floor %u, secondary %s, ground lit %u\n", v129_prim.empty() ? "MISSING" : "ok", v129_floors.size(), v129_shaded, v129_lit, v129_none, v133_sec.empty() ? "MISSING" : "ok", v133_grounded);
						filesystem::file v131_f("v129_model_sun.txt");
						v131_f.open("wb");
						v131_f.write(v131_msg);
						v131_f.close();
					}
					d.surfaceMaterials = allocator.allocate_array<zonetool::h1::GfxDrawSurf>(asset->surfaceCount);
					std::unordered_map<const void*, unsigned int> mat_index_map;
					const auto mat_index_of = [&](const void* m) -> unsigned int
					{
						const auto it = mat_index_map.find(m);
						if (it != mat_index_map.end()) return it->second;
						const auto idx = static_cast<unsigned int>(mat_index_map.size());
						mat_index_map[m] = idx;
						return idx;
					};
					for (unsigned int i = 0; i < asset->surfaceCount; i++) // runtime data
					{
						memset(&d.surfaceMaterials[i], 0, sizeof(d.surfaceMaterials[i])); // v27: IW7 packed bits are not valid H1 draw-surf bits
						// v32: fill the H1 draw-surf fields the renderer sorts/draws by
						d.surfaceMaterials[i].fields.objectId = static_cast<unsigned __int64>(i) & 0xFFFF;
						d.surfaceMaterials[i].fields.materialSortedIndex = static_cast<unsigned __int64>(mat_index_of(s.surfaces[i].material)) & 0x3FFF; // v33: dense per-material index in surface order
						{
							const auto* sm = s.surfaces[i].material;
							const unsigned int msk = (sm && sm->name) ? (static_cast<unsigned int>(sm->info.sortKey) & 0x3Fu) : 0u;
							d.surfaceMaterials[i].fields.primarySortKey = msk; // v34: material sort key
							if (i < 30 || i % 2000 == 0)
								ZONETOOL_INFO("v34 surf %u mat=%s mat_sortKey=%u surf_sortKey=%u flags=%u lm=%u", i, (sm && sm->name) ? sm->name : "(null)", msk, (unsigned)s.surfaces[i].sortKey, (unsigned)s.surfaces[i].flags, (unsigned)s.surfaces[i].lightmapIndex);
						}
						{ static std::map<std::string, unsigned int> use; static unsigned int total = 0; const auto* sm2 = s.surfaces[i].material; use[(sm2 && sm2->name) ? sm2->name : "(null)"]++; if (++total == asset->surfaceCount) for (const auto& kv : use) ZONETOOL_INFO("v71 matuse %u %s", kv.second, kv.first.c_str()); } // v71: per-material surface counts
						d.surfaceMaterials[i].fields.tessellation = s.surfaceMaterials ? (static_cast<unsigned __int64>(s.surfaceMaterials[i].fields.tessellation) & 3) : 0;
					}

					REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadow);
					COPY_VALUE(dpvs.sunShadowOptCount);
					COPY_VALUE(dpvs.sunSurfVisDataCount);
					REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadowOpt);
					d.surfaceDeptAndSurf = allocator.allocate_array<zonetool::h1::GfxDepthAndSurf>(d.staticSurfaceCount); // runtime data
					d.constantBuffersLit = allocator.allocate_array<zonetool::h1::GfxWorldDpvsVoid>(s.smodelCount);
					d.constantBuffersAmbient = allocator.allocate_array<zonetool::h1::GfxWorldDpvsVoid>(s.smodelCount);
					for (unsigned int cb = 0; cb < s.smodelCount; cb++)
					{
						d.constantBuffersLit[cb] = allocator.allocate<char>();
						d.constantBuffersAmbient[cb] = allocator.allocate<char>();
					}
					COPY_VALUE(dpvs.usageCount);
				}

				for (int i = 0; i < 2; i++)
				{
					new_asset->dpvsDyn.dynEntClientWordCount[i] = 0; // v18: dynents not converted
					new_asset->dpvsDyn.dynEntClientCount[i] = 0;
					new_asset->dpvsDyn.dynEntCellBits[i] = nullptr;
					for (int j = 0; j < 4; j++)
					{
						new_asset->dpvsDyn.dynEntVisData[i][j] = allocator.allocate_array<unsigned char>(new_asset->dpvsDyn.dynEntClientWordCount[i] * 32);
					}
				}

				COPY_VALUE(mapVtxChecksum);
				COPY_VALUE(heroOnlyLightCount);
				REINTERPRET_CAST_SAFE(heroOnlyLights);
				new_asset->fogTypesAllowed = zonetool::h1::FOG_NORMAL | zonetool::h1::FOG_DFOG;

				// umbra: IW7 tome is a newer format than H1 understands -> drop it (no occlusion culling)
				new_asset->umbraTomeSize = 0;
				new_asset->umbraTomeData = nullptr;
				memset(&new_asset->umbraTomePtr, 0, sizeof(new_asset->umbraTomePtr));
				new_asset->mdaoVolumesCount = 0;
				new_asset->mdaoVolumes = nullptr;

				new_asset->useLightGridDefaultModelLightingLookup = true;
				new_asset->useLightGridDefaultFXLightingLookup = true;
				memset(&new_asset->lightGridDefaultModelLightingLookup, 0, sizeof(new_asset->lightGridDefaultModelLightingLookup));
				memset(&new_asset->lightGridDefaultFXLightingLookup, 0, sizeof(new_asset->lightGridDefaultFXLightingLookup));

				new_asset->buildInfo.bspCommandline = nullptr;
				new_asset->buildInfo.lightCommandline = nullptr;
				new_asset->buildInfo.bspTimestamp = nullptr;
				new_asset->buildInfo.lightTimestamp = nullptr;

				return new_asset;
			}

			void dump(GfxWorld* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::gfx_world::dump(converted_asset);
				clipmap::dump_from_gfxworld(asset);
			}
		}
	}
}
