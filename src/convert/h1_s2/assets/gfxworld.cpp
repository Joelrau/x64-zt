#include <std_include.hpp>

#include "../include.hpp"
#include "gfxworld.hpp"
#include "comworld.hpp"
#include "gfximage.hpp"
#include "material.hpp"
#include "gfxworld/light_grid.hpp"

#include <bit>

namespace zonetool::h1::converter::s2::gfxworld
{
	namespace
	{
		constexpr zonetool::s2::GfxWorldSortKeys stock_sort_keys
		{
			.unk_0x1 = 38,
			.unk_0x2 = 39,
			.twoD = 62,
			.effectDecal = 44,
			.unk_0x5 = 21,
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

		unsigned short convert_cull_dist(const GfxStaticModelDrawInst& inst, const zonetool::s2::GfxStaticModelDrawInst& new_inst)
		{
			// s2 compares the cull distance with distance / maxScale, 0 is no cull
			const auto scale = new_inst.maxScale;
			auto dist = inst.cullDist && scale > 0.0f ? inst.cullDist / scale : 0.0f;
			if (!dist && inst.model && inst.model->numLods > 0 && inst.model->numLods <= 6)
			{
				dist = inst.model->lodInfo[inst.model->numLods - 1].dist * new_inst.lodScale;
			}

			return dist > 65535.0f ? 0 : static_cast<unsigned short>(std::round(dist));
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
					const auto lights = comworld::env_light_indices(*com_world, com_world->primaryLightEnvs[i]);
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
				data.texelDensities[i] = std::abs(vertex.binormalSign); // h1 binormalSign is the signed texel density
				data.colors[i] = vertex.color.packed;
				std::memcpy(data.texCoords[i], vertex.texCoord, sizeof(vertex.texCoord));
				std::memcpy(data.lmapCoords[i], vertex.lmapCoord, sizeof(vertex.lmapCoord));
				data.normals[i] = vertex.normal.packed;
				data.tangents[i] = pack_tangent(vertex.tangent.packed, vertex.binormalSign);
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
			world.movingScriptablePrimaryLightCount = 0;
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

		void convert_lightmap_tiles(const GfxWorldDraw& source, zonetool::s2::GfxWorldDraw& draw)
		{
			// every tile of every lightmap, stock lists only the tiles that its charts use
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
				draw.reflectionProbes[0] = draw.reflectionProbes[1]; // h1 probe 0 is a red default image
			}
			reserve_runtime(draw.reflectionProbeTextures, draw.reflectionProbeCount, allocator);

			draw.reflectionProbeOrigins = retype<zonetool::s2::GfxReflectionProbe>(source.reflectionProbeOrigins);
			draw.reflectionProbeReferenceCount = source.reflectionProbeReferenceCount;
			draw.reflectionProbeReferenceOrigins = retype<zonetool::s2::GfxReflectionProbeReferenceOrigin>(source.reflectionProbeReferenceOrigins);
			draw.reflectionProbeReferences = retype<zonetool::s2::GfxReflectionProbeReference>(source.reflectionProbeReferences);

			const auto fog_spline = gfximage::dump_fog_spline("*fog_spline0");
			draw.fogSplineCount = 1;
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

		light_proxy make_spot_proxy(const vec3& origin, const vec3& dir, const float radius, const float cos_outer)
		{
			constexpr auto ring_count = 12u;
			const auto axis = normalize({-dir[0], -dir[1], -dir[2]}); // s2 light dir points back at the light
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

		std::uint32_t pack_unorm_normal(const vec3& normal)
		{
			auto packed = 3u << 30;
			for (auto axis = 0; axis < 3; axis++)
			{
				packed |= static_cast<std::uint32_t>(std::lround((normal[axis] * 0.5f + 0.5f) * 1023.0f)) << (axis * 10);
			}

			return packed;
		}

		void set_frustum_light(const ComPrimaryLight& light, zonetool::s2::GfxFrustumLights& frustum, utils::memory::allocator& allocator)
		{
			constexpr auto proxy_scale = 1.15f; // the hull must reach past the falloff or the light ends on tile edges
			constexpr auto proxy_margin = 16.0f;
			constexpr auto spot_widen = 5.0f * std::numbers::pi_v<float> / 180.0f;
			constexpr auto min_spot_cos = 0.6f;

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

			// inward vertex normals, the tile shader grows the hull along them
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
			frustum.vertices = allocator.allocate_array<zonetool::s2::GfxFrustumLightVertex>(proxy.vertices.size());
			for (auto i = 0u; i < proxy.vertices.size(); i++)
			{
				zonetool::s2::GfxFrustumLightVertex vertex{};
				std::memcpy(vertex.xyz, proxy.vertices[i].data(), sizeof(vertex.xyz));
				vertex.normal = pack_unorm_normal(normalize(normals[i]));
				frustum.vertices[i] = vertex;
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
				// s2 shadows the whole frustum of a light without casters
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
					const auto& bounds = dpvs.smodelInsts[smodel].bounds;
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

			// TODO: stock also orders by the techset
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

		constexpr std::array<float, 16> lod_dist_index_to_scale{0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.875f, 1.0f, 1.125f, 1.25f, 1.5f, 1.75f, 2.0f,
			2.5f, 3.0f, 3.5f, 4.0f};

		bool casts_unbaked_sun_shadow(const GfxStaticModelDrawInst& inst)
		{
			return (inst.flags & STATIC_MODEL_FLAG_REACTIVEMOTION) && !(inst.flags & STATIC_MODEL_FLAG_NO_CAST_SHADOW);
		}

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
				new_inst.flags = 0x200 | lighting_flag | (inst.flags & STATIC_MODEL_FLAG_NO_CAST_SHADOW); // stock zones set 0x200
				new_inst.staticModelId = inst.staticModelId;
				new_inst.lightListOffset = lists.offset(inst.primaryLightEnvIndex);
				const auto& half_size = source.smodelInsts[i].bounds.halfSize;
				new_inst.radius = std::hypot(half_size[0], half_size[1], half_size[2]);
				new_inst.reflectionProbeIndex = inst.reflectionProbeIndex;
				new_inst.sortMaterialIndex = sort_material_index(inst.model);
				new_inst.sunShadowFlags = inst.sunShadowFlags;
				if (casts_unbaked_sun_shadow(inst))
				{
					new_inst.sunShadowFlags |= all_sun_shadow_partitions(source.sunShadowOptCount);
				}
				new_inst.lodScale = lod_dist_index_to_scale[inst.flags & STATIC_MODEL_FLAG_SCALE_MODIFIER_MASK];
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
			dpvs.sunShadowOptCount = std::max(source.sunShadowOptCount, 1u); // without one s2 drops every smodel from the sun shadow
			dpvs.sunSurfVisDataCount = source.sunSurfVisDataCount ? source.sunSurfVisDataCount : (asset->surfaceCount + 255) / 256 * 8;
			dpvs.primaryLightVisDataCount = (asset->primaryLightCount + 31) / 32 + 1;
			dpvs.smodelLightingStateBits = non_null_array<unsigned int>(2ull * source.smodelVisDataCount, allocator);

			mark_runtime(dpvs.smodelVisData, allocator);
			mark_runtime(dpvs.smodelVisDataUnused, allocator);
			mark_runtime(dpvs.smodelUnknownVisData, allocator);
			mark_runtime(dpvs.surfaceVisData, allocator);
			mark_runtime(dpvs.surfaceVisDataUnused, allocator);
			mark_runtime(dpvs.surfaceUnknownVisData, allocator);
			mark_runtime(dpvs.unk_0x2C8, allocator);
			mark_runtime(dpvs.primaryLightVisData, allocator);
			mark_runtime(dpvs.smodelFoliageVisData, allocator);
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
			world.fogTypesAllowed = 3;
			world.buildInfo = std::bit_cast<zonetool::s2::GfxBuildInfo>(asset->buildInfo);
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
			convert_light_grid_probes(asset, *world, allocator);

			// s2 culls the lights through the light lists only with a voxel tree
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
