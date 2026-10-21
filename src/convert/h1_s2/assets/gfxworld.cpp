#include <std_include.hpp>

#include "../include.hpp"
#include "gfxworld.hpp"
#include "comworld.hpp"
#include "gfximage.hpp"
#include "material.hpp"
#include "techset.hpp"

#include <formats/light_grid.hpp>
#include <formats/probe_volume.hpp>

#include <bit>

namespace zonetool::h1::converter::s2::gfxworld
{
	namespace
	{
		constexpr float light_grid_cell_size[3] = {32.0f, 32.0f, 64.0f};
		constexpr int light_grid_bias[3] = {4096, 4096, 2048};

		GfxWorld* pending_world = nullptr;

		template <typename To, typename From>
		To* retype(From* data)
		{
			static_assert(sizeof(To) == sizeof(From));
			return reinterpret_cast<To*>(data);
		}

		template <typename T>
		T* non_null_array(const std::size_t count, utils::memory::allocator& allocator)
		{
			return allocator.allocate_array<T>(std::max<std::size_t>(count, 1));
		}

		template <typename T>
		void reserve_runtime_buffer(T*& buffer, const unsigned int count, utils::memory::allocator& allocator)
		{
			if (count && !buffer)
			{
				buffer = allocator.allocate<T>();
			}
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

		Bounds smodel_bounds(const GfxStaticModelInst& inst)
		{
			Bounds bounds{};
			std::memcpy(bounds.midPoint, inst.mins, sizeof(bounds.midPoint));
			std::memcpy(bounds.halfSize, inst.maxs, sizeof(bounds.halfSize));
			return bounds;
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

		void convert_header(GfxWorld* asset, zonetool::s2::GfxWorld& world, utils::memory::allocator& allocator)
		{
			world.name = asset->name;
			world.baseName = asset->baseName;
			world.planeCount = asset->planeCount;
			world.nodeCount = asset->nodeCount;
			world.surfaceCount = asset->surfaceCount;
			world.portalGroupCount = asset->portalGroupCount;
			world.lastSunPrimaryLightIndex = 1;
			world.primaryLightCount = asset->primaryLightCount;
			world.primaryLightEnvCount = 1;
			world.sortKeyLitDecal = asset->primaryLightCount; // TODO unverified

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

			draw.reflectionProbeOrigins = retype<zonetool::s2::GfxReflectionProbe>(source.reflectionProbeOrigins);
			draw.reflectionProbeReferenceCount = source.reflectionProbeReferenceCount;
			draw.reflectionProbeReferenceOrigins = retype<zonetool::s2::GfxReflectionProbeReferenceOrigin>(source.reflectionProbeReferenceOrigins);
			draw.reflectionProbeReferences = retype<zonetool::s2::GfxReflectionProbeReference>(source.reflectionProbeReferences);

			const auto fog_spline = gfximage::dump_fog_spline("*fog_spline0");
			draw.fogSplineCount = 1; // fogSplineImages[0] 0x1402044C0
			draw.fogSplineImages = allocator.allocate_array<zonetool::s2::GfxImage*>(1);
			draw.fogSplineImages[0] = make_stub<zonetool::s2::GfxImage>(allocator.duplicate_string(fog_spline), allocator);
			draw.fogSplineInfo = allocator.allocate_array<char[116]>(1);

			const auto lightmap_count = static_cast<unsigned int>(std::max(source.lightmapCount, 0));
			draw.lightmapOverrideCount = lightmap_count;
			draw.lightmapOverrides = lightmap_count ? allocator.allocate_array<zonetool::s2::GfxLightmapArray>(lightmap_count) : nullptr;
			for (auto i = 0u; draw.lightmapOverrides && i < lightmap_count; i++)
			{
				draw.lightmapOverrides[i].primary = image_stub(source.lightmaps[i].primary, allocator);
				draw.lightmapOverrides[i].secondary = image_stub(source.lightmaps[i].secondary, allocator);
			}

			draw.lightmapOverridePrimary = nullptr;
			draw.lightmapOverrideSecondary = nullptr;
			draw.imageCount = 0;
			draw.images = nullptr;
			draw.lightmapParameters = std::bit_cast<zonetool::s2::GfxLightmapParameters>(source.lightmapParameters);

			std::ranges::fill(draw.transientZones, nullptr);
			draw.transientZones[0] = make_stub<zonetool::s2::GfxMapTrZone>(transient_zone, allocator);
			draw.transientZoneMaterials = nullptr;

			draw.indexCount = source.indexCount;
			draw.indices = source.indices;
		}

		void convert_primary_lights(GfxWorld* asset, zonetool::s2::GfxWorld& world, const light_lists& lists, utils::memory::allocator& allocator)
		{
			const auto com_world = comworld::source_world();
			const auto count = asset->primaryLightCount;
			world.frustumLights = count ? allocator.allocate_array<zonetool::s2::GfxFrustumLights>(count) : nullptr;
			for (auto i = 0u; world.frustumLights && i < count; i++)
			{
				const auto light = com_world && i < com_world->primaryLightCount ? &com_world->primaryLights[i] : nullptr;
				const auto is_local = light && light->type != GFX_LIGHT_TYPE_NONE && light->type != GFX_LIGHT_TYPE_DIR;
				for (auto axis = 0; axis < 3; axis++)
				{
					world.frustumLights[i].bounds.midPoint[axis] = is_local ? light->origin[axis] : 0.0f;
					world.frustumLights[i].bounds.halfSize[axis] = is_local ? light->radius : -std::numeric_limits<float>::max();
				}
			}

			world.lightLists.surfaceListOffsetCount = asset->surfaceCount;
			world.lightLists.surfaceListOffsets = allocator.allocate<unsigned int>();
			world.lightLists.smodelListOffsetCount = asset->dpvs.smodelCount;
			world.lightLists.smodelListOffsets = allocator.allocate<unsigned int>();

			const auto& data = lists.data();
			world.lightLists.listsSize = static_cast<unsigned int>(data.size());
			world.lightLists.lists = copy_array(data.data(), data.size(), allocator);
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

			world.shadowGeom = retype<zonetool::s2::GfxShadowGeometry>(asset->shadowGeom);
			world.shadowGeomOptimized = retype<zonetool::s2::GfxShadowGeometry>(asset->shadowGeomOptimized);
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

			const std::uint8_t unk_0x34[5]{0x3D, 0x00, 0x68, 0xD0, 0xD0}; // stock S2 surface value

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
				new_surface.flags = zonetool::s2::GFX_SURFACE_FLAG_DEFAULT;
				if (surface.laf.fields.flags & GFX_SURFACE_CASTS_SUN_SHADOW)
				{
					new_surface.flags |= zonetool::s2::GFX_SURFACE_FLAG_CASTS_SUN_SHADOW;
				}
				new_surface.lightListOffset = lists.offset(surface.laf.fields.primaryLightEnvIndex);

				auto& new_bounds = bounds[i];
				new_bounds.bounds = std::bit_cast<zonetool::s2::Bounds>(source.surfacesBounds[i].bounds);
				std::memcpy(new_bounds.midPoint, new_bounds.bounds.midPoint, sizeof(new_bounds.midPoint));
				new_bounds.unk_0x30 = 1.0f;
				std::memcpy(new_bounds.unk_0x34, unk_0x34, sizeof(unk_0x34));
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
				new_inst.cullDist = inst.cullDist;
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
				new_inst.lodScale = lod_dist_scales[inst.flags & STATIC_MODEL_FLAG_SCALE_MODIFIER_MASK];
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
			dpvs.sunShadowOptCount = source.sunShadowOptCount;
			dpvs.sunSurfVisDataCount = source.sunSurfVisDataCount;
			dpvs.unk_0x318 = non_null_array<unsigned int>(2ull * source.smodelVisDataCount, allocator);

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
			reserve_runtime_buffer(world.sceneDynModel, model_count, allocator);
			reserve_runtime_buffer(world.sceneDynBrush, brush_count, allocator);
			reserve_runtime_buffer(world.primaryLightDynEntShadowVis[0], model_count, allocator);
			reserve_runtime_buffer(world.primaryLightDynEntShadowVis[1], brush_count, allocator);
			reserve_runtime_buffer(world.nonSunPrimaryLightForModelDynEnt, model_count, allocator);
		}

		void convert_dynamic_data(GfxWorld* asset, zonetool::s2::GfxWorld& world, utils::memory::allocator& allocator)
		{
			convert_dpvs_dynamic(asset, world, allocator);
			convert_dyn_ent_scene_buffers(asset, world, allocator);
			world.mapVtxChecksum = asset->mapVtxChecksum;
			world.heroOnlyLightCount = 0;
			world.heroOnlyLights = nullptr;
			world.mdaoVolumesCount = 0;
			world.mdaoVolumes = nullptr;
			world.buildInfo = std::bit_cast<zonetool::s2::GfxBuildInfo>(asset->buildInfo);
		}

		int light_grid_cell(const float position, const int axis)
		{
			return static_cast<int>(std::floor(position / light_grid_cell_size[axis])) + light_grid_bias[axis];
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

			return sampler.sample(asset->dpvs.smodelInsts[index].lightingOrigin); // the voxel at the lighting origin is often inside the ground
		}

		void convert_static_model_probes(GfxWorld* asset, zonetool::s2::GfxWorld& world, light_grid_sampler& sampler, utils::memory::allocator& allocator)
		{
			auto& data = world.probeData;
			const auto count = asset->dpvs.smodelCount;
			if (!count || !sampler.valid())
			{
				ZONETOOL_WARNING("GfxWorld: the source map has no light grid, static models keep the donor probes");
				return;
			}

			const auto grids = allocator.allocate_array<zonetool::s2::GfxStaticModelProbeGrid>(count);
			const auto probes = allocator.allocate_array<zonetool::s2::GfxSHProbeData>(count);
			std::vector<std::optional<std::array<float, light_grid::probe_sh_count + 1>>> samples(count);
			probe_average average;
			for (auto i = 0u; i < count; i++)
			{
				const auto& inst = asset->dpvs.smodelInsts[i];
				grids[i] = make_single_probe_grid(i, smodel_bounds(inst));

				samples[i] = static_model_lighting(asset, sampler, i);
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

			world.voxelTreeCount = 1;
			world.voxelTree = make_voxel_tree(*volume, allocator);

			ZONETOOL_INFO("GfxWorld: light probe volume with %u probes (%u sampled from the light grid), %u tetrahedrons, %u voxel leaves of %d units",
				probe_count, average.count(), tetrahedron_count, volume->leaf_count, 1 << volume->node_coord_bit_shift[2]);
		}

		zonetool::s2::GfxWorld* convert(GfxWorld* asset, const zonetool::s2::GfxWorld* donor, const char* transient_zone, utils::memory::allocator& allocator)
		{
			const auto world = allocator.allocate<zonetool::s2::GfxWorld>();
			*world = *donor;
			std::memset(&world->unknownData, 0, sizeof(world->unknownData));

			const light_lists lists(comworld::source_world());
			convert_header(asset, *world, allocator);
			convert_cells(asset, *world, allocator);
			convert_draw(asset, *world, transient_zone, allocator);
			convert_primary_lights(asset, *world, lists, allocator);
			convert_models(asset, *world, allocator);
			convert_dpvs(asset, *world, lists, allocator);
			convert_dynamic_data(asset, *world, allocator);

			light_grid_sampler sampler(asset->lightGrid, comworld::source_world());
			convert_static_model_probes(asset, *world, sampler, allocator);
			convert_probe_volume(asset, *world, sampler, allocator);
			return world;
		}

		void dump_donor_image(const zonetool::s2::GfxWorld& world)
		{
			const auto image = world.draw.iesLookup;
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
		dump_donor_image(*world);
		zonetool::s2::gfx_world::dump(world);
		clear_stubs();
		add_csv_row("gfx_map", pending_world->name);

		ZONETOOL_INFO("GfxWorld \"%s\": %u surfaces, %u static models, %u primary lights", pending_world->name,
			pending_world->surfaceCount, pending_world->dpvs.smodelCount, pending_world->primaryLightCount);
		pending_world = nullptr;
	}
}
