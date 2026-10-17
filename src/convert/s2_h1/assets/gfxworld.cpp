#include <std_include.hpp>

#include "../include.hpp"
#include "gfxworld.hpp"
#include "comworld.hpp"
#include "gfximage.hpp"
#include "material.hpp"

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
					world->draw.vertexLayerDataSize += zones[i]->indexDataSize;
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

		std::uint16_t get_light_env(const GfxWorld* asset, const zonetool::h1::GfxWorld* world, const std::uint16_t list_offset)
		{
			const auto& light_lists = asset->lightLists;
			if (!light_lists.lists || list_offset >= light_lists.listsSize)
			{
				return 0;
			}

			std::vector<std::uint16_t> lights;
			const auto count = std::min<std::uint32_t>(light_lists.lists[list_offset], light_lists.listsSize - list_offset - 1);
			for (auto i = 0u; i < count; i++)
			{
				const auto light = light_lists.lists[list_offset + 1 + i];
				const auto is_extra_sun = light > 1 && light <= world->lastSunPrimaryLightIndex; // primary light 1 is the main sun
				if (light >= world->primaryLightCount || is_extra_sun)
				{
					continue;
				}

				lights.push_back(light);
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
			new_surface.laf.fields.flags = zonetool::h1::GFX_SURFACE_CASTS_SUN_SHADOW;

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
				dpvs.surfaces[new_index[i]] = convert_surface(world, surfaces[i], get_light_env(asset, world, surfaces[i].lightListOffset), allocator);
				new_bounds.bounds = std::bit_cast<zonetool::h1::Bounds>(bounds[i].bounds);
				std::memset(&new_bounds.unk, 0xFF, 11); // image stream distances, 0xFF is far
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

		std::uint16_t get_smodel_cull_dist(const GfxStaticModelInst& inst)
		{
			const auto& half_size = inst.bounds.halfSize;
			const auto half_extent = std::hypot(half_size[0], half_size[1], half_size[2]);
			return static_cast<std::uint16_t>(std::clamp(half_extent * 100.0f, 1500.0f, 30000.0f));
		}

		void convert_static_models(GfxWorld* asset, zonetool::h1::GfxWorld* world, utils::memory::allocator& allocator)
		{
			auto& dpvs = world->dpvs;
			const auto draw_insts = asset->dpvs.smodelDrawInsts;
			const auto insts = asset->dpvs.smodelInsts;

			dpvs.smodelInsts = retype<zonetool::h1::GfxStaticModelInst>(insts);
			dpvs.smodelDrawInsts = allocate_zeroed<zonetool::h1::GfxStaticModelDrawInst>(allocator, dpvs.smodelCount);
			dpvs.smodelLightingInsts = allocate_zeroed<zonetool::h1::GfxStaticModelLighting>(allocator, dpvs.smodelCount);

			for (auto i = 0u; i < dpvs.smodelCount; i++)
			{
				const auto& inst = draw_insts[i];
				auto& new_inst = dpvs.smodelDrawInsts[i];
				new_inst.placement = std::bit_cast<zonetool::h1::GfxPackedPlacement>(inst.placement);
				new_inst.model = make_stub<zonetool::h1::XModel>(inst.model ? inst.model->name : "void", allocator);
				new_inst.flags = zonetool::h1::STATIC_MODEL_FLAG_LIGHTGRID_LIGHTING | (inst.flags & zonetool::h1::STATIC_MODEL_FLAG_NO_CAST_SHADOW) | zonetool::h1::STATIC_MODEL_FLAG_SCALE_6;
				new_inst.cullDist = inst.cullDist ? inst.cullDist : get_smodel_cull_dist(insts[i]);
				new_inst.staticModelId = inst.staticModelId;
				new_inst.reflectionProbeIndex = inst.reflectionProbeIndex;
				new_inst.primaryLightEnvIndex = get_light_env(asset, world, inst.lightListOffset);
				new_inst.sunShadowFlags = (new_inst.flags & zonetool::h1::STATIC_MODEL_FLAG_NO_CAST_SHADOW) ? 0 : 1;
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

			dpvs.unknownSModelVisData1 = allocate_zeroed<unsigned int>(allocator, smodel_words);
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

				// R_LightgridLookup walks p_nodeTable when the grid point is inside the bounds, so an empty tree needs min > max
				for (auto axis = 0; axis < 3; axis++)
				{
					tree.coordMinGridSpace[axis] = 0x10000;
					tree.coordMaxGridSpace[axis] = 0;
				}
			}
		}

		void set_neutral_light_grid(zonetool::h1::GfxLightGrid& grid, utils::memory::allocator& allocator)
		{
			// H1 cannot read the S2 probe grid, stock H1 maps use these half float colors and one stage
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
			world->primaryLightEnvCount = asset->primaryLightEnvCount;
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
			disable_empty_light_grid_trees(world->lightGrid);
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

			world->dpvsDyn = {}; // zero counts keep H1 from walking the clip dyn ent list
			convert_dpvs(asset, world, allocator);
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
