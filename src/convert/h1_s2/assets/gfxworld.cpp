#include <std_include.hpp>

#include "../include.hpp"
#include "gfxworld.hpp"
#include "comworld.hpp"
#include "gfximage.hpp"
#include "techset.hpp"

#include <formats/light_grid.hpp>
#include <formats/probe_volume.hpp>

namespace zonetool::h1::converter::s2::gfxworld
{
	namespace
	{
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

		struct s2_surface_bounds
		{
			Bounds bounds;
			float midPoint[3];
			char __pad0[12];
			float unk48;
			std::uint8_t unk52[5];
			char __pad1[7];
		};
		static_assert(sizeof(s2_surface_bounds) == 64);
		static_assert(offsetof(s2_surface_bounds, unk48) == 48);

		struct s2_static_model_draw_inst
		{
			GfxPackedPlacement placement;
			float unk34[3];
			char __pad0[8];
			XModel* model;
			char __pad1[2];
			std::uint16_t cullDist;
			std::uint16_t flags;
			char __pad2[2];
			std::uint16_t staticModelId;
			char __pad3[2];
			std::uint32_t unk5C;
			char __pad4[10];
			std::uint16_t lightListOffset;
			float radius;
			std::uint8_t reflectionProbeIndex;
			char __pad5[1];
			std::uint8_t unk72;
			char __pad6[1];
			float unk74;
			char __pad7[8];
		};
		static_assert(sizeof(s2_static_model_draw_inst) == 128);
		static_assert(offsetof(s2_static_model_draw_inst, unk34) == 0x34);
		static_assert(offsetof(s2_static_model_draw_inst, model) == 0x48);
		static_assert(offsetof(s2_static_model_draw_inst, cullDist) == 0x52);
		static_assert(offsetof(s2_static_model_draw_inst, flags) == 0x54);
		static_assert(offsetof(s2_static_model_draw_inst, staticModelId) == 0x58);
		static_assert(offsetof(s2_static_model_draw_inst, unk5C) == 0x5C);
		static_assert(offsetof(s2_static_model_draw_inst, lightListOffset) == 0x6A);
		static_assert(offsetof(s2_static_model_draw_inst, radius) == 0x6C);
		static_assert(offsetof(s2_static_model_draw_inst, reflectionProbeIndex) == 0x70);
		static_assert(offsetof(s2_static_model_draw_inst, unk72) == 0x72);
		static_assert(offsetof(s2_static_model_draw_inst, unk74) == 0x74);

		struct s2_primary_light_bounds
		{
			Bounds bounds;
			std::uint32_t count24;
			std::uint32_t count28;
			void* data32;
			void* data40;
			char __pad0[16];
		};
		static_assert(sizeof(s2_primary_light_bounds) == 64);

		struct s2_lightmap
		{
			void* primary;
			void* secondary;
		};

		constexpr std::size_t world_plane_count = 20;
		constexpr std::size_t world_node_count = 24;
		constexpr std::size_t world_surface_count = 28;
		constexpr std::size_t world_sky_count = 32;
		constexpr std::size_t world_skies = 40;
		constexpr std::size_t world_portal_group_count = 48;
		constexpr std::size_t world_last_sun_primary_light = 52;
		constexpr std::size_t world_primary_light_count = 56;
		constexpr std::size_t world_unk60 = 60;
		constexpr std::size_t world_unk64 = 64;
		constexpr std::size_t world_cell_count = 88;
		constexpr std::size_t world_planes = 96;
		constexpr std::size_t world_nodes = 104;
		constexpr std::size_t world_aabb_tree_counts = 128;
		constexpr std::size_t world_aabb_trees = 136;
		constexpr std::size_t world_cell_unk144 = 144;
		constexpr std::size_t world_cells = 152;
		constexpr std::size_t world_portal_groups = 160;
		constexpr std::size_t world_anchor_count = 168;
		constexpr std::size_t world_anchors = 176;
		constexpr std::size_t world_draw = 192;
		constexpr std::size_t world_vertex_data = 2320;
		constexpr std::size_t world_primary_light_bounds = 2816;
		constexpr std::size_t world_shadow_surface_count = 2840;
		constexpr std::size_t world_shadow_surfaces = 2848;
		constexpr std::size_t world_shadow_smodel_count = 2856;
		constexpr std::size_t world_shadow_smodels = 2864;
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
		constexpr std::size_t world_hero_only_lights = 4208;
		constexpr std::size_t world_mdao_volume_count = 4220;
		constexpr std::size_t world_mdao_volumes = 4224;
		constexpr std::size_t world_build_info = 4264;

		constexpr std::size_t draw_reflection_probe_count = 0;
		constexpr std::size_t draw_reflection_probes = 8;
		constexpr std::size_t draw_reflection_probe_origins = 16;
		constexpr std::size_t draw_reflection_probe_reference_count = 32;
		constexpr std::size_t draw_reflection_probe_reference_origins = 40;
		constexpr std::size_t draw_reflection_probe_references = 48;
		constexpr std::size_t draw_fog_spline_count = 56;
		constexpr std::size_t draw_fog_spline_images = 64;
		constexpr std::size_t draw_fog_spline_info = 72;
		constexpr std::size_t draw_ies_lookup = 80;
		constexpr std::size_t draw_lightmap_override_count = 156;
		constexpr std::size_t draw_lightmap_overrides = 160;
		constexpr std::size_t draw_image184 = 184;
		constexpr std::size_t draw_image192 = 192;
		constexpr std::size_t draw_image_count = 200;
		constexpr std::size_t draw_images = 208;
		constexpr std::size_t draw_lightmap_parameters = 216;
		constexpr std::size_t draw_transient_zones = 640;
		constexpr std::size_t draw_transient_zone_count = 16;
		constexpr std::size_t draw_material_lists = 768;
		constexpr std::size_t draw_index_count = 776;
		constexpr std::size_t draw_indices = 784;

		constexpr std::size_t dpvs_smodel_count = 0;
		constexpr std::size_t dpvs_static_surface_count = 4;
		constexpr std::size_t dpvs_surface_range_begins = 8;
		constexpr std::size_t dpvs_surface_range_ends = 40;
		constexpr std::size_t dpvs_surface_range_count = 8;
		constexpr std::size_t dpvs_smodel_vis_data_count = 72;
		constexpr std::size_t dpvs_surface_vis_data_count = 76;
		constexpr std::size_t dpvs_sorted_surf_index = 744;
		constexpr std::size_t dpvs_surfaces = 752;
		constexpr std::size_t dpvs_surfaces_bounds = 760;
		constexpr std::size_t dpvs_smodel_insts = 776;
		constexpr std::size_t dpvs_smodel_draw_insts = 784;
		constexpr std::size_t dpvs_smodel_vis_unk792 = 792;
		constexpr std::size_t dpvs_smodel_lighting = 800;
		constexpr std::size_t dpvs_smodel_unk816 = 816;
		constexpr std::size_t dpvs_sun_shadow_opt_count = 824;
		constexpr std::size_t dpvs_sun_surf_vis_data_count = 828;

		constexpr std::size_t s2_smodel_lighting_size = 24;
		constexpr std::size_t s2_cell_unk144_size = 24;

		constexpr std::uint8_t surface_flag_default = 0x01;
		constexpr std::uint8_t surface_flag_casts_sun_shadow = 0x10;
		constexpr std::uint16_t smodel_flags_default = 0x240;
		constexpr std::uint8_t surface_bounds_unk52[5]{0x3D, 0x00, 0x68, 0xD0, 0xD0};

		GfxWorld* pending_world = nullptr;

		template <typename T>
		T* copy_array(const T* data, const std::size_t count, utils::memory::allocator& allocator)
		{
			if (!data || !count)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<T>(count);
			std::memcpy(result, data, sizeof(T) * count);
			return result;
		}

		Bounds smodel_bounds(const GfxStaticModelInst& inst)
		{
			Bounds bounds{};
			std::memcpy(bounds.midPoint, inst.mins, sizeof(bounds.midPoint)); // the H1 struct names center and half size mins and maxs
			std::memcpy(bounds.halfSize, inst.maxs, sizeof(bounds.halfSize));
			return bounds;
		}

		std::uint8_t* zero_bytes(const std::size_t size, utils::memory::allocator& allocator)
		{
			return allocator.allocate_array<std::uint8_t>(std::max<std::size_t>(size, 1));
		}

		void* image_stub(const GfxImage* image, utils::memory::allocator& allocator)
		{
			if (!image || !image->name)
			{
				return nullptr;
			}

			if (!gfximage::is_dumped(image->name))
			{
				ZONETOOL_WARNING("GfxWorld image \"%s\" is not converted", image->name);
			}

			return name_stub(image->name, allocator);
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
					const auto& env = com_world->primaryLightEnvs[i];
					std::vector<std::uint16_t> lights;
					for (auto j = 0u; j < std::min<std::uint32_t>(env.numIndices, 4); j++)
					{
						if (env.primaryLightIndices[j] < com_world->primaryLightCount)
						{
							lights.push_back(env.primaryLightIndices[j]);
						}
					}

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
			data.data0 = allocator.allocate_array<unsigned int>(count);
			data.data1 = allocator.allocate_array<unsigned int>(count);
			data.data2 = allocator.allocate_array<std::uint64_t>(count);
			data.data3 = allocator.allocate_array<std::uint64_t>(count);
			data.data4 = allocator.allocate_array<unsigned int>(count);
			data.data5 = allocator.allocate_array<unsigned int>(count);
			for (auto i = 0u; i < count; i++)
			{
				const auto& vertex = asset->draw.vd.vertices[i];
				std::memcpy(data.positions[i], vertex.xyz, sizeof(vertex.xyz));
				std::memcpy(&data.data0[i], &vertex.binormalSign, sizeof(vertex.binormalSign));
				data.data1[i] = vertex.color.packed;
				std::memcpy(&data.data2[i], vertex.texCoord, sizeof(vertex.texCoord));
				std::memcpy(&data.data3[i], vertex.lmapCoord, sizeof(vertex.lmapCoord));
				data.data4[i] = vertex.normal.packed;
				data.data5[i] = vertex.tangent.packed;
			}

			zone->indexDataSize = asset->draw.vertexLayerDataSize;
			zone->indices.data = asset->draw.vld.data;
			return zone;
		}

		void convert_header(GfxWorld* asset, void* world, utils::memory::allocator& allocator)
		{
			field<const char*>(world, 0) = asset->name;
			field<const char*>(world, 8) = asset->baseName;
			field<int>(world, world_plane_count) = asset->planeCount;
			field<int>(world, world_node_count) = asset->nodeCount;
			field<unsigned int>(world, world_surface_count) = asset->surfaceCount;
			field<unsigned int>(world, world_portal_group_count) = asset->portalGroupCount;
			field<unsigned int>(world, world_last_sun_primary_light) = 1;
			field<unsigned int>(world, world_primary_light_count) = asset->primaryLightCount;
			field<unsigned int>(world, world_unk60) = 1;
			field<unsigned int>(world, world_unk64) = asset->primaryLightCount;

			field<int>(world, world_sky_count) = asset->skyCount;
			const auto skies = copy_array(asset->skies, asset->skyCount, allocator);
			for (auto i = 0; skies && i < asset->skyCount; i++)
			{
				skies[i].skyImage = static_cast<GfxImage*>(image_stub(asset->skies[i].skyImage, allocator));
			}
			field<GfxSky*>(world, world_skies) = skies;
		}

		void convert_cells(GfxWorld* asset, void* world, utils::memory::allocator& allocator)
		{
			const auto cell_count = static_cast<unsigned int>(asset->dpvsPlanes.cellCount);
			field<unsigned int>(world, world_cell_count) = cell_count;
			field<cplane_s*>(world, world_planes) = asset->dpvsPlanes.planes;

			const auto nodes = allocator.allocate_array<std::uint32_t>(std::max(asset->nodeCount, 1));
			for (auto i = 0; asset->dpvsPlanes.nodes && i < asset->nodeCount; i++)
			{
				nodes[i] = asset->dpvsPlanes.nodes[i];
			}
			field<std::uint32_t*>(world, world_nodes) = asset->nodeCount ? nodes : nullptr;

			field<GfxCellTreeCount*>(world, world_aabb_tree_counts) = asset->aabbTreeCounts;
			field<GfxCellTree*>(world, world_aabb_trees) = asset->aabbTrees;
			field<std::uint8_t*>(world, world_cell_unk144) = cell_count ? zero_bytes(s2_cell_unk144_size * cell_count, allocator) : nullptr;
			field<GfxCell*>(world, world_cells) = asset->cells;

			const auto groups = copy_array(asset->portalGroup, asset->portalGroupCount, allocator);
			for (auto i = 0u; groups && i < asset->portalGroupCount; i++)
			{
				groups[i].targetNameIndex = 0;
			}
			field<GfxPortalGroup*>(world, world_portal_groups) = groups;
			field<unsigned int>(world, world_anchor_count) = asset->portalDistanceAnchorCount;
			field<vec4_t*>(world, world_anchors) = asset->portalDistanceAnchorsAndCloseDistSquared;
		}

		void convert_draw(GfxWorld* asset, void* world, const char* transient_zone, utils::memory::allocator& allocator)
		{
			const auto draw = static_cast<std::uint8_t*>(world) + world_draw;
			const auto& source = asset->draw;

			field<unsigned int>(draw, draw_reflection_probe_count) = source.reflectionProbeCount;
			const auto probes = source.reflectionProbeCount ? allocator.allocate_array<void*>(source.reflectionProbeCount) : nullptr;
			for (auto i = 0u; probes && i < source.reflectionProbeCount; i++)
			{
				probes[i] = image_stub(source.reflectionProbes[i], allocator);
			}

			// probe 0 is the solid red default probe. Models outside every probe volume use it and reflect red,
			// so give it the cubemap of the first real probe
			if (probes && source.reflectionProbeCount > 1)
			{
				probes[0] = probes[1];
			}
			field<void**>(draw, draw_reflection_probes) = probes;
			field<GfxReflectionProbe*>(draw, draw_reflection_probe_origins) = source.reflectionProbeOrigins;
			field<unsigned int>(draw, draw_reflection_probe_reference_count) = source.reflectionProbeReferenceCount;
			field<GfxReflectionProbeReferenceOrigin*>(draw, draw_reflection_probe_reference_origins) = source.reflectionProbeReferenceOrigins;
			field<GfxReflectionProbeReference*>(draw, draw_reflection_probe_references) = source.reflectionProbeReferences;

			field<unsigned int>(draw, draw_fog_spline_count) = 0;
			field<void*>(draw, draw_fog_spline_images) = nullptr;
			field<void*>(draw, draw_fog_spline_info) = nullptr;

			const auto lightmap_count = static_cast<unsigned int>(std::max(source.lightmapCount, 0));
			const auto lightmaps = lightmap_count ? allocator.allocate_array<s2_lightmap>(lightmap_count) : nullptr;
			for (auto i = 0u; lightmaps && i < lightmap_count; i++)
			{
				lightmaps[i].primary = image_stub(source.lightmaps[i].primary, allocator);
				lightmaps[i].secondary = image_stub(source.lightmaps[i].secondary, allocator);
			}
			field<unsigned int>(draw, draw_lightmap_override_count) = lightmap_count;
			field<s2_lightmap*>(draw, draw_lightmap_overrides) = lightmaps;

			field<void*>(draw, draw_image184) = nullptr;
			field<void*>(draw, draw_image192) = nullptr;
			field<unsigned int>(draw, draw_image_count) = 0;
			field<void*>(draw, draw_images) = nullptr;
			field<GfxLightmapParameters>(draw, draw_lightmap_parameters) = source.lightmapParameters;

			const auto zones = &field<void*>(draw, draw_transient_zones);
			std::fill_n(zones, draw_transient_zone_count, nullptr);
			zones[0] = name_stub(transient_zone, allocator);
			field<void*>(draw, draw_material_lists) = nullptr;

			field<unsigned int>(draw, draw_index_count) = source.indexCount;
			field<unsigned short*>(draw, draw_indices) = source.indices;
		}

		void convert_primary_lights(GfxWorld* asset, void* world, const light_lists& lists, utils::memory::allocator& allocator)
		{
			const auto com_world = comworld::source_world();
			const auto count = asset->primaryLightCount;
			const auto bounds = count ? allocator.allocate_array<s2_primary_light_bounds>(count) : nullptr;
			for (auto i = 0u; bounds && i < count; i++)
			{
				const auto light = com_world && i < com_world->primaryLightCount ? &com_world->primaryLights[i] : nullptr;
				const auto is_local = light && light->type != GFX_LIGHT_TYPE_NONE && light->type != GFX_LIGHT_TYPE_DIR;
				for (auto axis = 0; axis < 3; axis++)
				{
					bounds[i].bounds.midPoint[axis] = is_local ? light->origin[axis] : 0.0f;
					bounds[i].bounds.halfSize[axis] = is_local ? light->radius : -std::numeric_limits<float>::max();
				}
			}
			field<s2_primary_light_bounds*>(world, world_primary_light_bounds) = bounds;

			field<unsigned int>(world, world_shadow_surface_count) = asset->surfaceCount;
			field<std::uint8_t*>(world, world_shadow_surfaces) = zero_bytes(0, allocator);
			field<unsigned int>(world, world_shadow_smodel_count) = asset->dpvs.smodelCount;
			field<std::uint8_t*>(world, world_shadow_smodels) = zero_bytes(0, allocator);

			const auto& data = lists.data();
			field<unsigned int>(world, world_light_list_size) = static_cast<unsigned int>(data.size());
			field<std::uint16_t*>(world, world_light_lists) = copy_array(data.data(), data.size(), allocator);
		}

		void convert_models(GfxWorld* asset, void* world, utils::memory::allocator& allocator)
		{
			field<int>(world, world_model_count) = asset->modelCount;
			field<GfxBrushModel*>(world, world_models) = asset->models;
			field<Bounds>(world, world_bounds) = asset->bounds;
			field<Bounds>(world, world_shadow_bounds) = asset->shadowBounds;
			field<unsigned int>(world, world_checksum) = asset->checksum;
			field<int>(world, world_material_memory_count) = asset->materialMemoryCount;
			field<MaterialMemory*>(world, world_material_memory) = asset->materialMemory;
			// S2 needs a valid sun direction, keep the donor sun when the source map has none
			if (asset->sun.hasValidData)
			{
				field<sunflare_t>(world, world_sun) = asset->sun;
			}
			std::memcpy(&field<std::uint8_t>(world, world_outdoor_lookup_matrix), asset->outdoorLookupMatrix, sizeof(asset->outdoorLookupMatrix));
			field<void*>(world, world_outdoor_image) = image_stub(asset->outdoorImage, allocator);

			field<GfxShadowGeometry*>(world, world_shadow_geom) = asset->shadowGeom;
			field<GfxShadowGeometry*>(world, world_shadow_geom_optimized) = asset->shadowGeomOptimized;
			field<GfxLightRegion*>(world, world_light_region) = asset->lightRegion;
		}

		void set_surface_ranges(const GfxWorldDpvsStatic& source, std::uint8_t* dpvs)
		{
			const auto opaque_end = source.unkSurfsEnd > source.unkSurfsBegin ? source.unkSurfsEnd : source.litOpaqueSurfsEnd;
			const std::array<std::pair<unsigned int, unsigned int>, dpvs_surface_range_count> ranges
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
				field<unsigned int>(dpvs, dpvs_surface_range_begins + i * 4) = ranges[i].first;
				field<unsigned int>(dpvs, dpvs_surface_range_ends + i * 4) = ranges[i].second;
			}
		}

		void convert_surfaces(GfxWorld* asset, std::uint8_t* dpvs, const light_lists& lists, utils::memory::allocator& allocator)
		{
			const auto& source = asset->dpvs;
			const auto count = asset->surfaceCount;
			const auto surfaces = allocator.allocate_array<s2_surface>(std::max(count, 1u));
			const auto bounds = allocator.allocate_array<s2_surface_bounds>(std::max(count, 1u));
			for (auto i = 0u; i < count; i++)
			{
				const auto& surface = source.surfaces[i];
				auto& new_surface = surfaces[i];
				new_surface.vertexLayerData = surface.tris.vertexLayerData;
				new_surface.firstVertex = surface.tris.firstVertex;
				new_surface.vertexCount = surface.tris.vertexCount;
				new_surface.triCount = surface.tris.triCount;
				new_surface.baseIndex = surface.tris.baseIndex;
				new_surface.material = surface.material;
				new_surface.lightmapIndex = surface.laf.fields.lightmapIndex;
				new_surface.reflectionProbeIndex = surface.laf.fields.reflectionProbeIndex;
				new_surface.flags = surface_flag_default;
				if (surface.laf.fields.flags & GFX_SURFACE_CASTS_SUN_SHADOW)
				{
					new_surface.flags |= surface_flag_casts_sun_shadow;
				}
				new_surface.lightListOffset = lists.offset(surface.laf.fields.primaryLightEnvIndex);

				auto& new_bounds = bounds[i];
				new_bounds.bounds = source.surfacesBounds[i].bounds;
				std::memcpy(new_bounds.midPoint, new_bounds.bounds.midPoint, sizeof(new_bounds.midPoint));
				new_bounds.unk48 = 1.0f;
				std::memcpy(new_bounds.unk52, surface_bounds_unk52, sizeof(surface_bounds_unk52));
			}

			field<s2_surface*>(dpvs, dpvs_surfaces) = count ? surfaces : nullptr;
			field<s2_surface_bounds*>(dpvs, dpvs_surfaces_bounds) = count ? bounds : nullptr;
			field<unsigned int*>(dpvs, dpvs_sorted_surf_index) = source.sortedSurfIndex;
		}

		void convert_static_models(GfxWorld* asset, std::uint8_t* dpvs, const light_lists& lists, utils::memory::allocator& allocator)
		{
			const auto& source = asset->dpvs;
			const auto count = source.smodelCount;
			field<GfxStaticModelInst*>(dpvs, dpvs_smodel_insts) = source.smodelInsts;
			if (!count)
			{
				field<void*>(dpvs, dpvs_smodel_draw_insts) = nullptr;
				field<void*>(dpvs, dpvs_smodel_lighting) = nullptr;
				field<void*>(dpvs, dpvs_smodel_unk816) = nullptr;
				return;
			}

			const auto insts = allocator.allocate_array<s2_static_model_draw_inst>(count);
			for (auto i = 0u; i < count; i++)
			{
				const auto& inst = source.smodelDrawInsts[i];
				auto& new_inst = insts[i];
				new_inst.placement = inst.placement;
				std::fill_n(new_inst.unk34, 3, inst.placement.scale);
				new_inst.model = inst.model;
				new_inst.flags = smodel_flags_default | (inst.flags & STATIC_MODEL_FLAG_NO_CAST_SHADOW);
				new_inst.staticModelId = inst.staticModelId;
				new_inst.lightListOffset = lists.offset(inst.primaryLightEnvIndex);
				const auto& half_size = smodel_bounds(source.smodelInsts[i]).halfSize;
				new_inst.radius = std::hypot(half_size[0], half_size[1], half_size[2]);
				new_inst.reflectionProbeIndex = inst.reflectionProbeIndex;
				new_inst.unk72 = 1;
				new_inst.unk74 = 1.0f;
			}

			field<s2_static_model_draw_inst*>(dpvs, dpvs_smodel_draw_insts) = insts;
			// light grid models need a unique lighting slot each, stock maps never share one
			const auto lighting = zero_bytes(s2_smodel_lighting_size * count, allocator);
			for (auto i = 0u; i < count; i++)
			{
				field<std::uint32_t>(lighting, s2_smodel_lighting_size * i) = i;
			}

			field<std::uint8_t*>(dpvs, dpvs_smodel_lighting) = lighting;
			field<std::uint8_t*>(dpvs, dpvs_smodel_unk816) = zero_bytes(sizeof(std::uint16_t) * count, allocator);
		}

		void convert_dpvs(GfxWorld* asset, void* world, const light_lists& lists, utils::memory::allocator& allocator)
		{
			const auto dpvs = static_cast<std::uint8_t*>(world) + world_dpvs;
			const auto& source = asset->dpvs;
			field<unsigned int>(dpvs, dpvs_smodel_count) = source.smodelCount;
			field<unsigned int>(dpvs, dpvs_static_surface_count) = source.staticSurfaceCount;
			set_surface_ranges(source, dpvs);
			field<unsigned int>(dpvs, dpvs_smodel_vis_data_count) = source.smodelVisDataCount;
			field<unsigned int>(dpvs, dpvs_surface_vis_data_count) = source.surfaceVisDataCount;
			field<unsigned int>(dpvs, dpvs_sun_shadow_opt_count) = source.sunShadowOptCount;
			field<unsigned int>(dpvs, dpvs_sun_surf_vis_data_count) = source.sunSurfVisDataCount;
			field<std::uint8_t*>(dpvs, dpvs_smodel_vis_unk792) = zero_bytes(8ull * source.smodelVisDataCount, allocator);

			convert_surfaces(asset, dpvs, lists, allocator);
			convert_static_models(asset, dpvs, lists, allocator);
		}

		// same layout as S2, the runtime buffers are only reserved when the pointers are set
		void convert_dpvs_dynamic(GfxWorld* asset, void* world, utils::memory::allocator& allocator)
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

			field<GfxWorldDpvsDynamic>(world, world_dpvs_dynamic) = dpvs;
		}

		template <typename T>
		void reserve_runtime_buffer(T*& buffer, const unsigned int count, utils::memory::allocator& allocator)
		{
			if (count && !buffer)
			{
				buffer = allocator.allocate<T>();
			}
		}

		// the donor map can have no dynents of a list, so its pointers are null and the game reserves no buffer (crash at 0x1408B4EC9)
		void convert_dyn_ent_scene_buffers(GfxWorld* asset, void* world, utils::memory::allocator& allocator)
		{
			auto& target = *static_cast<zonetool::s2::GfxWorld*>(world);
			const auto model_count = asset->dpvsDyn.dynEntClientCount[0];
			const auto brush_count = asset->dpvsDyn.dynEntClientCount[1];
			reserve_runtime_buffer(target.sceneDynModel, model_count, allocator);
			reserve_runtime_buffer(target.sceneDynBrush, brush_count, allocator);
			reserve_runtime_buffer(target.primaryLightDynEntShadowVis[0], model_count, allocator);
			reserve_runtime_buffer(target.primaryLightDynEntShadowVis[1], brush_count, allocator);
			reserve_runtime_buffer(target.nonSunPrimaryLightForModelDynEnt, model_count, allocator);
		}

		void convert_tail(GfxWorld* asset, void* world, utils::memory::allocator& allocator)
		{
			convert_dpvs_dynamic(asset, world, allocator);
			convert_dyn_ent_scene_buffers(asset, world, allocator);
			field<unsigned int>(world, world_map_vtx_checksum) = asset->mapVtxChecksum;
			field<unsigned int>(world, world_hero_only_light_count) = 0;
			field<void*>(world, world_hero_only_lights) = nullptr;
			field<unsigned int>(world, world_mdao_volume_count) = 0;
			field<void*>(world, world_mdao_volumes) = nullptr;
			field<GfxBuildInfo>(world, world_build_info) = asset->buildInfo;
		}

		constexpr auto probe_sun_visibility = light_grid::probe_sh_count;
		using probe_values = std::array<float, light_grid::probe_sh_count + 1>;

		constexpr float light_grid_cell_size[3] = {32.0f, 32.0f, 64.0f};
		constexpr int light_grid_bias[3] = {4096, 4096, 2048};

		constexpr std::uint16_t voxel_light_list[] = {16384, 1, 0}; // stock preamble, the empty light list is at address 2
		constexpr std::uint16_t empty_voxel_light_list = 2;

		int light_grid_cell(const float position, const int axis)
		{
			return static_cast<int>(std::floor(position / light_grid_cell_size[axis])) + light_grid_bias[axis];
		}

		zonetool::s2::GfxSHProbeData pack_probe(const probe_values& values)
		{
			zonetool::s2::GfxSHProbeData probe{};
			for (auto i = 0u; i < values.size(); i++)
			{
				probe.coeffs[i] = half_float::float_to_half(values[i]);
			}
			return probe;
		}

		class probe_average
		{
		public:
			void add(const probe_values& values)
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

			std::optional<probe_values> value() const
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
				result[probe_sun_visibility] = 1.0f;
				return result;
			}

		private:
			probe_values sum_{};
			unsigned int count_ = 0;
		};

		class light_grid_sampler
		{
		public:
			light_grid_sampler(const GfxLightGrid& grid, const ComWorld* com_world)
				: grid_(grid)
				, tree_(light_grid::make_tree_view(grid.tree[0], h1_leaf_size_bit_count))
				, config_{grid.rangeExponent8BitsEncoding, grid.rangeExponent12BitsEncoding, grid.rangeExponent16BitsEncoding}
			{
				if (!com_world)
				{
					return;
				}

				this->sun_envs_.resize(com_world->primaryLightEnvCount);
				for (auto i = 0u; i < com_world->primaryLightEnvCount; i++)
				{
					const auto& env = com_world->primaryLightEnvs[i];
					for (auto j = 0u; j < std::min<std::uint32_t>(env.numIndices, 4); j++)
					{
						const auto light = env.primaryLightIndices[j];
						if (light < com_world->primaryLightCount && com_world->primaryLights[light].type == GFX_LIGHT_TYPE_DIR)
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

			const light_grid::tree_view& tree() const
			{
				return this->tree_;
			}

			std::optional<probe_values> sample(const float origin[3])
			{
				// trilinear over the 8 grid points around the origin, empty points do not count
				int base[3];
				float fraction[3];
				for (auto axis = 0; axis < 3; axis++)
				{
					const auto position = origin[axis] / light_grid_cell_size[axis];
					base[axis] = light_grid_cell(origin[axis], axis);
					fraction[axis] = position - std::floor(position);
				}

				probe_values result{};
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

		private:
			static constexpr auto h1_leaf_size_bit_count = 7;
			static constexpr auto search_radius_xy = 4;
			static constexpr auto search_radius_z = 2;

			const GfxLightGrid& grid_;
			light_grid::tree_view tree_;
			light_grid::palette_config config_;
			std::vector<bool> sun_envs_;
			std::unordered_map<unsigned int, probe_values> palette_cache_;

			std::optional<probe_values> nearest(const int base[3])
			{
				std::optional<probe_values> best;
				auto best_distance = std::numeric_limits<int>::max();
				for (auto dz = -search_radius_z; dz <= search_radius_z; dz++)
				{
					for (auto dy = -search_radius_xy; dy <= search_radius_xy; dy++)
					{
						for (auto dx = -search_radius_xy; dx <= search_radius_xy; dx++)
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

			std::optional<probe_values> cell_values(const int cell[3])
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
				values[probe_sun_visibility] = is_sun ? 1.0f : 0.0f;
				return values;
			}

			probe_values palette_values(const unsigned int color_index)
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

				probe_values values{};
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

		void convert_static_model_probes(GfxWorld* asset, zonetool::s2::GfxWorld* world, light_grid_sampler& sampler, utils::memory::allocator& allocator)
		{
			auto& data = world->probeData;
			const auto count = asset->dpvs.smodelCount;
			if (!count || !sampler.valid())
			{
				ZONETOOL_WARNING("GfxWorld: the source map has no light grid, static models keep the donor probes");
				return;
			}

			const auto grids = allocator.allocate_array<zonetool::s2::GfxStaticModelProbeGrid>(count);
			const auto probes = allocator.allocate_array<zonetool::s2::GfxSHProbeData>(count);
			std::vector<std::optional<probe_values>> samples(count);
			probe_average average;
			for (auto i = 0u; i < count; i++)
			{
				const auto& inst = asset->dpvs.smodelInsts[i];
				grids[i] = make_single_probe_grid(i, smodel_bounds(inst));

				samples[i] = sampler.sample(inst.lightingOrigin);
				if (samples[i])
				{
					average.add(*samples[i]);
				}
			}

			const auto fallback = average.value();
			if (!fallback)
			{
				ZONETOOL_WARNING("GfxWorld: no static model is inside the source light grid, static models keep the donor probes");
				return;
			}

			for (auto i = 0u; i < count; i++)
			{
				probes[i] = pack_probe(samples[i] ? *samples[i] : *fallback);
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

			const auto leaves = allocator.allocate_array<zonetool::s2::GfxVoxelLeafNode>(volume.leaf_count);
			for (auto i = 1u; i < volume.leaf_count; i++)
			{
				leaves[i].lightListAddress = empty_voxel_light_list;
			}

			tree->voxelTopDownViewNodeCount = top_down_count;
			tree->voxelInternalNodeCount = internal_count;
			tree->voxelLeafNodeCount = volume.leaf_count;
			tree->lightListArraySize = static_cast<unsigned int>(std::size(voxel_light_list));
			tree->voxelTreeHeader = header;
			tree->voxelTopDownViewNodeArray = top_down;
			tree->voxelInternalNodeArray = internal;
			tree->voxelLeafNodeArray = leaves;
			tree->lightListArray = copy_array(voxel_light_list, std::size(voxel_light_list), allocator);
			tree->voxelInternalNodeDynamicLightList = allocator.allocate_array<unsigned int[2]>(internal_count);
			return tree;
		}

		void convert_probe_volume(GfxWorld* asset, zonetool::s2::GfxWorld* world, light_grid_sampler& sampler, utils::memory::allocator& allocator)
		{
			float bounds_min[3];
			float bounds_max[3];
			for (auto axis = 0; axis < 3; axis++)
			{
				bounds_min[axis] = asset->bounds.midPoint[axis] - asset->bounds.halfSize[axis];
				bounds_max[axis] = asset->bounds.midPoint[axis] + asset->bounds.halfSize[axis];
			}

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

			const auto volume = probe_volume::build(bounds_min, bounds_max, cell_occupied);
			if (!volume)
			{
				ZONETOOL_WARNING("GfxWorld: the light probe volume does not fit the map, models use the donor light probes");
				return;
			}

			auto& data = world->probeData;
			const auto probe_count = static_cast<unsigned int>(volume->probe_positions.size());
			const auto positions = allocator.allocate_array<zonetool::s2::GfxGpuLightGridProbePosition>(probe_count);
			std::vector<std::optional<probe_values>> samples(probe_count);
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

			const auto average_values = average.value();
			const auto fallback = average_values ? pack_probe(*average_values)
				: data.zoneCount && data.zones ? data.zones[0].fallbackProbeData : zonetool::s2::GfxSHProbeData{};
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
			data.tetrahedronCountVisible = 0;
			data.tetrahedrons = tetrahedrons;
			data.tetrahedronNeighbors = neighbors;
			data.tetrahedronVisibility = nullptr;
			data.voxelStartTetrahedronCount = start_count;
			data.voxelStartTetrahedron = starts;

			world->voxelTreeCount = 1;
			world->voxelTree = make_voxel_tree(*volume, allocator);

			ZONETOOL_INFO("GfxWorld: light probe volume with %u probes (%u sampled from the light grid), %u tetrahedrons, %u voxel leaves of %d units",
				probe_count, average.count(), tetrahedron_count, volume->leaf_count, 1 << volume->node_coord_bit_shift[2]);
		}

		zonetool::s2::GfxWorld* convert(GfxWorld* asset, const zonetool::s2::GfxWorld* donor, const char* transient_zone,
			utils::memory::allocator& allocator)
		{
			const auto world = allocator.allocate<zonetool::s2::GfxWorld>();
			*world = *donor;
			std::memset(&field<std::uint8_t>(world, world_vertex_data), 0, world_primary_light_bounds - world_vertex_data);

			const light_lists lists(comworld::source_world());
			convert_header(asset, world, allocator);
			convert_cells(asset, world, allocator);
			convert_draw(asset, world, transient_zone, allocator);
			convert_primary_lights(asset, world, lists, allocator);
			convert_models(asset, world, allocator);
			convert_dpvs(asset, world, lists, allocator);
			convert_tail(asset, world, allocator);

			light_grid_sampler sampler(asset->lightGrid, comworld::source_world());
			convert_static_model_probes(asset, world, sampler, allocator);
			convert_probe_volume(asset, world, sampler, allocator);
			return world;
		}

		void dump_donor_image(const zonetool::s2::GfxWorld* world)
		{
			const auto image = field<zonetool::s2::GfxImage*>(const_cast<zonetool::s2::GfxWorld*>(world), world_draw + draw_ies_lookup);
			if (!image || !image->name)
			{
				return;
			}

			const auto& zone = techset::donor_zone();
			if (!zone.is_global_image(image->name))
			{
				db_scope _(zone.db());
				zonetool::s2::gfx_image::dump(image);
			}
			add_csv_row("image", image->name, zone.is_global_image(image->name));
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

		const auto donor = static_cast<zonetool::s2::GfxWorld*>(techset::donor_zone().find_asset(zonetool::s2::ASSET_TYPE_GFXWORLD));
		if (!donor)
		{
			ZONETOOL_ERROR("The S2 donor zone has no GfxWorld");
			pending_world = nullptr;
			return;
		}

		utils::memory::allocator allocator;
		const auto transient_zone = allocator.duplicate_string(transient_zone_name(pending_world));
		zonetool::s2::gfx_map_trzone::dump(convert_transient_zone(pending_world, transient_zone, allocator));
		add_csv_row("gfx_map_trzone", transient_zone);

		const auto world = convert(pending_world, donor, transient_zone, allocator);
		dump_donor_image(world);
		zonetool::s2::gfx_world::dump(world);
		add_csv_row("gfx_map", pending_world->name);

		ZONETOOL_INFO("GfxWorld \"%s\": %u surfaces, %u static models, %u primary lights", pending_world->name,
			pending_world->surfaceCount, pending_world->dpvs.smodelCount, pending_world->primaryLightCount);
		pending_world = nullptr;
	}
}
