#include <std_include.hpp>

#include "../include.hpp"
#include "gfxworld.hpp"
#include "comworld.hpp"
#include "gfximage.hpp"
#include "material.hpp"
#include "xmodel.hpp"
#include "gfxworld/light_grid.hpp"

#include <h1/assets/gfxworld.hpp>

#include <bit>

namespace zonetool::s2::converter::h1::gfxworld
{
	namespace
	{
		constexpr std::size_t transient_zone_count = 16;

		enum surface_range
		{
			range_lit_opaque,
			range_lit_opaque_sss,
			range_lit_decal,
			range_lit_trans,
			range_shadow_caster,
			range_emissive,
			range_count,
		};

		GfxWorld* pending_world = nullptr;
		std::array<GfxMapTrZone*, transient_zone_count> loaded_transient_zones{};
		std::array<std::uint32_t, transient_zone_count> transient_vertex_base{};
		std::array<std::uint32_t, transient_zone_count> transient_layer_data_base{};

		template <typename To, typename From>
		To* retype(From* data)
		{
			static_assert(sizeof(To) == sizeof(From));
			return reinterpret_cast<To*>(data);
		}

		template <typename T>
		T* allocate_zeroed(utils::memory::allocator& allocator, const std::size_t count)
		{
			return count ? allocator.allocate_array<T>(count) : nullptr;
		}

		template <typename T>
		std::unordered_map<std::string, T*>& stubs()
		{
			static std::unordered_map<std::string, T*> stubs;
			return stubs;
		}

		void clear_stubs()
		{
			stubs<zonetool::h1::Material>().clear();
			stubs<zonetool::h1::XModel>().clear();
			stubs<zonetool::h1::GfxImage>().clear();
		}

		template <typename T>
		T* make_stub(const char* name, utils::memory::allocator& allocator)
		{
			auto& stub = stubs<T>()[name];
			if (!stub)
			{
				stub = allocator.allocate<T>();
				stub->name = name;
			}

			return stub;
		}

		surface_range get_surface_range(const Material* material)
		{
			if (!material)
			{
				return range_lit_opaque;
			}

			const auto region = material::find_camera_region(material->name);
			if (!region)
			{
				return std::string_view(material->name).find("_dec") != std::string_view::npos ? range_lit_decal : range_lit_opaque;
			}

			switch (*region)
			{
			case zonetool::h1::CAMERA_REGION_LIT_OPAQUE:
				return range_lit_opaque;
			case zonetool::h1::CAMERA_REGION_LIT_OPAQUE_SSS:
				return range_lit_opaque_sss;
			case zonetool::h1::CAMERA_REGION_LIT_DECAL:
				return range_lit_decal;
			case zonetool::h1::CAMERA_REGION_EMISSIVE:
				return range_emissive;
			case zonetool::h1::CAMERA_REGION_NONE:
				return range_shadow_caster;
			default:
				return range_lit_trans;
			}
		}

		std::array<GfxMapTrZone*, transient_zone_count> get_transient_zones(const GfxWorld* asset)
		{
			auto zones = loaded_transient_zones;
			for (auto i = 0u; i < transient_zone_count; i++)
			{
				if (asset->draw.transientZones[i])
				{
					zones[i] = asset->draw.transientZones[i];
				}
			}

			return zones;
		}

		float convert_binormal_sign(const float texel_density, const unsigned int tangent)
		{
			const auto density = texel_density > 0.0f ? texel_density : 1.0f;
			return (tangent >> 30) >= 2 ? -density : density;
		}

		void convert_vertices(GfxWorld* asset, zonetool::h1::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto zones = get_transient_zones(asset);
			for (auto i = 0u; i < transient_zone_count; i++)
			{
				transient_vertex_base[i] = world->draw.vertexCount;
				transient_layer_data_base[i] = world->draw.vertexLayerDataSize;
				if (zones[i])
				{
					world->draw.vertexCount += zones[i]->vertexCount;
					world->draw.vertexLayerDataSize += zones[i]->vertexLayerDataSize;
				}
			}

			world->draw.vd.vertices = allocate_zeroed<zonetool::h1::GfxWorldVertex>(allocator, world->draw.vertexCount);
			world->draw.vld.data = allocate_zeroed<unsigned char>(allocator, world->draw.vertexLayerDataSize);

			auto vertex = world->draw.vd.vertices;
			auto layer_data = world->draw.vld.data;
			for (const auto zone : zones)
			{
				if (!zone)
				{
					continue;
				}

				const auto& data = zone->vertices.data;
				for (auto i = 0u; i < zone->vertexCount; i++, vertex++)
				{
					std::memcpy(vertex->xyz, data.positions[i], sizeof(vertex->xyz));
					vertex->binormalSign = convert_binormal_sign(data.texelDensities[i], data.tangents[i]);
					vertex->color.packed = data.colors[i];
					std::memcpy(vertex->texCoord, data.texCoords[i], sizeof(vertex->texCoord));
					std::memcpy(vertex->lmapCoord, data.lmapCoords[i], sizeof(vertex->lmapCoord));
					vertex->normal.packed = data.normals[i];
					vertex->tangent.packed = data.tangents[i] | 0xC0000000u;
				}

				if (zone->vld.data)
				{
					std::memcpy(layer_data, zone->vld.data, zone->vertexLayerDataSize);
				}

				layer_data += zone->vertexLayerDataSize;
			}

			world->draw.indexCount = asset->draw.indexCount;
			world->draw.indices = asset->draw.indices;
		}

		float get_max_edge_length(const zonetool::h1::GfxWorld* world, const zonetool::h1::srfTriangles_t& surface)
		{
			auto max_length = 0.0f;
			const auto indices = world->draw.indices + surface.baseIndex;
			for (auto tri = 0u; tri < surface.triCount; tri++)
			{
				for (auto edge = 0u; edge < 3; edge++)
				{
					const auto a = surface.firstVertex + indices[tri * 3 + edge];
					const auto b = surface.firstVertex + indices[tri * 3 + (edge + 1) % 3];
					if (a >= world->draw.vertexCount || b >= world->draw.vertexCount)
					{
						continue;
					}

					const auto& pa = world->draw.vd.vertices[a].xyz;
					const auto& pb = world->draw.vd.vertices[b].xyz;
					const auto dx = pa[0] - pb[0];
					const auto dy = pa[1] - pb[1];
					const auto dz = pa[2] - pb[2];
					max_length = std::max(max_length, std::sqrt(dx * dx + dy * dy + dz * dz));
				}
			}

			return max_length;
		}

		struct light_env_stats
		{
			std::size_t added;
			std::size_t listed_out_of_range;
		};

		light_env_stats env_stats{};

		float distance_sq_to_bounds(const float* point, const Bounds& bounds)
		{
			auto result = 0.0f;
			for (auto axis = 0; axis < 3; axis++)
			{
				const auto outside = std::max(0.0f, std::abs(point[axis] - bounds.midPoint[axis]) - bounds.halfSize[axis]);
				result += outside * outside;
			}

			return result;
		}

		bool reaches_bounds(const ComPrimaryLight& light, const Bounds& bounds)
		{
			if (light.type != zonetool::h1::GFX_LIGHT_TYPE_SPOT && light.type != zonetool::h1::GFX_LIGHT_TYPE_OMNI)
			{
				return false;
			}

			if (distance_sq_to_bounds(light.origin, bounds) > light.radius * light.radius)
			{
				return false;
			}

			if (light.type == zonetool::h1::GFX_LIGHT_TYPE_OMNI)
			{
				return true;
			}

			const float offset[3]{bounds.midPoint[0] - light.origin[0], bounds.midPoint[1] - light.origin[1], bounds.midPoint[2] - light.origin[2]};
			const auto along = -(offset[0] * light.dir[0] + offset[1] * light.dir[1] + offset[2] * light.dir[2]);
			const auto length_sq = offset[0] * offset[0] + offset[1] * offset[1] + offset[2] * offset[2];
			const auto across = std::sqrt(std::max(0.0f, length_sq - along * along));
			const auto radius = std::hypot(bounds.halfSize[0], bounds.halfSize[1], bounds.halfSize[2]);
			const auto sin_outer = std::sqrt(std::max(0.0f, 1.0f - light.cosHalfFovOuter * light.cosHalfFovOuter));
			return across * light.cosHalfFovOuter - along * sin_outer <= radius;
		}

		std::uint16_t get_light_env(const GfxWorld* asset, const zonetool::h1::GfxWorld* world, const std::uint16_t list_offset, const Bounds& bounds)
		{
			const auto* com_world = comworld::source_world();
			const auto is_local_light = [&](const std::uint32_t light)
			{
				return light > world->lastSunPrimaryLightIndex && light < world->primaryLightCount;
			};

			std::vector<std::uint16_t> lights;
			const auto& light_lists = asset->lightLists;
			if (light_lists.lists && list_offset < light_lists.listsSize)
			{
				const auto count = std::min<std::uint32_t>(light_lists.lists[list_offset], light_lists.listsSize - list_offset - 1);
				for (auto i = 0u; i < count; i++)
				{
					const auto light = light_lists.lists[list_offset + 1 + i];
					const auto is_extra_sun = light > 1 && light <= world->lastSunPrimaryLightIndex;
					if (light >= world->primaryLightCount || is_extra_sun)
					{
						continue;
					}

					if (com_world && is_local_light(light) && !reaches_bounds(com_world->primaryLights[light], bounds))
					{
						env_stats.listed_out_of_range++;
					}

					lights.push_back(light);
				}
			}

			if (com_world)
			{
				std::vector<std::pair<float, std::uint16_t>> reached;
				for (auto light = world->lastSunPrimaryLightIndex + 1u; light < world->primaryLightCount; light++)
				{
					const auto& source = com_world->primaryLights[light];
					if (std::ranges::find(lights, light) == lights.end() && reaches_bounds(source, bounds))
					{
						reached.emplace_back(distance_sq_to_bounds(source.origin, bounds), static_cast<std::uint16_t>(light));
					}
				}

				std::ranges::sort(reached);
				for (const auto& [distance, light] : reached)
				{
					if (lights.size() >= 4)
					{
						break;
					}

					lights.push_back(light);
					env_stats.added++;
				}
			}

			std::sort(lights.begin(), lights.end());
			return comworld::get_light_env(std::move(lights));
		}

		zonetool::h1::GfxSurface convert_surface(const zonetool::h1::GfxWorld* world, const GfxSurface& surface, const std::uint16_t light_env, utils::memory::allocator& allocator)
		{
			zonetool::h1::GfxSurface new_surface{};
			const auto zone = surface.transientZone < transient_zone_count ? surface.transientZone : 0;
			new_surface.tris.vertexLayerData = transient_layer_data_base[zone] + surface.vertexLayerData;
			new_surface.tris.firstVertex = transient_vertex_base[zone] + surface.firstVertex;
			new_surface.tris.unk = -1;
			new_surface.tris.vertexCount = surface.vertexCount;
			new_surface.tris.triCount = surface.triCount;
			new_surface.tris.baseIndex = surface.baseIndex;
			new_surface.tris.maxEdgeLength = get_max_edge_length(world, new_surface.tris);
			new_surface.material = make_stub<zonetool::h1::Material>(surface.material ? surface.material->name : "$default", allocator);
			new_surface.laf.fields.lightmapIndex = surface.lightmapIndex;
			new_surface.laf.fields.reflectionProbeIndex = surface.reflectionProbeIndex;
			new_surface.laf.fields.primaryLightEnvIndex = light_env;
			const auto partitions = (surface.flags & GFX_SURFACE_FLAG_SUN_SHADOW_PARTITION_MASK) >> 4;
			new_surface.laf.fields.flags = partitions ? static_cast<unsigned char>(zonetool::h1::GFX_SURFACE_CASTS_SUN_SHADOW | partitions * zonetool::h1::GFX_SURFACE_SUN_SHADOW_OPT_PARTITION_0) : 0;

			return new_surface;
		}

		void set_surface_ranges(zonetool::h1::GfxWorldDpvsStatic& dpvs, const std::array<unsigned int, range_count>& range_sizes)
		{
			std::array<unsigned int*, range_count> begins
			{
				&dpvs.litOpaqueSurfsBegin, &dpvs.unkSurfsBegin, &dpvs.litDecalSurfsBegin,
				&dpvs.litTransSurfsBegin, &dpvs.shadowCasterSurfsBegin, &dpvs.emissiveSurfsBegin,
			};

			auto begin = 0u;
			for (auto range = 0u; range < range_count; range++)
			{
				*begins[range] = begin;
				begin += range_sizes[range];
				begins[range][1] = begin;
			}
		}

		std::vector<unsigned int> convert_surfaces(GfxWorld* asset, zonetool::h1::GfxWorld* world, utils::memory::allocator& allocator)
		{
			auto& dpvs = world->dpvs;
			const auto surfaces = asset->dpvs.surfaces;
			const auto bounds = asset->dpvs.surfacesBounds;

			std::vector<surface_range> ranges(dpvs.staticSurfaceCount);
			std::array<unsigned int, range_count> range_sizes{};
			for (auto i = 0u; i < dpvs.staticSurfaceCount; i++)
			{
				ranges[i] = get_surface_range(surfaces[i].material);
				range_sizes[ranges[i]]++;
			}

			std::vector<unsigned int> order(dpvs.staticSurfaceCount);
			std::iota(order.begin(), order.end(), 0u);
			std::stable_sort(order.begin(), order.end(), [&](const unsigned int a, const unsigned int b)
			{
				return ranges[a] < ranges[b];
			});

			std::vector<unsigned int> new_index(world->surfaceCount);
			std::iota(new_index.begin(), new_index.end(), 0u);
			for (auto i = 0u; i < order.size(); i++)
			{
				new_index[order[i]] = i;
			}

			dpvs.surfaces = allocator.allocate_array<zonetool::h1::GfxSurface>(world->surfaceCount);
			dpvs.surfacesBounds = allocator.allocate_array<zonetool::h1::GfxSurfaceBounds>(world->surfaceCount);
			for (auto i = 0u; i < world->surfaceCount; i++)
			{
				auto& new_bounds = dpvs.surfacesBounds[new_index[i]];
				const auto light_env = get_light_env(asset, world, surfaces[i].lightListOffset, bounds[i].bounds);
				dpvs.surfaces[new_index[i]] = convert_surface(world, surfaces[i], light_env, allocator);
				new_bounds.bounds = std::bit_cast<zonetool::h1::Bounds>(bounds[i].bounds);
				new_bounds.mipRadius = bounds[i].mipRadius;
				std::memcpy(new_bounds.invHighMipRadius, bounds[i].invHighMipRadius, sizeof(bounds[i].invHighMipRadius));
			}

			set_surface_ranges(dpvs, range_sizes);

			dpvs.sortedSurfIndex = allocator.allocate_array<unsigned int>(dpvs.staticSurfaceCount);
			for (auto i = 0u; i < dpvs.staticSurfaceCount; i++)
			{
				dpvs.sortedSurfIndex[i] = new_index[asset->dpvs.sortedSurfIndex[i]];
			}

			ZONETOOL_INFO("GfxWorld surfaces: %u opaque, %u decal, %u trans, %u shadow caster, %u emissive, %u brush",
				range_sizes[range_lit_opaque] + range_sizes[range_lit_opaque_sss], range_sizes[range_lit_decal], range_sizes[range_lit_trans],
				range_sizes[range_shadow_caster], range_sizes[range_emissive], world->surfaceCount - dpvs.staticSurfaceCount);

			return new_index;
		}

		zonetool::h1::GfxShadowGeometry* convert_shadow_geometry(const GfxShadowGeometry* source, const unsigned int count,
			const std::vector<unsigned int>& new_index, utils::memory::allocator& allocator)
		{
			if (!source)
			{
				return nullptr;
			}

			const auto geometry = retype<const zonetool::h1::GfxShadowGeometry>(source);
			const auto new_geometry = allocator.allocate_array<zonetool::h1::GfxShadowGeometry>(count);
			for (auto i = 0u; i < count; i++)
			{
				new_geometry[i] = geometry[i];
				new_geometry[i].sortedSurfIndex = allocate_zeroed<unsigned int>(allocator, geometry[i].surfaceCount);
				for (auto j = 0u; j < geometry[i].surfaceCount; j++)
				{
					new_geometry[i].sortedSurfIndex[j] = new_index[geometry[i].sortedSurfIndex[j]];
				}
			}

			return new_geometry;
		}

		constexpr std::array<float, 16> lod_dist_index_to_scale{0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.875f, 1.0f, 1.125f, 1.25f, 1.5f, 1.75f, 2.0f,
			2.5f, 3.0f, 3.5f, 4.0f};

		template <typename T>
		std::vector<std::vector<T>> get_light_env_members(const zonetool::h1::GfxWorld* world, const unsigned int count, const auto& get_env)
		{
			std::vector<std::vector<T>> members(world->primaryLightCount);
			for (auto i = 0u; i < count; i++)
			{
				for (const auto light : comworld::get_light_env_lights(get_env(i)))
				{
					if (light > world->lastSunPrimaryLightIndex && light < world->primaryLightCount)
					{
						members[light].push_back(static_cast<T>(i));
					}
				}
			}

			return members;
		}

		template <typename T>
		unsigned short copy_light_list(const std::vector<T>& members, T*& list, utils::memory::allocator& allocator)
		{
			const auto count = std::min<std::size_t>(members.size(), std::numeric_limits<unsigned short>::max());
			list = count ? allocator.allocate_array<T>(count) : nullptr;
			std::copy_n(members.begin(), count, list);
			return static_cast<unsigned short>(count);
		}

		void set_light_env_shadow_geometry(zonetool::h1::GfxShadowGeometry* geometry, const zonetool::h1::GfxWorld* world, utils::memory::allocator& allocator)
		{
			if (!geometry)
			{
				return;
			}

			const auto surfaces = get_light_env_members<unsigned int>(world, world->surfaceCount, [&](const unsigned int i)
			{
				return world->dpvs.surfaces[i].laf.fields.primaryLightEnvIndex;
			});
			const auto smodels = get_light_env_members<unsigned short>(world, world->dpvs.smodelCount, [&](const unsigned int i)
			{
				return world->dpvs.smodelDrawInsts[i].primaryLightEnvIndex;
			});

			for (auto light = world->lastSunPrimaryLightIndex + 1u; light < world->primaryLightCount; light++)
			{
				geometry[light].surfaceCount = copy_light_list(surfaces[light], geometry[light].sortedSurfIndex, allocator);
				geometry[light].smodelCount = copy_light_list(smodels[light], geometry[light].smodelIndex, allocator);
			}
		}

		bool has_shadow_lists(const zonetool::h1::GfxShadowGeometry* geometry, const unsigned int count)
		{
			for (auto i = 0u; geometry && i < count; i++)
			{
				if (geometry[i].surfaceCount || geometry[i].smodelCount)
				{
					return true;
				}
			}

			return false;
		}

		zonetool::h1::GfxShadowGeometry* make_shadow_geometry(const zonetool::h1::GfxWorld* world, utils::memory::allocator& allocator)
		{
			std::vector<unsigned short> sun_casters;
			for (auto i = 0u; i < world->dpvs.smodelCount; i++)
			{
				if (world->dpvs.smodelDrawInsts[i].sunShadowFlags)
				{
					sun_casters.emplace_back(static_cast<unsigned short>(i));
				}
			}

			const auto geometry = allocate_zeroed<zonetool::h1::GfxShadowGeometry>(allocator, world->primaryLightCount);
			for (auto light = 0u; light < world->primaryLightCount; light++)
			{
				if (light && light <= world->lastSunPrimaryLightIndex)
				{
					geometry[light].smodelCount = static_cast<unsigned short>(sun_casters.size());
					geometry[light].smodelIndex = allocate_zeroed<unsigned short>(allocator, sun_casters.size());
					std::ranges::copy(sun_casters, geometry[light].smodelIndex);
				}
				else if (world->shadowGeomOptimized)
				{
					geometry[light] = world->shadowGeomOptimized[light];
				}
			}

			return geometry;
		}

		std::uint16_t get_lod_scale_index(const float lod_scale)
		{
			const auto nearest = std::ranges::min_element(lod_dist_index_to_scale, {}, [&](const float scale)
			{
				return std::abs(scale - lod_scale);
			});

			return static_cast<std::uint16_t>(nearest - lod_dist_index_to_scale.begin());
		}

		std::uint16_t to_h1_cull_dist(const float dist)
		{
			return dist >= 65535.0f ? 0 : static_cast<std::uint16_t>(std::max(std::round(dist), 1.0f));
		}

		std::uint16_t get_smodel_cull_dist(const GfxStaticModelDrawInst& draw_inst)
		{
			return draw_inst.cullDist ? to_h1_cull_dist(draw_inst.cullDist * draw_inst.maxScale) : 0;
		}

		zonetool::h1::GfxPackedPlacement convert_placement(const GfxStaticModelDrawInst& inst)
		{
			auto placement = std::bit_cast<zonetool::h1::GfxPackedPlacement>(inst.placement);
			if (inst.mirrored)
			{
				for (auto& value : placement.axis[1])
				{
					value = -value;
				}
			}

			for (auto axis = 1; axis < 3 && placement.scale != 0.0f; axis++)
			{
				const auto ratio = inst.axisScale[axis - 1] / placement.scale;
				for (auto& value : placement.axis[axis])
				{
					value *= ratio;
				}
			}

			return placement;
		}

		unsigned int get_worst_view_load(const zonetool::h1::GfxWorldDpvsStatic& dpvs, const std::vector<float>& cull_dists)
		{
			constexpr auto angle_bins = 64;
			constexpr auto half_fov_bins = 9;
			auto worst = 0u;
			for (auto viewer = 0u; viewer < dpvs.smodelCount; viewer += 16)
			{
				const auto& view_origin = dpvs.smodelDrawInsts[viewer].placement.origin;
				std::array<unsigned int, angle_bins> bins{};
				for (auto i = 0u; i < dpvs.smodelCount; i++)
				{
					const auto& origin = dpvs.smodelDrawInsts[i].placement.origin;
					const auto dx = origin[0] - view_origin[0];
					const auto dy = origin[1] - view_origin[1];
					if (dx * dx + dy * dy < cull_dists[i] * cull_dists[i])
					{
						const auto angle = std::atan2(dy, dx) / (2.0f * std::numbers::pi_v<float>) + 0.5f;
						bins[static_cast<int>(angle * angle_bins) % angle_bins]++;
					}
				}

				for (auto direction = 0; direction < angle_bins; direction++)
				{
					auto load = 0u;
					for (auto offset = -half_fov_bins; offset <= half_fov_bins; offset++)
					{
						load += bins[(direction + offset + angle_bins) % angle_bins];
					}

					worst = std::max(worst, load);
				}
			}

			return worst;
		}

		void fit_static_model_lighting_budget(zonetool::h1::GfxWorldDpvsStatic& dpvs)
		{
			const auto budget = 3600u; // 1.4 0x1405EEF40
			std::vector<float> radii(dpvs.smodelCount), cull_dists(dpvs.smodelCount);
			for (auto i = 0u; i < dpvs.smodelCount; i++)
			{
				const auto& half_size = dpvs.smodelInsts[i].bounds.halfSize;
				radii[i] = std::hypot(half_size[0], half_size[1], half_size[2]);
				const auto cull_dist = dpvs.smodelDrawInsts[i].cullDist;
				cull_dists[i] = cull_dist ? cull_dist : std::numeric_limits<float>::infinity();
			}

			const auto initial_load = get_worst_view_load(dpvs, cull_dists);
			if (initial_load <= budget)
			{
				ZONETOOL_INFO("GfxWorld static models: worst view lights %u of %u models", initial_load, budget);
				return;
			}

			const auto capped_dists = [&](const float radius_scale)
			{
				std::vector<float> dists(dpvs.smodelCount);
				for (auto i = 0u; i < dpvs.smodelCount; i++)
				{
					dists[i] = std::min(cull_dists[i], radii[i] * radius_scale);
				}

				return dists;
			};

			auto low = 1.0f, high = 1000.0f;
			for (auto step = 0; step < 16; step++)
			{
				const auto middle = std::sqrt(low * high);
				(get_worst_view_load(dpvs, capped_dists(middle)) <= budget ? low : high) = middle;
			}

			const auto dists = capped_dists(low);
			auto capped = 0u;
			for (auto i = 0u; i < dpvs.smodelCount; i++)
			{
				if (dists[i] >= cull_dists[i])
				{
					continue;
				}

				auto& inst = dpvs.smodelDrawInsts[i];
				inst.cullDist = to_h1_cull_dist(dists[i]);
				if (inst.reactiveMotionCullDist && inst.cullDist)
				{
					inst.reactiveMotionCullDist = std::min(inst.reactiveMotionCullDist, inst.cullDist);
				}

				capped += inst.cullDist ? 1u : 0u;
			}

			ZONETOOL_INFO("GfxWorld static models: worst view lights %u models, over the budget of %u, cull distance of %u small models capped at %.0f x radius",
				initial_load, budget, capped, low);
		}

		void convert_static_models(GfxWorld* asset, zonetool::h1::GfxWorld* world, utils::memory::allocator& allocator)
		{
			auto& dpvs = world->dpvs;
			const auto draw_insts = asset->dpvs.smodelDrawInsts;
			const auto insts = asset->dpvs.smodelInsts;
			std::unordered_set<std::string> missing_models;

			dpvs.smodelInsts = retype<zonetool::h1::GfxStaticModelInst>(insts);
			dpvs.smodelDrawInsts = allocate_zeroed<zonetool::h1::GfxStaticModelDrawInst>(allocator, dpvs.smodelCount);
			dpvs.smodelLightingInsts = allocate_zeroed<zonetool::h1::GfxStaticModelLighting>(allocator, dpvs.smodelCount);

			for (auto i = 0u; i < dpvs.smodelCount; i++)
			{
				const auto& inst = draw_insts[i];
				auto& new_inst = dpvs.smodelDrawInsts[i];
				new_inst.placement = convert_placement(inst);
				new_inst.model = make_stub<zonetool::h1::XModel>(inst.model ? inst.model->name : "void", allocator);
				const auto placement_scale = std::abs(new_inst.placement.scale) > 0.0f ? std::abs(new_inst.placement.scale) : 1.0f; // 1.4 0x1405A64BC
				new_inst.flags = zonetool::h1::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING | (inst.flags & (zonetool::h1::STATIC_MODEL_FLAG_NO_CAST_SHADOW | zonetool::h1::STATIC_MODEL_FLAG_ALLOW_FXMARK))
					| get_lod_scale_index(inst.lodScale / placement_scale);
				new_inst.cullDist = get_smodel_cull_dist(inst);
				const auto model_converted = inst.model && xmodel::is_dumped(inst.model->name);
				if (!model_converted && missing_models.insert(new_inst.model->name).second)
				{
					ZONETOOL_WARNING("static model %u: xmodel \"%s\" is not converted, h1 draws its default model", i, new_inst.model->name);
				}

				if (model_converted && xmodel::is_wind_sway(inst.model))
				{
					new_inst.flags |= zonetool::h1::STATIC_MODEL_FLAG_REACTIVEMOTION | zonetool::h1::STATIC_MODEL_FLAG_ANIMATED_VERTS;
					new_inst.reactiveMotionCullDist = new_inst.cullDist ? new_inst.cullDist : std::numeric_limits<std::uint16_t>::max();
					new_inst.reactiveMotionLOD = 6;
				}

				new_inst.staticModelId = inst.staticModelId;
				new_inst.reflectionProbeIndex = inst.reflectionProbeIndex;
				new_inst.primaryLightEnvIndex = get_light_env(asset, world, inst.lightListOffset, insts[i].bounds);
				new_inst.sunShadowFlags = (new_inst.flags & zonetool::h1::STATIC_MODEL_FLAG_NO_CAST_SHADOW) ? 0 : inst.sunShadowFlags;
			}

			dpvs.lodData = allocator.allocate_array<unsigned int>(dpvs.smodelCount + 1);
			std::iota(dpvs.lodData, dpvs.lodData + dpvs.smodelCount + 1, 0u);
		}

		void allocate_vis_data(zonetool::h1::GfxWorldDpvsStatic& dpvs, utils::memory::allocator& allocator)
		{
			const auto smodel_words = dpvs.smodelVisDataCount;
			const auto surface_words = dpvs.surfaceVisDataCount;

			for (auto& data : dpvs.smodelVisData) data = allocate_zeroed<unsigned int>(allocator, smodel_words);
			for (auto& data : dpvs.smodelUnknownVisData) data = allocate_zeroed<unsigned int>(allocator, smodel_words);
			for (auto& data : dpvs.surfaceVisData) data = allocate_zeroed<unsigned int>(allocator, surface_words);
			for (auto& data : dpvs.surfaceUnknownVisData) data = allocate_zeroed<unsigned int>(allocator, surface_words);
			for (auto& data : dpvs.smodelUmbraVisData) data = allocate_zeroed<unsigned int>(allocator, smodel_words + 1);
			for (auto& data : dpvs.surfaceUmbraVisData) data = allocate_zeroed<unsigned int>(allocator, surface_words);

			dpvs.unknownSModelVisData1 = allocate_zeroed<unsigned int>(allocator, smodel_words); // 1.4 0x1405AB020
			for (auto i = 0u; i < dpvs.smodelCount; i++)
			{
				const auto& inst = dpvs.smodelDrawInsts[i];
				if ((inst.flags & zonetool::h1::STATIC_MODEL_FLAG_REACTIVEMOTION) && inst.reactiveMotionLOD)
				{
					dpvs.unknownSModelVisData1[i >> 5] |= 0x80000000u >> (i & 31);
				}
			}

			dpvs.unknownSModelVisData2 = allocate_zeroed<unsigned int>(allocator, 2 * smodel_words);
			dpvs.tessellationCutoffVisData = allocate_zeroed<unsigned int>(allocator, surface_words);
			dpvs.surfaceCastsSunShadow = allocate_zeroed<unsigned int>(allocator, surface_words);
			dpvs.surfaceCastsSunShadowOpt = allocate_zeroed<unsigned int>(allocator, dpvs.sunShadowOptCount * dpvs.sunSurfVisDataCount);
			dpvs.surfaceDeptAndSurf = allocate_zeroed<zonetool::h1::GfxDepthAndSurf>(allocator, dpvs.staticSurfaceCount);

			const auto constant_buffer = allocator.allocate<char>();
			dpvs.constantBuffersLit = allocate_zeroed<zonetool::h1::GfxWorldDpvsVoid>(allocator, dpvs.smodelCount);
			dpvs.constantBuffersAmbient = allocate_zeroed<zonetool::h1::GfxWorldDpvsVoid>(allocator, dpvs.smodelCount);
			for (auto i = 0u; i < dpvs.smodelCount; i++)
			{
				dpvs.constantBuffersLit[i] = constant_buffer;
				dpvs.constantBuffersAmbient[i] = constant_buffer;
			}
		}

		void convert_dpvs(GfxWorld* asset, zonetool::h1::GfxWorld* world, utils::memory::allocator& allocator)
		{
			auto& dpvs = world->dpvs;
			dpvs.smodelCount = asset->dpvs.smodelCount;
			dpvs.staticSurfaceCount = asset->dpvs.staticSurfaceCount;
			dpvs.smodelVisDataCount = asset->dpvs.smodelVisDataCount;
			dpvs.surfaceVisDataCount = asset->dpvs.surfaceVisDataCount;
			dpvs.sunShadowOptCount = asset->dpvs.sunShadowOptCount;
			dpvs.sunSurfVisDataCount = asset->dpvs.sunSurfVisDataCount;

			const auto new_index = convert_surfaces(asset, world, allocator);
			world->shadowGeom = convert_shadow_geometry(asset->shadowGeom, world->primaryLightCount, new_index, allocator);
			world->shadowGeomOptimized = convert_shadow_geometry(asset->shadowGeomOptimized, world->primaryLightCount, new_index, allocator);

			convert_static_models(asset, world, allocator);
			if (!has_shadow_lists(world->shadowGeom, world->primaryLightCount))
			{
				world->shadowGeom = make_shadow_geometry(world, allocator);
			}

			set_light_env_shadow_geometry(world->shadowGeom, world, allocator);
			set_light_env_shadow_geometry(world->shadowGeomOptimized, world, allocator);

			allocate_vis_data(dpvs, allocator);
			dpvs.surfaceMaterials = allocate_zeroed<zonetool::h1::GfxDrawSurf>(allocator, world->surfaceCount);
		}

		void convert_draw(GfxWorld* asset, zonetool::h1::GfxWorld* world, utils::memory::allocator& allocator)
		{
			auto& draw = world->draw;
			const auto& source = asset->draw;

			draw.reflectionProbeCount = source.reflectionProbeCount;
			draw.reflectionProbes = allocate_zeroed<zonetool::h1::GfxImage*>(allocator, draw.reflectionProbeCount);
			for (auto i = 0u; i < draw.reflectionProbeCount; i++)
			{
				draw.reflectionProbes[i] = source.reflectionProbes[i] ? make_stub<zonetool::h1::GfxImage>(source.reflectionProbes[i]->name, allocator) : nullptr;
			}

			draw.reflectionProbeOrigins = retype<zonetool::h1::GfxReflectionProbe>(source.reflectionProbeOrigins);
			draw.reflectionProbeTextures = allocate_zeroed<zonetool::h1::GfxRawTexture>(allocator, draw.reflectionProbeCount);
			draw.reflectionProbeReferenceCount = source.reflectionProbeReferenceCount;
			draw.reflectionProbeReferenceOrigins = retype<zonetool::h1::GfxReflectionProbeReferenceOrigin>(source.reflectionProbeReferenceOrigins);
			draw.reflectionProbeReferences = retype<zonetool::h1::GfxReflectionProbeReference>(source.reflectionProbeReferences);

			draw.lightmapCount = static_cast<int>(source.lightmapOverrideCount);
			draw.lightmaps = allocate_zeroed<zonetool::h1::GfxLightmapArray>(allocator, draw.lightmapCount);
			for (auto i = 0; i < draw.lightmapCount; i++)
			{
				const auto& lightmap = source.lightmapOverrides[i];
				if (lightmap.primary)
				{
					draw.lightmaps[i].primary = make_stub<zonetool::h1::GfxImage>(lightmap.primary->name, allocator);
				}

				if (lightmap.secondary)
				{
					draw.lightmaps[i].secondary = make_stub<zonetool::h1::GfxImage>(lightmap.secondary->name, allocator);
				}
			}

			draw.lightmapPrimaryTextures = allocate_zeroed<zonetool::h1::GfxRawTexture>(allocator, draw.lightmapCount);
			draw.lightmapSecondaryTextures = allocate_zeroed<zonetool::h1::GfxRawTexture>(allocator, draw.lightmapCount);
			draw.lightmapParameters = std::bit_cast<zonetool::h1::GfxLightmapParameters>(source.lightmapParameters);

			convert_vertices(asset, world, allocator);
		}

		void convert_nodes(GfxWorld* asset, zonetool::h1::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto nodes = asset->dpvsPlanes.nodes;
			world->dpvsPlanes.nodes = allocate_zeroed<unsigned short>(allocator, world->nodeCount);
			for (auto i = 0; i < world->nodeCount; i++)
			{
				if (nodes[i] > std::numeric_limits<unsigned short>::max())
				{
					ZONETOOL_WARNING("GfxWorld \"%s\" node %d has value %u that does not fit 16 bits", asset->name, i, nodes[i]);
				}

				world->dpvsPlanes.nodes[i] = static_cast<unsigned short>(nodes[i]);
			}
		}

		void allocate_runtime_data(zonetool::h1::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto cell_count = static_cast<unsigned int>(world->dpvsPlanes.cellCount);
			const auto cell_words = (cell_count + 31) >> 5;
			const auto& client_counts = world->dpvsDyn.dynEntClientCount;
			const auto& word_counts = world->dpvsDyn.dynEntClientWordCount;
			const auto shadow_light_count = world->primaryLightCount - world->lastSunPrimaryLightIndex;

			world->dpvsPlanes.sceneEntCellBits = allocate_zeroed<unsigned int>(allocator, cell_count << 9);
			world->cellCasterBits = allocate_zeroed<unsigned int>(allocator, cell_count * cell_words);
			world->cellHasSunLitSurfsBits = allocate_zeroed<unsigned int>(allocator, cell_words);
			world->sceneDynModel = allocate_zeroed<zonetool::h1::GfxSceneDynModel>(allocator, client_counts[0]);
			world->sceneDynBrush = allocate_zeroed<zonetool::h1::GfxSceneDynBrush>(allocator, client_counts[1]);
			world->primaryLightEntityShadowVis = allocate_zeroed<unsigned int>(allocator, (shadow_light_count << 13) - 0x2000);
			world->primaryLightDynEntShadowVis[0] = allocate_zeroed<unsigned int>(allocator, client_counts[0] * (shadow_light_count - 1));
			world->primaryLightDynEntShadowVis[1] = allocate_zeroed<unsigned int>(allocator, client_counts[1] * (shadow_light_count - 1));
			world->nonSunPrimaryLightForModelDynEnt = allocate_zeroed<unsigned short>(allocator, client_counts[0]);

			for (auto client = 0u; client < 2; client++)
			{
				world->dpvsDyn.dynEntCellBits[client] = allocate_zeroed<unsigned int>(allocator, word_counts[client] * cell_count);
				for (auto& data : world->dpvsDyn.dynEntVisData[client])
				{
					data = allocate_zeroed<unsigned char>(allocator, 32 * word_counts[client]);
				}
			}
		}

		void convert_materials(GfxWorld* asset, zonetool::h1::GfxWorld* world, utils::memory::allocator& allocator)
		{
			world->materialMemoryCount = asset->materialMemoryCount;
			world->materialMemory = allocate_zeroed<zonetool::h1::MaterialMemory>(allocator, world->materialMemoryCount);
			for (auto i = 0; i < world->materialMemoryCount; i++)
			{
				const auto& memory = asset->materialMemory[i];
				world->materialMemory[i].memory = memory.memory;
				if (memory.material)
				{
					world->materialMemory[i].material = make_stub<zonetool::h1::Material>(memory.material->name, allocator);
				}
			}

			const auto stub_material = [&](const Material* material) -> zonetool::h1::Material*
			{
				return material ? make_stub<zonetool::h1::Material>(material->name, allocator) : nullptr;
			};

			world->sun = std::bit_cast<zonetool::h1::sunflare_t>(asset->sun);
			world->sun.spriteMaterial = stub_material(asset->sun.spriteMaterial);
			world->sun.flareMaterial = stub_material(asset->sun.flareMaterial);
		}

		void convert_images(GfxWorld* asset, zonetool::h1::GfxWorld* world, utils::memory::allocator& allocator)
		{
			world->skies = allocate_zeroed<zonetool::h1::GfxSky>(allocator, world->skyCount);
			for (auto i = 0; i < world->skyCount; i++)
			{
				world->skies[i] = std::bit_cast<zonetool::h1::GfxSky>(asset->skies[i]);
				if (asset->skies[i].skyImage)
				{
					const std::string name = asset->skies[i].skyImage->name;
					world->skies[i].skyImage = make_stub<zonetool::h1::GfxImage>(allocator.duplicate_string(gfximage::find_sky_cube(name).value_or(name)), allocator);
				}
			}

			if (asset->outdoorImage)
			{
				world->outdoorImage = make_stub<zonetool::h1::GfxImage>(asset->outdoorImage->name, allocator);
			}

			world->mdaoVolumesCount = asset->mdaoVolumesCount;
			world->mdaoVolumes = allocate_zeroed<zonetool::h1::MdaoVolume>(allocator, world->mdaoVolumesCount);
			for (auto i = 0u; i < world->mdaoVolumesCount; i++)
			{
				world->mdaoVolumes[i] = std::bit_cast<zonetool::h1::MdaoVolume>(asset->mdaoVolumes[i]);
				if (asset->mdaoVolumes[i].volumeData)
				{
					world->mdaoVolumes[i].volumeData = make_stub<zonetool::h1::GfxImage>(asset->mdaoVolumes[i].volumeData->name, allocator);
				}
			}
		}

		void disable_empty_light_grid_trees(zonetool::h1::GfxLightGrid& grid)
		{
			for (auto& tree : grid.tree)
			{
				if (tree.nodeCount)
				{
					continue;
				}

				for (auto axis = 0; axis < 3; axis++)
				{
					tree.coordMinGridSpace[axis] = 0x10000;
					tree.coordMaxGridSpace[axis] = 0;
				}
			}
		}

		void set_neutral_light_grid(zonetool::h1::GfxLightGrid& grid, utils::memory::allocator& allocator)
		{
			constexpr unsigned short neutral_color[3]{0x4173, 0x41FE, 0x426E};
			for (auto i = 0u; i < std::size(grid.defaultLightGridColors.rgb); i++)
			{
				std::memcpy(grid.skyLightGridColors.rgb[i], neutral_color, sizeof(neutral_color));
				std::memcpy(grid.defaultLightGridColors.rgb[i], neutral_color, sizeof(neutral_color));
			}

			grid.stageCount = 1;
			grid.stageLightingContrastGain = allocator.allocate<float>();
			*grid.stageLightingContrastGain = 0.3f;
		}

		zonetool::h1::GfxWorld* convert(GfxWorld* asset, utils::memory::allocator& allocator)
		{
			const auto world = allocator.allocate<zonetool::h1::GfxWorld>();
			world->name = asset->name;
			world->baseName = asset->baseName;
			world->bspVersion = asset->bspVersion;
			world->planeCount = asset->planeCount;
			world->nodeCount = asset->nodeCount;
			world->surfaceCount = asset->surfaceCount;
			world->skyCount = asset->skyCount;
			world->portalGroupCount = asset->portalGroupCount;
			world->lastSunPrimaryLightIndex = asset->lastSunPrimaryLightIndex;
			world->primaryLightCount = asset->primaryLightCount;
			world->sortKeyLitDecal = 7;
			world->sortKeyEffectDecal = 43;
			world->sortKeyTopDecal = 17;
			world->sortKeyEffectAuto = 53;
			world->sortKeyDistortion = 48;
			world->sortKeyHair = 18;
			world->sortKeyEffectBlend = 33;

			world->dpvsPlanes.cellCount = asset->dpvsPlanes.cellCount;
			world->dpvsPlanes.planes = retype<zonetool::h1::cplane_s>(asset->dpvsPlanes.planes);
			convert_nodes(asset, world, allocator);

			world->aabbTreeCounts = retype<zonetool::h1::GfxCellTreeCount>(asset->aabbTreeCounts);
			world->aabbTrees = retype<zonetool::h1::GfxCellTree>(asset->aabbTrees);
			world->cells = retype<zonetool::h1::GfxCell>(asset->cells);
			world->portalGroup = retype<zonetool::h1::GfxPortalGroup>(asset->portalGroup);
			world->portalDistanceAnchorCount = asset->portalDistanceAnchorCount;
			world->portalDistanceAnchorsAndCloseDistSquared = retype<zonetool::h1::vec4_t>(asset->portalDistanceAnchorsAndCloseDistSquared);

			convert_draw(asset, world, allocator);
			world->lightGrid = std::bit_cast<zonetool::h1::GfxLightGrid>(asset->lightGrid);
			set_neutral_light_grid(world->lightGrid, allocator);

			world->modelCount = asset->modelCount;
			world->models = retype<zonetool::h1::GfxBrushModel>(asset->models);
			world->bounds = std::bit_cast<zonetool::h1::Bounds>(asset->bounds);
			world->shadowBounds = std::bit_cast<zonetool::h1::Bounds>(asset->shadowBounds);
			world->checksum = asset->checksum;
			convert_materials(asset, world, allocator);
			std::memcpy(world->outdoorLookupMatrix, asset->outdoorLookupMatrix, sizeof(world->outdoorLookupMatrix));
			convert_images(asset, world, allocator);

			world->lightRegion = retype<zonetool::h1::GfxLightRegion>(asset->lightRegion);

			world->dpvsDyn = {};
			std::memcpy(world->dpvsDyn.dynEntClientWordCount, asset->dpvsDyn.dynEntClientWordCount, sizeof(world->dpvsDyn.dynEntClientWordCount));
			std::memcpy(world->dpvsDyn.dynEntClientCount, asset->dpvsDyn.dynEntClientCount, sizeof(world->dpvsDyn.dynEntClientCount));
			env_stats = {};
			convert_dpvs(asset, world, allocator);
			ZONETOOL_INFO("Light envs: %zu lights added by range, %zu listed lights out of range", env_stats.added, env_stats.listed_out_of_range);
			fit_static_model_lighting_budget(world->dpvs);
			convert_light_grid(asset, world, allocator);
			disable_empty_light_grid_trees(world->lightGrid);
			world->primaryLightEnvCount = comworld::get_light_env_count();
			allocate_runtime_data(world, allocator);

			world->mapVtxChecksum = asset->mapVtxChecksum;
			if (asset->heroOnlyLightCount)
			{
				ZONETOOL_WARNING("GfxWorld \"%s\" has %u hero only lights that are not converted", asset->name, asset->heroOnlyLightCount);
			}

			world->fogTypesAllowed = zonetool::h1::FOG_NORMAL | zonetool::h1::FOG_DFOG;
			world->buildInfo = std::bit_cast<zonetool::h1::GfxBuildInfo>(asset->buildInfo);
			return world;
		}
	}

	void dump(GfxWorld* asset)
	{
		pending_world = asset;
	}

	void add_transient_zone(GfxMapTrZone* zone)
	{
		const std::string_view name = zone->name;
		const auto index = std::atoi(name.substr(name.find_last_of('_') + 1).data());
		if (index < 0 || index >= static_cast<int>(transient_zone_count))
		{
			ZONETOOL_WARNING("GfxMapTrZone \"%s\" has no valid transient zone index", zone->name);
			return;
		}

		loaded_transient_zones[index] = zone;
	}

	void reference_models()
	{
		for (auto i = 0u; pending_world && i < pending_world->dpvs.smodelCount; i++)
		{
			xmodel::reference(pending_world->dpvs.smodelDrawInsts[i].model);
		}
	}

	void finish()
	{
		if (!pending_world)
		{
			return;
		}

		utils::memory::allocator allocator;
		const auto world = convert(pending_world, allocator);
		zonetool::h1::gfx_world::dump(world);
		add_csv_row("gfx_map", world->name);
		clear_stubs();
		pending_world = nullptr;
		loaded_transient_zones = {};
	}
}
