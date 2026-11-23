#include <std_include.hpp>
#include "bo2map.hpp"

#include "zonetool/h1/assets/gfxworld.hpp"
#include "zonetool/h1/assets/clipmap.hpp"
#include "zonetool/h1/assets/comworld.hpp"
#include "zonetool/h1/assets/mapents.hpp"
#include "zonetool/h1/assets/physworld.hpp"
#include "zonetool/h1/assets/fxworld.hpp"
#include "zonetool/h1/assets/glassworld.hpp"

#include "zonetool/iw7/converter/h1/include.hpp"
#include "zonetool/iw7/converter/h1/assets/physworld.hpp"

#include <algorithm>
#include <array>
#include <cfloat>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>

// Black Ops II -> H1 map, FIRST PASS (untested in game).
//
// What is converted for real: the drawn world (placed models are already
// flattened into it by the Rust exporter) as GfxWorld surfaces and vertices,
// the collision (built from the same triangles, as the IW7 converter does),
// the sun as the one primary light, the spawn and script entities, one
// placeholder material per Black Ops II material.
// What is STUBBED (as in the IW7 converter): lightmaps (one placeholder
// image), reflection probes (one placeholder), light grid (flat, 1 node),
// vis data (zeroed), sky (none), static models (none: they are in the world).
// The BSP/cell structure is one cell with one aabb tree whose children are
// 2048-unit grid cells of surfaces.

namespace zonetool::h1::importer
{
	namespace
	{
		struct reader
		{
			const std::vector<unsigned char>& b;
			size_t at = 0;

			bool ok(size_t n) const { return at + n <= b.size(); }
			std::uint32_t u()
			{
				std::uint32_t v = 0;
				if (ok(4)) std::memcpy(&v, &b[at], 4);
				at += 4;
				return v;
			}
			float f()
			{
				float v = 0;
				if (ok(4)) std::memcpy(&v, &b[at], 4);
				at += 4;
				return v;
			}
			std::uint16_t h()
			{
				std::uint16_t v = 0;
				if (ok(2)) std::memcpy(&v, &b[at], 2);
				at += 2;
				return v;
			}
			std::uint8_t c()
			{
				const auto v = static_cast<std::uint8_t>(ok(1) ? b[at] : 0);
				at += 1;
				return v;
			}
			std::string s()
			{
				const auto n = u();
				std::string out;
				if (ok(n)) out.assign(reinterpret_cast<const char*>(&b[at]), n);
				at += n;
				return out;
			}
		};

		enum kind : std::uint32_t
		{
			K_OPAQUE = 0,
			K_ALPHATEST = 1,
			K_DECAL = 2,
			K_BLEND = 3,
			K_GLASS = 4,
			K_WATER = 5,
			K_SKIP = 6,
		};

		struct material_in
		{
			std::string name;
			std::uint32_t kind;
			std::string h1_name; // wc/bo2_<...>
			std::string col, nrm, spc; // real Black Ops II picture names (images\\<name>.dds), empty = none
		};

		struct vertex_in
		{
			float xyz[3];
			float normal[3];
			float tangent[4];
			float uv[2];
			unsigned char rgba[4];
		};

		struct surface_in
		{
			std::uint32_t material;
			std::uint32_t base_vertex;
			std::uint32_t vertex_count;
			std::uint32_t first_index;
			std::uint32_t tri_count;
			float mins[3];
			float maxs[3];
		};

		struct map_in
		{
			std::string name;
			float mins[3], maxs[3];
			float sun_toward[3], sun_colour[3], ambient[3];
			std::vector<material_in> materials;
			std::vector<vertex_in> vertices;
			std::vector<std::uint16_t> indices;
			std::vector<surface_in> surfaces;
		};

		std::string safe_name(const std::string& in)
		{
			std::string out;
			for (const auto c : in)
			{
				out += (std::isalnum(static_cast<unsigned char>(c)) ? static_cast<char>(std::tolower(static_cast<unsigned char>(c))) : '_');
			}
			if (out.size() > 56) out.resize(56);
			return out;
		}

		bool read_map(const std::string& path, map_in& m)
		{
			std::ifstream f(path, std::ios::binary);
			if (!f) return false;
			const std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
			if (bytes.size() < 8 || std::memcmp(bytes.data(), "BO2H1M01", 8) != 0) return false;
			reader r{bytes, 8};
			m.name = r.s();
			for (auto& v : m.mins) v = r.f();
			for (auto& v : m.maxs) v = r.f();
			for (auto& v : m.sun_toward) v = r.f();
			for (auto& v : m.sun_colour) v = r.f();
			for (auto& v : m.ambient) v = r.f();
			const auto nm = r.u();
			std::unordered_map<std::string, int> seen;
			for (std::uint32_t i = 0; i < nm; i++)
			{
				material_in mat;
				mat.name = r.s();
				mat.kind = r.u();
				auto base = "wc/bo2_" + safe_name(mat.name);
				const auto n = seen[base]++;
				mat.h1_name = n ? base + "_" + std::to_string(n) : base;
				m.materials.push_back(std::move(mat));
			}
			const auto nv = r.u();
			m.vertices.resize(nv);
			for (auto& v : m.vertices)
			{
				for (auto& x : v.xyz) x = r.f();
				for (auto& x : v.normal) x = r.f();
				for (auto& x : v.tangent) x = r.f();
				for (auto& x : v.uv) x = r.f();
				for (auto& x : v.rgba) x = r.c();
			}
			const auto ni = r.u();
			m.indices.resize(ni);
			for (auto& i : m.indices) i = r.h();
			const auto ns = r.u();
			m.surfaces.resize(ns);
			for (auto& s : m.surfaces)
			{
				s.material = r.u();
				s.base_vertex = r.u();
				s.vertex_count = r.u();
				s.first_index = r.u();
				s.tri_count = r.u();
				for (auto& x : s.mins) x = r.f();
				for (auto& x : s.maxs) x = r.f();
			}
			return r.at <= bytes.size();
		}

		// ---- materials: one placeholder per Black Ops II material ----

		std::string g_src_dir;
		std::unordered_set<std::string> g_copied;

		// Copies <src>/images/<name>.dds into the dump's images folder (once). Returns false when the picture is missing.
		bool copy_picture(const std::string& name)
		{
			if (name.empty()) return false;
			if (g_copied.count(name)) return true;
			std::ifstream f(g_src_dir + "images\\" + name + ".dds", std::ios::binary);
			if (!f) return false;
			std::string bytes((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
			if (bytes.size() < 128) return false;
			auto out = zonetool::filesystem::file("images\\"s + name + ".dds"s);
			out.open("wb");
			out.write(bytes);
			out.close();
			g_copied.insert(name);
			return true;
		}

		void read_mats_tsv(const std::string& path, map_in& m)
		{
			std::ifstream f(path);
			if (!f) return;
			std::unordered_map<std::string, std::array<std::string, 3>> by_name;
			std::string line;
			while (std::getline(f, line))
			{
				if (!line.empty() && line.back() == '\r') line.pop_back();
				std::vector<std::string> c;
				std::stringstream lss(line);
				std::string t;
				while (std::getline(lss, t, '\t')) c.push_back(t);
				if (c.size() < 5) continue;
				auto fix = [](const std::string& v) { return v == "-" ? std::string() : v; };
				by_name[c[0]] = {fix(c[2]), fix(c[3]), fix(c[4])};
			}
			for (auto& mat : m.materials)
			{
				const auto it = by_name.find(mat.name);
				if (it == by_name.end()) continue;
				mat.col = it->second[0];
				mat.nrm = it->second[1];
				mat.spc = it->second[2];
			}
		}

		void write_material(const material_in& m)
		{
			const bool blended = m.kind == K_DECAL || m.kind == K_BLEND || m.kind == K_GLASS || m.kind == K_ALPHATEST;
			ordered_json j;
			j["name"] = m.h1_name;
			j["techniqueSet->name"] = blended ? "wc_l_sm_ndw_b0c0l0_cltrans_iw6" : "wc_l_sm_r0c0n0s0l0_nfwpf_iw6";
			j["gameFlags"] = blended ? 22 : 82;
			j["sortKey"] = blended ? 31 : 2;
			j["renderFlags"] = 0;
			j["textureAtlasRowCount"] = 1;
			j["textureAtlasColumnCount"] = 1;
			j["textureAtlasFrameBlend"] = 0;
			j["textureAtlasAsArray"] = 0;
			j["surfaceTypeBits"] = blended ? 8 : 131072;
			j["cameraRegion"] = blended ? 3 : 0;
			j["materialType"] = 36;
			j["assetFlags"] = 0;

			ordered_json constants = ordered_json::array();
			if (!blended)
			{
				ordered_json env;
				env["name"] = "envMapParms";
				env["nameHash"] = 1033475292u;
				env["literal"] = {0.2, 1.0, 2.5, 2.0};
				constants.push_back(env);
			}
			ordered_json tint;
			tint["name"] = "colorTint";
			tint["nameHash"] = 3054254906u;
			tint["literal"] = {1.0, 1.0, 1.0, 1.0};
			constants.push_back(tint);
			if (!blended)
			{
				ordered_json refl;
				refl["name"] = "reflectionRa";
				refl["nameHash"] = 3344177073u;
				refl["literal"] = {8096.0, 0.0, 0.0, 0.0};
				constants.push_back(refl);
			}
			ordered_json bloom;
			bloom["name"] = "bloomParams";
			bloom["nameHash"] = 3733414975u;
			bloom["literal"] = {1.0, 0.0, 0.0, 0.0};
			constants.push_back(bloom);
			j["constantTable"] = constants;

			// Placeholder pictures: H1's own built-in ones (the Black Ops II pictures come later).
			ordered_json textures = ordered_json::array();
			if (!blended)
			{
				ordered_json spec;
				spec["image"] = (!m.spc.empty() && copy_picture(m.spc)) ? m.spc : "$black";
				spec["semantic"] = 8;
				spec["samplerState"] = 19;
				spec["lastCharacter"] = 112;
				spec["firstCharacter"] = 115;
				spec["typeHash"] = 887934131u;
				textures.push_back(spec);
			}
			ordered_json nml;
			nml["image"] = (!m.nrm.empty() && copy_picture(m.nrm)) ? m.nrm : "$identitynormalmap";
			nml["semantic"] = 5;
			nml["samplerState"] = blended ? 1 : 19;
			nml["lastCharacter"] = 112;
			nml["firstCharacter"] = 110;
			nml["typeHash"] = 1507003663u;
			textures.push_back(nml);
			ordered_json col;
			col["image"] = (!m.col.empty() && copy_picture(m.col)) ? m.col : "$white";
			col["semantic"] = 2;
			col["samplerState"] = 19;
			col["lastCharacter"] = 112;
			col["firstCharacter"] = 99;
			col["typeHash"] = 2695565377u;
			textures.push_back(col);
			j["textureTable"] = textures;
			j["subMaterials"] = nullptr;

			const auto path = "materials\\"s + m.h1_name + ".json"s;
			auto file = zonetool::filesystem::file(path);
			const auto str = j.dump(4, ' ', true, nlohmann::detail::error_handler_t::replace);
			file.open("wb");
			file.write(str);
			file.close();
		}

		// ---- collision from triangles (the IW7 converter's builder) ----

		struct tri_t
		{
			float p[3][3];
			unsigned int mat;
			float c[3];
		};

		struct builder
		{
			std::vector<tri_t> tris;
			std::vector<zonetool::h1::CollisionAabbTree> nodes;
			std::vector<zonetool::h1::CollisionPartition> partitions;
			std::vector<float> verts;
			std::vector<unsigned short> tri_indices;
			bool overflow = false;
			unsigned long long dropped = 0;

			void bounds_of(const std::vector<unsigned int>& ids, float mn[3], float mx[3]) const
			{
				mn[0] = mn[1] = mn[2] = FLT_MAX;
				mx[0] = mx[1] = mx[2] = -FLT_MAX;
				for (const auto id : ids)
				{
					for (int v = 0; v < 3; v++)
					{
						for (int k = 0; k < 3; k++)
						{
							mn[k] = std::min(mn[k], tris[id].p[v][k]);
							mx[k] = std::max(mx[k], tris[id].p[v][k]);
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
				std::unordered_map<unsigned long long, unsigned int> local;
				std::vector<unsigned int> local_verts;
				const auto base_count = static_cast<unsigned int>(verts.size() / 3);
				const auto seg = base_count / 1024;
				const auto seg_base = seg * 1024;

				if (seg > 255)
				{
					overflow = true;
					dropped += ids.size();
				}

				nodes[node].childCount = 0;
				nodes[node].materialIndex = static_cast<unsigned short>(mat);

				if (overflow)
				{
					zonetool::h1::CollisionPartition p{};
					p.triCount = 0;
					p.borderCount = 0;
					p.firstVertSegment = 255;
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
						const auto it = local.find(key);
						unsigned int gi;
						if (it == local.end())
						{
							gi = base_count + static_cast<unsigned int>(local_verts.size());
							local_verts.push_back(gi);
							local.emplace(key, gi);
							verts.push_back(pt[0]);
							verts.push_back(pt[1]);
							verts.push_back(pt[2]);
						}
						else
						{
							gi = it->second;
						}
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
				bounds_of(ids, mn, mx);
				set_bounds(node, mn, mx);
				nodes[node].materialIndex = static_cast<unsigned short>(mat);

				if (ids.size() <= 255 || depth > 40)
				{
					if (ids.size() > 255)
					{
						const auto n = (ids.size() + 254) / 255;
						const auto first = static_cast<unsigned int>(nodes.size());
						nodes.resize(nodes.size() + n);
						nodes[node].childCount = static_cast<unsigned short>(n);
						nodes[node].u.firstChildIndex = static_cast<int>(first);
						for (size_t c = 0; c < n; c++)
						{
							std::vector<unsigned int> sub(ids.begin() + c * 255, ids.begin() + std::min(ids.size(), (c + 1) * 255));
							float smn[3], smx[3];
							bounds_of(sub, smn, smx);
							set_bounds(first + static_cast<unsigned int>(c), smn, smx);
							make_leaf(first + static_cast<unsigned int>(c), sub, mat);
						}
						return;
					}
					make_leaf(node, ids, mat);
					return;
				}

				int axis = 0;
				const float ext[3] = {mx[0] - mn[0], mx[1] - mn[1], mx[2] - mn[2]};
				if (ext[1] > ext[axis]) axis = 1;
				if (ext[2] > ext[axis]) axis = 2;
				const auto mid = ids.begin() + ids.size() / 2;
				std::nth_element(ids.begin(), mid, ids.end(), [&](unsigned int a, unsigned int b) { return tris[a].c[axis] < tris[b].c[axis]; });
				std::vector<unsigned int> left(ids.begin(), mid), right(mid, ids.end());
				ids.clear();
				ids.shrink_to_fit();

				const auto first = static_cast<unsigned int>(nodes.size());
				nodes.resize(nodes.size() + 2);
				nodes[node].childCount = 2;
				nodes[node].u.firstChildIndex = static_cast<int>(first);
				build(first, std::move(left), mat, depth + 1);
				build(first + 1, std::move(right), mat, depth + 1);
			}
		};

		unsigned int surface_type_for(const std::string& n)
		{
			static const std::pair<const char*, unsigned int> table[] = {
				{"grass", 10}, {"dirt", 6}, {"sand", 19}, {"gravel", 11}, {"mud", 15}, {"snow", 20}, {"ice", 12},
				{"wood", 22}, {"metal", 13}, {"grate", 14}, {"glass", 9}, {"brick", 2}, {"rock", 18}, {"stone", 18},
				{"plaster", 17}, {"carpet", 3}, {"cloth", 4}, {"fabric", 4}, {"paper", 16}, {"asphalt", 23},
				{"ceramic", 24}, {"tile", 24}, {"plastic", 25}, {"rubber", 26}, {"bark", 1}, {"foliage", 8},
				{"concrete", 5}, {"cement", 5}, {"deck", 13}, {"hull", 13}, {"ship", 13},
			};
			for (const auto& e : table)
			{
				if (n.find(e.first) != std::string::npos) return e.second;
			}
			return 5;
		}

		// Whether a surface is collidable: visible solid kinds only.
		bool solid(std::uint32_t k)
		{
			return k == K_OPAQUE || k == K_ALPHATEST;
		}

		zonetool::h1::clipMap_t* make_clipmap(const map_in& in, const std::vector<unsigned int>& order, utils::memory::allocator& allocator)
		{
			ZONETOOL_INFO("generating collision from the world triangles...");
			const auto h1_asset = allocator.allocate<zonetool::h1::clipMap_t>();
			h1_asset->name = allocator.duplicate_string("maps/mp/" + in.name + ".d3dbsp");
			h1_asset->isInUse = 0;

			builder b;
			std::vector<std::string> mat_names;
			std::unordered_map<std::uint32_t, unsigned int> mat_lookup;

			for (const auto surf_idx : order)
			{
				const auto& s = in.surfaces[surf_idx];
				if (s.material >= in.materials.size() || !solid(in.materials[s.material].kind)) continue;
				auto it = mat_lookup.find(s.material);
				unsigned int mi;
				if (it == mat_lookup.end())
				{
					mi = static_cast<unsigned int>(mat_names.size());
					mat_names.push_back(in.materials[s.material].name);
					mat_lookup.emplace(s.material, mi);
				}
				else
				{
					mi = it->second;
				}
				for (std::uint32_t t = 0; t < s.tri_count; t++)
				{
					tri_t tri{};
					tri.mat = mi;
					bool ok = true;
					for (int v = 0; v < 3; v++)
					{
						const auto ii = s.first_index + t * 3 + v;
						if (ii >= in.indices.size()) { ok = false; break; }
						const auto vi = static_cast<size_t>(s.base_vertex) + in.indices[ii];
						if (vi >= in.vertices.size()) { ok = false; break; }
						std::memcpy(tri.p[v], in.vertices[vi].xyz, sizeof(float) * 3);
					}
					if (!ok) continue;
					const float e1[3] = {tri.p[1][0] - tri.p[0][0], tri.p[1][1] - tri.p[0][1], tri.p[1][2] - tri.p[0][2]};
					const float e2[3] = {tri.p[2][0] - tri.p[0][0], tri.p[2][1] - tri.p[0][1], tri.p[2][2] - tri.p[0][2]};
					const float nx = e1[1] * e2[2] - e1[2] * e2[1], ny = e1[2] * e2[0] - e1[0] * e2[2], nz = e1[0] * e2[1] - e1[1] * e2[0];
					if (nx * nx + ny * ny + nz * nz < 1e-6f) continue;
					for (int k = 0; k < 3; k++) tri.c[k] = (tri.p[0][k] + tri.p[1][k] + tri.p[2][k]) / 3.0f;
					b.tris.push_back(tri);
				}
			}

			ZONETOOL_INFO("collision triangles: %llu, materials: %llu", static_cast<unsigned long long>(b.tris.size()), static_cast<unsigned long long>(mat_names.size()));
			if (b.tris.empty() || mat_names.empty()) mat_names.push_back("default");

			// The clipmap vertex pool is limited to 256 segments of 1024 verts, so keep only
			// the largest triangles (floors, walls, decks) when the map exceeds the budget.
			{
				constexpr size_t tri_budget = 255000;
				if (b.tris.size() > tri_budget)
				{
					auto area2 = [](const tri_t& t)
					{
						const float e1[3] = {t.p[1][0] - t.p[0][0], t.p[1][1] - t.p[0][1], t.p[1][2] - t.p[0][2]};
						const float e2[3] = {t.p[2][0] - t.p[0][0], t.p[2][1] - t.p[0][1], t.p[2][2] - t.p[0][2]};
						const float nx = e1[1] * e2[2] - e1[2] * e2[1], ny = e1[2] * e2[0] - e1[0] * e2[2], nz = e1[0] * e2[1] - e1[1] * e2[0];
						return nx * nx + ny * ny + nz * nz;
					};
					std::nth_element(b.tris.begin(), b.tris.begin() + tri_budget, b.tris.end(),
						[&](const tri_t& a, const tri_t& c) { return area2(a) > area2(c); });
					ZONETOOL_INFO("collision budget: dropping %llu smallest triangles", static_cast<unsigned long long>(b.tris.size() - tri_budget));
					b.tris.resize(tri_budget);
				}
			}

			std::vector<std::vector<unsigned int>> groups(mat_names.size());
			for (unsigned int i = 0; i < b.tris.size(); i++) groups[b.tris[i].mat].push_back(i);
			std::vector<unsigned int> used;
			for (unsigned int m = 0; m < groups.size(); m++) if (!groups[m].empty()) used.push_back(m);

			b.nodes.resize(1 + used.size());
			b.nodes[0].childCount = static_cast<unsigned short>(used.size());
			b.nodes[0].u.firstChildIndex = 1;
			float wmn[3] = {FLT_MAX, FLT_MAX, FLT_MAX}, wmx[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
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
				// Engine traversal keeps child ranges on a 63-entry stack: keep the fan-out at 2.
				unsigned int lvl_first = 1;
				auto lvl_count = static_cast<unsigned int>(used.size());
				while (lvl_count > 2)
				{
					const auto parents = (lvl_count + 1) / 2;
					const auto new_first = static_cast<unsigned int>(b.nodes.size());
					b.nodes.resize(b.nodes.size() + parents);
					for (unsigned int pi = 0; pi < parents; pi++)
					{
						const auto c0 = lvl_first + pi * 2;
						const auto cn = std::min(2u, lvl_count - pi * 2);
						float mn[3] = {FLT_MAX, FLT_MAX, FLT_MAX}, mx[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
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
				b.nodes[0].u.partitionIndex = 0;
				b.partitions.push_back(p);
				b.verts = {0, 0, 0};
				b.tri_indices = {};
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
				info->materials[i].surfaceFlags = static_cast<int>(surface_type_for(mat_names[i]) << 20);
				info->materials[i].contents = 1;
			}

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

			const size_t walk_bytes = (((static_cast<size_t>(tri_count) + 2 * static_cast<size_t>(tri_count)) + 31) >> 3) & 0x1FFFFFFC;
			info->pCollisionData.triEdgeIsWalkable = allocator.allocate_array<unsigned char>(std::max<size_t>(walk_bytes, 4));
			std::memset(info->pCollisionData.triEdgeIsWalkable, 0xFF, std::max<size_t>(walk_bytes, 4));

			info->pCollisionData.borderCount = 0;
			info->pCollisionData.borders = nullptr;
			info->pCollisionData.partitionCount = static_cast<int>(b.partitions.size());
			info->pCollisionData.partitions = allocator.allocate_array<zonetool::h1::CollisionPartition>(b.partitions.size());
			std::memcpy(info->pCollisionData.partitions, b.partitions.data(), b.partitions.size() * sizeof(zonetool::h1::CollisionPartition));

			info->sCollisionData.numStaticModels = 0;
			info->sCollisionData.staticModelList = nullptr;

			h1_asset->pInfo = &h1_asset->info;

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

			h1_asset->numSubModels = 1;
			h1_asset->cmodels = allocator.allocate_array<zonetool::h1::cmodel_t>(1);
			h1_asset->cmodels[0].bounds = world;
			h1_asset->cmodels[0].radius = std::sqrt(world.halfSize[0] * world.halfSize[0] + world.halfSize[1] * world.halfSize[1] + world.halfSize[2] * world.halfSize[2]);
			h1_asset->cmodels[0].info = nullptr;
			h1_asset->cmodels[0].leaf = h1_asset->leafs[1];

			h1_asset->mapEnts = allocator.allocate<zonetool::h1::MapEnts>();
			h1_asset->mapEnts->name = h1_asset->name;

			h1_asset->stageCount = 1;
			h1_asset->stages = allocator.allocate_array<zonetool::h1::Stage>(1);
			h1_asset->stages[0].name = allocator.duplicate_string("unknown");
			h1_asset->stages[0].radiometricScale = 0.001f;
			h1_asset->checksum = 0;
			return h1_asset;
		}

		// ---- the visible world ----

		unsigned int category(std::uint32_t k)
		{
			if (k == K_OPAQUE || k == K_ALPHATEST) return 0;
			if (k == K_DECAL) return 1;
			return 2; // blend, glass
		}

		zonetool::h1::GfxWorld* make_gfxworld(const map_in& in, const std::vector<unsigned int>& order, utils::memory::allocator& allocator)
		{
			auto* w = allocator.allocate<zonetool::h1::GfxWorld>();
			const auto map_name = "maps/mp/" + in.name + ".d3dbsp";
			w->name = allocator.duplicate_string(map_name);
			w->baseName = allocator.duplicate_string(in.name);
			w->bspVersion = 111;

			const auto surface_count = static_cast<unsigned int>(order.size());
			w->surfaceCount = surface_count;

			// One plane, one node, one cell.
			w->planeCount = 1;
			w->nodeCount = 1;
			w->skyCount = 0;
			w->skies = nullptr;
			w->lastSunPrimaryLightIndex = 0;
			w->primaryLightCount = 1;
			w->primaryLightEnvCount = 2;
			w->sortKeyLitDecal = 7;
			w->sortKeyEffectDecal = 43;
			w->sortKeyTopDecal = 17;
			w->sortKeyEffectAuto = 53;
			w->sortKeyDistortion = 48;
			w->sortKeyHair = 18;
			w->sortKeyEffectBlend = 33;

			w->dpvsPlanes.cellCount = 1;
			w->dpvsPlanes.planes = allocator.allocate_array<zonetool::h1::cplane_s>(1);
			w->dpvsPlanes.planes[0].normal[2] = 1.0f;
			w->dpvsPlanes.planes[0].dist = 1.0e6f;
			w->dpvsPlanes.planes[0].type = 2;
			w->dpvsPlanes.nodes = allocator.allocate_array<unsigned short>(1);
			w->dpvsPlanes.sceneEntCellBits = allocator.allocate_array<unsigned int>(1 << 9);

			// The aabb tree: a root over the whole world; its children are 2048-unit grid cells.
			struct cell_t
			{
				std::vector<unsigned int> surfaces; // positions in `order`
				float mn[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
				float mx[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
			};
			std::unordered_map<unsigned long long, cell_t> grid;
			constexpr float CELL = 2048.0f;
			// Surfaces must be contiguous per cell: sort the output order by (category, cell).
			// (`order` is already sorted by category; the cell sort is stable inside it.)
			float wmn[3] = {FLT_MAX, FLT_MAX, FLT_MAX}, wmx[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
			for (const auto s : in.surfaces)
			{
				(void)s;
			}
			for (unsigned int p = 0; p < surface_count; p++)
			{
				const auto& s = in.surfaces[order[p]];
				for (int k = 0; k < 3; k++)
				{
					wmn[k] = std::min(wmn[k], s.mins[k]);
					wmx[k] = std::max(wmx[k], s.maxs[k]);
				}
			}
			if (surface_count == 0)
			{
				for (int k = 0; k < 3; k++) { wmn[k] = -16.0f; wmx[k] = 16.0f; }
			}
			zonetool::h1::Bounds world{};
			for (int k = 0; k < 3; k++)
			{
				world.midPoint[k] = (wmn[k] + wmx[k]) * 0.5f;
				world.halfSize[k] = (wmx[k] - wmn[k]) * 0.5f;
			}

			// Children: every run of up to 64 consecutive surfaces (they are already batched in space by the exporter).
			constexpr unsigned int RUN = 64;
			const auto child_count = surface_count == 0 ? 0u : (surface_count + RUN - 1) / RUN;
			(void)grid;
			(void)CELL;
			w->aabbTreeCounts = allocator.allocate_array<zonetool::h1::GfxCellTreeCount>(1);
			w->aabbTrees = allocator.allocate_array<zonetool::h1::GfxCellTree>(1);
			w->aabbTreeCounts[0].aabbTreeCount = static_cast<int>(1 + child_count);
			w->aabbTrees[0].aabbTree = allocator.allocate_array<zonetool::h1::GfxAabbTree>(1 + child_count);
			{
				auto& root = w->aabbTrees[0].aabbTree[0];
				std::memcpy(&root.bounds, &world, sizeof(world));
				root.startSurfIndex = 0;
				root.surfaceCount = 0;
				root.childCount = static_cast<unsigned short>(std::min<unsigned int>(child_count, 0xFFFF));
				root.childrenOffset = static_cast<int>(sizeof(zonetool::h1::GfxAabbTree));
				root.smodelIndexCount = 0;
				root.smodelIndexes = nullptr;
				for (unsigned int c = 0; c < child_count; c++)
				{
					auto& n = w->aabbTrees[0].aabbTree[1 + c];
					const auto first = c * RUN;
					const auto count = std::min(RUN, surface_count - first);
					float mn[3] = {FLT_MAX, FLT_MAX, FLT_MAX}, mx[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
					for (unsigned int p = first; p < first + count; p++)
					{
						const auto& s = in.surfaces[order[p]];
						for (int k = 0; k < 3; k++)
						{
							mn[k] = std::min(mn[k], s.mins[k]);
							mx[k] = std::max(mx[k], s.maxs[k]);
						}
					}
					for (int k = 0; k < 3; k++)
					{
						n.bounds.midPoint[k] = (mn[k] + mx[k]) * 0.5f;
						n.bounds.halfSize[k] = (mx[k] - mn[k]) * 0.5f;
					}
					n.startSurfIndex = static_cast<int>(first);
					n.surfaceCount = static_cast<unsigned short>(count);
					n.childCount = 0;
					n.childrenOffset = 0;
					n.smodelIndexCount = 0;
					n.smodelIndexes = nullptr;
				}
			}

			w->cells = allocator.allocate_array<zonetool::h1::GfxCell>(1);
			std::memcpy(&w->cells[0].bounds, &world, sizeof(world));
			w->cells[0].portalCount = 0;
			w->cells[0].portals = nullptr;
			w->cells[0].reflectionProbeCount = 0;
			w->cells[0].reflectionProbes = nullptr;
			w->cells[0].reflectionProbeReferenceCount = 0;
			w->cells[0].reflectionProbeReferences = nullptr;
			w->portalGroupCount = 0;
			w->portalGroup = nullptr;
			w->portalDistanceAnchorCount = 0;
			w->portalDistanceAnchorsAndCloseDistSquared = nullptr;

			// Reflection probe and lightmap placeholders (images the port script copies in).
			w->draw.reflectionProbeCount = 1;
			w->draw.reflectionProbes = allocator.allocate_array<zonetool::h1::GfxImage*>(1);
			w->draw.reflectionProbes[0] = allocator.allocate<zonetool::h1::GfxImage>();
			w->draw.reflectionProbes[0]->name = allocator.duplicate_string("*reflection_probe0");
			w->draw.reflectionProbeOrigins = allocator.allocate_array<zonetool::h1::GfxReflectionProbe>(1);
			w->draw.reflectionProbeTextures = allocator.allocate_array<zonetool::h1::GfxRawTexture>(1);
			w->draw.reflectionProbeReferenceCount = 0;
			w->draw.reflectionProbeReferenceOrigins = nullptr;
			w->draw.reflectionProbeReferences = nullptr;

			w->draw.lightmapCount = 1;
			w->draw.lightmaps = allocator.allocate_array<zonetool::h1::GfxLightmapArray>(1);
			w->draw.lightmaps[0].primary = allocator.allocate<zonetool::h1::GfxImage>();
			w->draw.lightmaps[0].primary->name = allocator.duplicate_string("*lightmap0_primary");
			w->draw.lightmaps[0].secondary = allocator.allocate<zonetool::h1::GfxImage>();
			w->draw.lightmaps[0].secondary->name = allocator.duplicate_string("*lightmap0_secondary");
			w->draw.lightmapPrimaryTextures = allocator.allocate_array<zonetool::h1::GfxRawTexture>(1);
			w->draw.lightmapSecondaryTextures = allocator.allocate_array<zonetool::h1::GfxRawTexture>(1);
			w->draw.lightmapOverridePrimary = nullptr;
			w->draw.lightmapOverrideSecondary = nullptr;
			w->draw.lightmapParameters.lightmapWidthPrimary = 1024;
			w->draw.lightmapParameters.lightmapHeightPrimary = 1024;
			w->draw.lightmapParameters.lightmapWidthSecondary = 512;
			w->draw.lightmapParameters.lightmapHeightSecondary = 512;
			w->draw.lightmapParameters.lightmapModelUnitsPerTexel = 8;
			w->draw.trisType = 0;

			// Vertices: the surfaces' ranges, concatenated in draw order; indices rebased per surface.
			std::vector<std::uint32_t> first_vertex(surface_count);
			std::uint32_t total_vertices = 0;
			for (unsigned int p = 0; p < surface_count; p++)
			{
				first_vertex[p] = total_vertices;
				total_vertices += in.surfaces[order[p]].vertex_count;
			}
			w->draw.vertexCount = total_vertices;
			w->draw.vd.vertices = allocator.allocate_array<zonetool::h1::GfxWorldVertex>(std::max<std::uint32_t>(total_vertices, 1));
			w->draw.vertexLayerDataSize = 0;
			w->draw.vld.data = allocator.allocate_array<unsigned char>(4);

			std::uint32_t total_indices = 0;
			for (unsigned int p = 0; p < surface_count; p++) total_indices += in.surfaces[order[p]].tri_count * 3;
			w->draw.indexCount = total_indices;
			w->draw.indices = allocator.allocate_array<unsigned short>(std::max<std::uint32_t>(total_indices, 1));

			std::uint32_t at_index = 0;
			for (unsigned int p = 0; p < surface_count; p++)
			{
				const auto& s = in.surfaces[order[p]];
				for (std::uint32_t k = 0; k < s.vertex_count; k++)
				{
					const auto& src = in.vertices[static_cast<size_t>(s.base_vertex) + k];
					auto& dst = w->draw.vd.vertices[first_vertex[p] + k];
					std::memcpy(dst.xyz, src.xyz, sizeof(dst.xyz));
					dst.binormalSign = src.tangent[3] < 0.0f ? -1.0f : 1.0f;
					for (int c = 0; c < 4; c++) dst.color.array[c] = src.rgba[c];
					dst.texCoord[0] = src.uv[0];
					dst.texCoord[1] = src.uv[1];
					dst.lmapCoord[0] = 0.5f;
					dst.lmapCoord[1] = 0.5f;
					float n[3] = {src.normal[0], src.normal[1], src.normal[2]};
					float t[3] = {src.tangent[0], src.tangent[1], src.tangent[2]};
					dst.normal.packed = zonetool::PackedVec::Vec3PackUnitVec_H1(n);
					dst.tangent.packed = zonetool::PackedVec::Vec3PackUnitVec_H1(t);
				}
				for (std::uint32_t k = 0; k < s.tri_count * 3; k++)
				{
					const auto ii = s.first_index + k;
					w->draw.indices[at_index + k] = ii < in.indices.size() ? in.indices[ii] : 0;
				}
				at_index += s.tri_count * 3;
			}
			w->draw.displacementParmsCount = 0;
			w->draw.displacementParms = nullptr;

			// Light grid: flat, one node (the IW7 converter's stub).
			w->lightGrid.hasLightRegions = false;
			w->lightGrid.useSkyForLowZ = false;
			w->lightGrid.lastSunPrimaryLightIndex = 0;
			w->lightGrid.colorCount = 1;
			w->lightGrid.colors = allocator.allocate_array<zonetool::h1::GfxLightGridColors>(1);
			w->lightGrid.entryCount = 1;
			w->lightGrid.entries = allocator.allocate_array<zonetool::h1::GfxLightGridEntry>(1);
			w->lightGrid.rawRowDataSize = 4;
			w->lightGrid.rawRowData = allocator.allocate_array<unsigned char>(4);
			w->lightGrid.rowDataStart = allocator.allocate_array<unsigned short>(1);
			w->lightGrid.missingGridColorIndex = 0;
			w->lightGrid.tableVersion = 0;
			w->lightGrid.paletteVersion = 0;
			w->lightGrid.stageCount = 0;
			w->lightGrid.stageLightingContrastGain = nullptr;
			for (unsigned int i = 0; i < 56; i++)
			{
				for (unsigned int j = 0; j < 3; j++)
				{
					w->lightGrid.colors[0].rgb[i][j] = 0x3800;
					w->lightGrid.defaultLightGridColors.rgb[i][j] = zonetool::half_float::float_to_half(0.5f);
					w->lightGrid.skyLightGridColors.rgb[i][j] = zonetool::half_float::float_to_half(0.5f);
				}
			}
			for (auto i = 0; i < 3; i++)
			{
				auto& tree = w->lightGrid.tree[i];
				tree.index = static_cast<unsigned char>(i);
				tree.maxDepth = 0;
				tree.unused[0] = 0;
				tree.nodeCount = 1;
				tree.leafCount = 1;
				tree.p_nodeTable = allocator.allocate_array<unsigned int>(1);
				tree.leafTableSize = 4;
				tree.p_leafTable = allocator.allocate_array<unsigned char>(4);
			}

			// One brush model (the world).
			w->modelCount = 1;
			w->models = allocator.allocate_array<zonetool::h1::GfxBrushModel>(1);
			std::memcpy(&w->models[0].bounds, &world, sizeof(world));
			w->models[0].radius = std::sqrt(world.halfSize[0] * world.halfSize[0] + world.halfSize[1] * world.halfSize[1] + world.halfSize[2] * world.halfSize[2]);
			w->models[0].startSurfIndex = 0;
			w->models[0].surfaceCount = static_cast<unsigned short>(std::min<unsigned int>(surface_count, 0xFFFF));
			w->models[0].mdaoVolumeIndex = -1;

			std::memcpy(&w->bounds, &world, sizeof(world));
			std::memcpy(&w->shadowBounds, &world, sizeof(world));
			w->checksum = 0;
			w->materialMemoryCount = 0;
			w->materialMemory = nullptr;
			for (int i = 0; i < 4; i++) w->outdoorLookupMatrix[i][i] = 1.0f;
			w->outdoorImage = nullptr;

			// Runtime-block bit arrays the engine memsets at load.
			w->cellCasterBits = allocator.allocate_array<unsigned int>(1);
			w->cellHasSunLitSurfsBits = allocator.allocate_array<unsigned int>(1);
			w->sceneDynModel = nullptr;
			w->sceneDynBrush = nullptr;
			w->primaryLightEntityShadowVis = allocator.allocate_array<unsigned int>(0x800);
			w->primaryLightDynEntShadowVis[0] = nullptr;
			w->primaryLightDynEntShadowVis[1] = nullptr;
			w->nonSunPrimaryLightForModelDynEnt = nullptr;
			w->shadowGeom = allocator.allocate_array<zonetool::h1::GfxShadowGeometry>(1);
			w->shadowGeomOptimized = allocator.allocate_array<zonetool::h1::GfxShadowGeometry>(1);
			w->lightRegion = allocator.allocate_array<zonetool::h1::GfxLightRegion>(1);

			// dpvs
			{
				auto& d = w->dpvs;
				d.smodelCount = 0;
				d.subdivVertexLightingInfoCount = 0;
				d.subdivVertexLighting = nullptr;
				d.staticSurfaceCount = surface_count;

				// Category ranges (opaque, decal, translucent): `order` is sorted by category.
				unsigned int b0 = 0, b1 = 0, b2 = 0, e2 = surface_count;
				for (unsigned int p = 0; p < surface_count; p++)
				{
					const auto c = category(in.materials[in.surfaces[order[p]].material].kind);
					if (c == 0) b1 = p + 1;
					if (c <= 1) b2 = p + 1;
				}
				d.litOpaqueSurfsBegin = b0;
				d.litOpaqueSurfsEnd = b1;
				d.unkSurfsBegin = 0;
				d.unkSurfsEnd = 0;
				d.litDecalSurfsBegin = b1;
				d.litDecalSurfsEnd = b2;
				d.litTransSurfsBegin = b2;
				d.litTransSurfsEnd = e2;
				d.shadowCasterSurfsBegin = 0;
				d.shadowCasterSurfsEnd = 0;
				d.emissiveSurfsBegin = 0;
				d.emissiveSurfsEnd = 0;

				const auto vis_words = std::max<unsigned int>((surface_count + 31) >> 5, 1);
				d.smodelVisDataCount = 1;
				d.surfaceVisDataCount = vis_words;
				for (auto i = 0; i < 4; i++)
				{
					d.smodelVisData[i] = allocator.allocate_array<unsigned int>(1);
					d.surfaceVisData[i] = allocator.allocate_array<unsigned int>(vis_words);
					d.smodelUmbraVisData[i] = allocator.allocate_array<unsigned int>(1);
					d.surfaceUmbraVisData[i] = allocator.allocate_array<unsigned int>(vis_words);
				}
				for (auto i = 0; i < 27; i++)
				{
					d.smodelUnknownVisData[i] = allocator.allocate_array<unsigned int>(1);
					d.surfaceUnknownVisData[i] = allocator.allocate_array<unsigned int>(vis_words);
				}
				d.unknownSModelVisData1 = allocator.allocate_array<unsigned int>(1);
				d.unknownSModelVisData2 = allocator.allocate_array<unsigned int>(2);
				d.lodData = nullptr;
				d.tessellationCutoffVisData = allocator.allocate_array<unsigned int>(vis_words);

				d.sortedSurfIndex = allocator.allocate_array<unsigned int>(std::max<unsigned int>(surface_count, 1));
				for (unsigned int p = 0; p < surface_count; p++) d.sortedSurfIndex[p] = p;
				d.smodelInsts = nullptr;

				d.surfaces = allocator.allocate_array<zonetool::h1::GfxSurface>(std::max<unsigned int>(surface_count, 1));
				d.surfacesBounds = allocator.allocate_array<zonetool::h1::GfxSurfaceBounds>(std::max<unsigned int>(surface_count, 1));
				d.surfaceMaterials = allocator.allocate_array<zonetool::h1::GfxDrawSurf>(std::max<unsigned int>(surface_count, 1));

				std::unordered_map<std::uint32_t, zonetool::h1::Material*> material_ptrs;
				std::unordered_map<std::uint32_t, unsigned int> material_index;
				for (unsigned int p = 0; p < surface_count; p++)
				{
					const auto& s = in.surfaces[order[p]];
					auto& dst = d.surfaces[p];
					dst.tris.vertexLayerData = 0;
					dst.tris.firstVertex = first_vertex[p];
					dst.tris.maxEdgeLength = 0.0f;
					dst.tris.unk = -1;
					dst.tris.vertexCount = static_cast<unsigned short>(s.vertex_count);
					dst.tris.triCount = static_cast<unsigned short>(s.tri_count);
					// baseIndex: the surface's slice of the rebased index array
					std::uint32_t base = 0;
					(void)base;

					auto& mp = material_ptrs[s.material];
					if (!mp)
					{
						mp = allocator.allocate<zonetool::h1::Material>();
						mp->name = allocator.duplicate_string(in.materials[s.material].h1_name);
					}
					dst.material = mp;
					dst.laf.fields.lightmapIndex = 0;
					dst.laf.fields.reflectionProbeIndex = 0;
					dst.laf.fields.primaryLightEnvIndex = 1; // env 1 = the sun
					dst.laf.fields.flags = 0;

					std::memcpy(&d.surfacesBounds[p].bounds.midPoint, &world.midPoint, 0); // set below
					for (int k = 0; k < 3; k++)
					{
						d.surfacesBounds[p].bounds.midPoint[k] = (s.mins[k] + s.maxs[k]) * 0.5f;
						d.surfacesBounds[p].bounds.halfSize[k] = (s.maxs[k] - s.mins[k]) * 0.5f;
					}

					const auto idx = material_index.emplace(s.material, static_cast<unsigned int>(material_index.size())).first->second;
					std::memset(&d.surfaceMaterials[p], 0, sizeof(d.surfaceMaterials[p]));
					d.surfaceMaterials[p].fields.objectId = static_cast<unsigned __int64>(p) & 0xFFFF;
					d.surfaceMaterials[p].fields.materialSortedIndex = static_cast<unsigned __int64>(idx) & 0x3FFF;
					d.surfaceMaterials[p].fields.primarySortKey = in.materials[s.material].kind == K_OPAQUE ? 2u : 31u;
					d.surfaceMaterials[p].fields.tessellation = 0;
				}
				// baseIndex per surface
				{
					std::uint32_t at = 0;
					for (unsigned int p = 0; p < surface_count; p++)
					{
						d.surfaces[p].tris.baseIndex = at;
						at += in.surfaces[order[p]].tri_count * 3;
					}
				}

				d.surfaceCastsSunShadow = allocator.allocate_array<unsigned int>(vis_words);
				d.sunShadowOptCount = 0;
				d.sunSurfVisDataCount = 0;
				d.surfaceCastsSunShadowOpt = nullptr;
				d.smodelDrawInsts = nullptr;
				d.smodelLightingInsts = nullptr;
				d.surfaceDeptAndSurf = allocator.allocate_array<zonetool::h1::GfxDepthAndSurf>(std::max<unsigned int>(surface_count, 1));
				d.constantBuffersLit = nullptr;
				d.constantBuffersAmbient = nullptr;
				d.usageCount = 0;
			}

			for (int i = 0; i < 2; i++)
			{
				w->dpvsDyn.dynEntClientWordCount[i] = 0;
				w->dpvsDyn.dynEntClientCount[i] = 0;
				w->dpvsDyn.dynEntCellBits[i] = nullptr;
				for (int j = 0; j < 4; j++) w->dpvsDyn.dynEntVisData[i][j] = allocator.allocate_array<unsigned char>(4);
			}

			w->mapVtxChecksum = 0;
			w->heroOnlyLightCount = 0;
			w->heroOnlyLights = nullptr;
			w->fogTypesAllowed = zonetool::h1::FOG_NORMAL | zonetool::h1::FOG_DFOG;
			w->umbraTomeSize = 0;
			w->umbraTomeData = nullptr;
			std::memset(&w->umbraTomePtr, 0, sizeof(w->umbraTomePtr));
			w->mdaoVolumesCount = 0;
			w->mdaoVolumes = nullptr;
			w->useLightGridDefaultModelLightingLookup = true;
			w->useLightGridDefaultFXLightingLookup = true;
			std::memset(&w->lightGridDefaultModelLightingLookup, 0, sizeof(w->lightGridDefaultModelLightingLookup));
			std::memset(&w->lightGridDefaultFXLightingLookup, 0, sizeof(w->lightGridDefaultFXLightingLookup));
			w->buildInfo.bspCommandline = nullptr;
			w->buildInfo.lightCommandline = nullptr;
			w->buildInfo.bspTimestamp = nullptr;
			w->buildInfo.lightTimestamp = nullptr;
			return w;
		}

		zonetool::h1::ComWorld* make_comworld(const map_in& in, utils::memory::allocator& allocator)
		{
			auto* c = allocator.allocate<zonetool::h1::ComWorld>();
			c->name = allocator.duplicate_string("maps/mp/" + in.name + ".d3dbsp");
			c->isInUse = 0;
			c->primaryLightCount = 1;
			c->primaryLights = allocator.allocate_array<zonetool::h1::ComPrimaryLight>(1);
			auto& l = c->primaryLights[0];
			l.type = zonetool::h1::GFX_LIGHT_TYPE_DIR;
			l.canUseShadowMap = 1;
			l.physicallyBased = 0;
			l.exponent = 0;
			l.lightingState = 0;
			std::memcpy(l.color, in.sun_colour, sizeof(l.color));
			// TODO verify the direction convention against a working H1 map (toward the sun, as stored by Black Ops II).
			std::memcpy(l.dir, in.sun_toward, sizeof(l.dir));
			l.up[2] = 1.0f;
			l.cosHalfFovOuter = 0.0f;
			l.cosHalfFovInner = 0.0f;
			l.cosHalfFovExpanded = 0.0f;
			l.cucScaleVector[0] = l.cucScaleVector[1] = 1.0f;
			l.defName = nullptr;

			c->primaryLightEnvCount = 2;
			c->primaryLightEnvs = allocator.allocate_array<zonetool::h1::ComPrimaryLightEnv>(2);
			c->primaryLightEnvs[0].numIndices = 0;
			c->primaryLightEnvs[1].numIndices = 1;
			c->primaryLightEnvs[1].primaryLightIndices[0] = 0;
			return c;
		}

		zonetool::h1::MapEnts* make_mapents(const std::string& dir, const std::string& name, utils::memory::allocator& allocator)
		{
			auto* e = allocator.allocate<zonetool::h1::MapEnts>();
			e->name = allocator.duplicate_string("maps/mp/" + name + ".d3dbsp");
			std::string text = "{\n\"classname\" \"worldspawn\"\n}\n";
			std::ifstream f(dir + name + ".ents", std::ios::binary);
			if (f)
			{
				text.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
			}
			else
			{
				ZONETOOL_WARNING("no %s.ents next to the map: worldspawn only", name.c_str());
			}
			e->entityString = allocator.duplicate_string(text);
			e->numEntityChars = static_cast<int>(text.size());
			e->clientTrigger.trigger.count = 0;
			e->clientTrigger.numClientTriggerNodes = 0;
			e->splineList.splineCount = 0;
			e->spawnList.spawnsCount = 0;
			return e;
		}
	}

	bool import_bo2_map(const std::string& h1map_path, const std::string& name_override)
	{
		map_in in;
		if (!read_map(h1map_path, in))
		{
			ZONETOOL_ERROR("importmap: cannot read %s (not a BO2H1M01 file)", h1map_path.c_str());
			return false;
		}
		if (!name_override.empty()) in.name = name_override;
		{
			const auto sl = h1map_path.find_last_of("/\\");
			g_src_dir = sl == std::string::npos ? "" : h1map_path.substr(0, sl + 1);
			const auto dot = h1map_path.find_last_of('.');
			const auto stem_start = sl == std::string::npos ? 0 : sl + 1;
			const auto stem = h1map_path.substr(stem_start, (dot == std::string::npos || dot < stem_start ? h1map_path.size() : dot) - stem_start);
			g_copied.clear();
			read_mats_tsv(g_src_dir + stem + ".mats.tsv", in);
		}

		// Drawn surfaces only (no sky, tool or water), sorted by category (opaque, decal, translucent).
		std::vector<unsigned int> order;
		for (unsigned int i = 0; i < in.surfaces.size(); i++)
		{
			const auto& s = in.surfaces[i];
			if (s.material >= in.materials.size()) continue;
			const auto k = in.materials[s.material].kind;
			if (k == K_SKIP || k == K_WATER) continue;
			if (s.tri_count == 0 || s.vertex_count == 0 || s.vertex_count > 0xFFFF || s.tri_count > 0xFFFF) continue;
			order.push_back(i);
		}
		std::stable_sort(order.begin(), order.end(), [&](unsigned int a, unsigned int b)
		{
			return category(in.materials[in.surfaces[a].material].kind) < category(in.materials[in.surfaces[b].material].kind);
		});
		ZONETOOL_INFO("importmap %s: %llu of %llu surfaces drawn, %llu vertices, %llu materials",
			in.name.c_str(), static_cast<unsigned long long>(order.size()), static_cast<unsigned long long>(in.surfaces.size()),
			static_cast<unsigned long long>(in.vertices.size()), static_cast<unsigned long long>(in.materials.size()));

		// One placeholder material per Black Ops II material drawn.
		std::vector<bool> used(in.materials.size(), false);
		for (const auto i : order) used[in.surfaces[i].material] = true;
		{
			// The csv lines the port script adds to zone_source/<map>.csv.
			std::string list;
			for (size_t m = 0; m < in.materials.size(); m++)
			{
				if (!used[m]) continue;
				write_material(in.materials[m]);
				list += "material," + in.materials[m].h1_name + "\n";
			}
			auto file = zonetool::filesystem::file("maps\\mp\\" + in.name + ".materials.csv");
			file.open("wb");
			file.write(list);
			file.close();
		}

		utils::memory::allocator allocator;

		const auto world = make_gfxworld(in, order, allocator);
		zonetool::h1::gfx_world::dump(world);

		const auto clip = make_clipmap(in, order, allocator);
		zonetool::h1::clip_map::dump(clip);

		const auto pw = zonetool::iw7::converter::h1::physworld::generate(clip, allocator);
		zonetool::h1::phys_world::dump(pw);

		const auto com = make_comworld(in, allocator);
		zonetool::h1::com_world::dump(com);

		const auto fx = allocator.allocate<zonetool::h1::FxWorld>();
		fx->name = world->name;
		zonetool::h1::fx_world::dump(fx);

		const auto glass = allocator.allocate<zonetool::h1::GlassWorld>();
		glass->name = world->name;
		glass->g_glassData = allocator.allocate<zonetool::h1::G_GlassData>();
		zonetool::h1::glass_world::dump(glass);

		const auto slash = h1map_path.find_last_of("/\\");
		const std::string dir = slash == std::string::npos ? "" : h1map_path.substr(0, slash + 1);
		const auto ents = make_mapents(dir, in.name, allocator);
		zonetool::h1::map_ents::dump(ents);

		ZONETOOL_INFO("importmap %s: done", in.name.c_str());
		return true;
	}
}
