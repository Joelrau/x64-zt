#include <std_include.hpp>

#include "../include.hpp"
#include "gfxworld.hpp"
#include "comworld.hpp"
#include "gfximage.hpp"
#include "material.hpp"

#include <formats/light_grid.hpp>
#include <formats/probe_volume.hpp>

#include <bit>

namespace zonetool::h1::converter::s2::gfxworld
{
	namespace
	{
		constexpr float light_grid_cell_size[3] = {32.0f, 32.0f, 64.0f};
		constexpr int light_grid_bias[3] = {4096, 4096, 2048};

		// the same in every stock S2 map
		constexpr zonetool::s2::GfxWorldSortKeys stock_sort_keys
		{
			.unk_0x1 = 38,
			.unk_0x2 = 39,
			.twoD = 62,
			.unk_0x4 = 44,
			.effectDecal = 21,
			.unk_0x6 = 46,
			.effectAuto = 54,
			.distortion = 49,
			.transBegin = 22,
			.transEnd = 33,
			.emissiveBegin = 47,
			.emissiveEnd = 58,
			.postResolveLit = 38,
		};

		GfxWorld* pending_world = nullptr;

		template <typename T>
		T* non_null_array(const std::size_t count, utils::memory::allocator& allocator)
		{
			return allocator.allocate_array<T>(std::max<std::size_t>(count, 1));
		}

		template <typename T>
		std::unordered_map<std::string, T*>& stubs()
		{
			static std::unordered_map<std::string, T*> stubs;
			return stubs;
		}

		template <typename T>
		T* shared_stub(const char* name, utils::memory::allocator& allocator)
		{
			auto& stub = stubs<T>()[name];
			if (!stub)
			{
				stub = make_stub<T>(name, allocator);
			}

			return stub;
		}

		void clear_stubs()
		{
			stubs<zonetool::s2::Material>().clear();
			stubs<zonetool::s2::XModel>().clear();
		}

		// h1 cullDist is world units (lod out * scale * lod scale), s2 2.20 sub_140864F70/sub_1408661B0 compare it with distance / maxScale (no lodScale)
		unsigned short convert_cull_dist(const GfxStaticModelDrawInst& inst, const zonetool::s2::GfxStaticModelDrawInst& new_inst)
		{
			const auto scale = new_inst.maxScale;
			auto dist = inst.cullDist && scale > 0.0f ? inst.cullDist / scale : 0.0f;
			if (!dist && inst.model && inst.model->numLods > 0 && inst.model->numLods <= 6)
			{
				dist = inst.model->lodInfo[inst.model->numLods - 1].dist * new_inst.lodScale;
			}

			return static_cast<unsigned short>(std::clamp(std::round(dist), 0.0f, 65535.0f));
		}

		const Bounds& smodel_bounds(const GfxStaticModelInst& inst)
		{
			return inst.bounds;
		}

		zonetool::s2::GfxImage* image_stub(const GfxImage* image, utils::memory::allocator& allocator)
		{
			if (!image || !image->name)
			{
				return nullptr;
			}

			if (!gfximage::dump_used(image->name))
			{
				ZONETOOL_WARNING("GfxWorld image \"%s\" is not converted", image->name);
			}

			return make_stub<zonetool::s2::GfxImage>(image->name, allocator);
		}

		std::vector<std::uint16_t> env_light_indices(const ComWorld& com_world, const ComPrimaryLightEnv& env)
		{
			std::vector<std::uint16_t> lights;
			for (auto i = 0u; i < std::min<std::uint32_t>(env.numIndices, 4); i++)
			{
				if (env.primaryLightIndices[i] < com_world.primaryLightCount)
				{
					lights.push_back(env.primaryLightIndices[i]);
				}
			}
			return lights;
		}

		class light_lists
		{
		public:
			explicit light_lists(const ComWorld* com_world)
			{
				this->data_.push_back(0);
				if (!com_world)
				{
					return;
				}

				for (auto i = 0u; i < com_world->primaryLightEnvCount; i++)
				{
					const auto lights = env_light_indices(*com_world, com_world->primaryLightEnvs[i]);
					if (lights.empty())
					{
						this->offsets_.push_back(0);
						continue;
					}

					this->offsets_.push_back(static_cast<std::uint16_t>(this->data_.size()));
					this->data_.push_back(static_cast<std::uint16_t>(lights.size()));
					this->data_.insert(this->data_.end(), lights.begin(), lights.end());
				}
			}

			std::uint16_t offset(const std::uint32_t env) const
			{
				return env < this->offsets_.size() ? this->offsets_[env] : 0;
			}

			const std::vector<std::uint16_t>& data() const
			{
				return this->data_;
			}

		private:
			std::vector<std::uint16_t> data_;
			std::vector<std::uint16_t> offsets_;
		};

		std::string transient_zone_name(const GfxWorld* asset)
		{
			return std::format("{}_000", asset->baseName);
		}

		zonetool::s2::GfxMapTrZone* convert_transient_zone(const GfxWorld* asset, const char* name, utils::memory::allocator& allocator)
		{
			const auto zone = allocator.allocate<zonetool::s2::GfxMapTrZone>();
			const auto count = asset->draw.vertexCount;
			zone->name = name;
			zone->vertexCount = count;

			auto& data = zone->vertices.data;
			data.positions = allocator.allocate_array<zonetool::s2::vec3_t>(count);
			data.texelDensities = allocator.allocate_array<float>(count);
			data.colors = allocator.allocate_array<unsigned int>(count);
			data.texCoords = allocator.allocate_array<zonetool::s2::vec2_t>(count);
			data.lmapCoords = allocator.allocate_array<zonetool::s2::vec2_t>(count);
			data.normals = allocator.allocate_array<unsigned int>(count);
			data.tangents = allocator.allocate_array<unsigned int>(count);
			for (auto i = 0u; i < count; i++)
			{
				const auto& vertex = asset->draw.vd.vertices[i];
				std::memcpy(data.positions[i], vertex.xyz, sizeof(vertex.xyz));
				data.texelDensities[i] = std::abs(vertex.binormalSign);
				data.colors[i] = vertex.color.packed;
				std::memcpy(data.texCoords[i], vertex.texCoord, sizeof(vertex.texCoord));
				std::memcpy(data.lmapCoords[i], vertex.lmapCoord, sizeof(vertex.lmapCoord));
				data.normals[i] = vertex.normal.packed;
				data.tangents[i] = pack_tangent(vertex.tangent.packed, vertex.binormalSign); // h1 binormalSign is the signed texel density
			}

			zone->vertexLayerDataSize = asset->draw.vertexLayerDataSize;
			zone->vld.data = asset->draw.vld.data;
			return zone;
		}

		void convert_header(GfxWorld* asset, zonetool::s2::GfxWorld& world, utils::memory::allocator& allocator)
		{
			world.name = asset->name;
			world.baseName = asset->baseName;
			world.bspVersion = 204;
			world.planeCount = asset->planeCount;
			world.nodeCount = asset->nodeCount;
			world.surfaceCount = asset->surfaceCount;
			world.portalGroupCount = asset->portalGroupCount;
			world.lastSunPrimaryLightIndex = 1;
			world.primaryLightCount = asset->primaryLightCount;
			world.movingScriptablePrimaryLightCount = 0; // h1 has no moving scriptable lights
			world.unk_0x40 = asset->primaryLightCount;
			world.sortKeys = stock_sort_keys;

			const auto skies = copy_array(retype<zonetool::s2::GfxSky>(asset->skies), asset->skyCount, allocator);
			for (auto i = 0; skies && i < asset->skyCount; i++)
			{
				skies[i].skyImage = image_stub(asset->skies[i].skyImage, allocator);
			}
			world.skyCount = asset->skyCount;
			world.skies = skies;
		}

		void convert_cells(GfxWorld* asset, zonetool::s2::GfxWorld& world, utils::memory::allocator& allocator)
		{
			const auto cell_count = static_cast<unsigned int>(asset->dpvsPlanes.cellCount);
			world.dpvsPlanes.cellCount = cell_count;
			world.dpvsPlanes.planes = retype<zonetool::s2::cplane_s>(asset->dpvsPlanes.planes);

			world.dpvsPlanes.nodes = asset->nodeCount ? allocator.allocate_array<unsigned int>(asset->nodeCount) : nullptr;
			for (auto i = 0; asset->dpvsPlanes.nodes && i < asset->nodeCount; i++)
			{
				world.dpvsPlanes.nodes[i] = asset->dpvsPlanes.nodes[i];
			}
			mark_runtime(world.dpvsPlanes.sceneEntCellBits, allocator);
			mark_runtime(world.dpvsPlanes.unk_0x20, allocator);

			world.aabbTreeCounts = retype<zonetool::s2::GfxCellTreeCount>(asset->aabbTreeCounts);
			world.aabbTrees = retype<zonetool::s2::GfxCellTree>(asset->aabbTrees);
			world.cellTransientInfos = cell_count ? allocator.allocate_array<zonetool::s2::GfxCellTransientInfo>(cell_count) : nullptr;
			world.cells = retype<zonetool::s2::GfxCell>(asset->cells);

			const auto groups = copy_array(retype<zonetool::s2::GfxPortalGroup>(asset->portalGroup), asset->portalGroupCount, allocator);
			for (auto i = 0u; groups && i < asset->portalGroupCount; i++)
			{
				groups[i].targetNameIndex = 0;
			}
			world.portalGroup = groups;
			world.portalDistanceAnchorCount = asset->portalDistanceAnchorCount;
			world.portalDistanceAnchorsAndCloseDistSquared = retype<zonetool::s2::vec4_t>(asset->portalDistanceAnchorsAndCloseDistSquared);
		}

		// stock lists only the tiles its lightmap charts use, every tile of every lightmap is listed here
		void convert_lightmap_tiles(const GfxWorldDraw& source, zonetool::s2::GfxWorldDraw& draw)
		{
			const auto& params = source.lightmapParameters;
			const auto tile_width = std::max(params.lightmapWidthSecondary, 1);
			const auto tile_height = std::max(params.lightmapHeightSecondary, 1);
			const auto tiles_x = (params.lightmapWidthPrimary + tile_width - 1) / tile_width;
			const auto tiles_y = (params.lightmapHeightPrimary + tile_height - 1) / tile_height;
			for (auto layer = 0; layer < source.lightmapCount; layer++)
			{
				for (auto y = 0; y < tiles_y; y++)
				{
					for (auto x = 0; x < tiles_x && draw.lightmapTileCount < std::size(draw.lightmapTiles); x++)
					{
						auto& tile = draw.lightmapTiles[draw.lightmapTileCount++];
						tile[0] = static_cast<unsigned int>(x);
						tile[1] = static_cast<unsigned int>(y);
						tile[2] = static_cast<unsigned int>(layer);
					}
				}
			}

			draw.unk_0x264[0] = 1;
			draw.unk_0x264[1] = 1;
			draw.lightmapTileSize[0] = static_cast<unsigned int>(tile_width);
			draw.lightmapTileSize[1] = static_cast<unsigned int>(tile_height);
			draw.unk_0x274 = 8;
		}

		void convert_draw(GfxWorld* asset, zonetool::s2::GfxWorld& world, const char* transient_zone, utils::memory::allocator& allocator)
		{
			auto& draw = world.draw;
			const auto& source = asset->draw;

			draw.reflectionProbeCount = source.reflectionProbeCount;
			draw.reflectionProbes = source.reflectionProbeCount ? allocator.allocate_array<zonetool::s2::GfxImage*>(source.reflectionProbeCount) : nullptr;
			for (auto i = 0u; draw.reflectionProbes && i < source.reflectionProbeCount; i++)
			{
				draw.reflectionProbes[i] = image_stub(source.reflectionProbes[i], allocator);
			}

			if (draw.reflectionProbes && source.reflectionProbeCount > 1)
			{
				draw.reflectionProbes[0] = draw.reflectionProbes[1]; // probe 0 is default red
			}
			reserve_runtime(draw.reflectionProbeTextures, draw.reflectionProbeCount, allocator);

			draw.reflectionProbeOrigins = retype<zonetool::s2::GfxReflectionProbe>(source.reflectionProbeOrigins);
			draw.reflectionProbeReferenceCount = source.reflectionProbeReferenceCount;
			draw.reflectionProbeReferenceOrigins = retype<zonetool::s2::GfxReflectionProbeReferenceOrigin>(source.reflectionProbeReferenceOrigins);
			draw.reflectionProbeReferences = retype<zonetool::s2::GfxReflectionProbeReference>(source.reflectionProbeReferences);

			const auto fog_spline = gfximage::dump_fog_spline("*fog_spline0");
			draw.fogSplineCount = 1; // fogSplineImages[0] 0x1402044C0
			draw.fogSplineImages = allocator.allocate_array<zonetool::s2::GfxImage*>(1);
			draw.fogSplineImages[0] = make_stub<zonetool::s2::GfxImage>(allocator.duplicate_string(fog_spline), allocator);
			draw.fogSplineInfo = allocator.allocate_array<char[116]>(1);

			const auto ies_lookup = gfximage::dump_ies_lookup("*ieslookup");
			draw.iesLookup = make_stub<zonetool::s2::GfxImage>(allocator.duplicate_string(ies_lookup), allocator);

			const auto lightmap_count = static_cast<unsigned int>(std::max(source.lightmapCount, 0));
			draw.lightmapOverrideCount = lightmap_count;
			draw.lightmapOverrides = lightmap_count ? allocator.allocate_array<zonetool::s2::GfxLightmapArray>(lightmap_count) : nullptr;
			for (auto i = 0u; draw.lightmapOverrides && i < lightmap_count; i++)
			{
				draw.lightmapOverrides[i].primary = image_stub(source.lightmaps[i].primary, allocator);
				draw.lightmapOverrides[i].secondary = image_stub(source.lightmaps[i].secondary, allocator);
			}
			reserve_runtime(draw.lightmapPrimaryTextures, lightmap_count, allocator);
			reserve_runtime(draw.lightmapSecondaryTextures, lightmap_count, allocator);

			draw.lightmapParameters = std::bit_cast<zonetool::s2::GfxLightmapParameters>(source.lightmapParameters);
			convert_lightmap_tiles(source, draw);

			draw.transientZoneCount = 1;
			draw.transientZones[0] = make_stub<zonetool::s2::GfxMapTrZone>(transient_zone, allocator);

			draw.indexCount = source.indexCount;
			draw.indices = source.indices;
		}

		// S2 lights the world with the probe volume, every stock map carries this empty grid
		void convert_light_grid(zonetool::s2::GfxWorld& world, utils::memory::allocator& allocator)
		{
			auto& grid = world.lightGrid;
			grid.colAxis = 1;
			grid.rowDataStart = allocator.allocate<unsigned short>(); // one row, mins == maxs
			grid.tableVersion = 1;
			grid.paletteVersion = 1;
		}

		using vec3 = std::array<float, 3>;

		struct light_proxy
		{
			std::vector<vec3> vertices;
			std::vector<std::uint16_t> indices;
		};

		vec3 cross(const vec3& a, const vec3& b)
		{
			return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
		}

		float dot(const vec3& a, const vec3& b)
		{
			return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
		}

		vec3 sub(const vec3& a, const vec3& b)
		{
			return {a[0] - b[0], a[1] - b[1], a[2] - b[2]};
		}

		vec3 normalize(const vec3& v)
		{
			const auto length = std::sqrt(dot(v, v));
			return length > 0.0f ? vec3{v[0] / length, v[1] / length, v[2] / length} : vec3{0.0f, 0.0f, 1.0f};
		}

		// icosahedron split twice (162 vertices), scaled so that every face lies outside the light sphere
		light_proxy make_omni_proxy(const vec3& origin, const float radius)
		{
			constexpr auto phi = std::numbers::phi_v<float>;
			std::vector<vec3> points
			{
				{-1, phi, 0}, {1, phi, 0}, {-1, -phi, 0}, {1, -phi, 0}, {0, -1, phi}, {0, 1, phi},
				{0, -1, -phi}, {0, 1, -phi}, {phi, 0, -1}, {phi, 0, 1}, {-phi, 0, -1}, {-phi, 0, 1},
			};
			const std::vector<std::uint16_t> faces
			{
				0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11, 1, 5, 9, 5, 11, 4, 11, 10, 2, 10, 7, 6, 7, 1, 8,
				3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3, 8, 9, 4, 9, 5, 2, 4, 11, 6, 2, 10, 8, 6, 7, 9, 8, 1,
			};

			for (auto& point : points)
			{
				point = normalize(point);
			}

			std::map<std::pair<std::uint16_t, std::uint16_t>, std::uint16_t> midpoints;
			const auto midpoint = [&](const std::uint16_t a, const std::uint16_t b)
			{
				const auto key = std::minmax(a, b);
				if (const auto entry = midpoints.find(key); entry != midpoints.end())
				{
					return entry->second;
				}

				const auto index = static_cast<std::uint16_t>(points.size());
				points.emplace_back(normalize({points[a][0] + points[b][0], points[a][1] + points[b][1], points[a][2] + points[b][2]}));
				midpoints.emplace(key, index);
				return index;
			};

			light_proxy result;
			result.indices = faces;
			for (auto split = 0; split < 2; split++)
			{
				std::vector<std::uint16_t> indices;
				for (auto i = 0u; i < result.indices.size(); i += 3)
				{
					const auto a = result.indices[i], b = result.indices[i + 1], c = result.indices[i + 2];
					const auto ab = midpoint(a, b), bc = midpoint(b, c), ca = midpoint(c, a);
					indices.insert(indices.end(), {a, ab, ca, b, bc, ab, c, ca, bc, ab, bc, ca});
				}

				result.indices = std::move(indices);
			}

			auto inradius = 1.0f;
			for (auto i = 0u; i < result.indices.size(); i += 3)
			{
				const auto& a = points[result.indices[i]];
				const auto normal = normalize(cross(sub(points[result.indices[i + 1]], a), sub(points[result.indices[i + 2]], a)));
				inradius = std::min(inradius, std::abs(dot(normal, a)));
			}

			const auto scale = radius / inradius;
			for (const auto& point : points)
			{
				result.vertices.push_back({origin[0] + point[0] * scale, origin[1] + point[1] * scale, origin[2] + point[2] * scale});
			}

			return result;
		}

		// s2 primary light dir points back at the light, the ring circumscribes the outer cone at the light radius
		light_proxy make_spot_proxy(const vec3& origin, const vec3& dir, const float radius, const float cos_outer)
		{
			constexpr auto ring_count = 12u;
			const auto axis = normalize({-dir[0], -dir[1], -dir[2]});
			const auto side = normalize(cross(axis, std::abs(axis[2]) < 0.9f ? vec3{0.0f, 0.0f, 1.0f} : vec3{1.0f, 0.0f, 0.0f}));
			const auto up = cross(axis, side);
			const auto ring_radius = radius * std::sqrt(1.0f - cos_outer * cos_outer) / cos_outer / std::cos(std::numbers::pi_v<float> / ring_count);

			light_proxy result;
			result.vertices.push_back(origin);
			for (auto i = 0u; i < ring_count; i++)
			{
				const auto angle = 2.0f * std::numbers::pi_v<float> * static_cast<float>(i) / ring_count;
				const auto c = std::cos(angle) * ring_radius;
				const auto s = std::sin(angle) * ring_radius;
				result.vertices.push_back({origin[0] + axis[0] * radius + side[0] * c + up[0] * s, origin[1] + axis[1] * radius + side[1] * c + up[1] * s,
					origin[2] + axis[2] * radius + side[2] * c + up[2] * s});
			}

			result.vertices.push_back({origin[0] + axis[0] * radius, origin[1] + axis[1] * radius, origin[2] + axis[2] * radius});
			const auto cap = static_cast<std::uint16_t>(ring_count + 1);
			for (auto i = 0u; i < ring_count; i++)
			{
				const auto a = static_cast<std::uint16_t>(1 + i);
				const auto b = static_cast<std::uint16_t>(1 + (i + 1) % ring_count);
				result.indices.insert(result.indices.end(), {0, a, b, cap, b, a});
			}

			return result;
		}

		// the full cone or sphere like the stock hulls: every spot and omni casts shadows (comworld.cpp), which keeps the lights out of the rooms
		// behind their walls, so the hull has to cover everything the falloff reaches or the light ends on tile edges
		void set_frustum_light(const ComPrimaryLight& light, zonetool::s2::GfxFrustumLights& frustum, utils::memory::allocator& allocator)
		{
			constexpr auto proxy_scale = 1.15f; // stock hulls reach 1.03-1.09x radius (spots) and 1.19-1.37x (omnis), so tile edges stay outside the falloff
			constexpr auto proxy_margin = 16.0f; // units
			constexpr auto spot_widen = 5.0f * std::numbers::pi_v<float> / 180.0f; // the cone sides stay outside the outer cone
			constexpr auto min_spot_cos = 0.6f; // wider cones get the sphere hull, the cone ring would reach out to radius * tan(fov)

			const vec3 origin{light.origin[0], light.origin[1], light.origin[2]};
			const auto radius = light.radius * proxy_scale + proxy_margin;
			const auto spot_cos = std::cos(std::acos(std::clamp(light.cosHalfFovOuter, -1.0f, 1.0f)) + spot_widen);
			auto proxy = light.type == GFX_LIGHT_TYPE_SPOT && spot_cos >= min_spot_cos
				? make_spot_proxy(origin, {light.dir[0], light.dir[1], light.dir[2]}, radius, spot_cos)
				: make_omni_proxy(origin, radius);

			vec3 center{};
			for (const auto& vertex : proxy.vertices)
			{
				for (auto axis = 0; axis < 3; axis++)
				{
					center[axis] += vertex[axis] / static_cast<float>(proxy.vertices.size());
				}
			}

			for (auto i = 0u; i < proxy.indices.size(); i += 3)
			{
				const auto& a = proxy.vertices[proxy.indices[i]];
				const auto normal = cross(sub(proxy.vertices[proxy.indices[i + 1]], a), sub(proxy.vertices[proxy.indices[i + 2]], a));
				if (dot(normal, sub(a, center)) > 0.0f)
				{
					std::swap(proxy.indices[i + 1], proxy.indices[i + 2]);
				}
			}

			// frustum_lights_tile vs grows the hull by position - normal * expand, the normal is the inward vertex normal as 10:10:10 unorm at +28
			std::vector<vec3> normals(proxy.vertices.size());
			for (auto i = 0u; i < proxy.indices.size(); i += 3)
			{
				const auto& a = proxy.vertices[proxy.indices[i]];
				const auto normal = normalize(cross(sub(proxy.vertices[proxy.indices[i + 1]], a), sub(proxy.vertices[proxy.indices[i + 2]], a)));
				for (auto corner = 0u; corner < 3; corner++)
				{
					for (auto axis = 0; axis < 3; axis++)
					{
						normals[proxy.indices[i + corner]][axis] += normal[axis];
					}
				}
			}

			vec3 mins{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
			vec3 maxs{-mins[0], -mins[1], -mins[2]};
			frustum.vertexCount = static_cast<unsigned int>(proxy.vertices.size());
			frustum.vertices = allocator.allocate_array<char[32]>(proxy.vertices.size());
			for (auto i = 0u; i < proxy.vertices.size(); i++)
			{
				std::memcpy(frustum.vertices[i], proxy.vertices[i].data(), sizeof(vec3));
				const auto normal = normalize(normals[i]);
				auto packed = 3u << 30;
				for (auto axis = 0; axis < 3; axis++)
				{
					packed |= static_cast<std::uint32_t>(std::lround((normal[axis] * 0.5f + 0.5f) * 1023.0f)) << (axis * 10);
				}
				std::memcpy(frustum.vertices[i] + 28, &packed, sizeof(packed));
				for (auto axis = 0; axis < 3; axis++)
				{
					mins[axis] = std::min(mins[axis], proxy.vertices[i][axis]);
					maxs[axis] = std::max(maxs[axis], proxy.vertices[i][axis]);
				}
			}

			frustum.indexCount = static_cast<unsigned int>(proxy.indices.size());
			frustum.indices = copy_array(proxy.indices.data(), proxy.indices.size(), allocator);
			for (auto axis = 0; axis < 3; axis++)
			{
				frustum.bounds.midPoint[axis] = (mins[axis] + maxs[axis]) * 0.5f;
				frustum.bounds.halfSize[axis] = (maxs[axis] - mins[axis]) * 0.5f;
			}
		}

		// the frustum light job only tests the local lights a visible cell lists (sub_140876B20 / sub_1402454B0 2.20), stock lists them sorted
		void convert_cell_lights(GfxWorld* asset, zonetool::s2::GfxWorld& world, utils::memory::allocator& allocator)
		{
			for (auto cell = 0u; world.cellTransientInfos && cell < world.dpvsPlanes.cellCount; cell++)
			{
				const auto& bounds = asset->cells[cell].bounds;
				std::vector<std::uint16_t> lights;
				for (auto light = asset->lastSunPrimaryLightIndex + 1; light < asset->primaryLightCount; light++)
				{
					const auto& frustum = world.frustumLights[light].bounds;
					auto overlaps = frustum.halfSize[0] >= 0.0f;
					for (auto axis = 0; overlaps && axis < 3; axis++)
					{
						overlaps = std::abs(frustum.midPoint[axis] - bounds.midPoint[axis]) <= frustum.halfSize[axis] + bounds.halfSize[axis];
					}

					if (overlaps)
					{
						lights.push_back(static_cast<std::uint16_t>(light));
					}
				}

				world.cellTransientInfos[cell].indexCount = static_cast<unsigned short>(lights.size());
				world.cellTransientInfos[cell].indices = copy_array(lights.data(), lights.size(), allocator);
			}
		}

		void convert_primary_lights(GfxWorld* asset, zonetool::s2::GfxWorld& world, const light_lists& lists, utils::memory::allocator& allocator)
		{
			const auto com_world = comworld::source_world();
			const auto count = asset->primaryLightCount;
			world.frustumLights = count ? allocator.allocate_array<zonetool::s2::GfxFrustumLights>(count) : nullptr;
			for (auto i = 0u; world.frustumLights && i < count; i++)
			{
				const auto light = com_world && i < com_world->primaryLightCount ? &com_world->primaryLights[i] : nullptr;
				const auto is_local = light && light->type != GFX_LIGHT_TYPE_NONE && light->type != GFX_LIGHT_TYPE_DIR && light->radius > 0.0f;
				if (is_local)
				{
					set_frustum_light(*light, world.frustumLights[i], allocator);
					continue;
				}

				for (auto axis = 0; axis < 3; axis++)
				{
					world.frustumLights[i].bounds.midPoint[axis] = 0.0f;
					world.frustumLights[i].bounds.halfSize[axis] = -std::numeric_limits<float>::max();
				}
			}

			world.lightLists.surfaceListOffsetCount = asset->surfaceCount;
			world.lightLists.surfaceListOffsets = allocator.allocate<unsigned int>();
			world.lightLists.smodelListOffsetCount = asset->dpvs.smodelCount;
			world.lightLists.smodelListOffsets = allocator.allocate<unsigned int>();

			const auto& data = lists.data();
			world.lightLists.listsSize = static_cast<unsigned int>(data.size());
			world.lightLists.lists = copy_array(data.data(), data.size(), allocator);

			convert_cell_lights(asset, world, allocator);
		}

		bool overlaps_sphere(const float* mid, const float* half, const float* origin, const float radius)
		{
			auto distance = 0.0f;
			for (auto axis = 0; axis < 3; axis++)
			{
				const auto outside = std::max(std::abs(origin[axis] - mid[axis]) - half[axis], 0.0f);
				distance += outside * outside;
			}

			return distance <= radius * radius;
		}

		// s2 renders the static shadow map of a light from its shadowGeom casters; maps compiled without them (mp_test_h1) list none, and the
		// shadow then darkens the whole frustum, so local lights without casters get the opaque surfaces and static models in their radius
		zonetool::s2::GfxShadowGeometry* convert_shadow_geometry(const GfxWorld* asset, const GfxShadowGeometry* source, utils::memory::allocator& allocator)
		{
			const auto com_world = comworld::source_world();
			const auto count = asset->primaryLightCount;
			auto* geometry = count ? allocator.allocate_array<zonetool::s2::GfxShadowGeometry>(count) : nullptr;
			const auto& dpvs = asset->dpvs;
			const auto opaque_end = dpvs.unkSurfsEnd > dpvs.unkSurfsBegin ? dpvs.unkSurfsEnd : dpvs.litOpaqueSurfsEnd;
			for (auto i = 0u; geometry && i < count; i++)
			{
				if (source)
				{
					geometry[i] = std::bit_cast<zonetool::s2::GfxShadowGeometry>(source[i]);
				}

				const auto light = com_world && i < com_world->primaryLightCount ? &com_world->primaryLights[i] : nullptr;
				if (geometry[i].surfaceCount || geometry[i].smodelCount || !light || (light->type != GFX_LIGHT_TYPE_SPOT && light->type != GFX_LIGHT_TYPE_OMNI))
				{
					continue;
				}

				std::vector<unsigned int> surfaces;
				for (auto surface = dpvs.litOpaqueSurfsBegin; surface < std::min(opaque_end, asset->surfaceCount) && surfaces.size() < 0xFFFF; surface++)
				{
					const auto& bounds = dpvs.surfacesBounds[surface].bounds;
					if (dpvs.surfaces[surface].material && overlaps_sphere(bounds.midPoint, bounds.halfSize, light->origin, light->radius))
					{
						surfaces.emplace_back(surface);
					}
				}

				std::vector<unsigned short> smodels;
				for (auto smodel = 0u; dpvs.smodelInsts && smodel < dpvs.smodelCount && smodels.size() < 0xFFFF; smodel++)
				{
					const auto bounds = smodel_bounds(dpvs.smodelInsts[smodel]);
					if (!(dpvs.smodelDrawInsts[smodel].flags & STATIC_MODEL_FLAG_NO_CAST_SHADOW) && overlaps_sphere(bounds.midPoint, bounds.halfSize, light->origin, light->radius))
					{
						smodels.emplace_back(static_cast<unsigned short>(smodel));
					}
				}

				geometry[i].surfaceCount = static_cast<unsigned short>(surfaces.size());
				geometry[i].sortedSurfIndex = copy_array(surfaces.data(), surfaces.size(), allocator);
				geometry[i].smodelCount = static_cast<unsigned short>(smodels.size());
				geometry[i].smodelIndex = copy_array(smodels.data(), smodels.size(), allocator);
			}

			return geometry;
		}

		void convert_models(GfxWorld* asset, zonetool::s2::GfxWorld& world, utils::memory::allocator& allocator)
		{
			world.modelCount = asset->modelCount;
			world.models = retype<zonetool::s2::GfxBrushModel>(asset->models);
			world.bounds = std::bit_cast<zonetool::s2::Bounds>(asset->bounds);
			world.shadowBounds = std::bit_cast<zonetool::s2::Bounds>(asset->shadowBounds);
			world.checksum = asset->checksum;
			world.materialMemoryCount = asset->materialMemoryCount;
			world.materialMemory = retype<zonetool::s2::MaterialMemory>(asset->materialMemory);

			if (asset->sun.hasValidData)
			{
				world.sun = std::bit_cast<zonetool::s2::sunflare_t>(asset->sun);
			}
			std::memcpy(world.outdoorLookupMatrix, asset->outdoorLookupMatrix, sizeof(world.outdoorLookupMatrix));
			world.outdoorImage = image_stub(asset->outdoorImage, allocator);

			world.shadowGeom = convert_shadow_geometry(asset, asset->shadowGeom, allocator);
			world.shadowGeomOptimized = convert_shadow_geometry(asset, asset->shadowGeomOptimized, allocator);
			world.lightRegion = retype<zonetool::s2::GfxLightRegion>(asset->lightRegion);
		}

		void convert_surface_ranges(const GfxWorldDpvsStatic& source, zonetool::s2::GfxWorldDpvsStatic& dpvs)
		{
			const auto opaque_end = source.unkSurfsEnd > source.unkSurfsBegin ? source.unkSurfsEnd : source.litOpaqueSurfsEnd;
			const std::array<std::pair<unsigned int, unsigned int>, 8> ranges
			{{
				{source.litOpaqueSurfsBegin, source.litOpaqueSurfsBegin},
				{source.litOpaqueSurfsBegin, opaque_end},
				{source.litDecalSurfsBegin, source.litDecalSurfsEnd},
				{source.litDecalSurfsEnd, source.litDecalSurfsEnd},
				{source.litTransSurfsBegin, source.litTransSurfsEnd},
				{source.litTransSurfsEnd, source.litTransSurfsEnd},
				{source.shadowCasterSurfsBegin, source.shadowCasterSurfsEnd},
				{source.emissiveSurfsBegin, source.emissiveSurfsEnd},
			}};

			for (auto i = 0u; i < ranges.size(); i++)
			{
				dpvs.surfaceRangeBegins[i] = ranges[i].first;
				dpvs.surfaceRangeEnds[i] = ranges[i].second;
			}
		}

		void convert_surfaces(GfxWorld* asset, zonetool::s2::GfxWorldDpvsStatic& dpvs, const light_lists& lists, utils::memory::allocator& allocator)
		{
			const auto& source = asset->dpvs;
			const auto count = asset->surfaceCount;
			const auto surfaces = allocator.allocate_array<zonetool::s2::GfxSurface>(std::max(count, 1u));
			const auto bounds = allocator.allocate_array<zonetool::s2::GfxSurfaceBounds>(std::max(count, 1u));

			for (auto i = 0u; i < count; i++)
			{
				const auto& surface = source.surfaces[i];
				auto& new_surface = surfaces[i];
				new_surface.vertexLayerData = surface.tris.vertexLayerData;
				new_surface.firstVertex = surface.tris.firstVertex;
				new_surface.vertexCount = surface.tris.vertexCount;
				new_surface.triCount = surface.tris.triCount;
				new_surface.baseIndex = surface.tris.baseIndex;
				new_surface.material = surface.material ? shared_stub<zonetool::s2::Material>(surface.material->name, allocator) : nullptr;
				new_surface.lightmapIndex = surface.laf.fields.lightmapIndex;
				new_surface.reflectionProbeIndex = surface.laf.fields.reflectionProbeIndex;
				new_surface.flags = zonetool::s2::GFX_SURFACE_FLAG_CASTS_SHADOW;
				if (surface.laf.fields.flags & GFX_SURFACE_CASTS_SUN_SHADOW)
				{
					new_surface.flags |= zonetool::s2::GFX_SURFACE_FLAG_SUN_SHADOW_PARTITION_0;
				}
				new_surface.lightListOffset = lists.offset(surface.laf.fields.primaryLightEnvIndex);

				const auto& source_bounds = source.surfacesBounds[i];
				auto& new_bounds = bounds[i];
				new_bounds.bounds = std::bit_cast<zonetool::s2::Bounds>(source_bounds.bounds);
				new_bounds.coneCutoff = -std::numeric_limits<float>::max(); // TODO: build the cone from the H1 triangles
				new_bounds.mipRadius = source_bounds.mipRadius;
				std::memcpy(new_bounds.invHighMipRadius, source_bounds.invHighMipRadius, sizeof(new_bounds.invHighMipRadius));
			}

			dpvs.surfaces = count ? surfaces : nullptr;
			dpvs.surfacesBounds = count ? bounds : nullptr;
			dpvs.sortedSurfIndex = source.sortedSurfIndex;
		}

		bool has_vertex_lighting(const GfxStaticModelDrawInst& inst, const GfxStaticModelLighting& lighting)
		{
			return (inst.flags & STATIC_MODEL_FLAG_VERTEXLIT_LIGHTING) && lighting.vertexLightingInfo.lightingValues;
		}

		bool has_model_lightmap(const GfxWorld* asset, const GfxStaticModelDrawInst& inst, const GfxStaticModelLighting& lighting)
		{
			return (inst.flags & STATIC_MODEL_FLAG_LIGHTMAP_LIGHTING) && asset->draw.lightmaps
				&& lighting.modelLightmapInfo.lightmapIndex < static_cast<unsigned int>(std::max(asset->draw.lightmapCount, 0));
		}

		float determinant(const float (&axis)[3][3])
		{
			return axis[0][0] * (axis[1][1] * axis[2][2] - axis[1][2] * axis[2][1])
				- axis[0][1] * (axis[1][0] * axis[2][2] - axis[1][2] * axis[2][0])
				+ axis[0][2] * (axis[1][0] * axis[2][1] - axis[1][1] * axis[2][0]);
		}

		void convert_placement(const GfxPackedPlacement& placement, zonetool::s2::GfxStaticModelDrawInst& inst)
		{
			inst.placement = std::bit_cast<zonetool::s2::GfxPackedPlacement>(placement);
			inst.axisScale[0] = placement.scale;
			inst.axisScale[1] = placement.scale;
			inst.maxScale = std::abs(placement.scale);

			// s2 keeps the axes right handed and mirrors through the y scale
			if (determinant(placement.axis) < 0.0f)
			{
				for (auto& value : inst.placement.axis[1])
				{
					value = -value;
				}
				inst.axisScale[0] = -inst.axisScale[0];
				inst.mirrored = 1;
			}
		}

		std::uint8_t sort_material_index(const XModel* model)
		{
			if (!model || !model->materialHandles)
			{
				return 0;
			}

			// stock also orders by the techset, that order is not reversed
			const auto sort_order = [&](const int index)
			{
				const auto handle = model->materialHandles[index];
				return std::make_tuple(material::convert_sort_key(handle->info.sortKey), std::string_view(handle->name));
			};

			auto best = -1;
			for (auto i = 0; i < model->numsurfs; i++)
			{
				if (model->materialHandles[i] && (best < 0 || sort_order(i) < sort_order(best)))
				{
					best = i;
				}
			}
			return static_cast<std::uint8_t>(std::max(best, 0));
		}

		constexpr std::array<float, 16> lod_dist_scales{0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.875f, 1.0f, 1.125f, 1.25f, 1.5f, 1.75f, 2.0f,
			2.5f, 3.0f, 3.5f, 4.0f}; // h1 g_lodDistIndexToScale

		// h1 leaves its reactive motion grass out of the baked sun shadow partitions but still shadows it in game,
		// s2 only draws the members of a partition into the sun shadow
		bool casts_unbaked_sun_shadow(const GfxStaticModelDrawInst& inst)
		{
			return (inst.flags & STATIC_MODEL_FLAG_REACTIVEMOTION) && !(inst.flags & STATIC_MODEL_FLAG_NO_CAST_SHADOW);
		}

		// sunShadowFlags has one bit per sun shadow partition
		unsigned char all_sun_shadow_partitions(const unsigned int partition_count)
		{
			const auto count = std::clamp(partition_count, 1u, 8u);
			return static_cast<unsigned char>((1u << count) - 1);
		}

		void convert_static_models(GfxWorld* asset, zonetool::s2::GfxWorldDpvsStatic& dpvs, const light_lists& lists, utils::memory::allocator& allocator)
		{
			const auto& source = asset->dpvs;
			const auto count = source.smodelCount;
			dpvs.smodelInsts = retype<zonetool::s2::GfxStaticModelInst>(source.smodelInsts);
			if (!count)
			{
				dpvs.smodelDrawInsts = nullptr;
				dpvs.smodelLightingInsts = nullptr;
				dpvs.sortedSmodelIndices = nullptr;
				return;
			}

			const auto insts = allocator.allocate_array<zonetool::s2::GfxStaticModelDrawInst>(count);
			for (auto i = 0u; i < count; i++)
			{
				const auto& inst = source.smodelDrawInsts[i];
				auto& new_inst = insts[i];
				convert_placement(inst.placement, new_inst);
				new_inst.model = inst.model ? shared_stub<zonetool::s2::XModel>(inst.model->name, allocator) : nullptr;
				const auto lighting_flag = has_model_lightmap(asset, inst, source.smodelLightingInsts[i])
					? zonetool::s2::STATIC_MODEL_FLAG_LIGHTMAP_LIGHTING
					: has_vertex_lighting(inst, source.smodelLightingInsts[i])
					? zonetool::s2::STATIC_MODEL_FLAG_VERTEXLIT_LIGHTING
					: zonetool::s2::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING;
				new_inst.flags = 0x200 | lighting_flag | (inst.flags & STATIC_MODEL_FLAG_NO_CAST_SHADOW); // 0x200 comes from stock zones
				new_inst.staticModelId = inst.staticModelId;
				new_inst.lightListOffset = lists.offset(inst.primaryLightEnvIndex);
				const auto& half_size = smodel_bounds(source.smodelInsts[i]).halfSize;
				new_inst.radius = std::hypot(half_size[0], half_size[1], half_size[2]);
				new_inst.reflectionProbeIndex = inst.reflectionProbeIndex;
				new_inst.sortMaterialIndex = sort_material_index(inst.model);
				new_inst.sunShadowFlags = inst.sunShadowFlags;
				if (casts_unbaked_sun_shadow(inst))
				{
					new_inst.sunShadowFlags |= all_sun_shadow_partitions(source.sunShadowOptCount);
				}
				new_inst.lodScale = lod_dist_scales[inst.flags & STATIC_MODEL_FLAG_SCALE_MODIFIER_MASK];
				new_inst.cullDist = convert_cull_dist(inst, new_inst);
			}
			dpvs.smodelDrawInsts = insts;

			dpvs.smodelLightingInsts = allocator.allocate_array<zonetool::s2::GfxStaticModelLighting>(count);
			for (auto i = 0u; i < count; i++)
			{
				auto& lighting = dpvs.smodelLightingInsts[i];
				if (has_model_lightmap(asset, source.smodelDrawInsts[i], source.smodelLightingInsts[i]))
				{
					lighting.modelLightmapInfo = std::bit_cast<zonetool::s2::GfxStaticModelLightmapInfo>(source.smodelLightingInsts[i].modelLightmapInfo);
				}
				else if (has_vertex_lighting(source.smodelDrawInsts[i], source.smodelLightingInsts[i]))
				{
					const auto& vertex_lighting = source.smodelLightingInsts[i].vertexLightingInfo;
					lighting.vertexLightingInfo.lightingValues = retype<zonetool::s2::GfxStaticModelVertexLighting>(vertex_lighting.lightingValues);
					lighting.vertexLightingInfo.numLightingValues = vertex_lighting.numLightingValues;
				}
				else
				{
					lighting.lightGridInfo.slot = i;
				}
			}
			dpvs.sortedSmodelIndices = allocator.allocate_array<unsigned short>(count);
		}

		void convert_dpvs(GfxWorld* asset, zonetool::s2::GfxWorld& world, const light_lists& lists, utils::memory::allocator& allocator)
		{
			auto& dpvs = world.dpvs;
			const auto& source = asset->dpvs;
			dpvs.smodelCount = source.smodelCount;
			dpvs.staticSurfaceCount = source.staticSurfaceCount;
			convert_surface_ranges(source, dpvs);
			dpvs.smodelVisDataCount = source.smodelVisDataCount;
			dpvs.surfaceVisDataCount = source.surfaceVisDataCount;
			dpvs.sunShadowOptCount = std::max(source.sunShadowOptCount, 1u); // custom maps have none, s2 then drops every smodel from the sun shadow
			dpvs.sunSurfVisDataCount = source.sunSurfVisDataCount ? source.sunSurfVisDataCount : (asset->surfaceCount + 255) / 256 * 8; // mp_crash: 31583 surfaces -> 992
			dpvs.primaryLightVisDataCount = (asset->primaryLightCount + 31) / 32 + 1; // matches every stock map
			dpvs.unk_0x318 = non_null_array<unsigned int>(2ull * source.smodelVisDataCount, allocator);

			mark_runtime(dpvs.smodelVisData, allocator);
			mark_runtime(dpvs.smodelVisDataUnused, allocator);
			mark_runtime(dpvs.smodelUnknownVisData, allocator);
			mark_runtime(dpvs.surfaceVisData, allocator);
			mark_runtime(dpvs.surfaceVisDataUnused, allocator);
			mark_runtime(dpvs.surfaceUnknownVisData, allocator);
			mark_runtime(dpvs.unk_0x2C8, allocator);
			mark_runtime(dpvs.primaryLightVisData, allocator);
			mark_runtime(dpvs.unk_0x2D8, allocator);
			mark_runtime(dpvs.unk_0x2E0, allocator);
			mark_runtime(dpvs.surfaceSortKeys, allocator);
			mark_runtime(dpvs.surfaceCastsSunShadow, allocator);
			mark_runtime(dpvs.surfaceCastsSunShadowOpt, allocator);
			mark_runtime(dpvs.surfaceMaterials, allocator);
			mark_runtime(dpvs.smodelConstantBuffers, allocator);

			convert_surfaces(asset, dpvs, lists, allocator);
			convert_static_models(asset, dpvs, lists, allocator);
		}

		void convert_dpvs_dynamic(GfxWorld* asset, zonetool::s2::GfxWorld& world, utils::memory::allocator& allocator)
		{
			auto dpvs = asset->dpvsDyn;
			for (auto i = 0; i < 2; i++)
			{
				if (!dpvs.dynEntClientCount[i])
				{
					continue;
				}

				dpvs.dynEntCellBits[i] = dpvs.dynEntCellBits[i] ? dpvs.dynEntCellBits[i] : allocator.allocate<unsigned int>();
				for (auto& vis_data : dpvs.dynEntVisData[i])
				{
					vis_data = vis_data ? vis_data : allocator.allocate<unsigned char>();
				}
			}

			world.dpvsDyn = std::bit_cast<zonetool::s2::GfxWorldDpvsDynamic>(dpvs);
		}

		void convert_dyn_ent_scene_buffers(GfxWorld* asset, zonetool::s2::GfxWorld& world, utils::memory::allocator& allocator)
		{
			const auto model_count = asset->dpvsDyn.dynEntClientCount[0];
			const auto brush_count = asset->dpvsDyn.dynEntClientCount[1];
			mark_runtime(world.cellCasterBits, allocator);
			mark_runtime(world.cellHasSunLitSurfsBits, allocator);
			mark_runtime(world.primaryLightEntityShadowVis, allocator);
			reserve_runtime(world.sceneDynModel, model_count, allocator);
			reserve_runtime(world.sceneDynBrush, brush_count, allocator);
			reserve_runtime(world.primaryLightDynEntShadowVis[0], model_count, allocator);
			reserve_runtime(world.primaryLightDynEntShadowVis[1], brush_count, allocator);
			reserve_runtime(world.nonSunPrimaryLightForModelDynEnt, model_count, allocator);
		}

		void convert_dynamic_data(GfxWorld* asset, zonetool::s2::GfxWorld& world, utils::memory::allocator& allocator)
		{
			convert_dpvs_dynamic(asset, world, allocator);
			convert_dyn_ent_scene_buffers(asset, world, allocator);
			world.mapVtxChecksum = asset->mapVtxChecksum;
			world.fogTypesAllowed = 3; // every stock S2 map
			world.buildInfo = std::bit_cast<zonetool::s2::GfxBuildInfo>(asset->buildInfo);
		}

		int light_grid_cell(const float position, const int axis)
		{
			return static_cast<int>(std::floor(position / light_grid_cell_size[axis])) + light_grid_bias[axis];
		}

		// h1 draws models in a light env without the sun with a sunless lit technique, s2 has one lit technique for both:
		// the converted lit shaders drop the sun when the probe's sun visibility is 0
		float probe_sun_visibility(const float visibility, const bool has_sun)
		{
			constexpr auto min_visibility = 0.001f; // keeps the sun on sun env models the bake left fully shadowed

			return has_sun ? std::max(visibility, min_visibility) : 0.0f;
		}

		zonetool::s2::GfxSHProbeData pack_probe(const std::array<float, light_grid::probe_sh_count + 1>& values)
		{
			zonetool::s2::GfxSHProbeData probe{};
			for (auto i = 0u; i < values.size(); i++)
			{
				probe.coeffs[i] = half_float::float_to_half(values[i]);
			}
			return probe;
		}

		// the source map's own default light grid color, for probes no voxel can light
		std::array<float, light_grid::probe_sh_count + 1> default_probe(const GfxLightGrid& grid)
		{
			float radiance[light_grid::basis_dir_count][3];
			for (auto dir = 0u; dir < light_grid::basis_dir_count; dir++)
			{
				for (auto channel = 0; channel < 3; channel++)
				{
					radiance[dir][channel] = half_float::half_to_float(grid.defaultLightGridColors.rgb[dir][channel]);
				}
			}

			std::array<float, light_grid::probe_sh_count + 1> values{};
			light_grid::project_probe_sh(radiance, values.data());
			values.back() = 1.0f;
			return values;
		}

		class probe_average
		{
		public:
			void add(const std::array<float, light_grid::probe_sh_count + 1>& values)
			{
				for (auto i = 0u; i < values.size(); i++)
				{
					this->sum_[i] += values[i];
				}
				this->count_++;
			}

			unsigned int count() const
			{
				return this->count_;
			}

			std::optional<std::array<float, light_grid::probe_sh_count + 1>> value() const
			{
				if (!this->count_)
				{
					return {};
				}

				auto result = this->sum_;
				for (auto& value : result)
				{
					value /= static_cast<float>(this->count_);
				}
				result.back() = 1.0f;
				return result;
			}

		private:
			std::array<float, light_grid::probe_sh_count + 1> sum_{};
			unsigned int count_ = 0;
		};

		class light_grid_sampler
		{
		public:
			light_grid_sampler(const GfxLightGrid& grid, const ComWorld* com_world)
				: grid_(grid)
				, tree_(light_grid::make_tree_view(grid.tree[0], 7)) // leaf size bit count of H1
				, config_{grid.rangeExponent8BitsEncoding, grid.rangeExponent12BitsEncoding, grid.rangeExponent16BitsEncoding}
			{
				if (!com_world)
				{
					return;
				}

				this->sun_envs_.resize(com_world->primaryLightEnvCount);
				for (auto i = 0u; i < com_world->primaryLightEnvCount; i++)
				{
					for (const auto light : env_light_indices(*com_world, com_world->primaryLightEnvs[i]))
					{
						if (com_world->primaryLights[light].type == GFX_LIGHT_TYPE_DIR)
						{
							this->sun_envs_[i] = true;
						}
					}
				}
			}

			bool valid() const
			{
				return this->tree_.node_table && this->grid_.paletteBitstream && this->grid_.paletteEntryAddress;
			}

			// an env the source map doesn't list keeps the sun
			bool env_has_sun(const unsigned int env) const
			{
				return env >= this->sun_envs_.size() || this->sun_envs_[env];
			}

			const light_grid::tree_view& tree() const
			{
				return this->tree_;
			}

			std::optional<std::array<float, light_grid::probe_sh_count + 1>> sample(const float origin[3])
			{
				int base[3];
				float fraction[3];
				for (auto axis = 0; axis < 3; axis++)
				{
					const auto position = origin[axis] / light_grid_cell_size[axis];
					base[axis] = light_grid_cell(origin[axis], axis);
					fraction[axis] = position - std::floor(position);
				}

				std::array<float, light_grid::probe_sh_count + 1> result{};
				auto total_weight = 0.0f;
				for (auto corner = 0; corner < 8; corner++)
				{
					auto weight = 1.0f;
					int cell[3];
					for (auto axis = 0; axis < 3; axis++)
					{
						const auto upper = (corner >> axis) & 1;
						cell[axis] = base[axis] + upper;
						weight *= upper ? fraction[axis] : 1.0f - fraction[axis];
					}

					const auto values = this->cell_values(cell);
					if (!values || weight <= 0.0f)
					{
						continue;
					}

					for (auto i = 0u; i < result.size(); i++)
					{
						result[i] += (*values)[i] * weight;
					}
					total_weight += weight;
				}

				if (total_weight > 0.0f)
				{
					for (auto& value : result)
					{
						value /= total_weight;
					}
					return result;
				}

				return this->nearest(base);
			}

			std::optional<std::array<float, light_grid::probe_sh_count + 1>> baked(const GfxStaticModelAmbientLightingInfo& lighting)
			{
				if (lighting.colorIndex >= this->grid_.paletteEntryCount || lighting.colorIndex == this->grid_.missingGridColorIndex)
				{
					return {};
				}

				auto values = this->palette_values(lighting.colorIndex);
				values.back() = lighting.primaryLightWeight;
				return values;
			}

		private:
			const GfxLightGrid& grid_;
			light_grid::tree_view tree_;
			light_grid::palette_config config_;
			std::vector<bool> sun_envs_;
			std::unordered_map<unsigned int, std::array<float, light_grid::probe_sh_count + 1>> palette_cache_;

			std::optional<std::array<float, light_grid::probe_sh_count + 1>> nearest(const int base[3])
			{
				std::optional<std::array<float, light_grid::probe_sh_count + 1>> best;
				auto best_distance = std::numeric_limits<int>::max();
				for (auto dz = -2; dz <= 2; dz++)
				{
					for (auto dy = -4; dy <= 4; dy++)
					{
						for (auto dx = -4; dx <= 4; dx++)
						{
							const auto distance = dx * dx + dy * dy + 4 * dz * dz;
							if (distance >= best_distance)
							{
								continue;
							}

							const int cell[3] = {base[0] + dx, base[1] + dy, base[2] + dz};
							if (const auto values = this->cell_values(cell))
							{
								best = values;
								best_distance = distance;
							}
						}
					}
				}
				return best;
			}

			std::optional<std::array<float, light_grid::probe_sh_count + 1>> cell_values(const int cell[3])
			{
				const unsigned short grid_pos[3] =
				{
					static_cast<unsigned short>(cell[0]), static_cast<unsigned short>(cell[1]), static_cast<unsigned short>(cell[2]),
				};

				light_grid::voxel voxel{};
				if (!light_grid::find_voxel(this->tree_, grid_pos, voxel) || voxel.color_index >= this->grid_.paletteEntryCount)
				{
					return {};
				}

				auto values = this->palette_values(voxel.color_index);
				const auto is_sun = voxel.light_index < this->sun_envs_.size() && this->sun_envs_[voxel.light_index];
				values.back() = is_sun ? 1.0f : 0.0f;
				return values;
			}

			std::array<float, light_grid::probe_sh_count + 1> palette_values(const unsigned int color_index)
			{
				const auto cached = this->palette_cache_.find(color_index);
				if (cached != this->palette_cache_.end())
				{
					return cached->second;
				}

				light_grid::palette_sh sh{};
				light_grid::decode_palette_entry(this->config_, this->grid_.paletteBitstream + this->grid_.paletteEntryAddress[color_index], sh);

				float radiance[light_grid::basis_dir_count][3];
				light_grid::evaluate(sh, radiance);

				std::array<float, light_grid::probe_sh_count + 1> values{};
				light_grid::project_probe_sh(radiance, values.data());
				return this->palette_cache_.emplace(color_index, values).first->second;
			}
		};

		class light_grid_occupancy
		{
		public:
			explicit light_grid_occupancy(const light_grid::tree_view& tree)
			{
				for (auto axis = 0; axis < 3; axis++)
				{
					this->min_[axis] = tree.coord_min_grid_space[axis];
					this->size_[axis] = std::max(tree.coord_max_grid_space[axis] - tree.coord_min_grid_space[axis] + 1, 0);
				}

				this->occupied_.resize(static_cast<std::size_t>(this->size_[0]) * this->size_[1] * this->size_[2]);
				for (auto z = 0; z < this->size_[2]; z++)
				{
					for (auto y = 0; y < this->size_[1]; y++)
					{
						for (auto x = 0; x < this->size_[0]; x++)
						{
							const unsigned short grid_pos[3] =
							{
								static_cast<unsigned short>(this->min_[0] + x),
								static_cast<unsigned short>(this->min_[1] + y),
								static_cast<unsigned short>(this->min_[2] + z),
							};

							light_grid::voxel voxel{};
							if (!light_grid::find_voxel(tree, grid_pos, voxel))
							{
								continue;
							}

							this->occupied_[this->index(x, y, z)] = true;
							const int cell[3] = {x, y, z};
							for (auto axis = 0; axis < 3; axis++)
							{
								this->occupied_min_[axis] = std::min(this->occupied_min_[axis], cell[axis]);
								this->occupied_max_[axis] = std::max(this->occupied_max_[axis], cell[axis]);
							}
						}
					}
				}
			}

			void clip_to_voxels(float bounds_min[3], float bounds_max[3]) const
			{
				if (this->occupied_min_[0] > this->occupied_max_[0])
				{
					return;
				}

				for (auto axis = 0; axis < 3; axis++)
				{
					const auto first_cell = this->min_[axis] + this->occupied_min_[axis] - 1 - light_grid_bias[axis];
					const auto end_cell = this->min_[axis] + this->occupied_max_[axis] + 2 - light_grid_bias[axis];
					bounds_min[axis] = std::max(bounds_min[axis], static_cast<float>(first_cell) * light_grid_cell_size[axis]);
					bounds_max[axis] = std::min(bounds_max[axis], static_cast<float>(end_cell) * light_grid_cell_size[axis]);
				}
			}

			bool has_voxel_near(const float min[3], const float size) const
			{
				int first[3];
				int last[3];
				for (auto axis = 0; axis < 3; axis++)
				{
					first[axis] = std::max(light_grid_cell(min[axis], axis) - 1 - this->min_[axis], 0);
					last[axis] = std::min(light_grid_cell(min[axis] + size, axis) + 1 - this->min_[axis], this->size_[axis] - 1);
					if (first[axis] > last[axis])
					{
						return false;
					}
				}

				for (auto z = first[2]; z <= last[2]; z++)
				{
					for (auto y = first[1]; y <= last[1]; y++)
					{
						for (auto x = first[0]; x <= last[0]; x++)
						{
							if (this->occupied_[this->index(x, y, z)])
							{
								return true;
							}
						}
					}
				}
				return false;
			}

		private:
			int min_[3]{};
			int size_[3]{};
			int occupied_min_[3]{INT_MAX, INT_MAX, INT_MAX};
			int occupied_max_[3]{INT_MIN, INT_MIN, INT_MIN};
			std::vector<bool> occupied_;

			std::size_t index(const int x, const int y, const int z) const
			{
				return (static_cast<std::size_t>(z) * this->size_[1] + y) * this->size_[0] + x;
			}
		};

		zonetool::s2::GfxStaticModelProbeGrid make_single_probe_grid(const unsigned int probe, const Bounds& bounds)
		{
			zonetool::s2::GfxStaticModelProbeGrid grid{};
			grid.firstProbe = probe;
			grid.probeCountX = 1;
			grid.probeCountY = 1;
			grid.probeCountZ = 1;
			grid.averageProbe = probe;
			for (auto axis = 0; axis < 3; axis++)
			{
				const auto size = std::max(2.0f * bounds.halfSize[axis], 1.0f);
				const auto min = bounds.midPoint[axis] - 0.5f * size;
				grid.worldToGrid[axis][axis] = 1.0f / size;
				grid.worldToGrid[axis][3] = -min / size;
			}
			return grid;
		}

		std::optional<std::array<float, light_grid::probe_sh_count + 1>> static_model_lighting(GfxWorld* asset, light_grid_sampler& sampler, const unsigned int index)
		{
			if (asset->dpvs.smodelDrawInsts[index].flags & STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING)
			{
				if (const auto baked = sampler.baked(asset->dpvs.smodelLightingInsts[index].ambientLightingInfo))
				{
					return baked;
				}
			}

			// no baked color: blend the light grid cells around the lighting origin that hold data, or take the nearest one
			return sampler.sample(asset->dpvs.smodelInsts[index].lightingOrigin);
		}

		void convert_static_model_probes(GfxWorld* asset, zonetool::s2::GfxWorld& world, light_grid_sampler& sampler, utils::memory::allocator& allocator)
		{
			auto& data = world.probeData;
			const auto count = asset->dpvs.smodelCount;
			if (!count)
			{
				return;
			}

			if (!sampler.valid())
			{
				ZONETOOL_WARNING("GfxWorld: the source map has no light grid, static models use its default light grid color");
			}

			const auto grids = allocator.allocate_array<zonetool::s2::GfxStaticModelProbeGrid>(count);
			const auto probes = allocator.allocate_array<zonetool::s2::GfxSHProbeData>(count);
			std::vector<std::optional<std::array<float, light_grid::probe_sh_count + 1>>> samples(count);
			probe_average average;
			for (auto i = 0u; i < count; i++)
			{
				const auto& inst = asset->dpvs.smodelInsts[i];
				grids[i] = make_single_probe_grid(i, smodel_bounds(inst));

				samples[i] = sampler.valid() ? static_model_lighting(asset, sampler, i) : std::nullopt;
				if (samples[i])
				{
					average.add(*samples[i]);
				}
			}

			const auto fallback = average.value().value_or(default_probe(asset->lightGrid));
			for (auto i = 0u; i < count; i++)
			{
				auto values = samples[i] ? *samples[i] : fallback;
				const auto has_sun = sampler.env_has_sun(asset->dpvs.smodelDrawInsts[i].primaryLightEnvIndex);
				values.back() = probe_sun_visibility(values.back(), has_sun);
				probes[i] = pack_probe(values);
			}

			data.smodelProbeGridCount = count;
			data.smodelProbeGrids = grids;
			data.smodelProbeCount = count;
			data.smodelProbes = probes;

			ZONETOOL_INFO("GfxWorld: %u of %u static model probes sampled from the light grid", average.count(), count);
		}

		zonetool::s2::GfxVoxelTree* make_voxel_tree(const probe_volume::volume& volume, utils::memory::allocator& allocator)
		{
			const auto header = allocator.allocate<zonetool::s2::GfxVoxelTreeHeader>();
			const auto tree = allocator.allocate<zonetool::s2::GfxVoxelTree>();
			for (auto axis = 0; axis < 3; axis++)
			{
				header->rootNodeDimension[axis] = volume.root_node_dimension[axis];
				header->nodeCoordBitShift[axis] = volume.node_coord_bit_shift[axis];
				header->boundMin[axis] = volume.bound_min[axis];
				header->boundMax[axis] = volume.bound_max[axis];
				tree->zoneBound.midPoint[axis] = 0.5f * (volume.bound_min[axis] + volume.bound_max[axis]);
				tree->zoneBound.halfSize[axis] = 0.5f * (volume.bound_max[axis] - volume.bound_min[axis]);
			}

			const auto top_down_count = static_cast<unsigned int>(volume.top_down_nodes.size());
			const auto top_down = allocator.allocate_array<zonetool::s2::GfxVoxelTopDownViewNode>(top_down_count);
			for (auto i = 0u; i < top_down_count; i++)
			{
				const auto& node = volume.top_down_nodes[i];
				top_down[i] = {node.first_node_index, node.z_min, node.z_max};
			}

			const auto internal_count = static_cast<unsigned int>(volume.internal_nodes.size());
			const auto internal = allocator.allocate_array<zonetool::s2::GfxVoxelInternalNode>(internal_count);
			for (auto i = 0u; i < internal_count; i++)
			{
				const auto& node = volume.internal_nodes[i];
				for (auto half = 0; half < 2; half++)
				{
					internal[i].firstNodeIndex[half] = static_cast<int>(node.first_node_index[half]);
					internal[i].childNodeMask[half] = node.child_node_mask[half];
				}
			}

			const std::uint16_t light_list[] = {16384, 1, 0};
			const auto leaves = allocator.allocate_array<zonetool::s2::GfxVoxelLeafNode>(volume.leaf_count);
			for (auto i = 1u; i < volume.leaf_count; i++)
			{
				leaves[i].lightListAddress = 2;
			}

			tree->voxelTopDownViewNodeCount = top_down_count;
			tree->voxelInternalNodeCount = internal_count;
			tree->voxelLeafNodeCount = volume.leaf_count;
			tree->lightListArraySize = static_cast<unsigned int>(std::size(light_list));
			tree->voxelTreeHeader = header;
			tree->voxelTopDownViewNodeArray = top_down;
			tree->voxelInternalNodeArray = internal;
			tree->voxelLeafNodeArray = leaves;
			tree->lightListArray = copy_array(light_list, std::size(light_list), allocator);
			tree->voxelInternalNodeDynamicLightList = allocator.allocate_array<unsigned int[2]>(internal_count);
			return tree;
		}

		void convert_probe_volume(GfxWorld* asset, zonetool::s2::GfxWorld& world, light_grid_sampler& sampler, utils::memory::allocator& allocator)
		{
			float world_min[3];
			float world_max[3];
			for (auto axis = 0; axis < 3; axis++)
			{
				world_min[axis] = asset->bounds.midPoint[axis] - asset->bounds.halfSize[axis];
				world_max[axis] = asset->bounds.midPoint[axis] + asset->bounds.halfSize[axis];
			}

			float bounds_min[3];
			float bounds_max[3];
			std::copy_n(world_min, 3, bounds_min);
			std::copy_n(world_max, 3, bounds_max);

			std::optional<light_grid_occupancy> occupancy;
			probe_volume::cell_test cell_occupied;
			if (sampler.valid())
			{
				occupancy.emplace(sampler.tree());
				occupancy->clip_to_voxels(bounds_min, bounds_max);
				cell_occupied = [&occupancy](const float min[3], const float size)
				{
					return occupancy->has_voxel_near(min, size);
				};
			}

			auto volume = probe_volume::build(bounds_min, bounds_max, cell_occupied);
			if (!volume && cell_occupied)
			{
				ZONETOOL_WARNING("GfxWorld: no light probe volume fits the light grid voxels, it covers the whole map instead");
				volume = probe_volume::build(world_min, world_max, {});
			}

			if (!volume)
			{
				ZONETOOL_WARNING("GfxWorld: no light probe volume fits the map, the world has none");
				return;
			}

			auto& data = world.probeData;
			const auto probe_count = static_cast<unsigned int>(volume->probe_positions.size());
			const auto positions = allocator.allocate_array<zonetool::s2::GfxGpuLightGridProbePosition>(probe_count);
			std::vector<std::optional<std::array<float, light_grid::probe_sh_count + 1>>> samples(probe_count);
			probe_average average;
			for (auto i = 0u; i < probe_count; i++)
			{
				std::memcpy(positions[i].origin, volume->probe_positions[i].data(), sizeof(positions[i].origin));
				samples[i] = sampler.sample(positions[i].origin);
				if (samples[i])
				{
					average.add(*samples[i]);
				}
			}

			const auto fallback = pack_probe(average.value().value_or(default_probe(asset->lightGrid)));
			const auto probes = allocator.allocate_array<zonetool::s2::GfxSHProbeData>(probe_count);
			for (auto i = 0u; i < probe_count; i++)
			{
				probes[i] = samples[i] ? pack_probe(*samples[i]) : fallback;
			}

			const auto tetrahedron_count = static_cast<unsigned int>(volume->tetrahedrons.size());
			const auto tetrahedrons = allocator.allocate_array<zonetool::s2::GfxGpuLightGridTetrahedron>(tetrahedron_count);
			const auto neighbors = allocator.allocate_array<zonetool::s2::GfxGpuLightGridTetrahedronNeighbors>(tetrahedron_count);
			for (auto i = 0u; i < tetrahedron_count; i++)
			{
				std::copy_n(volume->tetrahedrons[i].data(), 4, tetrahedrons[i].indexFlags);
				std::copy_n(volume->tetrahedron_neighbors[i].data(), 4, neighbors[i].neighbors);
			}

			const auto start_count = static_cast<unsigned int>(volume->voxel_start_tetrahedron.size());
			const auto starts = allocator.allocate_array<zonetool::s2::GfxGpuLightGridVoxelStartTetrahedron>(start_count);
			for (auto i = 0u; i < start_count; i++)
			{
				starts[i].index = volume->voxel_start_tetrahedron[i];
			}

			const auto zone = allocator.allocate<zonetool::s2::GfxGpuLightGridZone>();
			zone->numProbes = probe_count;
			zone->numTetrahedrons = tetrahedron_count;
			zone->voxelTetrahedronInternalNodeShift = volume->leaf_count;
			zone->numVoxelTetrahedronIndices = start_count;
			zone->fallbackProbeData = fallback;

			data.probeCount = probe_count;
			data.probes = probes;
			data.probePositions = positions;
			data.zoneCount = 1;
			data.zones = zone;
			data.tetrahedronCount = tetrahedron_count;
			data.tetrahedrons = tetrahedrons;
			data.tetrahedronNeighbors = neighbors;
			data.voxelStartTetrahedronCount = start_count;
			data.voxelStartTetrahedron = starts;

			world.voxelTreeCount = 1;
			world.voxelTree = make_voxel_tree(*volume, allocator);

			ZONETOOL_INFO("GfxWorld: light probe volume with %u probes (%u sampled from the light grid), %u tetrahedrons, %u voxel leaves of %d units",
				probe_count, average.count(), tetrahedron_count, volume->leaf_count, 1 << volume->node_coord_bit_shift[2]);
		}

		// stock lights FX from one point in the playable area, the middle of the lit probe volume stands in for it
		void convert_lighting_lookups(GfxWorld* asset, zonetool::s2::GfxWorld& world)
		{
			world.useLightGridDefaultModelLightingLookup = asset->useLightGridDefaultModelLightingLookup;
			std::memcpy(world.lightGridDefaultModelLightingLookup, asset->lightGridDefaultModelLightingLookup, sizeof(world.lightGridDefaultModelLightingLookup));

			world.useLightGridDefaultFXLightingLookup = true;
			const auto header = world.voxelTree ? world.voxelTree->voxelTreeHeader : nullptr;
			for (auto axis = 0; axis < 3; axis++)
			{
				world.lightGridDefaultFXLightingLookup[axis] = asset->useLightGridDefaultFXLightingLookup ? asset->lightGridDefaultFXLightingLookup[axis]
					: header ? 0.5f * (header->boundMin[axis] + header->boundMax[axis]) : asset->bounds.midPoint[axis];
			}
		}

		zonetool::s2::GfxWorld* convert(GfxWorld* asset, const char* transient_zone, utils::memory::allocator& allocator)
		{
			const auto world = allocator.allocate<zonetool::s2::GfxWorld>();
			const light_lists lists(comworld::source_world());
			convert_header(asset, *world, allocator);
			convert_cells(asset, *world, allocator);
			convert_draw(asset, *world, transient_zone, allocator);
			convert_light_grid(*world, allocator);
			convert_primary_lights(asset, *world, lists, allocator);
			convert_models(asset, *world, allocator);
			convert_dpvs(asset, *world, lists, allocator);
			convert_dynamic_data(asset, *world, allocator);

			light_grid_sampler sampler(asset->lightGrid, comworld::source_world());
			convert_static_model_probes(asset, *world, sampler, allocator);
			convert_probe_volume(asset, *world, sampler, allocator);
			convert_lighting_lookups(asset, *world);

			// with culling on, static geometry marks its lights through the light lists while there is a voxel tree (sub_140869F10 2.20)
			world->cullPrimaryLights = world->voxelTreeCount > 0;
			return world;
		}
	}

	void dump(GfxWorld* asset)
	{
		pending_world = asset;
	}

	void finish()
	{
		if (!pending_world)
		{
			return;
		}

		utils::memory::allocator allocator;
		const auto transient_zone = allocator.duplicate_string(transient_zone_name(pending_world));
		zonetool::s2::gfx_map_trzone::dump(convert_transient_zone(pending_world, transient_zone, allocator));
		add_csv_row("gfx_map_trzone", transient_zone);

		const auto world = convert(pending_world, transient_zone, allocator);
		zonetool::s2::gfx_world::dump(world);
		clear_stubs();
		add_csv_row("gfx_map", pending_world->name);

		ZONETOOL_INFO("GfxWorld \"%s\": %u surfaces, %u static models, %u primary lights", pending_world->name,
			pending_world->surfaceCount, pending_world->dpvs.smodelCount, pending_world->primaryLightCount);
		pending_world = nullptr;
	}
}
