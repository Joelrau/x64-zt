#include <std_include.hpp>

#include "../include.hpp"
#include "gfxworld.hpp"
#include "comworld.hpp"
#include "gfximage.hpp"
#include "material.hpp"

#include <h1/assets/gfxworld.hpp>

namespace zonetool::s2::converter::h1::gfxworld
{
	namespace
	{
		namespace target = zonetool::h1;

		struct s2_surface
		{
			std::uint32_t vertexLayerData;
			std::uint32_t firstVertex;
			std::uint16_t vertexCount;
			std::uint16_t triCount;
			char __pad0[12];
			std::uint32_t baseIndex;
			char __pad1[4];
			Material* material;
			std::uint8_t lightmapIndex;
			std::uint8_t reflectionProbeIndex;
			std::uint8_t flags;
			char __pad2[1];
			std::uint16_t lightListOffset;
			char __pad3[2];
			std::uint32_t transientZone;
			char __pad4[4];
		};
		static_assert(sizeof(s2_surface) == 56);
		static_assert(offsetof(s2_surface, baseIndex) == 24);
		static_assert(offsetof(s2_surface, material) == 32);
		static_assert(offsetof(s2_surface, lightListOffset) == 44);
		static_assert(offsetof(s2_surface, transientZone) == 48);

		struct s2_static_model_draw_inst
		{
			target::GfxPackedPlacement placement;
			char __pad0[20];
			XModel* model;
			char __pad1[4];
			std::uint16_t flags;
			char __pad2[2];
			std::uint16_t staticModelId;
			char __pad3[16];
			std::uint16_t lightListOffset;
			char __pad5[4];
			std::uint8_t reflectionProbeIndex;
			char __pad4[15];
		};
		static_assert(sizeof(s2_static_model_draw_inst) == 128);
		static_assert(offsetof(s2_static_model_draw_inst, model) == 0x48);
		static_assert(offsetof(s2_static_model_draw_inst, flags) == 0x54);
		static_assert(offsetof(s2_static_model_draw_inst, staticModelId) == 0x58);
		static_assert(offsetof(s2_static_model_draw_inst, lightListOffset) == 0x6A);
		static_assert(offsetof(s2_static_model_draw_inst, reflectionProbeIndex) == 0x70);

		struct s2_surface_bounds
		{
			target::Bounds bounds;
			char __pad0[40];
		};
		static_assert(sizeof(s2_surface_bounds) == 64);

		struct s2_lightmap_override
		{
			GfxImage* primary;
			GfxImage* secondary;
		};

		struct s2_smodel_light_volume
		{
			std::uint32_t lightingIndex;
			std::uint16_t width;
			std::uint16_t height;
			std::uint32_t depth;
			std::uint32_t firstProbe;
			float worldToVolume[12];
		};
		static_assert(sizeof(s2_smodel_light_volume) == 64);

		struct s2_smodel_probe
		{
			std::uint16_t sh[3][9];
			std::uint16_t sunVisibility;
			char __pad0[8];
		};
		static_assert(sizeof(s2_smodel_probe) == 64);

		constexpr std::size_t world_bsp_version = 16;
		constexpr std::size_t world_plane_count = 20;
		constexpr std::size_t world_node_count = 24;
		constexpr std::size_t world_surface_count = 28;
		constexpr std::size_t world_sky_count = 32;
		constexpr std::size_t world_skies = 40;
		constexpr std::size_t world_portal_group_count = 48;
		constexpr std::size_t world_last_sun_primary_light = 52;
		constexpr std::size_t world_primary_light_count = 56;
		constexpr std::size_t world_primary_light_env_count = 60;
		constexpr std::size_t world_cell_count = 88;
		constexpr std::size_t world_planes = 96;
		constexpr std::size_t world_nodes = 104;
		constexpr std::size_t world_aabb_tree_counts = 128;
		constexpr std::size_t world_aabb_trees = 136;
		constexpr std::size_t world_cells = 152;
		constexpr std::size_t world_portal_groups = 160;
		constexpr std::size_t world_anchor_count = 168;
		constexpr std::size_t world_anchors = 176;
		constexpr std::size_t world_draw = 192;
		constexpr std::size_t world_light_grid = 992;
		constexpr std::size_t world_probe_grid = 2072;
		constexpr std::size_t world_light_list_size = 2872;
		constexpr std::size_t world_light_lists = 2880;
		constexpr std::size_t world_model_count = 2888;
		constexpr std::size_t world_models = 2896;
		constexpr std::size_t world_bounds = 2904;
		constexpr std::size_t world_shadow_bounds = 2928;
		constexpr std::size_t world_checksum = 2952;
		constexpr std::size_t world_material_memory_count = 2956;
		constexpr std::size_t world_material_memory = 2960;
		constexpr std::size_t world_sun = 2968;
		constexpr std::size_t world_outdoor_lookup_matrix = 3080;
		constexpr std::size_t world_outdoor_image = 3144;
		constexpr std::size_t world_shadow_geom = 3216;
		constexpr std::size_t world_shadow_geom_optimized = 3224;
		constexpr std::size_t world_light_region = 3232;
		constexpr std::size_t world_dpvs = 3240;
		constexpr std::size_t world_dpvs_dynamic = 4104;
		constexpr std::size_t world_map_vtx_checksum = 4200;
		constexpr std::size_t world_hero_only_light_count = 4204;
		constexpr std::size_t world_mdao_volume_count = 4220;
		constexpr std::size_t world_mdao_volumes = 4224;
		constexpr std::size_t world_build_info = 4264;

		constexpr std::size_t draw_reflection_probe_count = 0;
		constexpr std::size_t draw_reflection_probes = 8;
		constexpr std::size_t draw_reflection_probe_origins = 16;
		constexpr std::size_t draw_reflection_probe_reference_count = 32;
		constexpr std::size_t draw_reflection_probe_reference_origins = 40;
		constexpr std::size_t draw_reflection_probe_references = 48;
		constexpr std::size_t draw_lightmap_override_count = 156;
		constexpr std::size_t draw_lightmap_overrides = 160;
		constexpr std::size_t draw_lightmap_parameters = 216;
		constexpr std::size_t draw_transient_zones = 640;
		constexpr std::size_t draw_transient_zone_count = 16;
		constexpr std::size_t draw_index_count = 776;
		constexpr std::size_t draw_indices = 784;

		constexpr std::size_t probe_grid_smodel_volume_count = 192;
		constexpr std::size_t probe_grid_smodel_volumes = 200;
		constexpr std::size_t probe_grid_smodel_probe_count = 208;
		constexpr std::size_t probe_grid_smodel_probes = 216;

		constexpr std::size_t dpvs_smodel_count = 0;
		constexpr std::size_t dpvs_static_surface_count = 4;
		constexpr std::size_t dpvs_smodel_vis_data_count = 72;
		constexpr std::size_t dpvs_surface_vis_data_count = 76;
		constexpr std::size_t dpvs_sorted_surf_index = 744;
		constexpr std::size_t dpvs_surfaces = 752;
		constexpr std::size_t dpvs_surfaces_bounds = 760;
		constexpr std::size_t dpvs_smodel_insts = 776;
		constexpr std::size_t dpvs_smodel_draw_insts = 784;
		constexpr std::size_t dpvs_smodel_lighting = 800;
		constexpr std::size_t dpvs_sun_shadow_opt_count = 824;
		constexpr std::size_t dpvs_sun_surf_vis_data_count = 828;

		constexpr std::uint16_t smodel_flags_kept = target::STATIC_MODEL_FLAG_NO_CAST_SHADOW | target::STATIC_MODEL_FLAG_GROUND_LIGHTING |
			target::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING | target::STATIC_MODEL_FLAG_LIGHTMAP_LIGHTING | target::STATIC_MODEL_FLAG_ALLOW_FXMARK;
		constexpr std::uint16_t smodel_scale_one = target::STATIC_MODEL_FLAG_SCALE_9;
		constexpr std::uint16_t sun_primary_light_index = 1;

		// unk bytes 2..10 are per image slot stream distances (0xFF is far)
		constexpr std::size_t surface_image_stream_distance_size = 11; // distance cull flag

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
		std::array<GfxMapTrZone*, draw_transient_zone_count> loaded_transient_zones{};
		std::array<std::uint32_t, draw_transient_zone_count> transient_vertex_base{};
		std::array<std::uint32_t, draw_transient_zone_count> transient_layer_data_base{};

		template <typename T>
		T& field(GfxWorld* asset, const std::size_t offset)
		{
			return *reinterpret_cast<T*>(reinterpret_cast<std::uint8_t*>(asset) + offset);
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
			stubs<target::Material>().clear();
			stubs<target::XModel>().clear();
			stubs<target::GfxImage>().clear();
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
			case target::CAMERA_REGION_LIT_OPAQUE:
				return range_lit_opaque;
			case target::CAMERA_REGION_LIT_OPAQUE_SSS:
				return range_lit_opaque_sss;
			case target::CAMERA_REGION_LIT_DECAL:
				return range_lit_decal;
			case target::CAMERA_REGION_EMISSIVE:
				return range_emissive;
			case target::CAMERA_REGION_NONE:
				return range_shadow_caster;
			default:
				return range_lit_trans;
			}
		}

		std::array<GfxMapTrZone*, draw_transient_zone_count> get_transient_zones(GfxWorld* asset)
		{
			auto zones = loaded_transient_zones;
			const auto draw_zones = &field<GfxMapTrZone*>(asset, world_draw + draw_transient_zones);
			for (auto i = 0u; i < draw_transient_zone_count; i++)
			{
				if (draw_zones[i])
				{
					zones[i] = draw_zones[i];
				}
			}

			return zones;
		}

		void convert_vertices(GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto zones = get_transient_zones(asset);
			for (auto i = 0u; i < draw_transient_zone_count; i++)
			{
				transient_vertex_base[i] = world->draw.vertexCount;
				transient_layer_data_base[i] = world->draw.vertexLayerDataSize;
				if (zones[i])
				{
					world->draw.vertexCount += zones[i]->vertexCount;
					world->draw.vertexLayerDataSize += zones[i]->indexDataSize;
				}
			}

			world->draw.vd.vertices = allocate_zeroed<target::GfxWorldVertex>(allocator, world->draw.vertexCount);
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
					std::memcpy(&vertex->binormalSign, &data.data0[i], sizeof(vertex->binormalSign));
					vertex->color.packed = data.data1[i];
					std::memcpy(vertex->texCoord, &data.data2[i], sizeof(vertex->texCoord));
					std::memcpy(vertex->lmapCoord, &data.data3[i], sizeof(vertex->lmapCoord));
					vertex->normal.packed = data.data4[i];
					vertex->tangent.packed = data.data5[i];
				}

				if (zone->indices.data)
				{
					std::memcpy(layer_data, zone->indices.data, zone->indexDataSize);
				}

				layer_data += zone->indexDataSize;
			}

			world->draw.indexCount = field<std::uint32_t>(asset, world_draw + draw_index_count);
			world->draw.indices = field<unsigned short*>(asset, world_draw + draw_indices);
		}

		float get_max_edge_length(const target::GfxWorld* world, const target::srfTriangles_t& surface)
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

		// S2 surfaces and static models store an offset into a counted list of primary lights, H1 stores an index into the ComWorld light envs
		std::uint16_t get_light_env(GfxWorld* asset, const target::GfxWorld* world, const std::uint16_t list_offset)
		{
			const auto lists = field<std::uint16_t*>(asset, world_light_lists);
			const auto list_size = field<std::uint32_t>(asset, world_light_list_size);
			if (!lists || list_offset >= list_size)
			{
				return 0;
			}

			std::vector<std::uint16_t> lights;
			const auto count = std::min<std::uint32_t>(lists[list_offset], list_size - list_offset - 1);
			for (auto i = 0u; i < count; i++)
			{
				const auto light = lists[list_offset + 1 + i];
				const auto is_extra_sun = light > sun_primary_light_index && light <= world->lastSunPrimaryLightIndex;
				if (light >= world->primaryLightCount || is_extra_sun)
				{
					continue;
				}

				lights.push_back(light);
			}

			std::sort(lights.begin(), lights.end());
			return comworld::get_light_env(std::move(lights));
		}

		target::GfxSurface convert_surface(const target::GfxWorld* world, const s2_surface& surface, const std::uint16_t light_env, utils::memory::allocator& allocator)
		{
			target::GfxSurface new_surface{};
			const auto zone = surface.transientZone < draw_transient_zone_count ? surface.transientZone : 0;
			new_surface.tris.vertexLayerData = transient_layer_data_base[zone] + surface.vertexLayerData;
			new_surface.tris.firstVertex = transient_vertex_base[zone] + surface.firstVertex;
			new_surface.tris.unk = -1;
			new_surface.tris.vertexCount = surface.vertexCount;
			new_surface.tris.triCount = surface.triCount;
			new_surface.tris.baseIndex = surface.baseIndex;
			new_surface.tris.maxEdgeLength = get_max_edge_length(world, new_surface.tris);
			new_surface.material = make_stub<target::Material>(surface.material ? surface.material->name : "$default", allocator);
			new_surface.laf.fields.lightmapIndex = surface.lightmapIndex;
			new_surface.laf.fields.reflectionProbeIndex = surface.reflectionProbeIndex;
			new_surface.laf.fields.primaryLightEnvIndex = light_env;
			new_surface.laf.fields.flags = target::GFX_SURFACE_CASTS_SUN_SHADOW;

			return new_surface;
		}

		void set_surface_ranges(target::GfxWorldDpvsStatic& dpvs, const std::array<unsigned int, range_count>& range_sizes)
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

		std::vector<unsigned int> convert_surfaces(GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			auto& dpvs = world->dpvs;
			const auto surfaces = field<s2_surface*>(asset, world_dpvs + dpvs_surfaces);
			const auto bounds = field<s2_surface_bounds*>(asset, world_dpvs + dpvs_surfaces_bounds);

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

			dpvs.surfaces = allocator.allocate_array<target::GfxSurface>(world->surfaceCount);
			dpvs.surfacesBounds = allocator.allocate_array<target::GfxSurfaceBounds>(world->surfaceCount);
			for (auto i = 0u; i < world->surfaceCount; i++)
			{
				dpvs.surfaces[new_index[i]] = convert_surface(world, surfaces[i], get_light_env(asset, world, surfaces[i].lightListOffset), allocator);
				dpvs.surfacesBounds[new_index[i]].bounds = bounds[i].bounds;
				std::memset(&dpvs.surfacesBounds[new_index[i]].unk, 0xFF, surface_image_stream_distance_size);
			}

			set_surface_ranges(dpvs, range_sizes);

			const auto sorted_surf_index = field<unsigned int*>(asset, world_dpvs + dpvs_sorted_surf_index);
			dpvs.sortedSurfIndex = allocator.allocate_array<unsigned int>(dpvs.staticSurfaceCount);
			for (auto i = 0u; i < dpvs.staticSurfaceCount; i++)
			{
				dpvs.sortedSurfIndex[i] = new_index[sorted_surf_index[i]];
			}

			ZONETOOL_INFO("GfxWorld surfaces: %u opaque, %u decal, %u trans, %u shadow caster, %u emissive, %u brush",
				range_sizes[range_lit_opaque] + range_sizes[range_lit_opaque_sss], range_sizes[range_lit_decal], range_sizes[range_lit_trans],
				range_sizes[range_shadow_caster], range_sizes[range_emissive], world->surfaceCount - dpvs.staticSurfaceCount);

			return new_index;
		}

		target::GfxShadowGeometry* convert_shadow_geometry(const target::GfxShadowGeometry* geometry, const unsigned int count,
			const std::vector<unsigned int>& new_index, utils::memory::allocator& allocator)
		{
			if (!geometry)
			{
				return nullptr;
			}

			const auto new_geometry = allocator.allocate_array<target::GfxShadowGeometry>(count);
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

		std::optional<target::GfxColorHdr> get_smodel_ground_lighting(GfxWorld* asset, const target::GfxStaticModelLighting& lighting)
		{
			const auto volume_count = field<std::uint32_t>(asset, world_probe_grid + probe_grid_smodel_volume_count);
			const auto volumes = field<s2_smodel_light_volume*>(asset, world_probe_grid + probe_grid_smodel_volumes);
			const auto probe_count = field<std::uint32_t>(asset, world_probe_grid + probe_grid_smodel_probe_count);
			const auto probes = field<s2_smodel_probe*>(asset, world_probe_grid + probe_grid_smodel_probes);

			std::uint32_t lighting_index;
			std::memcpy(&lighting_index, &lighting, sizeof(lighting_index));
			if (!volumes || !probes || lighting_index >= volume_count)
			{
				return {};
			}

			const auto& volume = volumes[lighting_index];
			const auto count = static_cast<std::uint64_t>(volume.width) * volume.height * volume.depth;
			if (!count || volume.firstProbe + count > probe_count)
			{
				return {};
			}

			std::array<float, 4> sum{};
			for (auto i = 0ull; i < count; i++)
			{
				const auto& probe = probes[volume.firstProbe + i];
				for (auto channel = 0; channel < 3; channel++)
				{
					sum[channel] += half_float::half_to_float(probe.sh[channel][0]);
				}

				sum[3] += half_float::half_to_float(probe.sunVisibility);
			}

			target::GfxColorHdr color{};
			for (auto channel = 0u; channel < sum.size(); channel++)
			{
				color.array[channel] = half_float::float_to_half(std::max(0.0f, sum[channel] / static_cast<float>(count)));
			}

			return color;
		}

		void convert_static_models(GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			auto& dpvs = world->dpvs;
			const auto draw_insts = field<s2_static_model_draw_inst*>(asset, world_dpvs + dpvs_smodel_draw_insts);
			const auto lighting = field<target::GfxStaticModelLighting*>(asset, world_dpvs + dpvs_smodel_lighting);

			dpvs.smodelInsts = field<target::GfxStaticModelInst*>(asset, world_dpvs + dpvs_smodel_insts);
			dpvs.smodelDrawInsts = allocate_zeroed<target::GfxStaticModelDrawInst>(allocator, dpvs.smodelCount);
			dpvs.smodelLightingInsts = allocate_zeroed<target::GfxStaticModelLighting>(allocator, dpvs.smodelCount);

			auto missing_ground_lighting = 0u;
			for (auto i = 0u; i < dpvs.smodelCount; i++)
			{
				const auto& inst = draw_insts[i];
				auto& new_inst = dpvs.smodelDrawInsts[i];
				new_inst.placement = inst.placement;
				new_inst.model = make_stub<target::XModel>(inst.model ? inst.model->name : "void", allocator);
				new_inst.flags = (inst.flags & smodel_flags_kept) | smodel_scale_one;
				new_inst.staticModelId = inst.staticModelId;
				new_inst.reflectionProbeIndex = inst.reflectionProbeIndex;
				new_inst.primaryLightEnvIndex = get_light_env(asset, world, inst.lightListOffset);
				new_inst.sunShadowFlags = (new_inst.flags & target::STATIC_MODEL_FLAG_NO_CAST_SHADOW) ? 0 : 1;

				dpvs.smodelLightingInsts[i] = lighting[i];
				if (new_inst.flags & target::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING)
				{
					new_inst.flags = (new_inst.flags & ~target::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING) | target::STATIC_MODEL_FLAG_GROUND_LIGHTING;
					dpvs.smodelLightingInsts[i] = {};
					if (const auto ground_lighting = get_smodel_ground_lighting(asset, lighting[i]))
					{
						dpvs.smodelLightingInsts[i].ambientLightingInfo.groundLighting = *ground_lighting;
					}
					else
					{
						missing_ground_lighting++;
					}
				}
			}

			if (missing_ground_lighting)
			{
				ZONETOOL_WARNING("GfxWorld \"%s\" has %u light grid static models without probe lighting", asset->name, missing_ground_lighting);
			}

			dpvs.lodData = allocator.allocate_array<unsigned int>(dpvs.smodelCount + 1);
			std::iota(dpvs.lodData, dpvs.lodData + dpvs.smodelCount + 1, 0u);
		}

		void allocate_vis_data(target::GfxWorldDpvsStatic& dpvs, utils::memory::allocator& allocator)
		{
			const auto smodel_words = dpvs.smodelVisDataCount;
			const auto surface_words = dpvs.surfaceVisDataCount;

			for (auto& data : dpvs.smodelVisData) data = allocate_zeroed<unsigned int>(allocator, smodel_words);
			for (auto& data : dpvs.smodelUnknownVisData) data = allocate_zeroed<unsigned int>(allocator, smodel_words);
			for (auto& data : dpvs.surfaceVisData) data = allocate_zeroed<unsigned int>(allocator, surface_words);
			for (auto& data : dpvs.surfaceUnknownVisData) data = allocate_zeroed<unsigned int>(allocator, surface_words);
			for (auto& data : dpvs.smodelUmbraVisData) data = allocate_zeroed<unsigned int>(allocator, smodel_words + 1);
			for (auto& data : dpvs.surfaceUmbraVisData) data = allocate_zeroed<unsigned int>(allocator, surface_words);

			dpvs.unknownSModelVisData1 = allocate_zeroed<unsigned int>(allocator, smodel_words);
			dpvs.unknownSModelVisData2 = allocate_zeroed<unsigned int>(allocator, 2 * smodel_words);
			dpvs.tessellationCutoffVisData = allocate_zeroed<unsigned int>(allocator, surface_words);
			dpvs.surfaceCastsSunShadow = allocate_zeroed<unsigned int>(allocator, surface_words);
			dpvs.surfaceCastsSunShadowOpt = allocate_zeroed<unsigned int>(allocator, dpvs.sunShadowOptCount * dpvs.sunSurfVisDataCount);
			dpvs.surfaceDeptAndSurf = allocate_zeroed<target::GfxDepthAndSurf>(allocator, dpvs.staticSurfaceCount);

			const auto constant_buffer = allocator.allocate<char>();
			dpvs.constantBuffersLit = allocate_zeroed<target::GfxWorldDpvsVoid>(allocator, dpvs.smodelCount);
			dpvs.constantBuffersAmbient = allocate_zeroed<target::GfxWorldDpvsVoid>(allocator, dpvs.smodelCount);
			for (auto i = 0u; i < dpvs.smodelCount; i++)
			{
				dpvs.constantBuffersLit[i] = constant_buffer;
				dpvs.constantBuffersAmbient[i] = constant_buffer;
			}
		}

		void convert_dpvs(GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			auto& dpvs = world->dpvs;
			dpvs.smodelCount = field<unsigned int>(asset, world_dpvs + dpvs_smodel_count);
			dpvs.staticSurfaceCount = field<unsigned int>(asset, world_dpvs + dpvs_static_surface_count);
			dpvs.smodelVisDataCount = field<unsigned int>(asset, world_dpvs + dpvs_smodel_vis_data_count);
			dpvs.surfaceVisDataCount = field<unsigned int>(asset, world_dpvs + dpvs_surface_vis_data_count);
			dpvs.sunShadowOptCount = field<unsigned int>(asset, world_dpvs + dpvs_sun_shadow_opt_count);
			dpvs.sunSurfVisDataCount = field<unsigned int>(asset, world_dpvs + dpvs_sun_surf_vis_data_count);

			const auto new_index = convert_surfaces(asset, world, allocator);
			world->shadowGeom = convert_shadow_geometry(field<target::GfxShadowGeometry*>(asset, world_shadow_geom),
				world->primaryLightCount, new_index, allocator);
			world->shadowGeomOptimized = convert_shadow_geometry(field<target::GfxShadowGeometry*>(asset, world_shadow_geom_optimized),
				world->primaryLightCount, new_index, allocator);

			convert_static_models(asset, world, allocator);
			allocate_vis_data(dpvs, allocator);
			dpvs.surfaceMaterials = allocate_zeroed<target::GfxDrawSurf>(allocator, world->surfaceCount);
		}

		void convert_draw(GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			auto& draw = world->draw;
			draw.reflectionProbeCount = field<unsigned int>(asset, world_draw + draw_reflection_probe_count);
			draw.reflectionProbes = allocate_zeroed<target::GfxImage*>(allocator, draw.reflectionProbeCount);
			const auto probes = field<GfxImage**>(asset, world_draw + draw_reflection_probes);
			for (auto i = 0u; i < draw.reflectionProbeCount; i++)
			{
				draw.reflectionProbes[i] = probes[i] ? make_stub<target::GfxImage>(probes[i]->name, allocator) : nullptr;
			}

			draw.reflectionProbeOrigins = field<target::GfxReflectionProbe*>(asset, world_draw + draw_reflection_probe_origins);
			draw.reflectionProbeTextures = allocate_zeroed<target::GfxRawTexture>(allocator, draw.reflectionProbeCount);
			draw.reflectionProbeReferenceCount = field<unsigned int>(asset, world_draw + draw_reflection_probe_reference_count);
			draw.reflectionProbeReferenceOrigins = field<target::GfxReflectionProbeReferenceOrigin*>(asset, world_draw + draw_reflection_probe_reference_origins);
			draw.reflectionProbeReferences = field<target::GfxReflectionProbeReference*>(asset, world_draw + draw_reflection_probe_references);

			draw.lightmapCount = static_cast<int>(field<unsigned int>(asset, world_draw + draw_lightmap_override_count));
			draw.lightmaps = allocate_zeroed<target::GfxLightmapArray>(allocator, draw.lightmapCount);
			const auto lightmaps = field<s2_lightmap_override*>(asset, world_draw + draw_lightmap_overrides);
			for (auto i = 0; i < draw.lightmapCount; i++)
			{
				if (lightmaps[i].primary)
				{
					draw.lightmaps[i].primary = make_stub<target::GfxImage>(lightmaps[i].primary->name, allocator);
				}

				if (lightmaps[i].secondary)
				{
					draw.lightmaps[i].secondary = make_stub<target::GfxImage>(lightmaps[i].secondary->name, allocator);
				}
			}

			draw.lightmapPrimaryTextures = allocate_zeroed<target::GfxRawTexture>(allocator, draw.lightmapCount);
			draw.lightmapSecondaryTextures = allocate_zeroed<target::GfxRawTexture>(allocator, draw.lightmapCount);
			draw.lightmapParameters = field<target::GfxLightmapParameters>(asset, world_draw + draw_lightmap_parameters);

			convert_vertices(asset, world, allocator);
		}

		void convert_nodes(GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto nodes = field<std::uint32_t*>(asset, world_nodes);
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

		void allocate_runtime_data(target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			const auto cell_count = static_cast<unsigned int>(world->dpvsPlanes.cellCount);
			const auto cell_words = (cell_count + 31) >> 5;
			const auto& client_counts = world->dpvsDyn.dynEntClientCount;
			const auto& word_counts = world->dpvsDyn.dynEntClientWordCount;
			const auto shadow_light_count = world->primaryLightCount - world->lastSunPrimaryLightIndex;

			world->dpvsPlanes.sceneEntCellBits = allocate_zeroed<unsigned int>(allocator, cell_count << 9);
			world->cellCasterBits = allocate_zeroed<unsigned int>(allocator, cell_count * cell_words);
			world->cellHasSunLitSurfsBits = allocate_zeroed<unsigned int>(allocator, cell_words);
			world->sceneDynModel = allocate_zeroed<target::GfxSceneDynModel>(allocator, client_counts[0]);
			world->sceneDynBrush = allocate_zeroed<target::GfxSceneDynBrush>(allocator, client_counts[1]);
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

		void convert_materials(GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			world->materialMemoryCount = field<int>(asset, world_material_memory_count);
			world->materialMemory = allocate_zeroed<target::MaterialMemory>(allocator, world->materialMemoryCount);
			const auto memory = field<target::MaterialMemory*>(asset, world_material_memory);
			for (auto i = 0; i < world->materialMemoryCount; i++)
			{
				world->materialMemory[i].memory = memory[i].memory;
				if (memory[i].material)
				{
					world->materialMemory[i].material = make_stub<target::Material>(reinterpret_cast<Material*>(memory[i].material)->name, allocator);
				}
			}

			std::memcpy(&world->sun, &field<std::uint8_t>(asset, world_sun), sizeof(world->sun));
			const auto sun_material = [&](target::Material*& material)
			{
				if (material)
				{
					material = make_stub<target::Material>(reinterpret_cast<Material*>(material)->name, allocator);
				}
			};
			sun_material(world->sun.spriteMaterial);
			sun_material(world->sun.flareMaterial);
		}

		void convert_images(GfxWorld* asset, target::GfxWorld* world, utils::memory::allocator& allocator)
		{
			world->skies = allocate_zeroed<target::GfxSky>(allocator, world->skyCount);
			const auto skies = field<target::GfxSky*>(asset, world_skies);
			for (auto i = 0; i < world->skyCount; i++)
			{
				world->skies[i] = skies[i];
				if (skies[i].skyImage)
				{
					const std::string name = reinterpret_cast<GfxImage*>(skies[i].skyImage)->name;
					world->skies[i].skyImage = make_stub<target::GfxImage>(allocator.duplicate_string(gfximage::find_sky_cube(name).value_or(name)), allocator);
				}
			}

			if (const auto outdoor = field<GfxImage*>(asset, world_outdoor_image))
			{
				world->outdoorImage = make_stub<target::GfxImage>(outdoor->name, allocator);
			}

			world->mdaoVolumesCount = field<unsigned int>(asset, world_mdao_volume_count);
			world->mdaoVolumes = allocate_zeroed<target::MdaoVolume>(allocator, world->mdaoVolumesCount);
			const auto volumes = field<target::MdaoVolume*>(asset, world_mdao_volumes);
			for (auto i = 0u; i < world->mdaoVolumesCount; i++)
			{
				world->mdaoVolumes[i] = volumes[i];
				if (volumes[i].volumeData)
				{
					world->mdaoVolumes[i].volumeData = make_stub<target::GfxImage>(reinterpret_cast<GfxImage*>(volumes[i].volumeData)->name, allocator);
				}
			}
		}

		void disable_empty_light_grid_trees(target::GfxLightGrid& grid)
		{
			for (auto& tree : grid.tree)
			{
				if (tree.nodeCount)
				{
					continue;
				}

				// R_LightgridLookup walks p_nodeTable when the grid point is inside the bounds, so an empty tree needs min > max
				for (auto axis = 0; axis < 3; axis++)
				{
					tree.coordMinGridSpace[axis] = 0x10000;
					tree.coordMaxGridSpace[axis] = 0;
				}
			}
		}

		void set_neutral_light_grid(target::GfxLightGrid& grid, utils::memory::allocator& allocator)
		{
			// S2 lights models with a probe grid that H1 cannot read, so stock H1 maps use these half-float ambient colors and one stage
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

		target::GfxWorld* convert(GfxWorld* asset, utils::memory::allocator& allocator)
		{
			const auto world = allocator.allocate<target::GfxWorld>();
			world->name = asset->name;
			world->baseName = asset->baseName;
			world->bspVersion = field<unsigned int>(asset, world_bsp_version);
			world->planeCount = field<int>(asset, world_plane_count);
			world->nodeCount = field<int>(asset, world_node_count);
			world->surfaceCount = field<unsigned int>(asset, world_surface_count);
			world->skyCount = field<int>(asset, world_sky_count);
			world->portalGroupCount = field<unsigned int>(asset, world_portal_group_count);
			world->lastSunPrimaryLightIndex = field<unsigned int>(asset, world_last_sun_primary_light);
			world->primaryLightCount = field<unsigned int>(asset, world_primary_light_count);
			world->primaryLightEnvCount = field<unsigned int>(asset, world_primary_light_env_count);
			world->sortKeyLitDecal = 7;
			world->sortKeyEffectDecal = 43;
			world->sortKeyTopDecal = 17;
			world->sortKeyEffectAuto = 53;
			world->sortKeyDistortion = 48;
			world->sortKeyHair = 18;
			world->sortKeyEffectBlend = 33;

			world->dpvsPlanes.cellCount = field<int>(asset, world_cell_count);
			world->dpvsPlanes.planes = field<target::cplane_s*>(asset, world_planes);
			convert_nodes(asset, world, allocator);

			world->aabbTreeCounts = field<target::GfxCellTreeCount*>(asset, world_aabb_tree_counts);
			world->aabbTrees = field<target::GfxCellTree*>(asset, world_aabb_trees);
			world->cells = field<target::GfxCell*>(asset, world_cells);
			world->portalGroup = field<target::GfxPortalGroup*>(asset, world_portal_groups);
			world->portalDistanceAnchorCount = field<unsigned int>(asset, world_anchor_count);
			world->portalDistanceAnchorsAndCloseDistSquared = field<vec4_t*>(asset, world_anchors);

			convert_draw(asset, world, allocator);
			std::memcpy(&world->lightGrid, &field<std::uint8_t>(asset, world_light_grid), sizeof(world->lightGrid));
			disable_empty_light_grid_trees(world->lightGrid);
			set_neutral_light_grid(world->lightGrid, allocator);

			world->modelCount = field<int>(asset, world_model_count);
			world->models = field<target::GfxBrushModel*>(asset, world_models);
			world->bounds = field<target::Bounds>(asset, world_bounds);
			world->shadowBounds = field<target::Bounds>(asset, world_shadow_bounds);
			world->checksum = field<unsigned int>(asset, world_checksum);
			convert_materials(asset, world, allocator);
			std::memcpy(world->outdoorLookupMatrix, &field<std::uint8_t>(asset, world_outdoor_lookup_matrix), sizeof(world->outdoorLookupMatrix));
			convert_images(asset, world, allocator);

			world->lightRegion = field<target::GfxLightRegion*>(asset, world_light_region);
			
			// H1 walks dynEntClientCount entries of the clip dyn ent list
			world->dpvsDyn = {};
			convert_dpvs(asset, world, allocator);
			allocate_runtime_data(world, allocator);

			world->mapVtxChecksum = field<unsigned int>(asset, world_map_vtx_checksum);
			if (const auto hero_light_count = field<unsigned int>(asset, world_hero_only_light_count))
			{
				ZONETOOL_WARNING("GfxWorld \"%s\" has %u hero only lights that are not converted", asset->name, hero_light_count);
			}

			world->fogTypesAllowed = target::FOG_NORMAL | target::FOG_DFOG;
			world->buildInfo = field<target::GfxBuildInfo>(asset, world_build_info);
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
		if (index < 0 || index >= static_cast<int>(draw_transient_zone_count))
		{
			ZONETOOL_WARNING("GfxMapTrZone \"%s\" has no valid transient zone index", zone->name);
			return;
		}

		loaded_transient_zones[index] = zone;
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
